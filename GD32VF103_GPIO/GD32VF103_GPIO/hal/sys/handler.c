//See LICENSE for license details.
#include "riscv_encoding.h"


__attribute__((weak)) unsigned int handle_nmi()
{
  //write(1, "nmi\n", 5);
  //_exit(1);
  return 0;
}


__attribute__((weak)) unsigned int handle_trap(unsigned int mcause, unsigned int sp)
{
  //if(mcause == 0xFFF) {
      handle_nmi();
 // }
  //write(1, "trap\n", 5);
  //printf("In trap handler, the mcause is %d\n", mcause);
  //printf("In trap handler, the mepc is 0x%x\n", read_csr(mepc));
  //printf("In trap handler, the mtval is 0x%x\n", read_csr(mbadaddr));
  //_exit(mcause);
  return 0;
}





