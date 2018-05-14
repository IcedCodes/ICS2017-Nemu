#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
   rtl_push(&cpu.eflags.value);
   rtl_push((uint32_t*)&cpu.cs);
   rtl_push(&ret_addr);
   uint32_t low, high;
   low = vaddr_read(cpu.idtr.base + NO*8, 4) & 0x0000ffff;
   high = vaddr_read(cpu.idtr.base + NO*8 + 4, 4) & 0xffff0000;
   decoding.is_jmp = 1;
   decoding.jmp_eip = low + high;
}

void dev_raise_intr() {
}
