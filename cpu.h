#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <iostream>
#include "mmu.h"

// Flag Maskeleri
constexpr uint8_t FLAG_Z = 1 << 7;
constexpr uint8_t FLAG_N = 1 << 6;
constexpr uint8_t FLAG_H = 1 << 5;
constexpr uint8_t FLAG_C = 1 << 4;

class CPU
{
private:
    union
    {
        uint16_t AF;
        struct
        {
            uint8_t F;
            uint8_t A;
        };
    };
    union
    {
        uint16_t BC;
        struct
        {
            uint8_t C;
            uint8_t B;
        };
    };
    union
    {
        uint16_t DE;
        struct
        {
            uint8_t E;
            uint8_t D;
        };
    };
    union
    {
        uint16_t HL;
        struct
        {
            uint8_t L;
            uint8_t H;
        };
    };

    uint16_t SP = 0xFFFE;
    uint16_t PC = 0x0100;
    bool IME = false;
    bool is_halted = false;
    MMU &mmu_bus;

    // --- KOMUT SETİ MİMARİSİ ---

    // Fonksiyon göstericisi (Pointer-to-Member) tipi tanımı
    using OpcodeHandler = uint8_t (CPU::*)();

    // 256 komutluk ana tablo
    std::array<OpcodeHandler, 256> instruction_table;

    std::array<OpcodeHandler, 256> cbinstruction_table;

    // Tabloyu başlangıçta dolduracak fonksiyon
    void build_instruction_table();

    // Hata ayıklama için varsayılan boş komut
    uint8_t unimplemented_opcode();

    // Yardımcı Okuma Fonksiyonları
    uint8_t fetch();
    uint16_t fetch16(); // 16-bit okumalar için (örneğin JP komutu)

    // --- KOMUTLAR (Opcodes) ---
    uint8_t op_00(); // NOP
    uint8_t op_C3(); // JP a16
    uint8_t op_AF();
    uint8_t op_21();
    uint8_t op_0E();
    uint8_t op_06();
    uint8_t op_32();
    uint8_t op_05();
    uint8_t op_20();
    uint8_t op_0D();
    uint8_t op_3E();
    uint8_t op_F3();
    uint8_t op_E0();
    uint8_t op_F0();
    uint8_t op_FE();
    uint8_t op_36();
    uint8_t op_EA();
    uint8_t op_31();
    uint8_t op_2A();
    uint8_t op_E2();
    uint8_t op_0C();
    uint8_t op_CD();
    uint8_t op_01();
    uint8_t op_0B();
    uint8_t op_78();
    uint8_t op_B1();
    uint8_t op_C9();
    uint8_t op_FB();
    uint8_t op_2F();
    uint8_t op_E6();
    uint8_t op_47();
    uint8_t op_B0();
    uint8_t op_4F();
    uint8_t op_A9();
    uint8_t op_A1();
    uint8_t op_79();
    uint8_t op_EF();
    uint8_t op_87();
    uint8_t op_E1();
    uint8_t op_5F();
    uint8_t op_16();
    uint8_t op_19();
    uint8_t op_5E();
    uint8_t op_23();
    uint8_t op_56();
    uint8_t op_D5();
    uint8_t op_E9();
    uint8_t op_11();
    uint8_t op_12();
    uint8_t op_13();
    uint8_t op_E5();
    uint8_t op_1A();
    uint8_t op_22();
    uint8_t op_D1();
    uint8_t op_7C();
    uint8_t op_F5();
    uint8_t op_C5();
    uint8_t op_FA();
    uint8_t op_28();
    uint8_t op_A7();
    uint8_t op_35();
    uint8_t op_7E();
    uint8_t op_34();
    uint8_t op_18();
    uint8_t op_C1();
    uint8_t op_F1();
    uint8_t op_CA();
    uint8_t op_C0();
    uint8_t op_C8();
    uint8_t op_37();
    uint8_t op_1C();
    uint8_t op_24();
    uint8_t op_09();
    uint8_t op_3D();
    uint8_t op_3C();
    uint8_t op_D9();
    uint8_t op_2C();
    uint8_t op_77();
    uint8_t op_4E();
    uint8_t op_46();
    uint8_t op_69();
    uint8_t op_60();
    uint8_t op_0A();
    uint8_t op_03();
    uint8_t op_85();
    uint8_t op_6F();
    uint8_t op_C2();
    uint8_t op_3A();
    uint8_t op_57();
    uint8_t op_7B();
    uint8_t op_7A();
    uint8_t op_73();
    uint8_t op_72();
    uint8_t op_71();
    uint8_t op_2D();
    uint8_t op_67();
    uint8_t op_7D();
    uint8_t op_C6();
    uint8_t op_5D();
    uint8_t op_54();
    uint8_t op_F6();
    uint8_t op_6B();
    uint8_t op_62();
    uint8_t op_40();
    uint8_t op_1E();
    uint8_t op_26();
    uint8_t op_07();
    uint8_t op_80();
    uint8_t op_89();
    uint8_t op_B8();
    uint8_t op_A8();
    uint8_t op_A0();
    uint8_t op_D6();
    uint8_t op_27();
    uint8_t op_86();
    uint8_t op_8E();
    uint8_t op_D0();
    uint8_t op_1D();
    uint8_t op_B9();
    uint8_t op_30();
    uint8_t op_90();
    uint8_t op_61();
    uint8_t op_04();
    uint8_t op_D8();
    uint8_t op_1B();
    uint8_t op_B7();
    uint8_t op_38();
    uint8_t op_76();
    uint8_t op_BE();
    uint8_t op_EE();
    uint8_t op_96();
    uint8_t op_2B();
    uint8_t op_82();
    uint8_t op_25();
    uint8_t op_B2();
    uint8_t op_99();
    uint8_t op_DE();
    uint8_t op_2E();
    uint8_t op_70();
    uint8_t op_02();
    uint8_t op_C4();
    uint8_t op_CF();
    uint8_t op_48();
    uint8_t op_DF();
    uint8_t op_81();
    uint8_t op_83();
    uint8_t op_93();
    uint8_t op_D2();
    uint8_t op_44();
    uint8_t op_43();
    uint8_t op_4D();
    uint8_t op_4B();
    uint8_t op_91();
    uint8_t op_A6();
    uint8_t op_CC();
    uint8_t op_B6();
    uint8_t op_DA();
    uint8_t op_B5();

    void or_reg(uint8_t value);
    void and_reg(uint8_t value);
    void dec_8bit(uint8_t &reg);
    void inc_8bit(uint8_t &reg);
    void push_16bit(uint16_t value);
    void pop_16bit(uint16_t &reg);
    void add_8bit(uint8_t value);
    void cp_8bit(uint8_t value);
    void add_hl(uint16_t value);
    void xor_reg(uint8_t value);
    void adc_8bit(uint8_t val);
    void sub_8bit(uint8_t value);
    void sbc_8bit(uint8_t val);

    uint8_t op_cb37();
    uint8_t op_cb87();
    uint8_t op_cb27();
    uint8_t op_cb7F();
    uint8_t op_cb86();
    uint8_t op_cb50();
    uint8_t op_cb60();
    uint8_t op_cb68();
    uint8_t op_cb58();
    uint8_t op_cb7E();
    uint8_t op_cb3F();
    uint8_t op_cb40();
    uint8_t op_cb5F();
    uint8_t op_cb33();
    uint8_t op_cb77();
    uint8_t op_cb6F();
    uint8_t op_cb48();
    uint8_t op_cb61();
    uint8_t op_cb69();
    uint8_t op_cb47();
    uint8_t op_cbFE();
    uint8_t op_cbBE();
    uint8_t op_cb46();
    uint8_t op_cb70();
    uint8_t op_cb78();
    uint8_t op_cb41();
    uint8_t op_cb57();
    uint8_t op_cbDE();
    uint8_t op_cb9E();
    uint8_t op_cbD8();
    uint8_t op_cbF8();
    uint8_t op_cb71();
    uint8_t op_cb79();
    uint8_t op_cbD0();
    uint8_t op_cbF0();
    uint8_t op_cb6E();
    uint8_t op_cb76();
    uint8_t op_cb4F();
    uint8_t op_cbC6();
    uint8_t op_cb26();
    uint8_t op_cbC7();
    uint8_t op_cbD6();
    uint8_t op_cbCF();
    uint8_t op_cbCE();
    uint8_t op_cbD7();
    uint8_t op_cb96();
    uint8_t op_cbEE();
    uint8_t op_cbAE();
    uint8_t op_cb7D();

    void handle_interrupts();

public:
    CPU(MMU &mmu);

    // İşlemciyi 1 adım (1 komut) ileri götüren ana döngü fonksiyonu
    uint8_t step();
};

#endif