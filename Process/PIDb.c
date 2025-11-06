//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // 'printf' için
#include <sys/types.h>  // 'pid_t' gibi temel türler için
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // 'getpid', 'getppid', 'execv' için
#include <sys/wait.h>   // 'wait' fonksiyonu için

int main() {

    int x; // Bir tamsayı değişkeni tanımlanır.
           // !!! DİKKAT: Bu değişkene bir ilk değer ATANMADI (uninitialized).
           // İçinde "çöp" (garbage) bir değer vardır.

    /*
     * ==========================================================
     * !!! KRİTİK NOKTA: EXECV (EXECUTE VECTOR) !!!
     * ==========================================================
     * 'execv', 'fork()' GİBİ YENİ BİR SÜREÇ OLUŞTURMAZ!
     *
     * 'execv' fonksiyonu, mevcut süreci (bu programı)
     * tamamen durdurur, bellek alanını temizler ve o alanın
     * üzerine './f1' isimli programı yükler.
     *
     * Kısacası: Bu programı, './f1' programına DÖNÜŞTÜRÜR.
     * PID aynı kalır, ancak kodun geri kalanı atılır.
     */
    execv("./f1", NULL);

    /*
     * ==========================================================
     * BU BÖLÜM SADECE 'execv' BAŞARISIZ OLURSA ÇALIŞIR
     * ==========================================================
     *
     * 'execv' başarılı olursa, bu 'printf' satırı ASLA ÇALIŞMAZ.
     * Çünkü bu kodun yerini './f1' programı almıştır.
     *
     * 'execv' Neden Başarısız Olur?
     * 1. './f1' adında bir dosya bulunamazsa.
     * 2. Dosya bulunsa bile 'çalıştırma' (execute) izni (+x) yoksa.
     *
     * Eğer 'execv' başarısız olursa, program normal akışına devam eder:
     */

    // 'execv'nin başarısız olduğunu varsayarsak:
    // Bu satır, mevcut sürecin PID'ini, PPID'ini ve 'x'in DEĞERİNİ basar.
    // 'x' ilklendirilmediği için, ekrana TAHMİN EDİLEMEZ,
    // rastgele bir (çöp) sayı basılacaktır. Bu 'Undefined Behavior' (Tanımsız Davranış)
    // olarak bilinir.
    printf("pid= %d\t ppid= %d\t x = %d\n",getpid(),getppid(),x);

    // 'wait(NULL)' komutu, bir *çocuk* sürecin bitmesini bekler.
    // Bu program 'fork()' ile hiç çocuk oluşturmadığı için,
    // bu satır (aktif olsaydı) anında hata vererek ('ECHILD') geçerdi.
    //wait(NULL);

    // Sadece 'execv' başarısız olursa bu satıra ulaşılır.
    return 0;
}
/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, bir UNIX/Linux sistem çağrısı olan execv()'nin
 * temel davranışını gösterir.
 *
 * Ana Konsept: Süreç Dönüşümü
 * ----------------------------
 * fork(), bir süreci klonlayarak (ebeveyn + çocuk) süreç sayısını
 * ikiye çıkarır.
 *
 * execv() (ve execl, execvp gibi diğer exec ailesi fonksiyonları),
 * bir süreci dönüştürür. Mevcut programı bellekten siler ve
 * onun yerine (aynı PID altında) yepyeni bir program yükler.
 *
 * Bu kodun iki olası senaryosu vardır:
 *
 * 1. Başarılı Senaryo (Normal Durum):
 * ----------------------------------
 * - Program çalışır, execv("./f1", NULL); satırına gelir.
 * - İşletim sistemi, ./f1 dosyasını bulur ve çalıştırma izni
 * olduğunu görür.
 * - Bu programın çalışması o anda sonlanır.
 * - ./f1 programı, bu programın PID'i ile çalışmaya başlar.
 * - printf(...) satırı ASLA ÇALIŞMAZ.
 *
 * 2. Başarısız Senaryo (Hata Durumu):
 * -----------------------------------
 * - Program çalışır, execv satırına gelir.
 * - İşletim sistemi, ./f1 dosyasını bulamaz (veya çalıştırma izni
 * yoktur).
 * - execv hata verir ve program bir sonraki satırdan (printf)
 * çalışmaya devam eder.
 * - printf satırı çalışır.
 * - TUZAK: 'x' değişkeni ilklendirilmediği için (uninitialized),
 * ekrana 'x' için anlamsız, rastgele bir "çöp" (garbage)
 * değer basılır.
 *
 * Klasik Kullanım:
 * -----------------
 * 'exec' ailesi, genellikle fork()'tan sonra çocuk süreç içinde
 * kullanılır. Ebeveyn fork() yapar, çocuk execv() ile kendini
 * farklı bir programa dönüştürür ve ebeveyn de wait() ile
 * çocuğun bitirmesini bekler.
 *
 * Bu kod, bu deseni uygulamamaktadır; sadece execv'nin tek başına
 * davranışını göstermektedir.
 * ====================================================================
 */