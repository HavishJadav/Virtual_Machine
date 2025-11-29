.section
 .data
msg: 
.asciz "Hello, RISC-V!"
.section
.text
main:
    lui x5,0x10010
    addi x5,x5,0x0
    sw x5,-4(x8)
    lw x5,-4(x8)
    addi a0,x5,0
    addi a7,x0,4
    ecall
    addi x6,x0,0
    addi a0,x6,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
