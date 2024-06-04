.main:
	lui R2,1
	sw a,R2
	lui R6,2
	sw b,R6
	lw R10,a
	lw R11,b
	beq R10,R11,.L5
.L1:
	lw R15,b
	lui R16,19
	add R17,R16,R15
	sw b,R17
.L2:
	lw R22,b
	lui R23,15
	bge R23,R22,.L4
.L3:
	lw R27,b
	lui R28,2
	sub R29,R28,R27
	sw b,R29
	jal R0,.L2
.L4:
	lui R36,0
	sw c,R36
	jal R0,.L8
.L5:
	lui R42,1
	sw a,R42
.L6:
	lw R47,a
	lui R48,4
	bge R47,R48,.L7
	lw R51,b
	lw R52,a
	add R53,R52,R51
	sw b,R53
	lw R58,a
	lui R59,1
	add R60,R59,R58
	sw a,R60
	jal R0,.L6
.L7:
	lui R66,0
	sw c,R66
.L8:
	lui R71,1
	sw c,R71
