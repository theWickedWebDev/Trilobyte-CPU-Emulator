#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "opcodes.cpp"
#include "mem.cpp"

using namespace std;
using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

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

    void PrintDetails(string Label, Byte instruction, bool SingleStep)
    {
        if (SingleStep)
        {
            std::cout << "\x1B[2J\x1B[H"; // CLEARS OUPUT
            int labelWidth = 14;
            printf("\n[REGISTERS]\n");
            printf("%*s", labelWidth, ((string) "[A]: " + to_string((int)A)).c_str());
            printf("%*s", labelWidth, ((string) "[C]: " + to_string((int)C)).c_str());
            printf("%*s", labelWidth, ((string) "[D]: " + to_string((int)D)).c_str());
            printf("%*s", labelWidth, ((string) "[F]: " + to_string((int)F)).c_str());
            printf("\n\n[FLAGS]\n");
            printf("%*s", labelWidth, ((string) "[CF]: " + to_string((int)CF)).c_str());
            printf("%*s", labelWidth, ((string) "[SF]: " + to_string((int)SF)).c_str());
            printf("%*s", labelWidth, ((string) "[OF]: " + to_string((int)OF)).c_str());
            printf("%*s", labelWidth, ((string) "[ZF]: " + to_string((int)ZF)).c_str());
            printf("\n\n Next Instruction: %s 0x%x \n", Label.c_str(), instruction);

            cin.get();
        }
    }

    void Execute(MEM &memory, bool SingleStep)
    {
        while (1 == 1)
        {
            Byte instruction = FetchByte(memory);
            switch (instruction)
            {
            case MOV_A_IMM8:
            {
                PrintDetails("MOV_A_IMM8", instruction, SingleStep);
                Byte value = FetchByte(memory);
                Set_A(value);
            }
            break;
            case MOV_C_IMM8:
            {
                PrintDetails("MOV_C_IMM8", instruction, SingleStep);
                Byte value = FetchByte(memory);
                C = value;
            }
            break;
            case MOV_A_C:
            {
                PrintDetails("MOV_A_C", instruction, SingleStep);
                Set_A(C);
            }
            break;
            case PUSH_A:
            {
                PrintDetails("PUSH_A", instruction, SingleStep);
                Push(A, memory);
            }
            break;
            case PUSH_C:
            {
                PrintDetails("PUSH_C", instruction, SingleStep);
                Push(C, memory);
            }
            break;
            case POP_A:
            {
                PrintDetails("POP_A", instruction, SingleStep);
                Byte value = Pop(memory);
                Set_A(value);
            }
            break;
            case ADD_C:
            {
                PrintDetails("ADD_C", instruction, SingleStep);
                int res = (int)A + (int)C;
                CF = (res >> 8) > 0;
                SF = (res >> 8) & (0b1);
                OF = ((res >> 7) ^ ((int)A >> 7)) & (res >> 7) ^ ((int)C >> 7);
                ZF = res == 0x0;
                Set_A((Byte)res);
            }
            break;
            case JUMP_IMM16:
            {
                PrintDetails("JUMP_IMM16", instruction, SingleStep);
                Word address = FetchWord(memory);
                Set_PC(address);
            }
            break;
            case HALT:
            {
                PrintDetails("HALT", instruction, true);
                // TODO: Change to cin and trigger interrupt? (NOP)
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