addi x4, x4, 1
slli x4, x4, 2
slti x5, x4, 5
sltiu x6, x4, -1
xori x7, x6, 2
ori x8, x5, 6
andi x9, x8, 5
add x10, x8, x9 
sub x11, x10, x9
slt x12, x11, x10
xor x13, x10, x9
or x14, x10, x9
and x15, x10, x9
addi x20, x20, 20
addi x21, x20, 10
addi x22, x21, 10
sb x7, 0(x20) 
sh x8, 0(x21)
sw x9, 0(x22)
lb x20, 0(x20)
lh x21, 0(x21)
lw x22, 0(x22)
addi x23, x23, -1
addi x24, x24, 1
addi x23, x23, -1
bgeu x23, x24, l1
addi x23, x23, -1
l1: addi x24, x24, 1
