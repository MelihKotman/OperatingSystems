/*
========================================================
Soru 1: Süreç Yönetimi ve Exec Ailesi (30 Puan)
========================================================

Aşağıdaki işlem (process) ağacını oluşturacak C programını yazınız.

Ana Süreç (P0):
- Bir adet çocuk süreç (P1) oluşturur.
- P1 sürecinin bitmesini wait() ile bekler.
- Çocuğu sonlandıktan sonra kendi PID (Process ID) değerini ekrana yazdırır
  ve sonlanır.

P1 Süreci:
- Kendi içinde bir çocuk süreç (P2) oluşturur.
- P2 sürecinin bitmesini wait() ile bekler.
- Çocuğu sonlandıktan sonra kendi PID (Process ID) değerini ekrana yazdırır
  ve sonlanır.

P2 Süreci:
- execlp sistem çağrısını kullanarak
  bulunduğu dizindeki dosyaları listeleyen
  "ls -l" komutunu çalıştırır.
- Komut çalıştıktan sonra süreç sonlanır.

İpucu:
- fork()
- wait()
- execlp()
- getpid()
fonksiyonlarını kullanınız.
*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main(int argc, char **argv) {
  pid_t c1, c2;
  c1 = fork();
  if (c1 == 0) {
    c2 = fork();
    if (c2 == 0) {
      execlp("ls", "ls", "-l", NULL);
      perror("execlp failed");
      exit(1);
    }
    wait(NULL);
    printf("P1 Süreci PID: %d\n", getpid());
    exit(0);
  }
  wait(NULL);
  printf("P0 Süreci PID: %d\n", getpid());
  return 0;
}
*/

/*
========================================================
Soru 2: Bellek Yönetimi Hesaplamaları (35 Puan)
========================================================

Aşağıda verilen 16-bit mantıksal adres:
0010110010010110 (binary)

Bu adresin fiziksel adres karşılığını,
her iki bellek yönetim tekniği için
ayrı ayrı hesaplayınız.

--------------------------------------------------------
a) Sayfalama (Paging) Sistemi
--------------------------------------------------------

- Sayfa boyutu: 512 byte
- Sayfa tablosu kuralı:
  Frame No = Sayfa Numarası + 3

İstenilenler:
- Mantıksal adresi sayfa numarası + ofset olarak ayırınız.
- Frame numarasını hesaplayınız.
- Fiziksel adresi oluşturunuz.
- Sonucu ikili (binary) sistemde açıkça yazınız.

--------------------------------------------------------
b) Segmentasyon (Segmentation) Sistemi
--------------------------------------------------------

- Maksimum segment boyutu: 1024 adres (1K)
- Segment tablosu kuralı:
  Base Address = 30 + Segment Numarası
  (Base Address onluk tabandadır)

İstenilenler:
- Mantıksal adresi segment numarası + ofset olarak ayırınız.
- Baz adresini hesaplayınız.
- Fiziksel adresi bulunuz.
- Sonucu onluk (decimal) sistemde yazınız.

*/

/* a) Sayfalama (Paging) Sistemi
 * Sayfa boyutu: 512 byte = 2^9 byte yani 9 bit ofset
 *
 * Mantıksal adres: 0010 1100 1001 0110
 *
 * Sayfa Numarası: 0010 110 (ilk 7 bit)
 * Ofset: 0 1001 0110 (son 9 bit)
 *
 * Mevcut Kural: Çerçeve numarası = Sayfa Numarası + 3
 *
 * Sayfa Numarası (decimal): 22
 * Çerçeve Numarası: 22 + 3 = 25
 * Çerçeve Numarası (binary): 0011 001
 *
 * Fiziksel Adres: Çerçeve Numarası + Ofset
 * Fiziksel Adres (binary): 0011 0010 1001 0110
 *
 * Sonuç: Fiziksel Adres = 12918 (decimal) ve 0011 0010 1001 0110 (binary)
 * ------------------------------------------------------------------------------
 * b) Segmentasyon (Segmentation) Sistemi
 * Maksimum segment boyutu: 1024 adres = 2^10 adres yani 10 bit ofset
 *
 * Mantıksal adres: 0010 1100 1001 0110
 *
 * Segment Numarası: 0010 11 (ilk 6 bit)
 * Ofset: 00 1001 0110 (son 10 bit)
 *
 * Mevcut Kural: Baz Adresi = 30 + Segment Numarası
 *
 * Segment Numarası (decimal): 11
 * Baz Adresi: 30 + 11 = 41
 *
 * Fiziksel Adres: Baz Adresi + Ofset
 * Ofset (decimal): 150
 * Fiziksel Adres: 41 + 150 = 191
 *
 * Sonuç: Fiziksel Adres = 191 (decimal) ve 0010 1111 1111 (binary)
 */

