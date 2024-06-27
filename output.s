0000:	.foo:
0004:		addi x2,x2,-24
0008:		sd x1,16(x2)
000c:		sd x8,8(x2)
0010:		addi x8,x2,24
0014:		addi x5,x10,0
0018:		addi x6,x11,0
001c:		addi x7,x12,0
0020:		lw x28,-16(x8)
0024:		add x29,x28,x6
0028:		sub x29,x29,x5
002c:		addi x15,x29,0
0030:		jal x0,.L1
0034:	.L1:
0038:		ld x1,16(x2)
003c:		ld x8,8(x2)
0040:		addi x2,x2,24
0044:		jalr x0,0(x1)
0048:	.foo1:
004c:		addi x2,x2,-8
0050:		sd x1,0(x2)
0054:		sd x8,-8(x2)
0058:		addi x8,x2,8
005c:		addi x5,x10,0
0060:		addi x6,x0,100
0064:		addi x5,x5,100
0068:		addi x6,x0,1000
006c:		bge x6,x5,.L3
0070:	.L2:
0074:		addi x15,x5,0
0078:		jal x0,.L4
007c:	.L3:
0080:		addi x10,x5,0
0084:		sw x5,-4(x8)
0088:		sw x5,-4(x8)
008c:		jal x1,.foo1
0090:	.L4:
0094:		ld x1,0(x2)
0098:		ld x8,-8(x2)
009c:		addi x2,x2,8
00a0:		jalr x0,0(x1)
00a4:	.main:
00a8:		addi x2,x2,-64
00ac:		sd x1,56(x2)
00b0:		sd x8,48(x2)
00b4:		addi x8,x2,64
00b8:		addi x5,x0,2
00bc:		addi x6,x0,99
00c0:		addi x10,x6,0
00c4:		addi x11,x5,0
00c8:		addi x7,x0,19
00cc:		addi x12,x7,0
00d0:		sw x5,-8(x8)
00d4:		sw x6,-12(x8)
00d8:		jal x1,.foo
00dc:		addi x5,x10,0
00e0:		lw x6,-12(x8)
00e4:		addi x10,x6,0
00e8:		sw x5,-4(x8)
00ec:		sw x6,-12(x8)
00f0:		jal x1,.foo1
00f4:		addi x5,x0,199
00f8:		sub x5,x5,x10
00fc:		addi x6,x0,9
0100:		addi x15,x0,0
0104:		jal x0,.L5
0108:	.L5:
010c:		ld x1,56(x2)
0110:		ld x8,48(x2)
0114:		addi x2,x2,64
0118:		jalr x0,0(x1)
