.main:
	lui R1,0
	sw a,R1
	lui R5,1
	beq R5,R0,.(null)
	lw R6,a
	beq R6,R0,.L2
.L1:
	lui R11,2
	sw a,R11
.L2:
