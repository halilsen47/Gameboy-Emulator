#include "cpu.h"
#include <iomanip>
#include <stdexcept>
// interruptları ekledim ve şimdi interrupt kısımlarında yeni komutlar istiyor onları dolduracağım 0xC0 da kaldım en son.
CPU::CPU(MMU &mmu) : mmu_bus(mmu)
{
    build_instruction_table();

    // Boot ROM'u atlayıp doğrudan oyunu başlatmak için donanım başlangıç değerleri:

    AF = 0x01B0;
    BC = 0x0013;
    DE = 0x00D8;
    HL = 0x014D;
}

void CPU::build_instruction_table()
{
    // Öncelikle tüm 256 indeksi "yazılmamış komut" fonksiyonuna bağla
    instruction_table.fill(&CPU::unimplemented_opcode);
    cbinstruction_table.fill(&CPU::unimplemented_opcode);

    // Ardından sadece kodladığın komutları asıl fonksiyonlara yönlendir
    instruction_table[0x00] = &CPU::op_00;
    instruction_table[0xC3] = &CPU::op_C3;
    instruction_table[0xAF] = &CPU::op_AF;
    instruction_table[0x21] = &CPU::op_21;
    instruction_table[0x0E] = &CPU::op_0E;
    instruction_table[0x06] = &CPU::op_06;
    instruction_table[0x32] = &CPU::op_32;
    instruction_table[0x05] = &CPU::op_05;
    instruction_table[0x20] = &CPU::op_20;
    instruction_table[0x0D] = &CPU::op_0D;
    instruction_table[0x3E] = &CPU::op_3E;
    instruction_table[0xF3] = &CPU::op_F3;
    instruction_table[0xE0] = &CPU::op_E0;
    instruction_table[0xF0] = &CPU::op_F0;
    instruction_table[0XFE] = &CPU::op_FE;
    instruction_table[0x36] = &CPU::op_36;
    instruction_table[0xEA] = &CPU::op_EA;
    instruction_table[0x31] = &CPU::op_31;
    instruction_table[0x2A] = &CPU::op_2A;
    instruction_table[0xE2] = &CPU::op_E2;
    instruction_table[0x0C] = &CPU::op_0C;
    instruction_table[0xCD] = &CPU::op_CD;
    instruction_table[0x01] = &CPU::op_01;
    instruction_table[0x0B] = &CPU::op_0B;
    instruction_table[0x78] = &CPU::op_78;
    instruction_table[0xB1] = &CPU::op_B1;
    instruction_table[0xC9] = &CPU::op_C9;
    instruction_table[0xFB] = &CPU::op_FB;
    instruction_table[0x2F] = &CPU::op_2F;
    instruction_table[0xE6] = &CPU::op_E6;
    instruction_table[0x47] = &CPU::op_47;
    instruction_table[0xB0] = &CPU::op_B0;
    instruction_table[0x4F] = &CPU::op_4F;
    instruction_table[0xA9] = &CPU::op_A9;
    instruction_table[0XA1] = &CPU::op_A1;
    instruction_table[0x79] = &CPU::op_79;
    instruction_table[0xEF] = &CPU::op_EF;
    instruction_table[0x87] = &CPU::op_87;
    instruction_table[0xE1] = &CPU::op_E1;
    instruction_table[0x5F] = &CPU::op_5F;
    instruction_table[0x16] = &CPU::op_16;
    instruction_table[0x19] = &CPU::op_19;
    instruction_table[0x5E] = &CPU::op_5E;
    instruction_table[0x23] = &CPU::op_23;
    instruction_table[0x56] = &CPU::op_56;
    instruction_table[0xD5] = &CPU::op_D5;
    instruction_table[0xE9] = &CPU::op_E9;
    instruction_table[0x11] = &CPU::op_11;
    instruction_table[0x12] = &CPU::op_12;
    instruction_table[0x13] = &CPU::op_13;
    instruction_table[0xE5] = &CPU::op_E5;
    instruction_table[0x1A] = &CPU::op_1A;
    instruction_table[0x22] = &CPU::op_22;
    instruction_table[0xD1] = &CPU::op_D1;
    instruction_table[0x7C] = &CPU::op_7C;
    instruction_table[0xF5] = &CPU::op_F5;
    instruction_table[0xC5] = &CPU::op_C5;
    instruction_table[0xFA] = &CPU::op_FA;
    instruction_table[0x28] = &CPU::op_28;
    instruction_table[0xA7] = &CPU::op_A7;
    instruction_table[0x35] = &CPU::op_35;
    instruction_table[0x7E] = &CPU::op_7E;
    instruction_table[0x34] = &CPU::op_34;
    instruction_table[0x18] = &CPU::op_18;
    instruction_table[0xc1] = &CPU::op_C1;
    instruction_table[0xF1] = &CPU::op_F1;
    instruction_table[0xCA] = &CPU::op_CA;
    instruction_table[0xC0] = &CPU::op_C0;
    instruction_table[0xC8] = &CPU::op_C8;
    instruction_table[0x37] = &CPU::op_37;
    instruction_table[0x1C] = &CPU::op_1C;
    instruction_table[0x24] = &CPU::op_24;
    instruction_table[0x09] = &CPU::op_09;
    instruction_table[0x3D] = &CPU::op_3D;
    instruction_table[0x3C] = &CPU::op_3C;
    instruction_table[0xD9] = &CPU::op_D9;
    instruction_table[0x2C] = &CPU::op_2C;
    instruction_table[0x77] = &CPU::op_77;
    instruction_table[0x4E] = &CPU::op_4E;
    instruction_table[0x46] = &CPU::op_46;
    instruction_table[0x69] = &CPU::op_69;
    instruction_table[0x60] = &CPU::op_60;
    instruction_table[0x0A] = &CPU::op_0A;
    instruction_table[0x03] = &CPU::op_03;
    instruction_table[0x85] = &CPU::op_85;
    instruction_table[0x6F] = &CPU::op_6F;
    instruction_table[0xC2] = &CPU::op_C2;
    instruction_table[0x3A] = &CPU::op_3A;
    instruction_table[0x57] = &CPU::op_57;
    instruction_table[0x7B] = &CPU::op_7B;
    instruction_table[0x7A] = &CPU::op_7A;
    instruction_table[0x73] = &CPU::op_73;
    instruction_table[0x72] = &CPU::op_72;
    instruction_table[0x71] = &CPU::op_71;
    instruction_table[0x2D] = &CPU::op_2D;
    instruction_table[0x67] = &CPU::op_67;
    instruction_table[0x7D] = &CPU::op_7D;
    instruction_table[0xC6] = &CPU::op_C6;
    instruction_table[0x5D] = &CPU::op_5D;
    instruction_table[0x54] = &CPU::op_54;
    instruction_table[0xF6] = &CPU::op_F6;
    instruction_table[0x6B] = &CPU::op_6B;
    instruction_table[0x62] = &CPU::op_62;
    instruction_table[0x40] = &CPU::op_40;
    instruction_table[0x1E] = &CPU::op_1E;
    instruction_table[0x26] = &CPU::op_26;
    instruction_table[0x07] = &CPU::op_07;
    instruction_table[0x80] = &CPU::op_80;
    instruction_table[0x89] = &CPU::op_89;
    instruction_table[0xB8] = &CPU::op_B8;
    instruction_table[0xA8] = &CPU::op_A8;
    instruction_table[0xA0] = &CPU::op_A0;
    instruction_table[0xD6] = &CPU::op_D6;
    instruction_table[0x27] = &CPU::op_27;
    instruction_table[0x86] = &CPU::op_86;
    instruction_table[0x8E] = &CPU::op_8E;
    instruction_table[0xD0] = &CPU::op_D0;
    instruction_table[0x1D] = &CPU::op_1D;
    instruction_table[0xB9] = &CPU::op_B9;
    instruction_table[0x30] = &CPU::op_30;
    instruction_table[0x90] = &CPU::op_90;
    instruction_table[0x61] = &CPU::op_61;
    instruction_table[0x04] = &CPU::op_04;
    instruction_table[0xD8] = &CPU::op_D8;
    instruction_table[0x1B] = &CPU::op_1B;
    instruction_table[0xB7] = &CPU::op_B7;
    instruction_table[0x38] = &CPU::op_38;
    instruction_table[0xBE] = &CPU::op_BE;
    instruction_table[0xEE] = &CPU::op_EE;
    instruction_table[0x96] = &CPU::op_96;
    instruction_table[0x2B] = &CPU::op_2B;
    instruction_table[0x82] = &CPU::op_82;
    instruction_table[0x25] = &CPU::op_25;
    instruction_table[0xB2] = &CPU::op_B2;
    instruction_table[0x99] = &CPU::op_99;
    instruction_table[0xDE] = &CPU::op_DE;
    instruction_table[0x2E] = &CPU::op_2E;
    instruction_table[0x70] = &CPU::op_70;
    instruction_table[0x02] = &CPU::op_02;
    instruction_table[0x76] = &CPU::op_76;
    instruction_table[0xC4] = &CPU::op_C4;
    instruction_table[0xCF] = &CPU::op_CF;
    instruction_table[0x48] = &CPU::op_48;
    instruction_table[0xDF] = &CPU::op_DF;
    instruction_table[0x81] = &CPU::op_81;
    instruction_table[0x83] = &CPU::op_83;
    instruction_table[0x93] = &CPU::op_93;
    instruction_table[0xD2] = &CPU::op_D2;
    instruction_table[0x44] = &CPU::op_44;
    instruction_table[0x43] = &CPU::op_43;
    instruction_table[0x4D] = &CPU::op_4D;
    instruction_table[0x91] = &CPU::op_91;
    instruction_table[0x4B] = &CPU::op_4B;
    instruction_table[0xA6] = &CPU::op_A6;
    instruction_table[0xCC] = &CPU::op_CC;
    instruction_table[0xB6] = &CPU::op_B6;
    instruction_table[0xDA] = &CPU::op_DA;
    instruction_table[0xB5] = &CPU::op_B5;
    // Yeni bir komut yazdıkça buraya ekleyeceksin: instruction_table[0xAF] = &CPU::op_AF; vb.
    cbinstruction_table[0x37] = &CPU::op_cb37;
    cbinstruction_table[0x87] = &CPU::op_cb87;
    cbinstruction_table[0x27] = &CPU::op_cb27;
    cbinstruction_table[0x7F] = &CPU::op_cb7F;
    cbinstruction_table[0x86] = &CPU::op_cb86;
    cbinstruction_table[0x50] = &CPU::op_cb50;
    cbinstruction_table[0x60] = &CPU::op_cb60;
    cbinstruction_table[0x68] = &CPU::op_cb68;
    cbinstruction_table[0x58] = &CPU::op_cb58;
    cbinstruction_table[0x7E] = &CPU::op_cb7E;
    cbinstruction_table[0x3F] = &CPU::op_cb3F;
    cbinstruction_table[0x40] = &CPU::op_cb40;
    cbinstruction_table[0x5F] = &CPU::op_cb5F;
    cbinstruction_table[0x33] = &CPU::op_cb33;
    cbinstruction_table[0x77] = &CPU::op_cb77;
    cbinstruction_table[0x6F] = &CPU::op_cb6F;
    cbinstruction_table[0x48] = &CPU::op_cb48;
    cbinstruction_table[0x61] = &CPU::op_cb61;
    cbinstruction_table[0x69] = &CPU::op_cb69;
    cbinstruction_table[0x47] = &CPU::op_cb47;
    cbinstruction_table[0xFE] = &CPU::op_cbFE;
    cbinstruction_table[0xBE] = &CPU::op_cbBE;
    cbinstruction_table[0x46] = &CPU::op_cb46;
    cbinstruction_table[0x70] = &CPU::op_cb70;
    cbinstruction_table[0x78] = &CPU::op_cb78;
    cbinstruction_table[0x41] = &CPU::op_cb41;
    cbinstruction_table[0x57] = &CPU::op_cb57;
    cbinstruction_table[0xDE] = &CPU::op_cbDE;
    cbinstruction_table[0x9E] = &CPU::op_cb9E;
    cbinstruction_table[0xD8] = &CPU::op_cbD8;
    cbinstruction_table[0xF8] = &CPU::op_cbF8;
    cbinstruction_table[0x71] = &CPU::op_cb71;
    cbinstruction_table[0x79] = &CPU::op_cb79;
    cbinstruction_table[0xD0] = &CPU::op_cbD0;
    cbinstruction_table[0xF0] = &CPU::op_cbF0;
    cbinstruction_table[0x6E] = &CPU::op_cb6E;
    cbinstruction_table[0x76] = &CPU::op_cb76;
    cbinstruction_table[0x4F] = &CPU::op_cb4F;
    cbinstruction_table[0xC6] = &CPU::op_cbC6;
    cbinstruction_table[0x26] = &CPU::op_cb26;
    cbinstruction_table[0xC7] = &CPU::op_cbC7;
    cbinstruction_table[0xD6] = &CPU::op_cbD6;
    cbinstruction_table[0xCF] = &CPU::op_cbCF;
    cbinstruction_table[0xCE] = &CPU::op_cbCE;
    cbinstruction_table[0xD7] = &CPU::op_cbD7;
    cbinstruction_table[0x96] = &CPU::op_cb96;
    cbinstruction_table[0xEE] = &CPU::op_cbEE;
    cbinstruction_table[0xAE] = &CPU::op_cbAE;
    cbinstruction_table[0x7D] = &CPU::op_cb7D;
}

