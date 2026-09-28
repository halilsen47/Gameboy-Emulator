#ifndef MMU_H
#define MMU_H

#include <cstdint>
#include <array>
#include <fstream>

class TIMER;
class PPU;

class MMU
{

private:
    TIMER *timer_ptr = nullptr;
    PPU *ppu_ptr = nullptr;
    std::array<uint8_t, 65536> memory{};

public:
    MMU();
    uint8_t joypad_state = 0xFF;
    uint8_t readByte(uint16_t address);
    void writeByte(uint16_t address, uint8_t value);
    void link_timer(TIMER *timer) { timer_ptr = timer; }
    void link_ppu(PPU *ppu) { ppu_ptr = ppu; }
};

#endif