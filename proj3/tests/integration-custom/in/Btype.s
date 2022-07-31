addi t0, x0, 3
addi t1, x0, 5
addi t2, x0, -2

        beq t0, t1, end
        bne t0, t1, label2
label1:
        bge t0, t1, label3
        blt t0, t1, label3
label2:
        bgeu t1, t0, label1
label3:
        bltu t0, t2, end 
end:

