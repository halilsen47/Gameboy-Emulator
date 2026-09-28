#include "timer.h"
#include <iostream>
TIMER::TIMER(MMU &mmu) : mmu_bus(mmu)
{
}

void TIMER::tick(uint8_t cycle_len)
{
    divcounter += cycle_len;
    if (divcounter >= 256)
    {
        divcounter -= 256;
        DIV++;
        // if (DIV == 0)
        // {
        //     std::cout << "[TIMER] DIV sayaci tasiyor (Kalp Atisi)...\n";
        // }
    }

    if (!(TAC & 0x04))
        return;

    scounter += cycle_len;

    if ((TAC & 0x03) == 0)
        limit = 1024;
    else if ((TAC & 0x03) == 1)
        limit = 16;
    else if ((TAC & 0x03) == 2)
        limit = 64;
    else if ((TAC & 0x03) == 3)
        limit = 256;

    if (scounter >= limit)
    {
        scounter -= limit;
        if (TIMA == 0xFF)
        {
            TIMA = TMA;
            std::cout << "\n==========================================\n";
            std::cout << "🔥 [TIMER] OVERFLOW! KESME (INTERRUPT) ATILDI!\n";
            std::cout << "==========================================\n\n";
            uint8_t int_reg = mmu_bus.readByte(0xFF0F);
            mmu_bus.writeByte(0xFF0F, int_reg | 0x04);
            return;
        }
        TIMA++;
    }
}