/*
========================================================
Soru 3: İplikler (Threads) ve Mesaj Kuyruğu (35 Puan)
========================================================

Bir "Bilet Satış Simülasyonu" için
POSIX kütüphanesi kullanılarak
aşağıdaki isterleri sağlayan
C programını yazınız.

--------------------------------------------------------
Yapı:
--------------------------------------------------------

- Bir adet mesaj kuyruğu (Message Queue) oluşturulacaktır.

--------------------------------------------------------
Müşteri İpliği (Thread):
--------------------------------------------------------

- Rastgele 1 ile 5 arasında bir koltuk sayısı üretir.
- Bu değeri mesaj kuyruğuna gönderir.
- Mesaj tipi örnek olarak 1 olabilir.

--------------------------------------------------------
Gişe (Ana Süreç):
--------------------------------------------------------

- Mesaj kuyruğundan gelen istekleri okur.
- Başlangıç toplam koltuk sayısı: 50
- Her istek için:
  - Eğer yeterli koltuk varsa:
    * Stoktan düşer
    * "X bilet satıldı, Kalan: Y" mesajını yazar
  - Eğer stok yetersizse veya biterse:
    * İşlemi sonlandırır

--------------------------------------------------------
Temizlik:
--------------------------------------------------------

- Mesaj kuyruğu silinmelidir.
- Gerekli kaynaklar serbest bırakılmalıdır.

İpucu:
- msgget()
- msgsnd()
- msgrcv()
- pthread_create()

Not:
- Senkronizasyon hatası olmaması için
  mesaj kuyruğunun doğal sıralaması
  kullanılabilir.

*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
struct bilet_istegi {
    long msgtipi;
    int koltuk_sayisi;
};
int msgid;
void *musteri_thread(void *arg) {
    struct bilet_istegi istek;
    istek.msgtipi = 1;
    while (1) {
        istek.koltuk_sayisi = (rand() % 5) + 1;
        if (msgsnd(msgid, &istek, sizeof(int), 0) == -1) {
            perror("msgsnd failed");
            break;
        }
        sleep(1); // Biraz bekle
    }
    return NULL;
}
int main(int argc, char **argv) {
  pthread_t musteri;
  int toplam_koltuk = 50;
  struct bilet_istegi istek;
  key_t key = ftok(".", 'B');
  msgid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL);
  if (msgid == -1) {
      perror("msgget failed");
      exit(1);
  }
  srand(time(NULL));
  pthread_create(&musteri, NULL, musteri_thread, NULL);
  printf("Gişe açıldı. Toplam koltuk: %d\n", toplam_koltuk);
  while (toplam_koltuk > 0) {
    msgrcv(msgid,&istek,sizeof(istek.koltuk_sayisi),1,0);
    if (toplam_koltuk >= istek.koltuk_sayisi) {
        toplam_koltuk -= istek.koltuk_sayisi;
        printf("%d bilet satıldı, Kalan: %d\n", istek.koltuk_sayisi, toplam_koltuk);
    } else {
        printf("Yeterli koltuk yok. İşlem sonlandırılıyor.\n");
        break;
    }
  }
  printf("Stok tükenmiştir veya yetersizdir (%d).\n", toplam_koltuk);
  msgctl(msgid, IPC_RMID, NULL);
  return 0;
}
*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <time.h>
struct bilet_istegi {
  long msg_type;
  int koltuk_sayisi;
} istek;
int main() {
  int msg_id;
  key_t key;
  int stok = 50;
  pid_t pid;
  srand(time(NULL));
  key = ftok(".", 'B');
  msg_id = msgget(key, 0666 | IPC_CREAT | IPC_EXCL);
  pid = fork();
  if (pid == 0) {
    // --- ÇOCUK SÜREÇ (MÜŞTERİ) ---
    struct bilet_istegi msg;
    msg.msg_type = 1;
    while(1) {
      msg.koltuk_sayisi = (rand() % 5) + 1;
      if (msgsnd(msg_id, &msg, sizeof(int), 0) == -1) {
        printf("Çocuk: Gişe kapandı, eve gidiyorum.\n");
        break;
      }
      printf("Çocuk (PID %d): %d bilet istedi.\n", getpid(), msg.koltuk_sayisi);
      sleep(1);
    }
    exit(0);
  }
  else {
    struct bilet_istegi gelen;
    printf("--- GİŞE AÇILDI (PID %d) ---\n", getpid());
    while (stok > 0) {
      msgrcv(msg_id, &gelen, sizeof(int), 1, 0);

      printf(">> Gişe: %d istek geldi. ", gelen.koltuk_sayisi);

      if (stok >= gelen.koltuk_sayisi) {
        stok -= gelen.koltuk_sayisi;
        printf("Satıldı. Kalan Stok: %d\n", stok);
      } else {
        printf("Yetersiz stok!\n");
        break;
      }
    }
    printf("--- STOK BİTTİ, KUYRUK SİLİNİYOR ---\n");
    msgctl(msg_id, IPC_RMID, NULL);
    wait(NULL);
    printf("Ana süreç kapanıyor.\n");
  }

  return 0;
}
*/

