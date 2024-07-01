0000:	.main:
0004:		addi x2,x2,-24
0008:		sd x1,16(x2)
000c:		sd x8,8(x2)
0010:		addi x8,x2,24
0014:		lw x5,-4(x8)
0018:		lw x6,-12(x8)
001c:		add x7,x5,x6
0020:	.L1:
0024:		ld x1,16(x2)
0028:		ld x8,8(x2)
002c:		addi x2,x2,24
0030:		jalr x0,0(x1)
