.main:
	lui R1,1
	sw a,R1
	lui R5,2
	sw b,R5
	lw R9,a
	lw R10,b
	beq R9,R10,.L5
.L1:
	lw R14,b
	lui R15,19
	add R16,R15,R14
	sw b,R16
.L2:
	lw R21,b
	lui R22,15
	bge R22,R21,.L4
.L3:
	lw R26,b
	lui R27,2
	sub R28,R27,R26
	sw b,R28
	jal R0,.L2
.L4:
	lui R35,0
	sw c,R35
	jal R0,.L8
.L5:
	lui R41,1
	sw a,R41
.L6:
	lw R46,a
	lui R47,4
	bge R46,R47,.L8
	lw R50,b
	lw R51,a
	add R52,R51,R50
	sw b,R52
	lw R57,a
	lui R58,1
	add R59,R58,R57
	sw a,R59
	jal R0,.L6
.L7:
.L8:
	lui R66,1
	sw c,R66
