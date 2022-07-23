#pragma once

using Byte = unsigned char;
using u32 = unsigned int;

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