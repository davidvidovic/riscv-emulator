0000:	.main:
0004:		addi x2,x2,-16
0008:		sd x1,8(x2)
000c:		sd x8,0(x2)
0010:		addi x8,x2,16
0014:		addi x5,x0,3
0018:		addi x6,x0,0
001c:		addi x5,x0,0
0020:	.L1:
0024:		addi x7,x0,10
0028:		bge x7,x5,.L6
002c:		addi x6,x5,2
0030:		addi x7,x0,5
0034:		bge x7,x6,.L4
0038:	.L2:
003c:		jal x0,.L5
0040:	.L3:
0044:	.L4:
0048:		add x6,x6,x6
004c:	.L5:
0050:		addi x5,x5,1
0054:		jal x0,.L1
0058:	.L6:
005c:		lui x6,2
0060:		addi x6,x0,1808
0064:	.L7:
0068:		addi x7,x0,1
006c:		bne x7,x5,.L8
0070:		addi x6,x0,1111
0074:		jal x0,.L11
0078:	.L8:
007c:		addi x7,x0,2
0080:		bne x7,x5,.L9
0084:		addi x6,x0,3333
0088:		jal x0,.L11
008c:	.L9:
0090:		addi x7,x0,3
0094:		bne x7,x5,.L10
0098:		lui x6,1
009c:		addi x6,x0,348
00a0:		jal x0,.L11
00a4:	.L10:
00a8:		addi x6,x0,2222
00ac:		jal x0,.L11
00b0:	.L11:
00b4:		addi x5,x0,2
00b8:		addi x10,x0,0
00bc:		ld x1,8(x2)
00c0:		ld x8,0(x2)
00c4:		addi x2,x2,16
00c8:		jalr x0,0(x1)
