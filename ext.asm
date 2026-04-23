%macro next_instr 0
  mov al, byte[r13]
  movzx rcx, al
  shl rcx, 3
  add rcx, r14
  jmp qword[rcx]
%endmacro

%macro if_cond 1
    %if %1 = 0     ; jz case (ZF)
        sub r12, 8
        mov rcx, qword [r12]
        test rcx, rcx
    %else
        mov rcx, qword[r12 - 16]
        mov rbx, qword[r12 - 8]
        sub r12, 16
        cmp rcx, rbx
    %endif
    
    %if %1 = 0
        jz %%true
    %elif %1 = 1
        jg %%true
    %elif %1 = 2
        jge %%true
    %elif %1 = 3
        jl %%true
    %elif %1 = 4
        jle %%true
    %endif

    add r13, 9
    jmp %%exit
%%true:
    mov rcx, qword [r13 + 1]
    mov r13, rcx
%%exit:
    next_instr
%endmacro

section .text

extern heap_ptr
extern stack_ptr
extern call_stack_ptr
extern print_int64
extern instr_ptr;

global run
global run_exit

global add_instr
global sub_instr
global jmp_instr
global push_instr
global pop_instr
global if_instr
global ifg_instr
global ifl_instr
global ifge_instr
global ifle_instr
global call_instr
global ret_instr
global dup_instr
global printi8

run:
  mov r11, qword[rel call_stack_ptr]
  mov r12, qword[rel stack_ptr]
  mov r13, rsi
  mov r14, qword[rel instr_ptr]
  next_instr
run_exit:
  ret

push_instr:
  mov rcx, qword[r13 + 1]
  mov qword[r12], rcx
  add r12, 8
  add r13, 9
  next_instr

pop_instr:
  sub r12, 8
  next_instr

add_instr:
  mov rcx, qword[r12 - 8] 
  add qword[r12 - 16], rcx
  sub r12, 8
  inc r13
  next_instr

sub_instr:
  mov rcx, qword[r12 - 8] 
  sub qword[r12 - 16], rcx
  sub r12, 8
  inc r13
  next_instr

if_instr:
  if_cond 0

ifg_instr:
  if_cond 1

ifge_instr:
  if_cond 2

ifl_instr:
  if_cond 3

ifle_instr:
  if_cond 4

jmp_instr:
  mov rcx, qword[r13 + 1]
  mov r13, rcx
  next_instr

call_instr:
  mov rcx, r13
  inc rcx
  sub r12, 8
  mov r13, qword[r12]
  mov qword[r11], rcx
  add r11, 8
  next_instr

ret_instr:
  sub r11, 8
  mov r13, qword[r11]
  next_instr

dup_instr:
  inc r13
  mov rcx, qword[r12 - 8]
  mov qword[r12], rcx
  add r12, 8
  next_instr

printi8:
  push r11
  push r12
  push r13
  push r14
  mov rdi, qword[r12 - 8]
  call print_int64
  pop r14
  pop r13
  pop r12
  pop r11
  sub r12, 8
  inc r13
  next_instr
