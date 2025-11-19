//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // Standart Giriş/Çıkış (printf).
#include <stdlib.h>     // Genel kütüphane.
#include <sys/types.h>  // Sistem veri tipleri.
#include <fcntl.h>      // Dosya kontrol.
#include <unistd.h>     // POSIX API (sleep).
#include <sys/wait.h>   // Wait fonksiyonları.
#include <pthread.h>    // Thread ve Mutex kütüphanesi.
#include <semaphore.h>  // Semafor kütüphanesi.

/*
 * ==========================================================
 * PAYLAŞILAN KAYNAKLAR (SHARED RESOURCES)
 * ==========================================================
 */
int x = 0;     // Ürün Sayısı (Tamponda şu an kaç ürün var). Başlangıç 0.

// İki adet Semafor Tanımlanıyor:
// e (Empty): Boş raf sayısını tutar.
// f (Full): Dolu raf sayısını tutar.
sem_t e, f;

// Mutex (Kilit): 'x' değişkenini (Kritik Bölgeyi) korumak için.
pthread_mutex_t kilit = PTHREAD_MUTEX_INITIALIZER;

/*
 * 'uretici' - Üretici Thread Fonksiyonu
 * Görevi: Vitrine (Tampona) ürün koymak.
 */
void *uretici(void *arg) {
    int myID;
    myID = (int)arg;

        // 1. Adım: Boş yer var mı? (e > 0 mı?)
        // Linux'ta sem_wait çalışır. Eğer e=0 ise (vitrin doluysa) burada BEKLER.
        // Eğer e > 0 ise, e'yi 1 azaltır ve devam eder.
        sem_wait(&e);

        // 2. Adım: Kritik Bölgeye giriş için Mutex'i al.
        // Aynı anda başka bir üretici veya tüketici x'i değiştiremesin.
        pthread_mutex_lock(&kilit);

        // --- KRİTİK BÖLGE BAŞLANGICI ---
        ++x;        // Ürün üretildi, sayı arttı.
        printf("ID = %d\t x=%d (Uretildi)\n", myID, x);
        // --- KRİTİK BÖLGE SONU ---

        // 3. Adım: Mutex'i bırak.
        pthread_mutex_unlock(&kilit);

        // 4. Adım: Dolu raf sayısını (f) 1 artır.
        // Eğer 'f' semaforunda bekleyen bir Tüketici varsa, onu uyandırır.
        sem_post(&f);

    // İşlem simülasyonu için bekleme.
    sleep(2);
    return NULL;
}

/*
 * 'tuketici' - Tüketici Thread Fonksiyonu
 * Görevi: Vitrinden (Tampondan) ürün almak.
 */
void *tuketici(void *arg) {
    int myID;
    myID = (int)arg;

    // 1. Adım: Alınacak ürün var mı? (f > 0 mı?)
    // Başlangıçta f=0 olduğu için, ilk çalışan tüketici burada BLOKE OLUR (BEKLER).
    // Üretici 'sem_post(&f)' yapana kadar buradan geçemez.
    sem_wait(&f);

    // 2. Adım: Kritik Bölgeye giriş kilidi.
    pthread_mutex_lock(&kilit);

    // --- KRİTİK BÖLGE BAŞLANGICI ---
    --x;        // Ürün tüketildi, sayı azaldı.
    printf("ID = %d\t x=%d (Tuketildi)\n", myID, x);
    // --- KRİTİK BÖLGE SONU ---

    // 3. Adım: Kilidi bırak.
    pthread_mutex_unlock(&kilit);

    // 4. Adım: Boş raf sayısını (e) 1 artır.
    // Bekleyen bir üretici varsa (yer açılmasını bekleyen), onu uyandırır.
    sem_post(&e);

    // İşlem simülasyonu.
    sleep(2);
    return NULL;
}

/*
 * ==========================================================
 * ANA FONKSİYON (MAIN)
 * ==========================================================
 */
