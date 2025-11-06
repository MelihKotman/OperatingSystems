//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/msg.h>

/*
 * ==========================================================
 * 1. DOĞRU STRUCT (Mesaj Zarfı)
 * ==========================================================
 * System V Mesaj Kuyruğu, ilk elemanın ZORUNLU olarak
 * 'long mtype' (mesaj tipi) olmasını gerektirir.
 * Bu düzeltme kodun çalışması için kritiktir.
 */
struct msg_buf {
    long mtype; // Mesaj tipi (filtreleme için, "Kutu #1", "Kutu #2" gibi)
    int m;      // Asıl veri (payload) - 'x'in değerini burada taşıyacağız
} msg; // 'msg' adında global bir değişken

int main(int argc, char **argv) {
    int x;
    int msgqid, rc; // msgqid: Kuyruk ID'si, rc: Hata kodu
    key_t kK1;
    x = 0; // Ebeveyn'in kendi 'x'i = 0
    pid_t Ana_pid, ch1_pid;
    Ana_pid = getpid();
    printf("PID:%d\n", Ana_pid);

    /*
     * ==========================================================
     * 2. IPC KURULUMU (fork'tan önce)
     * ==========================================================
     * 'ftok' ile benzersiz bir anahtar (key) oluşturulur.
     */
    kK1 = ftok("./", 15);
    if (kK1 == -1) {
        perror("ftok failed"); return -1;
    }

    /*
     * 'msgget' ile bu anahtara bağlı bir mesaj kuyruğu OLUŞTURULUR.
     * Bu 'fork()'tan önce yapılır ki, hem Ebeveyn hem Çocuk
     * aynı 'msgqid'yi (kuyruk kimliğini) bilsin.
     * 'IPC_EXCL' kullanmak, "zaten varsa hata ver" demektir,
     * bu da temiz bir başlangıç sağlar.
     */
    msgqid = msgget(kK1, 0666 | IPC_CREAT | IPC_EXCL);
    if (msgqid == -1) {
        perror("msgget failed"); return -1;
    }

    // Süreç burada ikiye ayrılır
    ch1_pid = fork();

    if (ch1_pid == 0) {
        // --- ÇOCUK SÜREÇ (Gönderici - Sender) ---

        x = 10; // Çocuk, KENDİ 'x'ini 10 yapar.

        // 3. MESAJI HAZIRLA
        msg.mtype = 1; // Mesajı "Tip 1" olarak etiketle
        msg.m = x;     // Veriyi (10) mesaja koy

        printf("Bu cocuk proses: %d (x=%d değerini yolluyor)\n", getpid(), msg.m);

        // 4. MESAJI GÖNDER (msgsnd)
        // '&msg' : Hazırlanan mesajın adresi.
        // 'sizeof(msg.m)' : Gönderilen verinin boyutu (SADECE 'int m').
        // '0' : Özel bayrak yok (örn: kuyruk doluysa bekle).
        rc = msgsnd(msgqid, &msg, sizeof(msg.m), 0);

        if (rc == -1) {
            perror("msgsnd child failed"); return -1;
        }

    } else {
        // --- EBEVEYN SÜREÇ (Alıcı - Receiver) ---

        // 5. MESAJI AL (msgrcv) - BLOKLAYICI (BLOCKING)
        // 'msgrcv' komutu, kuyruğa bir mesaj gelene kadar
        // Ebeveyn süreci bu satırda BEKLETİR (donar).
        // Bu yüzden (yorum satırı yaptığın) 'waitpid'e gerek kalmaz.

        // 'msgqid' : Bu kuyruktan oku.
        // '&msg'   : Gelen mesajı bu 'msg' değişkenine yaz.
        // 'sizeof(msg.m)' : En fazla 'int' kadar veri al.
        // '1'      : SADECE 'mtype'ı 1 olan mesajları istiyorum.
        // '0'      : Özel bayrak yok.
        rc = msgrcv(msgqid, &msg, sizeof(msg.m), 1, 0);

        if (rc == -1) {
            perror("msgrcv parent failed"); return -1;
        }

        // 6. VERİYİ KULLAN
        // Gelen mesajın içindeki 'm' verisini (10) al
        // ve Ebeveyn'in KENDİ 'x'ine ata.
        x = msg.m;

        printf("x = %d (Kuyruktan alındı)\n", x);
        printf("Bu ana proses: %d\n", getpid());

        // 7. TEMİZLİK (İyi Alışkanlık)
        // İş bitti. Ebeveyn, oluşturduğu mesaj kuyruğunu
        // işletim sisteminden siler.
        msgctl(msgqid, IPC_RMID, NULL);
    }

    // Bu 'printf' hem Ebeveyn hem Çocuk tarafından çalıştırılır
    // Ebeveyn: "id: [PID_Ana] x=10" (Veriyi aldığı için)
    // Çocuk:   "id: [PID_Cocuk] x=10" (Veriyi kendi belirlediği için)
    printf("id:%d\tx=%d\n", getpid(), x);
    return (0);
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Mesaj Kuyruğu ile IPC)
 * ====================================================================
 *
 * Bu kod, `fork()` ile oluşturulan iki proses (Ebeveyn ve Çocuk)
 * arasında **Prosesler Arası Haberleşme (IPC)** sağlamak için
 * **Mesaj Kuyruğu (Message Queue)** kullanan, düzgün çalışan bir
 * programdır.
 *
 * Bu kod, `fork()`'un yarattığı **Bellek İzolasyonu** (Memory Isolation)
 * sorununu çözer. (Çocuğun 'x=10' yapması normalde Ebeveyn'i
 * etkilemezdi).
 *
 * İŞLEYİŞ:
 * 1. 📬 **Kurulum (Ebeveyn):** Ebeveyn, `fork()` yapmadan önce
 * `ftok` ile bir 'anahtar' (key) ve `msgget` ile bir
 * "posta kutusu" (mesaj kuyruğu) oluşturur.
 *
 * 2. 🍴 **Bölünme (`fork()`):** Ebeveyn ve Çocuk olarak ikiye
 * ayrılırlar. İkisi de aynı 'msgqid'yi (kuyruk ID'si) bilir.
 *
 * 3. 📤 **Gönderme (Çocuk):** Çocuk süreç `x=10` değerini alır,
 * bunu `mtype=1` olarak etiketlediği bir mesaja paketler ve
 * `msgsnd` ile kuyruğa yollar ("mektubu postalar").
 *
 * 4. 📥 **Bekleme ve Alma (Ebeveyn):** Ebeveyn süreç, `msgrcv`
 * satırına gelir. Kuyrukta "Tip 1" bir mesaj olmadığı için
 * **BLOKLANIR (bekler)**. Çocuk mesajı gönderdiği an Ebeveyn
 * uyanır, mesajı alır, içinden `10` değerini çıkarır ve
 * kendi `x` değişkenine atar.
 *
 * 5. 🧹 **Temizlik (Ebeveyn):** Ebeveyn, işi bittiği için
 * `msgctl(IPC_RMID)` ile mesaj kuyruğunu sistemden siler.
 *
 * SONUÇ:
 * Ebeveyn'in son `printf`'i 'x=10' basar; bu da verinin Çocuk'tan
 * Ebeveyn'e başarılı bir şekilde aktarıldığını kanıtlar.
 * ====================================================================
* * ====================================================================
 * 🚦 BLOKLAMA VE SENKRONİZASYON (Yorumların Açıklaması)
 * ====================================================================
 *
 * Yorumun: "Asenkron koddur yani kod beklemek zorunda kalıyor"
 * "Blocking messaging denebilir"
 *
 * Analiz: Bu gözlemler %100 doğrudur.
 * 1. **Asenkron Prosesler:** `fork()` yaptıktan sonra Ebeveyn ve
 * Çocuk "asenkron"dur, yani kimin önce çalışacağı (veya ne kadar
 * hızlı çalışacağı) belli değildir.
 *
 * 2. **Bloklayan (Blocking) IPC:** Senin de dediğin gibi, bu
 * kodun güzelliği "blocking messaging" (engelleyici mesajlaşma)
 * kullanmasıdır.
 *
 * 3. **`waitpid`'e Neden Gerek Yok?**
 * Ebeveyn süreçteki `msgrcv` komutu, "kuyrukta istediğim mesaj
 * yoksa, gelene kadar bekle" der.
 * Çocuk (Gönderici) henüz mesajı göndermemişse, Ebeveyn (Alıcı)
 * o satırda **donar ve bekler**.
 * Çocuk `msgsnd` ile mesajı gönderdiği an, Ebeveyn "uyanır",
 * mesajı alır ve yoluna devam eder.
 *
 * Bu `msgrcv` beklemesi, `waitpid`'in yaptığı işi (Çocuğun
 * bitirmesini beklemek) DOLAYLI YOLDAN ve OTOMATİK olarak
 * yapar. Buna "örtük senkronizasyon" (implicit synchronization)
 * denir.
 *
 * Yorumun: "Ama iki ayrı program yazıp... sender göndermeden
 * receiver alabilir..."
 *
 * Analiz: Evet, bu `fork()`'lu yapı yerine iki ayrı program
 * (Sender.c, Receiver.c) yazsaydın ve "Receiver"ı "Sender"dan
 * önce çalıştırsaydın, "Receiver"daki `msgrcv` komutu yine
 * BLOKLANIRDI ve "Sender" programı çalışıp bir mesaj gönderene
 * kadar beklemeye devam ederdi.
 * ====================================================================
 */
