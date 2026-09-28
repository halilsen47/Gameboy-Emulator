#ifndef PPU_H
#define PPU_H

#include <cstdint>
#include "mmu.h"

class PPU
{
private:
    /* data */
    void render_line();
    void oam_scan();
    void update_mode(uint8_t mode);
    void check_ly_lyc();
    uint16_t cycle_count;
    uint8_t VIP[10]{};
    uint8_t vip_count;
    uint8_t current_mode;

    MMU &mmu_bus;

    // control reg
    uint8_t LCDC;
    uint8_t STAT;

    // bg reg
    uint8_t SCY;
    uint8_t SCX;

    // line track
    uint8_t LY;
    uint8_t LYC;

    // color palettes
    uint8_t BGP;
    uint8_t OBP0;
    uint8_t OBP1;

    // window reg
    uint8_t WY;
    uint8_t WX;

public:
    PPU(MMU &mmu);
    void step(uint8_t cycle);
    uint8_t read_register(uint16_t address);
    void write_register(uint16_t address, uint8_t value);
    uint32_t screen_pixels[160 * 144]{};
};

#endif