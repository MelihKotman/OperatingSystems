//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // Standart Giriş/Çıkış işlemleri için (printf).
#include <stdlib.h>     // Genel yardımcı fonksiyonlar için.
#include <sys/types.h>  // Sistem veri tipleri için.
#include <fcntl.h>      // Dosya kontrol bayrakları için.
#include <unistd.h>     // POSIX API fonksiyonları (sleep, vs.).
#include <sys/wait.h>   // wait fonksiyonları için (prosesler arası).
#include <pthread.h>    // Thread'ler (iplikler) ve Mutex için ana kütüphane.

/*
 * ==========================================================
 * PAYLAŞILAN KAYNAKLAR (SHARED RESOURCES)
 * ==========================================================
 * DİKKAT: 20 thread'in hepsi bu iki değişkeni aynı bellek adresinde paylaşır.
 */
int x = 10;     // Kritik Veri: Değeri değiştirilen paylaşımlı değişken.

// Mutex (Kilit) Değişkeni: Kritik Bölgeyi korumak için kullanılır.
// PTHREAD_MUTEX_INITIALIZER: Mutex'i başlatmanın (init) en basit yolu.
pthread_mutex_t kilit = PTHREAD_MUTEX_INITIALIZER;

/*
 * 'arttir' - Arttırıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *arttir(void *arg) {
    int myID;
    myID = (int)arg;

    // Mutex'i KİLİTLE: Thread kritik bölgeye girmek için kilidi almaya çalışır.
    // Eğer kilit meşgulse (başka bir thread almışsa), thread burada bloke olur.
    pthread_mutex_lock(&kilit);

    // --- KRİTİK BÖLGE BAŞLANGICI ---
    ++x;        // Paylaşımlı veriyi güvenli bir şekilde değiştir (Atomic İşlem Garanti Edilir).
    printf("ID = %d\t x=%d\n",myID,x);
    // --- KRİTİK BÖLGE SONU ---

    // Mutex'i SERBEST BIRAK: Kilit açılır ve bekleyen diğer thread'lerin girmesine izin verilir.
    pthread_mutex_unlock(&kilit);

    // Thread'ler arasında ahengi bozmak ve yarış durumunu garantilemek için bekleme.
    sleep(2);

    return NULL;
}

/*
 * 'azalt' - Azaltıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *azalt(void *arg) {
    int myID;
    myID = (int)arg;

    // Mutex'i KİLİTLE: Thread kritik bölgeye girmek için kilidi almaya çalışır.
    pthread_mutex_lock(&kilit);

    // --- KRİTİK BÖLGE BAŞLANGICI ---
    --x;        // Paylaşımlı veriyi güvenli bir şekilde değiştir.
    printf("ID = %d\t x=%d\n",myID,x);
    // --- KRİTİK BÖLGE SONU ---

    // Mutex'i SERBEST BIRAK: Kilit açılır.
    pthread_mutex_unlock(&kilit);

    // Thread'ler arasında ahengi bozmak ve yarış durumunu garantilemek için bekleme.
    sleep(2);

    return NULL;
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
    printf("x = %d\n",x); // BEKLENEN: 10. (Mutex sayesinde artık 10 çıkması garanti edilir.)
    printf("Bitti");

    // Ana program sonlanır.
    return 0;
}

/*
 * ====================================================================
 * 📰 KODUN ÖZETİ VE AÇIKLAMASI: MUTEX İLE ÇÖZÜM
 * ====================================================================
 *
 * Bu kod, bir önceki derste başarısız olan yazılımsal kilit (`int y`) denemesinin aksine,
 * POSIX Mutex (Karşılıklı Dışlama) araçlarını kullanarak Kritik Bölge problemini doğru şekilde çözer.
 *
 * 📝 Amaç:
 * 10 kez artırma ve 10 kez azaltma işlemi sonucunda, Yarış Durumu olmadan 'x'in
 * matematiksel beklentiye uygun olarak **10** değerini koruduğunu göstermektir.
 *
 * 🔑 Çözüm Mekanizması (Mutex):
 * 1.  **Kilit Tanımı:** `pthread_mutex_t kilit` global olarak tanımlanmıştır.
 * 2.  **Kilit Alma:** Hem `arttir` hem de `azalt` fonksiyonları, paylaşılan `x` değişkenine
 * erişmeden hemen önce `pthread_mutex_lock(&kilit)` çağrısını yapar.
 * 3.  **Karşılıklı Dışlama:** Bu çağrı, **Atomik (Bölünmez)** bir işlemdir. Bu sayede,
 * bir thread kilidi aldığında, başka hiçbir thread'in Kritik Bölge'ye girmesine izin verilmez.
 * Önceki denemedeki gibi, iki thread'in aynı anda kilidi boş sanması sorunu ortadan kalkar.
 * 4.  **Kilit Bırakma:** Kritik bölgedeki işlem (`++x;` veya `--x;`) tamamlanır tamamlanmaz,
 * `pthread_mutex_unlock(&kilit)` çağrısı yapılarak kilit serbest bırakılır.
 *
 * 🎯 Sonuç:
 * Bu Mutex koruması sayesinde, `++x` ve `--x` işlemleri mantıksal olarak **sıralı** hale gelir.
 * Bu, **Karşılıklı Dışlama (Mutual Exclusion)** gereksiniminin sağlandığı ve programın sonucunun
 * her zaman **deterministik (öngörülebilir)** olacağı anlamına gelir. Finalde `x` değeri **10** olacaktır.
 *
 * ====================================================================
 */