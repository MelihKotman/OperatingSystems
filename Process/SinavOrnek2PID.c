//
//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h> // (Bu kodda kullanılmıyor)

int main(int argc, char **argv) {

    // Ebeveyn (P1) süreçte 'x' ve 'y' değişkenleri oluşturulur.
    // HOCANIZIN VARSAYIMI: x = 0 ve y = 0 olarak başlarlar.
    int x, y;

    // p0, p1, p3 HİÇ KULLANILMIYOR.
    pid_t p0,p1,p2,p3;

    // Ebeveyn (P1), Çocuk (C1) sürecini oluşturur.
    // 'fork()' anında C1, P1'in bellek kopyasını alır (x=0, y=0).
    p2 = fork();

    if (p2 == 0) {

        // --- SADECE ÇOCUK SÜREÇ (C1) ÇALIŞIR ---

        // C1, KENDİ 'x' kopyasına 10 atar.
        x = 10;
        // C1, KENDİ 'y' kopyasına 20 atar.
        y = 20;

        // C1, kendi güncel değerlerini basar.
        // BEKLENEN ÇIKTI (Çocuk): "PID = 124   x = 10    y = 20"
        printf("PID = %d\t x = %d\t y = %d\n",getpid(),x,y);
    }
    else {

        // --- SADECE EBEVEYN SÜREÇ (P1) ÇALIŞIR ---

        // !!! ANA KONSEPT: BELLEK İZOLASYONU !!!
        // C1'in 'x' ve 'y'de yaptığı değişiklikler P1'i ETKİLEMEZ.
        // P1'in 'x' ve 'y' değerleri, baştaki varsayılan
        // değerleri olan 0 olarak kalır.

        // P1, kendi (değişmemiş) değerlerini basar.
        // BEKLENEN ÇIKTI (Ebeveyn): "PID = 123   x = 0     y = 0"
        printf("PID = %d\t x = %d\t y = %d\n",getpid(),x,y);
    }
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Hocanızın Bakış Açısıyla)
 * ====================================================================
 *
 * Bu soru, "Bellek İzolasyonu"nu test etmek için hazırlanmıştır.
 *
 * VARSAYIM: Ebeveyn süreçte ilklendirilmeyen 'x' ve 'y'
 * değişkenlerinin değeri '0' (sıfır) olarak kabul edilir.
 *
 * 1. Ebeveyn (P1) başlar. Belleğinde x=0, y=0 vardır.
 * 2. 'fork()' olur. Çocuk (C1) oluşturulur. C1, P1'in belleğini
 * kopyalar (C1'de de x=0, y=0 olur).
 * 3. Çocuk (C1), 'if' bloğuna girer. KENDİ 'x'ini 10, KENDİ 'y'sini 20
 * yapar ve "x = 10, y = 20" çıktısını basar.
 * 4. Ebeveyn (P1), 'else' bloğuna girer. Bellek izolasyonu nedeniyle
 * C1'in yaptığı değişikliklerden etkilenmez.
 * 5. P1, KENDİ 'x' ve 'y' değerlerini (hala 0 olan) basar ve
 * "x = 0, y = 0" çıktısını basar.
 *
 * SONUÇ:
 * İki çıktı (sırası işletim sistemine bağlı) şöyledir:
 * PID = 123   x = 0     y = 0   (Ebeveynin çıktısı)
 * PID = 124   x = 10    y = 20  (Çocuğun çıktısı)
 *
 * Bu sonuç, (sizin düzelttiğiniz haliyle) C şıkkıyla eşleşir.
 * ====================================================================
 */
/* Ne çıkar?
 *
 * A) PID = 123 x = 0 y = 20
 *    PID = 124 x = 10 y = 20
 *
 * B) PID = 123 x = 10 y = 20
 *    PID = 124 x = 10 y = 20
 *
 * C) PID = 123 x = 0 y = 0 (Bu şıkka yakın bir cevap olacak)
 *    PID = 123 x = 10 y = 20
 *
 * D) HATA
 */