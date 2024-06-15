#include "assembler.h"

#define I_TYPE_OPCODE_OP    "0010011"
#define I_TYPE_OPCODE_LOAD  "0000011"
#define R_TYPE_OPCODE       "0110011"
#define S_TYPE_OPCODE       "0100011"
#define B_TYPE_OPCODE       "1100011"
#define J_TYPE_OPCODE       "1101111"
#define JALR_TYPE_OPCODE    "1100111"

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
    ((byte) & 0x80 ? '1' : '0'), \
    ((byte) & 0x40 ? '1' : '0'), \
    ((byte) & 0x20 ? '1' : '0'), \
    ((byte) & 0x10 ? '1' : '0'), \
    ((byte) & 0x08 ? '1' : '0'), \
    ((byte) & 0x04 ? '1' : '0'), \
    ((byte) & 0x02 ? '1' : '0'), \
    ((byte) & 0x01 ? '1' : '0') 

#define REG_TO_BINARY_PATTERN "%c%c%c%c%c"
#define REG_TO_BINARY(reg)  \
    ((reg) & 0x10 ? '1' : '0'), \
    ((reg) & 0x08 ? '1' : '0'), \
    ((reg) & 0x04 ? '1' : '0'), \
    ((reg) & 0x02 ? '1' : '0'), \
    ((reg) & 0x01 ? '1' : '0') 

#define IMM_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c"
#define IMM_TO_BINARY(imm)  \
    ((imm) & 0x800 ? '1' : '0'), \
    ((imm) & 0x400 ? '1' : '0'), \
    ((imm) & 0x200 ? '1' : '0'), \
    ((imm) & 0x100 ? '1' : '0'), \
    ((imm) & 0x080 ? '1' : '0'), \
    ((imm) & 0x040 ? '1' : '0'), \
    ((imm) & 0x020 ? '1' : '0'), \
    ((imm) & 0x010 ? '1' : '0'), \
    ((imm) & 0x008 ? '1' : '0'), \
    ((imm) & 0x004 ? '1' : '0'), \
    ((imm) & 0x002 ? '1' : '0'), \
    ((imm) & 0x001 ? '1' : '0') 

#define STORE_UPPER_IMM_TO_BINARY_PATTERN "%c%c%c%c%c%c%c"
#define STORE_UPPER_IMM_TO_BINARY(imm)  \
    ((imm) & 0x800 ? '1' : '0'), \
    ((imm) & 0x400 ? '1' : '0'), \
    ((imm) & 0x200 ? '1' : '0'), \
    ((imm) & 0x100 ? '1' : '0'), \
    ((imm) & 0x080 ? '1' : '0'), \
    ((imm) & 0x040 ? '1' : '0'), \
    ((imm) & 0x020 ? '1' : '0')

#define STORE_LOWER_IMM_TO_BINARY_PATTERN "%c%c%c%c%c"
#define STORE_LOWER_IMM_TO_BINARY(imm)  \
    ((imm) & 0x010 ? '1' : '0'), \
    ((imm) & 0x008 ? '1' : '0'), \
    ((imm) & 0x004 ? '1' : '0'), \
    ((imm) & 0x002 ? '1' : '0'), \
    ((imm) & 0x001 ? '1' : '0') 

#define BRANCH_UPPER_IMM_TO_BINARY_PATTERN "%c%c%c%c%c%c%c"
#define BRANCH_UPPER_IMM_TO_BINARY(imm)  \
    ((imm) & 0x1000 ? '1' : '0'), \
    ((imm) & 0x0400 ? '1' : '0'), \
    ((imm) & 0x0200 ? '1' : '0'), \
    ((imm) & 0x0100 ? '1' : '0'), \
    ((imm) & 0x0080 ? '1' : '0'), \
    ((imm) & 0x0040 ? '1' : '0'), \
    ((imm) & 0x0020 ? '1' : '0')

#define BRANCH_LOWER_IMM_TO_BINARY_PATTERN "%c%c%c%c%c"
#define BRANCH_LOWER_IMM_TO_BINARY(imm)  \
    ((imm) & 0x010 ? '1' : '0'), \
    ((imm) & 0x008 ? '1' : '0'), \
    ((imm) & 0x004 ? '1' : '0'), \
    ((imm) & 0x002 ? '1' : '0'), \
    ((imm) & 0x800 ? '1' : '0') 

