.main:
	lui R0,0
	sw a,R0
	lui R4,3
	sw b,R4
	lw R8,b
	lui R9,3
	bge R9,R8,.L2
	lui R12,2
	sw a,R12
	lw R16,b
	lui R17,19
	blt R17,R16,.L1
	lui R20,1
	sw a,R20
.L1:
	lui R26,44
	lui R27,1
	add R28,R27,R26
	sw c,R28
.L2:
	lui R34,19
	sw b,R34
	lw R38,b
	lui R39,19
	blt R39,R38,.L4
	lw R42,a
	lui R43,3
	bne R43,R42,.L3
	lui R46,1
	sw a,R46
.L3:
	lui R52,55
	lui R53,11
	sub R54,R53,R52
	sw c,R54
.L4:
	lui R60,0
	sw a,R60
