0000:	.main:
0004:		addi x2,x2,-64
0008:		sd x1,56(x2)
000c:		sd x8,48(x2)
0010:		addi x8,x2,64
0014:		addi x5,x0,2
0018:		addi x6,x0,0
001c:	.L1:
0020:		addi x7,x0,4
0024:		bge x7,x6,.L3
0028:		add x7,x5,x6
002c:		addi x28,x7,9
0030:		addi x29,x0,1
0034:		addi x29,x6,1
0038:		slli x29,x29,2
003c:		addi x30,x8,-64
0040:		add x31,x29,x30
0044:		srli x29,x29,2
004c:		sw x28,0(x31)
0050:	.L2:
0054:		addi x7,x0,1
0058:		addi x6,x6,1
005c:		jal x0,.L1
0060:	.L3:
0064:		addi x7,x0,9
0068:		addi x15,x0,0
006c:		jal x0,.L4
0070:	.L4:
0074:		ld x1,56(x2)
0078:		ld x8,48(x2)
007c:		addi x2,x2,64
0080:		jalr x0,0(x1)
