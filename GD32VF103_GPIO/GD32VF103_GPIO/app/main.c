#include "gd32vf103.h"
#include "gd32vf103_usart.h"
#include "stdarg.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"


#define LED_RED_ON()     gpio_bit_reset(GPIOC, GPIO_PIN_13)
#define LED_BLUE_ON()    gpio_bit_reset(GPIOA, GPIO_PIN_2)
#define LED_GREEN_ON()   gpio_bit_reset(GPIOA, GPIO_PIN_1)


#define LED_RED_OFF()     gpio_bit_set(GPIOC, GPIO_PIN_13)
#define LED_GREEN_OFF()   gpio_bit_set(GPIOA, GPIO_PIN_1)
#define LED_BLUE_OFF()    gpio_bit_set(GPIOA, GPIO_PIN_2)


#define FLASH_MEMORY_ADDR  0x1FFFF7E0
#define DEVICE_ID_ADDR     0x1FFFF7E8
/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
 void test_printf(const char* fmt, ...);

void Delay(unsigned int t)
{
    unsigned int volatile i,j;
    for(i=0;i<t;i++)
        for(j=0;j<11000;j++);
}

void GPIO_Init(void)
{
    // RCU_APB2EN |= 1<<2;   //使能GPIOA时钟
    // GPIO_CTL0(GPIOA) &= ~(0XF<<4);  //清除GPIOA0配置
    // GPIO_CTL0(GPIOA) |= (0x03<<4);  //配置GPIOA0为推挽输出
    // GPIO_OCTL(GPIOA) |= (1<<1);     //设置GPIOA0为高电平

    rcu_periph_clock_enable(RCU_GPIOA);   //打开GPIOA时钟
    rcu_periph_clock_enable(RCU_GPIOC);   //打开GPIOA时钟
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
    gpio_init(GPIOC, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
    gpio_bit_set(GPIOA, GPIO_PIN_1);
    gpio_bit_set(GPIOA, GPIO_PIN_2);
    gpio_bit_set(GPIOC, GPIO_PIN_13);
}

void UART0_Init(void)
{
    rcu_periph_clock_enable(RCU_USART0);  //打开USART0时钟
    rcu_periph_clock_enable(RCU_GPIOA);   //打开GPIOA时钟
    //TX  PA9
    //RX  PA10
    gpio_init(GPIOA,GPIO_MODE_AF_PP,GPIO_OSPEED_10MHZ,GPIO_PIN_9);  //设置GPIOA9为服用输出模式  
    gpio_init(GPIOA,GPIO_MODE_IN_FLOATING,GPIO_OSPEED_10MHZ,GPIO_PIN_10);  //设置GPIOA10为浮空输入模式

    usart_deinit(USART0);   //复位USART0
    rcu_periph_clock_enable(RCU_AF);  //使能复用时钟
    usart_baudrate_set(USART0,115200);  //设置波特率为115200
    usart_parity_config(USART0,USART_PM_NONE);  //设置校验位为无
    usart_word_length_set(USART0,USART_WL_8BIT);  //设置传输长度8Bit
    usart_stop_bit_set(USART0,USART_STB_1BIT);  //设置停止位1位
    usart_transmit_config(USART0,USART_TRANSMIT_ENABLE);  //设置传输使能
    usart_enable(USART0);    //开启UART0
}


void timer_int_init()
{
    uint32_t timer_periph = TIMER3;
    /* TIMER2 configuration: generate PWM signals with different duty cycles:
    TIMER2CLK = SystemCoreClock / 120 = 1MHz */
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER3);
    timer_deinit(timer_periph);

    timer_struct_para_init(&timer_initpara);
    /* TIMER2 configuration */
    timer_initpara.prescaler         = 10799;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 1000;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(timer_periph,&timer_initpara);

    timer_update_event_enable(timer_periph);
    timer_update_source_config(timer_periph,TIMER_UPDATE_SRC_GLOBAL);
    timer_interrupt_enable(timer_periph,TIMER_INT_UP);
    
    
    timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_UP);
    /* auto-reload preload enable */

    timer_enable(timer_periph);

    /* enable the global interrupt */
    eclic_global_interrupt_enable();
    eclic_priority_group_set(ECLIC_PRIGROUP_LEVEL3_PRIO1);
    /* enable and set key EXTI interrupt priority */
    //eclic_set_intattr(TIMER2_IRQn,7);
    eclic_set_nonvmode(TIMER3_IRQn);
    eclic_set_posedge_trig(TIMER3_IRQn);
    eclic_irq_enable(TIMER3_IRQn, 1, 0);
    test_printf("eclic_get_mth=%d\r\n",eclic_get_mth());
    
}
 

 void test_printf(const char* fmt, ...)
 {
    char buf[256]={0};
    va_list ap;
    va_start(ap,fmt);
    int len = vsnprintf(buf,(unsigned int)256,fmt,ap);
    if(len)
    {
        int i;
        for(i=0;i<len;i++)
        {
            usart_data_transmit(USART0,buf[i]);
        }   
    }
    va_end(ap);
 }

