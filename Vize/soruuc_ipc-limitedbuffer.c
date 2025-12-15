//
//Created by Melih Kotman
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h> // wait için gerekli
#include <time.h>     // random sayı için

// Mesaj yapısı (Zorunlu Format: long mtype ile başlamalı)
struct mesaj_kutusu {
    long mtype;      // Mesaj tipi (Her zaman > 0 olmalı)
    int sayi;        // Göndereceğimiz veri
};

int main() {
    pid_t pid;
    key_t key;
    int msgid;
    struct mesaj_kutusu gonderilen_mesaj, alinan_mesaj;

    // 1. Anahtar Üretimi (ftok)
    // "." mevcut klasörü temsil eder, 65 ise proje ID'sidir.
    key = ftok(".", 65);
    //if (key == -1) {
    //    perror("Anahtar hatasi");
    //    exit(1);
    //}

    // 2. Mesaj Kuyruğu Oluşturma (msgget)
    msgid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL); //-rw-rw-rw-
    //if (msgid == -1) {
    //    perror("Kuyruk hatasi");
    //    exit(1);
    //}

    // 3. Süreç Çatallama (fork)
    pid = fork();

    if (pid != 0) {
        // --- ÜRETİCİ PROSES (ANA / PARENT) ---

        // Rastgele sayı üretimi için seed
        srand(time(NULL));
        int uretilen_sayi = rand() % 101; // 0-100 arası

        // Mesajı hazırla
        gonderilen_mesaj.mtype = 1; // Tip 1 olsun
        gonderilen_mesaj.sayi = uretilen_sayi;

        // Mesajı Gönder (msgsnd)
        // Parametreler: id, veri adresi, veri boyutu, bayrak(0)
        msgsnd(msgid, &gonderilen_mesaj, sizeof(int), 0);
        //if (msgsnd(msgid, &gonderilen_mesaj, sizeof(int), 0) == -1) {
        //    perror("Gonderme hatasi");
        //} else {
            printf("Uretici (Ana): %d sayisini kuyruga gonderdi.\n", uretilen_sayi);
        //}

        // Çocuğun işini bitirmesini bekle
        wait(NULL);

        // Kuyruğu sil (Temizlik)
        msgctl(msgid, IPC_RMID, NULL);

    } else {
        // --- TÜKETİCİ PROSES (ÇOCUK / CHILD) ---

        // Mesajı Oku (msgrcv)
        // Parametreler: id, veri adresi, veri boyutu, istenen tip(1), bayrak(0)
        // Not: Sondaki '0' bayrağı, mesaj yoksa BEKLEMESİNİ (block) sağlar.
        msgrcv(msgid, &alinan_mesaj, sizeof(int), 1, 0);
        //if (msgrcv(msgid, &alinan_mesaj, sizeof(int), 1, 0) == -1) {
        //    perror("Alma hatasi");
        //} else {
            printf("Tuketici (Cocuk): Kuyruktan %d sayisini okudu.\n", alinan_mesaj.sayi);
        //}

        exit(0); // Çocuk sonlanır
    }

    return 0;
}