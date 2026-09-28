#include "ppu.h"
#include "mmu.h"
PPU::PPU(MMU &mmu) : mmu_bus(mmu)
{
    update_mode(2);
}

void PPU::step(uint8_t cycle)
{
    uint8_t ctrl = LCDC & 0x80;
    if (!ctrl)
    {
        LY = 0;
        cycle_count = 0;
        update_mode(0);
        return;
    }
    cycle_count += cycle;
    switch (current_mode)
    {
    case 2:
        if (cycle_count >= 80)
        {

            oam_scan();
            update_mode(3);
        }
        break;
    case 3:
        if (cycle_count >= 252)
        {

            render_line();
            update_mode(0);
            // Hblank kesmesi
            if (STAT & 0x08)
            {
                uint8_t current_if = mmu_bus.readByte(0xFF0F);
                mmu_bus.writeByte(0xFF0F, current_if | 0x02); // 1. Biti (STAT Kesmesi) yak
            }
        }
        break;
    case 0:
        if (cycle_count >= 456)
        {

            LY++;
            check_ly_lyc();
            cycle_count -= 456;
            if (LY == 144)
            {
                update_mode(1);
                // Vblank kesmesi
                uint8_t current_if = mmu_bus.readByte(0xFF0F);
                mmu_bus.writeByte(0xFF0F, current_if | 0x01);
                // stat kesmesi
                if (STAT & 0x10)
                {
                    current_if = mmu_bus.readByte(0xFF0F);
                    mmu_bus.writeByte(0xFF0F, current_if | 0x02);
                }
            }
            else
            {
                update_mode(2);
                // mode 2 interruptı isteniyorsa
                if (STAT & 0x20)
                {
                    uint8_t current_if = mmu_bus.readByte(0xFF0F);
                    mmu_bus.writeByte(0xFF0F, current_if | 0x02);
                }
            }
        }

        break;
    case 1:
        if (cycle_count >= 456)
        {

            LY++;
            check_ly_lyc();
            cycle_count -= 456;
            if (LY > 153)
            {
                update_mode(2);
                // mode 2 interruptı isteniyorsa

                if (STAT & 0x20)
                {
                    uint8_t current_if = mmu_bus.readByte(0xFF0F);
                    mmu_bus.writeByte(0xFF0F, current_if | 0x02);
                }
                LY = 0;
                check_ly_lyc();
            }
        }
        break;
    default:
        break;
    }
}

void PPU ::update_mode(uint8_t mode)
{
    current_mode = mode;
    STAT &= 0xFC;
    STAT |= mode;
}

void PPU::oam_scan()
{
    // 10 tane nesne bulana kadar 40 adet obje arasında dönmem gerek bunu da 0xFFE0 dan başlayıp bakmam lazım her obje 4 byte yer kaplıyor yakaladığım objenin y değerine bakmam ve bu değerden 16 çıkarmam ve bu değerden 8 değer fazlasına bakmam lazım bu aralığa benim LY değerim geliyorsa VIP dizisinin içine indexi atacağım
    vip_count = 0;
    for (size_t i = 0; i < 40; i++)
    {
        uint16_t addr = 0xFE00 + (i * 4);
        uint8_t obj_y = mmu_bus.readByte(addr);
        // LY ye 16 ekleme vs işlemlerini şu an 8x8 için yapıyoruz ileride bir tileın yarısı gözükmezse 8x16 tile kullanılıyordur onun içi bu matematik işlemini dinamik yapmam gerekebilir.
        if (LY + 16 >= obj_y && LY + 8 < obj_y)
        {
            VIP[vip_count] = i;
            vip_count++;
            if (vip_count == 10)
                break;
        }
    }
}

