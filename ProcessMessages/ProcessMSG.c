//
// Created by Melih Yiğit Kotman
//
#include <string.h>
#include <time.h>
#include <sys/ipc.h>    // IPC için ana kütüphane (ftok, msgget, shmget, semget)
#include <sys/wait.h>
#include <sys/errno.h>  // 'errno' (hata kodu) değişkenine erişim için
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>    // Mesaj Kuyrukları (msgget) için
#include <sys/sem.h>    // Semaforlar (semget) için
#include <sys/shm.h>    // Paylaşımlı Bellek (shmget) için

int main(int argc, char **argv) {
    key_t myKey;     // Sistem çapında (system-wide) benzersiz bir "anahtar"
                     // tutacak değişken. Bu, IPC objelerinin "adresi" gibidir.
    int msgID, shmID,semID; // Oluşturulan IPC objelerinin "kimliklerini"
                         // (tanımlayıcılarını) tutacak değişkenler.

    /*
     * ==========================================================
     * 1. ANAHTAR OLUŞTURMA (ftok - File to Key)
     * ==========================================================
     * IPC objeleri, bir 'anahtar' (key) ile bulunur.
     * 'ftok', bir dosya yolundan ve bir karakterden benzersiz
     * bir anahtar üretir.
     *
     * ÖNEMLİ: Aynı dosya yolunu ve aynı karakteri kullanan
     * BÜTÜN prosesler, AYNI 'myKey' değerini elde eder.
     * Prosesler birbirlerini bu şekilde bulur.
     */
    // "/home/user/.bashrc" dosyasını (var olan bir dosya olmalı)
    // ve 'X' karakterini kullanarak bir anahtar üret.
    myKey = ftok("/home/user/.bashsrc",'X'); //myKey = ftok("./",'X');

    // 'ftok' -1 dönerse (örn: dosya bulunamadı) hata ver ve çık.
    if (myKey == -1) {
        printf("Ftok failed with errno = %d\n",errno);
        return -1;
    }

    // Üretilen anahtarı (hexadecimal) olarak bas.
    printf("IPC_Key = %x\n",myKey);

    /*
     * ==========================================================
     * 2. MESAJ KUYRUĞU OLUŞTURMA (msgget)
     * ==========================================================
     * 'msgget', o anahtara (myKey) bağlı bir mesaj kuyruğu
     * oluşturur veya var olana erişir.
     */
    // 'myKey' anahtarını kullan
    // '0666': Okuma/Yazma izinleri (herkes için)
    // 'IPC_CREAT': Eğer bu anahtarla bir kuyruk YOKSA, YENİ BİR TANE OLUŞTUR.
    // 'IPC_EXCL': (IPC_CREAT ile kullanılır) Eğer bu kuyruk ZATEN VARSA,
    //             hata ver (-1 döndür). Bu, "sadece ben oluşturayım" demektir.
    msgID = msgget(myKey, 0666 | IPC_CREAT | IPC_EXCL);

    /*
     * ==========================================================
     * 3. PAYLAŞIMLI BELLEK OLUŞTURMA (shmget)
     * ==========================================================
     * 'shmget', o anahtara bağlı bir paylaşımlı bellek
     * segmenti oluşturur.
     */
    // 'myKey' anahtarını kullan
    // '50': 50 byte büyüklüğünde bir bellek alanı oluştur.
    // '0666 | IPC_CREAT | IPC_EXCL': Aynı mantık, yoksa oluştur, varsa hata ver.
    shmID = shmget(myKey, 50, 0666 | IPC_CREAT | IPC_EXCL);

    /*
     * ==========================================================
     * 4. SEMAFOR OLUŞTURMA (semget)
     * ==========================================================
     * 'semget', o anahtara bağlı bir semafor (veya semafor seti)
     * oluşturur. Semaforlar, paylaşımlı belleğe erişimi
     * senkronize etmek (Yarış Durumunu engellemek) için kullanılır.
     */
    // 'myKey' anahtarını kullan
    // '1': 1 adet semafordan oluşan bir "set" oluştur.
    // '0666 | IPC_CREAT': Yoksa oluştur.
    // DİKKAT: Burada 'IPC_EXCL' kullanılmamış. Yani semafor zaten
    // varsa, bu komut hata vermez, var olanın ID'sini döndürür.
    semID = semget(myKey, 1, 0666 | IPC_CREAT);

    // 'semget' hata verirse (örn: izinler yetmedi)
    if (semID == -1) {
        printf("Semget failed with errno = %d\n",errno);
        return -1;
    }

    // Oluşturulan 3 IPC objesinin de benzersiz "kimliklerini" bas.
    // Diğer prosesler bu ID'leri kullanarak (key'i değil) veri yollayacak.
    printf("msgID = %d\t shmID = %d\t semID = %d\n",msgID, shmID, semID);
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Ne Öğrendik?)
 * ====================================================================
 *
 * Bu kod, "Prosesler Arası Haberleşme" (IPC) için gerekli altyapıyı
 * (infrastructure) kurar. Bu programı bir "sunucu" (server) veya
 * "başlatıcı" (initializer) olarak düşünebilirsiniz.
 *
 * 1. 🔑 ftok (Anahtar Üretici):
 * Bu, tüm olayın kilit noktasıdır. 'ftok', sistemdeki var olan
 * bir dosyayı kullanarak, tüm proseslerin "buluşabileceği" ortak
 * bir 'anahtar' (myKey) üretir. Bu, IPC objelerinin "telefon
 * numarası" veya "adresi" gibidir.
 *
 * 2. 📦 msgget (Mesaj Kuyruğu):
 * Bu anahtarı kullanarak, proseslerin birbirine (posta kutusu gibi)
 * mesajlar (veri paketleri) yollayabileceği bir 'Mesaj Kuyruğu'
 * oluşturur. 'msgID' bu kuyruğun kimliğidir.
 *
 * 3. 🧠 shmget (Paylaşımlı Bellek):
 * Aynı anahtarı kullanarak, 50 byte'lık bir 'Paylaşımlı Bellek'
 * alanı oluşturur. Bu, 'fork()'un bellek izolasyonunu aşar.
 * Buraya yazılan veriyi, bu belleğe erişen diğer tüm prosesler
 * anında görür. Çok hızlıdır ancak tehlikelidir (Yarış Durumu).
 * 'shmID' bu belleğin kimliğidir.
 *
 * 4. 🚦 semget (Semafor - Trafik Lambası):
 * Aynı anahtarı kullanarak, 'Paylaşımlı Bellek' gibi kaynakları
 * korumak için bir 'Semafor' (trafik lambası) oluşturur. Semafor,
 * paylaşımlı belleğe aynı anda SADECE BİR prosesin yazmasını
 * garanti altına alır, böylece 'Race Condition' engellenir.
 * 'semID' bu semaforun kimliğidir.
 *
 * 5. IPC_CREAT vs. IPC_EXCL
 * - 'IPC_CREAT': "Yoksa oluştur."
 * - 'IPC_CREAT | IPC_EXCL': "Yoksa oluştur, ama ZATEN VARSA HATA VER."
 * Bu 'EXCL' bayrağı, programı birden fazla kez çalıştırıp
 * "zaten var" hatası alarak objeyi tekrar oluşturmayı engellemek
 * için kullanılır.
 * ====================================================================
 */