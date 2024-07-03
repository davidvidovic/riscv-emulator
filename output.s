0000:	.foo:
0004:		addi x2,x2,-8
0008:		sd x1,0(x2)
000c:		sd x8,-8(x2)
0010:		addi x8,x2,8
0014:		addi x5,x10,0
0018:		addi x5,x5,1
001c:		addi x15,x0,0
0020:		jal x0,.L1
0024:	.L1:
0028:		ld x1,0(x2)
002c:		ld x8,-8(x2)
0030:		addi x2,x2,8
0034:		jalr x0,0(x1)
0038:	.main:
003c:		addi x2,x2,-64
0040:		sd x1,56(x2)
0044:		sd x8,48(x2)
0048:		addi x8,x2,64
004c:		addi x5,x0,1
0050:		lui x6,1
0054:		addi x6,x0,1
0058:		add x6,x6,x5
005c:		addi x7,x5,19
0060:		sub x7,x7,x6
0064:		addi x28,x0,0
0068:	.L2:
006c:		addi x29,x0,10
0070:		bge x28,x29,.L4
0074:		add x29,x7,x28
007c:		slli x28,x28,2
0080:		addi x30,x8,-64
0084:		add x31,x28,x30
0088:		srli x28,x28,2
008c:		sw x29,0(x31)
0090:		addi x6,x6,888
0094:	.L3:
0098:		addi x28,x28,1
009c:		jal x0,.L2
00a0:	.L4:
00a4:		addi x10,x5,0
00a8:		sw x5,-12(x8)
00ac:		sw x6,-16(x8)
00b0:		sw x6,-16(x8)
00b4:		sw x6,-16(x8)
00b8:		sw x7,-8(x8)
00bc:		sw x7,-8(x8)
00c0:		sw x28,-4(x8)
00c4:		sw x28,-4(x8)
00c8:		jal x1,.foo
00cc:		addi x5,x10,0
00d0:		addi x6,x0,19
00d4:		bge x5,x6,.L6
00d8:	.L5:
00dc:		addi x6,x0,999
00e0:		jal x0,.L7
00e4:	.L6:
00e8:		addi x7,x0,0
00ec:	.L7:
00f0:		addi x28,x0,0
00f4:		addi x15,x0,0
00f8:		jal x0,.L8
00fc:	.L8:
0100:		ld x1,56(x2)
0104:		ld x8,48(x2)
0108:		addi x2,x2,64
010c:		jalr x0,0(x1)
