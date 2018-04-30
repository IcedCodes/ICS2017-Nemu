#include "cpu/exec.h"

make_EHelper(test) {
  rtl_and(&t1, &id_dest->val, &id_src->val);
  rtl_update_ZFSF(&t1,id_dest->width); 
  t1 = 0;
  rtl_set_OF(&t1);
  rtl_set_CF(&t1);

  print_asm_template2(test);
}

make_EHelper(and) {
  //扩展填充
  //printf("id_dest=%x id_src=%x id_src.width=%x dest_width=%x\n",id_dest->val,id_src->val,id_src->width,id_dest->width);
  //rtl_sext(&t0,&id_src->val,id_src->width);
  rtl_and(&t2, &id_dest->val ,&t0);
  operand_write(id_dest, &t2);
  //printf("t2: %x\n", t2);
  t0 = 0;
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);
  rtl_update_ZFSF(&t1, id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  rtl_xor(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_set_CF(&t0);
  rtl_set_OF(&t0);

  print_asm_template2(xor);
}

make_EHelper(or) {
  rtl_or(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->val);
  t1 = 0;
  rtl_set_CF(&t1);
  rtl_set_OF(&t1);

  print_asm_template2(or);
}

make_EHelper(sar) {
  rtl_sar(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);

  // unnecessary to update CF and OF in NEMU

  print_asm_template2(sar);
}

make_EHelper(shl) {
  rtl_shl(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  rtl_shr(&t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);
  rtl_update_ZFSF(&t1, id_dest->width);
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtl_mv(&t1, &id_dest->val);
  rtl_not(&t1);
  operand_write(id_dest, &t1);

  print_asm_template1(not);
}