uint8_t CPU::fetch()
{
    return mmu_bus.readByte(PC++);
}

uint16_t CPU::fetch16()
{
    // Game Boy Little-Endian çalıştığı için önce alt bayt (low), sonra üst bayt (high) okunur
    uint16_t lo = fetch();
    uint16_t hi = fetch();
    return (hi << 8) | lo;
}

uint8_t CPU::step()
{

    if (is_halted)
    {
        uint8_t ie = mmu_bus.readByte(0xFFFF);
        uint8_t _if = mmu_bus.readByte(0xFF0F);

        if ((ie & _if & 0x1F) > 0)
        {
            is_halted = false;
        }
        else
        {
            return 1;
        }
    }
    handle_interrupts();

    uint8_t opcode = fetch();
    uint8_t cycles_used = 0;

    if (opcode == 0xCB)
    {
        uint8_t cb_opcode = fetch();
        // if (HL >= 0xC0A0 && HL <= 0xC0A4)
        // {
        //     printf("SKOR ISLEMI -> PC: %04X | Opcode: %02X | A:%02X F:%02X HL:%04X\n", PC, opcode, A, F, HL);
        // }
        cycles_used = (this->*cbinstruction_table[cb_opcode])();
    }
    else
    {
        // if (HL >= 0xC0A0 && HL <= 0xC0A4)
        // {
        //     printf("SKOR ISLEMI -> PC: %04X | Opcode: %02X | A:%02X F:%02X HL:%04X\n", PC, opcode, A, F, HL);
        // }
        cycles_used = (this->*instruction_table[opcode])();
    }

    return cycles_used;
}

