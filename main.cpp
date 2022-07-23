#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;
using s32 = signed int;

struct MEM
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte Data[MAX_MEM];

    void initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            Data[i] = 0x0;
        }
    }

    // Read Byte
    Byte operator[](u32 address) const
    {
        return Data[address];
    }

    // Write Byte
    Byte &operator[](u32 address)
    {
        return Data[address];
    }
};

struct CPU
{
    Word PC;            // Program Counter
    Word SP;            // Stack Pointer
    Byte A, C, D;       // 8bit Registers
    Word F;             // 16bit Registers
    Byte B, S1, S2, S3; // Private Registers

    // Flags
    Byte ZF : 1; // Zero
    Byte OF : 1; // Overflow
    Byte SF : 1; // Sign
    Byte CF : 1; // Carry

    void Reset(MEM &Memory)
    {
        PC = 0x0;
        A = C = D = ZF = OF = SF = CF = 0x0;
        SP = 0xFF;
        Memory.initialize();
    }

    Byte FetchByte(MEM &memory)
    {
        Byte Data = memory[PC];
        PC++;
        return Data;
    }

    Word FetchWord(MEM &memory)
    {
        Word Data = memory[PC];
        PC++;

        Data |= (memory[PC] << 8);
        PC++;

        return Data;
    }

    Byte ReadByte(MEM &Memory, Word Address)
    {
        Byte Data = Memory[Address];
        return Data;
    }

    void WriteByte(Byte value, Word Address, MEM &Memory)
    {
        Memory[Address] = value;
    }

    // Write two bytes
    void WriteWord(Word Value, Word Address, MEM &Memory)
    {
        Memory[Address] = Value & 0xFF;
        Memory[Address + 1] = (Value >> 8);
    }

    void Push(Byte value, MEM &Memory)
    {
        WriteWord(value, SP, Memory);
        SP--;
    }

    Byte Pop(MEM &Memory)
    {

        SP++;
        Byte value = ReadByte(Memory, SP);
        return value;
    }

    void Set_A(Byte value)
    {
        A = value;
        ZF = (A == 0);
        SF = (A & 0b10000000) > 0;
    }

    void Set_PC(Word value)
    {
        PC = value;
    }

    static constexpr Byte
        // MOV A
        MOV_A_IMM8 = 0x0,
        MOV_A_C = 0x1,
        MOV_A_D = 0x2,
        MOV_A_MEM_CD = 0x3,
        MOV_A_MEM_F = 0x4,
        MOV_A_MEM_IMM16 = 0x5,
        // MOV C
        MOV_C_IMM8 = 0x6,
        MOV_C_A = 0x7,
        MOV_C_D = 0x8,
        MOV_C_MEM_F = 0x9,
        MOV_C_MEM_IMM16 = 0xa,
        // MOV D
        MOV_D_IMM8 = 0xb,
        MOV_D_A = 0xc,
        MOV_D_C = 0xd,
        MOV_D_MEM_F = 0xe,
        MOV_D_MEM_IMM16 = 0xf,
        // MOV F
        MOV_F_IMM8 = 0x10,
        MOV_F_IMM16 = 0x11,
        MOV_F_AC = 0x12,
        MOV_F_CD = 0x13,
        MOV_F_MEM_AC = 0x14,
        MOV_F_MEM_CD = 0x15,
        MOV_F_MEM_IMM16 = 0x16,
        // MOV Memory
        MOV_MEM_A = 0x17,
        MOV_MEM_C = 0x18,
        MOV_MEM_D = 0x19,
        MOV_MEM_F = 0x1a,
        MOV_MEM_IMM8 = 0x1b,
        MOV_MEM_IMM16 = 0x1c,
        // MOV Flags
        MOV_FLAGS_A = 0x1d,
        MOV_FLAGS_IMM8 = 0x1e,
        MOV_FLAGS_MEM_F = 0x1f,
        MOV_FLAGS_MEM_IMM16 = 0x20,
        // MOV Stack Pointer
        MOV_SP_IMM16 = 0x2a,
        MOV_SP_F = 0x2b,
        // MOV Segment
        MOV_DS_IMM8 = 0x2c,
        MOV_DS_MEM = 0x2d,
        MOV_DS_A = 0x2e,
        MOV_SS_IMM8 = 0x2f,
        MOV_SS_MEM = 0x30,
        MOV_SS_A = 0x31,
        // MOV Interrupt Registers
        MOV_IPR_IMM = 0x32,
        MOV_IPR_A = 0x33,
        MOV_IPR_MEM_F = 0x34,
        MOV_IMR_IMM = 0x35,
        MOV_IMR_A = 0x36,
        MOV_IMR_MEM_F = 0x37,
        // STACK
        PUSH_A = 0x38,
        PUSH_C = 0x39,
        PUSH_D = 0x3a,
        PUSH_F = 0x3b,
        PUSH_FLAGS = 0x3c,
        PUSH_DS = 0x3d,
        PUSH_SS = 0x3e,
        PUSH_IMM8 = 0x3f,
        PUSH_IMM16 = 0x40,
        POP_A = 0x41,
        POP_C = 0x42,
        POP_D = 0x43,
        POP_F = 0x44,
        POP_FLAGS = 0x45,
        POP_DS = 0x46,
        POP_SS = 0x47,
        // ALU
        ADD_C = 0x48,
        ADD_D = 0x49,
        ADD_IMM8 = 0x4a,
        ADC_C = 0x4b,
        ADC_D = 0x4c,
        ADC_IMM8 = 0x4d,
        SUB_C = 0x4e,
        SUB_D = 0x4f,
        SUB_IMM8 = 0x50,
        SBB_C = 0x51,
        SBB_D = 0x52,
        SBB_IMM8 = 0x53,
        AND_C = 0x54,
        AND_D = 0x55,
        AND_IMM = 0x56,
        NAND_C = 0x57,
        NAND_D = 0x58,
        NAND_IMM = 0x59,
        OR_C = 0x5a,
        OR_D = 0x5b,
        OR_IMM = 0x5c,
        NOR_C = 0x5d,
        NOR_D = 0x5e,
        NOR_IMM = 0x5f,
        XOR_C = 0x60,
        XOR_D = 0x61,
        XOR_IMM = 0x61,
        XNOR_C = 0x62,
        XNOR_D = 0x63,
        XNOR_IMM = 0x64,
        NOT_A = 0x65,
        NOT_C = 0x66,
        NOT_D = 0x67,
        SHL = 0x68,
        SHR = 0x69,
        ASL = 0x6a,
        ASR = 0x6b,
        ROR = 0x6c,
        ROL = 0x6d,
        INC = 0x6e,
        DEC = 0x6f,
        CMP_C = 0x70,
        CMP_D = 0x71,
        CMP_IMM8 = 0x72,
        CMP_MEM_IMM16 = 0x73,
        TST_C = 0x74,
        TST_D = 0x75,
        TST_IMM8 = 0x76,
        TST_MEM_IMM16 = 0x77,
        // JUMPS
        JUMP_IMM16 = 0x78,
        // CPU
        HALT = 0xff;

