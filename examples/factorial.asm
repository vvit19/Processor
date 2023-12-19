call main
hlt

main:
pop rcx
push 0
pop [rax + 0]
in
pop [rax + 0]
push rcx
push [rax + 0]
push rax + 1
pop rax
call factorial
pop rcx
push rax
push 1
sub
pop rax
push rbx
pop [rax + 1]
push [rax + 1]
out
push 0
pop rbx
push rcx
ret

factorial:
pop rcx
pop [rax + 0]
push [rax + 0]
push 1
jne if_0
push 1
pop rbx
push rcx
ret

if_0:
push [rax + 0]
push 1
sub
pop [rax + 1]
push rcx
push [rax + 1]
push rax + 2
pop rax
call factorial
pop rcx
push rax
push 2
sub
pop rax
push rbx
pop [rax + 2]
push [rax + 0]
push [rax + 2]
mult
pop rbx
push rcx
ret
