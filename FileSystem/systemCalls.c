//
// Created by Melih Yiğit Kotman on 29.12.2025.
//
/*
 * ====================================================================
 * 🐧 SİSTEM ÇAĞRILARI İLE DOSYA İŞLEMLERİ (SYSTEM CALLS)
 * ====================================================================
 *
 * Amaç:
 * Standart C kütüphanesi (fopen, fprintf) yerine, doğrudan işletim
 * sistemi çekirdeği (Kernel) ile iletişim kuran POSIX sistem çağrılarını
 * kullanarak dosya işlemlerini gerçekleştirmek.
 *
 * Kullanılan Sistem Çağrıları:
 * 1. open: Dosya açar veya oluşturur. (Dönüş: File Descriptor - fd)
 * 2. write: Dosya tanımlayıcısına (fd) veri yazar.
 * 3. read: Dosya tanımlayıcısından veri okur.
 * 4. close: Açık olan dosya tanımlayıcısını kapatır.
 * 5. unlink: Dosyayı dosya sisteminden siler.
 * 6. lseek: Dosya içindeki okuma/yazma konumunu (offset) değiştirir.
 *
 * Gerekli Kütüphaneler:
 * - <fcntl.h>: Dosya kontrol seçenekleri (O_CREAT, O_RDWR vb.)
 * - <unistd.h>: read, write, close, unlink gibi POSIX çağrıları.
 *
 * ====================================================================
 */

#include <stdio.h>      // printf, perror
#include <stdlib.h>     // exit
#include <string.h>     // strlen
#include <fcntl.h>      // open, O_RDWR, O_CREAT, O_TRUNC
#include <unistd.h>     // write, read, close, unlink, lseek

int main() {
    int fd; // Dosya Tanımlayıcısı (File Descriptor)
    const char *dosya_adi = "sistem_dosyasi.txt";
    const char *yazilacak_veri = "Merhaba, bu bir Kernel seviyesi yazisidir!\n";
    char okuma_tamponu[100]; // Okunan veriyi tutacak dizi
    ssize_t bytes_read, bytes_written;

    printf("--- SISTEM CAGRI DEMOSU BASLIYOR ---\n");

    /*
     * 1. ADIM: DOSYA OLUŞTURMA VE AÇMA (open)
     * ---------------------------------------
     * open(const char *pathname, int flags, mode_t mode);
     *
     * Parametreler:
     * - pathname: Dosya yolu ("sistem_dosyasi.txt").
     * - flags: Açma modu bayrakları (Bitwise OR | ile birleştirilir).
     * - O_RDONLY: Sadece okuma (Read Only).
     * - O_WRONLY: Sadece yazma (Write Only).
     * - O_RDWR  : Hem okuma hem yazma (Read Write).
     * - O_CREAT : Dosya yoksa oluştur (Create).
     * - O_TRUNC : Dosya varsa içeriğini sil, sıfırla (Truncate).
     * - O_APPEND: Yazılanları dosya sonuna ekle.
     * - mode: Dosya izinleri (sadece O_CREAT varsa gereklidir).
     * - 0644: Sahip (rw-), Grup (r--), Diğer (r--).
     *
     * Dönüş Değeri (fd):
     * - Başarılı: Negatif olmayan tamsayı (File Descriptor).
     * - Başarısız: -1 (errno set edilir).
     */
    printf(">> Dosya olusturuluyor: %s\n", dosya_adi);

    fd = open(dosya_adi, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd == -1) {
        perror("Hata: Dosya acilamadi (open)");
        exit(1);
    }
    printf(">> Dosya basariyla acildi. File Descriptor (fd): %d\n", fd);

    /*
     * 2. ADIM: DOSYAYA VERİ YAZMA (write)
     * -----------------------------------
     * ssize_t write(int fd, const void *buf, size_t count);
     *
     * Parametreler:
     * - fd: Yazılacak dosyanın tanımlayıcısı (open'dan dönen değer).
     * - buf: Yazılacak verinin bellekteki adresi (pointer).
     * - count: Yazılacak bayt sayısı.
     *
     * Dönüş Değeri:
     * - Başarılı: Yazılan bayt sayısı.
     * - Başarısız: -1.
     */
    printf(">> Veri yaziliyor...\n");
    bytes_written = write(fd, yazilacak_veri, strlen(yazilacak_veri));

    if (bytes_written == -1) {
        perror("Hata: Yazma islemi basarisiz (write)");
        close(fd); // Hata olsa bile dosyayı kapatmak iyi pratiktir.
        exit(1);
    }
    printf(">> %zd byte veri yazildi.\n", bytes_written);

    // Yazma işlemi bitti, dosyayı kapatıyoruz.
    // close(int fd): Dosya tanımlayıcısını serbest bırakır. Başarılıysa 0, değilse -1 döner.
    close(fd);


    /*
     * 3. ADIM: DOSYAYI OKUMA İÇİN TEKRAR AÇMA
     * ---------------------------------------
     * Bu sefer O_RDONLY (Sadece Okuma) bayrağı ile açıyoruz.
     * İzin (mode) parametresine gerek yok çünkü dosya zaten var.
     */
    printf(">> Dosya okuma icin tekrar aciliyor...\n");
    fd = open(dosya_adi, O_RDONLY);

    if (fd == -1) {
        perror("Hata: Dosya okuma icin acilamadi");
        exit(1);
    }

    /*
     * 4. ADIM: DOSYADAN VERİ OKUMA (read)
     * -----------------------------------
     * ssize_t read(int fd, void *buf, size_t count);
     *
     * Parametreler:
     * - fd: Okunacak dosya.
     * - buf: Okunan verinin kaydedileceği bellek alanı (buffer).
     * - count: Okunacak maksimum bayt sayısı (buffer boyutu kadar olmalı).
     *
     * Dönüş Değeri:
     * - > 0: Okunan bayt sayısı.
     * - 0  : Dosya sonu (EOF - End Of File).
     * - -1 : Hata.
     */

    // Tamponu temizle (Garbage data kalmasın)
    memset(okuma_tamponu, 0, sizeof(okuma_tamponu));

    printf(">> Dosya icerigi okunuyor...\n");
    bytes_read = read(fd, okuma_tamponu, sizeof(okuma_tamponu) - 1);

    if (bytes_read == -1) {
        perror("Hata: Okuma islemi basarisiz (read)");
        close(fd);
        exit(1);
    }

    printf("--- OKUNAN VERI ---\n");
    printf("%s", okuma_tamponu);
    printf("-------------------\n");

    // Okuma işlemi bitti, kapat.
    close(fd);

    /*
     * 5. ADIM: DOSYAYI SİLME (unlink)
     * -------------------------------
     * int unlink(const char *pathname);
     *
     * Parametreler:
     * - pathname: Silinecek dosyanın yolu.
     *
     * Dönüş Değeri:
     * - 0 : Başarılı.
     * - -1: Hata.
     *
     * Not: Dosya sistemindeki "bağlantıyı" (link) kaldırır. Dosyaya başka
     * link yoksa ve dosya açık değilse diskten silinir.
     */
    printf(">> Dosya siliniyor (unlink)...\n");
    if (unlink(dosya_adi) == -1) {
        perror("Hata: Dosya silinemedi");
    } else {
        printf(">> Dosya basariyla silindi.\n");
    }

    return 0;
}