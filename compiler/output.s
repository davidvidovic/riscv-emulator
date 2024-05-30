.main:
	lui R0,0
	sw a,R0
	lui R4,3
	sw b,R4
	lw R12,a
	bne R12,R0,.L1
	lw R8,c
	beq R8,R0,.L2
	lw R9,b
	beq R9,R0,.L2
.L1:
	lui R16,1
	sw c,R16
	jal R0,.L6
.L2:
	lw R23,a
	lui R24,0
	bne R24,R23,.L4
.L3:
	lui R28,19
	sw b,R28
	jal R0,.L5
.L4:
	lui R35,90
	sw b,R35
.L5:
	lui R41,2
	sw c,R41
.L6:
	lui R46,1
	sw a,R46
