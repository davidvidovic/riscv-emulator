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
0064:		bne x6,x5,.L8
0068:	.L7:
006c:		addi x15,x0,555
0070:		jal x0,.L14
0074:	.L8:
0078:		addi x5,x0,2
007c:	.L9:
0080:		addi x7,x0,1
0084:		bne x7,x5,.L10
0088:		addi x6,x0,1111
008c:		jal x0,.L13
0090:	.L10:
0094:		addi x7,x0,2
0098:		bne x7,x5,.L11
009c:		addi x6,x0,3333
00a0:		jal x0,.L13
00a4:	.L11:
00a8:		addi x7,x0,3
00ac:		bne x7,x5,.L12
00b0:		lui x6,1
00b4:		addi x6,x0,348
00b8:		jal x0,.L13
00bc:	.L12:
00c0:		addi x6,x0,2222
00c4:		jal x0,.L13
00c8:	.L13:
00cc:		addi x5,x0,2
00d0:		addi x15,x0,0
00d4:		jal x0,.L14
00d8:	.L14:
00dc:		ld x1,8(x2)
00e0:		ld x8,0(x2)
00e4:		addi x2,x2,16
00e8:		jalr x0,0(x1)
