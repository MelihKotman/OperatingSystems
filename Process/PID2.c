//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>      // 'printf' gibi standart G/Ç fonksiyonları için.
#include <sys/types.h>  // 'pid_t' gibi özel işletim sistemi veri türleri için.
#include <fcntl.h>      // 'open' gibi düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // 'getpid', 'getppid', 'fork' ve 'sleep' fonksiyonları için.

/*
 * 'main' fonksiyonu, programın giriş noktasıdır.
 */
int main(int argc,char **argv) {

    /*
     * 3 adet PID değişkeni tanımlıyoruz:
     * pid: Bu programın (ebeveyn) kendi PID'i.
     * p_pid: Bu programı başlatanın (ebeveynin ebeveyni) PPID'i.
     * ch1_pid: 'fork()' ile oluşturulacak 'çocuk' sürecin PID'ini tutmak için.
     */
    pid_t pid,p_pid, ch1_pid;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---

    // Ebeveyn, kendi PID'ini alır.
    pid = getpid();
    // Ebeveyn, kendi ebeveyninin (terminalin) PPID'ini alır.
    p_pid = getppid();

    // Ebeveyn, kendi bilgilerini ekrana basar.
    printf("PID = %d\n",pid);
    printf("pPID = %d\n",p_pid);

    /*
     * ==========================================================
     * !!! KRİTİK NOKTA: FORK (ÇATALLANMA) !!!
     * ==========================================================
     * 'fork()' çağrıldığı anda, işletim sistemi bu sürecin
     * BİREBİR KOPYASINI (Çocuk Süreç) oluşturur.
     *
     * 'fork()' fonksiyonu İKİ DEĞER döndürür:
     * 1. EBEVEYN SÜRECE: Yeni oluşturduğu çocuğun PID'ini (örn: 5001) döndürür.
     * 2. YENİ ÇOCUK SÜRECE: '0' (sıfır) değerini döndürür.
     *
     * Bu satırdan sonra, bu kodun aynısını çalıştıran İKİ süreç olur.
     */
    ch1_pid = fork();

    // --- BU BÖLÜM 2 KEZ (HEM EBEVEYN HEM ÇOCUK) ÇALIŞIR ---

    /*
     * Bu 'printf' satırı artık 2 kez çalışacak:
     * 1. Ebeveyn'de: 'ch1_pid' çocuğun PID'i olduğu için (örn: 5001),
     * ekrana "c1_pid = 5001" yazar.
     * 2. Çocuk'ta: 'ch1_pid' 0 olduğu için,
     * ekrana "c1_pid = 0" yazar.
     */
    printf("c1_pid = %d\n",ch1_pid);

    /*
     * Süreçleri ayırt etme: 'ch1_pid' değişkeninin değerine bakarak
     * şu an bu kodu Ebeveyn'in mi yoksa Çocuk'un mu çalıştırdığını anlarız.
     */
    if (ch1_pid == 0) {
        // --- BU BLOK SADECE ÇOCUK SÜREÇ TARAFINDAN ÇALIŞTIRILIR ---

        /*
         * NOT: Bu 'printf' satırında BİR HATA var.
         * Format string'i ("%d\n") sadece BİR sayı bekliyor, ama siz
         * 'getpid()' ve 'getppid()' olmak üzere İKİ fonksiyon vermişsiniz.
         * 'getppid()' fonksiyonu yok sayılacaktır.
         *
         * Sadece çocuğun kendi PID'ini basar.
         * Doğrusu: printf("Cocuk: Kendi PID'im %d, Ebeveynim %d\n", getpid(), getppid());
         * olmalıydı.
         */
        printf("Cocuk: %d\n",getpid(),getppid());
    }

    /*
     * Bu 'sleep' komutu da 2 kez (hem ebeveyn hem çocuk) çalışır.
     * Her iki süreç de 20 saniye bekler.
     * Bu, 'ps -ef' komutuyla süreç ağacını görmemiz için bize zaman tanır.
     */
    sleep(20);

    /*
     * Her iki süreç de (ebeveyn ve çocuk) kendi çalışmalarını
     * başarıyla bitirir ve işletim sistemine '0' döndürür.
     */
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, UNIX/Linux sistemlerindeki temel süreç oluşturma
 * mekanizması olan 'fork()' sistem çağrısını gösterir.
 *
 * Yaptığı iş şudur:
 * 1. Başlangıçta tek bir "Ebeveyn" (Parent) süreç olarak çalışır.
 * 2. Kendi PID ve PPID (ebeveyninin PID'i) bilgilerini ekrana basar.
 * 3. 'fork()' fonksiyonunu çağırır. Bu noktada işletim sistemi,
 * ebeveyn sürecin birebir aynısı olan yeni bir "Çocuk" (Child)
 * süreç oluşturur.
 * 4. 'fork()' çağrısı ebeveyne çocuğun PID'ini, çocuğa ise '0'
 * değerini döndürür.
 * 5. Bu noktadan sonraki tüm kodlar (örn: `printf` ve `sleep(20)`)
 * artık hem ebeveyn hem de çocuk tarafından AYRI AYRI çalıştırılır.
 * 6. Kod, `if (ch1_pid == 0)` kontrolünü kullanarak hangi sürecin
 * çocuk olduğunu anlar ve sadece çocuğun çalıştıracağı
 * özel bir 'printf' komutu içerir.
 * 7. Her iki süreç de 20 saniye bekler (gözlem yapabilmek için)
 * ve ardından sonlanır.
 *
 * ====================================================================
 */