uint8_t CPU::unimplemented_opcode()
{
    // Program Counter (PC) komutu okurken 1 arttığı için hatanın nerede olduğunu bulmak adına PC-1 değerine bakıyoruz.
    uint16_t error_pc = PC - 1;
    uint8_t opcode = mmu_bus.readByte(error_pc);
    if (mmu_bus.readByte(error_pc - 1) == 0xCB)
        printf("CB instruction\n");
    else
        printf("Normal instruction\n");

    std::cerr << "HATA: Henuz yazilmamis opcode okundu!\n"
              << "Opcode: 0x" << std::hex << std::uppercase << (int)opcode << "\n"
              << "PC Adresi: 0x" << std::setw(4) << std::setfill('0') << error_pc << "\n";

    // Emülatörü acil durdur
    throw std::runtime_error("Islemci bilinmeyen bir komutla karsilasti.");
}

void CPU::handle_interrupts()
{
    if (!IME)
        return;

    uint8_t ie = mmu_bus.readByte(0xFFFF);
    uint8_t _if = mmu_bus.readByte(0xFF0F);

    uint8_t pending_interrupts = ie & _if & 0x1F;

    if (pending_interrupts > 0)
    {

        IME = false;
        push_16bit(PC);

        if (pending_interrupts & 0x01)
        {
            mmu_bus.writeByte(0xFF0F, _if & ~0x01);
            PC = 0x0040;
        }
        else if (pending_interrupts & 0x02)
        {
            mmu_bus.writeByte(0xFF0F, _if & ~0x02);
            PC = 0x0048;
        }
        else if (pending_interrupts & 0x04)
        {
            mmu_bus.writeByte(0xFF0F, _if & ~0x04);
            PC = 0x0050;
        }
        else if (pending_interrupts & 0x08)
        {
            mmu_bus.writeByte(0xFF0F, _if & ~0x08);
            PC = 0x0058;
        }
        else if (pending_interrupts & 0x10)
        {
            mmu_bus.writeByte(0xFF0F, _if & ~0x10);
            PC = 0x0060;
        }
    }
}

// --- KOMUT İMPLEMENTASYONLARI ---

uint8_t CPU::op_76()
{
    is_halted = true;
    return 1;
}

uint8_t CPU::op_00()
{
    // 0x00: NOP (No Operation)
    // Hiçbir şey yapmaz, işlemci sadece birkaç saat döngüsü harcar.
    return 1;
}

uint8_t CPU::op_C3()
{
    // 0xC3: JP a16 (Jump to 16-bit address)
    uint16_t address = fetch16();
    PC = address; // Program Counter'ı doğrudan yeni adrese eşitle
    return 4;
}

uint8_t CPU::op_AF()
{
    xor_reg(A);
    return 2;
}

uint8_t CPU::op_21()
{
    uint16_t data = fetch16();
    HL = data;
    return 3;
}

uint8_t CPU::op_0E()
{
    C = fetch();
    return 2;
}

uint8_t CPU::op_06()
{
    B = fetch();
    return 2;
}

uint8_t CPU::op_32()
{

    mmu_bus.writeByte(HL, A);
    HL--;
    return 2;
}

uint8_t CPU::op_05()
{
    dec_8bit(B);
    return 1;
}

