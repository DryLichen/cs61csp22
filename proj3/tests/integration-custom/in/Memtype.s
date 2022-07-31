addi t0, x0, 2
addi t1, x0, 256

addi sp, sp, -12
sw t1, 0(sp) 
sh t0, 4(sp)
sb t2, 8(sp)

lw a0, 0(sp)
lh a0, 4(sp)
lb a0, 8(sp)
addi sp, sp, 12