.main:
	lui R2,1
	sw a,R2
	lui R6,2
	sw b,R6
	lw R10,a
	addi R12,R10,1
	sw a,R12
	lw R16,a
	lw R17,b
	beq R16,R17,.L5
.L1:
	lw R21,b
	addi R23,R21,19
	sw b,R23
.L2:
	lw R28,b
	lui R29,15
	bge R29,R28,.L4
.L3:
	lw R33,b
	lui R34,2
	sub R35,R34,R33
	sw b,R35
	jal R0,.L2
.L4:
	sw c,R0
	jal R0,.L8
.L5:
	lui R47,1
	sw a,R47
.L6:
	lw R52,a
	lui R53,4
	bge R52,R53,.L7
	lw R56,b
	lw R57,a
	add R58,R57,R56
	sw b,R58
	lw R63,a
	addi R65,R63,1
	sw a,R65
	jal R0,.L6
.L7:
	sw c,R0
.L8:
	lui R75,1
	sw c,R75