    void Execute(MEM &memory)
    {
        while (1 == 1)
        {
            Byte instruction = FetchByte(memory);
            switch (instruction)
            {
            case MOV_A_IMM8:
            {
                Byte value = FetchByte(memory);
                Set_A(value);
            }
            break;
            case MOV_C_IMM8:
            {
                Byte value = FetchByte(memory);
                C = value;
            }
            break;
            case MOV_A_C:
            {
                Set_A(C);
            }
            break;
            case PUSH_A:
            {
                Push(A, memory);
            }
            break;
            case PUSH_C:
            {
                Push(C, memory);
            }
            break;
            case POP_A:
            {
                Byte value = Pop(memory);
                Set_A(value);
            }
            break;
            case ADD_C:
            {
                int res = (int)A + (int)C;
                CF = (res >> 8) > 0;
                SF = (res >> 8) & (0b1);
                OF = ((res >> 7) ^ ((int)A >> 7)) & (res >> 7) ^ ((int)C >> 7);
                ZF = res == 0x0;
                printf("%d \n", res);
                Set_A((Byte)res);
            }
            break;
            case JUMP_IMM16:
            {
                Word address = FetchWord(memory);
                Set_PC(address);
            }
            break;
            case HALT:
            {
                printf("A: %d \n", (int)A);
                printf("CF: %d | ", (int)CF);
                printf("SF: %d | ", (int)SF);
                printf("OF: %d | ", (int)OF);
                printf("ZF: %d \n", (int)ZF);
                std::exit(0);
            }
            break;
            default:
            {
                printf("Instruction not handled: %d", instruction);
            }
            break;
            }
        }
    }
};

int main()
{
    MEM mem;
    CPU cpu;
    cpu.Reset(mem);

    int num1;
    cout << "Enter Operand:";
    cin >> num1;
    int num2;
    cout << "Enter Operand:";
    cin >> num2;

    // Program Code
    mem[0x0] = CPU::JUMP_IMM16;
    mem[0x1] = 0x0;
    mem[0x2] = 0x80;
    mem[0x8000] = CPU::MOV_C_IMM8;
    mem[0x8001] = num1;
    mem[0x8002] = CPU::MOV_A_IMM8;
    mem[0x8003] = num2;
    mem[0x8004] = CPU::ADD_C;
    mem[0x8005] = CPU::HALT;
    // End Program

    // Execute
    cpu.Execute(mem);

    return 0;
}
