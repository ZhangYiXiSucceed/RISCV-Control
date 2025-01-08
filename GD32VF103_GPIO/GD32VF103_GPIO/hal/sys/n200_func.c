// See LICENSE for license details.
#include "gd32vf103.h"

#include "riscv_encoding.h"
#include "n200_func.h"

    // Configure PMP to make all the address space accesable and executable
void pmp_open_all_space(){
    // Config entry0 addr to all 1s to make the range cover all space
    asm volatile ("li x6, 0xffffffff":::"x6");
    asm volatile ("csrw pmpaddr0, x6":::);
    // Config entry0 cfg to make it NAPOT address mode, and R/W/X okay
    asm volatile ("li x6, 0x7f":::"x6");
    asm volatile ("csrw pmpcfg0, x6":::);
}

void switch_m2u_mode(){
    clear_csr (mstatus,MSTATUS_MPP);
    //printf("\nIn the m2u function, the mstatus is 0x%x\n", read_csr(mstatus));
    //printf("\nIn the m2u function, the mepc is 0x%x\n", read_csr(mepc));
    asm volatile ("la x6, 1f    ":::"x6");
    asm volatile ("csrw mepc, x6":::);
    asm volatile ("mret":::);
    asm volatile ("1:":::);
} 

unsigned int mtime_lo(void)
{
  return *(volatile unsigned int *)(TIMER_CTRL_ADDR + TIMER_MTIME);
}


unsigned int mtime_hi(void)
{
  return *(volatile unsigned int *)(TIMER_CTRL_ADDR + TIMER_MTIME + 4);
}

unsigned long long get_timer_value()
{
  while (1) {
    unsigned int hi = mtime_hi();
    unsigned int lo = mtime_lo();
    if (hi == mtime_hi())
      return ((unsigned long long)hi << 32) | lo;
  }
}

unsigned int get_timer_freq()
{
  return TIMER_FREQ;
}

unsigned long long get_instret_value()
{
  while (1) {
    unsigned int hi = read_csr(minstreth);
    unsigned int lo = read_csr(minstret);
    if (hi == read_csr(minstreth))
      return ((unsigned long long)hi << 32) | lo;
  }
}

unsigned long long get_cycle_value()
{
  while (1) {
    unsigned int hi = read_csr(mcycleh);
    unsigned int lo = read_csr(mcycle);
    if (hi == read_csr(mcycleh))
      return ((unsigned long long)hi << 32) | lo;
  }
}

unsigned int __attribute__((noinline)) measure_cpu_freq(unsigned int n)
{
  unsigned int start_mtime, delta_mtime;
  unsigned int mtime_freq = get_timer_freq();

  // Don't start measuruing until we see an mtime tick
  unsigned int tmp = mtime_lo();
  do {
    start_mtime = mtime_lo();
  } while (start_mtime == tmp);

  unsigned int start_mcycle = read_csr(mcycle);

  do {
    delta_mtime = mtime_lo() - start_mtime;
  } while (delta_mtime < n);

  unsigned int delta_mcycle = read_csr(mcycle) - start_mcycle;

  return (delta_mcycle / delta_mtime) * mtime_freq
         + ((delta_mcycle % delta_mtime) * mtime_freq) / delta_mtime;
}

unsigned int get_cpu_freq()
{
  unsigned int cpu_freq;

  // warm up
  measure_cpu_freq(1);
  // measure for real
  cpu_freq = measure_cpu_freq(100);

  return cpu_freq;
}



// Note that there are no assertions or bounds checking on these
// parameter values.



  
void eclic_init ( unsigned int num_irq )
{

  typedef volatile unsigned int vuint32_t;

  //clear cfg register 
  *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_CFG_OFFSET)=0;

  //clear minthresh register 
  *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_MTH_OFFSET)=0;

  //clear all IP/IE/ATTR/CTRL bits for all interrupt sources
  vuint32_t * ptr;

  vuint32_t * base = (vuint32_t*)(ECLIC_ADDR_BASE + ECLIC_INT_IP_OFFSET);
  vuint32_t * upper = (vuint32_t*)(base + num_irq*4);

  for (ptr = base; ptr < upper; ptr=ptr+4){
    *ptr = 0;
  }
}



void eclic_enable_interrupt (unsigned int source) {
    *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_IE_OFFSET+source*4) = 1;
}

