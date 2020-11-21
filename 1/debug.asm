global    _start

section   .text
_start:
          pop       rdi ; argc
          pop       rdi ; &argv[0]
openfile:
          xor       rax, rax
          mov       rax, 2    ; sys_open
          xor       rsi, rsi  ; flags
          xor       rdx, rdx  ; mode: read
          syscall
prepare_stack:
          mov       rbp, rsp
          sub       rsp, 0x5000
readfile:
          mov       rdi, rax  ; fd
          xor       rax, rax  ; sys_read
          mov       rsi, rbp  ; buf
          sub       rsi, 0x4000;
          mov       rdx, 0x4000 ;bufsize
          syscall
          mov       [rbp-0x4500], rax
convert_result:
          mov  byte [rbp-0x48e0], 0xa ; output buf + 33 <- \n
          mov       rcx, 0
          mov       r10, 0
          mov       rdx, rbp
          sub       rdx, 0x4000
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
          xor       rdx, rdx
          mov       rdx, 33  ; outputcount
          xor       rax, rax
          mov       rax, 1    ; sys_write
          xor       rdi, rdi
          inc       rdi       ; fd of stdout
          syscall
exit:
          mov       rax, 60
          mov       rdi, 0
          syscall

section   .data
rotates   db 7, 12, 17, 22
          db 5, 9, 14, 20
          db 4, 11, 16, 23
          db 6, 10, 15, 21
init_a    dd 0x67452301
init_b    dd 0xefcdab89
init_c    dd 0x98badcfe
init_d    dd 0x10325476
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
