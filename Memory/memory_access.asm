section .data
    val db '5', 0xA  ; Veri bölümü. 'val' adında bir değişken tanımlanıyor.
                     ; '5' karakteri ve 0xA (New Line - \n) içeriyor.

section .text
    global _start    ; Programın başlangıç noktasını belirtir.
_start:
    mov eax, 4       ; eax = 4 -> sys_write sistem çağrısı numarası
    mov ebx, 1       ; ebx = 1 -> Dosya tanımlayıcı (File Descriptor). 1 = stdout (ekran)
    mov ecx, val     ; ecx = val -> Yazdırılacak verinin adresi ('5' ve '\n')

    mov edx, 2       ; edx = 2 -> Yazdırılacak bayt sayısı (1 karakter + 1 yeni satır = 2)
    int 0x80         ; Kesme (Interrupt) oluştur. İşletim sistemine "Hadi yap" de.

    mov byte [val], '9' ; [val] adresindeki veriyi '9' karakteri ile değiştir.

    mov eax, 4       ; eax = 4 -> sys_write
    mov ebx, 1       ; ebx = 1 -> stdout
    mov ecx, val     ; ecx = val -> Yazdırılacak veri (Artık '9' oldu)

    mov edx, 2       ; edx = 2 -> 2 bayt yaz
    int 0x80         ; İşletim sistemini çağır

    mov eax, 1       ; eax = 1 -> sys_exit sistem çağrısı numarası
    mov ebx, 0       ; ebx = 0 -> Çıkış kodu (0 = Başarılı, Hata yok)
    int 0x80         ; İşletim sistemini çağır ve programı kapat.

; ======================================================================================
; KOD ÖZETİ: ASSEMBLY SİSTEM ÇAĞRILARI VE BELLEK MANİPÜLASYONU
; ======================================================================================
;
; 1. SİSTEM ÇAĞRILARI (System Calls - int 0x80):
;    - Bu kod, Linux çekirdeği (kernel) ile doğrudan iletişim kurar.
;    - EAX register'ı "hangi işlemi yapacağımızı" söyler (4: sys_write, 1: sys_exit).
;    - Diğer registerlar (EBX, ECX, EDX) fonksiyonun parametreleridir:
;      * EBX: Nereye yazılacak? (1 = stdout/ekran)
;      * ECX: Hangi adresteki veri? (val adresi)
;      * EDX: Kaç bayt? (2 bayt)
;
; 2. DOĞRUDAN BELLEK ERİŞİMİ ([val]):
;    - 'mov byte [val], '9'' satırı kodun en kritik mantığıdır.
;    - Köşeli parantez [], C dilindeki pointer (*) mantığıyla aynıdır (Dereference).
;    - "val etiketinin gösterdiği bellek adresine git ve oraya '9' değerini koy" demektir.
;    - 'byte' ifadesi ise "sadece 1 baytlık alanı değiştir" diyerek işlem boyutunu belirtir.
;
; 3. PROGRAM AKIŞI:
;    - Adım 1: Bellekte tanımlı '5' değerini ekrana bas.
;    - Adım 2: Belleğe müdahale et ve '5'i silip yerine '9' yaz.
;    - Adım 3: Aynı adresi tekrar ekrana bas (Bu sefer ekrana '9' çıkar).
;    - Adım 4: İşletim sistemine "bitti" diyerek çıkış yap.
; ======================================================================================