uint8_t CPU::op_20()
{
    int8_t len = (int8_t)fetch();

    if (!(F & FLAG_Z))
    {
        PC += len;
        return 3;
    }
    return 2;
}

uint8_t CPU::op_0D()
{
    dec_8bit(C);
    return 1;
}

uint8_t CPU::op_3E()
{
    A = fetch();
    return 2;
}

uint8_t CPU::op_F3()
{
    IME = false;
    return 1;
}

uint8_t CPU::op_E0()
{
    uint8_t offset = fetch();
    mmu_bus.writeByte(0xFF00 + offset, A);
    return 3;
}

uint8_t CPU::op_F0()
{
    uint8_t offset = fetch();
    A = mmu_bus.readByte(0xFF00 + offset);
    return 3;
}

uint8_t CPU::op_FE()
{
    uint8_t d8 = fetch();
    cp_8bit(d8);
    return 2;
}

uint8_t CPU::op_36()
{
    uint8_t d8 = fetch();
    mmu_bus.writeByte(HL, d8);
    return 3;
}

uint8_t CPU::op_EA()
{
    uint16_t addr = fetch16();
    mmu_bus.writeByte(addr, A);
    return 4;
}

uint8_t CPU::op_31()
{
    uint16_t d16 = fetch16();
    SP = d16;
    return 3;
}

uint8_t CPU::op_2A()
{
    A = mmu_bus.readByte(HL);
    HL++;
    return 2;
}

uint8_t CPU::op_E2()
{
    mmu_bus.writeByte(0XFF00 + C, A);
    return 2;
}

uint8_t CPU::op_0C()
{
    inc_8bit(C);
    return 1;
}

uint8_t CPU::op_CD()
{
    uint16_t calladdr = fetch16();
    push_16bit(PC);
    PC = calladdr;
    return 6;
}

uint8_t CPU::op_01()
{
    uint16_t d16 = fetch16();
    BC = d16;
    return 3;
}

uint8_t CPU::op_0B()
{
    BC--;
    return 2;
}

uint8_t CPU::op_78()
{
    A = B;
    return 1;
}

uint8_t CPU::op_B1()
{
    or_reg(C);
    return 1;
}

uint8_t CPU::op_C9()
{
    pop_16bit(PC);
    return 4;
}

uint8_t CPU::op_FB()
{
    // ileride 1 cyclelık gecikme eklenmeli diğer oyunlar için(tetriste sorun yok)
    IME = true;
    return 1;
}

uint8_t CPU::op_2F()
{
    A = ~A;
    F |= FLAG_N;
    F |= FLAG_H;

    return 1;
}

uint8_t CPU::op_E6()
{
    uint8_t d8 = fetch();
    and_reg(d8);
    return 2;
}

uint8_t CPU::op_47()
{
    B = A;
    return 1;
}

uint8_t CPU::op_B0()
{
    or_reg(B);
    return 1;
}

uint8_t CPU::op_4F()
{
    C = A;
    return 1;
}

uint8_t CPU::op_A9()
{
    xor_reg(C);
    return 1;
}

uint8_t CPU::op_A1()
{
    and_reg(C);
    return 1;
}

uint8_t CPU::op_79()
{
    A = C;
    return 1;
}

uint8_t CPU::op_EF()
{
    push_16bit(PC);
    PC = 0x28;
    return 4;
}

uint8_t CPU::op_87()
{
    add_8bit(A);
    return 1;
}

uint8_t CPU::op_E1()
{
    pop_16bit(HL);
    return 3;
}

uint8_t CPU::op_5F()
{
    E = A;
    return 1;
}

uint8_t CPU::op_16()
{
    D = fetch();
    return 2;
}

uint8_t CPU::op_19()
{
    add_hl(DE);
    return 2;
}

uint8_t CPU::op_5E()
{
    E = mmu_bus.readByte(HL);
    return 2;
}

uint8_t CPU::op_23()
{
    HL++;
    return 2;
}

uint8_t CPU::op_56()
{
    D = mmu_bus.readByte(HL);
    return 2;
}

uint8_t CPU::op_D5()
{
    push_16bit(DE);
    return 4;
}

uint8_t CPU::op_E9()
{
    PC = HL;
    return 1;
}

uint8_t CPU::op_11()
{
    DE = fetch16();
    return 3;
}

uint8_t CPU::op_12()
{
    mmu_bus.writeByte(DE, A);
    return 2;
}

uint8_t CPU::op_13()
{
    DE++;
    return 2;
}

uint8_t CPU::op_E5()
{
    push_16bit(HL);
    return 4;
}

uint8_t CPU::op_1A()
{
    A = mmu_bus.readByte(DE);
    return 2;
}

uint8_t CPU::op_22()
{
    mmu_bus.writeByte(HL, A);
    HL++;
    return 2;
}

uint8_t CPU::op_D1()
{
    pop_16bit(DE);
    return 3;
}

uint8_t CPU::op_7C()
{
    A = H;
    return 1;
}

uint8_t CPU::op_F5()
{
    F &= 0xF0;
    push_16bit(AF);
    return 4;
}

uint8_t CPU::op_C5()
{
    push_16bit(BC);
    return 4;
}

uint8_t CPU::op_FA()
{
    A = mmu_bus.readByte(fetch16());
    return 4;
}

uint8_t CPU::op_28()
{
    int8_t len = (int8_t)fetch();

    if (F & FLAG_Z)
    {
        PC += len;
        return 3;
    }
    return 2;
}

uint8_t CPU::op_A7()
{
    and_reg(A);
    return 1;
}

uint8_t CPU::op_35()
{
    uint8_t value = mmu_bus.readByte(HL);
    dec_8bit(value);
    mmu_bus.writeByte(HL, value);
    return 3;
}

uint8_t CPU::op_7E()
{
    A = mmu_bus.readByte(HL);
    return 2;
}

uint8_t CPU::op_34()
{
    uint8_t value = mmu_bus.readByte(HL);
    inc_8bit(value);
    mmu_bus.writeByte(HL, value);
    return 3;
}

uint8_t CPU::op_18()
{
    int8_t len = (int8_t)fetch();
    PC += len;
    return 3;
}

