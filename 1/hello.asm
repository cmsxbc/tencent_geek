BITS 64
  org 0x400000
global    _start

ehdr:           ; Elf64_Ehdr
  db 0x7f, "ELF", 2, 1, 1, 0 ; e_ident
  times 8 db 0
  ; dd 0x67452301
  ; dd 0xefcdab89
; _start:

;   inc       al
;   shl       rax, 32
;   jmp     prepare_stack
  dw  2         ; e_type
  dw  0x3e      ; e_machine
  dd  1         ; e_version
  dq  _start    ; e_entry
  dq  phdr - $$ ; e_phoff
  dq  0         ; e_shoff
  dd  0         ; e_flags
  dw  ehdrsize  ; e_ehsize
  dw  phdrsize  ; e_phentsize
  dw  1         ; e_phnum
  dw  0         ; e_shentsize
  dw  0         ; e_shnum
  dw  0         ; e_shstrndx
; phdr:
;   dw  1         ; e_phnum
;   dw  0         ; e_shentsize
;   dw  7         ; e_shnum
;   dw  0         ; e_shstrndx
ehdrsize  equ  $ - ehdr
phdr:
  dw  1         ; e_phnum
  dw  0         ; e_shentsize
  dw  7         ; e_shnum
  dw  0         ; e_shstrndx
  dq  0         ; p_offset
  dq  $$        ; p_vaddr
  dq  $$        ; p_paddr
  dq  filesize  ; p_filesz
  dq  filesize  ; p_memsz
  dq  0x1000    ; p_align
  phdrsize  equ  $ - phdr

_start:
prepare_stack:
          inc       al
          shl       rax, 32
          mov       rbp, rsp
          sub       rsp, 0x220
          cvtsi2sd  xmm1, rax
          shr       rax, 32
calc_k:
          cvtsi2sd  xmm0, rax
          movsd     [rsp], xmm0
          fld       QWORD [rsp]
          fsin
          fabs
          movsd     [rsp], xmm1
          fmul      QWORD [rsp]
          fstp      QWORD [rsp]
          movsd     xmm0, [rsp]
          cvttsd2si rdx, xmm0
          mov       [rsp+rax*4+0x1c], rdx
          inc       al
          cmp       al, 64
          jle       calc_k
padding:
          mov qword rsi, 0x400000
          mov       rax, [rsi+0x60]
          mov  byte [rax+rsi], 0x80
          mov       rdx, rax
          push      rdx
          and       rdx, 0x3f
          ; use nothing if filesize % 64 == 0
          add       rax, 0x38 ; if filesize % 64 < 58; use this.()
          ;add       rax, 078; if filesize % 64 > 58; use this
          ;cmp       rdx, 0x38
          ;je        append_length
          ;jl        append_small
          ;add       rax, 0x40
;append_small:
          ;add       rax, 0x38
          sub       rax, rdx
append_length:
          pop       rdx
          shl       rdx, 3
          mov       [rsi+rax], rdx
          add       rax, 8
md5sum:
          mov       r8d, 0x67452301
          mov       r9d, 0xefcdab89
          mov       r10d, 0x98badcfe
          mov       r11d, 0x10325476
          push      r11
          push      r10
          push      r9
          push      r8

md5sum_loop:
          push      rax ;push filesize
          xor       rax, rax
md5sum_table_loop:
          mov       edx, r11d ; use edx for calcutate, use D for first
          push      rax
          mov       ah, al
          shr       ah, 4
          cmp       ah, 0
          je        md5sum_op0
          cmp       ah, 1
          je        md5sum_op1
          cmp       ah, 2
          je        md5sum_op2
md5sum_op3:
          not       edx
          or        edx, r9d
          xor       edx, r10d
          imul      eax, 7
          jmp       md5sum_loop_common
md5sum_op0:
          xor       edx, r10d
          and       edx, r9d
          xor       edx, r11d
          jmp       md5sum_loop_common
md5sum_op1:
          mov       edx, r9d
          xor       edx, r10d
          and       edx, r11d
          xor       edx, r10d
          imul      eax, 5
          inc       al
          jmp       md5sum_loop_common
md5sum_op2:
          xor       edx, r10d
          xor       edx, r9d
          imul      eax, 3
          add       al, 5
md5sum_loop_common:
          add       edx, r8d ; F + A

          and       rax, 0x0f;
          add       edx, [rsi+rax*4]; F + source[i]

          pop       rax
          add       edx, [rbp+rax*4-0x200]; F + K[i]

          mov       rcx, rax
          and       cl, 0xf0
          shr       cl, 2
          mov       rdi, rax
          and       rdi, 0x03
          add       rdi, rcx
          mov       rcx, rotates
          mov  byte cl, [rcx + rdi] ;rcx: s[i]

          rol       edx, cl

md5sum_rotate:
          push      r11
          mov       r11d, r10d
          mov       r10d, r9d
          add       r9d, edx
          pop       r8

md5sum_loop_check:
          inc       al
          cmp       al, 64
          jl        md5sum_table_loop

md5sum_mov2result:
          pop       rax ;pop filesize
          pop       rbx
          add       r8d, ebx
          pop       rbx
          add       r9d, ebx
          pop       rbx
          add       r10d, ebx
          pop       rbx
          add       r11d, ebx
          push      r11
          push      r10
          push      r9
          push      r8


          add       rsi, 64
          sub       rax, 64
          jnz       md5sum_loop

push_result_4_output:
          shl       r11, 32
          or        r11, r10
          push      r11
          shl       r9, 32
          or        r9, r8
          push      r9

convert_result:
          mov  byte [rbp], 0xa ; output buf + 33 <- \n
          ; xor       rcx, rcx
          xor       r10, r10
          xor       r8, r8
          mov       rsi, rbp
          sub       rsi, 0x20
convert_result_loop:
          mov  byte r8b, [rsp+rax]
          xor       r9, r9
          mov       r9b, r8b
          shr       r8b, 4
convert_result_d:
          and       r8b, 0x0f
          add       r8b, 0x30
          cmp       r8b, 0x39
          jna       convert_result_save
          add       r8b, 0x27
convert_result_save:
          mov  byte [rsi+r10], r8b
          inc       r10
          cmp       r9, 0x100
          jnl       convert_result_next
          mov       r8b, r9b
          mov       r9, 0x100
          jmp       convert_result_d
convert_result_next:
          inc       al
          cmp       al, 16
          jl        convert_result_loop
output:
          mov       rdx, 33  ; outputcount
          mov       al, 1
          mov       dil, 1    ; fd of stdout
          syscall
exit:
          mov       al, 60
          dec       dil
          syscall


rotates   db 7, 12, 17, 22
          db 5, 9, 14, 20
          db 4, 11, 16, 23
          db 6, 10, 15, 21


filesize  equ  $ - $$