#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
	rtl_push(&id_dest->val);
	print_asm_template1(push);
}

make_EHelper(pop) {
  rtl_pop(&t0);
  operand_write(id_dest, &t0);

  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  reg_l(R_ESP) = reg_l(R_EBP);
  rtl_lm(&t0, &reg_l(R_ESP), id_src->width);
  id_dest->type = OP_TYPE_REG;
  id_dest->reg = R_EBP;
  operand_write(id_dest, &t0);
  if (id_dest->width == 2) 
  {
		  reg_w(R_ESP) += 2;
  }
  else 
  {
		  reg_l(R_ESP) += 4;
  }

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    rtl_sext(&t1, &cpu.eax, 2);
	rtl_shri(&cpu.edx, &t1, 16);
  }
  else {
    rtl_sari(&cpu.edx, &cpu.eax, 31);
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    rtl_lr(&t0, R_AL, 1);
	t0 = (int16_t)(int8_t)(uint8_t)t0;
	rtl_sr(R_AX, 2, &t0);
  }
  else {
    rtl_lr(&t0, R_AX, 2);
	t0 = (int32_t)(int16_t)(uint16_t)t0;
	rtl_sr(cpu.eax, 4, &t0);
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
