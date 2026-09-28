# C++ Game Boy Emulator (Linux)

Linux ortamında C++ ile sıfırdan geliştirilmiş minimalist bir Game Boy (LR35902) emülatörüdür. Bu proje, genel amaçlı tam kapsamlı bir emülatör geliştirmekten ziyade; **Tetris** ve **Dr. Mario** gibi klasik ROM'ları başarıyla ayağa kaldırmak ve emülasyon mimarisini (CPU, PPU, Timer, Memory Bus) derinlemesine öğrenmek amacıyla tasarlanmıştır.

---

## 🛠️ Önemli Sınırlandırmalar ve Tasarım Kararları

*   **Eksik Komut Seti (ISA):** LR35902 işlemcisinin tüm komut seti (opcode) henüz yazılmamıştır. Yalnızca hedef oyunların (*Tetris* ve *Dr. Mario*) çalışması için mutlak surette gereksinim duyulan komutlar implemente edilmiştir.
*   **MBC Desteği Yoktur:** Projede henüz bir **Memory Bank Controller (MBC)** implementasyonu bulunmamaktadır. Bu nedenle şimdilik sadece **30 KB - 32 KB ve altındaki** (No-MBC) ROM dosyaları desteklenmektedir.
*   **Geliştirme Ortamı:** Proje tamamen **Linux** (Ubuntu/Debian tabanlı) ortamında, **SDL2** kütüphanesi kullanılarak geliştirilmiştir.

---

## 📂 Dosya Yapısı ve Mimarisi

Tüm kaynak kodları ve test ROM'ları doğrudan proje ana dizininde yer almaktadır:

```text
gameboy-emulator/
│
├── main.cpp            # Giriş noktası (SDL2 pencere yönetimi, ana emülasyon döngüsü)
├── cpu.h & cpu.cpp     # LR35902 İşlemci mantığı, registerlar ve opcode çözücü
├── mmu.h & mmu.cpp     # Bellek Yönetim Birimi (Memory Management Unit & Bus)
├── ppu.h & ppu.cpp     # Görüntü İşleme Birimi (Pixel Processing Unit & LCD)
├── timer.h & timer.cpp # Zamanlayıcı ve Divider/Counter register yönetimi
├── tetris.gb           # Test ROM'u (Tetris)
└── dr_mario.gb         # Test ROM'u (Dr. Mario)

🔄 Bileşenler Arası Veri Akışı Bağlantıları
1-) main.cpp, SDL2 kütüphanesini kullanarak grafik penceresini oluşturur ve oyun döngüsünü (while loop) işletir. Her adımda CPU'yu çalıştırarak donanımı ilerletir.

2-) mmu.cpp (Memory Bus), bellek haritasını (ROM, VRAM, WRAM, I/O Register, HRAM) merkezi olarak yönetir. CPU, PPU ve Timer modülleri okuma/yazma işlemlerini bu bus üzerinden gerçekleştirir.

3-) cpu.cpp, MMU üzerinden sıradaki opcode'u çeker, register'ları (A, B, C, D, E, H, L, PC, SP) ve bayrakları günceller, harcanan döngü (cycle) miktarını döndürür.

4-) ppu.cpp, VRAM verilerini ve LCD durumlarını işleyerek ekran piksellerini SDL2 ekran buffer'ına aktarır.

5-) timer.cpp, CPU döngülerini takip ederek Game Boy'un iç saat (DIV ve TIMA) register'larını günceller ve gerektiğinde kesme (interrupt) üretir.

⚙️ Gereksinimler ve Bağımlılıklar
Projeyi Linux üzerinde derleyip çalıştırmak için sisteminizde aşağıdaki araçların kurulu olması gerekir:

1-) C++17 destekli bir derleyici (g++ veya clang)

2-) SDL2 kütüphanesi (Görselleştirme ve girişler için)

🗺️ Gelecek Planlar (Roadmap)
[ ] Eksik LR35902 opcode'larının tamamlanması

[ ] MBC1/MBC3 desteğinin eklenmesiyle daha büyük ROM'ların çalıştırılması

[ ] Ses (APU) entegrasyonu
