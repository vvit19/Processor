in ; push n
pop rax ; rax = n

push 1
pop rbx ; rbx = k

push 1
pop rcx ; rcx = k!

call fact

push rcx
out

hlt

fact:
    push rax
    push rbx
    je exit ; if (n == k) return; (0)
    push rbx
    push 1
    add ;
    pop rbx ; rbx++ (0)
    push rbx
    push rcx
    mult
    pop rcx ; rcx *= k (0)
    call fact

exit:
    ret
