global    _start

section   .text
_start:
          pop       rsi ; argc
          pop       rsi ; &argv[0]
          xor       rdx, rdx ;
          mov       rbx, rsi
strlen:
          mov       rax, [rbx]
          inc       rdx
          inc       rbx
          test      al, al
          jnz       strlen
output:
          xor       rax, rax
          mov       rax, 1
          xor       rdi, rdi
          inc       rdi
          syscall
exit:
          mov       rax, 60
          mov       rdi, 0
          syscall
