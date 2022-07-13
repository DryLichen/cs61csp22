.globl read_matrix

.text
# ==============================================================================
# FUNCTION: Allocates memory and reads in a binary file as a matrix of integers
#
# FILE FORMAT:
#   The first 8 bytes are two 4 byte ints representing the # of rows and columns
#   in the matrix. Every 4 bytes afterwards is an element of the matrix in
#   row-major order.
# Arguments:
#   a0 (char*) is the pointer to string representing the filename
#   a1 (int*)  is a pointer to an integer, we will set it to the number of rows
#   a2 (int*)  is a pointer to an integer, we will set it to the number of columns
# Returns:
#   a0 (int*)  is the pointer to the matrix in memory
# Exceptions:
#   - If malloc returns an error,
#     this function terminates the program with error code 26
#   - If you receive an fopen error or eof,
#     this function terminates the program with error code 27
#   - If you receive an fclose error or eof,
#     this function terminates the program with error code 28
#   - If you receive an fread error or eof,
#     this function terminates the program with error code 29
# ==============================================================================

read_matrix:
	# Prologue
	addi, sp, sp, -24
	sw s0, 0(sp)
	sw s1, 4(sp)
	sw s2, 8(sp)
	sw s3, 12(sp)
	sw s4, 16(sp)
	sw ra, 20(sp)

	# save the arguments
	mv s1, a1
	mv s2, a2

	# open the matrix file
	addi a1, x0, 0
	jal fopen
	mv s3, a0
	addi t0, x0, -1
	beq a0, t0, fopen_error

	# read the number of rows
	mv a1, s1
	addi a2, x0, 4
	jal fread
	addi t0, x0, 4
	bne a0, t0, fread_error

	# read the number of columns
	mv a0, s3
	mv a1, s2
	addi a2, x0, 4
	jal fread
	addi t0, x0, 4
	bne a0, t0, fread_error

	# allocate space on the heap to store the matrix
	lw t0, 0(s1)
	lw t1, 0(s2)
	mul s4, t0, t1
	slli s4, s4, 2
	mv a0, s4
	jal malloc
	beq a0, zero, malloc_error
	mv s0, a0

	# read the matrix
	mv a0, s3
	mv a1, s0
	mv a2, s4
	jal fread
	bne a0, s4, fread_error

	# close the file
	mv a0, s3
	jal fclose
	bne a0, zero, fclose_error

	# Epilogue
	mv a0, s0

	lw s0, 0(sp)
	lw s1, 4(sp)
	lw s2, 8(sp)
	lw s3, 12(sp)
	lw s4, 16(sp)
	lw ra, 20(sp)
	addi, sp, sp, 24

	ret

malloc_error:
	li a0, 26
	j exit

fopen_error:
	li a0, 27
	j exit

fclose_error:
	li a0, 28
	j exit

fread_error:
	li a0, 29
	j exit
