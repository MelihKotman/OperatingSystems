//
// Created by Melih Yiğit Kotman on 15.12.2025.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * ==========================================================
 * 1. GLOBAL DEĞİŞKEN (Data Segment)
 * ==========================================================
 * 'int a[100];' global bir dizidir.
 * Bu dizi, belleğin "Data Segment" (veya BSS) bölümünde saklanır.
 * Program çalıştığı sürece bellekte kalır.
 */
int a[100];

int main(int argc, char **argv) {
    /*
     * ==========================================================
     * 2. YEREL DEĞİŞKENLER (Stack Segment)
     * ==========================================================
     * 'y', 'p' ve 'x' main fonksiyonunun içinde tanımlandığı için
     * belleğin "Stack" (Yığın) bölümünde saklanır.
     */
    int y = 10;
    int *p;

    /*
     * YORUM SATIRINDAKİ KODLAR:
     * - system("pmap..."): Linux'ta çalışan işlemin bellek haritasını (memory map)
     * görmek için kullanılır. Hoca muhtemelen derste terminalden bunu göstermiştir.
     * - malloc: Dinamik bellek tahsisi (Heap Segment). Bu kodda kapalı.
     */
    //system("pmap -x %s >> memory.txt",str(getpid()));
    //sleep(10);
    //p = (int) malloc(sizeof(int)*atoi(argv[i]));

    /*
     * ==========================================================
     * 3. DİZİ TANIMLAMA VE BELLEK YERLEŞİMİ
     * ==========================================================
     * 5 elemanlı bir tamsayı dizisi tanımlanıyor.
     * x[0]=2, x[1]=5, x[2]=3, x[3]=1, x[4]=7
     */
    int x[5] = {2, 5, 3, 1, 7};

    /*
     * ==========================================================
     * 4. POINTER ARİTMETİĞİ VE ADRES MANTIĞI (ÖNEMLİ!)
     * ==========================================================
     * printf formatları:
     * %d: Tamsayı (Decimal)
     * %x: Onaltılık Sayı (Hexadecimal) - Adresleri göstermek için kullanılır.
     */

    // --- SATIR 1: Temel Bilgiler ---
    // sizeof(x): Dizinin toplam boyutu. 5 eleman * 4 byte (int) = 20 byte.
    // x        : Dizinin başlangıç adresi (Pointer olarak).
    // &x[0]    : Dizinin ilk elemanının adresi. (x ile aynıdır!)
    // x[0]     : Dizinin ilk elemanının değeri (2).
    printf("%d\n %x\n %x\n %d\n", sizeof(x), x, &x[0], x[0]);

    // --- SATIR 2: Pointer Aritmetiği (x + 1) ---
    // sizeof(x): Yine 20.
    // x + 1    : DİKKAT! Bu, sayısal toplama değildir.
    //            "Bir sonraki elemanın adresi" demektir.
    //            int pointer olduğu için adres 4 byte (veya 64 bit sistemde 8) artar.
    // &x[0]    : İlk elemanın adresi (Referans noktası).
    // x[0]     : Değer (2).
    printf("%d\n %x\n %x\n %d\n", sizeof(x), x + 1, &x[0], x[0]);

    // --- SATIR 3: Dizinin Adresi (&x) ---
    // &x       : Dizinin tamamının adresi. Değer olarak 'x' ile aynıdır
    //            ama türü 'int (*)[5]'tir.
    printf("%d\n %x\n %x\n %d\n", sizeof(x), &x, &x[0], x[0]);

    // --- SATIR 4: İkinci Elemanın Adresi (&x[1]) ---
    // x        : Başlangıç adresi.
    // &x[1]    : İkinci elemanın adresi. (x + 1 ile aynı adresi verir).
    printf("%d\n %x\n %x\n %d\n", sizeof(x), x, &x[1], x[0]);

    /*
     * ==========================================================
     * 5. HATALI VE GEÇERLİ ATAMALAR (L-Value Hatası)
     * ==========================================================
     */

    // (x+1) = 10;  // HATA! (x+1) bir adrestir (R-value), bir değişken (L-value) değildir.
                    // "Adresin kendisine" 10 atayamazsınız.

    // x[1] = 10;   // GEÇERLİ. Dizinin 2. elemanının DEĞERİNİ 10 yapar.

    // *(x+1) = 10; // GEÇERLİ. (x+1) adresinin GÖSTERDİĞİ YERE (yani x[1]'e) 10 yazar.
    //              Bu, x[1] = 10 ile tamamen aynıdır.

    printf("%d\n", x[1]); // x[1]'in son değerini basar.
    return 0;
}