/*
========================================================
Soru 4: Dosya İşlemleri - POSIX System Calls (25 Puan)
========================================================

Linux ortamında "kaynak.txt" adındaki bir dosyanın
içeriğini okuyup, "kopya.txt" adında yeni bir dosya
oluşturarak içine yazan C programını yazınız.

Kurallar:
- Standart C kütüphanesi fonksiyonları
  (fopen, fprintf, fread, fwrite vb.) KULLANILMAYACAKTIR.
- Sadece aşağıdaki POSIX sistem çağrıları kullanılacaktır:
  * open()
  * read()
  * write()
  * close()

Dosya işlemleri:
- "kopya.txt" dosyası:
  * Yoksa oluşturulmalıdır (O_CREAT)
  * Varsa içeriği temizlenmelidir (O_TRUNC)
- Dosya açma bayrakları uygun şekilde kullanılmalıdır.

Okuma işlemi:
- 1024 byte'lık bir buffer (tampon) kullanılacaktır.
- read() çağrısı döngü içerisinde kullanılmalıdır.
- Okunan byte sayısı kadar write() yapılmalıdır.


*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
int main(int argc, char **argv) {
  int kaynak_fd, kopya_fd;
  ssize_t bytes_read, bytes_written;
  char buffer[BUFFER_SIZE];
  kaynak_fd = open("/Users/melih/CLionProjects/OperatingSystemsCode/Final/kaynak.txt", O_RDONLY );
  if (kaynak_fd == -1) {
    perror("kaynak.txt açılamadı");
    exit(1);
  }
  printf("Kaynak dosyası açıldı\n");
  kopya_fd = open("/Users/melih/CLionProjects/OperatingSystemsCode/Final/kopya.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (kopya_fd == -1) {
    perror("kopya.txt açılamadı");
    close(kaynak_fd);
    exit(1);
  }
  while ((bytes_read = read(kaynak_fd, buffer, BUFFER_SIZE)) > 0) {
    // Okuduğumuz miktar kadarını diğer dosyaya yazıyoruz
    bytes_written = write(kopya_fd, buffer, bytes_read);
    if (bytes_written != bytes_read) {
      perror("Hata: Yazma islemi eksik oldu");
      break;
    }
  }
  if (bytes_read == -1) {
    perror("Hata: Okuma sirasinda sorun olustu");
  }
  printf("Kopyalama islemi basariyla tamamlandi.\n");
  close(kaynak_fd);
  close(kopya_fd);
  return 0;
}
*/

/*
========================================================
Soru 5: CPU Planlama Algoritmaları - Round Robin (25 Puan)
========================================================

Aşağıdaki süreç tablosunu,
Round Robin (RR) algoritmasına göre analiz ediniz.

Zaman Kuantumu (Time Quantum): 3 ms

--------------------------------------------------------
Süreç Tablosu:
--------------------------------------------------------

Süreç | Varış Zamanı (ms) | Patlama Zamanı (ms)
P1    | 0                | 8
P2    | 1                | 4
P3    | 2                | 2

--------------------------------------------------------
İstenenler:
--------------------------------------------------------

a) Round Robin algoritmasına göre
   Gantt şemasını çizerek süreçlerin
   CPU'da çalışma sırasını gösteriniz.

b) Her sürecin Bekleme Süresini
   (Waiting Time) ayrı ayrı hesaplayınız.

c) Sistemin Ortalama Bekleme Süresini bulunuz.


*/
/* Öncelikle Gantt şemasını oluşturalım:
 * Zaman Kuantumu: 3 ms
 * | P1 | P2 | P3 | P1 | P2 | P1 |
 * 0    3    6    8    11   12   14
 *
 * Ortalama Bekleme Süresi Hesaplamaları:
 * P1: (0-0) + (8-3) + (12-11) = 0 + 5 + 1 = 6 ms
 * P2: (3-1) + (11-6) = 2 + 5 = 7 ms
 * P3: (6-2) = 4 ms
 * Ortalama Bekleme Süresi = (6 + 7 + 4) / 3 = 17 / 3 ≈ 5.67 ms
 *
 * Ortalama Çalışma Süresi Hesaplamaları:
 * P1: 8 ms + 6 ms = 14 ms
 * P2: 4 ms + 7 ms = 11 ms
 * P3: 2 ms + 4 ms = 6 ms
 * Ortalama Çalışma Süresi = (14 + 11 + 6) / 3 = 31 / 3 ≈ 10.33 ms
 */

/*
========================================================
Soru 6: Okuyucu - Yazıcı Problemi (Readers-Writers) (30 Puan)
========================================================

Bir veritabanına:
- Aynı anda birden fazla OKUYUCU erişebilir.
- Bir YAZICI eriştiğinde:
  * Başka hiçbir okuyucu veya yazıcı erişemez.

Aşağıda verilen C kodundaki boşlukları,
senkronizasyonu doğru şekilde sağlayacak
sem_t (semafor) ve mutex fonksiyonları ile doldurunuz.

Amaç:
- Okuyucu sayısını takip etmek
- İlk gelen okuyucunun yazarı kilitlemesi
- Son çıkan okuyucunun kilidi açması

--------------------------------------------------------
Verilen Değişkenler:
--------------------------------------------------------

sem_t db_rw;                 // Veritabanı yazma kilidi
pthread_mutex_t mutex;       // Okuyucu sayısını koruyan mutex
int read_count = 0;          // Aktif okuyucu sayısı

--------------------------------------------------------
Tamamlanacak Kod:
--------------------------------------------------------

void *okuyucu(void *arg) {
    while(1) {

        // 1. ADIM: Okuyucu sayısını güvenli artır
        ___________________________________________ // mutex kilitle
        read_count++;
        if (read_count == 1) {
            _______________________________________ // ilk okuyucuysa yazarı kilitle (sem_wait)
        }
        ___________________________________________ // mutex kilidi aç

        // KRİTİK BÖLGE: OKUMA İŞLEMİ
        printf("Okuyucu okuyor...\n");

        // 2. ADIM: Okuyucu sayısını azalt
        ___________________________________________ // mutex kilitle
        read_count--;
        if (read_count == 0) {
            _______________________________________ // son okuyucuysa yazara izin ver (sem_post)
        }
        ___________________________________________ // mutex kilidi aç
    }
}


*/

