//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // printf için
#include <stdlib.h>     // genel fonksiyonlar
#include <unistd.h>     // sleep için
#include <pthread.h>    // threadler için
#include <semaphore.h>  // !!! SEMAFOR KÜTÜPHANESİ !!!

int x = 10;     // Paylaşılan Kritik Değişken

// Semafor Tanımlaması
// sem_t: POSIX semafor veri tipi.
sem_t semafor;

void *arttir(void *arg) {
    int myID = (int)arg;

    // --- WAIT (P) ---
    // S <= 0 ise bekle, S > 0 ise S-- yap ve gir.
    // Binary semafor kullandığımız için 1 ise 0 yapar ve girer.
    sem_wait(&semafor);

    // --- KRİTİK BÖLGE ---
    ++x;
    printf("ID = %d\t x=%d (Arttırıldı)\n", myID, x);
    // --------------------

    // --- SIGNAL (V) ---
    // S++ yapar (0 ise 1 yapar). Bekleyen varsa uyandırır.
    sem_post(&semafor);

    sleep(2);
    return NULL;
}

void *azalt(void *arg) {
    int myID = (int)arg;

    // --- WAIT (P) ---
    sem_wait(&semafor);

    // --- KRİTİK BÖLGE ---
    --x;
    printf("ID = %d\t x=%d (Azaltıldı)\n", myID, x);
    // --------------------

    // --- SIGNAL (V) ---
    sem_post(&semafor);

    sleep(2);
    return NULL;
}

int main(int argc, char **argv) {
    pthread_t th[20];
    int i;

    // --- SEMAFOR BAŞLATMA (INIT) ---
    // Parametre 1: Semaforun adresi (&semafor)
    // Parametre 2: 1 = Paylaşım var
    // Parametre 3: 1 = Başlangıç Değeri (VALUE).
    // !!! BURASI ÖNEMLİ: 1 verdiğimiz için "Binary Semafor" (Mutex) gibi davranır.
    // Eğer 5 verseydik, aynı anda 5 thread kritik bölgeye girebilirdi.
    sem_init(&semafor, 1, 1);

    printf("x = %d (Baslangic)\n", x);

    // 10 Arttırıcı Thread
    for (i = 0; i < 10; i++) {
        pthread_create(&th[i], NULL, arttir, (void*)i);
    }

    // 10 Azaltıcı Thread
    for (i = 10; i < 20; i++) {
        pthread_create(&th[i], NULL, azalt, (void*)i);
    }

    // Bekleme (Join)
    for (i = 0; i < 20; i++) {
        pthread_join(th[i], NULL);
    }

    // --- SEMAFOR YOK ETME ---
    sem_destroy(&semafor);

    printf("x = %d (Sonuc)\n", x); // Mutex gibi çalıştığı için sonuç kesinlikle 10 olacaktır.
    printf("Bitti");

    return 0;
}
/*
 * ====================================================================
 * 📝 KODUN ÖZETİ VE AÇIKLAMASI: BINARY SEMAFOR İLE ÇÖZÜM
 * ====================================================================
 *
 * Bu kod, Kritik Bölge Problemini çözmek için Binary Semafor kullanır.
 *
 * 1. Başlatma (sem_init):
 * Kodun en kritik yeri main içindeki 'sem_init(&semafor, 1, 1);' satırıdır.
 * - Buradaki sondaki '1' değeri, semaforun başlangıç değeridir.
 * - '1' olduğu için semafor "boş" (erişilebilir) olarak başlar.
 * - Bu sayede, 'sem_wait' çağıran ilk thread içeri girer ve değeri 0 yapar.
 * İkinci gelen thread 0'ı görünce bekler.
 *
 * 2. Wait (sem_wait):
 * Mutex'teki 'lock' işleminin karşılığıdır. Değeri kontrol eder ve azaltır.
 * Atomik bir işlemdir. (S <= 0 ise bekle, değilse azalt ve gir).
 *
 * 3. Signal (sem_post):
 * Mutex'teki 'unlock' işleminin karşılığıdır. Değeri artırır ve bekleyen
 * bir thread varsa onu içeri alır (uyandırır).
 *
 * 🎯 Sonuç:
 * Semafor değeri sadece 0 ve 1 arasında gidip geldiği için (Binary), bu kodda
 * Semafor tam olarak bir Mutex (Kilit) görevi görmüş ve 'x' değişkenini
 * yarış durumundan koruyarak sonucun 10 çıkmasını garanti etmiştir.
 *
 * ====================================================================
 */