void PPU::render_line()
{

    bool bg_window_enable = (LCDC & 0x01) != 0;
    bool obj_enable = (LCDC & 0x02) != 0;
    bool window_enable = (LCDC & 0x20) != 0;

    bool draw_window = false;
    if (window_enable && LY >= WY)
    {
        draw_window = true;
    }

    for (int x = 0; x < 160; x++)
    {
        uint8_t bg_color_id = 0;
        uint8_t final_color = 0;

        if (bg_window_enable)
        {
            uint16_t map_base;
            uint8_t x_pos, y_pos;

            bool is_window = draw_window && x >= (WX - 7);

            if (is_window)
            {
                x_pos = x - (WX - 7);
                y_pos = LY - WY;
                map_base = (LCDC & 0x40) ? 0x9C00 : 0x9800;
            }
            else
            {

                x_pos = x + SCX;
                y_pos = LY + SCY;
                map_base = (LCDC & 0x08) ? 0x9C00 : 0x9800;
            }

            uint8_t tile_col = x_pos / 8;
            uint8_t tile_row = y_pos / 8;
            uint16_t tile_address = map_base + (tile_row * 32) + tile_col;
            uint8_t tile_id = mmu_bus.readByte(tile_address);

            uint16_t data_address;
            if (LCDC & 0x10) // Bit 4
            {
                data_address = 0x8000 + (tile_id * 16);
            }
            else
            {
                int8_t signed_id = static_cast<int8_t>(tile_id);
                data_address = 0x9000 + (signed_id * 16);
            }

            uint8_t pixel_y = y_pos % 8;
            uint8_t pixel_x = x_pos % 8;

            uint16_t line_address = data_address + (pixel_y * 2);
            uint8_t byte1 = mmu_bus.readByte(line_address);
            uint8_t byte2 = mmu_bus.readByte(line_address + 1);

            int bit_index = 7 - pixel_x;
            uint8_t low_bit = (byte1 >> bit_index) & 0x01;
            uint8_t high_bit = (byte2 >> bit_index) & 0x01;

            bg_color_id = (high_bit << 1) | low_bit;

            final_color = (BGP >> (bg_color_id * 2)) & 0x03;
        }
        else
        {

            bg_color_id = 0;
            final_color = 0;
        }

        if (obj_enable)
        {
            for (int i = 0; i < vip_count; i++)
            {
                uint8_t obj_index = VIP[i];
                uint16_t obj_addr = 0xFE00 + (obj_index * 4);

                uint8_t obj_y = mmu_bus.readByte(obj_addr);
                uint8_t obj_x = mmu_bus.readByte(obj_addr + 1);
                uint8_t tile_id = mmu_bus.readByte(obj_addr + 2);
                uint8_t attributes = mmu_bus.readByte(obj_addr + 3);

                if (x >= (obj_x - 8) && x < obj_x)
                {

                    int obj_pixel_x = x - (obj_x - 8);
                    int obj_pixel_y = LY - (obj_y - 16);

                    // Donanımsal Çevirmeler (Flip) var mı?
                    bool y_flip = (attributes & 0x40) != 0; // Bit 6
                    bool x_flip = (attributes & 0x20) != 0; // Bit 5

                    if (y_flip)
                        obj_pixel_y = 7 - obj_pixel_y;
                    if (x_flip)
                        obj_pixel_x = 7 - obj_pixel_x;

                    uint16_t obj_data_address = 0x8000 + (tile_id * 16) + (obj_pixel_y * 2);

                    uint8_t byte1 = mmu_bus.readByte(obj_data_address);
                    uint8_t byte2 = mmu_bus.readByte(obj_data_address + 1);

                    int bit_index = 7 - obj_pixel_x;
                    uint8_t low_bit = (byte1 >> bit_index) & 0x01;
                    uint8_t high_bit = (byte2 >> bit_index) & 0x01;

                    uint8_t obj_color_id = (high_bit << 1) | low_bit;

                    if (obj_color_id == 0)
                        continue;

                    bool obj_behind_bg = (attributes & 0x80) != 0;
                    if (obj_behind_bg && bg_color_id != 0)
                        continue;

                    uint8_t palette = (attributes & 0x10) ? OBP1 : OBP0;

                    final_color = (palette >> (obj_color_id * 2)) & 0x03;

                    break;
                }
            }
        }

        int buffer_index = (LY * 160) + x;
        screen_pixels[buffer_index] = final_color;
    }
}

uint8_t PPU::read_register(uint16_t address)
{
    switch (address)
    {
    case 0xFF40:
        return LCDC;
    case 0xFF41:
        return STAT;
    case 0xFF42:
        return SCY;
    case 0xFF43:
        return SCX;
    case 0xFF44:
        return LY;
    case 0xFF45:
        return LYC;
    // 0xFF46 DMA Transferidir, PPU'nun değil genelde MMU'nun kendi işidir, o yüzden atlıyoruz.
    case 0xFF47:
        return BGP;
    case 0xFF48:
        return OBP0;
    case 0xFF49:
        return OBP1;
    case 0xFF4A:
        return WY;
    case 0xFF4B:
        return WX;
    default:
        return 0xFF;
    }
}

void PPU::write_register(uint16_t address, uint8_t value)
{
    switch (address)
    {
    case 0xFF40:
        LCDC = value;
        break;

    case 0xFF41:
        // DİKKAT: İlk 3 bit (Bit 0,1,2) READ-ONLY'dir, PPU kontrol eder.
        // Sadece üst bitler (Interrupt şalterleri) CPU tarafından değiştirilebilir.
        STAT = (value & 0xF8) | (STAT & 0x07);
        break;

    case 0xFF42:
        SCY = value;
        break;
    case 0xFF43:
        SCX = value;
        break;

    case 0xFF44:
        // DİKKAT: LY registerı READ-ONLY'dir.
        // Donanım kuralı gereği CPU buraya herhangi bir şey yazmaya kalkarsa LY sıfırlanır!
        LY = 0;
        break;

    case 0xFF45:
        LYC = value;
        break;

        // 0xFF46 (DMA) donanımda MMU (veya OAM DMA kontrolcüsü) üzerinden yapılır,
        // PPU'nun iç işi değildir, o yüzden es geçiyoruz.

    case 0xFF47:
        BGP = value;
        break;
    case 0xFF48:
        OBP0 = value;
        break;
    case 0xFF49:
        OBP1 = value;
        break;
    case 0xFF4A:
        WY = value;
        break;
    case 0xFF4B:
        WX = value;
        break;

    default:
        break;
    }
}

void PPU::check_ly_lyc()
{
    if (LY == LYC)
    {
        STAT |= 0x04; // Eşitlik var: STAT'ın 2. Bitini 1 yap

        if (STAT & 0x40) // 6. Bit (LY=LYC STAT Interrupt Şalteri) açıksa
        {
            uint8_t current_if = mmu_bus.readByte(0xFF0F);
            mmu_bus.writeByte(0xFF0F, current_if | 0x02); // STAT kesmesini yolla
        }
    }
    else
    {
        STAT &= 0xFB; // Eşit değil: STAT'ın 2. Bitini 0 yap (0xFB = 11111011)
    }
}