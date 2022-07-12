.globl argmax

.text
# =================================================================
# FUNCTION: Given a int array, return the index of the largest
#   element. If there are multiple, return the one
#   with the smallest index.
# Arguments:
#   a0 (int*) is the pointer to the start of the array
#   a1 (int)  is the # of elements in the array
# Returns:
#   a0 (int)  is the first index of the largest element
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
# =================================================================
argmax:
	# length of the array can't less than 1
	addi t0, x0, 1
	bge a1, t0, prologue
	li a0, 36
	j exit

prologue:
	addi sp, sp -12
	sw s0, 0(sp)
	sw s1, 4(sp)
	sw s2, 8(sp)
	# loop counter t0
	add t0, x0, x0
	# load the 0th item in s2, index in t2
	lw s2, 0(a0)
	add t2, x0, x0

loop_start:
	# end the loop if the pointer meets the end
	bge t0, a1, loop_end

	add s0, a0, x0
	slli t1, t0, 2
	add s0, s0, t1
	lw s1, 0(s0)
	# skip the exchange if s1 is not the larger number
	bge s2, s1, loop_continue
	add s2, s1, x0
	add t2, t0, x0

loop_continue:
	addi t0, t0, 1
	j loop_start

loop_end:
	# Epilogue
	lw s0, 0(sp)
	lw s1, 4(sp)
	lw s2, 8(sp)
	addi sp, sp, 12
	add a0, t2, x0
	ret
