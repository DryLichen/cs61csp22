.globl relu

.text
# ==============================================================================
# FUNCTION: Performs an inplace element-wise ReLU on an array of ints
# Arguments:
#   a0 (int*) is the pointer to the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   None
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# ==============================================================================
relu:
	addi t0, x0, 1
	bge a1, t0, prologue
	li a0, 36
	j exit

prologue:
	addi sp, sp, -8
	sw s0, 0(sp)
	sw s1, 4(sp)
	add t0, x0, x0

loop_start:
	add s0, x0, a0
	# stop the loop when the counter equals the size
	beq t0, a1, loop_end
	# get the address offset
	slli t1, t0, 2
	add s0, s0, t1
	lw s1, 0(s0)
	# set value to zero conditional
	bge s1, x0, loop_continue
	add s1, x0, x0

loop_continue:
	sw s1, 0(s0)  
	addi t0, t0, 1
	j loop_start

loop_end:
	# Epilogue
	lw s0, 0(sp)
	lw s1, 4(sp)
	addi sp, sp, 8
	ret
