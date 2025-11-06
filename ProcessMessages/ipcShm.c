//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/shm.h> // Paylaşımlı Bellek için kütüphane

int main(int argc, char **argv) {
    int x; // Süreçlerin kendi yerel (local) 'x' değişkenleri
    int shmid; // Paylaşımlı bellek segmentinin ID'si
    key_t kK1;
    x = 0; // Ebeveyn'in yerel 'x'i = 0
    pid_t Ana_pid, ch1_pid;

    // Paylaşımlı belleğin sanal adresini tutacak pointer
    int *shm_ptr;

    Ana_pid = getpid();
    printf("PID:%d\n", Ana_pid);

    // 1. ANAHTARI OLUŞTUR (ftok)
    // Tıpkı Mesaj Kuyruğu gibi, ortak bir 'key' gerekir.
    kK1 = ftok("./", 15);
    if (kK1 == -1) {
        perror("ftok failed"); return -1;
    }

    /*
     * ==========================================================
     * 2. BELLEK SEGMENTİ OLUŞTUR (shmget) - (fork'tan önce)
     * ==========================================================
     * 'fork()'tan önce Ebeveyn, ortak kullanılacak bellek
     * alanını (sadece 1 integer'lık yer) oluşturur.
     */
    shmid = shmget(kK1, sizeof(int), 0666 | IPC_CREAT | IPC_EXCL);
    if (shmid == -1) {
        perror("shmget failed"); return -1;
    }

    printf("Paylaşımlı bellek (shmid=%d) oluşturuldu.\n", shmid);

    // Süreç burada ikiye ayrılır
    ch1_pid = fork();

    if (ch1_pid == 0) {
        // --- ÇOCUK SÜREÇ (Yazıcı - Writer) ---

        x = 10; // Çocuk, KENDİ yerel 'x'ini 10 yapar.
        printf("Bu cocuk proses: %d (x=%d değerini yazıyor)\n", getpid(), x);

        /*
         * ==========================================================
         * 3. BELLEĞE BAĞLAN (shmat - Attach)
         * ==========================================================
         * Çocuk, 'shmid'yi kullanarak o ortak bellek alanını
         * kendi adres alanına "bağlar". 'shm_ptr' artık
         * o ortak "beyaz tahtayı" göstermektedir.
         */
        shm_ptr = (int *)shmat(shmid, NULL, 0);
        if (shm_ptr == (int *)(-1)) {
            perror("shmat child failed"); exit(1);
        }

        /*
         * ==========================================================
         * 4. BELLEĞE YAZ (Write)
         * ==========================================================
         * Çocuk, 'x'in (10) değerini 'shm_ptr' aracılığıyla
         * doğrudan PAYLAŞIMLI BELLEĞE yazar.
         */
        *shm_ptr = x;

        // 5. BELLEKTEN AYRIL (shmdt - Detach)
        // Çocuğun paylaşımlı bellekle işi bitti.
        shmdt(shm_ptr);

        exit(0); // Çocuğun işi bitti, sonlanır.

    } else {
        // --- EBEVEYN SÜREÇ (Okuyucu - Reader) ---

        /*
         * ==========================================================
         * 6. SENKRONİZASYON (waitpid) - (Çok Önemli)
         * ==========================================================
         * Mesaj Kuyruğunda 'msgrcv' bizi OTOMATİK olarak bekletmişti.
         * Paylaşımlı bellekte bu yoktur. Ebeveyn'in, Çocuğun
         * belleğe yazmayı BİTİRDİĞİNDEN emin olması gerekir.
         * 'waitpid', Çocuğun 'exit(0)' komutunu çalıştırana
         * kadar Ebeveyn'i burada BLOKLAR (bekletir).
         */
        waitpid(ch1_pid, NULL, 0);

        // 7. BELLEĞE BAĞLAN (shmat)
        // Ebeveyn de aynı 'shmid'yi kullanarak o ortak alana bağlanır.
        shm_ptr = (int *)shmat(shmid, NULL, 0);
        if (shm_ptr == (int *)(-1)) {
            perror("shmat parent failed"); return -1;
        }

        /*
         * ==========================================================
         * 8. BELLEKTEN OKU (Read)
         * ==========================================================
         * Ebeveyn, 'shm_ptr' aracılığıyla paylaşımlı bellekten
         * (Çocuğun oraya yazdığı) değeri okur ve kendi 'x'ine atar.
         */
        x = *shm_ptr;

        printf("x = %d (Paylaşımlı Bellekten okundu)\n", x);
        printf("Bu ana proses: %d\n", getpid());

        // 9. BELLEKTEN AYRIL (shmdt)
        shmdt(shm_ptr);

        /*
         * ==========================================================
         * 10. TEMİZLİK (shmctl - IPC_RMID)
         * ==========================================================
         * İş bitti. Ebeveyn, oluşturduğu paylaşımlı bellek
         * segmentini işletim sisteminden kalıcı olarak siler.
         */
        shmctl(shmid, IPC_RMID, NULL);
    }

    // Sadece Ebeveyn bu satıra ulaşır (Çocuk 'exit(0)' ile çıktı).
    // Ebeveyn 'x=10' basar, bu da verinin başarıyla
    // aktarıldığını gösterir.
    printf("id:%d\tx=%d\n", getpid(), x);
    return (0);
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Paylaşımlı Bellek ile IPC)
 * ====================================================================
 *
 * Bu kod, bir önceki "Mesaj Kuyruğu" (MQ) örneğiyle aynı
 * "Bellek İzolasyonu" sorununu (Ebeveyn-Çocuk arası
 * veri aktarımı) **Paylaşımlı Bellek (Shared Memory - SHM)**
 * ile çözer.
 *
 * MQ (Posta Kutusu)'nun aksine, SHM (Ortak Beyaz Tahta) çok
 * daha hızlıdır çünkü veri *kopyalanmaz*, *paylaşılır*.
 *
 * İŞLEYİŞ:
 * 1. 📋 **Kurulum (Ebeveyn):** Ebeveyn, `fork()` yapmadan önce
 * `shmget` ile "ortak beyaz tahtayı" (paylaşımlı bellek
 * segmenti) oluşturur.
 *
 * 2. 🍴 **Bölünme (`fork()`):** Ebeveyn ve Çocuk olarak ikiye
 * ayrılırlar. İkisi de aynı 'shmid'yi (tahtanın ID'si) bilir.
 *
 * 3. ✍️ **Yazma (Çocuk):** Çocuk, 'shmat' ile "tahtaya
 * bağlanır", `*shm_ptr = 10` komutuyla "tahtaya 10 yazar",
 * 'shmdt' ile "tahtadan ayrılır" ve `exit(0)` ile işini bitirir.
 *
 * 4. ⏳ **Senkronizasyon (Ebeveyn):** Ebeveyn, 'waitpid'
 * kullanarak Çocuğun "tahtaya yazmayı bitirmesini" BEKLER.
 * (Mesaj Kuyruğundaki 'msgrcv'nin otomatik beklemesinin aksine,
 * burada beklemeyi ('waitpid' ile) bizim *açıkça* yapmamız gerekir.)
 *
 * 5. 📖 **Okuma (Ebeveyn):** Çocuk bitince Ebeveyn uyanır,
 * 'shmat' ile "tahtaya bağlanır", `x = *shm_ptr` ile
 * "tahtadaki 10'u okur" ve 'shmdt' ile "tahtadan ayrılır".
 *
 * 6. 🧹 **Temizlik (Ebeveyn):** Ebeveyn, işi bittiği için
 * `shmctl(IPC_RMID)` ile "beyaz tahtayı" sistemden siler.
 *
 * SONUÇ:
 * Ebeveyn 'x=10' basar. Bu, SHM'nin MQ'ya göre daha
 * doğrudan (ve daha hızlı), ancak senkronizasyon için daha
 * fazla (örn: 'waitpid') kod gerektiren bir IPC yöntemi
 * olduğunu gösterir.
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