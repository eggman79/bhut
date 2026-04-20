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
extern print_int64

extern instr_ptr;
global test
global add
global run
global run_exit
global push
global iff
global printi8

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

iff:
  sub r12, 8
  mov rcx, qword[r12]
  test rcx, rcx
  jz if_false
  mov rcx, qword[r13 + 1]
  add r13, rcx
  jmp if_exit
if_false:
  add r13, 9
if_exit:
  next_instr

printi8:
  push r12
  push r13
  push r14
  mov rdi, qword[r12 - 8]
  call print_int64
  pop r14
  pop r13
  pop r12
  sub r12, 8
  inc r13
  next_instr
