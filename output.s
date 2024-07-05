0000:	.main:
0004:		addi x2,x2,-16
0008:		sd x1,8(x2)
000c:		sd x8,0(x2)
0010:		addi x8,x2,16
0014:		addi x5,x0,1
0018:		addi x6,x0,1
001c:		addi x7,x0,16
0020:		addi x28,x6,1
0024:		sraw x7,x7,x28
0028:		addi x15,x0,0
002c:		jal x0,.L1
0030:	.L1:
0034:		ld x1,8(x2)
0038:		ld x8,0(x2)
003c:		addi x2,x2,16
0040:		jalr x0,0(x1)
