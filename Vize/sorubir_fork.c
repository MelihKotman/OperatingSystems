//
//Created by Melih Kotman
//

#include <stdio.h> //Temel C Fonksiyonları için
#include <unistd.h> //fork'larımız kodlar için
#include <sys/wait.h> //wait

//       P0(ana process)
//     /      \
//    P1      P2
//   / \
// P3  P4
//

int main() {
    //Ana proses'i yazdıralım
    printf("P0(Ana Process) PID = %d\n",getpid()); //P0 ID'leri yazdırsın.
    pid_t p1,p2,p3,p4; //Process isimlerini verdik
    //İlk fork'umuz P1 yapıyoruz
    p1 = fork();
    //P1 kendi çocuklarını fork'laması için bir if kararı aç
    if (p1 == 0) {
        //P1'in iki çocuğu olacak P3 ve P4 şeklinde
        printf("P1 PID(P0'in Cocugu)= %d\t P1'in PPID(P0) = %d\n",getpid(),getppid()); //P1 ID'leri yazdırsın.
        //P1 ilk çocuğu P3'ü forklasın
        p3 = fork();
        //P3'ün sadece kendini çalıştıracağı alan
        if (p3 == 0) {
            printf("P3 PID(P1'in Cocugu) = %d\t P3'in PPID(P1) = %d\n",getpid(),getppid()); //P3 ID'leri yazdırsın.
            return 0; //İş bitince çık *ÖNEMLİ*
        }
        //P1 ikinci çocuğunu P4'ü forklasın
        p4 = fork();
        //P4'ün sadece kendini çalıştıracağı alan
        if (p4 == 0) {
            printf("P4 PID(P1'in Cocugu) = %d\t P4'in PPID(P1) = %d\n",getpid(),getppid()); //P4 ID'leri yazdırsın.
            return 0; //İş bitince çık *ÖNEMLİ*
        }
        //P1 işi bitti ve çocuklarını beklesin
        wait(NULL);wait(NULL);
        return 0; //İş bitince çık *ÖNEMLİ*
    }
    //İkinci fork'umuz olan P2 yapıyoruz
    p2 = fork();
    if (p2 == 0) {
        //Sadece P2'miz olacak
        printf("P2 PID(P0'in Cocugu) = %d\t P2'in PPID(P0) = %d\n",getpid(),getppid()); //P2 ID'leri yazdırsın.
        return 0; //İş bitince çık *ÖNEMLİ*
    }
   wait(NULL);wait(NULL);//P0 işi bitti ve çocuklarını beklesin
    return 0; //İş bitince çık *ÖNEMLİ*
}