.section
.text
main:
    addi x5,x0,15
    sw x5,-4(x8)
    lw x6,-4(x8)
    sw x6,-8(x8)
    addi x7,x0,1
    sw x7,-12(x8)
    lw x28,-12(x8)
    sw x28,-16(x8)
    addi x29,x0,1
    sw x29,-20(x8)
    lw x30,-20(x8)
    sw x30,-24(x8)
    addi x31,x0,1
    sw x31,-28(x8)
    beq x6,x31,L1
    beq x0,x0,L2
L1:
    addi a0,x28,0
    addi x5,x0,0
    sw x5,-60(x8)
# ---- start of spill ----
    sw x30,-24(x8)
    sw x29,-20(x8)
    sw x31,-28(x8)
    sw x28,-16(x8)
    sw x7,-12(x8)
    sw x6,-8(x8)
    sw x5,-4(x8)
# ---- end of spill ----
    beq x0,x0,L0
L2:
L0:
L3:
    addi x5,x0,2
    sw x5,-32(x8)
    lw x7,-8(x8)
    lw x5,-32(x8)
    sub x6,x7,x5
    sw x6,-36(x8)
    beq x6,x0,L5
L4:
    lw x28,-16(x8)
    lw x29,-24(x8)
    add x6,x28,x29
    sw x6,-36(x8)
    lw x30,-36(x8)
    sw x30,-40(x8)
    lw x29,-16(x8)
    sw x29,-24(x8)
    lw x28,-40(x8)
    sw x28,-16(x8)
# ---- start of spill ----
    sw x30,-40(x8)
    sw x6,-36(x8)
    sw x5,-32(x8)
    sw x29,-24(x8)
    sw x28,-16(x8)
    sw x7,-8(x8)
# ---- end of spill ----
    beq x0,x0,L3
L5:
    addi x5,x0,0
    sw x5,-36(x8)
    addi a0,x5,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
