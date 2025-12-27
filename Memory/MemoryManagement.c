//
// Created by Melih Yiğit Kotman on 27.12.2025.
//
/*
 * ====================================================================
 * 🧠 BELLEK YÖNETİMİ: MALLOC, FREE VE BRK ÖRNEĞİ
 * ====================================================================
 *
 * Amaç:
 * Bu kod, işletim sisteminin bellek yönetiminde kullanılan temel
 * fonksiyonları (malloc, free, brk) basit bir şekilde simüle eder.
 *
 * Kullanılan Fonksiyonlar:
 * 1. sbrk: Heap alanını büyütmek veya küçültmek için kullanılır.
 * 2. my_malloc: İstenilen boyutta bellek ayırmak için kullanılır.
 * 3. my_free: Ayrılan belleği serbest bırakmak için kullanılır.
 *
 * ====================================================================
 */

#include <stdio.h>
#include <unistd.h>

// Basit malloc fonksiyonu
void *my_malloc(size_t size) {
    void *ptr = sbrk(0); // Mevcut heap sonunu al
    void *request = sbrk(size); // Heap'i büyüt
    if (request == (void*) -1) {
        return NULL; // Hata durumunda NULL dön
    } else {
        return ptr; // Ayrılan alanın başlangıcını dön
    }
}

// Basit free fonksiyonu (Gerçekte daha karmaşıktır)
void my_free(void *ptr) {
    // Bu basit örnekte free işlemi gerçek anlamda belleği iade etmez,
    // sadece işaretçiyi null yapar. Gerçek sistemlerde bellek
    // yeniden kullanılabilir hale getirilir.
    ptr = NULL;
}

int main() {
    int *ptr;

    // Bellek ayırma
    printf("Bellek ayriliyor...\n");
    ptr = (int*) my_malloc(10 * sizeof(int));

    if (ptr == NULL) {
        printf("Bellek ayrilamadi!\n");
        return 1;
    }

    // Belleği kullanma
    for (int i = 0; i < 10; i++) {
        ptr[i] = i * 2;
        printf("%d ", ptr[i]);
    }
    printf("\n");

    // Belleği serbest bırakma
    printf("Bellek serbest birakiliyor...\n");
    my_free(ptr);

    return 0;
}

/*
 * ====================================================================
 * 📝 KODUN ÖZETİ VE ÇALIŞMA MANTIĞI
 * ====================================================================
 *
 * 1. MALLOC (Memory Allocation):
 * - 'my_malloc' fonksiyonu, 'sbrk' sistem çağrısını kullanarak heap
 * alanını büyütür.
 * - 'sbrk(0)' ile mevcut heap sonu alınır.
 * - 'sbrk(size)' ile heap, istenilen boyut kadar büyütülür.
 * - Başarılı olursa, ayrılan alanın başlangıç adresi döndürülür.
 *
 * 2. FREE (Memory Deallocation):
 * - 'my_free' fonksiyonu, ayrılan belleği serbest bırakır.
 * - Bu basit örnekte sadece işaretçi NULL yapılır, ancak gerçek
 * sistemlerde bellek bloğu serbest listesine eklenir.
 *
 * 3. BRK/SBRK (Program Break):
 * - 'sbrk', veri segmentinin sonunu (program break) değiştirir.
 * - 'malloc' ve 'free' gibi fonksiyonlar arka planda bu çağrıları
 * kullanarak bellek yönetimini gerçekleştirir.
 * ====================================================================
 */