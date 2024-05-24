#ifndef instructions
#define instructions

typedef enum IR_instruction_type {
    NONE,
    IR_PROCEDURE,
    IR_LOAD,
    IR_STORE,
    IR_ADD,
    IR_SUB
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

#endif