//
// Created by Melih Yiğit Kotman on 29.12.2025.
//
/*
 * ====================================================================
 * 📚 OKUR - YAZAR PROBLEMİ (READERS - WRITERS)
 * ====================================================================
 *
 * Senaryo:
 * - Ortak bir veri (kitap/veritabanı) var.
 * - Yazarlar: Veriyi değiştirir (Tekelci/Exclusive erişim gerekir).
 * - Okuyucular: Veriyi okur (Paylaşımlı/Concurrent erişim serbest).
 *
 * Amaç:
 * Veri bütünlüğünü bozmadan, okuyucuların aynı anda okumasına izin vermek
 * ama yazar yazarken kimseyi içeri almamak.
 *
 * Çözüm (Okuyucu Öncelikli):
 * - İlk giren okuyucu, yazarları engeller (kilitler).
 * - Son çıkan okuyucu, kilidi açar (yazarlara izin verir).
 * - Aradaki okuyucular kilide takılmadan rahatça girip çıkar.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

// --- PAYLAŞILAN KAYNAKLAR ---
int veri = 0;           // Kitap/Veritabanı
int okuyucu_sayisi = 0; // İçerideki okuyucu sayısı

// --- SENKRONİZASYON ARAÇLARI ---
sem_t rw_mutex;         // Yazma kilidi (Yazarlar ve İlk Okuyucu için)
pthread_mutex_t mutex;  // okuyucu_sayisi değişkenini korumak için

// --- YAZAR (WRITER) FONKSİYONU ---
void *yazar(void *arg) {
    int id = *(int*)arg;
    while(1) {
        // 1. YAZMA İZNİ İSTE
        // Eğer içeride okuyucu veya başka yazar varsa BEKLE.
        sem_wait(&rw_mutex);

        // --- KRİTİK BÖLGE (YAZMA) ---
        veri++;
        printf("✍️ YAZAR %d: Veriyi değiştirdi -> %d\n", id, veri);
        // ----------------------------

        // 2. YAZMA İZNİNİ BIRAK
        sem_post(&rw_mutex);

        sleep(2); // Yazar biraz dinlensin
    }
}

// --- OKUYUCU (READER) FONKSİYONU ---
void *okuyucu(void *arg) {
    int id = *(int*)arg;
    while(1) {
        // --- GİRİŞ PROTOKOLÜ ---

        // 1. Okuyucu sayısını artırmak için 'mutex' al (Kendi aramızda karışmasın)
        pthread_mutex_lock(&mutex);

        okuyucu_sayisi++;
        if (okuyucu_sayisi == 1) {
            // Eğer ben İLK gelen okuyucuysam, Yazarları ENGELLEMEM lazım!
            // Kapıyı kilitle ki yazar girmesin.
            sem_wait(&rw_mutex);
        }

        pthread_mutex_unlock(&mutex); // Sayacı artırdım, mutexi bırak

        // --- KRİTİK BÖLGE (OKUMA) ---
        // Buraya birden fazla okuyucu aynı anda girebilir!
        printf("👀 OKUYUCU %d: Okuyor -> %d (İçeride: %d okuyucu)\n", id, veri, okuyucu_sayisi);
        // ----------------------------

        // --- ÇIKIŞ PROTOKOLÜ ---

        // 2. Okuyucu sayısını azaltmak için 'mutex' al
        pthread_mutex_lock(&mutex);

        okuyucu_sayisi--;
        if (okuyucu_sayisi == 0) {
            // Eğer ben SON çıkan okuyucuysam, Yazarların kilidini AÇ!
            // Artık içeride kimse yok, yazar girebilir.
            sem_post(&rw_mutex);
        }

        pthread_mutex_unlock(&mutex); // Mutexi bırak

        sleep(1); // Okuyucu biraz dinlensin
    }
}

int main() {
    pthread_t r[5], w[2]; // 5 Okuyucu, 2 Yazar
    int id[5];
    int i;

    // --- BAŞLATMA ---
    sem_init(&rw_mutex, 0, 1);       // Yazma kilidi açık (1)
    pthread_mutex_init(&mutex, NULL); // Mutex açık

    // ID Dizisini Doldur
    for(i=0; i<5; i++) id[i] = i+1;

    // 2 Yazar Oluştur
    for(i=0; i<2; i++)
        pthread_create(&w[i], NULL, yazar, &id[i]);

    // 5 Okuyucu Oluştur
    for(i=0; i<5; i++)
        pthread_create(&r[i], NULL, okuyucu, &id[i]);

    // Programı bir süre çalıştırıp kapatalım (Sonsuz döngüden çıkmak için)
    sleep(10);
    printf("\n--- Süre doldu, program kapatılıyor ---\n");
    return 0;
}

/*
 * ====================================================================
 * 📝 KODUN ÇALIŞMA MANTIĞI VE ÖZETİ
 * ====================================================================
 *
 * 1. Yazarların Durumu:
 * Yazarlar 'sem_wait(&rw_mutex)' ile korunur. Bu kilit, odaya (veriye)
 * tek başına girilmesini sağlar. Yazar içerideyken kimse (okuyucu dahil)
 * giremez.
 *
 * 2. Okuyucuların Durumu (Kritik Nokta):
 * Okuyucuların gücü "birlikten kuvvet doğar" ilkesidir.
 * - İLK gelen okuyucu ('okuyucu_sayisi == 1'), fedakarlık yapar ve
 * 'rw_mutex' kilidini alarak yazarları engeller.
 * - SONRAKİ okuyucular, kilit zaten alındığı için 'rw_mutex' ile uğraşmaz,
 * direkt içeri girer. Böylece 100 okuyucu aynı anda okuyabilir.
 * - SON çıkan okuyucu ('okuyucu_sayisi == 0'), ışığı kapatır gibi
 * 'rw_mutex' kilidini açar ve yazarlara izin verir.
 *
 * 3. Senkronizasyon:
 * 'mutex' sadece 'okuyucu_sayisi' değişkenini (sayaç) korur.
 * 'rw_mutex' ise asıl veriyi (yazma işlemini) korur.
 *
 * Bu yöntem "Okuyucu Öncelikli"dir. Okuyucular sürekli gelirse,
 * yazarlar asla sıra bulamayabilir (Yazar Açlığı / Writer Starvation).
 * ====================================================================
 */