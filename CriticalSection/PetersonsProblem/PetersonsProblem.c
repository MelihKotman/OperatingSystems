
//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // Standart Giriş/Çıkış işlemleri için (printf).
#include <stdlib.h>     // Genel yardımcı fonksiyonlar için.
#include <sys/types.h>  // Sistem veri tipleri için.
#include <fcntl.h>      // Dosya kontrol bayrakları için.
#include <unistd.h>     // POSIX API fonksiyonları (sleep, vs.).
#include <sys/wait.h>   // wait fonksiyonları için.
#include <pthread.h>    // Thread'ler (iplikler) için ana kütüphane.

/*
 * ==========================================================
 * PAYLAŞILAN KAYNAKLAR (SHARED RESOURCES)
 * ==========================================================
 * DİKKAT: 20 thread'in hepsi bu iki değişkeni aynı bellek adresinde paylaşır.
 */
int x = 10;     // Kritik Veri: Değeri değiştirilen paylaşımlı değişken.
int y = 1;      // Yazılımsal Kilit Bayrağı: 1=Serbest (Lock Free), 0=Meşgul (Lock Taken).

/*
 * 'arttir' - Arttırıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *arttir(void *arg) {
    int myID;
    myID = (int)arg;

    // !!! YAZILIMSAL KİLİT GİRİŞ BÖLÜMÜ !!!
    // GİRİŞ KONTROLÜ: Kilit serbest mi? (y == 1 mi?)
    if (y == 1) {
        // KRİTİK HATA NOKTASI: Bu kontrol ve aşağıdaki atama ATOMİK DEĞİLDİR!

        y = 0;      // KİLİDİ AL (y=0 yaparak meşgul ilan et).

        // --- KRİTİK BÖLGE ---
        ++x;        // Paylaşımlı veriyi değiştir.
        printf("ID = %d\t x=%d\n",myID,x);
        // --- KRİTİK BÖLGE SONU ---

        y = 1;      // KİLİDİ BIRAK (y=1 yaparak serbest bırak).
    }

    // Thread'ler arasında ahengi bozmak ve yarış durumunu garantilemek için bekleme.
    sleep(2);
}

/*
 * 'azalt' - Azaltıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *azalt(void *arg) {
    int myID;
    myID = (int)arg;

    // !!! YAZILIMSAL KİLİT GİRİŞ BÖLÜMÜ !!!
    // GİRİŞ KONTROLÜ: Kilit serbest mi? (y == 1 mi?)
    if (y == 1) {
        // KRİTİK HATA NOKTASI: Aynı anda iki thread de buraya girebilir!

        y = 0;      // KİLİDİ AL (y=0 yaparak meşgul ilan et).

        // --- KRİTİK BÖLGE ---
        --x;        // Paylaşımlı veriyi değiştir.
        printf("ID = %d\t x=%d\n",myID,x);
        // --- KRİTİK BÖLGE SONU ---

        y = 1;      // KİLİDİ BIRAK (y=1 yaparak serbest bırak).
    }

    // Thread'ler arasında ahengi bozmak ve yarış durumunu garantilemek için bekleme.
    sleep(2);
}

/*
 * ==========================================================
 * ANA FONKSİYON (MAIN)
 * ==========================================================
 */
int main(int argc, char **argv) {
    pthread_t th[20]; // 20 thread için kimlik dizisi.
    int i;            // Döngü sayacı.

    // Başlangıç değerini basar.
    printf("x = %d\n",x); // ÇIKTI: "x = 10"

    // 10 adet "arttir" thread'i oluşturulur.
    for (i = 0;i < 10;i++) {
        // iplik oluşturma: &th[i] ID'si ile arttir fonksiyonunu çağırır.
        pthread_create(&th[i],NULL,arttir,(void*)i);
    }

    // 10 adet "azalt" thread'i oluşturulur.
    for (i = 10;i < 20;i++) {
        // iplik oluşturma: &th[i] ID'si ile azalt fonksiyonunu çağırır.
        pthread_create(&th[i],NULL,azalt,(void*)i);
    }

    // Ana thread (main), 20 thread'in HEPSİNİN bitmesini bekler (Senkronizasyon).
    for (i = 0;i < 20;i++) {
        // pthread_join: Ana ipliğin, th[i] ipliğinin bitmesini beklemesini sağlar.
        pthread_join(th[i],NULL);
    }

    // !!! SONUÇ !!!
    // 20 thread'in tamamı işini bitirdikten sonra 'x'in son
    // değeri basılır.
    printf("x = %d\n",x); // BEKLENEN: 10. ÇIKACAK: 10'dan farklı (Yarış Durumu nedeniyle).
    printf("Bitti");

    // Ana program sonlanır.
    return 0;
}
/*
 * ====================================================================
 * 📰 KODUN ÖZETİ VE AÇIKLAMASI
 * ====================================================================
 *
 * Bu kod, eşzamanlı programlamada bir Yarış Durumu (Race Condition) olduğunu
 * ve basit yazılımsal kilitlerin (lock) neden başarısız olduğunu gösteren bir örnektir.
 *
 * 📝 Amaç
 * Kodun amacı, global değişken 'x' üzerinde 10 artırma ve 10 azaltma işlemi yaparak,
 * sonucun matematiksel olarak beklendiği gibi 10 olup olmadığını test etmektir.
 *
 * 💥 Kritik Sorun: Kilit Mekanizmasındaki Yarış Durumu
 * Bu kod, kritik bölgeyi korumak için basit bir bayrak (int y) kullanmayı deniyor:
 *
 * Kontrol: if (y == 1) (Kilit boş mu?)
 * Atama: y = 0; (Kilidi al.)
 *
 * Problem: Bu iki adım arasında başka bir thread araya girebilir (Interleaving).
 *
 * Thread A, if (y == 1) kontrolünü yapar (TRUE).
 * İşletim sistemi araya girer.
 * Thread B, hemen ardından if (y == 1) kontrolünü yapar (hala TRUE).
 * İşletim sistemi tekrar araya girer.
 * Hem A hem de B, kilidin boş olduğuna inanarak kritik bölgeye girer.
 *
 * Sonuç: Bu basit kontrol, Karşılıklı Dışlama (Mutual Exclusion) gereksinimini
 * sağlayamaz. Her iki thread de aynı anda ++x veya --x yaptığında, tıpkı korumasız
 * kodda olduğu gibi, güncellemeler birbirinin üzerine yazar ve x'in son değeri 10'dan
 * farklı (non-deterministic) çıkar.
 *
 * Bu örnek, Peterson's Algoritması veya Mutex gibi daha karmaşık araçların, kilidi alma
 * ve kontrol etme işlemlerini atomik (bölünmez) hale getirerek bu sorunu nasıl çözdüğünü
 * anlamak için kritik bir adımdır.
 *
 * ====================================================================
 */