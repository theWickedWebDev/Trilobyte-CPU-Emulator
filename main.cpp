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

    Byte FetchByte(u32 &Cycles, MEM &memory)
    {
        Byte Data = memory[PC];
        PC++;
        Cycles--;
        return Data;
    }

    Word FetchWord(u32 &Cycles, MEM &memory)
    {
        Word Data = memory[PC];
        PC++;

        Data |= (memory[PC] << 8);
        PC++;

        Cycles += 2;
        return Data;
    }

    Byte ReadByte(MEM &Memory, Word Address, u32 &Cycles)
    {
        Byte Data = Memory[Address];
        Cycles--;
        return Data;
    }

    void WriteByte(Byte value, u32 &Cycles, Word Address, MEM &Memory)
    {
        Memory[Address] = value;
        Cycles--;
    }

    // Write two bytes
    void WriteWord(Word Value, u32 &Cycles, Word Address, MEM &Memory)
    {
        Memory[Address] = Value & 0xFF;
        Memory[Address + 1] = (Value >> 8);
        Cycles -= 2;
    }

    void Push(Byte value, u32 &Cycles, MEM &Memory)
    {
        WriteWord(value, Cycles, SP, Memory);
        SP--;
        Cycles--;
    }

    Byte Pop(u32 &Cycles, MEM &Memory)
    {

        SP++;
        Byte value = ReadByte(Memory, SP, Cycles);
        Cycles--;
        return value;
    }

    void Set_A(Byte value, u32 &Cycles)
    {
        A = value;
        ZF = (A == 0);
        SF = (A & 0b10000000) > 0;
        Cycles--;
    }

    void Set_PC(Word value, u32 &Cycles)
    {
        PC = value;
        Cycles--;
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
        // JUMPS
        JUMP_IMM16 = 0x48,
        // CPU
        HALT = 0xff;

    void
    Execute(u32 Cycles, MEM &memory)
    {
        while (Cycles > 0)
        {
            Byte instruction = FetchByte(Cycles, memory);
            switch (instruction)
            {
            case MOV_A_IMM8:
            {
                Byte value = FetchByte(Cycles, memory);
                Set_A(value, Cycles);
            }
            break;
            case MOV_C_IMM8:
            {
                Byte value = FetchByte(Cycles, memory);
                C = value;
                Cycles--;
            }
            break;
            case MOV_A_C:
            {
                Set_A(C, Cycles);
            }
            break;
            case PUSH_A:
            {
                Push(A, Cycles, memory);
            }
            break;
            case PUSH_C:
            {
                Push(C, Cycles, memory);
            }
            break;
            case POP_A:
            {
                Byte value = Pop(Cycles, memory);
                Set_A(value, Cycles);
            }
            break;
            case JUMP_IMM16:
            {
                Word address = FetchWord(Cycles, memory);
                Set_PC(address, Cycles);
            }
            break;
            case HALT:
            {
                std::exit(0);
            }
            break;
            default:
            {
                printf("Instruction not handled: %d", instruction);
                Cycles = 0;
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

    mem[0x0] = CPU::JUMP_IMM16;
    mem[0x1] = 0x6;
    mem[0x2] = 0x0;
    mem[0x6] = CPU::MOV_C_IMM8;
    mem[0x7] = 0x55;
    mem[0x8] = CPU::PUSH_C;
    mem[0x9] = CPU::POP_A;
    mem[0xA] = CPU::HALT;

    cpu.Execute(20, mem);

    return 0;
}
