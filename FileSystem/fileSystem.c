//
// Created by Melih Yiğit Kotman on 24.12.2025.
//

#include <errno.h>      // Hata kodlarını (errno) kullanmak için gerekli.
#include <fcntl.h>      // Dosya kontrol seçenekleri (O_RDONLY, O_CREAT vb.) için gerekli.
#include <stdio.h>      // Standart giriş/çıkış fonksiyonları (printf, fprintf) için.
#include <stdlib.h>     // Standart kütüphane fonksiyonları (exit, EXIT_SUCCESS) için.
#include <string.h>     // String işlemleri ve hata mesajları (strerror) için.
#include <unistd.h>     // POSIX işletim sistemi API'si (read, write, close, getpid) için.

#define BUFFER_SIZE 1024 // Okuma işlemi için kullanılacak tampon bellek (buffer) boyutu.

int main(int argc, char *argv[]) {
    int fd;                     // Dosya Tanımlayıcısı (File Descriptor). Dosyanın kimliğidir.
    ssize_t bytes_read;         // read() fonksiyonundan dönen "okunan bayt sayısı".
    char buffer[BUFFER_SIZE];   // Okunan verilerin geçici olarak tutulacağı karakter dizisi.
    const char *filename;       // Açılacak dosyanın ismini tutan pointer.


    // Komut satırı argümanlarını kontrol et.
    if (argc < 2) {
        // Eğer kullanıcı bir dosya adı girmediyse varsayılanı kullan.
        filename = "test.txt";
        printf("Dosya adı sağlanmadı. Varsayılan kullanılıyor: %s\n", filename);
    }
    else {
        // Kullanıcının girdiği dosya adını al (örn: ./program dosya.txt).
        filename = argv[1];
    }

    // ======================================
    // OPEN SİSTEM ÇAĞRISI (SYSTEM CALL)
    // ======================================
    // open() - Bir dosyayı açar ve bir dosya tanımlayıcısı (file descriptor) döndürür.
    // Sözdizimi: int open (const char *pathname, int flags, mode_t mode);
    //
    // Yaygın Bayraklar (Flags):
    // O_RDONLY - Sadece okumak için aç (Read Only)
    // O_WRONLY - Sadece yazmak için aç (Write Only)
    // O_RDWR   - Hem okuma hem yazma için aç (Read Write)
    // O_CREAT  - Dosya yoksa oluştur (Create)
    // O_TRUNC  - Dosya varsa içeriğini silip sıfırla (Truncate)
    // O_APPEND - Dosyanın sonuna ekleme yap (Append)
    // ======================================

    printf("\n=== Dosya açılıyor: %s ===\n", filename);

    // NOT: Kodda art arda iki 'open' çağrısı var. İkincisi geçerli olacaktır.
    // İlk 'open': Dosyayı sadece okuma modunda açmaya çalışır.
    // fd = open(filename, O_RDONLY);

    // İkinci 'open' (Aktif olan):
    // Dosyayı okuma modunda (O_RDONLY) açar.
    // Eğer dosya yoksa oluşturur (O_CREAT).
    // İzinler (Permissions): 755 (Sahibi: Oku/Yaz/Çalıştır, Grup: Oku/Çalıştır, Diğer: Oku/Çalıştır)
    // S_IRWXU: Kullanıcı (User) için Read, Write, Execute
    // S_IRGRP | S_IXGRP: Grup için Read, Execute
    // S_IROTH | S_IXOTH: Diğerleri (Others) için Read, Execute
    fd = open(filename, O_RDONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);

    // Hata Kontrolü: open() başarısız olursa -1 döndürür.
    if (fd == -1) {
        // Hata mesajını standart hata akışına (stderr) yazdır.
        // strerror(errno): Hata kodunu okunabilir metne çevirir.
        fprintf(stderr, "Dosya açılırken hata oluştu '%s' : %s\n", filename, strerror(errno));
        return EXIT_FAILURE; // Programı hata koduyla sonlandır.
    }

    printf("Dosya başarıyla açıldı. Dosya tanımlayıcısı (FD): %d\n", fd);

    // ======================================
    // READ SİSTEM ÇAĞRISI (SYSTEM CALL)
    // ======================================
    // read() - Dosya tanımlayıcısından veriyi okuyup tampon belleğe (buffer) yazar.
    // Sözdizimi: ssize_t read(int fd, void *buf, size_t count);
    //
    // Dönüş Değerleri:
    // > 0 : Okunan bayt sayısı
    // 0   : Dosya sonu (EOF - End of File)
    // -1  : Hata oluştu
    // ======================================

    printf("\n=== Dosya içeriği okunuyor: ===\n\n");

    // Döngü: Dosya bitene kadar (read 0 döndürene kadar) oku.
    // read(fd, buffer, BUFFER_SIZE - 1): En fazla 1023 bayt oku (son karakter \0 için ayrıldı).
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        // Okunan veriyi string olarak basabilmek için sonuna Null Character (\0) ekle.
        buffer[bytes_read] = '\0';
        // Tampon bellekteki veriyi ekrana bas.
        printf("%s", buffer);
    }

    // Döngü bittiğinde read() -1 döndürmüş mü kontrol et (Hata durumu).
    if (bytes_read == -1) {
        fprintf(stderr, "\nDosya okunurken hata: %s\n", strerror(errno));
        close(fd); // Hata olsa bile dosyayı kapatmayı unutma.
        return EXIT_FAILURE;
    }

    printf("\n\n=== Dosya Sonuna (EOF) Ulaşıldı ===\n");

    // ======================================
    // CLOSE SİSTEM ÇAĞRISI (SYSTEM CALL)
    // ======================================
    // close() - Bir dosya tanımlayıcısını kapatır ve kaynağı serbest bırakır.
    // Sözdizimi: int close(int fd);
    //
    // Dönüş Değerleri:
    // 0   : Başarılı
    // -1  : Hata oluştu
    // ======================================

    // Dosyayı kapat ve hata kontrolü yap.
    if (close(fd) == -1) {
        fprintf(stderr, "Dosya kapatılırken hata: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    printf("Dosya başarıyla kapatıldı.\n");

    return EXIT_SUCCESS; // Program başarıyla sonlandı.
}

/*
 * ======================================================================================
 * KOD ÖZETİ VE MANTIĞI: DÜŞÜK SEVİYE DOSYA İŞLEMLERİ (SYSTEM CALLS)
 * ======================================================================================
 *
 * BU PROGRAMIN AMACI:
 * C dilinde standart kütüphane fonksiyonları (fopen, fscanf vb.) yerine,
 * doğrudan işletim sistemi çekirdeğine (kernel) hitap eden POSIX sistem çağrılarının
 * (open, read, close) nasıl kullanılacağını göstermektir.
 *
 * ÇALIŞMA MANTIĞI:
 *
 * 1. DOSYA TANIMLAYICISI (File Descriptor - FD):
 * - Program bir dosya ile işlem yapmak istediğinde işletim sistemi ona bir tam sayı verir.
 * - Bu sayıya 'FD' denir (kodda 'int fd'). Program dosya ile her iletişim kurduğunda
 * bu kimlik numarasını kullanır.
 *
 * 2. AÇMA (open):
 * - Program belirtilen dosyayı 'open()' fonksiyonu ile açar.
 * - 'O_RDONLY | O_CREAT' bayrakları kullanılarak; dosya varsa okunur, yoksa
 * yeni bir dosya oluşturulur.
 *
 * 3. TAMPON BELLEK İLE OKUMA (Buffered Reading):
 * - Dosya tek seferde belleğe yüklenmez (dosya çok büyük olabilir).
 * - Bunun yerine 'read()' fonksiyonu ile parça parça (chunk) okunur.
 * - Kodda 'BUFFER_SIZE' (1024 byte) kadar veri okunup ekrana basılır, sonra
 * kaldığı yerden okumaya devam eder. Bu döngü dosya bitene (EOF) kadar sürer.
 *
 * 4. KAPATMA (close):
 * - İşletim sistemlerinde açık dosya sayısı sınırlıdır.
 * - İşlem bitince 'close(fd)' ile dosya serbest bırakılır, kaynaklar iade edilir.
 *
 * ÖZETLE:
 * Bu kod, Linux/Unix sistem programlamanın temel taşı olan "Her şey bir dosyadır"
 * mantığını ve bu dosyaların sistem çağrıları ile nasıl yönetildiğini gösterir.
 * ======================================================================================
 */