.main:
	lui x5,2
	add x6,x5,x5
	add x7,x6,x5
	addi R28,R7,4
	lui x7,5
	lui x30,6
	lui x31,7
	sw a,x5
	lui x5,8
	sw h,x5
	lui x5,9
	sw i,x5
	lui x6,11
	addi R5,R6,1
	sw b,x6
	add x28,x5,x6
	bne x0,x6,.L3
.L1:
.L2:
.L3:
