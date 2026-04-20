%macro next_instr 0
  mov al, byte[r13]
  movzx rcx, al
  shl rcx, 3
  add rcx, r14
  jmp qword[rcx]
%endmacro

section .text

extern heap_ptr
extern stack_ptr
extern instr_ptr;
global test
global add
global run
global run_exit
global push
global print

run:
  mov r12, qword[rel stack_ptr]
  mov r13, rsi
  mov r14, qword[rel instr_ptr]
  next_instr
run_exit:
  ret

push:
  mov rcx, qword[r13 + 1]
  mov qword[r12], rcx
  add r12, 8
  add r13, 9
  next_instr

add:
  mov rcx, qword[r12 - 8] 
  add qword[r12 - 16], rcx
  sub r12, 8
  inc r13
  next_instr
