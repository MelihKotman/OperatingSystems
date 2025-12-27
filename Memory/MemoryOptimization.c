//
// Created by Melih Yiğit Kotman on 28.12.2025.
//
/*
 * ====================================================================
 * 🧠 BELLEK KULLANIM BİLDİRİMİ SİMÜLASYONU
 * ====================================================================
 *
 * Amaç:
 * Bir programın, işletim sistemine (OS) gerçek bellek kullanımını
 * bildirerek Swapping (Takas) işlemini optimize etmesini sağlamak.
 *
 * Senaryo:
 * Program büyük bir bellek alanı (100 MB) ayırır, ancak bunun sadece
 * küçük bir kısmını (10 MB) kullanır. İşletim sistemine bu durumu
 * bildirerek gereksiz disk yazma işlemlerini önler.
 *
 * Not: 'request_memory' ve 'release_memory' standart C fonksiyonları
 * değildir. Bu kod, sunumdaki konsepti göstermek için yazılmış bir
 * simülasyondur (Pseudo-Code).
 * ====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep

// Simüle edilen Sistem Çağrıları (System Calls)
void request_memory(void *ptr, size_t size) {
    printf("[SİSTEM]: %p adresinden itibaren %zu MB bellek 'AKTİF' olarak işaretlendi.\n", ptr, size / (1024*1024));
    printf("          -> Swap durumunda sadece bu kısım diske yazılacak.\n");
}

void release_memory(void *ptr, size_t size) {
    printf("[SİSTEM]: %p adresinden itibaren %zu MB bellek 'PASİF' olarak işaretlendi.\n", ptr, size / (1024*1024));
    printf("          -> Bu kısım artık Swap edilmeyecek (Zaman kazancı!).\n");
}

int main() {
    size_t total_size = 100 * 1024 * 1024; // 100 MB
    size_t used_size = 10 * 1024 * 1024;   // 10 MB

    printf("--- PROGRAM BAŞLIYOR ---\n");

    // 1. Büyük bir bellek alanı ayır (100 MB)
    // İşletim sistemi bu 100 MB'ın tamamını "kullanımda" sanar.
    char *buffer = (char*) malloc(total_size);
    if (!buffer) {
        perror("Malloc hatasi");
        return 1;
    }
    printf(">> 100 MB Bellek Ayrıldı (Adres: %p)\n\n", buffer);

    // 2. Belleğin sadece bir kısmını kullan (10 MB)
    for (size_t i = 0; i < used_size; i++) {
        buffer[i] = 'A'; // Veri yazılıyor
    }
    printf(">> Sadece ilk 10 MB veri ile dolduruldu.\n\n");

    // --- KRİTİK NOKTA: İŞLETİM SİSTEMİNE BİLDİRİM ---

    // Normalde OS, swap yaparken 100 MB'ın hepsini diske yazar.
    // Ancak biz ona "Sadece 10 MB kullanıyorum" diyeceğiz.

    // "Bu 10 MB'a ihtiyacım var, bunu sakla"
    request_memory(buffer, used_size);

    // "Geri kalan 90 MB boş, onu takas etmene (diske yazmana) gerek yok!"
    // (buffer + used_size) adresinden başlayarak (total - used) kadar alan.
    release_memory(buffer + used_size, total_size - used_size);

    printf("\n>> İşlem uyuyor... (Bu sırada Swap olursa sadece 10 MB yazılacak)\n");
    sleep(5);

    printf("--- PROGRAM BİTTİ ---\n");
    free(buffer);

    return 0;
}

/*
 * ====================================================================
 * 📝 KODUN ÖZETİ VE ÇALIŞMA MANTIĞI
 * ====================================================================
 *
 * Bu kod, işletim sisteminin Swapping (Takas) işlemini optimize etmek için
 * kullanılan bir konsepti simüle eder.
 *
 * 1. SORUN:
 * Program 100 MB'lık büyük bir bellek alanı ayırır (malloc). İşletim sistemi,
 * bu alanın tamamının dolu olduğunu ve önemli olduğunu varsayar. Eğer RAM dolarsa
 * ve bu programı diske (Swap alanı) taşıması gerekirse, OS güvenli olsun diye
 * tüm 100 MB'ı diske yazar. Bu işlem (disk I/O) çok yavaştır (Örn: 4 saniye).
 *
 * 2. GERÇEK DURUM:
 * Program aslında bu 100 MB'ın sadece ilk 10 MB'ını veri ile doldurmuştur.
 * Geri kalan 90 MB boştur ve diske yazılması zaman israfıdır.
 *
 * 3. ÇÖZÜM (request_memory / release_memory):
 * Program, işletim sistemine "ipucu" verir:
 * - request_memory: "İlk 10 MB benim için çok önemli, burayı sakla."
 * - release_memory: "Geri kalan 90 MB'ı kullanmıyorum. Eğer swap yaparsan,
 * bu kısmı diske yazmana gerek yok, at gitsin."
 *
 * 4. SONUÇ:
 * İşletim sistemi bu ipuçlarını alırsa, swap sırasında sadece 10 MB veri yazar.
 * Bu da takas işleminin süresini dramatik şekilde azaltır (Örn: 4 saniyeden 0.4 saniyeye).
 * Gerçek sistemlerde bu işlem 'madvise()' sistem çağrısı ile yapılır.
 * ====================================================================
 */