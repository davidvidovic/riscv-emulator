0000:	.foo:
0004:		addi x2,x2,-8
0008:		sd x1,0(x2)
000c:		sd x8,-8(x2)
0010:		addi x8,x2,8
0014:		addi x5,x10,0
0018:		addi x6,x0,1
001c:		addi x5,x5,1
0020:		addi x15,x0,0
0024:		jal x0,.L1
0028:	.L1:
002c:		ld x1,0(x2)
0030:		ld x8,-8(x2)
0034:		addi x2,x2,8
0038:		jalr x0,0(x1)
003c:	.main:
0040:		addi x2,x2,-64
0044:		sd x1,56(x2)
0048:		sd x8,48(x2)
004c:		addi x8,x2,64
0050:		addi x5,x0,1
0054:		lui x6,1
0058:		addi x6,x0,1
005c:		add x6,x6,x5
0060:		addi x7,x0,19
0064:		addi x7,x5,19
0068:		sub x7,x7,x6
006c:		addi x28,x0,0
0070:	.L2:
0074:		addi x29,x0,10
0078:		bge x29,x28,.L4
007c:		add x29,x7,x28
0084:		slli x28,x28,2
0088:		addi x30,x8,-64
008c:		add x31,x28,x30
0090:		srli x28,x28,2
0098:		sw x29,0(x31)
009c:	.L3:
00a0:		jal x0,.L2
00a4:	.L4:
00a8:		addi x10,x5,0
00ac:		sw x5,-12(x8)
00b0:		sw x5,-12(x8)
00b4:		sw x5,-12(x8)
00b8:		sw x6,-16(x8)
00bc:		sw x6,-16(x8)
00c0:		sw x7,-8(x8)
00c4:		sw x28,-4(x8)
00c8:		sw x28,-4(x8)
00cc:		jal x1,.foo
00d0:		addi x5,x10,0
00d4:		addi x6,x0,19
00d8:		bge x6,x5,.L6
00dc:	.L5:
00e0:		addi x6,x0,999
00e4:		jal x0,.L7
00e8:	.L6:
00ec:		addi x7,x0,0
00f0:	.L7:
00f4:		addi x28,x0,0
00f8:		addi x15,x0,0
00fc:		jal x0,.L8
0100:	.L8:
0104:		ld x1,56(x2)
0108:		ld x8,48(x2)
010c:		addi x2,x2,64
0110:		jalr x0,0(x1)
