//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>    // 'printf' için
#include <unistd.h>     // 'fork', 'getpid', 'getppid', 'sleep' için
#include <sys/wait.h>   // 'wait' fonksiyonları için (bu kodda aktif kullanılmıyor)

int main() {

    // Süreç ID'lerini tutacak değişkenler
    pid_t p0,p1,p2;
    // Tamsayı değişkeni, başlangıç değeri 10
    int x = 10;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---
    // (P0 - Ebeveyn, PID=5000, PPID=1234 varsayalım)

    // p0 değişkenine ebeveynin PID'i (5000) atanır.
    p0 = getpid();

    // Ebeveyn kendi bilgilerini basar. 'p0' ve 'getpid()' aynı değeri (5000) basar.
    // ÇIKTI: "pid = 5000   ppid = 5000   x = 10"
    printf("pid = %d\t ppid = %d\t x = %d\n",p0,getpid(),x);

    /*
     * ==========================================================
     * FORK #1: Süreç sayısı 2'ye çıkar.
     * ==========================================================
     * P0 (Ebeveyn, 5000) yeni bir süreç (C1, 5001) oluşturur.
     * 'x = 10' değeri C1'e kopyalanır.
     *
     * 'p1' değişkeninin değeri:
     * - P0 (Ebeveyn) için: 5001 (Çocuğun PID'i)
     * - C1 (Çocuk 1) için: 0
     */
    p1 = fork();

    /*
     * ==========================================================
     * SÜREÇ AYRIŞTIRMA
     * ==========================================================
     */
    if (p1 == 0) {
        // --- BU BLOK SADECE ÇOCUK 1 (C1 - 5001) TARAFINDAN ÇALIŞTIRILIR ---

        // C1, KENDİ 'x' kopyasının değerini 20 yapar.
        // Bu, P0'ın 'x'ini ASLA ETKİLEMEZ. (Bellek İzolasyonu)
        x = 20;

        // C1 kendi kimliğini basar.
        // ÇIKTI: "Cocuk surec: 5001   parent: 5000"
        printf("Cocuk surec: %d\t parent: %d\n",getpid(),getppid());
        //sleep(60);
    }
    else {
        // --- BU BLOK SADECE EBEVEYN (P0 - 5000) TARAFINDAN ÇALIŞTIRILIR ---

        /*
         * ==========================================================
         * FORK #2: P0 tekrar çatallanır. Toplam süreç sayısı 3'e çıkar.
         * ==========================================================
         * P0 (Ebeveyn, 5000) yeni bir süreç (C2, 5002) oluşturur.
         * 'x = 10' değeri (P0'ın mevcut değeri) C2'ye kopyalanır.
         *
         * 'p2' değişkeninin değeri:
         * - P0 (Ebeveyn) için: 5002 (C2'nin PID'i)
         * - C2 (Çocuk 2) için: 0
         */
        p2 = fork();

        /*
         * BU 'printf' SATIRI, Fork #2'den sonra HEM P0 HEM DE C2
         * TARAFINDAN ÇALIŞTIRILIR. (C1 buraya hiç girmez).
         */
        // P0 (5000) ÇIKTISI: "parent surec: 5000   parent: 1234"
        // C2 (5002) ÇIKTISI: "parent surec: 5002   parent: 5000"
        printf("parent surec: %d\t parent: %d\n",getpid(),getppid());
        //sleep(10);
    } // 'if-else' bloğunun sonu

    // --- BU BÖLÜMDEN İTİBAREN KOD 3 KEZ ÇALIŞIR ---
    // (P0, C1, ve C2 tarafından)

    /*
     * Bu son 'printf', 3 sürecin de 'x' değişkeninin son durumunu gösterir.
     *
     * 1. P0 (Ebeveyn, 5000):
     * 'if'e girmedi, 'else'e girdi. 'x'i hiç değişmedi.
     * ÇIKTI: "pid=5000   x = 10"
     *
     * 2. C1 (Çocuk 1, 5001):
     * 'if'e girdi, 'x'i 20 yaptı. 'else'e girmedi.
     * ÇIKTI: "pid=5001   x = 20"
     *
     * 3. C2 (Çocuk 2, 5002):
     * 'if'e girmedi, 'else' içinde P0 tarafından oluşturuldu.
     * 'x'i 10 olarak P0'dan miras aldı ve hiç değiştirmedi.
     * ÇIKTI: "pid=5002   x = 10"
     */
    printf("pid=%d\t x = %d\n",getpid(),x);

    // 3 sürecin hepsi gözlem yapabilmek için 30 saniye bekler.
    sleep(30);
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, "asimetrik süreç oluşturma" ve "bellek izolasyonu"
 * (Copy-on-Write) kavramlarını gösteren klasik bir örnektir.
 *
 * SÜREÇ AKIŞI:
 * 1. Ebeveyn (P0) başlar, x=10'dur.
 * 2. P0, 'fork()' ile C1'i oluşturur. (Toplam 2 süreç)
 * 3. 'if (p1 == 0)' kontrolü başlar:
 * - C1 (p1==0) 'if' bloğuna girer.
 * - P0 (p1!=0) 'else' bloğuna girer.
 * 4. 'if' bloğunda: C1, KENDİ 'x'ini 20 yapar ve biter.
 * 5. 'else' bloğunda: P0, 'fork()' ile C2'yi oluşturur. (Toplam 3 süreç)
 *
 * SÜREÇ AĞACI:
 * P0 (Ebeveyn), hem C1'in hem de C2'nin ebeveynidir. C1 ve C2 kardeştir.
 *
 * Terminal
 * |
 * P0 (Ebeveyn)
 * /      \
 * C1       C2
 *
 * BELLEK İZOLASYONU (ANA DERS):
 * 'x' değişkeni P0'da 10 olarak başlar.
 * C1 oluşturulduğunda 'x=10' kopyasını alır ve bunu '20' yapar.
 * C2 oluşturulduğunda P0'dan 'x=10' kopyasını alır.
 *
 * Sonuçta 3 süreç de son 'printf'i çalıştırdığında:
 * - P0'ın x'i 10'dur.
 * - C1'in x'i 20'dir.
 * - C2'nin x'i 10'dur.
 *
 * Bu, bir sürecin bellekte yaptığı değişikliğin (x=20) diğer
 * süreçleri etkilemediğini kanıtlar.
 * ====================================================================
 */