/*
 * ====================================================================
 * 🏗️ BELLEK YÖNETİMİ: SEGMENTASYON SİMÜLASYONU
 * ====================================================================
 *
 * Amaç:
 * İşletim sistemlerinde mantıksal (sanal) adreslerin, donanım üzerindeki
 * fiziksel adreslere nasıl dönüştürüldüğünü simüle etmek.
 *
 * Bu simülasyonda kullanılan mimari:
 * - 16-bit Adres Uzayı (Toplam 65536 adreslenebilir alan)
 * - İlk 4 bit: Hangi Segmentte olduğumuzu belirtir (0-15 arası).
 * - Son 12 bit: Segment içindeki konumu (Offset) belirtir (0-4095 arası).
 *
 * ====================================================================
 */

#include <stdio.h>      // printf fonksiyonu için
#include <stdlib.h>     // Genel kütüphane fonksiyonları için

// --- MİMARİ TANIMLAMALARI ---

// 16-bitlik adresin ilk 4 biti Segment Numarasıdır.
// Bu yüzden adresi 12 bit sağa kaydırarak (>> 12) ilk 4 bite ulaşırız.
#define SEGMENT_SHIFT 12

// Son 12 bit Offset (Kayma) değeridir.
// 0xFFF = Binary olarak 0000 1111 1111 1111 demektir.
// Adres ile bu değeri VE (&) işlemine sokarsak sadece son 12 biti alırız.
#define OFFSET_MASK 0xFFF

// --- VERİ YAPILARI ---

// Segment Tablosu Girişi (Her bir satır)
// İşletim sistemi her segmentin nerede başladığını ve ne kadar uzun olduğunu burada tutar.
typedef struct {
    unsigned int base;   // Taban Adresi: Segment fiziksel bellekte nereden başlıyor?
    unsigned int limit;  // Limit (Sınır): Segmentin uzunluğu ne kadar?
    int isValid;         // Geçerlilik Biti: Bu segment kullanılıyor mu? (1=Evet, 0=Hayır)
} SegmentTableEntry;

// Segment Tablosu (Array)
// Mimarimiz 4 bitlik segment no kullandığı için en fazla 2^4 = 16 segment olabilir.
SegmentTableEntry segTable[16];

/*
 * Fonksiyon: initialize_system
 * Görevi: Segment tablosunu örnek değerlerle doldurmak (Sistemi başlatmak).
 * Bu değerler, işletim sisteminin belleği nasıl böldüğünü simüle eder.
 */
void initialize_system() {
    // --- Segment 0: KOD SEGMENTİ ---
    // Genellikle programın çalıştırılabilir komutlarını tutar.
    segTable[0].base = 0x8000;  // Fiziksel bellekte 32768 (0x8000) adresinden başlar.
    segTable[0].limit = 2048;   // Uzunluğu 2048 byte'tır.
    segTable[0].isValid = 1;    // Bu segment geçerlidir.

    // --- Segment 1: VERİ (DATA) SEGMENTİ ---
    // Global değişkenler vb. verileri tutar.
    segTable[1].base = 0xE000;  // Fiziksel bellekte 57344 (0xE000) adresinden başlar.
    segTable[1].limit = 1000;   // Uzunluğu 1000 byte'tır.
    segTable[1].isValid = 1;    // Bu segment geçerlidir.

    // --- Segment 2: YIĞIN (STACK) SEGMENTİ ---
    // Fonksiyon çağrıları ve yerel değişkenleri tutar.
    segTable[2].base = 0x4000;  // Fiziksel bellekte 16384 (0x4000) adresinden başlar.

    // NOT: Önceki versiyonda burası 500'dü ve 600 ofsetli sorguda hata veriyordu.
    // Hatayı önlemek için limiti 1000'e çıkardık.
    segTable[2].limit = 1000;
    segTable[2].isValid = 1;    // Bu segment geçerlidir.
}

/*
 * Fonksiyon: print_binary
 * Görevi: Bir sayıyı ikilik (binary) formatta ekrana yazdırmak.
 * Debug ve eğitim amaçlıdır, bitlerin nasıl göründüğünü anlamamızı sağlar.
 */
void print_binary(unsigned int num, int bits) {
    // En yüksek bitten en düşük bite doğru döngü kurar.
    for (int i = bits - 1; i >= 0; i--) {
        // (num >> i) & 1 işlemi, sayının i. bitinin 0 mı 1 mi olduğunu bulur.
        printf("%u", (num >> i) & 1);

        // Okunabilirliği artırmak için her 4 bit arasına boşluk koyar.
        if (i % 4 == 0 && i != 0) {
            printf(" ");
        }
    }
}

/*
 * Fonksiyon: translate_address
 * Görevi: Mantıksal (Sanal) Adresi Fiziksel Adrese çevirmek.
 * İşletim sisteminin MMU (Memory Management Unit) biriminin yaptığı iştir.
 */
