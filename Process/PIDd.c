//
// Created by Melih Yiğit Kotman
//
/*
 * Bu kod, bir önceki 'int x = 0;' örneğiyle AYNI sonuca sahiptir.
 * Bu kez değişken 'heap' (malloc ile) üzerinde oluşturulmuştur.
 * Amaç: 'fork()'un 'heap' belleğini de 'stack' gibi
 * kopyaladığını (bellek izolasyonu) göstermektir.
*/
#include <stdio.h>
#include <stdlib.h>     // 'malloc' fonksiyonu için bu kütüphane gereklidir.
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>    // 'pthread' kütüphanesi. (Bu kodda kullanılmıyor).

int main(int argc, char **argv) {

    // 'int x = 0;' YERİNE:
    // 1. Heap'ten 4 byte (bir integer için) yer ayırıyoruz.
    // 'x' bu bellek alanının ADRESİNİ tutan bir pointer'dır.
    int *x = malloc(4);

    // 2. O adresteki 'değere' 0 atıyoruz.
    *x = 0;

    pid_t main_pid, ch1_pid, ch2_pid; // ch2_pid HİÇ KULLANILMIYOR.

    // --- EBEVEYN BAŞLANGICI ---
    // (P1 - Ebeveyn, PID=7000 varsayalım)

    main_pid = getpid();
    printf("PID: %d\n",main_pid); // ÇIKTI: "PID: 7000"

    /*
     * ==========================================================
     * FORK (ÇATALLANMA): Süreç kopyalanır.
     * ==========================================================
     * P1 (Ebeveyn, 7000) yeni bir süreç (C1, 7001) oluşturur.
     *
     * ÖNEMLİ: C1 (Çocuk), P1'in 'x' pointer'ının (adresi tutan)
     * bir KOPYASINI alır. Aynı zamanda 'x'in işaret ettiği
     * heap belleğinin (*x = 0) de bir KOPYASINI alır.
     *
     * 'ch1_pid' değişkeninin değeri:
     * - P1 (Ebeveyn) için: 7001 (Çocuğun PID'i)
     * - C1 (Çocuk 1) için: 0
     */
    ch1_pid = fork();

    if (ch1_pid == 0) {

        // --- SADECE ÇOCUK SÜREÇ (C1 - 7001) ---

        // C1, KENDİ 'x' pointer'ının gösterdiği KENDİ bellek kopyasına 10 yazar.
        // Bu, EBEVEYNİN heap belleğini ETKİLEMEZ.
        *x = 10;

        printf("Bu cocuk proses: %d\n",getpid()); // ÇIKTI: "Bu cocuk proses: 7001"
    }
    else {

        // --- SADECE EBEVEYN SÜREÇ (P1 - 7000) ---

        // P1, KENDİ 'x' pointer'ının gösterdiği KENDİ bellek alanını okur.
        // C1'in değişikliği P1'i etkilemediği için değer hala 0'dır.
        printf("x = %d\n",*x); // ÇIKTI: "x = 0"

        printf("Bu ana proses: %d\n",getpid()); // ÇIKTI: "Bu ana proses: 7000"
    }

    // --- HER İKİ SÜREÇ (P1 VE C1) ---

    // P1 "x = 0" basar.
    // C1 "x = 10" basar.
    // Bu, bellek izolasyonunun 'heap' için de geçerli olduğunu kanıtlar.
    printf("ID = %d\t x = %d\n",getpid(),*x);

    sleep(10); // Gözlem için bekle.
    return 0;
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, bir önceki 'int x = 0' (stack) örneğiyle aynı
 * 'Bellek İzolasyonu' kavramını, bu kez 'malloc' (heap)
 * kullanarak gösterir.
 *
 * 1. Ebeveyn (Ana) süreç, 'heap' üzerinde bir bellek alanı ayırır
 * ve 'x' pointer'ı aracılığıyla bu alana '0' yazar.
 * 2. fork() çağrılır. Çocuk süreç, ebeveynin tüm bellek alanının
 * (stack, heap, vs.) bir kopyasını alır.
 * 3. Çocuk Süreç (if (ch1_pid == 0)): Kendi 'x' pointer'ının
 * gösterdiği KENDİ heap kopyasına '10' yazar.
 * 4. Ebeveyn Süreç (else): Kendi 'x' pointer'ının gösterdiği
 * KENDİ heap alanını ('0') okur ve ekrana basar.
 * 5. Son 'printf' satırı, durumu netleştirir: Ebeveyn '*x = 0'
 * basarken, Çocuk '*x = 10' basar.
 *
 * Bu, bellek izolasyonunun hem 'stack' hem de 'heap' için
 * geçerli olduğunu kanıtlar.
 *
 * ====================================================================
 * 👨‍🏫 Hocanızın Sorusu: Prosesler Arası Haberleşme (IPC)
 * ====================================================================
 *
 * Bu kodda, Ebeveyn (ana proses) '*x = 0' basar, çünkü çocuğun '*x = 10'
 * olarak yaptığı değişiklik, çocuğun kendi özel bellek alanında kalır.
 *
 * Hocanızın sorduğu gibi, Ebeveyn'in bu '*x = 10' değerini
 * öğrenebilmesi (yani '*x'i Çocuğun güncellediği haliyle alabilmesi)
 * için Prosesler Arası Haberleşme (IPC) mekanizmalarından birinin
 * kullanılması zorunludur.
 *
 * Örneğin:
 * - Çocuk, bu '*x = 10' değerini Ebeveyn'e bir Pipe (Boru Hattı)
 * kullanarak yazabilir.
 * - Çocuk, değeri bir Message Queue (Mesaj Kuyruğu) aracılığıyla
 * gönderebilir.
 * - Veya her iki süreç de fork()'tan önce oluşturulmuş bir
 * Shared Memory (Paylaşımlı Bellek) alanını kullanabilir.
 * (NOT: Normal 'malloc' paylaşımlı DEĞİLDİR, 'shm_open' gibi
 * özel fonksiyonlar gerekir).
 *
 * Yani evet, '*x' değeri prosesler arası haberleşmeyle aktarılabilir,
 * ancak bu kodda böyle bir mekanizma kullanılmamıştır (burada bu olmaz).
 * ====================================================================
 */