0000:	.foo:
0004:		addi x2,x2,-16
0008:		sd x1,8(x2)
000c:		sd x8,0(x2)
0010:		addi x8,x2,16
0014:		addi x5,x10,0
0018:		addi x6,x0,1
001c:		add x7,x5,x6
0020:		addi x15,x7,0
0024:		jal x0,.L1
0028:	.L1:
002c:		sw x5,-4(x8)
0030:		sw x6,-8(x8)
0034:		ld x1,8(x2)
0038:		ld x8,0(x2)
003c:		addi x2,x2,16
0040:		jalr x0,0(x1)
0044:	.main:
0048:		addi x2,x2,-48
004c:		sd x1,40(x2)
0050:		sd x8,32(x2)
0054:		addi x8,x2,48
0058:		addi x5,x0,1
005c:		addi x6,x0,2
0060:		addi x7,x0,3
0064:		addi x28,x0,4
0068:		addi x29,x0,5
006c:		addi x30,x0,6
0070:		addi x31,x0,7
0074:		sw x6,-44(x8)
0078:		addi x6,x0,8
007c:		sw x6,-20(x8)
0080:		addi x6,x0,9
0084:		sw x6,-16(x8)
0088:		addi x6,x0,10
008c:		sw x6,-12(x8)
0090:		addi x6,x0,11
0094:		sw x6,-8(x8)
0098:		addi x6,x0,0
009c:	.L2:
00a0:		addi x5,x0,10
00a4:		blt x6,x5,.L4
00a8:		addi x10,x6,0
00ac:		sw x6,-4(x8)
00b0:		sw x7,-40(x8)
00b4:		sw x28,-36(x8)
00b8:		sw x29,-48(x8)
00bc:		sw x30,-28(x8)
00c0:		sw x31,-24(x8)
00c4:		jal x1,.foo
00c8:		lw x5,-4(x8)
00cc:		add x6,x10,x0
00d0:		lw x7,-12(x8)
00d4:		add x7,x7,x6
00d8:	.L3:
00dc:		addi x5,x5,1
00e0:		jal x0,.L2
00e4:	.L4:
00e8:		addi x6,x0,88
00ec:		addi x28,x0,777
00f0:	.L5:
00f4:		addi x29,x0,0
00f8:		bne x28,x29,.L6
00fc:		addi x29,x0,0
0100:		jal x0,.L8
0104:	.L6:
0108:		addi x30,x0,777
010c:		bne x28,x30,.L7
0110:		addi x30,x0,1
0114:		jal x0,.L8
0118:	.L7:
011c:		addi x29,x0,2
0120:	.L8:
0124:		addi x31,x0,11
0128:		bge x29,x31,.L10
012c:	.L9:
0130:		addi x31,x0,444
0134:		jal x0,.L11
0138:	.L10:
013c:		addi x31,x0,555
0140:	.L11:
0144:		sw x6,-16(x8)
0148:		addi x6,x0,11
014c:		addi x15,x0,0
0150:		jal x0,.L12
0154:	.L12:
0158:		sw x5,-4(x8)
015c:		sw x6,-40(x8)
0160:		sw x7,-12(x8)
0164:		sw x28,-28(x8)
0168:		sw x29,-48(x8)
016c:		sw x30,-20(x8)
0170:		sw x31,-36(x8)
0174:		ld x1,40(x2)
0178:		ld x8,32(x2)
017c:		addi x2,x2,48
0180:		jalr x0,0(x1)