int main(int argc, char **argv) {
    // --- SEMAFOR BAŞLATMA (LINUX) ---
    // sem_init(&semafor, pshared, value)
    // pshared = 1 (Linux'ta threadler veya processler arası paylaşım için genelde 0 kullanılır ama 1 de çalışır).

    // 'e' (Empty): Başlangıç değeri 10. (Vitrin 10 ürün alabilir, hepsi boş).
    sem_init(&e, 1, 10);

    // 'f' (Full): Başlangıç değeri 0. (Vitrinde hiç ürün yok).
    sem_init(&f, 1, 0);

    pthread_t th[100]; // 100 thread ID'si.
    int i;

    // Başlangıç değerini basar. (x=0)
    printf("x = %d (Baslangic)\n", x);

    // Thread Oluşturma Döngüsü (100 Adet)
    // 50 Tüketici, 50 Üretici oluşturulacak.
    for (i = 0; i < 100; i++) {
        // Önce Tüketici oluşturuluyor.
        // Linux'ta Tüketici başlar, f=0 olduğu için sem_wait(&f)'te beklemeye geçer.
        pthread_create(&th[i], NULL, tuketici, (void*)i);

        i++; // Sayaç manuel arttırılıyor.

        // Sonra Üretici oluşturuluyor.
        // Üretici başlar, e=10 olduğu için sem_wait(&e) başarılı olur, ürün üretir,
        // sonra sem_post(&f) yaparak bekleyen tüketiciyi uyandırır.
        pthread_create(&th[i], NULL, uretici, (void*)i);
    }

    // Tüm thread'lerin bitmesini bekle.
    for (i = 0; i < 100; i++) {
        pthread_join(th[i], NULL);
    }

    // !!! SONUÇ !!!
    // 50 üretim (+50) ve 50 tüketim (-50) yapıldığı için,
    // Başlangıç x=0 ise, Sonuç x=0 olmalıdır.
    printf("x = %d (Sonuc)\n", x);
    printf("Bitti");

    // Semaforları temizle (İyi programlama pratiği)
    sem_destroy(&e);
    sem_destroy(&f);
    pthread_mutex_destroy(&kilit);

    return 0;
}

/*
 * ====================================================================
 * 🐧 KODUN ÖZETİ VE AÇIKLAMASI (LINUX ORTAMINDA)
 * ====================================================================
 *
 * Bu kod, Sınırlı Tampon (Bounded Buffer) problemini Linux üzerinde
 * doğru bir şekilde çözer.
 *
 * 1. Semaforların Rolü (Senkronizasyon):
 * - 'e' (Empty) semaforu 10 ile başlar. Üreticilerin tampon dolana kadar (10 ürün)
 * çalışmasına izin verir. Tampon dolarsa (e=0), üreticileri bekletir.
 * - 'f' (Full) semaforu 0 ile başlar. Tüketicilerin, tamponda ürün yokken
 * çalışmasını engeller.
 *
 * 2. Akış Mantığı:
 * - Kodda döngü içinde ÖNCE 'tuketici' thread'i oluşturulur.
 * - Tüketici başlar, 'sem_wait(&f)' çağırır. f=0 olduğu için Tüketici BEKLER (Block).
 * - Hemen ardından 'uretici' thread'i oluşturulur.
 * - Üretici başlar, 'sem_wait(&e)' çağırır. e=10 olduğu için geçer (e=9 olur).
 * - Üretici 'x'i arttırır ve 'sem_post(&f)' çağırır (f=1 olur).
 * - 'f' arttığı için, bekleyen Tüketici UYANIR, 'x'i azaltır ve yoluna devam eder.
 *
 * 3. Mutex'in Rolü (Kritik Bölge):
 * - 'x' değişkeni (tampondaki ürün sayısı) bir Mutex ile korunur.
 * - Bu sayede, aynı anda hem üretici hem tüketici x'i değiştirmeye çalışırsa
 * veri bozulması (Yarış Durumu) yaşanmaz.
 *
 * 🎯 Sonuç:
 * Linux'ta bu kod hatasız çalışır.
 * - Tüketici olmayan ürünü tüketemez (Bekletilir).
 * - Üretici taşan rafa ürün koyamaz (Bekletilir).
 * - x değeri asla negatif olmaz ve veri tutarlılığı korunur.
 * - 50 üretim ve 50 tüketim sonunda x tekrar 0 olur.
 * ====================================================================
 */