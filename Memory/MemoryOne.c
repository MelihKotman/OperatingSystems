//
// Created by Melih Yiğit Kotman on 15.12.2025.
//
#include <stdio.h>      // Standart giriş/çıkış kütüphanesi (printf)
#include <stdlib.h>     // Standart kütüphane (malloc, atoi)
#include <unistd.h>     // POSIX işletim sistemi API'si (getpid)

/*
 * GLOBAL DEĞİŞKENLER (Data Segment - BSS)
 * Global değişkenler, programın 'Data Segment' (eğer ilk değer atanmışsa)
 * veya 'BSS Segment' (ilk değer atanmamışsa) bölümünde saklanır.
 * 'a' dizisi, program çalıştığı sürece bellekte kalır.
 */
int a[100]; // BSS Segmentinde yer alır (başlangıç değeri 0 olur).

int main(int argc, char **argv) {
    /*
     * YEREL DEĞİŞKENLER (Stack Segment)
     * Fonksiyon içinde tanımlanan değişkenler 'Stack' (Yığın) bellekte saklanır.
     */
    int y = 10;
    int *p;

    /*
     * DİNAMİK BELLEK TAHSİSİ (Heap Segment)
     * 'malloc' ile ayrılan bellek 'Heap' (Yığın) bölgesindedir.
     * Bu satır yorumda olduğu için çalışmaz, ancak mantığı şöyledir:
     * Program argümanı olarak gelen sayı kadar int boyutu ayırır.
     */
    // p = (int *) malloc(sizeof(int) * atoi(argv[i]));

    /*
     * CHAR DİZİSİ (Stack Segment)
     * Burada 'char' tipinde 5 elemanlı bir dizi tanımlanmış.
     * char bellekte 1 bayt yer kaplar.
     * x[0]=2, x[1]=5, x[2]=3, x[3]=1, x[4]=7
     * NOT: Dizideki sayılar (2, 5...) küçük olduğu için 1 bayta sığar.
     */
    char x[5] = {2, 5, 3, 1, 7};

    /*
     * BELLEK ADRES ANALİZİ VE POINTER ARİTMETİĞİ
     * Bu kısım, char pointer ile int pointer arasındaki aritmetik farkı gösterir.
     */

    // 1. SATIR: Temel Dizi Bilgileri
    // sizeof(x): Dizinin boyutu. 5 eleman * 1 bayt (char) = 5 bayt.
    // x        : Dizinin başlangıç adresi (Örn: 0x1000).
    // &x[1]    : Dizinin 2. elemanının (index 1) adresi. Char olduğu için (x + 1 bayt).
    // x[1]     : Dizinin 2. elemanının değeri (5).
    printf("%d\n %x\n %x\n %d\n", sizeof(x), x, &x[1], x[1]);

    /*
     * KRİTİK NOKTA: (x + 1) = 10; HATA!
     * Bu satır derleme hatası verir (L-value required).
     * Çünkü (x + 1) bir hesaplama sonucudur (bir adrestir), bir değişken değildir.
     * "Adresin kendisine" 10 atayamazsınız.
     * Ancak o adresin İÇİNE atayabilirsiniz (aşağıdaki satır).
     */
    // (x + 1) = 10;
    printf("%d\n", x[1]);

    /*
     * POINTER İLE DEĞER DEĞİŞTİRME
     * *(x+1) = 10;
     * x bir adrestir. (x+1), char pointer olduğu için "bir sonraki baytın adresi"dir.
     * *(x+1) demek, "o adresteki kutunun içi" demektir.
     * Bu işlem, x[1] = 10; ile tamamen aynıdır.
     * x[1]'in değeri 5'ten 10'a değişir.
     */
    *(x + 1) = 10;

    // Değişimi görmek için x[1] yazdırılıyor. (Çıktı: 10)
    printf("%d\n", x[1]);


    return 0;
}