void led_blink(unsigned int counter)
{
    unsigned int blink_state_num = counter%8;
    switch(blink_state_num)
    {
        case 0:
        {
            LED_RED_ON();
            LED_GREEN_OFF();
            LED_BLUE_OFF();
        }break;
        case 1:
        {
            LED_RED_ON();
            LED_BLUE_ON();
            LED_GREEN_ON();
        }break;
        case 2:
        {
            LED_RED_ON();
            LED_GREEN_ON();
            LED_BLUE_OFF();
        }break;
        case 3:
        {
            LED_RED_OFF();
            LED_BLUE_OFF();
            LED_GREEN_ON();
        }break;
        case 4:
        {
            LED_RED_OFF();
            LED_GREEN_ON();
            LED_BLUE_ON();
        }break;
        case 5:
        {
            LED_RED_OFF();
            LED_GREEN_OFF();
            LED_BLUE_ON();
        }break;
        case 6:
        {
            LED_RED_ON();
            LED_BLUE_ON();
            LED_GREEN_OFF();
        }break;
        case 7:
        {
            LED_RED_OFF();
            LED_GREEN_OFF();
            LED_BLUE_OFF();
        }break;
        default:
        break;

    }
}


int test_counter_g = 0;
 //__attribute__((interrupt)) void TIMER3_IRQHandler()
   void TIMER3_IRQHandler()
{
    if(timer_interrupt_flag_get(TIMER3,TIMER_INT_FLAG_UP))
    {
        test_counter_g++;
    }  
    timer_interrupt_flag_clear(TIMER3,TIMER_INT_FLAG_UP); 
}


int main(void)
{
    GPIO_Init();
    UART0_Init();
    timer_int_init();
    unsigned int mtvec_value = read_csr(mtvec);
    test_printf("mtvec_value=%x\r\n",mtvec_value);

    unsigned int flash_memory_data = *((unsigned int*)FLASH_MEMORY_ADDR);
    unsigned int flash_size = flash_memory_data & 0xFFFF;
    unsigned int sram_size  = flash_memory_data>>16 & 0xFFFF;
    test_printf("flash_size=%dKB sram_size=%dKB \r\n",flash_size, sram_size);

    unsigned int i;
    unsigned int device_id_buf[3];
    test_printf("device id:     ");
    device_id_buf[0] = *((unsigned int*)DEVICE_ID_ADDR);
    device_id_buf[1] = *((unsigned int*)DEVICE_ID_ADDR +  4);
    device_id_buf[2] = *((unsigned int*)DEVICE_ID_ADDR -  20);
    for(i=0;i<3;i++)
    {
        
        test_printf("%x\t",device_id_buf[i]);
    }
    
    while (1)
    {
        //  GPIO_OCTL(GPIOA) |= (1<<1);
        //  Delay(1000);
        //  GPIO_OCTL(GPIOA) &= ~(1<<1);
        //  Delay(1000);
         led_blink(test_counter_g);
         test_printf("test_counter_g=%d timer_counter=%d\r\n",test_counter_g,timer_counter_read(TIMER3));
        //  if(timer_flag_get(TIMER3,TIMER_FLAG_UP))
        //  {
        //     test_counter_g++;
        //     timer_flag_clear(TIMER3,TIMER_FLAG_UP);
            
        //  }
    }
}

