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
0060:		addi x6,x0,1919
0064:		addi x5,x5,1919
0068:		addi x15,x5,0
006c:		jal x0,.L2
0070:	.L2:
0074:		ld x1,0(x2)
0078:		ld x8,-8(x2)
007c:		addi x2,x2,8
0080:		jalr x0,0(x1)
0084:	.main:
0088:		addi x2,x2,-64
008c:		sd x1,56(x2)
0090:		sd x8,48(x2)
0094:		addi x8,x2,64
0098:		addi x5,x0,2
009c:		addi x6,x0,99
00a0:		addi x10,x6,0
00a4:		addi x11,x5,0
00a8:		addi x7,x0,19
00ac:		addi x12,x7,0
00b0:		call foo
00b4:		lw x5,-8(x8)
00b8:		addi x13,x5,0
00bc:		call foo1
00c0:		addi x5,x0,9
00c4:		addi x15,x0,0
00c8:		jal x0,.L3
00cc:	.L3:
00d0:		ld x1,56(x2)
00d4:		ld x8,48(x2)
00d8:		addi x2,x2,64
00dc:		jalr x0,0(x1)