#define JUMP_IMM_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c"
#define JUMP_IMM_TO_BINARY(imm)  \
    ((imm) & 0x100000 ? '1' : '0'), \
    ((imm) & 0x000400 ? '1' : '0'), \
    ((imm) & 0x000200 ? '1' : '0'), \
    ((imm) & 0x000100 ? '1' : '0'), \
    ((imm) & 0x000080 ? '1' : '0'), \
    ((imm) & 0x000040 ? '1' : '0'), \
    ((imm) & 0x000020 ? '1' : '0'), \
    ((imm) & 0x000010 ? '1' : '0'), \
    ((imm) & 0x000008 ? '1' : '0'), \
    ((imm) & 0x000004 ? '1' : '0'), \
    ((imm) & 0x000002 ? '1' : '0'), \
    ((imm) & 0x000800 ? '1' : '0'), \
    ((imm) & 0x080000 ? '1' : '0'), \
    ((imm) & 0x040000 ? '1' : '0'), \
    ((imm) & 0x020000 ? '1' : '0'), \
    ((imm) & 0x010000 ? '1' : '0'), \
    ((imm) & 0x008000 ? '1' : '0'), \
    ((imm) & 0x004000 ? '1' : '0'), \
    ((imm) & 0x002000 ? '1' : '0'), \
    ((imm) & 0x001000 ? '1' : '0')



void assemble_binary_output(IR_node *IR_head, IR_node *IR_tail)
{
    FILE *bin_file = fopen("../output.bin", "w");
    
    IR_node *temp_head = IR_head;
    while(temp_head != IR_tail)
    {
        temp_head = temp_head->prev;   

        switch(temp_head->ir_type)
        {
            case IR_OP:
                /* R-type */
                fprintf(bin_file, "%s\n", assemble_R_type(temp_head));
            break;

            case IR_OP_IMM:
            case IR_LOAD:
                /* I-type */
                fprintf(bin_file, "%s\n", assemble_I_type(temp_head));
            break;
            
            case IR_STORE:
                /* S-type */
                fprintf(bin_file, "%s\n", assemble_S_type(temp_head));
            break;

            case IR_BRANCH:
                /* B-type */
                fprintf(bin_file, "%s\n", assemble_B_type(temp_head, IR_head, IR_tail));
            break;

            case IR_JUMP:
                /* J-type */
                if(temp_head->instr_type == JAL)
                    fprintf(bin_file, "%s\n", assemble_J_type(temp_head, IR_head, IR_tail));
                else
                    fprintf(bin_file, "%s\n", assemble_I_type(temp_head));
            break;
        }

        
    }


    fclose(bin_file);
}


char* assemble_I_type(IR_node *node)
{
    char* instruction = (char*)malloc(32*sizeof(char));
    char* temp = (char*)malloc(7*sizeof(char));

    // imm field
    switch(node->ir_type)
    {
        case IR_LOAD:
        case IR_JUMP:
            sprintf(instruction, IMM_TO_BINARY_PATTERN, IMM_TO_BINARY(node->sf_offset));
        break;

        case IR_LOAD_IMM:
            sprintf(instruction, IMM_TO_BINARY_PATTERN, IMM_TO_BINARY(node->rs1.int_constant));
        break;

        case IR_OP_IMM:
            sprintf(instruction, IMM_TO_BINARY_PATTERN, IMM_TO_BINARY(node->rs2.int_constant));
        break;
    }

    // rs1 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rs1.reg));
    strcat(instruction, temp);

    // funct3 field
    switch(node->instr_type)
    {
        case ADDI:
        case LB:
        case JALR:
            sprintf(temp, "000");
        break;

        case LH:
            sprintf(temp, "001");
        break;

        case SLTI:
        case LW:
            sprintf(temp, "010");
        break;

        case SLTIU:
        case LD: // check this one, it's different in Ripes
            sprintf(temp, "011");
        break;

        case XORI:
        case LBU:
            sprintf(temp, "100");
        break;

        case LHU:
            sprintf(temp, "101");
        break;

        case ORI:
            sprintf(temp, "110");
        break;

        case ANDI:
            sprintf(temp, "111");
        break;
    }
    strcat(instruction, temp);

    // rd field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rd.reg));
    strcat(instruction, temp);

    // opcode field
    switch(node->ir_type)
    {
        case IR_LOAD:
            sprintf(temp, I_TYPE_OPCODE_LOAD);
        break;

        case IR_OP_IMM:
            sprintf(temp, I_TYPE_OPCODE_OP);
        break;

        case IR_JUMP:
            sprintf(temp, JALR_TYPE_OPCODE);
        break;
    }
    strcat(instruction, temp);
    
    return instruction;
}

