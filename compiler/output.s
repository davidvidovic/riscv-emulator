.main:
	lui x5,2
	lui x5,1
	add x6,x5,x5
	add x7,x6,x5
	addi x28,x7,4
	lui x29,5
	lui x7,10
	lui x30,6
	lui x31,7
	sw b,x6
	lui x6,8
	sw h,x6
	lui x6,9
	sw i,x6
	lui x6,11
	add x28,x6,x5
	addi x7,x5,1
	addi x31,x31,99
.L1:
	beq x5,x0,.L3
	sw a,x5
	lw x5,b
	beq x5,x0,.L3
.L2:
	sw j,x6
	lw x6,a
	add x7,x5,x6
	jal x0,.L1
.L3:
	lui x28,19