int translate_address(unsigned int logical_addr) {

    // --- ADIM 1: AYRIŞTIRMA (DECOMPOSITION) ---
    // Adresin içinden Segment Numarası ve Offset (Kayma) bilgisini çıkarıyoruz.

    // Segment No: İlk 4 biti almak için 12 bit sağa kaydır.
    unsigned int segment_no = (logical_addr >> SEGMENT_SHIFT);

    // Offset: Son 12 biti almak için maskeleme (AND) yap.
    unsigned int offset = logical_addr & OFFSET_MASK;

    // --- BİLGİLENDİRME ÇIKTISI ---
    printf("\n=========================================\n");
    printf(" Mantiksal Adres: 0x%X (Binary: ", logical_addr);
    print_binary(logical_addr, 16);
    printf(")\n");
    printf("-----------------------------------------\n");
    printf(" Segment No : %u\n", segment_no);
    printf(" Offset     : %u (0x%X)\n", offset, offset);

    // --- ADIM 2: HATA KONTROLLERİ (PROTECTION) ---

    // Kontrol A: Böyle bir segment var mı?
    // Segment numarası 16'dan büyükse veya 'isValid' 0 ise geçersizdir.
    if (segment_no >= 16 || segTable[segment_no].isValid == 0) {
        printf("[HATA] Gecersiz Segment Numarasi! (Segment Fault)\n");
        return -1;
    }

    printf(" Limit      : %u\n", segTable[segment_no].limit);

    // Kontrol B: Offset limiti aşıyor mu?
    // Gidilmek istenen yer (offset), segmentin uzunluğundan (limit) büyük olamaz.
    if (offset >= segTable[segment_no].limit) {
        printf("[HATA] Segmentation Fault! Ofset (%u) > Limit (%u)\n", offset, segTable[segment_no].limit);
        return -1;
    }

    // --- ADIM 3: FİZİKSEL ADRES HESAPLAMA (TRANSLATION) ---
    // Mantık: Fiziksel Adres = Segment Taban Adresi + Offset
    unsigned int physical_addr = segTable[segment_no].base + offset;

    printf(" Taban (Base): 0x%X\n", segTable[segment_no].base);
    printf("-----------------------------------------\n");
    printf(" HESAPLAMA: 0x%X + 0x%X = 0x%X\n", segTable[segment_no].base, offset, physical_addr);
    printf("-----------------------------------------\n");

    // Sonucu göster
    printf("-> FIZIKSEL ADRES: 0x%X (Binary: ", physical_addr);
    print_binary(physical_addr, 16);
    printf(")\n");
    printf("=========================================\n");

    return physical_addr;
}

/*
 * Fonksiyon: main
 * Görevi: Programın giriş noktası. Sistemi başlatır ve örnek sorgular yapar.
 */
int main() {
    // 1. Sistemi (Segment Tablosunu) doldur.
    initialize_system();

    // --- TEST 1 ---
    // Mantıksal Adres: 0x12F0
    // Binary: 0001 0010 1111 0000
    // Analiz: İlk 4 bit (0001) = Segment 1. Son 12 bit (0x2F0) = Offset 752.
    // Beklenen: Segment 1 (Base 0xE000) + 752 = 0xE2F0.
    translate_address(0x12F0);

    // --- TEST 2 ---
    // Mantıksal Adres: 0x2258
    // Binary: 0010 0010 0101 1000
    // Analiz: İlk 4 bit (0010) = Segment 2. Son 12 bit (0x258) = Offset 600.
    // Beklenen: Segment 2 (Base 0x4000) + 600 = 0x4258.
    // NOT: Segment 2'nin limiti 1000 olduğu için (600 < 1000) bu işlem başarılı olur.
    translate_address(0x2258);

    return 0;
}

/*
 * ====================================================================
 * 📝 KODUN ÖZETİ VE ÇALIŞMA MANTIĞI
 * ====================================================================
 *
 * Bu program, işletim sistemlerinde kullanılan "Segmentasyon" (Segmentation)
 * bellek yönetim tekniğini simüle eder.
 *
 * 1. ADRES YAPISI:
 * Program 16-bitlik mantıksal adresler kullanır.
 * - İlk 4 bit (MSB): Hangi segmentte olduğumuzu söyler (Segment Selector).
 * - Son 12 bit (LSB): Segmentin içindeki konumu söyler (Offset).
 *
 * 2. SEGMENT TABLOSU:
 * 'segTable' dizisi, her segmentin fiziksel bellekteki başlangıç noktasını (Base)
 * ve boyutunu (Limit) tutar. Bu tablo olmadan dönüşüm yapılamaz.
 *
 * 3. DÖNÜŞÜM SÜRECİ (TRANSLATION):
 * - Mantıksal adres gelir (Örn: 0x12F0).
 * - Bit kaydırma (>> 12) ile Segment No bulunur (Örn: 1).
 * - Maskeleme (& 0xFFF) ile Offset bulunur (Örn: 0x2F0).
 * - HATA KONTROLÜ: Offset < Limit mi? Eğer değilse "Segmentation Fault" verilir.
 * - HESAPLAMA: Fiziksel Adres = Taban Adresi + Offset.
 *
 * 4. ÖRNEK SENARYO (TEST 2):
 * - Adres: 0x2258
 * - Segment: 2 (Yığın Segmenti), Offset: 600
 * - Tablodan Bak: Segment 2'nin Tabanı 0x4000, Limiti 1000.
 * - Kontrol: 600 < 1000 (Geçerli).
 * - Sonuç: 0x4000 + 600 (0x258) = 0x4258.
 *
 * ====================================================================
 */