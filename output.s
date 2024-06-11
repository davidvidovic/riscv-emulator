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
	sw a,x5
	lw x5,b
	sw j,x6
	lw x6,a
	bge x5,x6,.L2
.L1:
	lui x31,77
	jal x0,.L5
.L2:
	lui x7,88
	bge x31,x7,.L4
.L3:
	sw a,x6
	lui x6,19
.L4:
	lui x28,9
.L5:
	lui x28,19
.L6:
	sw j,x6
	lui x6,3
	beq x6,x0,.L8
	beq x5,x0,.L8
.L7:
	addi x5,x5,1
	jal x0,.L6
.L8:
	lui x6,0
.L9:
	sw i,x6
	lui x6,10
	sw c,x7
	lw x7,i
	bge x6,x7,.L10
	lw x6,a
	add x6,x7,x6
	jal x0,.L9
.L10:
