//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // fork, getpid, sleep için.
#include <sys/wait.h>   // 'wait' fonksiyonları için. (Bu kodda kullanılmıyor).
#include <pthread.h>    // 'pthread' (thread) kütüphanesi. (Bu kodda kullanılmıyor).

int main(int argc, char **argv) {
    int x = 0; // Ebeveyn (Ana) prosesin belleğinde x = 0 olarak ayarlandı.
    pid_t main_pid, ch1_pid, ch2_pid; // ch2_pid HİÇ KULLANILMIYOR.

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---
    // (P1 - Ebeveyn, PID=6000 varsayalım)

    main_pid = getpid(); // Ebeveyn kendi PID'ini alır (main_pid = 6000).
    printf("PID: %d\n",main_pid); // ÇIKTI: "PID: 6000"

    /*
     * ==========================================================
     * FORK (ÇATALLANMA): Süreç kopyalanır.
     * ==========================================================
     * P1 (Ebeveyn, 6000) yeni bir süreç (C1, 6001) oluşturur.
     *
     * ÖNEMLİ: C1 (Çocuk), P1'in o anki bellek dökümünün (x=0)
     * bir KOPYASINA sahip olur.
     *
     * 'ch1_pid' değişkeninin değeri:
     * - P1 (Ebeveyn) için: 6001 (Çocuğun PID'i)
     * - C1 (Çocuk 1) için: 0
     */
    ch1_pid = fork();

    /*
     * ==========================================================
     * SÜREÇ AYRIŞTIRMA
     * ==========================================================
     */
    if (ch1_pid == 0) {

        // --- BU BLOK SADECE ÇOCUK SÜREÇ (C1 - 6001) TARAFINDAN ÇALIŞTIRILIR ---

        // C1, KENDİ 'x' kopyasının değerini 10 yapar.
        // Bu, EBEVEYNİN 'x'ini ASLA ETKİLEMEZ.
        x = 10;

        // C1 kendi kimliğini basar.
        // ÇIKTI: "Bu cocuk proses: 6001"
        printf("Bu cocuk proses: %d\n",getpid());
    }
    else {

        // --- BU BLOK SADECE EBEVEYN SÜREÇ (P1 - 6000) TARAFINDAN ÇALIŞTIRILIR ---

        // P1, KENDİ 'x' kopyasının değerini basar.
        // C1'in 'x'i 10 yapması P1'i etkilemediği için 'x' hala 0'dır.
        // ÇIKTI: "x = 0"
        printf("x = %d\n",x);

        // P1 kendi kimliğini basar.
        // ÇIKTI: "Bu ana proses: 6000"
        printf("Bu ana proses: %d\n",getpid());
    }

    // --- BU BÖLÜMDEN İTİBAREN KOD 2 KEZ ÇALIŞIR (P1 VE C1) ---

    /*
     * Bu son 'printf', iki sürecin de 'x' değişkeninin son durumunu
     * kendi bakış açılarından gösterir.
     *
     * 1. EBEVEYN (P1) ÇIKTISI: "ID = 6000  x = 0"
     * (Çünkü P1'in 'x'i hiç değişmedi)
     *
     * 2. ÇOCUK (C1) ÇIKTISI: "ID = 6001   x = 10"
     * (Çünkü C1 kendi 'x'ini 10 yapmıştı)
     */
    printf("ID = %d\t x = %d\n",getpid(),x);

    // Hem P1 hem de C1 10 saniye bekler.
    sleep(10);
    return 0;
}
/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, fork() sistem çağrısının yarattığı Bellek İzolasyonu
 * (Memory Isolation) kavramını göstermek için tasarlanmış temel
 * bir örnektir.
 *
 * 1. Ebeveyn (Ana) süreç, 'x' adında bir değişkeni '0' olarak başlatır.
 * 2. fork() çağrılır ve bir Çocuk süreç oluşturulur.
 * 3. Çocuk süreç, ebeveynin belleğinin (ve 'x = 0' değerinin) bir
 * kopyasını alır.
 * 4. Çocuk Süreç (if (ch1_pid == 0)): Kendi 'x' kopyasının değerini
 * '10' yapar.
 * 5. Ebeveyn Süreç (else): Kendi 'x' kopyasının değerini ('0')
 * ekrana basar.
 * 6. Son 'printf' satırı, durumu netleştirir: Ebeveyn 'x = 0' basarken,
 * Çocuk 'x = 10' basar.
 *
 * Bu, bir süreçte (Çocuk) yapılan bellek değişikliğinin, diğer süreci
 * (Ebeveyn) etkilemediğini kanıtlar.
 *
 * ====================================================================
 * 👨‍🏫 Hocanızın Sorusu: Prosesler Arası Haberleşme (IPC)
 * ====================================================================
 *
 * Bu kodda, Ebeveyn (ana proses) 'x = 0' basar, çünkü çocuğun 'x = 10'
 * olarak yaptığı değişiklik, çocuğun kendi özel bellek alanında kalır.
 * Buna Bellek İzolasyonu denir.
 *
 * Hocanızın sorduğu gibi, Ebeveyn'in bu 'x = 10' değerini
 * öğrenebilmesi (yani 'x'i Çocuğun güncellediği haliyle alabilmesi)
 * için Prosesler Arası Haberleşme (IPC - Inter-Process Communication)
 * mekanizmalarından birinin kullanılması zorunludur.
 *
 * Örneğin:
 * - Çocuk, bu 'x = 10' değerini Ebeveyn'e bir Pipe (Boru Hattı)
 * kullanarak yazabilir.
 * - Çocuk, değeri bir Message Queue (Mesaj Kuyruğu) aracılığıyla
 * gönderebilir.
 * - Veya her iki süreç de fork()'tan önce oluşturulmuş bir
 * Shared Memory (Paylaşımlı Bellek) alanını kullanabilir.
 *
 * Yani evet, 'x' değeri prosesler arası haberleşmeyle aktarılabilir,
 * ancak bu kodda böyle bir mekanizma kullanılmamıştır (burada bu olmaz).
 * Bu kod, tam da bu haberleşmeye neden ihtiyaç duyulduğunu göstermek
 * için yazılmıştır.
 * ====================================================================
 */
/*
 *  int main(int argc, char **argv) {
 *  int *x = malloc(4);
 *  *x = 0;
 *  pid_t main_pid, ch1_pid, ch2_pid;
 *  main_pid = getpid();
 *  printf("PID: %d\n",main_pid);
 *  ch1_pid = fork();
 *  if (ch1_pid == 0) {
 *  *x = 10;
 *  printf("Bu cocuk proses: %d\n",getpid());
 *  }
 *  else {
 *  printf("x = %d\n",*x);
 *  printf("Bu ana proses: %d\n",getpid());
 *  }
 *  printf("ID = %d\t x = %d\n",getpid(),*x);
 *  sleep(10);
 *  return 0;
 *  }
*/