#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

sem_t access, reader, order;
int data = 0, nreaders = 0;

void *Reader(void *arg){
    int index = (int) arg;
    sem_wait(&order);
    sem_wait(&reader);
    if (nreaders == 0)
      sem_wait(&access);
    nreaders++;
    sem_post(&order);
    sem_post(&reader);

    printf("Reader - %d read %d\n", index, data);
    sleep(1);

    sem_wait(&reader);
    nreaders--;
    if (nreaders == 0)
      sem_post(&access);
    sem_post(&reader);
}

void *Writer(void *arg){
  int index = (int) arg;
  sem_wait(&order);
  sem_wait(&access);
  sem_post(&order);

  data++;
  printf("Writer - %d wrote %d\n", index, data);
  sleep(1);

  sem_post(&access);
}

int main() {
  int i,b; 
  pthread_t readers[5],writers[5];
  sem_init(&access, 0, 1);
  sem_init(&reader, 0, 1);
  sem_init(&order, 0, 1);

  // Create both reader and writer threads
  for (i = 0; i <= 2; i++) {
    pthread_create(&writers[i], NULL, Writer, (void *) i);
    pthread_create(&readers[i], NULL, Reader, (void *) i);
  }
  // Join the created threads
  for (i=0 ; i <= 2 ; i++) {
    pthread_join(writers[i], NULL);
    pthread_join(readers[i], NULL);
  }
  return 0;
}
