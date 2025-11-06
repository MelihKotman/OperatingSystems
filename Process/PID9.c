//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>      // Düşük seviyeli dosya işlemleri için. (Bu kodda kullanılmıyor).
#include <unistd.h>     // fork, getpid, sleep için.
#include <sys/wait.h>   // 'waitpid' fonksiyonu (şu an yorum satırında) için gereklidir.

/*
 * İki adet tamsayı 'pointer'ı (adresi) alan basit bir toplama fonksiyonu.
 * Pointer'ların işaret ettiği 'değerleri' (*x ve *y) toplar.
 */
int toplama(int *x,int *y) {
    return *x + *y;
}

int main(int argc,char **argv) {

    // p_pid ve a_pid değişkenleri tanımlanmış ama HİÇ KULLANILMIYOR.
    pid_t p_pid, ch1_pid,a_pid;
    int a,b,c;

    // --- BU BÖLÜM SADECE 1 KEZ (EBEVEYN TARAFINDAN) ÇALIŞIR ---
    // (P1 - Ebeveyn, PID=11000 varsayalım)

    // Ebeveyn'in (P1) bellek alanında a, b, c değişkenleri oluşturulur.
    a = 5;
    b = 3;
    c = 0;

    /*
     * ==========================================================
     * FORK (ÇATALLANMA): Süreç kopyalanır.
     * ==========================================================
     * P1 (Ebeveyn, 11000) yeni bir süreç (C1, 11001) oluşturur.
     *
     * ÖNEMLİ: C1 (Çocuk), P1'in o anki bellek dökümünün (a=5, b=3, c=0)
     * bir KOPYASINA sahip olur. C1'in 'a'sı, P1'in 'a'sından artık
     * bağımsızdır (ancak 'Copy-on-Write' optimizasyonu kullanılır).
     *
     * 'ch1_pid' değişkeninin değeri:
     * - P1 (Ebeveyn) için: 11001 (Çocuğun PID'i)
     * - C1 (Çocuk 1) için: 0
     */
    ch1_pid = fork();

    /*
     * Bu satır 2 kez çalışır (P1 ve C1).
     *
     * İki süreç de 'a' değişkeninin 'sanal bellek' adresini basar.
     * Modern işletim sistemlerindeki sanal bellek yönetimi nedeniyle,
     * bu iki sürecin de AYNI ADRESİ (örn: 0x7ffee...) basması
     * çok yüksek ihtimaldir. Ancak bu adresler,
     * FARKLI FİZİKSEL HAFIZA konumlarına işaret eder.
     */
    printf("Ata: adres (a) = %x\n",&a);

    /*
     * SÜREÇ AYRIŞTIRMA
     */
    if (ch1_pid == 0) {

        // --- BU BLOK SADECE ÇOCUK SÜREÇ (C1) TARAFINDAN ÇALIŞTIRILIR ---

        // C1, KENDİ 'a' (5) ve 'b' (3) değişkenlerinin adreslerini 'toplama'ya yollar.
        // Dönen sonuç (8), C1'in KENDİ 'c' değişkenine yazılır.
        // EBEVEYNİN (P1) 'c' DEĞİŞKENİ HALA 0 OLARAK KALIR.
        c = toplama(&a, &b);

        // C1, KENDİ 'c'sinin adresini basar.
        printf("Çocuk: adres(c) = %x\n",&c);

        // C1, KENDİ 'c'sinin (artık 8 olan) DEĞERİNİ basar.
        printf("%d\n",c); // Çıktı: 8

        // C1, 10 saniye bekler.
        sleep(10);

    } // --- Çocuk bloğunun sonu ---

    /*
     * BU 'printf' KOMUTU HEM EBEVEYN (P1) HEM DE ÇOCUK (C1) TARAFINDAN ÇALIŞTIRILIR.
     */
    printf("%d\n",c);

    // 1. EBEVEYN (P1) Çalıştırması:
    // P1, 'if' bloğuna hiç girmedi. P1'in 'c' değişkeninin değeri
    // en baştan beri 0 idi ve hiç değişmedi.
    // Çıktı: 0

    // 2. ÇOCUK (C1) Çalıştırması:
    // C1, 'if' bloğundan çıkar (10 sn sonra) ve bu satıra gelir.
    // C1'in 'c' değişkeninin değeri 8 olarak değişmişti.
    // Çıktı: 8 (ikinci kez)


    // 'waitpid', Ebeveyn sürecin, 'ch1_pid' ID'li çocuk sürecin
    // bitmesini beklemesini sağlar.
    // Eğer bu satırın yorumu kaldırılsaydı, P1 "0" çıktısını basmadan
    // önce C1'in 10 saniye uyuyup bitirmesini beklerdi.
    // waitpid(ch1_pid,0,0);


    // Bu satır da 2 kez çalışır (P1 ve C1).
    // Her süreç kendi 'getpid()' sonucunu basar.
    printf("Surec = %d\n",getpid()); // P1 için 11000, C1 için 11001 basar
}

/*
 * ====================================================================
 * KODUN ÖZETİ
 * ====================================================================
 *
 * Bu program, 'fork()' sonrası süreçlerin belleklerinin
 * birbirinden bağımsız olduğunu (Bellek İzolasyonu) kanıtlar.
 *
 * 1. Ebeveyn (P1), a=5, b=3, c=0 değişkenlerini tanımlar.
 * 2. 'fork()' çağrılır. Çocuk (C1) bu değişkenlerin bir KOPYASINI alır.
 * 3. C1 (if bloğu içi):
 * - Kendi 'a' ve 'b'sini toplayıp (5+3=8) KENDİ 'c'sine yazar.
 * - 'c'nin değeri 8'i ekrana basar.
 * 4. P1 (if bloğu dışı):
 * - 'if' bloğunu atlar. Kendi 'c' değişkeni HİÇ DEĞİŞMEMİŞTİR.
 * - 'c'nin değeri 0'ı ekrana basar.
 * 5. C1 (if bloğu dışı):
 * - 10 saniye uyandıktan sonra 'if' bloğundan çıkar.
 * - Kendi 'c'sinin değeri 8'i İKİNCİ KEZ ekrana basar.
 *
 * ANA KONSEPT (Copy-on-Write):
 * 'fork()' olduğunda, C1 ve P1 aynı fiziksel belleği paylaşıyormuş
 * gibi başlar. Ancak C1, 'c' değişkenine YAZMA (c = toplama(...))
 * girişiminde bulunduğu an, işletim sistemi o bellek sayfasının
 * C1'e özel bir kopyasını oluşturur. C1, bu özel kopyayı
 * değiştirir. P1'in orijinal bellek sayfası bundan etkilenmez.
 * ====================================================================
 */