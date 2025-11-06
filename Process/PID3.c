//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // 'printf' gibi G/Ç fonksiyonları için.
#include <sys/types.h>  // 'pid_t' veri tipi için.
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // 'getpid', 'getppid', 'fork', 'sleep' fonksiyonları için.

int main(int argc,char **argv) {

    // Süreç kimliklerini tutacak değişkenler tanımlanır.
    pid_t mypPid, myPid, ch1_pid;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---

    /*
     * 1. EBEVEYN BİLGİLERİNİ ALIR
     * Ebeveyn süreç, kendi PID'ini (örn: 5000) 'myPid' değişkenine atar.
     */
    myPid = getpid();

    /*
     * Ebeveyn süreç, kendi ebeveyninin (terminalin) PID'ini (örn: 1234)
     * 'mypPid' değişkenine atar.
     */
    mypPid = getppid();

    /*
     * ==========================================================
     * !!! KRİTİK NOKTA: FORK (ÇATALLANMA) !!!
     * ==========================================================
     * 'fork()' burada çağrılır. İşletim sistemi, ebeveyn sürecin
     * kopyasını (Çocuk Süreç) oluşturur.
     *
     * ÇOCUĞA MİRAS KALANLAR:
     * 'myPid' değişkeni (değeri 5000 olarak) çocuğa kopyalanır.
     * 'mypPid' değişkeni (değeri 1234 olarak) çocuğa kopyalanır.
     *
     * 'fork()' DÖNÜŞ DEĞERİ:
     * 1. EBEVEYN SÜRECE: Çocuğun PID'i (örn: 5001) 'ch1_pid'e atanır.
     * 2. YENİ ÇOCUK SÜRECE: '0' (sıfır) değeri 'ch1_pid'e atanır.
     */
    ch1_pid = fork();

    // --- BU BÖLÜMDEN İTİBAREN KOD 2 KEZ (HEM EBEVEYN HEM ÇOCUK) ÇALIŞIR ---

    /*
     * !!! KAVRAMSAL OLARAK EN ÖNEMLİ SATIR !!!
     * Bu 'printf' satırı 'fork()'tan SONRA olduğu için 2 kez çalışır.
     *
     * 1. Ebeveyn basar: "PID = 5000\n my pPID = 1234\n"
     * 2. Çocuk basar:   "PID = 5000\n my pPID = 1234\n"
     *
     * NEDEN? Çünkü çocuk süreç, 'getpid()' veya 'getppid()' fonksiyonlarını
     * HİÇ ÇAĞIRMADI. Bunun yerine, 'fork()' anında ebeveyninin
     * 'myPid' ve 'mypPid' değişkenlerinin o anki değerlerini
     * kendi belleğine kopyaladı (miras aldı).
     * Bu yüzden çocuk, kendi PID'i yerine EBEVEYNİNİN PID'ini basar.
     */
    printf("PID = %d\n my pPID = %d\n",myPid,mypPid);

    /*
     * Bu 'printf' satırı da 2 kez çalışır.
     * Hangi sürecin hangisi olduğunu bu satırdan anlarız.
     *
     * 1. Ebeveyn basar: "c1_pid = 5001" (Çocuğun gerçek PID'i)
     * 2. Çocuk basar:   "c1_pid = 0" ('fork()'un çocuğa dönüş değeri)
     */
    printf("c1_pid = %d\n",ch1_pid);

    /*
     * Hem ebeveyn süreç hem de çocuk süreç 20 saniye bekler.
     * Bu sırada 'ps -ef' ile süreç ağacını inceleyebiliriz.
     * İki sürecin de (5000 ve 5001) çalıştığını görebiliriz.
     */
    sleep(20);

    // Her iki süreç de (ebeveyn ve çocuk) başarıyla sonlanır.
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, 'fork()' sistem çağrısının "bellek kopyalama"
 * (copy-on-write) doğasını gösteren klasik bir İşletim Sistemleri
 * dersi örneğidir.
 *
 * Önceki örnekten (PID2.c) temel farkı, 'getpid()' ve 'getppid()'
 * fonksiyonlarının 'fork()' çağrısından ÖNCE çalıştırılmasıdır.
 *
 * Yaptığı iş şudur:
 * 1. Ebeveyn süreç, KENDİ PID ve PPID'sini öğrenir ve bunları
 * 'myPid' ile 'mypPid' değişkenlerine atar.
 * 2. 'fork()' çağrılır. Ebeveyn sürecin bir kopyası (çocuk) oluşturulur.
 * 3. 'myPid' ve 'mypPid' değişkenlerinin o anki değerleri, çocuğun bellek
 * alanına olduğu gibi KOPYALANIR (miras kalır).
 * 4. 'fork()', ebeveyne çocuğun PID'ini, çocuğa ise '0' döndürür.
 * 5. 'fork()'tan sonraki 'printf' komutları hem ebeveyn hem de
 * çocuk tarafından çalıştırılır.
 *
 * SONUÇ (ve TUZAK):
 * Her iki süreç de ilk 'printf' satırını çalıştırdığında,
 * ekrana EBEVEYNİN PID VE PPID'SİNİ basarlar. Çünkü çocuk süreç,
 * 'getpid()' fonksiyonunu kendisi çağırmamış, bu değerleri
 * ebeveynden 'fork' anında miras almıştır.
 *
 * İki sürecin farklı kimlikte olduğunu sadece 'c1_pid' değişkeninin
 * çıktısına bakarak anlayabiliriz (Ebeveyn için > 0, Çocuk için == 0).
 *
 * ====================================================================
 */