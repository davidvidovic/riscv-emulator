.main:
	lui R1,999
	sw a,R1
	lui R5,48
	sw g,R5
	lui R9,1
	sw a,R9
	lw R13,a
	lw R14,b
	beq R13,R14,.L2
.L1:
	lui R18,65
	sw g,R18
	jal R0,.L5
.L2:
	lui R25,49
	sw g,R25
	lw R29,b
	beq R29,R0,.L4
	lw R30,a
	beq R30,R0,.L4
.L3:
	lui R35,97
	sw g,R35
.L4:
	lui R41,19
	lui R42,999
	sw a,R42
	sw c,R0
.L5:
	lui R51,19
	sw a,R51
