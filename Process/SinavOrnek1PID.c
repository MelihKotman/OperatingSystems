//
// Created by Melih Yiğit Kotman
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char **argv) {

    // 'x' değişkeni 'main' fonksiyonu kapsamında (scope) tanımlanır.
    // DİKKAT: 'x'e bir ilk değer ATANMADI. İçinde "çöp" (garbage) bir değer var.
    int x;

    // p0, p1, p3 HİÇ KULLANILMIYOR.
    pid_t p0,p1,p2,p3;

    // Ebeveyn (P1) ve Çocuk (C1) süreçleri oluşturulur.
    p2 = fork();

    if (p2 == 0) {

        // --- SADECE ÇOCUK SÜREÇ (C1) ÇALIŞIR ---

        // C1, 'main' kapsamında tanımlanan 'x'in kendi kopyasına 10 atar.
        x = 10;

        // !!! KRİTİK NOKTA !!!
        // 'y' değişkeni, 'if' bloğunun SADECE İÇİNDE tanımlanır.
        // 'y'nin "kapsamı" bu 'if' bloğu ile sınırlıdır.
        // Bu blok bittiğinde 'y' değişkeni yok olur.
        int y = 20;

        // Bu satır GEÇERLİDİR, çünkü 'x' ve 'y' bu kapsamda tanınır.
        // ÇIKTI (Örn): "PID = 8001    x = 10    y = 20"
        printf("PID = %d\t x = %d\t y = %d\n",getpid(),x,y);
    }
    else {

        // --- SADECE EBEVEYN SÜREÇ (P1) ÇALIŞIR ---

        // !!! DERLEME HATASI BURADA OLUŞUR !!!
        // Ebeveyn (P1), 'else' bloğundadır.
        // 1. 'x': 'main' kapsamında olduğu için P1 tarafından görülebilir.
        // (Ama değeri 'çöp'tür, çünkü hiç atanmadı).
        //
        // 2. 'y': Bu kapsamda ('else' bloğu) 'y' adında bir değişken
        // HİÇ TANIMLANMAMIŞTIR. 'y', 'if' bloğunun yerel (local)
        // değişkenidir ve buradan erişilemez.

        // Derleyici (compiler) bu satırda "error: 'y' undeclared"
        // (hata: 'y' tanımlanmamış) diyecektir.
        printf("PID = %d\t x = %d\t y = %d\n",getpid(),x,y);
    }
}

/*
 * ====================================================================
 * 🚀 KODUN ÖZETİ (VE HATA AÇIKLAMASI)
 * ====================================================================
 *
 * Bu program, 'fork()' bellek kurallarından önce temel C dili
 * "Kapsam" (Scope) kuralı nedeniyle BAŞARISIZ OLUR.
 *
 * HATA:
 * 1. 'int y = 20;' değişkeni, 'if (p2 == 0)' bloğunun içinde
 * (yerel olarak) tanımlanmıştır.
 * 2. C dilinde, bir blok (süslü parantez {...}) içinde tanımlanan
 * değişkenlere, o bloğun dışından (örneğin 'else' bloğundan)
 * erişilemez.
 * 3. Ebeveyn süreç, 'else' bloğundaki 'printf' komutuyla 'y'
 * değişkenine erişmeye çalıştığında, derleyici 'y' adında bir
 * değişkenin o kapsamda var olmadığını belirten bir
 * "Tanımsız Değişken" (Undeclared Identifier) hatası verir.
 *
 * Sonuç: Program derlenmez (compile olmaz). Bu nedenle A, B ve C
 * şıkları (programın çalıştığını varsayan) yanlıştır.
 *
 * İKİNCİL HATA (Derleme Hatası Olmasaydı):
 * Ebeveyn sürecin ('else' bloğu) 'x' değişkeni hiç ilklendirilmemiştir
 * (uninitialized). Ebeveyn 'printf'e ulaşabilseydi bile, 'x' için
 * ekrana '0' değil, rastgele bir "çöp" (garbage) değer basardı.
 * ====================================================================
 */
/* Ne çıkar?
 *
 * A) PID = 123 x = 0 y = 20
 *    PID = 124 x = 10 y = 20
 *
 * B) PID = 123 x = 10 y = 20
 *    PID = 124 x = 10 y = 20
 *
 * C) PID = 123 x = 0 y = 0
 *    PID = 123 x = 10 y = 20
 *
 * D) HATA(Y'ye ulaşamayız denmiş)
 */