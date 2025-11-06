//
// Created by Melih Yiğit Kotman
//
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>  // (Bu kütüphane bu kodda kullanılmıyor)
#include <sys/shm.h>  // Paylaşımlı Bellek (shmget, shmat) için ana kütüphane
#define SHM_SIZE 1024 // "Yazıcı" ile AYNI boyutta olmalı!

int main(int argc,char **argv) {
    key_t key;
    // ... (pid, mode, i değişkenleri HİÇ KULLANILMIYOR) ...
    int shmid;
    char *veri; // Paylaşımlı bellekten okunacak veriyi tutacak pointer

    /*
     * ==========================================================
     * 1. ANAHTARI BULMA (ftok)
     * ==========================================================
     * "Yazıcı" (Writer) programı ile BİREBİR AYNI parametreleri
     * ('.' dizini ve 'R' karakteri) kullanarak, "Yazıcı"nın
     * oluşturduğu 'key'in AYNISINI bulur.
     * İki prosesin buluşma noktası bu anahtardır.
     */
    if ((key = ftok(".",'R'))== -1) {
        perror("ftok");
        exit(1);
    }

    /*
     * ==========================================================
     * 2. BELLEK SEGMENTİNİ ALMA (shmget)
     * ==========================================================
     * Bu 'key'i kullanarak, "Yazıcı"nın daha önce oluşturduğu
     * paylaşımlı bellek segmentinin 'shmid'sini işletim
     * sisteminden alır.
     *
     * (Not: 'IPC_CREAT' bayrağı, "eğer segment yoksa oluştur"
     * demektir. Bu sayede "Okuyucu" programı "Yazıcı"dan önce
     * çalışsa bile segmenti oluşturabilir. "Yazıcı" da aynı
     * bayrağı kullandığı için, ilk çalışan oluşturur, diğeri
     * var olanı kullanır. Bu, 'IPC_EXCL' kullanmamaktan
     * daha güvenli bir yöntemdir.)
     */
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    // Kendi PID'ini ve bulduğu 'shmid'yi basar.
    printf("PID = %d\n --- shmid = %d\n\n",getpid(),shmid);

    /*
     * ==========================================================
     * 3. BELLEĞE BAĞLANMA (shmat - Attach)
     * ==========================================================
     * 'shmat', 'shmid' ile tanımlanan ve işletim sisteminin
     * yönettiği o paylaşımlı bellek alanını, bu prosesin kendi
     * bellek alanına "bağlar".
     * 'veri' pointer'ı, artık o paylaşımlı alanın başlangıç
     * adresini tutar.
     */
    veri = shmat(shmid,(void*)0,0);

    /*
     * ==========================================================
     * 4. VERİYİ OKUMA VE YAZDIRMA
     * ==========================================================
     * Bu 'printf', 'veri' pointer'ının gösterdiği yerdeki
     * (yani paylaşımlı belleğin İÇİNDEKİ) metni okur
     * ve ekrana basar.
     *
     * BU SATIRIN BAŞARISI, "Yazıcı"nın 'strcpy' kullanarak
     * o belleğe düzgün bir metin kopyalamış olmasına bağlıdır.
     *
     * Eğer "Yazıcı", 'strcpy' kullandıysa, bu satır:
     * "Segment icerigi:Deneme mesaji...." basar.
     *
     * Eğer "Yazıcı", hatalı 'data = "..."' kodunu kullandıysa,
     * bu satır "çöp" (garbage) veri veya boşluk basar.
     */
    printf("Segment icerigi:%s\n",veri);

    // Yorum satırı: "Okuyucu"nun normalde "Yazıcı"nın işine
    // karışmaması ve veriyi değiştirmemesi gerekir.
    //char *p = veri;
    //strcpy(p,"Deneme");

    // (İyi bir alışkanlık olarak 'shmdt(veri)' ile bellekten
    // ayrılınabilirdi.)
    return 0;
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ ("Okuyucu" Programı)
 * ====================================================================
 *
 * Bu, bir "Paylaşımlı Bellek (Shared Memory) Okuyucusu"
 * (Reader) programıdır.
 *
 * 1. 🔑 Anahtar Bulma ('ftok'):
 * "Yazıcı" programı ile aynı 'key'i bularak doğru bellek
 * segmentini hedefler.
 *
 * 2. 🧠 Segmenti Bulma ('shmget'):
 * 'key'i kullanarak 'shmid'yi (paylaşımlı belleğin kimliği)
 * işletim sisteminden alır.
 *
 * 3. 🔗 Bağlanma ('shmat'):
 * 'shmat' ile o bellek segmentine bağlanır ve 'veri'
 * pointer'ı aracılığıyla erişim kazanır.
 *
 * 4. 📖 Okuma ('printf'):
 * 'veri' pointer'ı aracılığıyla paylaşımlı belleğin İÇERİĞİNİ
 * okur ve ekrana basar.
 *
 * BU KODUN DOĞRU ÇALIŞMASI (yani "Deneme mesaji...."
 * basması), BİR ÖNCEKİ "YAZICI" PROGRAMININ 'strcpy()'
 * kullanarak o belleğe veriyi BAŞARIYLA KOPYALAMIŞ
 * olmasına bağlıdır.
 * ====================================================================
 */