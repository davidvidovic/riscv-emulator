0000:	.main:
0004:		addi x2,x2,-64
0008:		sd x1,56(x2)
000c:		sd x8,48(x2)
0010:		addi x8,x2,64
0014:		addi x5,x0,1
0018:		addi x6,x0,2
001c:		addi x7,x0,5
0020:		addi x7,x5,5
0024:		add x7,x7,x6
0028:		addi x7,x7,11
002c:		addi x28,x0,2
0030:		addi x28,x7,2
0034:		slli x28,x28,2
0038:		addi x29,x8,-64
003c:		add x30,x28,x29
0040:		addi x28,x0,999
0044:		sw x28,0(x30)
0048:		addi x28,x0,1
004c:		slli x28,x28,2
0050:		addi x29,x8,-64
0054:		add x30,x28,x29
0058:		addi x28,x0,888
005c:		sw x28,0(x30)
0060:		addi x28,x0,0
0064:	.L1:
0068:		addi x29,x0,5
006c:		bge x29,x28,.L3
0074:		slli x28,x28,2
0078:		addi x29,x8,-64
007c:		add x30,x28,x29
0080:		addi x29,x0,1
0084:		sw x29,0(x30)
0088:	.L2:
008c:		jal x0,.L1
0090:		addi x15,x0,0
0094:		jal x0,.L3
0098:	.L3:
009c:		ld x1,56(x2)
00a0:		ld x8,48(x2)
00a4:		addi x2,x2,64
00a8:		jalr x0,0(x1)