/*
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

sem_t db_rw;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int read_count = 0;
int read_var = 0;

void *okuyucu(void *arg) {
  int ID = (int) arg;
  while (1) {
    pthread_mutex_lock(&mutex);
    read_count++;
    if (read_count == 1) {
      sem_wait(&db_rw);
    }
    pthread_mutex_unlock(&mutex);

    printf("Okuyucu veriyi okuyor: ID = %d Veri = %d \n", ID, read_var);

    pthread_mutex_lock(&mutex);
    read_count--;
    if (read_count == 0) {
      sem_post(&db_rw);
    }
    pthread_mutex_unlock(&mutex);
    sleep(1);
  }

}
void *yazici(void *arg) {
  int ID = (int) arg;
  while (1) {
    sem_wait(&db_rw);
    read_var++;
    printf("Yazici veriyi yazdi: ID = %d Veri = %d \n", ID, read_var);
    sem_post(&db_rw);
    sleep(2);
  }
}
int main(int argc, char **argv) {
  pthread_t r_threads[5], w_threads[2];
  sem_init(&db_rw, 0, 1);
  for (int i = 0; i < 5; i++) {
    pthread_create(&r_threads[i], NULL, okuyucu, (void *)i);
  }
  for (int i = 0; i < 2; i++) {
    pthread_create(&w_threads[i], NULL, yazici, (void *)i);
  }
  for (int i = 0; i < 5; i++) {
    pthread_join(r_threads[i], NULL);
  }
  for (int i = 0; i < 2; i++) {
    pthread_join(w_threads[i], NULL);
  }
  sleep(2);
  printf("Okuma ve yazma islemleri tamamlandi. Son veri = %d\n",read_var);

  return 0;
}
*/

/*
========================================================
Soru 7: Filozofları Doyurma Problemi ve Deadlock (20 Puan)
========================================================

5 Filozof ve 5 Çatal bulunan bir sistemde,
her filozofun:
- Önce solundaki,
- Sonra sağındaki
çatalı almaya çalıştığı "saf çözüm" deadlock'a yol açabilir.

--------------------------------------------------------
İstenenler:
--------------------------------------------------------

a) Bu senaryoda deadlock'ın neden oluştuğunu
   tek cümleyle açıklayınız.

b) PDF notlarında anlatılan
   "Asimetrik Çözüm" (Tek / Çift numaralı filozof yaklaşımı)
   mantığını kısaca açıklayınız.

c) Bu yaklaşımın deadlock'ı nasıl engellediğini belirtiniz.


*/

/*
 * a) Deadlock, tüm filozofların aynı anda sol çatalı alması ve sağ çatalı beklemesi durumunda oluşur,
 * çünkü her biri diğerinin elindeki çatalı bekler ve hiçbir filozof ilerleyemez.
 *
 * b) Asimetrik Çözümde, tek numaralı filozoflar önce sol çatalı, çift numaralı filozoflar ise önce sağ çatalı alır.
 * Bu sayede tüm filozofların aynı anda aynı çatalı almaları engellenir.
 *
 * c) Bu yaklaşım, en az bir filozofun her zaman iki çatalı alabilmesini sağlar,
 * böylece deadlock durumu önlenir çünkü tüm filozoflar aynı anda bloke olmaz.
 */

/*
========================================================
Soru 8: CPU Planlama Algoritmaları
(Hepsini İçeren Soru) (40 Puan)
========================================================

Aşağıdaki süreç tablosunu dikkate alarak,
istenilen CPU planlama algoritmaları için:

- Gantt Şemalarını çiziniz
- Her algoritma için Ortalama Bekleme Süresini hesaplayınız

--------------------------------------------------------
Süreç Tablosu:
--------------------------------------------------------

Süreç | Varış Zamanı (ms) | Patlama Zamanı (ms) | Öncelik
P1    | 0                | 10                 | 3
P2    | 1                | 6                  | 1 (En Yüksek)
P3    | 3                | 2                  | 2
P4    | 5                | 4                  | 4 (En Düşük)

--------------------------------------------------------
İstenen Algoritmalar:
--------------------------------------------------------

a) FCFS (First Come First Served)
- İlk gelen süreç, ilk hizmet alır.
- Kesme (preemption) yoktur.

b) SJF (Shortest Job First - Preemptive / SRTF)
- Kesilmeli en kısa iş önce algoritmasıdır.
- Daha kısa kalan süresi olan bir süreç geldiğinde,
  mevcut süreç durdurulur ve yeni süreç çalıştırılır.

c) Öncelik Tabanlı (Priority - Non-Preemptive)
- Kesilmesiz öncelik sıralaması uygulanır.
- İşlemci bir sürece verildiğinde,
  süreç bitene kadar geri alınmaz.
- Sayı küçüldükçe öncelik artmaktadır.

d) Round Robin (RR)
- Zaman Kuantumu (Quantum): 4 ms
- Süreçler sırayla CPU alır,
  süresi bitmeyenler kuyruğun sonuna atılır.

--------------------------------------------------------
Her algoritma için:
--------------------------------------------------------
- Gantt Şeması
- Süreç bazlı Bekleme Süreleri
- Ortalama Bekleme Süresi
hesaplanacaktır.
*/

