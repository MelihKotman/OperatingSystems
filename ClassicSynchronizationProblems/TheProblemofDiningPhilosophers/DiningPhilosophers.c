//
// Created by Melih Yiğit Kotman on 29.12.2025.
//
/*
 * ====================================================================
 * 🍝 FİLOZOFLARI DOYURMA PROBLEMİ (DINING PHILOSOPHERS)
 * ====================================================================
 *
 * Senaryo:
 * - 5 Filozof, 5 Çatal.
 * - Yemek yemek için 2 çatala (Sağ ve Sol) ihtiyaç var.
 * - Aynı anda sadece 2 filozof yemek yiyebilir (çatal kısıtı).
 *
 * Amaç:
 * Kilitlenme (Deadlock) ve Açlık (Starvation) olmadan tüm filozofların
 * hem düşünüp hem de yemek yiyebilmesini sağlamak.
 *
 * Çözüm (Asimetrik Yaklaşım):
 * - Tek numaralı filozoflar önce SOL, sonra SAĞ çatalı alır.
 * - Çift numaralı filozoflar önce SAĞ, sonra SOL çatalı alır.
 * - Bu sayede dairesel bekleme (circular wait) kırılır ve Deadlock önlenir.
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5 // Filozof sayısı

// --- GLOBAL DEĞİŞKENLER ---
sem_t catal[N]; // Her çatal için bir semafor (Binary Semaphore / Mutex)
pthread_t filozoflar[N];
int filozof_id[N]; // Filozofların ID'lerini tutmak için

// --- FİLOZOFLARIN YAŞAM DÖNGÜSÜ ---
void *filozof_yasam(void *num) {
    int i = *(int *)num; // Filozofun ID'si (0, 1, 2, 3, 4)

    // Sonsuz döngü (Filozoflar ölene kadar düşünür ve yer)
    // Sınav/Test için 3 tur döndürüyoruz.
    int tur = 0;
    while (tur < 3) {

        // 1. DÜŞÜNME EVRESİ
        printf("Filozof %d: Düşünüyor... 🤔\n", i);
        sleep(1); // Düşünme süresi

        printf("Filozof %d: Acıktı! Çatal arıyor... 🍽️\n", i);

        // 2. ÇATAL ALMA EVRESİ (DEADLOCK ÖNLEYİCİ MANTIK)
        // Eğer filozof ID'si tek ise -> Önce Sol, Sonra Sağ
        // Eğer filozof ID'si çift ise -> Önce Sağ, Sonra Sol

        if (i % 2 != 0) { // Tek numaralı filozof (1, 3)
            // Önce SOL çatalı (i) al
            sem_wait(&catal[i]);
            printf("Filozof %d: SOL çatalı (%d) aldı.\n", i, i);

            // Sonra SAĞ çatalı ((i+1)%N) al
            sem_wait(&catal[(i + 1) % N]);
            printf("Filozof %d: SAĞ çatalı (%d) aldı.\n", i, (i + 1) % N);
        }
        else { // Çift numaralı filozof (0, 2, 4)
            // Önce SAĞ çatalı ((i+1)%N) al
            sem_wait(&catal[(i + 1) % N]);
            printf("Filozof %d: SAĞ çatalı (%d) aldı.\n", i, (i + 1) % N);

            // Sonra SOL çatalı (i) al
            sem_wait(&catal[i]);
            printf("Filozof %d: SOL çatalı (%d) aldı.\n", i, i);
        }

        // 3. YEME EVRESİ (KRİTİK BÖLGE)
        printf("Filozof %d: YEMEK YİYOR! 🍝\n", i);
        sleep(2); // Yemek yeme süresi

        // 4. ÇATAL BIRAKMA EVRESİ
        // Sırayla her iki çatalı da bırak (Sıra önemli değil)
        sem_post(&catal[i]);
        sem_post(&catal[(i + 1) % N]);

        printf("Filozof %d: Çatalları bıraktı ve doydu.\n", i);

        tur++;
    }
    return NULL;
}

int main() {
    int i;

    // 1. Semaforları Başlat (Her çatal bir Mutex gibidir)
    for (i = 0; i < N; i++) {
        sem_init(&catal[i], 0, 1); // 1 = Çatal masada (Boş)
    }

    // 2. Filozofları (Thread) Oluştur
    for (i = 0; i < N; i++) {
        filozof_id[i] = i; // ID ataması
        pthread_create(&filozoflar[i], NULL, filozof_yasam, &filozof_id[i]);
    }

    // 3. Filozofların Doymasını Bekle
    for (i = 0; i < N; i++) {
        pthread_join(filozoflar[i], NULL);
    }

    // 4. Temizlik
    for (i = 0; i < N; i++) {
        sem_destroy(&catal[i]);
    }

    printf("\nTüm filozoflar doydu, program bitti.\n");
    return 0;
}

/*
 * ====================================================================
 * 📝 KODUN ÇALIŞMA MANTIĞI VE ÖZETİ
 * ====================================================================
 *
 * 1. Çatallar ve Semaforlar:
 * Masa yuvarlak olduğu için her filozof 'i' ile '(i+1)%5' numaralı çatalları
 * paylaşır. Her çatal bir 'semafor' ile temsil edilir.
 * - sem_wait(&catal[x]): Çatalı al (Eğer başkası aldıysa bekle).
 * - sem_post(&catal[x]): Çatalı bırak.
 *
 * 2. Deadlock Problemi:
 * Eğer kodda herkes "Önce SOL çatalı al" deseydi:
 * - Herkes aynı anda solundaki çatalı alırdı.
 * - Herkesin elinde 1 çatal olurdu ama sağdaki çatal dolu olduğu için
 * kimse yemek yiyemez ve sonsuza kadar beklerdi (Deadlock).
 *
 * 3. Asimetrik Çözüm (Kodun Kalbi):
 * Kodda 'if (i % 2 != 0)' bloğu ile filozoflar iki gruba ayrıldı:
 * - Tekliler (1, 3) önce Sol'a saldırır.
 * - Çiftler (0, 2, 4) önce Sağ'a saldırır.
 *
 * Bu sayede, örneğin Filozof 0 ve Filozof 1 arasındaki çatal (Çatal 1) için
 * bir rekabet (Race) oluşur. Biri alır, diğeri bekler. Bekleyen kişi
 * eline hiç çatal almadığı için sistemi kilitlemez. Böylece en az bir kişi
 * her zaman iki çatalı da alabilir ve sistem akmaya devam eder.
 * ====================================================================
 */