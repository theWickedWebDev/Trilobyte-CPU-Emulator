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

    int num1 = 144;
    int num2 = 208;
    // printf("%s", "Add C to A \n"); // 144 + 208 sets OF
    // cout << "Enter A: ";
    // cin >> num1;
    // cout << "Enter C: ";
    // cin >> num2;

    // Program Code
    mem[0x0] = JUMP_IMM16;
    mem[0x1] = 0x0;
    mem[0x2] = 0x80;
    mem[0x8000] = MOV_C_IMM8;
    mem[0x8001] = num2;
    mem[0x8002] = MOV_A_IMM8;
    mem[0x8003] = num1;
    mem[0x8004] = ADD_C;
    mem[0x8005] = HALT;
    // End Program

    // Execute
    bool SingleStep = false;
    cpu.Execute(mem, SingleStep);

    return 0;
}
