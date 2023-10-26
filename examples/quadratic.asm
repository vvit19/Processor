call input_coeffs
call count_discr
call solve_equation
hlt

input_coeffs:

    in
    in
    in
    pop rcx
    pop rbx
    pop rax
    ret

count_discr:

    push rbx
    push rbx
    mult ; rbx^2 (1)

    push -4
    push rax
    push rcx
    mult ; a*c
    mult ; -4*a*c (2)

    add
    pop rdx
    ret

solve_equation:

    push 0
    push rax
    je solve_linear

    push 0
    push rdx
    jb output_no_sols ; if (d < 0) no solutions

    push rdx
    sqrt ; sqrt(d)
    push rbx
    sub ; -b + sqrt(d) (1)

    push 2
    push rax
    mult ; 2 * a (2)

    div
    out ; output first solution

    push 0
    push rdx
    je exit ; if (d == 0) one solution => exit

    push rdx
    sqrt
    push rbx
    add
    push -1
    mult

    push 2
    push rax
    mult ; 2 * a (2)

    div
    out
    ret

solve_linear:

    push rbx
    push 0
    je const_eq_zero

    push rcx
    push -1
    mult
    push rbx
    div

    out

    hlt

const_eq_zero:

    push rcx
    push 0
    je output_inf
    jump output_no_sols

output_inf:

    push 105 ; 'i'
    outc
    push 110 ; 'n'
    outc
    push 102 ; 'f'
    outc
    push 10 ; '\n'
    outc

    hlt


output_no_sols:

    push 110 ; 'n'
    outc
    push 111 ; 'o'
    outc
    push 32 ; ' '
    outc
    push 115 ; 's'
    outc
    push 111 ; 'o'
    outc
    push 108 ; 'l'
    outc
    push 115 ; 's'
    outc
    push 10 ; '\n'
    outc

    hlt


exit:

    hlt
