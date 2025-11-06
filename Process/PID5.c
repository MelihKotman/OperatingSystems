//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

/*
 * Bu program ("Program 2"), 'fork()'tan SONRA çağrılan
 * 'getpid()' ve 'getppid()' fonksiyonlarının, her süreç için
 * nasıl farklı sonuçlar ürettiğini gösterir.
 */
int main(int argc,char **argv) {

    pid_t mypPid, myPid, ch1_pid;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞTIRILIR ---

    // Ebeveyn PID'ini alır (örn: 8000) ve 'myPid'e atar.
    // ANCAK, bu 'myPid' değişkeni aşağıdaki 'printf'te HİÇ KULLANILMIYOR.
    myPid = getpid();

    // Ebeveyn PPID'ini alır (örn: 1234) ve 'mypPid'e atar.
    // ANCAK, bu 'mypPid' değişkeni aşağıdaki 'printf'te HİÇ KULLANILMIYOR.
    mypPid = getppid();

    // Bu (ch1_pid = 20;) satırının 'fork()'tan hemen önce olması
    // nedeniyle HİÇBİR ETKİSİ YOKTUR, bir sonraki satırda üzerine yazılır.
    ch1_pid = 20;

    // ==========================================================
    // FORK (ÇATALLANMA): Süreç burada kopyalanır.
    // ==========================================================
    // 'fork()' DÖNÜŞ DEĞERİ ('ch1_pid'e atanan):
    // 1. EBEVEYN'e: Çocuğun PID'i (örn: 8001).
    // 2. ÇOCUK'a: '0' (sıfır).
    ch1_pid = fork();

    // --- BU BÖLÜMDEN İTİBAREN KOD 2 KEZ ÇALIŞIR (EBEVEYN VE ÇOCUK) ---

    // !!! KRİTİK FARK BURADA !!!
    // 'printf' fonksiyonu, 'fork()'tan SONRA çağrılır VE
    // 'getpid()', 'getppid()' fonksiyonlarını O ANDA YENİDEN ÇAĞIRIR.
    //
    // Bu 'printf' 2 kez çalışır:
    //
    // 1. Ebeveyn (8000) çalıştırır:
    //    'getpid()' -> 8000 döndürür.
    //    'getppid()' -> 1234 döndürür (Terminalin PID'i).
    //    Ekrana basar: "PID = 8000\n my pPID = 1234\n"
    //
    // 2. Çocuk (8001) çalıştırır:
    //    'getpid()' -> 8001 döndürür (KENDİ PID'i).
    //    'getppid()' -> 8000 döndürür (EBEVEYNİNİN PID'i).
    //    Ekrana basar: "PID = 8001\n my pPID = 8000\n"
    //
    printf("PID = %d\n my pPID = %d\n", getpid(), getppid());

    // Bu satır da 2 kez çalışır ve hangi sürecin hangisi olduğunu gösterir:
    // 1. Ebeveyn basar: "c1_pid = 8001" (Çocuğun PID'i)
    // 2. Çocuk basar:   "c1_pid = 0"
    printf("c1_pid = %d\n", ch1_pid);

    // Her iki süreç de (ebeveyn ve çocuk) gözlem yapabilmek için 20 saniye bekler.
    sleep(20);

    // Her iki süreç de başarıyla sonlanır.
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ (PROGRAM 2)
 * ====================================================================
 * Bu program, "Ayrı Yürütme Bağlamı" (Separate Execution Context)
 * kavramını gösterir.
 *
 * 'fork()' gerçekleştikten sonra, ebeveyn ve çocuk birbirinden
 * bağımsız süreçler haline gelir.
 *
 * 'printf' komutu, 'fork()'tan SONRA çağrıldığı ve 'getpid()', 'getppid()'
 * fonksiyonlarını o an YENİDEN çağırdığı için:
 *
 * - Ebeveyn: Kendi güncel PID/PPID'sini alır.
 * - Çocuk:   Kendi güncel PID/PPID'sini alır.
 *
 * Sonuç olarak, her iki süreç de ekrana kendi (farklı) kimlik
 * bilgilerini basar.
 * ====================================================================
 */