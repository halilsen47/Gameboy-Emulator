#ifndef TIMER_H
#define TIMER_H

#include <cstdint>
#include "mmu.h"

#define DIV_ADDR 0xFF04
#define TIMA_ADDR 0xFF05
#define TMA_ADDR 0xFF06
#define TAC_ADDR 0xFF07
class TIMER
{
private:
    // bu değer TAC de belirtilen cycle sayısına gelince TİMA değerini 1 arttıracağız
    uint16_t scounter;
    // bu değer 256 olunca DIV değerini 1 arttıracağız
    uint16_t divcounter;
    uint16_t limit;

    // timer registerları
    uint8_t DIV = 0;
    uint8_t TIMA = 0;
    uint8_t TMA = 0;
    uint8_t TAC = 0;

    MMU &mmu_bus;

public:
    TIMER(MMU &mmu);
    void tick(uint8_t cycle_len);

    uint8_t get_div() const { return DIV; }
    void reset_div()
    {
        DIV = 0;
        divcounter = 0;
    }

    uint8_t get_tima() const { return TIMA; }
    void set_tima(uint8_t val) { TIMA = val; }

    uint8_t get_tma() const { return TMA; }
    void set_tma(uint8_t val) { TMA = val; }

    uint8_t get_tac() const { return TAC; }
    void set_tac(uint8_t val) { TAC = val; }
};

#endif