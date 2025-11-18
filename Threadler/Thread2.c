//
// Created by Melih Yiğit Kotman
//

#include <stdio.h>      // Standart Giriş/Çıkış işlemleri için (printf).
#include <sys/types.h>  // Sistem veri tipleri için (pid_t, vs. - pthreads için dolaylı olarak gerekli).
#include <unistd.h>     // POSIX API fonksiyonları için (sleep, vs.).
#include <sys/wait.h>   // wait fonksiyonları için (prosesler arası, thread'lerde doğrudan kullanılmaz).
#include <pthread.h>    // Thread'ler (iplikler) için ana kütüphane.

/*
 * ==========================================================
 * PAYLAŞILAN KAYNAK (SHARED RESOURCE)
 * ==========================================================
 * Thread'ler, bu global değişkeni (x) aynı bellek adresinde paylaşır.
 * Eğer burası değiştirilirse (x++), Yarış Durumu oluşur.
 */
int x = 10;

/*
 * ==========================================================
 * İPLİK RUTİNİ (THREAD ROUTINE)
 * ==========================================================
 * Her iplik oluşturulduğunda bu fonksiyonu çalıştırır.
 * arg parametresi, ipliğin ID'sidir (0'dan 3'e kadar).
 */
void *threadRoutine(void *arg) {
    int y; // Her ipliğin kendine ait yerel (local) değişkeni.

    // //sleep(15);
    // // NOT: Bu yorum satırı açılsaydı, Yarış Durumu riskini çok artırırdı.

    // Global x değerini ve iplik ID'sini ekrana basar.
    // DİKKAT: Global x sadece okunuyor, değiştirilmiyor.
    printf("Thread ID = %d\t x = %d\n",(int) arg,x);

    // Basit bir hesaplama: Kendi ID'si ile global x'i çarpar.
    // Hesaplama sonucu sadece yerel 'y' değişkeninde kalır.
    y = (int) arg * x;

    // //pthread_exit(22); // Bu çağrı açılsaydı, iplik bu noktada sonlanırdı.
    // Fonksiyon sonlandığında, iplik otomatik olarak sona erer (return ile).
}

/*
 * ==========================================================
 * ANA FONKSİYON (MAIN)
 * ==========================================================
 */
int main(int argc,char **argv) {
    pthread_t th[4]; // 4 adet iplik kimliğini (ID) tutacak dizi.
    int i,b,c;       // Döngü sayacı (i), pthread_join'den gelen değer (b), hata kodu (c).

    // 4 adet iplik oluşturma döngüsü.
    for (i = 0;i < 4;i++) {
        // pthread_create: Yeni bir iplik oluşturur.
        // &th[i]: Yeni ipliğin ID'si bu adrese yazılır.
        // NULL: İpliğin nitelikleri (attribute) (varsayılan).
        // threadRoutine: İpliğin çalıştıracağı fonksiyondur.
        // (void*)i: threadRoutine fonksiyonuna parametre olarak 'i' değerini (iplik ID'si) gönderir.
        c = pthread_create(&th[i],NULL,threadRoutine,(void*)i);

        // iplik oluşturma işleminin durumunu (r=döngü, c=hata kodu) ekrana basar.
        printf("r = %d\t c = %d\n",i,c);
    }

    // İpliklerin bitmesini bekleme döngüsü (Senkronizasyon).
    for (i = 0;i < 4;i++) {
        // pthread_join: Ana ipliğin, th[i] ipliğinin bitmesini bekler.
        // &b: İpliğin dönüş değerini (exit status) bu adrese kaydeder (burada kullanılmıyor).
        pthread_join(th[i],&b);

        // İpliğin dönüş değerini basar.
        printf("b = %d\n",b);
    }

    // //pthread_exit(NULL); // Bu çağrı açılsaydı, sadece main ipliği sonlanır, diğer iplikler bitene kadar sistem çalışmaya devam ederdi.

    // Tüm iplikler bittikten sonra ana program normal şekilde sonlanır.
    return 0;
}
/*
 * ====================================================================
 * 📰 KODUN ÖZETİ VE AÇIKLAMASI
 * ====================================================================
 *
 * Bu C kodu, POSIX Pthreads kütüphanesini kullanarak 4 adet eşzamanlı iplik (thread)
 * oluşturan temel bir şablondur.
 *
 * 📝 Amaç:
 * Kodun temel amacı, iplik oluşturma ve ipliklerin paylaşılan global bir kaynağa
 * (int x = 10;) nasıl eriştiğini göstermektir.
 *
 * ⚙️ Ana İşlevler:
 * 1. İplik Oluşturma (pthread_create): main fonksiyonu bir döngüde 4 farklı iplik oluşturur.
 * 2. Paylaşılan Kaynak: Tüm iplikler bellekteki AYNI 'x' değişkenine erişir.
 * 3. İplik Rutini (threadRoutine): Her iplik, kendi ID'sini ve o anki global 'x' değerini okur ve ekrana yazar.
 * 4. İplik Birleştirme (pthread_join): main fonksiyonundaki ikinci döngü, ana programın,
 * oluşturduğu 4 ipliğin HEPSİNİN işini bitirmesini beklemesini sağlar. Bu, iplikler arası bir senkronizasyon mekanizmasıdır.
 *
 * 💡 Durum:
 * Bu haliyle kod, paylaşılan 'x' değişkenini SADECE OKUDUĞU (değiştirmediği) için herhangi
 * bir YARIŞ DURUMUNA (Race Condition) neden olmaz. Ancak bu kod, hocanızın KRİTİK BÖLGE
 * problemlerini çözmek için kullanacağı ve daha sonra '++x;' veya '--x;' gibi değiştirme
 * işlemleri ekleyeceği TEMEL BAŞLANGIÇ NOKTASIDIR.
 * * ====================================================================
 */