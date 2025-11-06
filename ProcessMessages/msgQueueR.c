//
// Created by Melih Yiğit Kotman
//
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/errno.h> // 'errno' (hata kodu) için
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h> // Mesaj kuyruğu fonksiyonları için

extern int errno; //error NO.
#define MSGPERM 0666 //msg queue permission
#define  MSGTXTLEN 128 //msg text length

int msgqid,rc;
int done; // (Bu kodda HİÇ KULLANILMIYOR)

// Gönderici ile BİREBİR AYNI yapı (struct) olmalıdır.
struct msg_buf{
    long mtype;
    char mtext[MSGTXTLEN];
}msg;

int main(int argc,char **argv) {
    key_t key;

    /*
     * ==========================================================
     * 1. ANAHTARI BULMA (Doğru Kısım)
     * ==========================================================
     * Gönderici ile aynı dosya ("msg_queue.c") ve aynı ID (42)
     * kullanılarak, Gönderici'nin ürettiği 'key'in AYNISI üretilir.
     * Prosesler birbirlerini bu "anahtar" sayesinde bulur.
     */
    key = ftok("msg_queue.c",42);

    /*
     * ==========================================================
     * 2. KUYRUĞA BAĞLANMA (!!! KRİTİK HATA BURADA !!!)
     * ==========================================================
     * Gönderici: msgget(key, MSGPERM|IPC_CREAT|IPC_EXCL) kullandı.
     * Anlamı: "Yoksa oluştur, varsa hata ver." Bu DOĞRUYDU (kurucu için).
     *
     * Alıcı (Bu kod): msgget(key, MSGPERM|IPC_EXCL) kullanıyor.
     * Anlamı: Bu kullanım hatalıdır. 'IPC_EXCL' bayrağı, SADECE
     * 'IPC_CREAT' ile birlikte kullanılır.
     *
     * SİZİN NİYETİNİZ (VAR OLANA BAĞLANMAK):
     * Var olan bir kuyruğa bağlanmak için 'IPC_CREAT' veya 'IPC_EXCL'
     * kullanmamalısınız. Doğru satır şöyle olmalıydı:
     *
     * msgqid = msgget(key, MSGPERM); // veya msgget(key, 0666);
     *
     * HATA SONUCU: Bu 'msgget' satırı başarısız olacak, 'msgqid'ye -1
     * atanacak ve 'errno' (hata kodu) 'EINVAL' (Geçersiz Argüman)
     * olarak ayarlanacaktır.
     */
    msgqid = msgget(key, MSGPERM|IPC_EXCL);

    // Hata nedeniyle, bu satır 'Message queue -1 created' basacaktır.
    // "created" metni, bir önceki koddan kopyala/yapıştır olduğu
    // için yanıltıcıdır.
    printf("Message queue %d created\n",msgqid);

    // 'ipcs' komutu çalışır ve Gönderici'nin oluşturduğu kuyruğu
    // listede GÖSTERİR (ama programınız ona bağlanamadı).
    printf("System IPC's \n");
    system("ipcs"); //Operating System command execute function

    // msg.mtype = 1; // (Alıcıda bu satıra gerek yok)

    /*
     * ==========================================================
     * 3. MESAJI ALMA (msgrcv)
     * ==========================================================
     * 'msgqid' -1 olduğu için, bu fonksiyon ANINDA BAŞARISIZ OLACAK
     * ve 'errno'ya "Invalid argument" hatası atayacaktır.
     *
     * EĞER DÜZGÜN ÇALIŞSAYDI (msgget doğru olsaydı):
     * 'msgqid' : Bu ID'li kuyruktan oku.
     * '&msg'   : Okuduğun mesajı bu 'msg' yapısına yaz.
     * 'sizeof(msg.mtext)': En fazla bu kadar byte oku.
     * '0' (mtype): "Mesaj tipine (mtype) bakma, kuyruktaki İLK
     * mesajı bana ver." (Çok önemli parametre!)
     * '0' (flags): "Özel bayrak yok." (Anlamı: Eğer kuyrukta
     * mesaj yoksa, program bu satırda BLOKLANIR
     * (donar) ve bir mesaj gelene kadar bekler.)
     */
    rc = msgrcv(msgqid, &msg, sizeof(msg.mtext),0,0);

    // 'msgrcv' başarısız olduğu için 'msg.mtext' çöp (garbage) veri
    // içerecek ve ekrana anlamsız bir şey basılacaktır.
    printf("received msg = %s\n -- %d\n",msg.mtext,msg.mtype);

    /*
     * ==========================================================
     * 4. KUYRUĞU SİLME (msgctl) - (Yorum Satırında)
     * ==========================================================
     * 'rc = msgctl(msgqid,IPC_RMID,NULL);'
     * Bu komut, 'msgqid'li kuyruğu sistemden TAMAMEN SİLER.
     * Genellikle tüm iş bittikten sonra "temizlik" yapmak için kullanılır.
     * Yorumu kaldırılsaydı, 'msgqid' -1 olduğu için bu da hata verirdi.
     */
    printf("Message queue %d is gone\n",msgqid);
    return 0;
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (Hata ve Niyet)
 * ====================================================================
 *
 * Bu program, bir "Posta Alıcısı" (Receiver) olmayı amaçlamaktadır.
 *
 * 1. 🔑 Anahtar Bulma ('ftok'):
 * DOĞRU. Gönderici ile aynı 'key'i bularak doğru "posta kutusunu"
 * hedefler.
 *
 * 2. 📬 Posta Kutusuna Erişme ('msgget'):
 * KRİTİK HATA! Kod, 'msgget(key, MSGPERM|IPC_EXCL)' kullanır.
 * 'IPC_EXCL' bayrağı, SADECE 'IPC_CREAT' ile (yani "oluştururken")
 * kullanılır. Var olan bir kuyruğa erişmek (bağlanmak) için
 * bayraksız 'msgget(key, MSGPERM)' kullanılmalıydı.
 *
 * Hata nedeniyle, program 'msgqid' için '-1' (hata) alır.
 *
 * 3. 📥 Mektubu Alma ('msgrcv'):
 * Programın niyeti, 'msgrcv' ile kuyruktan mesajı çekmektir.
 * Kullandığı 'mtype=0' parametresi "tipine bakmaksızın ilk mesajı al"
 * anlamına gelir.
 * Ancak 'msgqid' -1 olduğu için bu komut BAŞARISIZ olur ve
 * "Deneme mesaji...." ekrana basılamaz.
 *
 * 4. 🧹 Temizlik ('msgctl' - Yorumda):
 * Kodda, 'IPC_RMID' kullanarak kuyruğu sistemden silme (temizleme)
 * seçeneği de bulunmaktadır (ancak şu an yorum satırındadır).
 *
 * SONUÇ:
 * Bu kod, 'msgget' satırındaki hatalı bayrak kullanımı nedeniyle
 * çalışmayacak ve mesajı kuyruktan okuyamayacaktır.
 * ====================================================================
 */