addi t0, x0, 3
addi t1, x0, 7

add a0, t0, t1
sub s1, a0, t1

and a0, t0, t1
or a0, t0, t1
xor a0, t0, t1

sll s1, t1, t0
srl s1, t1, t0
sra a0, t1, t0

slt a0, t1, t0

mul a0, t0, t1

addi t0, x0, 256
addi t0, x0, 2000
mulh a0, t0, t1
mulhu a0, t0, t1