//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc,char **argv) {
    pid_t mypPid, myPid, ch1_pid;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---

    // Ebeveyn, kendi PID'ini (örn: 7000) 'myPid' değişkenine kaydeder.
    myPid = getpid();

    // Ebeveyn, kendi ebeveyninin (terminal) PID'ini (örn: 1234) 'mypPid'e kaydeder.
    mypPid = getppid();

    /*
     * ŞAŞIRTMACA SATIRI:
     * 'ch1_pid' değişkenine 20 atanır. ANCAK, bir sonraki satır
     * ('ch1_pid = fork()') bu değerin üzerine yazacağı için
     * bu satırın programın çalışmasına HİÇBİR ETKİSİ YOKTUR.
     */
    ch1_pid = 20;

    /*
     * ==========================================================
     * FORK (ÇATALLANMA): Süreç burada kopyalanır.
     * ==========================================================
     * 'fork()' çağrıldığı anda:
     * 1. 'myPid' (değeri 7000) ve 'mypPid' (değeri 1234) değişkenleri
     * Çocuk sürece olduğu gibi KOPYALANIR (Miras kalır).
     * 2. 'fork()' fonksiyonu 'ch1_pid' değişkenine değer atar:
     * - EBEVEYN'e: Çocuğun PID'ini (örn: 7001) atar.
     * - ÇOCUK'a: '0' (sıfır) değerini atar.
     */
    ch1_pid = fork();

    // --- BU BÖLÜMDEN İTİBAREN KOD 2 KEZ (HEM EBEVEYN HEM ÇOCUK) ÇALIŞIR ---

    /*
     * !!! ÖNEMLİ NOKTA !!!
     * Bu 'printf', 'fork()'tan ÖNCE alınan DEĞİŞKENLERİ basar.
     * Bu satır 2 kez çalışır:
     *
     * 1. Ebeveyn basar: "PID = 7000\n my pPID = 1234\n"
     * 2. Çocuk basar:   "PID = 7000\n my pPID = 1234\n"
     *
     * NEDEN? Çünkü çocuk, kendi PID'ini öğrenmek için 'getpid()'i
     * çağırmadı. Bunun yerine, 'fork' anında Ebeveyn'den miras aldığı
     * 'myPid' ve 'mypPid' değişkenlerinin DEĞERLERİNİ bastı.
     */
    printf("PID = %d\n my pPID = %d\n",myPid,mypPid);

    /*
     * Bu satır, hangi sürecin hangisi olduğunu ayırt etmemizi sağlar.
     * 1. Ebeveyn basar: "c1_pid = 7001" (Çocuğun PID'i)
     * 2. Çocuk basar:   "c1_pid = 0"
     */
    printf("c1_pid = %d\n",ch1_pid);

    // Hem ebeveyn (7000) hem de çocuk (7001) gözlem için 20 saniye bekler.
    sleep(20);
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ (PROGRAM 1)
 * ====================================================================
 * Bu program, 'fork()' anında "Durum Mirası" (State Inheritance)
 * kavramını gösterir.
 *
 * 'getpid()' ve 'getppid()' fonksiyonları 'fork()'tan ÖNCE çağrılır
 * ve sonuçları 'myPid', 'mypPid' değişkenlerine kaydedilir.
 * 'fork()' olunca, çocuk süreç bu değişkenleri ve içerdikleri DEĞERLERİ
 * (yani ebeveyninin PID/PPID'sini) miras alır.
 *
 * Sonuç olarak, 'fork()'tan sonra her iki süreç de aynı 'printf'i
 * çalıştırmasına rağmen, ikisi de ekrana EBEVEYNİNİN bilgilerini basar.
 * ====================================================================
 */
