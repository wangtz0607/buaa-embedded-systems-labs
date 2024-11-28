    .arch armv7-a
    .text
    .global sort
    .thumb
    .thumb_func
sort:
    push {r7}
    sub sp, sp, #28
    mov r7, sp
    @ [r7]      = n
    @ [r7, #4]  = a
    @ [r7, #12] = t
    @ [r7, #16] = j
    @ [r7, #20] = i
    str r0, [r7, #4]
    str r1, [r7]
    mov r3, #0          @ 0
    str r3, [r7, #20]   @ i = 0
    b .L2
.L6:
    mov r3, #0          @ 0
    str r3, [r7, #16]   @ j = 0
    b .L3
.L5:
    ldr r3, [r7, #16]   @ j
    lsl r3, r3, #2      @ j * 4
    ldr r2, [r7, #4]    @ a
    add r3, r3, r2      @ &a[j]
    ldr r2, [r3]        @ a[j]
    ldr r3, [r7, #16]   @ j
    add r3, r3, #1      @ j + 1
    lsl r3, r3, #2      @ (j + 1) * 4
    ldr r1, [r7, #4]    @ a
    add r3, r3, r1      @ &a[j + 1]
    ldr r3, [r3]        @ a[j + 1]
    cmp r2, r3          @ if (a[j] <= a[j + 1])
    ble .L4
    ldr r3, [r7, #16]   @ j
    add r3, r3, #1      @ j + 1
    lsl r3, r3, #2      @ (j + 1) * 4
    ldr r2, [r7, #4]    @ a
    add r3, r3, r2      @ &a[j + 1]
    ldr r3, [r3]        @ a[j + 1]
    str r3, [r7, #12]   @ t = a[j + 1]
    ldr r3, [r7, #16]   @ j
    add r3, r3, #1      @ j + 1
    lsl r3, r3, #2      @ (j + 1) * 4
    ldr r2, [r7, #4]    @ a
    add r3, r3, r2      @ &a[j + 1]
    ldr r2, [r7, #16]   @ j
    lsl r2, r2, #2      @ j * 4
    ldr r1, [r7, #4]    @ a
    add r2, r2, r1      @ &a[j]
    ldr r2, [r2]        @ a[j]
    str r2, [r3]        @ a[j + 1] = a[j]
    ldr r3, [r7, #16]   @ j
    lsl r3, r3, #2      @ j * 4
    ldr r2, [r7, #4]    @ a
    add r3, r3, r2      @ &a[j]
    ldr r2, [r7, #12]   @ t
    str r2, [r3]        @ a[j] = t
.L4:
    ldr r3, [r7, #16]   @ j
    add r3, r3, #1      @ j + 1
    str r3, [r7, #16]   @ j = j + 1
.L3:
    ldr r2, [r7]        @ n
    ldr r3, [r7, #20]   @ i
    sub r3, r2, r3      @ n - i
    sub r2, r3, #1      @ n - i - 1
    ldr r3, [r7, #16]   @ j
    cmp r3, r2          @ if (j < n - i - 1)
    blt .L5
    ldr r3, [r7, #20]   @ i
    add r3, r3, #1      @ i + 1
    str r3, [r7, #20]   @ i = i + 1
.L2:
    ldr r2, [r7, #20]   @ i
    ldr r3, [r7]        @ n
    sub r3, #1          @ n - 1
    cmp r2, r3          @ if (i < n - 1)
    blt .L6
    add sp, sp, #28
    pop {r7}
    bx lr
