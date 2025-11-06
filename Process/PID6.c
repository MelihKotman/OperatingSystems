//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // fork, getpid, getppid, sleep için.

int main(int argc,char **argv) {

    // Süreç kimliklerini tutacak değişkenler.
    pid_t mypPid, myPid, ch1_pid,ch2_pid;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---
    // (P1 - Ebeveyn, PID=8000, PPID=1234 varsayalım)

    // Ebeveyn PID'ini alır (myPid = 8000).
    // ANCAK, bu değişken aşağıdaki 'printf'te HİÇ KULLANILMIYOR.
    myPid = getpid();

    // Ebeveyn PPID'ini alır (mypPid = 1234).
    // ANCAK, bu değişken aşağıdaki 'printf'te HİÇ KULLANILMIYOR.
    mypPid = getppid();

    // Bu satırın hiçbir etkisi yok, bir sonraki satırda üzerine yazılacak.
    ch1_pid = 20;

    /*
     * ==========================================================
     * FORK #1: Süreç sayısı 2'ye çıkar.
     * ==========================================================
     * P1 (Ebeveyn, 8000) yeni bir süreç (C1, 8001) oluşturur.
     *
     * 'ch1_pid' değişkeninin değeri:
     * - P1 (Ebeveyn) için: 8001 (Çocuğun PID'i)
     * - C1 (Çocuk 1) için: 0
     */
    ch1_pid = fork();

    /*
     * ==========================================================
     * FORK #2: Süreç sayısı 4'e çıkar.
     * ==========================================================
     * Bu satır, hem P1 (Ebeveyn) hem de C1 (Çocuk 1) tarafından
     * AYRI AYRI çalıştırılır.
     *
     * 1. P1 (Ebeveyn, 8000) çalıştırır:
     * - C2 (Çocuk 2, 8002) oluşur.
     * - P1 için: 'ch2_pid' = 8002
     * - C2 için: 'ch2_pid' = 0
     *
     * 2. C1 (Çocuk 1, 8001) çalıştırır:
     * - C3 (Çocuk 3, 8003) oluşur.
     * - C1 için: 'ch2_pid' = 8003
     * - C3 için: 'ch2_pid' = 0
     *
     * ARTIK TOPLAM 4 SÜREÇ VAR: P1, C1, C2, C3
     */
    ch2_pid = fork();

    // --- BU BÖLÜMDEN İTİBAREN KOD 4 KEZ ÇALIŞIR ---
    // (P1, C1, C2, ve C3 tarafından)

    /*
     * Bu 'printf', 'fork()'lardan SONRA ve 'getpid()' fonksiyonlarını
     * o an ÇAĞIRARAK çalıştığı için, 4 süreç de KENDİ
     * benzersiz kimliklerini basar.
     *
     * ÇIKTILAR (Sırası karışık olabilir):
     * P1: "PID = 8000\n my pPID = 1234\n"
     * C1: "PID = 8001\n my pPID = 8000\n" (Ebeveyni P1)
     * C2: "PID = 8002\n my pPID = 8000\n" (Ebeveyni P1)
     * C3: "PID = 8003\n my pPID = 8001\n" (Ebeveyni C1)
     */
    printf("PID = %d\n my pPID = %d\n",getpid(),getppid());

    /*
     * Bu 'printf' de 4 kez çalışır ve 'ch1_pid' değişkeninin
     * o anki değerini basar.
     *
     * ÇIKTILAR (Sırası karışık olabilir):
     * P1: "c1_pid = 8001" (Fork #1'den aldığı değer)
     * C1: "c1_pid = 0"   (Fork #1'den aldığı değer)
     * C2: "c1_pid = 8001" (Bu değeri P1'den miras aldı)
     * C3: "c1_pid = 0"   (Bu değeri C1'den miras aldı)
     */
    printf("c1_pid = %d\n",ch1_pid);

    // 4 sürecin hepsi gözlem yapabilmek için 20 saniye bekler.
    sleep(20);

    // 4 sürecin hepsi başarıyla sonlanır.
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, ardışık 'fork()' çağrılarının süreç sayısını
 * nasıl üssel (exponential) olarak artırdığını gösterir ($2^n$).
 *
 * 1. Başlangıçta 1 Ebeveyn (P1) süreç vardır.
 * 2. İlk 'fork()' çağrılır. P1, C1'i oluşturur. Süreç sayısı 2 olur (P1, C1).
 * 3. İkinci 'fork()' çağrılır. Bu satır artık 2 süreç (P1 ve C1)
 * tarafından çalıştırılır:
 * - P1, C2'yi oluşturur.
 * - C1, C3'ü oluşturur.
 * 4. Toplam süreç sayısı 4'e (P1, C1, C2, C3) ulaşır.
 *
 * SONUÇ:
 * 'printf' ve 'sleep' komutları toplam 4 kez, her bir bağımsız
 * süreç tarafından çalıştırılır.
 *
 * Süreç Ağacı Şöyledir:
 *
 * Terminal (1234)
 * |
 * P1 (8000)
 * /      \
 * C1 (8001)   C2 (8002)
 * |
 * C3 (8003)
 *
 * 'printf' çıktıları (özellikle PPID'ler) bu ağaç yapısını
 * doğrulamamızı sağlar.
 * ====================================================================
 */