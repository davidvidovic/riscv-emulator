#ifndef instructions
#define instructions

typedef enum IR_instruction_type {
    NONE,
    IR_LABEL, // LABEL
    IR_LOAD, // REG, ADDRESS
    IR_LOAD_IMM, // REG, CONST
    IR_STORE, // REG, ADDRESS
    IR_OP, // REG, REG, REG
    IR_OP_IMM, // REG, REG, CONST
    IR_BRANCH,
    IR_JUMP
} IR_instruction_type;

typedef enum instruction_type {
    HEAD = 0,
    IR_NO_TYPE,
    LABEL,
    LUI,
    AUIPC,
    JAL,
    JALR,
    BEQ,
    BNE,
    BLT,
    BGE,
    BLTU,
    BGEU,
    // Additional BRANCH instructions not supported by ISA
    BGT,
    BLE,
    // End to additional BRANCH instructions
    LB,
    LH,
    LW,
    LBU,
    LHU,
    SB,
    SH,
    SW,
    ADDI,
    SLTI,
    SLTIU,
    XORI,
    ORI,
    ANDI,
    SLLI,
    SRLI,
    SRAI,
    ADD,
    SUB,
    SLL,
    SLT,
    SLTU,
    XOR,
    SRL,
    SRA,
    OR,
    AND,
    FENCE,
    FENCE_I,
    // RV64I extension
    LWU,
    LD,
    SD,
    ADDIW,
    SLLIW,
    SRLIW,
    SRAIW,
    ADDW,
    SUBW,
    SLLW,
    SRLW,
    SRAW
} instruction_type;


typedef enum IR_register {
    zero,
    ra,
    sp,
    gp,
    tp,
    t0, t1, t2,
    s0, // fp - frame pointer
    s1,
    a0, a1, a2, a3, a4, a5, a6, a7,
    s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,
    t3, t4, t5, t6
} IR_register;

#endif