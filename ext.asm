%define call_stack_reg r12
%define stack_reg r13
%define code_reg r14
%define instr_reg r15

%macro next_instr 0
  mov al, byte[code_reg]
  movzx rcx, al
  shl rcx, 3
  add rcx, instr_reg
  jmp qword[rcx]
%endmacro

%macro if_cond 1
    %if %1 = 0     ; jz case (ZF)
        sub stack_reg, 8
        mov rcx, qword [stack_reg]
        test rcx, rcx
    %else
        mov rcx, qword[stack_reg - 16]
        mov rbx, qword[stack_reg - 8]
        sub stack_reg, 16
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

    add code_reg, 9
    jmp %%exit
%%true:
    mov rcx, qword [code_reg + 1]
    mov code_reg, rcx
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
  mov call_stack_reg, qword[rel call_stack_ptr]
  mov stack_reg, qword[rel stack_ptr]
  mov code_reg, rsi
  mov instr_reg, qword[rel instr_ptr]
  next_instr
run_exit:
  ret

push_instr:
  mov rcx, qword[code_reg + 1]
  mov qword[stack_reg], rcx
  add stack_reg, 8
  add code_reg, 9
  next_instr

pop_instr:
  sub stack_reg, 8
  next_instr

add_instr:
  mov rcx, qword[stack_reg - 8] 
  add qword[stack_reg - 16], rcx
  sub stack_reg, 8
  inc code_reg
  next_instr

sub_instr:
  mov rcx, qword[stack_reg - 8] 
  sub qword[stack_reg - 16], rcx
  sub stack_reg, 8
  inc code_reg
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
  mov rcx, qword[code_reg + 1]
  mov code_reg, rcx
  next_instr

call_instr:
  mov rcx, code_reg
  inc rcx
  sub stack_reg, 8
  mov code_reg, qword[stack_reg]
  mov qword[call_stack_reg], rcx
  add call_stack_reg, 8
  next_instr

ret_instr:
  sub call_stack_reg, 8
  mov code_reg, qword[call_stack_reg]
  next_instr

dup_instr:
  inc code_reg
  mov rcx, qword[stack_reg - 8]
  mov qword[stack_reg], rcx
  add stack_reg, 8
  next_instr

printi8:
  mov rdi, qword[stack_reg - 8]
  call print_int64
  sub stack_reg, 8
  inc code_reg
  next_instr
