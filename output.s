.main:
	addi x2,x2,-48
	sd x1,40(x2)
	sd x8,32(x2)
	addi x8,x2,48
	lui x5,2
	lui x5,1
	sub x6,x5,x5
	add x7,x5,x6
	lui x28,4
	sub x29,x7,x28
	lui x28,5
	lui x7,10
	lui x30,6
	lui x31,7
	sw x6,-40(x8)
	lui x6,8
	lui x6,9
	sw x6,-12(x8)
	lui x6,11
	lw x6,-8(x8)
	add x29,x5,x6
	addi x7,x5,1
	addi x31,x31,99
	sltiu x5,x5,1
	andi x5,x5,255
	bne x5,x0,.L2
.L1:
	xori x31,x30,-1
	jal x0,.L5
.L2:
	lui x7,88
	bge x31,x7,.L4
.L3:
	lui x6,19
.L4:
	lui x29,9
.L5:
	lui x29,19
.L6:
	sw x5,-4(x8)
	lui x5,3
	beq x5,x0,.L8
	lw x5,-40(x8)
	beq x5,x0,.L8
.L7:
	addi x5,x5,1
	jal x0,.L6
.L8:
	lui x5,888
	sw x6,-8(x8)
	lui x6,0
.L9:
	lui x6,10
	sw x7,-36(x8)
	lw x7,-12(x8)
	bge x6,x7,.L10
	lw x6,-4(x8)
	add x6,x6,x7
	addi x7,x7,1
	jal x0,.L9
.L10:
	lui x6,999
	lui x10,0
	ld x1,40(x2)
	ld x8,32(x2)
	addi x2,x2,48
	jalr x0,.ra
