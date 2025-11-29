.section
.text
main:
    addi x5,x0,4
    sw x5,-4(x8)
    lw x6,-4(x8)
    sw x6,-8(x8)
    addi x7,x0,1
    sw x7,-12(x8)
    beq x6,x7,L1
    beq x0,x0,L2
L1:
    addi x28,x0,1
    sw x28,-16(x8)
    lw x6,-8(x8)
    lw x28,-16(x8)
    add x29,x6,x28
    sw x29,-20(x8)
    lw x6,-20(x8)
    sw x6,-8(x8)
# ---- start of spill ----
    sw x29,-20(x8)
    sw x7,-12(x8)
    sw x28,-16(x8)
    sw x6,-8(x8)
    sw x5,-4(x8)
# ---- end of spill ----
    beq x0,x0,L0
L2:
    addi x5,x0,2
    sw x5,-24(x8)
    beq x6,x5,L3
    beq x0,x0,L4
L3:
    addi x7,x0,2
    sw x7,-28(x8)
    lw x6,-8(x8)
    lw x7,-28(x8)
    add x28,x6,x7
    sw x28,-32(x8)
    lw x6,-32(x8)
    sw x6,-8(x8)
# ---- start of spill ----
    sw x7,-28(x8)
    sw x5,-24(x8)
    sw x28,-32(x8)
    sw x6,-8(x8)
# ---- end of spill ----
    beq x0,x0,L0
L4:
    addi x5,x0,3
    sw x5,-36(x8)
    lw x7,-8(x8)
    lw x5,-36(x8)
    add x6,x7,x5
    sw x6,-40(x8)
    lw x7,-40(x8)
    sw x7,-8(x8)
L0:
    addi x28,x0,0
    sw x28,-44(x8)
    addi a0,x28,0
    addi x5,x0,0
    sw x5,-60(x8)
end:
