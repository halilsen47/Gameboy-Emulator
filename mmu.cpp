#include "mmu.h"
#include "timer.h"
#include "ppu.h"
MMU::MMU()
{
    // std::ifstream file("Dr. Mario.gb", std::ios::binary | std::ios::ate);
    std::ifstream file("tetris.gb", std::ios::binary | std::ios::ate);

    if (!file.is_open())
        throw std::runtime_error("ROM bulunamadi!");

    std::streamsize size = file.tellg();

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(memory.data()), size);
}

uint8_t MMU::readByte(uint16_t address)
{
    if (address == 0xFF00)
    {
        uint8_t select = memory[0xFF00];

        // yön tuşları
        if ((select & 0x10) == 0x00)
        {
            return 0xC0 | (select & 0x30) | (joypad_state & 0x0F);
        }
        else if ((select & 0x20) == 0x00)
        {
            return 0xC0 | (select & 0x30) | ((joypad_state >> 4) & 0x0F);
        }

        return 0xFF;
    }
    // ppu yönlendirmesi
    if (address >= 0xFF40 && address <= 0xFF4B)
    {
        return ppu_ptr->read_register(address);
    }
    // timer yönlendirmesi
    if (address == 0xFF04)
        return timer_ptr->get_div();
    if (address == 0xFF05)
        return timer_ptr->get_tima();
    if (address == 0xFF06)
        return timer_ptr->get_tma();
    if (address == 0xFF07)
        return timer_ptr->get_tac();

    return memory[address];
}

void MMU::writeByte(uint16_t address, uint8_t value)
{
    // DMA Transfer Tetikleyicisi (0xFF46)
    if (address == 0xFF46)
    {
        // Yazılan değer (value), kopyalanacak kaynağın üst baytıdır.
        // Mesela value = 0xC1 ise, kaynak adres 0xC100 olur.
        uint16_t source_addr = value << 8;

        // OAM belleği tam 160 byte'tır (40 obje * 4 byte)
        for (int i = 0; i < 160; i++)
        {
            uint8_t data = readByte(source_addr + i);
            // 0xFE00 bölgesine doğrudan yazıyoruz
            memory[0xFE00 + i] = data;

            // DİKKAT: Eğer 0xFE00 için özel bir writeByte koşulun varsa,
            // sonsuz döngüye girmemesi için memory dizisine (veya vector'e)
            // direkt yazman daha güvenlidir.
        }

        // 0xFF46 adresinin kendisine de o değeri yazmayı unutma
        memory[0xFF46] = value;
        return;
    }
    // ppu yönlendirmesi
    if (address >= 0xFF40 && address <= 0xFF4B)
    {
        ppu_ptr->write_register(address, value);
    }
    // timer yönlendirmesi
    if (address == 0xFF04)
    {
        timer_ptr->reset_div();
        return;
    }
    if (address == 0xFF05)
    {
        timer_ptr->set_tima(value);
        return;
    }
    if (address == 0xFF06)
    {
        timer_ptr->set_tma(value);
        return;
    }
    if (address == 0xFF07)
    {
        timer_ptr->set_tac(value);
        return;
    }

    if (address == 0xFF04)
    {
        memory[0xFF04] = 0x00;
        return;
    }

    if (!(address >= 0x0000 && address < 0x8000))
        memory[address] = value;
}
