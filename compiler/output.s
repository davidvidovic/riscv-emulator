.main:
	lui R0,0
	sw a,R0
	lui R4,3
	sw b,R4
	lw R8,b
	beq R8,R0,.(null)
	lui R9,3
	beq R9,R0,.L1
	lui R13,2
	sw a,R13
.L1:
	lui R19,1
	sw c,R19
