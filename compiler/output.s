.main:
	lui R0,0
	sw a,R0
	lui R4,3
	sw b,R4
	lw R12,a
	bne R12,R0,.L1
	lw R8,c
	beq R8,R0,.L4
	lw R9,b
	beq R9,R0,.L4
.L1:
	lui R16,1
	sw a,R16
	lw R20,b
	beq R20,R0,.L3
	lw R21,a
	beq R21,R0,.L3
.L2:
	lui R26,3
	sw c,R26
.L3:
	lui R32,19
	sw b,R32
.L4:
	lui R38,1
	sw c,R38
