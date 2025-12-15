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

