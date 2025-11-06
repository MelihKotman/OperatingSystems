//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h> // Thread'ler (iplikler) için ana kütüphane.
                     // 'fork()'un aksine, bu kütüphane SÜREÇ (process) değil,
                     // İPLİK (thread) oluşturur.

/*
 * 'printHello' - HER BİR THREAD'İN (İPLİĞİN) ÇALIŞTIRACAĞI FONKSİYON
 * -----------------------------------------------------------------
 * Bu, 'fork()'taki gibi bir bellek kopyası DEĞİLDİR. Bu,
 * aynı bellek alanını paylaşan bir "iş parçacığı"dır.
 *
 * Yorumunuz: "1 argüman alır ve buna pointer tipinde göndermem gerek
 * ama birden fazlaysa struct şeklnde gönderebilirim"
 * Analiz: %100 DOĞRU. 'void*' jenerik bir pointer'dır. Tek bir değer
 * (int, long) gönderebilirsiniz, veya birden fazla veriyi bir 'struct'
 * içine koyup o 'struct'ın adresini gönderebilirsiniz.
 */
void *printHello(void *tID) { // Argüman olarak (void*)i değerini alır.

    // Yorumunuz: "İçerisindeki adres sekansı tutan bağımsız bir fonksiyondur..."
    // Analiz: Evet, 'myTID' değişkeni, her thread'in KENDİ ÖZEL YIĞIN (stack)
    // belleğinde oluşturulur. Bir thread'in 'myTID'si, diğerinin 'myTID'sinden
    // tamamen bağımsızdır.
    int myTID;

    // 'pthread_create' ile '(void*)i' olarak gönderilen 'i' değerini
    // (0, 1, 2, veya 3) geri 'int' tipine çeviriyoruz.
    myTID = (int) tID;

    //sleep(5);

    /*
     * EŞZAMANLILIK (CONCURRENCY) TAM BURADA OLUR:
     * 4 thread de bu 'printf' satırına ULAŞMAK İÇİN YARIŞIR.
     * İşletim sistemi (CPU zamanlayıcı), hangisinin ne zaman çalışacağına
     * karar verir.
     * Bu yüzden çıktı sırası (0,1,2,3) GARANTİ DEĞİLDİR.
     */
    printf("Hello World, my thread ID:%d\n",myTID);

    // Thread, 'pthread_join' ile kendisini bekleyene (main) 'myTID' değerini
    // (return (void*)5; gibi sabit bir değer de olabilirdi) geri döndürür.
    return (void*) myTID;

    // Alternatif çıkış: 'pthread_exit(5);' - Bu da 'return (void*)5;' ile
    // benzer bir iş yapar, 5 değerini döndürerek thread'i sonlandırır.
}

// YORUMUNUZ: "//int x = 10; //Buraya global bir değişken yazarsam..."
// ANALİZ: Evet, bu 'x' global olsaydı, 4 thread de AYNI 'x'i görürdü.
// (fork()'tan farkı bu!) Hepsi 'x = x + 1' yapsaydı, "Yarış Durumu"
// (Race Condition) oluşurdu ve 'x'in son değerinin 14 olmasının garantisi
// olmazdı (örn: 12 veya 13 çıkabilirdi).

