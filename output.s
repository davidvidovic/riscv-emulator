0000:	.foo:
0004:		addi x2,x2,-16
0008:		sd x1,8(x2)
000c:		sd x8,0(x2)
0010:		addi x8,x2,16
0014:		addi x5,x0,19
0018:		addi x15,x0,0
001c:		jal x0,.L1
0020:	.L1:
0024:		ld x1,8(x2)
0028:		ld x8,0(x2)
002c:		addi x2,x2,16
0030:		jalr x0,0(x1)
0034:	.foo1:
0038:		addi x2,x2,-8
003c:		sd x1,0(x2)
0040:		sd x8,-8(x2)
0044:		addi x8,x2,8
0048:		addi x5,x0,1919
004c:		addi x15,x0,0
0050:		jal x0,.L2
0054:	.L2:
0058:		ld x1,0(x2)
005c:		ld x8,-8(x2)
0060:		addi x2,x2,8
0064:		jalr x0,0(x1)
0068:	.main:
006c:		addi x2,x2,-64
0070:		sd x1,56(x2)
0074:		sd x8,48(x2)
0078:		addi x8,x2,64
007c:		addi x6,x0,2
0080:		addi x7,x0,0
0084:	.L3:
0088:		addi x28,x0,4
008c:		bge x28,x7,.L5
0090:		add x28,x6,x7
0094:		addi x29,x28,9
0098:		addi x30,x0,1
009c:		addi x30,x7,1
00a0:		slli x30,x30,2
00a4:		addi x31,x8,-64
00a8:		add x6,x30,x31
00ac:		srli x30,x30,2
00b4:		sw x29,0(x6)
00b8:	.L4:
00bc:		addi x6,x0,1
00c0:		addi x7,x7,1
00c4:		jal x0,.L3
00c8:	.L5:
00cc:		addi x5,x0,9
00d0:		addi x15,x0,0
00d4:		jal x0,.L6
00d8:	.L6:
00dc:		ld x1,56(x2)
00e0:		ld x8,48(x2)
00e4:		addi x2,x2,64
00e8:		jalr x0,0(x1)
