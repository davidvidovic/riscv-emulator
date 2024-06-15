0000:	.main:
0004:		addi x2,x2,-48
0008:		sd x1,40(x2)
000c:		sd x8,32(x2)
0010:		addi x8,x2,48
0014:		lui x5,2
0018:		lui x5,1
001c:		sub x6,x5,x5
0020:		add x7,x5,x6
0024:		lui x28,4
0028:		sub x29,x7,x28
002c:		lui x28,5
0030:		lui x7,10
0034:		lui x30,6
0038:		lui x31,7
003c:		sw x6,-40(x8)
0040:		lui x6,8
0044:		lui x6,9
0048:		sw x6,-12(x8)
004c:		lui x6,11
0050:		lw x6,-8(x8)
0054:		add x29,x5,x6
0058:		addi x7,x5,1
005c:		addi x31,x31,99
0060:		sltiu x5,x5,1
0064:		andi x5,x5,255
0068:		bne x5,x0,.L2
006c:	.L1:
0070:		xori x31,x30,-1
0074:		jal x0,.L5
0078:	.L2:
007c:		lui x7,88
0080:		bge x31,x7,.L4
0084:	.L3:
0088:		lui x6,19
008c:	.L4:
0090:		lui x29,9
0094:	.L5:
0098:		lui x29,19
009c:	.L6:
00a0:		sw x5,-4(x8)
00a4:		lui x5,3
00a8:		beq x5,x0,.L8
00ac:		lw x5,-40(x8)
00b0:		beq x5,x0,.L8
00b4:	.L7:
00b8:		addi x5,x5,1
00bc:		jal x0,.L6
00c0:	.L8:
00c4:		lui x5,888
00c8:		sw x6,-8(x8)
00cc:		lui x6,0
00d0:	.L9:
00d4:		lui x6,10
00d8:		sw x7,-36(x8)
00dc:		lw x7,-12(x8)
00e0:		bge x6,x7,.L10
00e4:		lw x6,-4(x8)
00e8:		add x6,x6,x7
00ec:		addi x7,x7,1
00f0:		jal x0,.L9
00f4:	.L10:
00f8:		lui x6,999
00fc:		lui x10,0
0100:		ld x1,40(x2)
0104:		ld x8,32(x2)
0108:		addi x2,x2,48
010c:		jalr x0,0(x1)
