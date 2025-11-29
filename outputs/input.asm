.section
.text
main:
input b STR
    addi a0,x5,0
    addi a7,x0,4
    ecall
    addi x6,x0,0
    addi a0,x6,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
