//
//Created by Melih Kotman
//

#include <stdio.h> //Temel C fonksiyonları
#include <stdlib.h> // Temel C fonksiyonları
#include <pthread.h> //İplikler için kütüphanemiz
#include <time.h> //Zaman fonksiyonumuz
#include <unistd.h>

#define TOPLAM_MESAFE 100 //Örnek mesafe 100 dedik
#define HIZ 11 //Örnek hiz aralığını 11 dedik


void *yarisma(void *arg) {
    int id = (int) arg; //Gelen id değerini int'e çevirdik
    sleep(2);
    printf("Kosucu %d Basladi\n",id);
    //UZUN ÇÖZÜM Hızları ve mesafeleri bizim verdiğimiz senaryo
    /*int toplam_mesafe = 0; //İpliklerin gittikleri toplam mesafeyi aldık
    int hiz = 0; //İlk başta ikisi de hız olarak 0 dedik.
    printf("Kosucu %d basladi\n",id); //Yarış başlattık.
    while (toplam_mesafe < TOPLAM_MESAFE) {
        sleep(1); //Adil yarış olsun diye 1 saniye bekleye bekleye yarıştırdık.
        hiz = rand() % HIZ; //Üretilen rastgele sayının 0-10 aralığında olması için 11'e modunu aldık.
        if (toplam_mesafe + HIZ > TOPLAM_MESAFE) { //Toplam mesafe ve hız toplamı TOPLAM_MESAFE'yi geçiyorsa
            hiz = rand() % (TOPLAM_MESAFE - toplam_mesafe + 1); //Tekrardan kalan yola göre rastgele sayı al.
        }
        toplam_mesafe = toplam_mesafe + hiz; //Sonrasında hızı toplam mesafeye ekle
        printf("Kosucu %d, %d kadar yol gitti. Toplam mesafe(%d) : %d\n",id,hiz,id,toplam_mesafe); //Her adımı açıkla
    }
    */
    printf("Kosucu %d Yarisi Bitirdi...\n",id); //Koşucu bitirince bitirdiğini söyle
    return NULL; //Hiçbir şey gönderme.
}
int main() {
    pthread_t th[2]; //İki tane iplik oluşturduk.
    srand(time(NULL)); //Çekirdeğin çalışma süresine göre random sayı ürettik.
    pthread_create(&th[0],NULL,yarisma,(void*)1); //İlk ipliğimiz çalıştı
    pthread_create(&th[1],NULL,yarisma,(void*)2); //İkinci ipliğimiz çalıştı

    //Önce biten iplik birbirini beklesin dedik.
    pthread_join(th[0],NULL);
    pthread_join(th[1],NULL);


    printf("YARIS BITTI - KOSUCULAR PARKURU TAMAMLADI\n");
    return 0;
}