/*
 * a) FCFS:
 * Gantt Şeması:
 * | P1 | P2 | P3 | P4 |
 * 0    10   16   18   22
 *
 * Ortalama Bekleme Süreleri:
 * P1 = 0 ms
 * P2 = 9 ms
 * P3 = 13 ms
 * P4 = 13 ms
 * Ortalama Bekleme Süresi = (0 + 9 + 13 + 13) / 4 = 8.75 ms
 *
 * Ortalama Çalışma Süreleri:
 * P1 = 0 + 10 = 10 ms
 * P2 = 9 + 6 = 15 ms
 * P3 = 13 + 2 = 15 ms
 * P4 = 13 + 4 = 17 ms
 * Ortalama Çalışma Süresi = (10 + 15 + 15 + 17) / 4 = 14.25 ms
 *
 * b) SJF (Preemptive):
 * Gantt Şeması:
 * | P1 | P2 | P3 | P2 | P4 | P1  |
 * 0    1    3    5    9    13   22
 *
 * Ortalama Bekleme Süreleri:
 * P1 = 12 ms
 * P2 = 2 ms
 * P3 = 0 ms
 * P4 = 4 ms
 * Ortalama Bekleme Süresi = (12 + 2 + 0 + 4) / 4 = 4.5 ms
 *
 * Ortalama Çalışma Süreleri:
 * P1 = 12 + 10 = 20 ms
 * P2 = 2 + 6 = 8 ms
 * P3 = 0 + 2 = 2 ms
 * P4 = 4 + 4 = 8 ms
 * Ortalama Çalışma Süresi = (20 + 8 + 2 + 8) / 4 = 9.5 ms
 *
 * c) Öncelik Tabanlı (Preemptive):
 * Gantt Şeması:
 * | P1 | P2 | P3 | P1 | P4 |
 * 0    1    7    9    18   22
 *
 * Ortalama Bekleme Süreleri:
 * P1 = 8 ms
 * P2 = 0 ms
 * P3 = 4 ms
 * P4 = 13 ms
 * Ortalama Bekleme Süresi = (8 + 0 + 4 + 13) / 4 = 6.25 ms
 *
 * Ortalama Çalışma Süreleri:
 * P1 = 8 + 10 = 18 ms
 * P2 = 0 + 6 = 6 ms
 * P3 = 4 + 2 = 6 ms
 * P4 = 13 + 4 = 17 ms
 * Ortalama Çalışma Süresi = (18 + 6 + 6 + 17) / 4 = 11.75 ms
 *
 * d) Round Robin (Quantum = 4 ms):
 * Gantt Şeması:
 * | P1 | P2 | P3 | P4 | P1 | P2 |  P1 |
 * 0    4    8    10   14   18   20   22
 *
 * Ortalama Bekleme Süreleri:
 * P1 = 10 ms
 * P2 = 13 ms
 * P3 = 5 ms
 * P4 = 5 ms
 * Ortalama Bekleme Süresi = (10 + 13 + 5 + 5) / 4 = yaklaşık 8 ms
 *
 * Ortalama Çalışma Süreleri:
 * P1 = 10 + 10 = 20 ms
 * P2 = 13 + 6 = 18 ms
 * P3 = 5 + 2 = 7 ms
 * P4 = 5 + 4 = 9 ms
 * Ortalama Çalışma Süresi = (20 + 18 + 7 + 9) / 4 = yaklaşık 10 ms
 */

/*
========================================================
Soru 9: Bellek Yerleştirme ve Parçalanma
(Memory Allocation) (35 Puan)
========================================================

Bir işletim sisteminde bellek,
Devingen Bölümlü (Dynamic Partitioning)
yöntemi ile yönetilmektedir.

--------------------------------------------------------
Bellekteki Boşluklar (Soldan Sağa):
--------------------------------------------------------

- 100 KB
- 500 KB
- 200 KB
- 300 KB
- 600 KB

--------------------------------------------------------
Sisteme Gelen Süreçler:
--------------------------------------------------------

P1: 212 KB
P2: 417 KB
P3: 112 KB
P4: 426 KB

--------------------------------------------------------
a) Yerleştirme Algoritmaları:
--------------------------------------------------------

Aşağıdaki algoritmalar için,
süreçlerin hangi boşluklara yerleştiğini
adım adım gösteriniz.

- İlk Uygun (First Fit):
  Baştan başla, sığdığı ilk boşluğa yerleştir.

- En Uygun (Best Fit):
  Tüm boşlukları kontrol et,
  en az boşluk kalan (en sıkı uyan) bölgeye yerleştir.

- Sonraki Uygun (Next Fit):
  En son yerleştirme yapılan noktadan
  itibaren aramaya devam et.

Sorular:
- Hangi algoritma P4 için yer bulamaz?
- Yerleştirme adımlarını açıkça gösteriniz.

--------------------------------------------------------
b) Parçalanma Analizi:
--------------------------------------------------------

- Bu senaryoda Dış Parçalanma (External Fragmentation)
  oluşmuş mudur? Neden?

- İç Parçalanma (Internal Fragmentation) ile
  arasındaki farkı açıklayınız.


*/

