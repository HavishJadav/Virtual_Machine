.section
.text
main:
    addi x5,x0,24
    sw x5,-4(x8)
    addi x6,x0,5
    sw x6,-8(x8)
    lw x5,-4(x8)
    lw x6,-8(x8)
    div x7,x5,x6
    sw x7,-12(x8)
    addi x28,x0,0
    addi a0,x28,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