/*
 * ====================================================================
 * ÖZET: Mesaj Kuyruğu (MQ) vs. Paylaşımlı Bellek (SHM)
 * ====================================================================
 *
 * Özellik         Mesaj Kuyruğu (Message Queue)
 * --------------- ----------------------------------------------------
 * Yöntem:         Veri *kopyalanır* (Çocuk -> Kernel -> Ebeveyn).
 * Benzetme:       Posta Kutusu / Mektup Göndermek
 * Hız:            Yavaş (Çekirdeğe 2 kez kopyalama olur).
 * Senkronizasyon: Kısmen yerleşik ('msgrcv' boştaysa bekler).
 * Kullanım:       Küçük, yapılandırılmış mesajlar yollamak için.
 *
 * Özellik         Paylaşımlı Bellek (Shared Memory)
 * --------------- ----------------------------------------------------
 * Yöntem:         Veri *paylaşılır* (Çocuk ve Ebeveyn aynı yere
 * yazar/okur).
 * Benzetme:       Ortak Not Defteri / Beyaz Tahta
 * Hız:            Çok Hızlı (Doğrudan bellek erişimi).
 * Senkronizasyon: Sizin Sorumluluğunuzda (Örn: 'waitpid'
 * veya Semafor).
 * Kullanım:       Büyük veri bloklarını (örn: video, matris)
 * paylaşmak için.
 *
 * ====================================================================
 */