uint8_t CPU::op_C1()
{
    pop_16bit(BC);
    return 3;
}

uint8_t CPU::op_F1()
{
    pop_16bit(AF);
    F &= 0xF0;
    return 3;
}

uint8_t CPU::op_CA()
{
    uint16_t addr = fetch16();
    if (F & FLAG_Z)
    {
        PC = addr;
        return 4;
    }
    return 3;
}

uint8_t CPU::op_C0()
{
    if (!(F & FLAG_Z))
    {
        pop_16bit(PC);
        return 5;
    }
    return 2;
}

uint8_t CPU::op_C8()
{
    if (F & FLAG_Z)
    {
        pop_16bit(PC);
        return 5;
    }
    return 2;
}

uint8_t CPU::op_37()
{
    F &= ~FLAG_H;
    F &= ~FLAG_N;
    F |= FLAG_C;
    return 1;
}

uint8_t CPU::op_1C()
{
    inc_8bit(E);
    return 1;
}

uint8_t CPU::op_24()
{
    inc_8bit(H);
    return 1;
}

uint8_t CPU::op_09()
{
    add_hl(BC);
    return 2;
}

uint8_t CPU::op_3D()
{
    dec_8bit(A);
    return 1;
}

uint8_t CPU::op_3C()
{
    inc_8bit(A);
    return 1;
}

uint8_t CPU::op_D9()
{
    pop_16bit(PC);
    IME = true;
    return 4;
}

uint8_t CPU::op_2C()
{
    inc_8bit(L);
    return 1;
}

uint8_t CPU::op_77()
{
    mmu_bus.writeByte(HL, A);
    return 2;
}

uint8_t CPU::op_4E()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    C = d8;
    return 2;
}

uint8_t CPU::op_46()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    B = d8;
    return 2;
}

uint8_t CPU::op_69()
{
    L = C;
    return 1;
}

uint8_t CPU::op_60()
{
    H = B;
    return 1;
}

uint8_t CPU::op_0A()
{
    uint16_t bc_address = (B << 8) | C;

    A = mmu_bus.readByte(bc_address);

    return 2;
}

uint8_t CPU::op_03()
{
    BC++;
    return 2;
}

uint8_t CPU::op_85()
{
    add_8bit(L);
    return 1;
}

uint8_t CPU::op_6F()
{
    L = A;
    return 1;
}

uint8_t CPU::op_C2()
{
    uint16_t addr = fetch16();
    if (!(F & FLAG_Z))
    {

        PC = addr;
        return 4;
    }

    return 3;
}

uint8_t CPU::op_3A()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    HL--;
    A = d8;
    return 2;
}

uint8_t CPU::op_57()
{
    D = A;
    return 1;
}

uint8_t CPU::op_7B()
{
    A = E;
    return 1;
}

uint8_t CPU::op_7A()
{
    A = D;
    return 1;
}

uint8_t CPU::op_73()
{
    mmu_bus.writeByte(HL, E);
    return 2;
}

uint8_t CPU::op_72()
{
    mmu_bus.writeByte(HL, D);
    return 2;
}

uint8_t CPU::op_71()
{
    mmu_bus.writeByte(HL, C);
    return 2;
}

uint8_t CPU::op_2D()
{
    dec_8bit(L);
    return 2;
}

uint8_t CPU::op_67()
{
    H = A;
    return 1;
}

uint8_t CPU::op_7D()
{
    A = L;
    return 1;
}

uint8_t CPU::op_C6()
{
    uint8_t d8 = fetch();
    add_8bit(d8);
    return 2;
}

uint8_t CPU::op_5D()
{
    E = L;
    return 1;
}

uint8_t CPU::op_54()
{
    D = H;
    return 1;
}

uint8_t CPU::op_F6()
{
    uint8_t d8 = fetch();
    or_reg(d8);
    return 2;
}

uint8_t CPU::op_6B()
{
    L = E;
    return 1;
}

uint8_t CPU::op_62()
{
    H = D;
    return 1;
}

uint8_t CPU::op_40()
{
    B = B;
    return 1;
}

uint8_t CPU::op_1E()
{
    uint8_t d8 = fetch();
    E = d8;
    return 2;
}

uint8_t CPU::op_26()
{
    H = fetch();
    return 2;
}

uint8_t CPU::op_07()
{
    uint8_t bit7 = (A & 0x80) >> 7;
    A = (A << 1) | bit7;
    F = 0;
    if (bit7)
        F |= FLAG_Z;

    return 1;
}

uint8_t CPU::op_80()
{
    add_8bit(B);
    return 1;
}

uint8_t CPU::op_89()
{
    adc_8bit(C);
    return 1;
}

uint8_t CPU::op_B8()
{
    cp_8bit(B);
    return 1;
}

uint8_t CPU::op_A8()
{
    xor_reg(B);
    return 1;
}

uint8_t CPU::op_A0()
{
    and_reg(B);
    return 1;
}

uint8_t CPU::op_D6()
{
    uint8_t d8 = fetch();
    sub_8bit(d8);
    return 2;
}

uint8_t CPU::op_27()
{
    uint8_t a = A;
    uint8_t adjust = 0;

    if (!(F & FLAG_N))
    {

        if ((F & FLAG_C) || (a > 0x99))
        {
            adjust |= 0x60;
            F |= FLAG_C;
        }

        if ((F & FLAG_H) || ((a & 0x0F) > 0x09))
        {
            adjust |= 0x06;
        }

        A += adjust;
    }
    else
    {

        if (F & FLAG_C)
        {
            A -= 0x60;
        }
        if (F & FLAG_H)
        {
            A -= 0x06;
        }
    }

    if (A == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_H;

    return 1;
}

uint8_t CPU::op_86()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    add_8bit(d8);
    return 2;
}

uint8_t CPU::op_8E()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    adc_8bit(d8);
    return 2;
}

uint8_t CPU::op_D0()
{

    if (!(F & FLAG_C))
    {
        pop_16bit(PC);
        return 5;
    }
    return 2;
}

uint8_t CPU::op_1D()
{
    dec_8bit(E);
    return 1;
}

