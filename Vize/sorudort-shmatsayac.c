//
//Created by Melih Kotman
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>

int main() {
    // 1. Anahtar ve Bellek Oluşturma
    // ftok ile anahtar üretilir (dosya "main.c" veya "." olabilir)
    key_t key = ftok(".", 65);

    // shmget: Bellek alanı oluştur (int boyutu kadar)
    int shmid = shmget(key, sizeof(int), 0666 | IPC_CREAT);

    // 2. Belleği Bağlama (Attach)
    int *sayac = (int*) shmat(shmid, NULL, 0);

    // Başlangıç değeri
    *sayac = 100;
    printf("Baslangic Degeri: %d\n", *sayac);

    if (fork() == 0) {
        // --- ÇOCUK SÜREÇ (Sayacı Azaltan) ---
        while (*sayac > 0) {
            (*sayac)--; // Ortak bellekteki değeri azalt
            printf("Cocuk: Sayac = %d\n", *sayac);
            // sleep(1); // Hoca isterse eklenebilir, zorunlu değil
        }
        shmdt(sayac); // Çocuk bellekten ayrılır
        exit(0);
    } else {
        // --- ANA SÜREÇ (Bekleyen) ---
        wait(NULL); // Çocuğun işini bitirmesini bekle

        printf("Ana Surec: Islem bitti. Son Deger: %d\n", *sayac);

        // Temizlik (Detach ve Remove)
        shmdt(sayac);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}