int main() {
    pthread_t th[4]; // 4 thread'in kimliklerini (ID) tutacak dizi.
    int n;
    int i,rc; // rc: 'return code' (hata kodu) tutar.
    int b; // Thread'den 'return' ile dönen değeri (myTID) tutmak için.
    n = 4;

    /*
     * ==========================================================
     * 1. DÖNGÜ: THREAD OLUŞTURMA (pthread_create)
     * ==========================================================
     * 'main' thread'i, 4 adet "işçi" thread'i "ateşler".
     * Bu, 'fork()' gibi bloklayan bir komut değildir; thread'i başlatır
     * ve 'main' anında yoluna devam eder.
     */
    for (i = 0;i < n;i++) {
        // &th[i]     : Oluşturulan thread'in kimliğini bu diziye kaydet.
        // NULL       : Varsayılan thread ayarları.
        // printHello : Bu thread'in çalıştıracağı fonksiyon.
        // (void*)i   : O fonksiyona 'i' değerini (0, 1, 2, 3) argüman olarak gönder.
        rc = pthread_create(&th[i],NULL,printHello,(void*)i);

        // 'pthread_create' başarılı olursa 0 döndürür. Başarısız olursa
        // (örn: sistem limiti aşıldı) pozitif bir hata kodu döndürür.
        printf("rc = %d\n",rc);
    }

    /*
     * ==========================================================
     * 2. DÖNGÜ: SENKRONİZASYON (pthread_join) - BEKLEME
     * ==========================================================
     * 'main' thread'in, başlattığı işlerin bitmesini beklemesini sağlar.
     */
    for (i = 0;i < n;i++) {
        // 'th[i]' ID'li thread'in işini BİTİRMESİNİ BEKLE.
        // 'main' thread bu satırda BLOKLANIR (donar) ve 'th[i]' bitene
        // kadar bir sonraki 'i'ye geçmez.
        // &b: Dönen '(void*)myTID' değerini 'b' değişkenine kaydeder.
        pthread_join(th[i], &b);

        // YORUMUNUZ: pthread_join(th[0},NULL); //ikisi arasındaki farkı anlatalım
        // ANALİZ: 'pthread_join(th[i],...)' döngüsü HEPSİNİN bitmesini bekler.
        // Sadece 'pthread_join(th[0],...)' yazsaydınız (döngüsüz), main SADECE
        // ilk thread'in (th[0]) bitmesini bekler, sonra devam eder ve diğerlerini
        // (1,2,3) muhtemelen bitmeden öldürürdü. 'NULL' ise "dönen değer
        // umurumda değil" demektir.

        //printf("b = %d\n",b); // (Açsaydınız, 0, 1, 2, 3 basardı - sırasız!)
    }

    // YORUMUNUZDAKİ ALTERNATİF BEKLEME YÖNTEMİ
    //sleep(5);

    /*
     * ==========================================================
     * 3. BİTİŞ
     * ==========================================================
     * Bu satıra ancak 2. döngü (join) bittikten sonra gelinir.
     * Bu da, 4 thread'in de işini bitirdiğini GARANTİLER.
     */
    printf("Ana proses bitti...");
} // 'main' burada biter ve tüm süreci (prosesi) sonlandırır.

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ VE SENARYOLAR
 * ====================================================================
 *
 * Bu kod, 'fork()'tan (süreçler) farklı olarak 'pthread' (iplik)
 * kullanarak eşzamanlı programlamayı gösterir. İplikler,
 * 'fork'un aksine, belleği (global değişkenler gibi) PAYLAŞIR.
 *
 * GÖZLEM: "bu threadlerin birbiriyle sıralı olmasını beklerken bambaşka
 * sıralamada oldu."
 * Bu, Eşzamanlılığın (Concurrency) normal sonucudur. İşletim sistemi
 * 'printf' sırasını garanti etmez (Buna "Non-Determinism" denir).
 *
 * --------------------------------------------------------------------
 * SENARYO 1: KODUN MEVCUT HALİ (2. for AÇIK, sleep KAPALI)
 * --------------------------------------------------------------------
 * 'main', 4 thread'i oluşturur. 'pthread_join' döngüsü sayesinde
 * 4'ünün de bitmesini bekler. "Ana proses bitti..." en son yazar.
 * Bu, GÜVENLİ ve DOĞRU yöntemdir.
 *
 * --------------------------------------------------------------------
 * SENARYO 2: "hem sleep hem 2. for'u kaparsam" (JOIN YOK, SLEEP YOK)
 * --------------------------------------------------------------------
 * 'main', 4 thread'i oluşturur. BEKLEMEZ ('join' yok).
 * Anında 'printf("Ana proses bitti...");' satırına geçer.
 * 'main', 'return 0;' ile biter ve tüm süreci (prosesi) sonlandırır.
 *
 * SONUÇ: 'main', diğer 4 thread'e "Hello World" yazma FIRSATI VERMEDEN
 * programı bitirir. Muhtemelen hiç "Hello World" çıktısı görülmez.
 *
 * --------------------------------------------------------------------
 * SENARYO 3: 2. for (JOIN) KAPALI, ama 'sleep(5)' AÇIK
 * --------------------------------------------------------------------
 * 'main', 4 thread'i oluşturur. 'join' ile beklemez.
 * 'sleep(5);' satırına gelir ve 5 saniye UYUR.
 * Bu 5 saniyelik uyku, 4 "işçi" thread'e 'printf' yapmaları için
 * ZAMAN TANIR (çıktılar karışık sırada gelir).
 * 5 saniye sonra 'main' uyanır, "Ana proses bitti..." basar ve
 * (varsa) hala çalışan thread'leri sonlandırır.
 *
 * SONUÇ: Bu, 'join'in GÜVENİLMEZ bir taklididir. Ya iş 6 saniye
 * sürerse? 'pthread_join' ise süreye değil, işin bitmesine bakar.
 * ====================================================================
 */