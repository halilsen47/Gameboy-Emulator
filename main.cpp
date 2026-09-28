#include <iostream>
#include <stdexcept>
#include <SDL2/SDL.h>
#include "mmu.h"
#include "cpu.h"
#include "timer.h"
#include "ppu.h" // PPU başlığı eklendi

// --- SDL VE GRAFİK SABİTLERİ ---
const int GB_WIDTH = 160;
const int GB_HEIGHT = 144;
const int SCALE = 4; // Ekranı 4 kat büyütüyoruz (640x576)

// Orijinal Game Boy "Ispanak Yeşili" RGB Renk Paleti (Hex: 0xAARRGGBB)
const uint32_t PALETTE[4] = {
    0xFF9BBC0F, // 0: En Açık Yeşil (Beyaz)
    0xFF8BAC0F, // 1: Açık Yeşil
    0xFF306230, // 2: Koyu Yeşil
    0xFF0F380F  // 3: En Koyu Yeşil (Siyah)
};

int main(int argc, char *argv[])
{
    // ==========================================
    // 1. SDL2 BAŞLATMA VE PENCERE KURULUMU
    // ==========================================
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL Baslatilamadi! Hata: " << SDL_GetError() << "\n";
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow(
        "Game Boy Emulator - Tetris",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GB_WIDTH * SCALE, GB_HEIGHT * SCALE,
        SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GB_WIDTH, GB_HEIGHT);

    uint32_t sdl_pixels[GB_WIDTH * GB_HEIGHT]; // RGB piksellerini tutacağımız dizi

    try
    {
        // ==========================================
        // 2. GAME BOY DONANIMLARINI BAŞLAT
        // ==========================================
        MMU mmu;
        TIMER timer(mmu);
        PPU ppu(mmu);
        CPU cpu(mmu);

        mmu.link_timer(&timer);
        mmu.link_ppu(&ppu);

        std::cout << "Emülatör baslatildi. Tetris calistiriliyor...\n";

        bool is_running = true;
        SDL_Event event;

        uint8_t prev_ly = 0; // Bir önceki satırı tutmak için

        // ==========================================
        // 3. ANA DONANIM DÖNGÜSÜ (CLOCK LOOP)
        // ==========================================
        while (is_running)
        {
            // A. Donanımlara Clock (Saat Vuruşu) Gönder
            uint8_t m_cycles = cpu.step();
            uint8_t t_cycles = m_cycles * 4;
            timer.tick(t_cycles);
            ppu.step(t_cycles);

            // B. Ekran Çizimi Bitti mi Kontrol Et (V-Blank'e giriş anı)
            // Eğer LY 144 olduysa ve bir önceki adımda 144 değilse, yeni kare hazırdır!
            if (mmu.readByte(0xFF44) == 144 && prev_ly != 144)
            {
                // 1. Pencere olaylarını (Çarpıya basma vs.) kontrol et
                while (SDL_PollEvent(&event))
                {
                    if (event.type == SDL_QUIT)
                        is_running = false;

                    // JOYPAD KONTROL
                    else if (event.type == SDL_KEYDOWN)
                    {
                        bool pressed = true;

                        switch (event.key.keysym.sym)
                        {
                        case SDLK_RIGHT:
                            mmu.joypad_state &= ~(1 << 0);
                            break;
                        case SDLK_LEFT:
                            mmu.joypad_state &= ~(1 << 1);
                            break;
                        case SDLK_UP:
                            mmu.joypad_state &= ~(1 << 2);
                            break;
                        case SDLK_DOWN:
                            mmu.joypad_state &= ~(1 << 3);
                            break;
                        case SDLK_z:
                            mmu.joypad_state &= ~(1 << 4);
                            break; // Z tuşu = A Butonu
                        case SDLK_x:
                            mmu.joypad_state &= ~(1 << 5);
                            break; // X tuşu = B Butonu
                        case SDLK_SPACE:
                            mmu.joypad_state &= ~(1 << 6);
                            break; // Boşluk = Select
                        case SDLK_RETURN:
                            mmu.joypad_state &= ~(1 << 7);
                            break; // Enter = Start
                        default:
                            pressed = false;
                            break;
                        }

                        if (pressed)
                        {
                            // IF register'ının (0xFF0F) 4. bitini (Joypad Interrupt) 1 yapıyoruz
                            uint8_t current_if = mmu.readByte(0xFF0F);
                            mmu.writeByte(0xFF0F, current_if | 0x10);
                        }
                    }
                    else if (event.type == SDL_KEYUP)
                    {
                        switch (event.key.keysym.sym)
                        {
                        case SDLK_RIGHT:
                            mmu.joypad_state |= (1 << 0);
                            break;
                        case SDLK_LEFT:
                            mmu.joypad_state |= (1 << 1);
                            break;
                        case SDLK_UP:
                            mmu.joypad_state |= (1 << 2);
                            break;
                        case SDLK_DOWN:
                            mmu.joypad_state |= (1 << 3);
                            break;
                        case SDLK_z:
                            mmu.joypad_state |= (1 << 4);
                            break;
                        case SDLK_x:
                            mmu.joypad_state |= (1 << 5);
                            break;
                        case SDLK_SPACE:
                            mmu.joypad_state |= (1 << 6);
                            break;
                        case SDLK_RETURN:
                            mmu.joypad_state |= (1 << 7);
                            break;
                        }
                    }
                }

                // 2. PPU'nun hazırladığı 0,1,2,3 değerlerini RGB'ye çevir
                for (int i = 0; i < (GB_WIDTH * GB_HEIGHT); i++)
                {
                    sdl_pixels[i] = PALETTE[ppu.screen_pixels[i]];
                }

                // 3. Pikselleri Tuvale ve Ekrana Aktar
                SDL_UpdateTexture(texture, nullptr, sdl_pixels, GB_WIDTH * sizeof(uint32_t));
                SDL_RenderClear(renderer);
                SDL_RenderCopy(renderer, texture, nullptr, nullptr);
                SDL_RenderPresent(renderer);

                // Oyunun çok hızlı akmaması için ~60 FPS sabitleme (Kabaca 16ms gecikme)
                SDL_Delay(16);
            }

            prev_ly = mmu.readByte(0xFF44);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Sistem durduruldu: " << e.what() << "\n";
    }

    // ==========================================
    // 4. KAPANIŞ VE TEMİZLİK
    // ==========================================
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << "Emülatör güvenle kapatildi.\n";
    return 0;
}