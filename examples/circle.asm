call main

; \n ascii
push 10
outc

hlt

main:

    push 0
    pop rax ; x coord

    push 0
    pop rbx ; y coord

    push 0
    pop rcx ; counter

    call push_to_ram

    call print_circle

    ret

push_to_ram:

    push rcx
    push 121
    je exit ; (if i == RAM_SIZE)

    push rax
    push 11
    je next_line ; (if x == 11)
    next_line_ret:

    call calc_dist

    push rdx
    push 25
    jae ram_push_el
    ram_push_el_ret:

    push rax + 1
    pop rax ; x coord++

    push rcx + 1
    pop rcx ; i++

    jump push_to_ram

exit:

    ret

next_line:

    push 0
    pop rax

    push rbx + 1
    pop rbx

    jump next_line_ret

calc_dist:

    push rax
    push 5
    sub
    push rax
    push 5
    sub
    mult ; (x - 5)^2

    push rbx
    push 5
    sub
    push rbx
    push 5
    sub
    mult ; (y - 5)^2

    add
    pop rdx

    ret

ram_push_el:

    push rdx
    pop [rcx]

    jump ram_push_el_ret

print_circle:

    push 0
    pop rcx ; counter_1 = 0

    push 0
    pop rdx ; counter_2 = 0

    for_circle:

        push rcx
        push 121
        je exit ; (if i == RAM_SIZE)

        push rdx
        push 11
        je out_enter
        out_enter_ret:

        push [rcx]
        push 0
        jne out_zv

        push 32 ; ' ' ascii
        outc
        push 46 ; '.' ascii
        outc
        push 32 ; ' ' ascii
        outc

        out_zv_ret:

        push rcx + 1
        pop rcx

        push rdx + 1
        pop rdx

        jump for_circle


out_enter:

    push 10 ; \n ascii
    outc

    push 0
    pop rdx

    jump out_enter_ret

out_zv:

    push 32 ; ' ' ascii
    outc
    push 42 ; '*' ascii
    outc
    push 32 ; ' ' ascii
    outc

    jump out_zv_ret
