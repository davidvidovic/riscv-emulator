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
	sw j,x6
	lui x6,3
	sw c,x7
	add x7,x5,x6
	add x28,x7,x5
	sw b,x6
	addi x6,x5,1
	addi x31,x31,99
	addi x30,x30,88
	sw c,x6
	lw x6,i
	sw j,x7
	add x7,x31,x6
	sw i,x6
	lui x6,3
	sw d,x28
	lui x28,9
.L1:
	lw x5,i
	lw x7,b
	bge x5,x7,.L3
.L2:
	sw h,x6
	add x6,x7,x28
	jal x0,.L1
.L3:
	sw c,x6
	lui x6,19
