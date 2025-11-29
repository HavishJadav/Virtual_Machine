.text
.global main
.global main
main:
    lw x5, -4(x8)
    sw x5, -8(x8)
    lw x6, -12(x8)
    sw x6, -16(x8)
    lw x7, -20(x8)
    sw x7, -24(x8)
    lw x6, -16(x8)
    lw x7, -24(x8)
    add x7, x6, x7
    sw x7, -24(x8)
    li x28, 0
    mv a0, x28
    ret
end:
