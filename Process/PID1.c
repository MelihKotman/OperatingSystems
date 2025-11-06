//
// Created by Melih Yiğit Kotman
//
/*
 * Gerekli kütüphaneleri "import" ediyoruz.
 * Bunlar, C'ye hazır fonksiyonları nasıl kullanacağını öğretir.
 */
#include <stdio.h>      // 'printf' gibi standart giriş/ç çıkış fonksiyonları burada tanımlıdır.
#include <sys/types.h>  // 'pid_t' gibi özel işletim sistemi veri türleri burada tanımlıdır.
#include <fcntl.h>      // Dosya kilitleme (flock) veya 'open' gibi düşük seviyeli dosya işlemleri için kullanılır.
#include <unistd.h>     // 'getpid', 'getppid' ve 'sleep' gibi POSIX işletim sistemi
                        // fonksiyonları için en önemli kütüphane budur.

/*
 * Her C programı 'main' fonksiyonu ile başlar.
 * İşletim sistemi programı çalıştırdığında bu fonksiyonu çağırır.
 */
int main(int argc, char **argv) {

    /*
     * İki adet değişken tanımlıyoruz.
     * 'pid_t' türü, özellikle Proses ID (PID) numaralarını tutmak için
     * tasarlanmış bir veri türüdür (genellikle bir integer'dır).
     */
    pid_t pid, p_pid;

    /*
     * 1. KENDİ KİMLİĞİMİZİ (PID) ÖĞRENME
     * 'getpid()' fonksiyonu işletim sistemine "Benim PID numaram kaç?" diye sorar.
     * Gelen cevabı (sayıyı) 'pid' isimli değişkenimize atarız.
     */
    pid = getpid();

    /*
     * 2. EBEVEYNİMİZİN KİMLİĞİNİ (PPID) ÖĞRENME
     * 'getppid()' fonksiyonu işletim sistemine "Beni başlatan programın
     * PID numarası kaç?" diye sorar.
     * Gelen cevabı 'p_pid' isimli değişkenimize atarız.
     */
    p_pid = getppid();

    /*
     * 3. BİLGİLERİ EKRANA YAZDIRMA
     * 'printf', ekrana formatlı metin basar.
     * "%d" gördüğü yere, virgülden sonraki değişkenin sayısal değerini koyar.
     * "\n" ise "new line" demektir, bir alt satıra geçer.
     */

    // Az önce öğrendiğimiz 'pid' değişkeninin değerini ekrana yazdırıyoruz.
    printf("PID = %d\n", pid);

    // 'p_pid' değişkeninin değerini (ebeveyn PID'i) ekrana yazdırıyoruz.
    printf("pPID = %d\n", p_pid);

    /*
     * 4. GÖZLEM İÇİN BEKLEME
     * 'sleep(30)' fonksiyonu, işletim sistemine "Beni 30 saniye boyunca
     * duraklat (uyku moduna al)" der.
     * Program bu satırda 30 saniye boyunca donmuş gibi bekler.
     *
     * AMACI: Program hemen kapanmasın ki, biz başka bir terminal açıp
     * 'ps' komutuyla bu PID ve PPID değerlerini sistemde görebilelim.
     */
    sleep(30);

    /*
     * 5. PROGRAMDAN ÇIKIŞ
     * 'return 0' işletim sistemine "Ben işimi başarıyla tamamladım" demektir.
     * 'sleep(30)' bittikten sonra bu satır çalışır ve program sonlanır.
     */
    return(0);
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, bir İşletim Sistemleri dersi için temel bir "süreç" (process)
 * konsepti gösterimidir.
 *
 * Yaptığı iş şudur:
 * 1. 'getpid()' fonksiyonunu kullanarak kendi benzersiz Süreç Kimliğini (PID) öğrenir.
 * 2. 'getppid()' fonksiyonunu kullanarak kendisini başlatan Ebeveyn Sürecin
 * Kimliğini (PPID) öğrenir.
 * 3. Bu iki kimlik numarasını (PID ve PPID) ekrana yazdırır.
 * 4. 'sleep(30)' komutu ile 30 saniye boyunca kendini "uyku" moduna alır.
 *
 * Asıl amacı 4. adımdır: Bu 30 saniyelik bekleme, programı kullanan
 * öğrenciye zaman tanır. Öğrenci bu sırada yeni bir terminal açarak
 * 'ps -ef' veya 'pstree' gibi komutlar yardımıyla, programın çıktısındaki
 * PID ve PPID'nin işletim sistemi tarafından nasıl yönetildiğini
 * canlı olarak gözlemleyebilir ve sürecin "S" (Sleep) durumunda olduğunu
 * görebilir.
 *
 * ====================================================================
 */