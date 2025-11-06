//
// Created by Melih Yiğit Kotman
//
// ... Gerekli kütüphaneler ...
// 'sys/msg.h', msgget, msgsnd gibi mesaj kuyruğu
// fonksiyonları için eklenmiştir.
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/wait.h>
//#include <sys/errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

extern int errno; // Hata numaralarını ('errno.h'den gelir) kullanmak için.
#define MSGPERM 0666 // İzinler: Herkes okuyabilir ve yazabilir.
#define  MSGTXTLEN 128 // Mesajın maksimum metin uzunluğu.

int msgqid,rc; // msgqid: Kuyruğun ID'si, rc: Hata kodları için (return code).
int done;      // (Bu kodda 'done' değişkeni HİÇ KULLANILMIYOR).

/*
 * ==========================================================
 * 1. MESAJ YAPISI (struct msg_buf)
 * ==========================================================
 * System V mesaj kuyruğu için bu yapı (struct) zorunludur.
 */
struct msg_buf{
    // 'mtype' (Mesaj Tipi) İLK ELEMAN OLMALIDIR.
    // Bu, mesajı "etiketler". Alıcılar bu tipe göre filtreleme yapabilir.
    long mtype;

    // 'mtext' (Mesaj Metni) asıl verinin (payload) tutulduğu yerdir.
    char mtext[MSGTXTLEN];
} msg; // 'msg' adında global bir 'msg_buf' değişkeni oluşturulur.

int main(int argc,char **argv) {
    key_t key; // IPC Anahtarı (Tüm proseslerin buluşma noktası)

    // 'ftok', bu kaynak kod dosyasının ('msg_queue.c') yolunu ve '42'
    // sayısını kullanarak sistemde benzersiz bir anahtar (key) üretir.
    key = ftok("msg_queue.c",42);

    /*
     * ==========================================================
     * 2. KUYRUK OLUŞTURMA (msgget)
     * ==========================================================
     * 'msgget', o anahtara bağlı mesaj kuyruğunu alır veya oluşturur.
     */
    // 'key'     : ftok ile alınan anahtar.
    // 'MSGPERM' : 0666 izinleri.
    // 'IPC_CREAT': Kuyruk yoksa, oluştur.
    // 'IPC_EXCL': (IPC_CREAT ile birlikte) Kuyruk zaten varsa, HATA VER.
    // Bu, bu programın "kurucu" olduğunu garantiler. İkinci kez
    // çalıştırırsanız "File exists" hatası alırsınız.
    msgqid = msgget(key, MSGPERM|IPC_CREAT|IPC_EXCL);

    printf("Message queue %d created\n",msgqid);

    /*
     * ==========================================================
     * 3. SİSTEM KONTROLÜ (ipcs)
     * ==========================================================
     * 'system("ipcs")' komutu, bir Linux/UNIX terminal komutunu çalıştırır.
     * 'ipcs', sistemde o an aktif olan tüm Mesaj Kuyruklarını,
     * Paylaşımlı Bellekleri ve Semaforları listeler.
     * Bu çıktıda, az önce oluşturduğunuz 'msgqid'yi görmelisiniz.
     */
    printf("System IPC's \n");
    system("ipcs"); // İşletim sistemi komutunu çalıştır.

    /*
     * ==========================================================
     * 4. MESAJI HAZIRLAMA VE GÖNDERME (msgsnd)
     * ==========================================================
     */

    // Mesajımızı "Tip 1" olarak etiketliyoruz.
    // Alıcı programın da "Tip 1" mesajı istemesi gerekecek.
    msg.mtype = 1;

    // 'sprintf', "Deneme mesaji...." metnini 'msg.mtext' dizisine
    // güvenli bir şekilde kopyalar.
    sprintf(msg.mtext,"%s\n","Deneme mesaji....");

    // 'msgsnd' (Message Send) fonksiyonunu çağır:
    // 'msgqid'     : Bu ID'li kuyruğa gönder.
    // '&msg'       : Gönderilecek mesajın adresi.
    // 'sizeof(msg.mtext)': GÖNDERİLECEK VERİNİN UZUNLUĞU.
    //                    (Tüm struct'ın değil, SADECE 'mtext'in boyutu!)
    // '0'          : Özel bayrak yok (örn: kuyruk doluysa bekle).
    rc = msgsnd(msgqid, &msg, sizeof(msg.mtext),0);

    return 0; // Program sonlanır.
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Ne Öğrendik?)
 * ====================================================================
 *
 * Bu kod, bir "Posta Göndericisi" programıdır.
 *
 * 1. 📬 Posta Kutusu Oluşturma:
 * 'ftok' ile benzersiz bir "adres" (key) bulunur.
 * 'msgget' ile bu adreste bir "posta kutusu" (mesaj kuyruğu)
 * oluşturulur ('IPC_CREAT | IPC_EXCL' sayesinde).
 *
 * 2. 📝 Mektup Yazma:
 * 'struct msg_buf' bir mektup zarfı gibidir.
 * 'msg.mtype = 1': Zarfın üzerine "Kutu #1" yazmak gibidir.
 * 'sprintf(msg.mtext...)': Mektubun içeriğini yazmaktır.
 *
 * 3. 📤 Postalama:
 * 'msgsnd()', hazırlanan mektubu ('&msg') o posta kutusuna ('msgqid')
 * atar.
 *
 * 4. ⌛ Sonuç (Ne Oldu?):
 * Program 'return 0;' ile sonlandığında, o mesaj artık programın
 * belleğinde değildir; **İşletim Sistemi Çekirdeğinde (Kernel)**
 * saklanmaktadır.
 *
 * 'ipcs' komutuyla baktığınızda, o kuyrukta 1 adet mesajın
 * ("Nmsgs: 1") beklediğini görürsünüz.
 *
 * Bu mesaj, başka bir program (bir "Alıcı" - Receiver) gelip
 * aynı 'key'i kullanarak 'msgget' ile kuyruğu bulana ve
 * 'msgrcv()' (Message Receive) fonksiyonunu çağırarak
 * "Tip 1" mesajları okuyana kadar orada bekleyecektir.
 * ====================================================================
 */