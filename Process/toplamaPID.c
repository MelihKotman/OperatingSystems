//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // fork, getpid, sleep için.
#include <sys/wait.h>   // 'waitpid' fonksiyonu için.

// İki tamsayı pointer'ının gösterdiği değerleri toplayan standart fonksiyon.
int toplama(int *x,int *y) {
    return *x + *y;
}

int main(int argc,char **argv) {
    // p_pid ve a_pid değişkenleri HİÇ KULLANILMIYOR.
    pid_t p_pid, ch1_pid,a_pid;

    /*
     * ==========================================================
     * FORK (ÇATALLANMA): Süreç burada kopyalanır.
     * ==========================================================
     * Ebeveyn (P1) ve Çocuk (C1) süreçleri oluşturulur.
     * 'ch1_pid' değeri:
     * - P1 (Ebeveyn) için: Çocuğun PID'i (örn: 13001)
     * - C1 (Çocuk 1) için: 0
     */
    ch1_pid = fork();

    /*
     * !!! ANA KONSEPT BURADA !!!
     * 'int a,b;' satırı 'fork()'tan SONRA yer alıyor.
     * Bu, hem P1'in hem de C1'in bu satırı AYRI AYRI çalıştıracağı anlamına gelir.
     *
     * P1 (Ebeveyn): KENDİ yığın (stack) belleğinde 'a' ve 'b' oluşturur.
     * C1 (Çocuk):   KENDİ yığın (stack) belleğinde 'a' ve 'b' oluşturur.
     *
     * Bu değişkenler miras alınmadı, bağımsız olarak yaratıldılar.
     */
    int a,b;

    // P1, kendi 'a'sına 5 atar.
    // C1, kendi 'a'sına 5 atar.
    a = 5;

    // P1, kendi 'b'sine 3 atar.
    // C1, kendi 'b'sine 3 atar.
    b = 3;

    /*
     * Bu 'printf' satırı 2 KEZ çalışır (hem P1 hem C1).
     * İşletim sisteminin sanal bellek yönetimi nedeniyle,
     * P1'in 'a'sının sanal adresi ile C1'in 'a'sının sanal adresi
     * büyük ihtimalle BİRBİRİNİN AYNISI olacaktır.
     * Ancak bu sanal adresler, FARKLI fiziksel bellek konumlarına
     * işaret eder.
     */
    printf("a:%x\n",&a);

    /*
     * SÜREÇ AYRIŞTIRMA
     */
    if (ch1_pid == 0) {

        // --- SADECE ÇOCUK SÜREÇ (C1) ÇALIŞIR ---

        // C1, kendi 'a'sının adresini tekrar basar.
        printf("Adres (a): %x\n",&a);

        // C1, kendi 'a' (5) ve 'b' (3) değerlerini toplar.
        // ÇIKTI: "8"
        printf("Toplama = %d\n",toplama(&a,&b));
    }

    /*
     * Bu 'waitpid' satırı 2 KEZ çalışır (hem P1 hem C1).
     *
     * 1. P1 (Ebeveyn) çalıştırır:
     * 'ch1_pid' (örn: 13001) geçerlidir. P1, C1 süreci
     * bitene kadar burada BLOKLANIR (bekler).
     *
     * 2. C1 (Çocuk) çalıştırır:
     * 'ch1_pid' (0)'dır. 'waitpid(0, ...)' "kendi grubundaki
     * herhangi bir çocuğu bekle" demektir. C1'in çocuğu
     * olmadığı için bu komut anında '-1' (hata) döndürür
     * ve C1 yoluna devam eder.
     */
    waitpid(ch1_pid,0,0);

    /*
     * Bu 'printf' satırı 2 KEZ çalışır (hem P1 hem C1).
     *
     * Çıktı sırası 'waitpid' sayesinde GARANTİLİDİR:
     * 1. C1, 'waitpid'den hemen geçer ve kendi PID'ini basar. (örn: 13001)
     * 2. C1 sonlanır.
     * 3. P1, 'waitpid' beklemesinden (bloklanmasından) C1 bittiği
     * için kurtulur ve kendi PID'ini basar. (örn: 13000)
     */
    printf("Surec = %d\n",getpid());
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, değişkenlerin 'fork()' çağrısından *sonra*
 * tanımlanmasının etkisini gösterir.
 *
 * 1. 'fork()' çağrısı ile Ebeveyn (P1) ve Çocuk (C1) süreçleri
 * oluşturulur.
 * 2. 'int a,b;' satırı, 'fork()'tan sonra olduğu için, hem P1
 * hem de C1 tarafından *bağımsız olarak* çalıştırılır.
 * 3. P1, kendi 'a' ve 'b' değişkenlerini oluşturur; C1 de KENDİ
 * 'a' ve 'b' değişkenlerini oluşturur. Bu değişkenler arasında
 * "miras" veya "paylaşım" yoktur.
 * 4. Her iki süreç de bu değişkenlere 5 ve 3 değerlerini atar.
 * 5. C1 (if bloğu), bu değerleri toplayarak ekrana '8' yazar.
 * 6. P1 (Ebeveyn), 'waitpid' komutu sayesinde C1'in bitmesini
 * bekler.
 * 7. C1, kendi PID'ini basar ve sonlanır.
 * 8. P1, beklemeden kurtulur ve kendi PID'ini basar.
 *
 * Ana Konsept: Değişkenler 'fork()'tan önce tanımlanırsa
 * "kopyalanır" (miras alınır). 'fork()'tan sonra tanımlanırlarsa,
 * her süreç için "bağımsız" ve "yerel" olurlar.
 * ====================================================================
 */