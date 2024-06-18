0000:	.main:
0004:		addi x2,x2,-16
0008:		sd x1,8(x2)
000c:		sd x8,0(x2)
0010:		addi x8,x2,16
0014:		lui x5,2
0018:		addi x6,x5,3
001c:	.L1:
0020:		lui x7,6
0024:		bge x7,x6,.L3
0028:	.L2:
002c:		lw x28,-4(x8)
0030:		add x7,x7,x28
0034:		jal x0,.L1
0038:	.L3:
003c:		lui x10,0
0040:		ld x1,8(x2)
0044:		ld x8,0(x2)
0048:		addi x2,x2,16
004c:		jalr x0,0(x1)