uint8_t CPU::op_B9()
{
    cp_8bit(C);
    return 1;
}

uint8_t CPU::op_30()
{
    int8_t r8 = (int8_t)fetch();

    if (!(F & FLAG_C))
    {
        PC += r8;
        return 3;
    }

    return 2;
}

uint8_t CPU::op_90()
{
    sub_8bit(B);
    return 1;
}

uint8_t CPU::op_61()
{
    H = C;
    return 1;
}

uint8_t CPU::op_04()
{
    inc_8bit(B);
    return 1;
}

uint8_t CPU::op_D8()
{
    if (F & FLAG_C)
    {
        pop_16bit(PC);
        return 5;
    }

    return 2;
}

uint8_t CPU::op_1B()
{
    DE--;
    return 2;
}

uint8_t CPU::op_B7()
{
    or_reg(A);
    return 1;
}

uint8_t CPU::op_38()
{
    int8_t s8 = (int8_t)fetch();
    if (F & FLAG_C)
    {
        PC += s8;
        return 3;
    }
    return 2;
}

uint8_t CPU::op_BE()
{
    // 1. HL'nin gösterdiği adresteki veriyi oku
    uint8_t val = mmu_bus.readByte(HL);

    // 2. Z Bayrağı (Sıfır): Değerler eşitse (A - val == 0)
    if (A == val)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    // 3. N Bayrağı (Çıkarma): Her zaman 1 (Set) olmalı
    F |= FLAG_N;

    // 4. H Bayrağı (Half-Carry): Alt 4 bitte (Nibble) çıkarma borcu var mı?
    if ((A & 0x0F) < (val & 0x0F))
        F |= FLAG_H;
    else
        F &= ~FLAG_H;

    // 5. C Bayrağı (Carry): A değeri, okunan değerden küçük mü? (Borç alındı mı?)
    if (A < val)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    // 6. Bu komut belleğe eriştiği için 2 Machine Cycle (8 T-Cycle) harcar
    return 2;
}

uint8_t CPU::op_EE()
{
    // 1. Sonraki baytı (d8) bellekten oku
    uint8_t d8 = fetch();

    // 2. A yazmacı ile okunan değeri XOR'la ve sonucu A'ya yaz
    A = A ^ d8;

    // 3. Bayrakları (Flags) güncelle
    if (A == 0)
        F |= FLAG_Z; // Sonuç sıfırsa Z bayrağı 1
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N; // N her zaman 0
    F &= ~FLAG_H; // H her zaman 0
    F &= ~FLAG_C; // C her zaman 0

    // 4. Döndürülen cycle (2) DOĞRU! (8 T-Cycle)
    return 2;
}

uint8_t CPU::op_96()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    sub_8bit(d8);
    return 2;
}

uint8_t CPU::op_2B()
{
    HL--;
    return 2;
}

uint8_t CPU::op_82()
{
    add_8bit(D);
    return 1;
}

uint8_t CPU::op_25()
{
    dec_8bit(H);
    return 1;
}

uint8_t CPU::op_B2()
{
    or_reg(D);
    return 1;
}

uint8_t CPU::op_99()
{
    sbc_8bit(C);
    return 1;
}

uint8_t CPU::op_DE()
{
    sbc_8bit(fetch());
    return 1;
}

uint8_t CPU::op_2E()
{
    L = fetch();
    return 2;
}

uint8_t CPU::op_70()
{
    mmu_bus.writeByte(HL, B);
    return 2;
}

uint8_t CPU::op_02()
{
    mmu_bus.writeByte(BC, A);
    return 2;
}

uint8_t CPU::op_C4()
{
    uint16_t addr = fetch16();
    if (!(F & FLAG_Z))
    {
        push_16bit(PC);
        PC = addr;
        return 6;
    }

    return 3;
}

uint8_t CPU::op_CF()
{
    push_16bit(PC);
    PC = 0x0008;
    return 4;
}

uint8_t CPU::op_48()
{
    C = B;
    return 1;
}

uint8_t CPU::op_DF() // RST 18h
{
    push_16bit(PC);
    PC = 0x0018;
    return 4;
}

uint8_t CPU::op_81()
{
    add_8bit(C);
    return 1;
}

uint8_t CPU::op_83()
{
    add_8bit(E);
    return 1;
}

uint8_t CPU::op_93()
{
    sub_8bit(E);
    return 1;
}

uint8_t CPU::op_D2() // JP NC, a16
{
    // 16-bit hedef adresi oku ve PC'yi 2 artır
    uint16_t addr = fetch16();

    if (!(F & FLAG_C))
    {
        PC = addr;
        return 4;
    }

    return 3;
}

uint8_t CPU::op_44()
{
    B = H;
    return 1;
}

uint8_t CPU::op_43()
{
    B = E;
    return 1;
}

uint8_t CPU::op_4D()
{
    L = C;
    return 1;
}

uint8_t CPU::op_4B()
{
    C = E;
    return 1;
}

uint8_t CPU::op_91()
{
    sub_8bit(C);
    return 1;
}

uint8_t CPU::op_A6()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    and_reg(d8);
    return 2;
}

uint8_t CPU::op_CC()
{

    uint16_t addr = fetch16();

    if (F & FLAG_Z)
    {
        push_16bit(PC);
        PC = addr;
        return 6;
    }

    return 3; // Koşul sağlanmazsa 3 Machine Cycles (12 T-Cycles)
}

uint8_t CPU::op_B6()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    or_reg(d8);
    return 2;
}

uint8_t CPU::op_DA() // JP C, a16
{

    uint16_t addr = fetch16();

    if (F & FLAG_C)
    {
        PC = addr;
        return 4;
    }

    return 3;
}

uint8_t CPU::op_B5()
{
    or_reg(L);
    return 1;
}