/* a) Yerleştirme Algoritmaları:
 *
 * 1. İlk Uygun (First Fit):
 * Mantık: İlk sığan boşluğa yerleştir.
 * P1 geldi | 100KB | 500KB | 200KB | 300KB | 600KB |
 * 100'e sığmaz, 500'e sığar → P1 burada (288KB boş kaldı)
 * P2 geldi | 100KB | P1(212KB) | 288 KB | 200KB | 300KB | 600KB |
 * 100'e 288'e 200'e ve 300'e sığmaz, 600'e sığar → P2 burada (183KB boş kaldı)
 * P3 geldi | 100KB | P1(212KB) | 288 KB | 200KB | 300KB | P2(417KB) | 183KB |
 * 100'e sığmaz 288'e sığar → P3 burada (176KB boş kaldı)
 * P4 geldi | 100KB | P1(212KB) | P3(112KB) | 176KB | 200KB | 300KB | P2(417KB) | 183KB |
 * Buradaki hiçbirine sığmaz çünkü en büyük boşluk 300KB
 *
 * 2. En Uygun (Best Fit):
 * Mantık: Tüm boşlukları kontrol et, en az boşluk kalan yere yerleştir.
 * P1 geldi | 100KB | 500KB | 200KB | 300KB | 600KB |
 * 500'e 300'e ve 600'a sığar ama en az boşluk 300KB → P1 burada (88KB boş kaldı)
 * P2 geldi | 100KB | 500KB | 200KB | P1(212KB) | 88KB | 300KB | 600KB |
 * 500'e 600'e sığar ama en az boşluk 500KB → P2 burada (83KB boş kaldı)
 * P3 geldi | 100KB | P2(417KB) | 83KB |  200KB | P1(212KB) | 88KB | 300KB | 600KB |
 * 200'e 300'e ve 600'e sığar ama en az boşluk 200KB → P3 burada (88KB boş kaldı)
 * P4 geldi | 100KB | P2(417KB) | 83KB | P3(112KB) | 88KB | P1(212KB) | 88KB | 300KB | 600KB |
 * 600'e sığar (174KB boş kaldı) → P4 burada
 *
 * 3. Sonraki Uygun (Next Fit):
 * Mantık: En son yerleştirme yapılan noktadan itibaren aramaya devam et.
 * P1 geldi | 100KB | 500KB | 200KB | 300KB | 600KB |
 * 500'e sığar → P1 burada (288KB boş kaldı)
 * P2 geldi | 100KB | P1(212KB) | 288 KB | 200KB | 300KB | 600KB |
 * 600'e sığar → P2 burada (183KB boş kaldı)
 * P3 geldi | 100KB | P1(212KB) | 288 KB | 200KB | 300KB | P2(417KB) | 183KB |
 * 183'e sığar → P3 burada (71KB boş kaldı)
 * P4 geldi | 100KB | P1(212KB) | 288 KB | 200KB | 300KB | P2(417KB) | P3(112KB) | 71KB |
 * Burada başa döner ve başa dönünce hiçbir boşluğa sığmaz çünkü en büyük boşluk 300KB
 *
 * b) Parçalanma Analizi:
 * Burada iç parçalanma yoktur çünkü zaten geliştirilme amacı iç parçalanmaya yöneliktir.
 * Fakat bakıldığında dış parçalanma vardır çünkü yukarıya bakarsak toplam boş alan giremeyen P4 için yeterli
 * Ama bitiştirme olmadığı için yerleştirilememiştir.
 */

/*
========================================================
Soru 10: Linux Dosya Sistemi, TLB ve Sanal Bellek
(25 Puan)
========================================================

Aşağıdaki kısa cevaplı soruları yanıtlayınız.

--------------------------------------------------------
a) Linux Dosya Hiyerarşisi
--------------------------------------------------------

Linux işletim sisteminde aşağıdaki dizinlerin
temel görevlerini birer cümleyle açıklayınız:

- /bin
- /etc
- /dev


--------------------------------------------------------
b) TLB (Translation Lookaside Buffer)
--------------------------------------------------------

Sayfalamalı (Paging) bellek yönetiminde:

- Mantıksal adresin fiziksel adrese çevrilmesinde
  TLB'nin görevi nedir?

- "TLB Miss" (TLB Iskalama) durumunda
  sistem hangi adımları izler?



--------------------------------------------------------
c) Sayfa Hatası (Page Fault) ve Swapping
--------------------------------------------------------

- Bir program çalışırken ihtiyaç duyduğu sayfa
  RAM'de (fiziksel bellekte) yoksa ne olur?

- Bu durumda Swapping (Takas) işlemi
  nasıl devreye girer?


*/

/* a) Linux Dosya Hiyerarşisi:
 * /bin = Temel kullanıcı komutlarının çalıştırılabilir dosyalarının bulunduğu klasördür.
 * /etc = Sistemin ve kurulan programların yapılandırma ayar dosyalarının tutulduğu klasördür.
 * /dev = Donanım aygıtlarının dosya olarak temsil edildiği klasördür.
 *
 * b) TLB (Translation Lookaside Buffer):
 * Görevi: CPU ile RAM arasında bulunan küçük, hızlı bir önbellektir.
 * Amacı: Mantıksal adreslerin fiziksel adreslere hızlı bir şekilde çevrilmesini sağlamaktır.
 * TLB Miss Durumu: Eğer aranan sayfa TLB'de bulunamazsa,
 * sistem sayfa tablosuna bakar ve gerekli fiziksel adresi bulur,
 * ardından bu adresi TLB'ye ekler.
 * TLB Hit Durumu: Eğer aranan sayfa TLB'de bulunursa,
 * direkt fiziksel adrese erişilir.
 *
 * c) Sayfa Hatası (Page Fault) ve Swapping:
 * Bir program çalışırken ihtiyaç duyduğu sayfa RAM'de yoksa,
 * donanım tarafından bir "kesme" Trap gönderilir.
 *
 * Swapping ise o an RAM doluysa veya ilgili sayfa yoksa işletim sistemi
 * Backing Store denilen bölgeye başvurur.
 * Buradan kullanılmayan veya az kullanılan bir sayfayı RAM'e alır
 * İhtiyaç duyulan sayfayı ise RAM'e yükler.
 */

