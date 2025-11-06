//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>  // 'pid_t' veri tipi için.
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // fork, getpid, getppid, sleep için.

int main(int argc,char **argv) {

    // Süreç kimliklerini tutacak değişkenler.
    pid_t mypPid, myPid, ch1_pid,ch2_pid;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---
    // (P1 - Ebeveyn, PID=10000, PPID=1234 varsayalım)

    // Ebeveyn PID'ini alır (myPid = 10000).
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
     * P1 (Ebeveyn, 10000) yeni bir süreç (C1, 10001) oluşturur.
     *
     * 'ch1_pid' değişkeninin değeri:
     * - P1 (Ebeveyn) için: 10001 (Çocuğun PID'i)
     * - C1 (Çocuk 1) için: 0
     */
    ch1_pid = fork();

    /*
     * ==========================================================
     * KOŞULLU FORK #2
     * ==========================================================
     * Bu 'if' bloğu artık 2 süreç tarafından kontrol edilir:
     *
     * 1. P1 (Ebeveyn) kontrol eder:
     * 'ch1_pid' (10001) != 0 mı? EVET.
     * P1 bu 'if' bloğuna GİRER.
     *
     * 2. C1 (Çocuk 1) kontrol eder:
     * 'ch1_pid' (0) != 0 mı? HAYIR.
     * C1 bu 'if' bloğunu ATLAR.
     */
    if (ch1_pid != 0) {

        // SADECE P1 (Ebeveyn, 10000) bu satırı çalıştırır.
        // P1, ikinci bir çocuk (C2, 10002) oluşturur.
        //
        // 'ch2_pid' değişkeninin değeri:
        // - P1 (Ebeveyn) için: 10002 (İkinci çocuğunun PID'i)
        // - C2 (Çocuk 2) için: 0
        ch2_pid = fork();
    }

    // --- BU BÖLÜMDEN İTİBAREN KOD 3 KEZ ÇALIŞIR ---
    // (P1, C1, ve C2 tarafından)

    /*
     * Bu 'printf', 'getpid()' fonksiyonlarını o an ÇAĞIRDIĞI için,
     * 3 süreç de KENDİ benzersiz kimliklerini basar.
     *
     * ÇIKTILAR (Sırası karışık olabilir):
     * P1: "PID = 10000\n my pPID = 1234\n" (Ebeveyni Terminal)
     * C1: "PID = 10001\n my pPID = 10000\n" (Ebeveyni P1)
     * C2: "PID = 10002\n my pPID = 10000\n" (Ebeveyni P1)
     */
    printf("PID = %d\n my pPID = %d\n",getpid(),getppid());

    /*
     * Bu 'printf' de 3 kez çalışır ve 'ch1_pid' değişkeninin
     * o anki değerini basar.
     *
     * ÇIKTILAR (Sırası karışık olabilir):
     * P1: "c1_pid = 10001" (Fork #1'den aldığı değer)
     * C1: "c1_pid = 0"    (Fork #1'den aldığı değer)
     * C2: "c1_pid = 10001" (Bu değeri ebeveyni P1'den MİRAS ALDI)
     */
    printf("c1_pid = %d\n",ch1_pid);

    // 3 sürecin hepsi gözlem yapabilmek için 20 saniye bekler.
    sleep(20);

    // 3 sürecin hepsi başarıyla sonlanır.
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, "koşullu çatallanma" kullanarak Ebeveyn sürecin
 * birden fazla çocuk oluşturmasını sağlar.
 *
 * SÜREÇ AKIŞI:
 * 1. Başlangıçta 1 Ebeveyn (P1) süreç vardır.
 * 2. İlk 'fork()' çağrılır. P1, C1'i oluşturur. (Toplam 2 süreç: P1, C1).
 * 3. 'if (ch1_pid != 0)' kontrolü gelir:
 * - P1'in 'ch1_pid' değeri > 0 olduğu için 'if'e GİRER.
 * - C1'in 'ch1_pid' değeri == 0 olduğu için 'if'e GİRMEZ.
 * 4. 'if' bloğunun içindeki ikinci 'fork()', SADECE P1 tarafından
 * çalıştırılır.
 * 5. P1, ikinci çocuğu olan C2'yi oluşturur. (Toplam 3 süreç: P1, C1, C2).
 *
 * SONUÇ:
 * 'printf' ve 'sleep' komutları toplam 3 kez, her bir bağımsız
 * süreç tarafından çalıştırılır.
 *
 * Süreç Ağacı Bir "Zincir" Değil, "Ağaç" Şeklindedir:
 *
 * Terminal (1234)
 * |
 * P1 (10000)
 * /      \
 * C1 (10001)   C2 (10002)
 *
 * 'printf' çıktısındaki PPID değerleri (C1 ve C2'nin PPID'si
 * aynı P1'in PID'idir) bu ağaç yapısını kanıtlar.
 * ====================================================================
 */