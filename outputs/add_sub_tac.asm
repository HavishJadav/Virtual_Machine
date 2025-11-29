.section
.text
main:
    addi x5,x0,2
    sw x5,-4(x8)
    addi x6,x0,3
    sw x6,-8(x8)
    lw x5,-4(x8)
    lw x6,-8(x8)
    add x7,x5,x6
    sw x7,-12(x8)
    lw x5,-4(x8)
    lw x6,-8(x8)
    sub x28,x5,x6
    sw x28,-16(x8)
    addi x29,x0,0
    addi a0,x29,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