/*
========================================================
Soru 11: Assembly ve Sistem Çağrıları (System Calls)
========================================================

PDF'in 24. sayfasında,
bir C kodunun arka plandaki Assembly karşılığı verilmiştir.

İşletim sisteminde bir sistem çağrısı (system call)
yapılırken, kayıtçıların (registers) görevleri
aşağıdaki gibidir.

Aşağıdaki Assembly kod satırlarının yanına,
ne iş yaptıklarını kısaca yazınız.
(Özellikle sys_write örneği üzerinden)

--------------------------------------------------------
Assembly Kodları:
--------------------------------------------------------

mov eax, 4:
________________________________________________________
(EAX register'ına yapılacak sistem çağrısının numarası
yüklenir. 4 → sys_write)

mov ebx, 1:
________________________________________________________
(EBX register'ına dosya tanıtıcısı (file descriptor)
yüklenir. 1 → stdout)

mov ecx, veri:
________________________________________________________
(ECX register'ına yazdırılacak verinin
başlangıç adresi yüklenir)

mov edx, 2:
________________________________________________________
(EDX register'ına yazdırılacak byte sayısı yüklenir)

int 0x80:
________________________________________________________
(Kernel moda geçilir ve sistem çağrısı çalıştırılır)


*/

/*
========================================================
Soru 12: Heap Yönetimi (brk & sbrk vs malloc)
========================================================

Programlamada dinamik bellek ayırmak için genellikle
malloc() fonksiyonu kullanılır.
Ancak işletim sistemi seviyesinde bellek yönetimi
farklı sistem çağrıları ile gerçekleştirilir.

--------------------------------------------------------
a) brk() ve sbrk()
--------------------------------------------------------

- brk() ve sbrk() sistem çağrılarının
  temel görevi nedir?

- Bu çağrıların malloc() fonksiyonu ile
  ilişkisini açıklayınız.

--------------------------------------------------------
b) free() Fonksiyonu
--------------------------------------------------------

- free() fonksiyonu belleği fiziksel olarak
  hemen işletim sistemine geri verir mi?

- Yoksa sadece belleği kullanılabilir olarak mı
  işaretler?

Kısaca açıklayınız.


*/

/* a) brk() ve sbrk():
 * Temel Görev: brk() ve sbrk() sistem çağrıları,
 * programın veri segmentinin (heap) boyutunu
 * artırmak veya azaltmak için kullanılır.
 * brk() belirli bir adresi işaret ederken,
 * sbrk() ise mevcut brk noktasından belirli bir miktar
 * artırma veya azaltma yapar.
 * İlişki: malloc() fonksiyonu, arka planda
 * brk() ve sbrk() çağrılarını kullanarak
 * dinamik bellek ayırır. Aslında bu standart
 * C kütüphanesi fonksiyonu, arka planda
 * işletim sistemi seviyesindeki bu çağrıları
 * kullanarak heap'ten bellek blokları talep eder.

 *
 * b) free() Fonksiyonu:
 * free() fonksiyonu belleği fiziksel olarak
 * hemen işletim sistemine geri vermez.
 * Sadece belleği kullanılabilir olarak işaretler,
 * böylece gelecekteki malloc() çağrıları
 * bu alanı yeniden kullanabilir.
 */

/*
========================================================
Soru 13: Statik ve Dinamik Bağlama (Linking)
========================================================

Bir program derlenirken kullanılan kütüphanelerin
bağlanma yöntemi, performansı ve dosya boyutunu
doğrudan etkiler.

Aşağıdaki ifadelerin:
- Statik Bağlama (Static Linking)
- Dinamik Bağlama (Dynamic Linking)

hangisine ait olduğunu belirtiniz
ve nedenini kısaca açıklayınız.

--------------------------------------------------------
İfadeler:
--------------------------------------------------------

"100 tane printf kullanan program varsa,
sistemde 100 tane printf kodu kopyalanır
ve bellek şişer."

→ ________________________________
(Nedeni ile birlikte açıklayınız)

--------------------------------------------------------

"Bellekte sadece tek bir printf kodu
(shared library) bulunur,
tüm programlar oraya yönlendirilir."

→ ________________________________
(Nedeni ile birlikte açıklayınız)


*/