void eclic_disable_interrupt (unsigned int source){
    *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_IE_OFFSET+source*4) = 0;
}

void eclic_set_pending(unsigned int source){
    *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_IP_OFFSET+source*4) = 1;
}

void eclic_clear_pending(unsigned int source){
    *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_IP_OFFSET+source*4) = 0;
}

void eclic_set_intctrl (unsigned int source, unsigned char intctrl){
  *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_CTRL_OFFSET+source*4) = intctrl;
}

unsigned char eclic_get_intctrl  (unsigned int source){
  return *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_CTRL_OFFSET+source*4);
}

void eclic_set_intattr (unsigned int source, unsigned char intattr){
  *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_ATTR_OFFSET+source*4) = intattr;
}

unsigned char eclic_get_intattr  (unsigned int source){
  return *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_INT_ATTR_OFFSET+source*4);
}

void eclic_set_cliccfg (unsigned char cliccfg){
  *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_CFG_OFFSET) = cliccfg;
}

unsigned char eclic_get_cliccfg  (){
  return *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_CFG_OFFSET);
}

void eclic_set_mth (unsigned char mth){
  *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_MTH_OFFSET) = mth;
}

unsigned char eclic_get_mth  (){
  return *(volatile unsigned char*)(ECLIC_ADDR_BASE+ECLIC_MTH_OFFSET);
}

//sets nlbits 
void eclic_set_nlbits(unsigned char nlbits) {
  //shift nlbits to correct position
  unsigned char nlbits_shifted = nlbits << ECLIC_CFG_NLBITS_LSB;

  //read the current cliccfg 
  unsigned char old_cliccfg = eclic_get_cliccfg();
  unsigned char new_cliccfg = (old_cliccfg & (~ECLIC_CFG_NLBITS_MASK)) | (ECLIC_CFG_NLBITS_MASK & nlbits_shifted); 

  eclic_set_cliccfg(new_cliccfg);
}

//get nlbits 
unsigned char eclic_get_nlbits(void) {
  //extract nlbits
  unsigned char nlbits = eclic_get_cliccfg();
  nlbits = (nlbits & ECLIC_CFG_NLBITS_MASK) >> ECLIC_CFG_NLBITS_LSB;
  return nlbits;
}

//sets an interrupt level based encoding of nlbits and ECLICINTCTLBITS
void eclic_set_irq_lvl(unsigned int source, unsigned char lvl) {
  //extract nlbits
  unsigned char nlbits = eclic_get_nlbits();
  if (nlbits > ECLICINTCTLBITS) {
    nlbits = ECLICINTCTLBITS;
  }

  //shift lvl right to mask off unused bits
  lvl = lvl >> (8-nlbits);
  //shift lvl into correct bit position
  lvl = lvl << (8-nlbits);
 
  //write to clicintctrl
  unsigned char current_intctrl = eclic_get_intctrl(source);
  //shift intctrl left to mask off unused bits
  current_intctrl = current_intctrl << nlbits;
  //shift intctrl into correct bit position
  current_intctrl = current_intctrl >> nlbits;

  eclic_set_intctrl(source, (current_intctrl | lvl));
}

//gets an interrupt level based encoding of nlbits
unsigned char eclic_get_irq_lvl(unsigned int source) {
  //extract nlbits
  unsigned char nlbits = eclic_get_nlbits();
  if (nlbits > ECLICINTCTLBITS) {
    nlbits = ECLICINTCTLBITS;
  }

  unsigned char intctrl = eclic_get_intctrl(source);

  //shift intctrl
  intctrl = intctrl >> (8-nlbits);
  //shift intctrl
  unsigned char lvl = intctrl << (8-nlbits);

  return lvl;
}

void eclic_set_irq_lvl_abs(unsigned int source, unsigned char lvl_abs) {
  //extract nlbits
  unsigned char nlbits = eclic_get_nlbits();
  if (nlbits > ECLICINTCTLBITS) {
    nlbits = ECLICINTCTLBITS;
  }

  //shift lvl_abs into correct bit position
  unsigned char lvl = lvl_abs << (8-nlbits);
 
  //write to clicintctrl
  unsigned char current_intctrl = eclic_get_intctrl(source);
  //shift intctrl left to mask off unused bits
  current_intctrl = current_intctrl << nlbits;
  //shift intctrl into correct bit position
  current_intctrl = current_intctrl >> nlbits;

  eclic_set_intctrl(source, (current_intctrl | lvl));
}

