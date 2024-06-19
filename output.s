0000:	.main:
0004:		addi x2,x2,-24
0008:		sd x1,16(x2)
000c:		sd x8,8(x2)
0010:		addi x8,x2,24
0014:		lui x5,3
0018:		lui x6,5
001c:		add x7,x5,x6
0020:		addi x28,x5,1
0024:	.L1:
0028:		bne x7,x5,.L3
002c:	.L2:
0030:		add x7,x5,x6
0034:		addi x28,x5,1
0038:		jal x0,.L1
003c:	.L3:
0040:		lui x29,99
0044:		lui x10,0
0048:		ld x1,16(x2)
004c:		ld x8,8(x2)
0050:		addi x2,x2,24
0054:		jalr x0,0(x1)
