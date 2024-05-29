.main:
	lui R0,0
	sw a,R0
	lui R4,3
	sw b,R4
	lw R8,a
	bne R8,R0,.L2
	lw R9,b
	beq R9,R0,.L2
.L1:
	lui R14,2
	sw a,R14
.L2:
	lui R20,1
	sw c,R20