unsigned char eclic_get_irq_lvl_abs(unsigned int source) {
  //extract nlbits
  unsigned char nlbits = eclic_get_nlbits();
  if (nlbits > ECLICINTCTLBITS) {
    nlbits = ECLICINTCTLBITS;
  }

  unsigned char intctrl = eclic_get_intctrl(source);

  //shift intctrl
  intctrl = intctrl >> (8-nlbits);
  //shift intctrl
  unsigned char lvl_abs = intctrl;

  return lvl_abs;
}

//sets an interrupt priority based encoding of nlbits and ECLICINTCTLBITS
unsigned char eclic_set_irq_priority(unsigned int source, unsigned char priority) {
	//extract nlbits
	unsigned char nlbits = eclic_get_nlbits();
	if (nlbits >= ECLICINTCTLBITS) {
		nlbits = ECLICINTCTLBITS;
		return 0;
	}

	//shift priority into correct bit position
	priority = priority << (8 - ECLICINTCTLBITS);

	//write to eclicintctrl
	unsigned char current_intctrl = eclic_get_intctrl(source);
	//shift intctrl right to mask off unused bits
	current_intctrl = current_intctrl >> (8-nlbits);
	//shift intctrl into correct bit position
	current_intctrl = current_intctrl << (8-nlbits);

	eclic_set_intctrl(source, (current_intctrl | priority));

	return priority;
}

//gets an interrupt priority based encoding of nlbits
unsigned char eclic_get_irq_priority(unsigned int source) {
	//extract nlbits
	unsigned char nlbits = eclic_get_nlbits();
	if (nlbits > ECLICINTCTLBITS) {
		nlbits = ECLICINTCTLBITS;
	}

	unsigned char intctrl = eclic_get_intctrl(source);

	//shift intctrl
	intctrl = intctrl << nlbits;
	//shift intctrl
	unsigned char priority = intctrl >> (nlbits+(8 - ECLICINTCTLBITS));

	return priority;
}

void eclic_mode_enable() {
  unsigned int mtvec_value = read_csr(mtvec);
  mtvec_value = mtvec_value & 0xFFFFFFC0;
  mtvec_value = mtvec_value | 0x00000003;
  write_csr(mtvec,mtvec_value);
}

//sets vector-mode or non-vector mode 
void eclic_set_vmode(unsigned int source) {
  //read the current attr 
  unsigned char old_intattr = eclic_get_intattr(source);
      // Keep other bits unchanged and only set the LSB bit
  unsigned char new_intattr = (old_intattr | 0x1); 

  eclic_set_intattr(source,new_intattr);
}

void eclic_set_nonvmode(unsigned int source) {
  //read the current attr 
  unsigned char old_intattr = eclic_get_intattr(source);
      // Keep other bits unchanged and only clear the LSB bit
  unsigned char new_intattr = (old_intattr & (~0x1));

  eclic_set_intattr(source,new_intattr);
}

//sets interrupt as level sensitive
//Bit 1, trig[0], is defined as "edge-triggered" (0: level-triggered, 1: edge-triggered); 
//Bit 2, trig[1], is defined as "negative-edge" (0: positive-edge, 1: negative-edge).

void eclic_set_level_trig(unsigned int source) {
  //read the current attr 
  unsigned char old_intattr = eclic_get_intattr(source);
      // Keep other bits unchanged and only clear the bit 1
  unsigned char new_intattr = (old_intattr & (~0x2));

  eclic_set_intattr(source,new_intattr);
}

void eclic_set_posedge_trig(unsigned int source) {
  //read the current attr 
  unsigned char old_intattr = eclic_get_intattr(source);
      // Keep other bits unchanged and only set the bit 1
  unsigned char new_intattr = (old_intattr | 0x2);
      // Keep other bits unchanged and only clear the bit 2
  new_intattr = (old_intattr & (~0x4));

  eclic_set_intattr(source,new_intattr);
}

void eclic_set_negedge_trig(unsigned int source) {
  //read the current attr 
  unsigned char old_intattr = eclic_get_intattr(source);
      // Keep other bits unchanged and only set the bit 1
  unsigned char new_intattr = (old_intattr | 0x2);
      // Keep other bits unchanged and only set the bit 2
  new_intattr = (old_intattr | 0x4);

  eclic_set_intattr(source,new_intattr);
}

//void wfe() {
//  core_wfe();
//}



