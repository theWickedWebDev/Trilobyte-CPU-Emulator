#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "mem.cpp"
#include "opcodes.cpp"
#include "cpu.cpp"

using namespace std;
using Byte = unsigned char;
using Word = unsigned short;
using u32 = unsigned int;

int main()
{
    MEM mem;
    CPU cpu;
    cpu.Reset(mem);

    //  Program Code
    mem[0x0] = JUMP_IMM16;
    mem[0x1] = 0x0;
    mem[0x2] = 0x80;
    mem[0x8000] = MOV_F_IMM16;
    mem[0x8001] = 0xff;
    mem[0x8002] = 0x22;
    mem[0x8003] = MOV_C_MEM_F;
    mem[0x8004] = MOV_A_IMM8;
    mem[0x8005] = 0x55;
    mem[0x8006] = ADD_C;
    mem[0x8007] = HALT;
    mem[0x22ff] = 0x1;
    // End Program

    // Execute
    bool SingleStep = false;
    cpu.Execute(mem, SingleStep);

    return 0;
}
