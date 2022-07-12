.globl dot

.text
# =======================================================
# FUNCTION: Dot product of 2 int arrays
# Arguments:
#   a0 (int*) is the pointer to the start of arr0
#   a1 (int*) is the pointer to the start of arr1
#   a2 (int)  is the number of elements to use
#   a3 (int)  is the stride of arr0
#   a4 (int)  is the stride of arr1
# Returns:
#   a0 (int)  is the dot product of arr0 and arr1
# Exceptions:
#   - If the length of the array is less than 1,
#     this function terminates the program with error code 36
#   - If the stride of either array is less than 1,
#     this function terminates the program with error code 37
# =======================================================
dot:
	addi t0, x0, 1
	blt a2, t0, lengthError
	blt a3, t0, strideError
	blt a4, t0, strideError

    #prologue
	addi sp, sp, -20
	sw s0, 0(sp)
	sw s1, 4(sp)
	sw s2, 8(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)

	# loop counter t0
	add t0, x0, x0
	# sum container
	add s4, x0, x0

loop_start:
	# end the loop when the pointer meets the end 
	bge t0, a2, loop_end
	# address offset
	mv s0, a0
	mv s1, a1
	mul t1, t0, a3
	slli t1, t1, 2
	add s0, s0, t1
	lw s2, 0(s0)
	mul t2, t0, a4
	slli t2, t2, 2
	add s1, s1, t2
	lw s3, 0(s1)
	# calculation
	mul s0, s2, s3
	add s4, s4, s0
	addi t0, t0, 1
	j loop_start

loop_end:
	mv a0, s4
	# Epilogue
	lw s0, 0(sp)
	lw s1, 4(sp)
	lw s2, 8(sp)
	lw s3, 12(sp)
	lw s4, 16(sp)
	addi sp, sp, 20

	ret

lengthError:
	li a0, 36
	j exit
strideError:
	li a0, 37
	j exit