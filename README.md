# OperatingSystems 🖥️

**OperatingSystems** — Temel işletim sistemi kavramlarını C ve Assembly ile örnekleyen bir eğitim/proje deposudur. Bu projede klasik senkronizasyon problemleri, işlem & bellek yönetimi, dosya sistemi, mesajlaşma ve çoklu iş parçacığı (thread) temaları üzerinde örnekler ve uygulamalar bulunur. 

## 📄 Proje Hakkında

Bu depo, işletim sistemleri derslerinde karşılaşılan temel kavram ve algoritmaların **C** ve kısmen **Assembly** ile somutlaştırılmış implementasyonlarını içerir. Amaç, klasik teorik konuları pratik kodlarla anlamayı kolaylaştırmaktır. 

Projede şu temalar işlenir:

- Senkronizasyon problemleri (ClassicSynchronizationProblems)  
- Kritik Bölge & Mutual Exclusion (CriticalSection)  
- Dosya Sistemi örnekleri (FileSystem)  
- Bellek yönetimi (Memory)  
- İşlem & Thread yönetimi (Process, Threadler)  
- Mesajlaşma / IPC mekanizmaları (ProcessMessages)  
- Vize Örnekleri (Vize)  
- CMake yapılandırması (.gitignore, CMakeLists.txt)

---

## 📂 Dosya / Modül Açıklamaları

Aşağıda depodaki ana klasör ve dosyaların kısa açıklamaları yer almaktadır: 


| Klasör / Dosya | Açıklama |
|----------------|----------|
| `ClassicSynchronizationProblems/` | Klasik senkronizasyon problemlerinin örnekleri (örneğin Üretici-Tüketici vb.) |
| `CriticalSection/` | Kritik bölge ve mutual exclusion (çoklu thread’lerde güvenli erişim) örnekleri |
| `FileSystem/` | Basit dosya sistemi benzetimleri ve örnek kodlar |
| `Memory/` | Bellek tahsisi / yönetimi ile ilgili örnekler |
| `Process/` | İşlem kontrolü, fork/exec benzeri yapılar |
| `ProcessMessages/` | Process’ler arası mesajlaşma / dispatcher örnekleri |
| `Threadler/` | Thread oluşturma, yönetme ve eş zamanlılık örnekleri |
| `Vize/` | Değerlendirme/test programları ve görselleştirme araçları |
| `CMakeLists.txt` | Projenin CMake yapılandırma dosyası |
| `.gitignore` | Git için yoksayılan dosya tanımları |

---

## ⚙️ Kurulum & Çalıştırma

Bu proje **CMake** kullanılarak derlenebilir.

### Gereksinimler

- C/C++ derleyicisi (GCC/Clang/MSVC)
- CMake
- Make veya benzeri build aracı

### Adımlar

```bash
# Depoyu klonla
git clone https://github.com/MelihKotman/OperatingSystems.git
cd OperatingSystems

# Build klasörü oluştur ve derle
mkdir build
cd build
cmake ..
make

# Örnek bir modülü çalıştır
./ClassicSynchronizationProblems/example
