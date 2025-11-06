# İşletim Sistemleri (Operating Systems) Alıştırmaları

Bu depo, İşletim Sistemleri dersi kapsamında C dilinde yazdığım temel alıştırmaları içermektedir.

Proje, işletim sistemlerinin üç ana temel konseptini kapsayan üç ana klasöre ayrılmıştır:

1.  📁 **Process**
    * `fork()`, `getpid()`, `getppid()` ve `waitpid()` gibi temel süreç (process) yönetimi komutları.
    * `fork()` çağrısının bellek izolasyonu (stack ve heap) üzerindeki etkilerini gösteren örnekler.

2.  📁 **ProcessMessages (IPC)**
    * Prosesler Arası Haberleşme (IPC) mekanizmaları.
    * **Paylaşımlı Bellek (Shared Memory - shm):** `shmget`, `shmat`, `shmdt` kullanarak proseslerin aynı bellek alanını paylaşması.
    * **Mesaj Kuyrukları (Message Queues - msg):** `msgget`, `msgsnd`, `msgrcv` kullanarak proseslerin birbirine güvenli bir şekilde mesaj göndermesi.

3.  📁 **Threadler (Pthreads)**
    * `pthread_create`, `pthread_join` kullanarak iplik (thread) oluşturma ve yönetme.
    * Global değişkenler üzerinde "Yarış Durumu" (Race Condition) ve senkronizasyon sorunlarını gösteren örnekler.

## Derleme

Tüm proje, ana dizindeki `CMakeLists.txt` dosyası kullanılarak CLion veya komut satırı (cmake/make) üzerinden derlenebilir. Her `.c` dosyası, kendi adıyla (veya `CMakeLists.txt` içinde belirtilen adla) çalıştırılabilir bir program olarak derlenir.