char* assemble_R_type(IR_node *node)
{
    char* instruction = (char*)malloc(32*sizeof(char));
    char* temp = (char*)malloc(7*sizeof(char));

    // funct7 field
    switch(node->instr_type)
    {
        case SUB:
        case SRA:
        case SRAI:
            sprintf(instruction, "0100000");
        break;

        default:
            sprintf(instruction, "0000000");
        break;
    }   

    // rs2 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rs2.reg));
    strcat(instruction, temp);

    // rs1 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rs1.reg));
    strcat(instruction, temp);

    // funct3 field
    switch(node->instr_type)
    {
        case ADD:
        case SUB:
            sprintf(temp, "000");
        break;

        case SLL:
        case SLLI:
            sprintf(temp, "001");
        break;

        case SLT:
            sprintf(temp, "010");
        break;

        case SLTU:
            sprintf(temp, "011");
        break;

        case XOR:
            sprintf(temp, "100");
        break;

        case SRL:
        case SRA:
        case SRLI:
        case SRAI:
            sprintf(temp, "101");
        break;

        case OR:
            sprintf(temp, "110");
        break;

        case AND:
            sprintf(temp, "111");
        break;
    }
    strcat(instruction, temp);

    // rd field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rd.reg));
    strcat(instruction, temp);

    // opcode field
    sprintf(temp, R_TYPE_OPCODE);
    strcat(instruction, temp);
    
    return instruction;
}

char* assemble_S_type(IR_node *node)
{
    char* instruction = (char*)malloc(32*sizeof(char));
    char* temp = (char*)malloc(7*sizeof(char));

    // imm field
    sprintf(instruction, STORE_UPPER_IMM_TO_BINARY_PATTERN, STORE_UPPER_IMM_TO_BINARY(node->sf_offset));

    // rs2 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rs1.reg));
    strcat(instruction, temp);

    // rs1 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rd.reg));
    strcat(instruction, temp);

    // funct3 field
    switch(node->instr_type)
    {
        case SB:
            sprintf(temp, "000");
        break;

        case SH:
            sprintf(temp, "001");
        break;

        case SW:
            sprintf(temp, "010");
        break;

        case SD:
            sprintf(temp, "011");
        break;
    }
    strcat(instruction, temp);

    // imm field
    sprintf(temp, STORE_LOWER_IMM_TO_BINARY_PATTERN, STORE_LOWER_IMM_TO_BINARY(node->sf_offset));
    strcat(instruction, temp);

    // opcode field
    sprintf(temp, S_TYPE_OPCODE);
    strcat(instruction, temp);
    
    return instruction;
}

char* assemble_B_type(IR_node *node, IR_node *IR_head, IR_node *IR_tail)
{
    char* instruction = (char*)malloc(32*sizeof(char));
    char* temp = (char*)malloc(7*sizeof(char));

    int imm = get_label_address(node->rd.label, IR_head, IR_tail);
    imm = imm - node->ir_address;

    // imm field
    sprintf(instruction, BRANCH_UPPER_IMM_TO_BINARY_PATTERN, BRANCH_UPPER_IMM_TO_BINARY(imm));

    // rs2 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rs2.reg));
    strcat(instruction, temp);

    // rs1 field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rs1.reg));
    strcat(instruction, temp);

    // funct3 field
    switch(node->instr_type)
    {
        case BEQ:
            sprintf(temp, "000");
        break;

        case BNE:
            sprintf(temp, "001");
        break;

        case BLT:
            sprintf(temp, "100");
        break;

        case BGE:
            sprintf(temp, "101");
        break;

        case BLTU:
            sprintf(temp, "110");
        break;

        case BGEU:
            sprintf(temp, "111");
        break;
    }
    strcat(instruction, temp);

    // imm field
    sprintf(temp, BRANCH_LOWER_IMM_TO_BINARY_PATTERN, BRANCH_LOWER_IMM_TO_BINARY(imm));
    strcat(instruction, temp);

    // opcode field
    sprintf(temp, B_TYPE_OPCODE);
    strcat(instruction, temp);
    
    return instruction;
}

char* assemble_J_type(IR_node *node, IR_node *IR_head, IR_node *IR_tail)
{
    char* instruction = (char*)malloc(32*sizeof(char));
    char* temp = (char*)malloc(7*sizeof(char));

    int imm = get_label_address(node->rs1.label, IR_head, IR_tail);

    // imm field
    sprintf(instruction, JUMP_IMM_TO_BINARY_PATTERN, JUMP_IMM_TO_BINARY(imm));

    // rd field
    sprintf(temp, REG_TO_BINARY_PATTERN, REG_TO_BINARY(node->rd.reg));
    strcat(instruction, temp);

    // opcode field
    sprintf(temp, J_TYPE_OPCODE);
    strcat(instruction, temp);
    
    return instruction;
}