/* "100 tane printf kullanan program varsa,
 * sistemde 100 tane printf kodu kopyalanır
 * ve bellek şişer."
 * -> Statik Bağlama (Static Linking)
 * Statik bağlamada program derlenirken ihtiyaç duyduğu
 * kütüphane kodları programın içine dahil edilir.
 * Bu nedenle, aynı kütüphane fonksiyonunu
 * kullanan birden fazla program varsa,
 * her program kendi kopyasını içerir,
 * bu da bellek kullanımını artırır.
 *
 * "Bellekte sadece tek bir printf kodu
 * (shared library) bulunur,
 * tüm programlar oraya yönlendirilir."
 * -> Dinamik Bağlama (Dynamic Linking)
 * Dinamik bağlamada kütüphane kodları
 * programın dışında tutulur ve çalışma zamanında
 * yüklenir.
 * Bu sayede, aynı kütüphane fonksiyonunu
 * kullanan birden fazla program,
 * tek bir kütüphane kopyasını paylaşır,
 * bu da bellek kullanımını optimize eder.
 */

/*
========================================================
Soru 14: Süreç Durumları (Process States)
========================================================

PDF'in 1. sayfasında,
"Süreç Durumu" (Process State) kavramı açıklanmaktadır.

Bir sürecin yaşam döngüsündeki
5 temel durumu:

- Çizerek (state diagram)
VEYA
- Maddeler halinde

yazarak açıklayınız.

--------------------------------------------------------
İpucu:
--------------------------------------------------------

New        → Ready
Ready      → Running
Running    → Waiting
Waiting    → Ready
Running    → Terminated

Her durumun ne anlama geldiğini
kısaca belirtiniz.

*/

/* Süreç Durumları:
 *
 * 1. New (Yeni):
 * Süreç oluşturulduğunda bu duruma geçer.
 * Henüz çalışmaya başlamamıştır.
 *
 * 2. Ready (Hazır):
 * Süreç, CPU tarafından çalıştırılmayı bekler.
 * Tüm kaynaklara sahiptir ancak CPU'ya erişimi yoktur.
 *
 * 3. Running (Çalışıyor):
 * Süreç, CPU tarafından çalıştırılmaktadır.
 * İşlemci zamanını kullanarak görevini yerine getirir.
 *
 * 4. Waiting (Bekliyor):
 * Süreç, bir olayın gerçekleşmesini bekler
 * (örneğin, I/O işlemi tamamlanması).
 * CPU'dan ayrılır ve diğer süreçlerin çalışmasına izin verir.
 *
 * 5. Terminated (Sonlandırıldı):
 * Süreç, görevini tamamladıktan sonra
 * işletim sistemi tarafından sonlandırılır.
 * Bellek ve diğer kaynaklar serbest bırakılır.
 *
 *
 */

/*
========================================================
Soru 15: Buddy (Eşiz) Bellek Yönetimi
========================================================

Toplam boyutu 1024 KB (1 MB) olan bir bellekte
Buddy (Eşiz) bellek yönetim sistemi kullanılmaktadır.

Başlangıç durumu:
- Bellek tamamen boştur.
- Tek parça halinde 1024 KB olarak kabul edilir.

--------------------------------------------------------
Sisteme Gelen Süreçler:
--------------------------------------------------------

A Süreci: 70 KB
B Süreci: 350 KB
C Süreci: 80 KB

--------------------------------------------------------
İstenenler:
--------------------------------------------------------

- Her süreç için,
  bellekte yapılan bölünme (splitting) adımlarını
  sırasıyla gösteriniz.

- Buddy sisteminde kullanılan:
  * 2'nin kuvveti olan bloklara yuvarlama
  * Uygun blok bulunana kadar yapılan bölmeler
  açıkça belirtilmelidir.

- Her sürecin hangi boyuttaki bloğa
  yerleştirildiğini gösteriniz.

- Tüm süreçler yerleştirildikten sonra,
  belleğin SON DURUMUNU:
  * Blok boyutları
  * Dolu / Boş durumları
  ile birlikte çizerek veya şematik olarak gösteriniz.

Not:
- İç parçalanma (Internal Fragmentation)
  oluşup oluşmadığını belirtiniz.
- Buddy sisteminin neden dış parçalanmayı
  engellediğini kısaca açıklayınız.

*/

/* Buddy Bellek Yönetimi:
 * 1MB başlangıç bloğu varsa 1 MB = 1024 KB'tır
 * Burada yerleşecek süreç şuna tabii tutulur 2'nin kuvveti olan bloklara yuvarlama
 * 1. A Süreci: 70 KB
 * 64 < 70KB < 128 → 128 KB bloğa yerleştirilir.
 * 2. B Süreci: 350 KB
 * 256 < 350KB < 512 → 512 KB bloğa yerleştirilir.
 * 3. C Süreci: 80 KB
 * 64 < 80KB < 128 → 128 KB bloğa yerleştirilir.
 * Belleğin Son Durumu:
 * | 128 KB (Dolu - A Süreci) | 128 KB (Dolu - C Süreci) | 256 KB (BOŞ) | 512 KB (Dolu - B Süreci) |
 *
 * Parçalanma Analizi:
 * İç Parçalanma: Evet, A süreci için 58 KB, B süreci için 162 KB ve C süreci için 48 KB iç parçalanma oluşmuştur.
 * Dış Parçalanma: Buddy sistemi, bellek bloklarını ikili olarak bölerek yönetir, dış parçalanmayı azaltmaya
 * yöneliktir. Bu sayede, boş bloklar birleşerek daha büyük bloklar oluşturabilir ve böylece
 * bellek israfı minimize edilir.
 *
 */