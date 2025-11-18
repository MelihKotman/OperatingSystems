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
// Mutex (Kilit) Değişkeni: Kritik Bölgeyi korumak için kullanılır.
pthread_mutex_t kilit = PTHREAD_MUTEX_INITIALIZER; // Mutex, Pthreads tarafından başlatıldı.

/*
 * 'arttir' - Arttırıcı thread'lerin çalıştıracağı fonksiyon.
 */
void *arttir(void *arg) {
    int myID;
    myID = (int)arg;

    // KİLİTLE: Mutex'i al (Başarılı: İçeri girer; Başarısız: Sonsuza dek bloke olur).
    pthread_mutex_lock(&kilit);

    // --- KRİTİK BÖLGE BAŞLANGICI ---
    ++x;        // Paylaşımlı veriyi değiştir (Bu işlem artık güvenli).
    printf("ID = %d\t x=%d\n",myID,x);
    // --- KRİTİK BÖLGE SONU ---

    // //pthread_mutex_unlock(&kilit); // <<< KRİTİK HATA NOKTASI: Kilit Bırakılmadı!

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

    // KİLİTLE: Mutex'i al.
    pthread_mutex_lock(&kilit);

    // --- KRİTİK BÖLGE BAŞLANGICI ---
    --x;        // Paylaşımlı veriyi değiştir.
    printf("ID = %d\t x=%d\n",myID,x);
    // --- KRİTİK BÖLGE SONU ---

    // //pthread_mutex_unlock(&kilit); // <<< KRİTİK HATA NOKTASI: Kilit Bırakılmadı!

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
        pthread_create(&th[i],NULL,arttir,(void*)i);
    }

    // 10 adet "azalt" thread'i oluşturulur.
    for (i = 10;i < 20;i++) {
        pthread_create(&th[i],NULL,azalt,(void*)i);
    }

    // Ana thread (main), 20 thread'in HEPSİNİN bitmesini bekler (Senkronizasyon).
    for (i = 0;i < 20;i++) {
        // pthread_join: Ana ipliğin, th[i] ipliğinin bitmesini beklemesini sağlar.
        // DİKKAT: Çoğu iplik, kilit açılmadığı için burada sonsuza dek takılı kalacaktır.
        pthread_join(th[i],NULL);
    }

    // !!! SONUÇ !!!
    // Bu satırlara muhtemelen asla ulaşılamayacaktır.
    printf("x = %d\n",x);
    printf("Bitti");

    // Ana program sonlanır.
    return 0;
}

/*
 * ====================================================================
 * 📰 KODUN ÖZETİ VE AÇIKLAMASI: KİLİTLENME HATASI (DEADLOCK)
 * ====================================================================
 *
 * Bu kod, Mutex'in pratik kullanımındaki en kritik hatayı göstermektedir: Kilit açma (unlock) işleminin unutulması.
 *
 * 1. Karşılıklı Dışlama (Mutex) Başarılı:
 * * İlk çalışan thread (örneğin ID=0) `pthread_mutex_lock(&kilit)` çağrısını başarıyla yapar.
 * * Bu thread kritik bölgeye girer ve `++x` işlemini güvenli bir şekilde gerçekleştirir.
 *
 * 2. Ölümcül Hata: Kilit Bırakılmıyor:
 * * `pthread_mutex_unlock(&kilit)` çağrısı yorum satırı yapıldığı için, ilk thread Kritik Bölge'den çıksa bile **kilidi asla serbest bırakmaz**.
 *
 * 3. Sonuç: Garanti Kilitlenme (Deadlock/Permanent Blockage):
 * * İlk thread'den sonra gelen diğer 19 thread'in tamamı, `pthread_mutex_lock(&kilit)` satırına ulaştığında, kilidin kalıcı olarak meşgul olduğunu görür.
 * * Bu thread'ler sonsuza dek kilit üzerinde bloke olur (Deadlock/Permanent Blockage).
 * * Ana fonksiyon (main) ise, `pthread_join` döngüsünde bu 19 bloke olmuş thread'in bitmesini sonsuza dek bekler.
 *
 * 💥 Özet: Bu örnek, bir önceki dersteki pratik kuralı doğrular: "Kilitlendiğin yerde, işin bitince kilidi bırakmak (unlock) zorundasın, yoksa sistem kilitlenir."
 *
 * ====================================================================
 */