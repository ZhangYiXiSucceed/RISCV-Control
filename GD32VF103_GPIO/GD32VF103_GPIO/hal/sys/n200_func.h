// See LICENSE file for licence details

#ifndef N200_FUNC_H
#define N200_FUNC_H


#include "n200_timer.h"
#include "n200_eclic.h"

#define	ECLIC_GROUP_LEVEL0_PRIO4	0
#define	ECLIC_GROUP_LEVEL1_PRIO3	1
#define	ECLIC_GROUP_LEVEL2_PRIO2	2
#define	ECLIC_GROUP_LEVEL3_PRIO1	3
#define	ECLIC_GROUP_LEVEL4_PRIO0	4

void pmp_open_all_space();

void switch_m2u_mode();

unsigned int get_mtime_freq();

unsigned int mtime_lo(void);

unsigned int mtime_hi(void);

unsigned long long get_mtime_value();

unsigned long long get_instret_value();

unsigned long long get_cycle_value();

unsigned int get_cpu_freq();

unsigned int __attribute__((noinline)) measure_cpu_freq(unsigned int n);


///////////////////////////////////////////////////////////////////
/////// ECLIC relevant functions
///////
void eclic_init ( unsigned int num_irq );
unsigned long long get_timer_value();
void eclic_enable_interrupt (unsigned int source);
void eclic_disable_interrupt (unsigned int source);

void eclic_set_pending(unsigned int source);
void eclic_clear_pending(unsigned int source);

void eclic_set_intctrl (unsigned int source, unsigned char intctrl);
unsigned char eclic_get_intctrl  (unsigned int source);

void eclic_set_intattr (unsigned int source, unsigned char intattr);
unsigned char eclic_get_intattr  (unsigned int source);

void eclic_set_cliccfg (unsigned char cliccfg);
unsigned char eclic_get_cliccfg ();

void eclic_set_mth (unsigned char mth);
unsigned char eclic_get_mth();

//sets nlbits 
void eclic_set_nlbits(unsigned char nlbits);


//get nlbits 
unsigned char eclic_get_nlbits();

void eclic_set_irq_lvl(unsigned int source, unsigned char lvl);
unsigned char eclic_get_irq_lvl(unsigned int source);

void eclic_set_irq_lvl_abs(unsigned int source, unsigned char lvl_abs);
unsigned char eclic_get_irq_lvl_abs(unsigned int source);

unsigned char eclic_set_irq_priority(unsigned int source, unsigned char priority);
unsigned char eclic_get_irq_priority(unsigned int source);

void eclic_mode_enable();

void eclic_set_vmode(unsigned int source);
void eclic_set_nonvmode(unsigned int source);

void eclic_set_level_trig(unsigned int source);
void eclic_set_posedge_trig(unsigned int source);
void eclic_set_negedge_trig(unsigned int source);


///** \brief  Wait For Interrupt
//
//    Wait For Interrupt is a hint instruction that suspends execution
//    until one of a number of events occurs.
// */
__attribute__( ( always_inline ) ) static inline void __WFI(void) {
	__asm volatile ("wfi");
}
//
//
/** \brief  Wait For Event

    Wait For Event is a hint instruction that permits the processor to enter
    a low-power state until one of a number of events occurs.
 */
__attribute__( ( always_inline ) ) static inline  void __WFE(void) {
	__asm volatile ("csrs 0x810, 0x1");
	__asm volatile ("wfi");
	__asm volatile ("csrc 0x810, 0x1");
}


#endif
