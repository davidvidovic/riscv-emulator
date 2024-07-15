0000:	.main:
0004:		addi x2,x2,-48
0008:		sd x1,40(x2)
000c:		sd x8,32(x2)
0010:		addi x8,x2,48
0014:		addi x5,x0,1
0018:		addi x6,x0,2
001c:		addi x7,x0,3
0020:		addi x28,x0,4
0024:		addi x29,x0,5
0028:		addi x30,x0,6
002c:		addi x31,x0,7
0030:		sw x6,-40(x8)
0034:		addi x6,x0,8
0038:		sw x6,-16(x8)
003c:		addi x6,x0,9
0040:		sw x6,-12(x8)
0044:		addi x6,x0,10
0048:		sw x6,-8(x8)
004c:		addi x6,x0,11
0050:		sw x6,-4(x8)
0054:		addi x6,x0,88
0058:		addi x30,x0,777
005c:	.L1:
0060:		addi x5,x0,19
0064:		sw x6,-12(x8)
0068:		lw x6,-4(x8)
006c:		bge x6,x5,.L3
0070:	.L2:
0074:		lw x5,-8(x8)
0078:		sw x6,-4(x8)
007c:		addi x6,x5,1
0080:		jal x0,.L1
0084:		addi x15,x0,0
0088:		jal x0,.L3
008c:	.L3:
0090:		sw x5,-8(x8)
0094:		sw x6,-4(x8)
0098:		sw x7,-36(x8)
009c:		sw x28,-32(x8)
00a0:		sw x29,-44(x8)
00a4:		sw x30,-24(x8)
00a8:		sw x31,-20(x8)
00ac:		ld x1,40(x2)
00b0:		ld x8,32(x2)
00b4:		addi x2,x2,48
00b8:		jalr x0,0(x1)
