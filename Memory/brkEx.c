//
// Created by Melih Yiğit Kotman on 15.12.2025.
//
/*
 * ====================================================================
 * 📄 SİSTEM SAYFA BOYUTU (PAGE SIZE) GÖSTERİCİ
 * ====================================================================
 *
 * Amaç:
 * İşletim sisteminin bellek yönetiminde kullandığı temel birim olan
 * "Sayfa Boyutu"nu (Page Size) 'sysconf' sistem çağrısı ile öğrenip
 * ekrana yazdırmak.
 *
 * Kullanılan Fonksiyon:
 * - sysconf(_SC_PAGESIZE): POSIX standardına uygun olarak sistem
 * konfigürasyon bilgisini (sayfa boyutunu) döndürür.
 *
 * ====================================================================
 */

#include <stdio.h>      // printf için
#include <unistd.h>     // sysconf ve _SC_PAGESIZE için

int main() {
    // 1. Sistemden Sayfa Boyutunu İste
    // sysconf fonksiyonu, parametre olarak verilen konfigürasyonun
    // değerini 'long' (uzun tamsayı) olarak döndürür.
    long sayfa_boyutu = sysconf(_SC_PAGESIZE);

    // Hata kontrolü (Opsiyonel ama iyi bir pratiktir)
    if (sayfa_boyutu == -1) {
        perror("sysconf hatasi");
        return 1;
    }

    // 2. Sonucu Ekrana Yazdır
    printf("page_size = %ld byte\n", sayfa_boyutu);

    return 0;
}