.section
 .data
msg: 
.asciz "Hello 1"
a: 
.asciz "Hello 2"
b: 
.asciz "Hello 3"
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
-STR a
    lui x6,0x10010
    addi x6,x6,0x0
    sw x6,-8(x8)
    lw x6,-8(x8)
    addi a0,x6,0
    addi a7,x0,4
    ecall
-STR b
    lui x7,0x10010
    addi x7,x7,0x0
    sw x7,-12(x8)
    lw x7,-12(x8)
    addi a0,x7,0
    addi a7,x0,4
    ecall
    addi x28,x0,0
    addi a0,x28,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