//
void CPU::inc_8bit(uint8_t &reg)
{
    if ((reg & 0x0F) == 0x0F)
        F |= FLAG_H;
    else
        F &= ~FLAG_H;

    reg++;

    if (reg == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
}

void CPU::dec_8bit(uint8_t &reg)
{
    F |= FLAG_N;

    if ((reg & 0x0F) == 0)
        F |= FLAG_H;
    else
        F &= ~FLAG_H;

    reg--; // Çıkarma işlemi

    if (reg == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;
}

void CPU::and_reg(uint8_t value)
{
    A &= value;
    F = 0;
    if (A == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F |= FLAG_H;
}

void CPU::or_reg(uint8_t value)
{
    A |= value;
    F = 0;
    if (A == 0)
        F |= FLAG_Z;
}

void CPU::xor_reg(uint8_t value)
{
    A ^= value;
    F = 0;
    if (A == 0)
        F |= FLAG_Z;
}

void CPU::push_16bit(uint16_t value)
{
    SP--;
    mmu_bus.writeByte(SP, ((value >> 8) & 0xFF));
    SP--;
    mmu_bus.writeByte(SP, (value & 0xFF));
}

void CPU::pop_16bit(uint16_t &reg)
{
    uint8_t lower_byte = mmu_bus.readByte(SP);
    SP++;
    uint8_t higher_byte = mmu_bus.readByte(SP);
    SP++;

    reg = (higher_byte << 8) | lower_byte;
}

void CPU::add_8bit(uint8_t value)
{
    uint16_t res = A + value;

    if ((res & 0xFF) == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;
    F &= ~FLAG_N;
    if (((A & 0x0F) + (value & 0x0F)) > 0x0F)
        F |= FLAG_H;
    else
        F &= ~FLAG_H;
    if (res > 0xFF)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    A = res & 0xFF;
}

void CPU::cp_8bit(uint8_t value)
{
    if (A == value)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;
    F |= FLAG_N;
    if ((A & 0x0F) < (value & 0x0F))
        F |= FLAG_H;
    else
        F &= ~FLAG_H;
    if (A < value)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;
}

void CPU::add_hl(uint16_t value)
{
    uint32_t res = HL + value;

    F &= ~FLAG_N;
    if ((HL & 0x0FFF) + (value & 0x0FFF) > 0x0FFF)
        F |= FLAG_H;
    else
        F &= ~FLAG_H;
    if (res > 0xFFFF)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    HL = res & 0xFFFF;
}

void CPU::adc_8bit(uint8_t val)
{
    uint8_t carry = (F & FLAG_C) ? 1 : 0;

    uint16_t result = A + val + carry;

    F = 0;

    if ((result & 0xFF) == 0)
        F |= FLAG_Z;

    if (((A & 0x0F) + (val & 0x0F) + carry) > 0x0F)
        F |= FLAG_H;

    if (result > 0xFF)
        F |= FLAG_C;

    A = result & 0xFF;
}

void CPU::sub_8bit(uint8_t value)
{
    if (A == value)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F |= FLAG_N;

    if ((A & 0x0F) < (value & 0x0F))
        F |= FLAG_H;
    else
        F &= ~FLAG_H;

    if (A < value)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    A = A - value;
}

void CPU::sbc_8bit(uint8_t val)
{
    // O anki Carry (Elde/Borç) durumunu al
    uint8_t carry = (F & FLAG_C) ? 1 : 0;

    // Matematiksel sonucu hesapla (Eksi değerlere düşebilmesi için int kullanıyoruz)
    int result = A - val - carry;

    // H (Half-Carry): Alt 4 bitten borç alındı mı?
    if (((A & 0x0F) - (val & 0x0F) - carry) < 0)
        F |= FLAG_H;
    else
        F &= ~FLAG_H;

    // C (Carry): 8. bitten borç alındı mı? (Yani sonuç 0'dan küçük mü?)
    if (result < 0)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    // A yazmacını güncelle
    A = result & 0xFF;

    // Z (Zero): Sonuç sıfırsa 1 yap
    if (A == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    // N (Subtract): Çıkarma işlemi olduğu için HER ZAMAN 1
    F |= FLAG_N;
}

//
uint8_t CPU::op_cb37()
{
    uint8_t lowernibble = A & 0x0F;
    uint8_t highernibbe = A & 0xF0;

    A = (lowernibble << 4) | (highernibbe >> 4);

    F = 0;
    if (A == 0)
        F |= FLAG_Z;

    return 2;
}

uint8_t CPU::op_cb87()
{
    A &= 0xFE;
    return 2;
}

uint8_t CPU::op_cb27()
{
    uint8_t bit7 = (A & 0x80) >> 7;

    A = A << 1;

    if (A == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F &= ~FLAG_H;

    if (bit7)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    return 2;
}

uint8_t CPU::op_cb7F()
{
    uint8_t bit7 = A & 0x80;
    if (!bit7)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;

    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb86()
{
    uint8_t new_byte = mmu_bus.readByte(HL) & 0xFE;
    mmu_bus.writeByte(HL, new_byte);
    return 4;
}

uint8_t CPU::op_cb50()
{
    uint8_t bit2 = B & 0x04;
    if (!bit2)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 2;
}

uint8_t CPU::op_cb60()
{
    uint8_t bit4 = B & 0x10;
    if (!bit4)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 2;
}

uint8_t CPU::op_cb68()
{
    uint8_t bit5 = B & 0x20;
    if (!bit5)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 2;
}

uint8_t CPU::op_cb58()
{
    uint8_t bit3 = B & 0x8;
    if (!bit3)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 2;
}

uint8_t CPU::op_cb7E()
{
    uint8_t bit7 = mmu_bus.readByte(HL) & 0x80;
    if (!bit7)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 3;
}

uint8_t CPU::op_cb3F()
{
    if (A & 0x01)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    A >>= 1;

    if (A == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F &= ~FLAG_H;

    return 2;
}

uint8_t CPU::op_cb40()
{
    uint8_t bit0 = B & 0x01;

    if (!bit0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb5F()
{
    uint8_t bit3 = A & 0x08;

    if (!bit3)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb33()
{
    F = 0;
    uint8_t lower_nibble = E & 0x0F;
    uint8_t higher_nibble = E & 0xF0;

    E = (lower_nibble << 4) | (higher_nibble >> 4);

    if (E == 0)
        F |= FLAG_Z;

    return 2;
}

uint8_t CPU::op_cb77()
{
    uint8_t bit6 = A & 0x40;

    if (!bit6)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb6F()
{
    uint8_t bit5 = A & 0x20;

    if (!bit5)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb48()
{
    uint8_t bit1 = B & 0x01;

    if (!bit1)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb61()
{
    uint8_t bit4 = C & 0x10;

    if (!bit4)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb69()
{
    uint8_t bit5 = C & 0x20;

    if (!bit5)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb47()
{
    uint8_t bit0 = A & 0x01;
    if (!bit0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 2;
}

uint8_t CPU::op_cbFE()
{
    uint8_t d8 = mmu_bus.readByte(HL) | 0x80;
    mmu_bus.writeByte(HL, d8);
    return 4;
}

uint8_t CPU::op_cbBE()
{
    // 1. HL adresindeki veriyi oku
    uint8_t val = mmu_bus.readByte(HL);

    // 2. 7. Biti SIFIRLA (0x80'in tersi yani 0x7F ile AND işlemi yaparak)
    val = val & 0x7F; // (val &= ~0x80; şeklinde de yazabilirsin, aynı şeydir)

    // 3. Veriyi HL adresine geri yaz
    mmu_bus.writeByte(HL, val);

    // 4. Bellekten okuyup tekrar belleğe yazdığı için 4 cycle (16 T-Cycle) harcar
    return 4;
}

uint8_t CPU::op_cb46()
{
    // 1. HL adresindeki veriyi oku
    uint8_t val = mmu_bus.readByte(HL);

    // 2. 0. Bit (0x01) 0 mı diye kontrol et
    if ((val & 0x01) == 0)
        F |= FLAG_Z; // Bit 0'ın değeri 0 ise Z bayrağını 1 yap
    else
        F &= ~FLAG_Z; // Değilse Z bayrağını 0 yap

    F &= ~FLAG_N; // N bayrağını 0 yap
    F |= FLAG_H;  // H bayrağını her zaman 1 yap (BIT komutları kuralı)
    // C bayrağına (Carry) ASLA dokunulmaz!

    // 3. Bellekten okuma yaptığı için 3 Machine Cycle (12 T-Cycle) harcar
    return 3;
}

uint8_t CPU::op_cb70()
{
    uint8_t bit6 = B & 0x40;

    if (!bit6)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb78()
{
    uint8_t bit7 = B & 0x80;

    if (!bit7)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb41()
{
    uint8_t bit0 = C & 0x01;

    if (!bit0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb57()
{
    uint8_t bit2 = A & 0x04;

    if (!bit2)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cbDE()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    mmu_bus.writeByte(HL, (d8 | 0x08));
    return 4;
}

uint8_t CPU::op_cb9E()
{
    uint8_t d8 = mmu_bus.readByte(HL);
    mmu_bus.writeByte(HL, (d8 & 0xF7));
    return 4;
}

uint8_t CPU::op_cbD8()
{
    B |= 0x08;
    return 2;
}

uint8_t CPU::op_cbF8()
{
    B |= 0x80;
    return 2;
}

uint8_t CPU::op_cb71()
{
    uint8_t bit6 = C & 0x40;

    if (!bit6)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cb79()
{
    uint8_t bit7 = C & 0x80;

    if (!bit7)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}

uint8_t CPU::op_cbD0()
{
    B |= 0x04;
    return 2;
}

uint8_t CPU::op_cbF0()
{
    B |= 0x40;
    return 2;
}

uint8_t CPU::op_cb6E() // BIT 5, (HL)
{
    uint8_t value = mmu_bus.readByte(HL);

    if (value & (1 << 5))
        F &= ~FLAG_Z;
    else
        F |= FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 3;
}

uint8_t CPU::op_cb76()
{
    uint8_t value = mmu_bus.readByte(HL);

    if (value & (1 << 6))
        F &= ~FLAG_Z;
    else
        F |= FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 3;
}

uint8_t CPU::op_cb4F()
{

    uint8_t val = A | 0x02;

    if (val)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;
    return 2;
}

uint8_t CPU::op_cbC6()
{
    uint8_t val = mmu_bus.readByte(HL);
    val |= (1 << 0);
    mmu_bus.writeByte(HL, val);

    return 4;
}

uint8_t CPU::op_cb26()
{
    uint8_t val = mmu_bus.readByte(HL);

    if (val & 0x80)
        F |= FLAG_C;
    else
        F &= ~FLAG_C;

    val <<= 1;

    if (val == 0)
        F |= FLAG_Z;
    else
        F &= ~FLAG_Z;

    F &= ~FLAG_N;
    F &= ~FLAG_H;

    mmu_bus.writeByte(HL, val);

    return 4;
}

uint8_t CPU::op_cbC7() // SET 0, A
{
    A |= (1 << 0);
    return 2;
}

uint8_t CPU::op_cbD6()
{
    uint8_t val = mmu_bus.readByte(HL);
    val |= (1 << 2);
    mmu_bus.writeByte(HL, val);

    return 4;
}

uint8_t CPU::op_cbCF()
{
    A |= (1 << 1);
    return 2;
}

uint8_t CPU::op_cbCE()
{
    // SET 1, (HL)
    uint8_t value = mmu_bus.readByte(HL);

    value |= (1 << 1);

    mmu_bus.writeByte(HL, value);

    return 4;
}

uint8_t CPU::op_cbD7() // SET 2, A
{
    A |= (1 << 2);
    return 2;
}

uint8_t CPU::op_cb96()
{
    uint8_t val = mmu_bus.readByte(HL);
    val &= ~(1 << 2);
    mmu_bus.writeByte(HL, val);

    return 4;
}

uint8_t CPU::op_cbEE()
{
    uint8_t val = mmu_bus.readByte(HL);
    val |= (1 << 5);
    mmu_bus.writeByte(HL, val);

    return 4;
}

uint8_t CPU::op_cbAE()
{
    uint8_t val = mmu_bus.readByte(HL);
    val &= ~(1 << 5);
    mmu_bus.writeByte(HL, val);

    return 4;
}

uint8_t CPU::op_cb7D()
{
    if (L & (1 << 7))
        F &= ~FLAG_Z;
    else
        F |= FLAG_Z;

    F &= ~FLAG_N;
    F |= FLAG_H;

    return 2;
}
//
