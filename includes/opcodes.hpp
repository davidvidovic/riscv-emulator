#define I_TYPE	0x13
	#define ADDI	0x0
	#define SLLI	0x1
	#define SLTI	0x2
	#define SLTIU	0x3
	#define XORI	0x4
	#define SRI		0x5
		#define SRLI	0x00
		#define SRAI	0x20
	#define ORI		0x6
	#define ANDI	0x7

#define LOAD_TYPE	0x03
	#define LB	0x00
	#define LH	0x01
	#define LW	0x02
	#define	LD	0x04

#define STORE_TYPE	0x23
	#define SB	0x00
	#define SH	0x01
	#define SW	0x02
	#define SD	0x04

#define R_TYPE	0x33
	#define ADD_SUB	0x0
		#define ADD		0x0
		#define SUB		0x20
	#define SLT		0x2
	#define SLTU	0X3
	#define XOR		0x4
	#define OR		0x6
	#define AND		0x7

#define	B_TYPE	0x63
	#define BEQ	0x00
	#define BNE	0x01
	#define BLT	0x04
	#define BGE	0x05
	#define BLTU	0x06
	#define BGEU	0x07

#define	JAL		0x6f
#define	JALR	0x67
