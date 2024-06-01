.main:
	lui R1,0
	sw b,R1
	lui R5,4
	sw a,R5
.L1:
	lw R10,a
	bne R10,R0,.L2
	lw R11,b
	beq R11,R0,.L6
.L2:
	lui R16,2
	sw a,R16
	lw R20,a
	lui R21,2
	bne R21,R20,.L4
.L3:
	lui R25,1
	sw a,R25
	jal R0,.L5
.L4:
	lui R32,0
	sw b,R32
.L5:
	lui R38,19
	sw a,R38
	jal R0,.L1
.L6:
	lui R44,19
	sw b,R44
