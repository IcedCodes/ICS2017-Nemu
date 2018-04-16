#include "cpu/exec.h"

make_EHelper(jmp) {
  // the target address is calculated at the decode stage
  decoding.is_jmp = 1;

  print_asm("jmp %x", decoding.jmp_eip);
}

make_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  decoding.is_jmp = t2;

  print_asm("j%s %x", get_cc_name(subcode), decoding.jmp_eip);
}

make_EHelper(jmp_rm) {
  decoding.jmp_eip = id_dest->val;
  decoding.is_jmp = 1;

  print_asm("jmp *%s", id_dest->str);
}

make_EHelper(call) {
  // the target address is calculated at the decode stage
  printf("decoding.jmp_eip: %d\n id_dest->val:%d\nid_src->val:%d\nid_src2->val: %d\n", decoding.jmp_eip,id_dest->val,id_src->val,id_src2->val);
  rtl_push(&cpu.eip+4);
  printf("decoding.jmp_eip: %d\n id_dest->val:%d\nid_src->val:%d\nid_src2->val: %d\n", decoding.jmp_eip,id_dest->val,id_src->val,id_src2->val);
  rtl_addi(&decoding.jmp_eip, &cpu.eip, id_dest->val);
  decoding.is_jmp=1;
  print_asm("call %x", decoding.jmp_eip);
}

make_EHelper(ret) {
  TODO();

  print_asm("ret");
}

make_EHelper(call_rm) {
  TODO();

  print_asm("call *%s", id_dest->str);
}
