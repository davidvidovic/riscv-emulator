0000:	.main:
0004:		addi x2,x2,-24
0008:		sd x1,16(x2)
000c:		sd x8,8(x2)
0010:		addi x8,x2,24
0014:		lui x5,2
0018:		addi x5,x0,1807
001c:		addi x6,x0,4095
0020:		bge x6,x5,.L2
0024:	.L1:
0028:		addi x7,x0,10
002c:		jal x0,.L3
0030:	.L2:
0034:		addi x7,x0,20
0038:		addi x15,x0,0
003c:		jal x0,.L3
0040:	.L3:
0044:		ld x1,16(x2)
0048:		ld x8,8(x2)
004c:		addi x2,x2,24
0050:		jalr x0,0(x1)
