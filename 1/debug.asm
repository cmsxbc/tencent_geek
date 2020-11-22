global    _start

section   .text
_start:
prepare_stack:
          mov       rbp, rsp
          sub       rsp, 0x5000
padding:
          mov       rax, 0x2718; hardcore size
          mov  byte [rax+0x400000], 0x80
          mov       rdx, rax
          mov       rdi, rax
          and       rdx, 0x3f
          cmp       rdx, 0x38
          je        append_length
          cmp       rdx, 0x38
          jl        append_small
          add       rax, 0x40
append_small:
          add       rax, 0x38
          sub       rax, rdx
append_length:
          shl       rdi, 3
          mov       [rax+0x400000], rdi
          add       rax, 8
          mov       rsi, 0x400000
md5sum:
          mov dword [rbp-0x4400], 0x67452301 ; init a
          mov dword [rbp-0x43fc], 0xefcdab89 ; init b
          mov dword [rbp-0x43f8], 0x98badcfe ; init c
          mov dword [rbp-0x43f4], 0x10325476 ; init d
md5sum_loop:
          mov       r8d, [rbp-0x4400]
          mov       r9d, [rbp-0x43fc]
          mov       r10d, [rbp-0x43f8]
          mov       r11d, [rbp-0x43f4]
          push      rax ;push filesize
          mov       rax, 0
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

          mov       rcx, k
          pop       rax
          add       edx, [rcx+rax*4]; F + K[i]

          mov       rcx, rax
          and       cl, 0xf0
          shr       cl, 2
          mov       rdi, rax
          and       rdi, 0x03
          add       rdi, rcx
          mov       rcx, rotates
          mov  byte cl, [rcx + rdi] ;rcx: s[i]
          and       rcx, 0xff

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
          add       r8d, [rbp-0x4400]
          add       r9d, [rbp-0x43fc]
          add       r10d, [rbp-0x43f8]
          add       r11d, [rbp-0x43f4]
          mov       [rbp-0x4400], r8d
          mov       [rbp-0x43fc], r9d
          mov       [rbp-0x43f8], r10d
          mov       [rbp-0x43f4], r11d
          add       rsi, 64
          pop       rax ;pop filesize
          sub       rax, 64
          jnz       md5sum_loop
convert_result:
          mov  byte [rbp-0x48e0], 0xa ; output buf + 33 <- \n
          xor       rcx, rcx
          xor       r10, r10
          xor       r8, r8
          mov       rdx, rbp
          sub       rdx, 0x4400
          mov       rsi, rbp
          sub       rsi, 0x4900
convert_result_loop:
          mov  byte r8b, [rdx+rcx]
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
          jmp        convert_result_d
convert_result_next:
          inc       rcx
          cmp       rcx, 16
          jl        convert_result_loop
output:
          mov       rsi, rbp
          sub       rsi, 0x4900    ; output buf
          mov       rdx, 33  ; outputcount
          mov       rax, 1    ; sys_write
          mov       rdi, 1    ; fd of stdout
          syscall
exit:
          mov       rax, 60
          dec       rdi
          syscall

section   .data
rotates   db 7, 12, 17, 22
          db 5, 9, 14, 20
          db 4, 11, 16, 23
          db 6, 10, 15, 21
k         dd 0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee
          dd 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501
          dd 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be
          dd 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
          dd 0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa
          dd 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8
          dd 0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed
          dd 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
          dd 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c
          dd 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70
          dd 0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05
          dd 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
          dd 0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039
          dd 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1
          dd 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1
          dd 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391