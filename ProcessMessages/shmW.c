//
// Created by Melih Yiğit Kotman
//
// ... Kütüphaneler (string.h burada 'strcpy' için çok önemli) ...
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/shm.h> // Paylaşımlı Bellek (shmget, shmat) için ana kütüphane
#define SHM_SIZE 1024

int main(int argc,char **argv) {
    key_t key;
    // ... (pid, mode, i kullanılmıyor) ...
    int shmid;
    char *data; // Paylaşımlı belleğin adresini tutacak pointer

    /*
     * ==========================================================
     * 1. ANAHTAR ve SEGMENT OLUŞTURMA (ftok, shmget)
     * ==========================================================
     * '.' (bu dizin) ve 'R' karakteriyle bir anahtar (key) üretir.
     * Bu anahtarla 1024 byte'lık bir paylaşımlı bellek segmenti
     * oluşturur (veya varsa ona erişir).
     */
    if ((key = ftok(".",'R'))== -1) {
        perror("ftok"); exit(1);
    }
    if ((shmid = shmget(key, SHM_SIZE, 0644 | IPC_CREAT)) == -1) {
        perror("shmget"); exit(1);
    }

    printf("PID = %d\n --- shmid = %d\n\n",getpid(),shmid);

    /*
     * ==========================================================
     * 2. BELLEĞE BAĞLANMA (shmat - Attach)
     * ==========================================================
     * 'shmat', işletim sisteminden 'shmid' ID'li bellek segmentini
     * bu prosesin bellek alanına bağlamasını ister.
     * 'data' pointer'ı, artık o paylaşımlı alanın başlangıç adresini
     * tutar. 'data'ya yazılan her şey, paylaşımlı belleğe yazılır.
     */
    data = shmat(shmid,(void*)0,0);

    /*
     * ==========================================================
     * 3. VERİ YAZMA (Önceki Hatayı Düzeltme)
     * ==========================================================
     */

    // YORUMUN: "//data = "Deneme mesaji...."; //Bundan olmuyor ben yapınca olmadı"
    // ANALİZ: KESİNLİKLE DOĞRU. Bu satır (önceki kodda), 'data' pointer'ının
    // kendisini değiştirir, paylaşımlı belleğin *içine* veri YAZMAZ.
    // Bu yüzden "Okuyucu" programı hiçbir şey bulamazdı.

    // DOĞRU YÖNTEM (strcpy - String Copy):
    // "Deneme mesaji....", metnini (byte'larını) al ve 'data'
    // pointer'ının GÖSTERDİĞİ YERE (yani paylaşımlı belleğin İÇİNE)
    // KOPYALA.
    strcpy(data, "Deneme mesaji....");

    // Şimdi bu printf, 'data'nın gösterdiği yeri, yani paylaşımlı
    // belleğin *içeriğini* okur ve ekrana basar.
    printf("data:%s\n",data);

    /*
     * ==========================================================
     * 4. HOCANIZIN YORUMU (argv[1])
     * ==========================================================
     * YORUMUN: "...C kodu diye girmedi buraya... bilmelisiniz dedi"
     * ANALİZ: Bu yorum satırındaki kod, metni sabit ("Deneme...")
     * olarak değil, programı çalıştırırken komut satırından
     * (argv[1]) almayı sağlar.
     * Örn: ./bu_program "Merhaba Dünya"
     * 'strcpy(data, argv[1])' komutu, "Merhaba Dünya" metnini
     * paylaşımlı belleğe yazardı.
     *
     * Hocanızın "C kodu diye girmedi" demesi, onun IPC (shmget, shmat)
     * mekanizmasına odaklandığını, 'strcpy' veya 'argv' gibi temel
     * C konularına odaklanmadığını gösterir.
     * Ama "bilmelisiniz" demesi, IPC'yi kullanmak için bu temel C
     * bilgisine (pointer'a veri kopyalamaya) mecbur olduğunuzu
     * vurgulamak içindir.
     */
    /*char *p = data;
    strcpy(p,argv[1]);
    printf("%s\n",data);
    */

    // Not: 'shmdt(data);' (detach) ile bellekten ayrılmak iyi
    // bir alışkanlıktır, ancak program sonlandığında
    // işletim sistemi bunu zaten yapar.
    return 0;
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Düzeltilmiş Hali)
 * ====================================================================
 *
 * Bu, paylaşımlı bellek (Shared Memory) kullanan DOĞRU "Yazıcı"
 * (Writer) programıdır.
 *
 * 1. 🔑 Anahtar ve Segment ('ftok', 'shmget'):
 * Sistemde bir paylaşımlı bellek segmenti oluşturur.
 *
 * 2. 🔗 Bağlanma ('shmat'):
 * 'data' pointer'ı aracılığıyla o segmente bağlanır.
 *
 * 3. ✍️ Yazma ('strcpy'):
 * Kritik fark buradadır. 'data = "..."' (HATALI pointer ataması)
 * yerine, 'strcpy(data, "...")' (DOĞRU bellek kopyalaması)
 * kullanır.
 *
 * Bu sayede "Deneme mesaji...." metni, gerçekten paylaşımlı bellek
 * segmentinin İÇİNE yazılır.
 *
 * 4. 📤 Sonuç:
 * Bu program çalıştıktan sonra, "Okuyucu" (Reader) programı
 * çalıştırıldığında, aynı segmente bağlanacak ve 'printf'
 * komutuyla 'data'nın (artık dolu olan) içeriğini okuyarak
 * ekrana "Segment icerigi:Deneme mesaji...." basacaktır.
 * ====================================================================
 */