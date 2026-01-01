//
// Created by Melih Yiğit Kotman on 30.12.2025.
//
/*
 * ====================================================================
 * 🚀 EXEC AİLESİ: exec, execlp, execvp, execl ÖRNEKLERİ
 * ====================================================================
 *
 * Amaç:
 * Bu kod, 'exec' fonksiyon ailesinin 4 farklı üyesinin kullanımını ve
 * parametre farklılıklarını gösterir. Her bir fonksiyon, ayrı bir çocuk
 * süreç içinde 'ls -l' komutunu çalıştırmak için kullanılır.
 *
 * Fonksiyonlar ve Farkları:
 * 1. execl  (l: list)       -> Tam yol ister, argümanlar liste olarak verilir.
 * 2. execlp (p: path)       -> PATH'de arar, argümanlar liste olarak verilir.
 * 3. execv  (v: vector)     -> Tam yol ister, argümanlar dizi (array) olarak verilir.
 * 4. execvp (p: path)       -> PATH'de arar, argümanlar dizi (array) olarak verilir.
 *
 * Not: 'exec' başarılı olursa geriye dönmez, süreç yeni programa dönüşür.
 * ====================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>     // fork, exec ailesi, getpid
#include <sys/wait.h>   // wait

int main() {
    pid_t pid;

    printf("--- EXEC AİLESİ DEMOSU BAŞLIYOR ---\n\n");

    // =================================================================
    // 1. execl (List, Full Path)
    // - Programın TAM YOLUNU (/bin/ls) vermelisiniz.
    // - Argümanları tek tek (liste halinde) vermelisiniz.
    // =================================================================
    pid = fork();
    if (pid == 0) {
        printf("[Çocuk 1]: 'execl' ile /bin/ls çalıştırılıyor...\n");

        // Parametreler: (Tam Yol, Arg0, Arg1, ..., NULL)
        execl("/bin/ls", "ls", "-l", NULL);

        // Buraya gelirse hata var demektir
        perror("execl hatası");
        exit(1);
    }
    wait(NULL); // 1. Çocuğu bekle
    printf("---------------------------------------------------\n");


    // =================================================================
    // 2. execlp (List, Path Environment)
    // - Programın sadece ADINI (ls) verebilirsiniz (PATH'de arar).
    // - Argümanları tek tek (liste halinde) vermelisiniz.
    // =================================================================
    pid = fork();
    if (pid == 0) {
        printf("[Çocuk 2]: 'execlp' ile ls çalıştırılıyor...\n");

        // Parametreler: (Dosya Adı, Arg0, Arg1, ..., NULL)
        execlp("ls", "ls", "-l", NULL);

        perror("execlp hatası");
        exit(1);
    }
    wait(NULL); // 2. Çocuğu bekle
    printf("---------------------------------------------------\n");


    // =================================================================
    // 3. execv (Vector, Full Path)
    // - Programın TAM YOLUNU (/bin/ls) vermelisiniz.
    // - Argümanları bir DİZİ (vector) içinde vermelisiniz.
    // =================================================================
    pid = fork();
    if (pid == 0) {
        printf("[Çocuk 3]: 'execv' ile /bin/ls çalıştırılıyor...\n");

        // Argüman dizisi hazırlanır (Son eleman NULL olmalı)
        char *args[] = {"ls", "-l", NULL};

        // Parametreler: (Tam Yol, Argüman Dizisi)
        execv("/bin/ls", args);

        perror("execv hatası");
        exit(1);
    }
    wait(NULL); // 3. Çocuğu bekle
    printf("---------------------------------------------------\n");


    // =================================================================
    // 4. execvp (Vector, Path Environment)
    // - Programın sadece ADINI (ls) verebilirsiniz.
    // - Argümanları bir DİZİ (vector) içinde vermelisiniz.
    // =================================================================
    pid = fork();
    if (pid == 0) {
        printf("[Çocuk 4]: 'execvp' ile ls çalıştırılıyor...\n");

        // Argüman dizisi
        char *args[] = {"ls", "-l", NULL};

        // Parametreler: (Dosya Adı, Argüman Dizisi)
        execvp("ls", args);

        perror("execvp hatası");
        exit(1);
    }
    wait(NULL); // 4. Çocuğu bekle

    printf("\n--- TÜM ÇOCUKLAR İŞİNİ BİTİRDİ, ANA PROGRAM KAPANIYOR ---\n");
    return 0;
}