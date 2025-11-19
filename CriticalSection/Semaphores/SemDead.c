//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // Standart Giriş/Çıkış işlemleri için.
#include <stdlib.h>     // Genel yardımcı fonksiyonlar için.
#include <sys/types.h>  // Sistem veri tipleri için.
#include <fcntl.h>      // Dosya kontrol bayrakları için.
#include <unistd.h>     // POSIX API (sleep vb.) için.
#include <sys/wait.h>   // wait fonksiyonları için.
#include <pthread.h>    // Thread (iplik) işlemleri için.
#include <semaphore.h>  // Semafor işlemleri için gerekli kütüphane.

/*
 * ==========================================================
 * PAYLAŞILAN KAYNAKLAR (SHARED RESOURCES)
 * ==========================================================
 */
int x = 10;     // Kritik Veri: Değeri değiştirilen paylaşımlı değişken.

// İki adet Semafor tanımlıyoruz.
// Bunlar "Binary Semafor" olarak başlatılacak (Mutex gibi davranacak).
sem_t s, q;

/*
 * 'arttir' - Arttırıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *arttir(void *arg) {
    int myID;
    myID = (int)arg;

        // --- KAYNAK ALMA SIRASI: ÖNCE S, SONRA Q ---

        // 1. Kaynağı (S) Kilitle. (S=1 ise 0 yap gir, S=0 ise bekle).
        sem_wait(&s);

        // Buraya 'sleep' girmese bile, işletim sistemi (OS) burada
        // context switch yaparsa Deadlock riski doğar.
        sleep(10);

        // 2. Kaynağı (Q) Kilitle.
        // Eğer 'azalt' fonksiyonu Q'yu almışsa, burada sonsuza kadar beklersin!
        sem_wait(&q);

        // --- KRİTİK BÖLGE ---
        ++x;        // Paylaşımlı veriyi değiştir.
        printf("ID = %d\t x=%d (Arttir)\n", myID, x);
        // --- KRİTİK BÖLGE SONU ---

        // --- KAYNAKLARI BIRAKMA ---
        sem_post(&s); // S kilidini aç.
        sem_post(&q); // Q kilidini aç.

    // Thread'ler arasında geçişi teşvik etmek için bekleme.
    sleep(2);
}

/*
 * 'azalt' - Azaltıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *azalt(void *arg) {
    int myID;
    myID = (int)arg;

    // --- KAYNAK ALMA SIRASI: ÖNCE Q, SONRA S (TERS SIRA!) ---
    // Deadlock'un ana sebebi bu ters sıralamadır.

    // 1. Kaynağı (Q) Kilitle.
    sem_wait(&q);

    // Buraya 'sleep' girmese bile, işletim sistemi (OS) burada
    // context switch yaparsa Deadlock riski doğar.
    sleep(10);

    // 2. Kaynağı (S) Kilitle.
    // Eğer 'arttir' fonksiyonu S'i almışsa, burada sonsuza kadar beklersin!
    sem_wait(&s);

    // --- KRİTİK BÖLGE ---
    // Not: Fonksiyon adı 'azalt' ama kodda ++x (arttırma) yapılmış.
    // Deadlock mantığı açısından işlemin ne olduğu fark etmez.
    --x;
    printf("ID = %d\t x=%d (Azalt)\n", myID, x);
    // --- KRİTİK BÖLGE SONU ---

    // --- KAYNAKLARI BIRAKMA ---
    sem_post(&q); // Q kilidini aç.
    sem_post(&s); // S kilidini aç.

    // Thread'ler arasında geçişi teşvik etmek için bekleme.
    sleep(2);
}

/*
 * ==========================================================
 * ANA FONKSİYON (MAIN)
 * ==========================================================
 */
int main(int argc, char **argv) {
    // --- SEMAFOR BAŞLATMA ---
    // Parametre 2 (1): Process-shared (Prosesler arası paylaşım).
    //                  1 de çalışacak.
    // Parametre 3 (1): Başlangıç Değeri. 1 olduğu için Binary Semafor (Mutex) gibi davranır.
    sem_init(&s, 1, 1);
    sem_init(&q, 1, 1);

    pthread_t th[20]; // 20 thread kimliği.
    int i;            // Döngü sayacı.

    printf("x = %d\n", x); // Başlangıç değeri.

    // Döngü i < 20 olduğu sürece döner.
    // DİKKAT: Döngü içinde 'i' bir kez daha arttırılıyor (i++).
    // Bu yüzden döngü 10 kez döner, her turda 2 thread (toplam 20) oluşturur.
    for (i = 0; i < 20; i++) {

        // Çift sayılı ID'ler (0, 2, 4...) 'arttir' fonksiyonuna gider.
        pthread_create(&th[i], NULL, arttir, (void*)i);

        i++; // Döngü sayacını manuel arttır.

        // Tek sayılı ID'ler (1, 3, 5...) 'azalt' fonksiyonuna gider.
        pthread_create(&th[i], NULL, azalt, (void*)i);
    }

    // Ana thread, tüm thread'lerin bitmesini bekler.
    // Deadlock oluşacağı için bu döngü muhtemelen asla tamamlanamaz.
    for (i = 0; i < 20; i++) {
        pthread_join(th[i], NULL);
    }

    // Deadlock nedeniyle buraya asla ulaşılamayacaktır.
    printf("x = %d\n", x);
    printf("Bitti");

    // Ana program sonlanır.
    return 0;
}

/*
 * ====================================================================
 * 📝 KODUN ÖZETİ VE AÇIKLAMASI: ÇAPRAZ KİLİTLENME (DEADLOCK)
 * ====================================================================
 *
 * Bu kod, iki farklı Binary Semafor (S ve Q) kullanılarak oluşturulmuş
 * klasik bir "Kilitlenme" (Deadlock) senaryosudur.
 *
 * 1. Kaynaklar (Semaforlar):
 * - 's' ve 'q' adında iki semafor, 1 değeriyle (boş/erişilebilir) başlatılır.
 *
 * 2. 'arttir' Fonksiyonunun Davranışı:
 * - Önce S semaforunu alır (wait).
 * - Sonra Q semaforunu almaya çalışır.
 *
 * 3. 'azalt' Fonksiyonunun Davranışı (Ters Sıra):
 * - Önce Q semaforunu alır (wait).
 * - Sonra S semaforunu almaya çalışır.
 *
 * 💥 Kilitlenme Senaryosu (Circular Wait):
 * - Thread A (arttir) çalışır, S'yi kilitler.
 * - Tam bu anda işletim sistemi (OS) Thread A'yı durdurur, Thread B'ye (azalt) geçer.
 * - Thread B çalışır, Q'yu kilitler.
 * - Thread B şimdi S'yi ister. Fakat S, Thread A'dadır. B beklemeye geçer.
 * - OS tekrar Thread A'ya döner. A şimdi Q'yu ister. Fakat Q, Thread B'dedir.
 *
 * 🚫 Sonuç:
 * A, B'yi bekler; B, A'yı bekler. Bu "Döngüsel Bekleme" sonsuza kadar sürer.
 * Program donar ve asla bitmez.
 * ====================================================================
 */