//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>

/*
 * ==========================================================
 * 1. PAYLAŞILAN KAYNAK (SHARED RESOURCE)
 * ==========================================================
 * 'x' bir global değişkendir. 'fork()'un aksine, thread'ler
 * belleği kopyalamaz, PAYLAŞIR.
 * 20 thread'in hepsi AYNI 'x' değişkenine (aynı bellek adresine)
 * erişecektir.
 */
int x = 10;

/*
 * 'arttir' - Arttırıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *arttir(void *tID) {
    // 'sleep(2)' komutu, bir "Yarış Durumu" oluşmasını
    // neredeyse garanti altına alır.
    sleep(2);

    // !!! KRİTİK BÖLGE (CRITICAL SECTION) !!!
    // '++x' işlemi "atomic" (bölünemez) DEĞİLDİR.
    // Bu satırda thread'ler birbiriyle YARIŞACAKTIR.
    ++x;

    printf("th+ = %d\t x=%d\n",(int)tID,x);
}

/*
 * 'azalt' - Azaltıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *azalt(void *tID) {
    sleep(2);

    // !!! KRİTİK BÖLGE (CRITICAL SECTION) !!!
    // '--x' işlemi de "atomic" DEĞİLDİR.
    --x;

    printf("th- = %d\t x=%d\n",(int)tID,x);
}

int main(int argc, char **argv) {
    pthread_t th[20]; // 20 thread için kimlik dizisi.
    int i;

    // Başlangıç değerini basar.
    printf("x = %d\n",x); // ÇIKTI: "x = 10"

    // 10 adet "arttir" thread'i oluşturulur.
    for (i = 0;i < 10;i++) {
        pthread_create(&th[i],NULL,arttir,(void*)i);
    }

    // 10 adet "azalt" thread'i oluşturulur.
    for (i = 10;i < 20;i++) {
        pthread_create(&th[i],NULL,azalt,(void*)i);
    }

    // Ana thread (main), 20 thread'in HEPSİNİN bitmesini bekler.
    for (i = 0;i < 20;i++) {
        pthread_join(th[i],NULL);
    }

    // !!! SONUÇ !!!
    // 20 thread'in tamamı işini bitirdikten sonra 'x'in son
    // değeri basılır.
    printf("x = %d\n",x); // ÇIKTI: 10 OLMAYACAK! (örn: 9, 11, 8...)
    printf("Bitti");
}
/*
 * ====================================================================
 * 💥 Kritik Sorun: Yarış Durumu (Race Condition)
 * ====================================================================
 *
 * Neden x'in son değeri 10 olmaz?
 *
 * Matematiksel Beklenti: x = 10
 * 10 kez ++x (+10)
 * 10 kez --x (-10)
 * Sonuç: 10 + 10 - 10 = 10 olmalı.
 *
 * Gerçekte Olan (Yarış Durumu):
 * ++x ve --x komutları tek bir CPU komutu değildir. İşlemci (CPU)
 * seviyesinde bu komutlar 3 adımdan oluşur (Buna Read-Modify-Write denir):
 *
 * 1. Oku (Read): x'in o anki değerini bellekten (RAM) CPU'nun
 * yazmacına (register) oku.
 * 2. Değiştir (Modify): Yazmaçtaki (register) değeri 1 arttır
 * veya 1 azalt.
 * 3. Yaz (Write): Yazmaçtaki (register) yeni değeri belleğe (RAM)
 * geri yaz.
 *
 * Felaket Senaryosu (Kayıp Güncelleme - Lost Update):
 * ---------------------------------------------------
 * x'in değeri şu an 10 olsun.
 *
 * 1. Thread A (arttir): x'in değerini okur. (A'nın yazmacında 10 var).
 * 2. (İşletim sistemi A'yı durdurur, B'ye geçer)
 * 3. Thread B (azalt): x'in değerini okur. (B'nin yazmacında 10 var).
 * 4. (İşletim sistemi B'yi durdurur, A'ya döner)
 * 5. Thread A: Yazmacındaki değeri 1 arttırır (Artık 11 var) ve
 * bunu belleğe yazar. (x'in değeri 11 oldu).
 * 6. (İşletim sistemi A'yı durdurur, B'ye döner)
 * 7. Thread B: (Bellekten tekrar okumaz!) Kendi yazmacındaki değeri
 * (hala 10) 1 azaltır (Artık 9 var) ve bunu belleğe yazar.
 * 8. SONUÇ: x'in son değeri 9 oldu.
 *
 * Bir "arttırma" ve bir "azaltma" işlemi yapıldı, ancak x'in değeri
 * 10 kalması gerekirken 9 oldu. Yani Thread A'nın yaptığı ++x
 * güncellemesi "kayboldu" (Lost Update).
 *
 * Kodunuzdaki sleep(2); komutu, bir thread'in (Adım 1) x'i
 * okuduktan sonra, (Adım 3) x'e yazmadan önce "uykuya dalmasını"
 * sağlar. Bu sırada diğer 19 thread'in araya girmesine izin verir ve
 * bu "Kayıp Güncelleme" senaryosunun yaşanmasını neredeyse garanti eder.
 *
 * ====================================================================
 * 🚀 Kodun Özeti
 * ====================================================================
 *
 * Bu kod, eşzamanlı erişimin tehlikelerini gösterir. Birden fazla
 * thread, global (paylaşımlı) bir kaynağı (x değişkeni) koruma
 * olmadan (senkronizasyon olmadan) değiştirmeye çalışmaktadır.
 *
 * ++x ve --x işlemlerinin atomic (bölünemez) olmaması nedeniyle,
 * thread'ler birbirlerinin güncellemelerinin üzerine yazar.
 *
 * Sonuç: Programın sonundaki x değeri belirlenemez (non-deterministic)
 * olacaktır. Matematiksel olarak 10 olması gerekirken, kaç adet
 * güncellemenin "kaybolduğuna" bağlı olarak 10'dan farklı
 * (9, 11, 8, 12...) bir değer çıkacaktır.
 *
 * ÇÖZÜM(GEREK YOK ANLATMADI HOCA): 'x'e erişmeden önce bir Mutex (pthread_mutex_t)
 * kullanılarak bölge kilitlenmeli (lock) ve işlem bittikten
 * sonra kilit açılmalıydı (unlock).
 * ====================================================================
 */
