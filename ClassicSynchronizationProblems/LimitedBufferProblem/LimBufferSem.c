//
// Created by Melih Yiğit Kotman on 3.01.2026.
//
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>

sem_t empty,full,mutex;
int x = 0;

void *producer(void *arg) {
    int ID = (int) arg;
    sem_wait(&empty);
    sem_wait(&mutex);
    x++;
    printf("Üretici %d: Ürün üretti, x = %d\n", ID, x);
    sem_post(&mutex);
    sem_post(&full);
    sleep(2);
    return NULL;
}
void *consumer(void *arg) {
    int ID = (int) arg;
    sem_wait(&full);
    sem_wait(&mutex);
    x--;
    printf("Tüketici %d: Ürün tüketti, x = %d\n",ID, x);
    sem_post(&mutex);
    sem_post(&empty);
    sleep(2);
    return NULL;
}
int main(int argc, char **argv) {
    pthread_t th[20];
    sem_init(&empty, 1, 10);
    sem_init(&full, 1, 0);
    sem_init(&mutex, 1, 1);
    for (int i = 0; i < 20; i++) {
        pthread_create(&th[i],NULL,producer,(void *)i);
        i++;
        pthread_create(&th[i],NULL,consumer,(void *)i);
    }
    for (int i = 0; i < 20; i++) {
        pthread_join(th[i],NULL);
    }
    printf("İşlem bitmiştir... x = %d\n",x);
    return 0;
}