.globl matmul

.text
# =======================================================
# FUNCTION: Matrix Multiplication of 2 integer matrices
#   d = matmul(m0, m1)
# Arguments:
#   a0 (int*)  is the pointer to the start of m0
#   a1 (int)   is the # of rows (height) of m0
#   a2 (int)   is the # of columns (width) of m0
#   a3 (int*)  is the pointer to the start of m1
#   a4 (int)   is the # of rows (height) of m1
#   a5 (int)   is the # of columns (width) of m1
#   a6 (int*)  is the pointer to the the start of d
# Returns:
#   None (void), sets d = matmul(m0, m1)
# Exceptions:
#   Make sure to check in top to bottom order!
#   - If the dimensions of m0 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m1 do not make sense,
#     this function terminates the program with exit code 38
#   - If the dimensions of m0 and m1 don't match,
#     this function terminates the program with exit code 38
# =======================================================
matmul:
	# Error checks
	addi t0, x0, 1
	blt a1, t0, error
	blt a2, t0, error
	blt a4, t0, error
	blt a5, t0, error
	bne a2, a4, error

	# Prologue
	addi sp, sp, -32
	sw s0, 0(sp)
	sw s1, 4(sp)
	sw s2, 8(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw s5, 20(sp)
	sw s6, 24(sp)
	sw ra, 28(sp)
	# save the arguments
	mv s0, a0
	mv s1, a1
	mv s2, a2
	mv s3, a3
	mv s4, a4
	mv s5, a5
	mv s6, a6

	# outer loop counter
	add t0, x0, x0

# iterate all the rows in m0
outer_loop_start:
	# end the loop at the last row
	bge t0, s1, outer_loop_end
	# inner loop counter
	add t1, x0, x0

	# address offset of the first array
	mul t2, t0, s2
	slli t2, t2, 2
	add t2, s0, t2
	mv a0, t2	

# iterate all the columns in m1
inner_loop_start:
	# end the loop at the last column
	bge t1, s5, inner_loop_end
	
	# initialize part of the arguments of function dot
	mv a2, s2
	addi a3, x0, 1
	mv a4, s5

	# address offset of the first array
	slli t2, t1, 2
	add t2, s3, t2
	mv a1, t2	

	# save temporary registers
	addi sp, sp, -12
	sw t0, 0(sp)
	sw t1, 4(sp)
	sw a0, 8(sp)
	# call function dot
	jal ra, dot
	#restore temporary registers
	lw t0, 0(sp)
	lw t1, 4(sp)
	# save the value in matrix c
	mul t2, t0, s5  
	add t2, t2, t1
	slli t2, t2, 2
	add t2, s6, t2
	sw a0, 0(t2)
	# restore a0
	lw a0, 8(sp)
	addi sp, sp, 12
	
	addi t1, t1, 1	# increase the counter
	j inner_loop_start

inner_loop_end:
	addi t0, t0, 1	# increase the counter
	j outer_loop_start

outer_loop_end:
	# Epilogue
	lw s0, 0(sp)
	lw s1, 4(sp)
	lw s2, 8(sp)
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw s5, 20(sp)
	lw s6, 24(sp)
	lw ra, 28(sp)
	addi sp, sp, 32

	ret

error:
	li a0, 38
	j exit