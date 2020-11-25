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
          ; bts       rax, 32
          pop       rsi
          shl       rsi, 32
          push      rsi
          fild      QWORD [rsp]
          push      rsp
          pop       rbp
          mov       cl, 64
calc_k:
          push      rcx
          fild      QWORD [rsp]
          fsin
          fabs
          fmul      st1
          fisttp    QWORD [rsp]
          loop       calc_k
padding:

          shr       rsi, 10
          mov       rax, [rsi+0x60]
          mov  byte [rax+rsi], 0x80
          add       rax, pad_len
append_length:
          mov qword [rsi+rax-8], filesizeb
md5sum:
          mov       r8d, 0x67452301
          mov       r9d, 0xefcdab89
          mov       r10d, 0x98badcfe
          mov       r11d, 0x10325476

md5sum_loop:
          push      r11
          push      r10
          push      r9
          push      r8
          push      rax ;push filesize
          xor       rax, rax
          ; mov       cl, 0x10
md5sum_table_loop:
          mov       ebx, r11d ; use ebx for calcutate, use D for first
          push      rax
          ; div       cl
          mov       ah, al
          shr       ah, 5 ; consider shr 1 bit for 0x0-0x4
                              ; 00: zf, pf
                              ; 01: zf, pf, cf
                              ; 10: <nothing>
                              ; 11: cf
                              ; ja: cf=0&zf=0
                              ; jpo: pf=0
                              ; jb: cf=1
          ja        md5sum_op2
          jpo       md5sum_op3
          jb        md5sum_op1
md5sum_op0:
          xor       ebx, r10d
          and       ebx, r9d
          xor       ebx, r11d
          jmp       md5sum_loop_common
md5sum_op2:
          xor       ebx, r10d
          xor       ebx, r9d
          imul      eax, 3
          add       al, 5
          jmp       md5sum_loop_common
md5sum_op3:
          not       ebx
          or        ebx, r9d
          xor       ebx, r10d
          imul      eax, 7
          jmp       md5sum_loop_common
md5sum_op1:
          mov       ebx, r9d
          xor       ebx, r10d
          and       ebx, r11d
          xor       ebx, r10d
          imul      eax, 5
          inc       al
md5sum_loop_common:
          add       ebx, r8d ; F + A

          and       rax, 0x0f;
          add       ebx, [rsi+rax*4]; F + source[g]

          pop       rax
          add       ebx, [rsp+rax*8+40]; F + K[i]


          mov       cl, al
          and       cl, 0xf0
          shr       cl, 2
          push      rax

          and       al, 0x03
          mov  byte cl, [rotates + rax + rcx] ;rcx: s[i]
          pop       rax
          rol       ebx, cl
md5sum_rotate:
          xadd      r9d, ebx
          xchg      r10d, ebx
          mov       r8d, r11d
          mov      r11d, ebx

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

          add       rsi, 64
          sub       rax, 64
          jnz       md5sum_loop

push_result_4_output:
          mov       rcx, 0x20
          shrd      rax, r10, cl
          shrd      rax, r11, cl
          shrd      rdx, r8, cl
          shrd      rdx, r9, cl
          bswap     rdx
          bswap     rax


convert_result:
          mov  byte [rsi], 0xa ; output buf + 33 <- \n
          sub       rsi, rcx
convert_result_loop:
          mov  byte bl, al
convert_result_d:
          and       bl, 0x0f
          add       bl, 0x30
          cmp       bl, 0x39
          jna       convert_result_save
          add       bl, 0x27
convert_result_save:
          mov  byte [rsi+rcx-1], bl
          shrd      rax, rdx, 4
          shr       rdx, 4
          loop      convert_result_loop
output:
          mov       dl, 33  ; outputcount
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
; use nothing if filesize % 64 == 56
hehe      equ 0x40 ;if filesize % 64 < 56; use this.()
; hehe      equ 0x80 ; if filesize % 64 > 56; use this
pad_len   equ  hehe - (filesize % 64)
filesizeb equ filesize * 8
