#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
typedef struct Monitor {
  pthread_mutex_t *methodLock;
  pthread_cond_t *barrera;
  int nHebras;
  int count;
  int etapa;
} Monitor;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int nHebras = 5;
int nEtapas = 4;

Monitor m;

void wait(Monitor *m) {
  pthread_mutex_lock(m->methodLock);
  m->count--;
  if (m->count != 0) {
    pthread_cond_wait(m->barrera, m->methodLock);
  } else {
    m->count = m->nHebras;
    m->etapa++;
    pthread_cond_broadcast(m->barrera);
  }
  pthread_mutex_unlock(m->methodLock);
}

void *threadRoutine(void *args) {
  for (int i = 0; i < nEtapas; i++) {
    usleep(rand() % 10000);
    printf("hebra %lu ,entrando etapa %d \n", (uintptr_t)args, m.etapa);
    wait(&m);
    printf("hebra %lu ,saliendo de etapa %d \n",(uintptr_t)args ,m.etapa - 1);
  }
  return NULL;
}
int main(int argc, char** argv) {

  if(argc != 3){
    exit(0);
  }
  nHebras = atoi(argv[1]);
  nEtapas = atoi(argv[2]);

  pthread_t threads[nHebras];

  m.methodLock = &mutex;
  m.barrera = &cond;
  m.nHebras = nHebras;
  m.count = m.nHebras;
  m.etapa = 1;

  srand(time(NULL));
  for (uintptr_t i = 0; i < nHebras; i++) {
    pthread_create(threads + i, NULL, &threadRoutine, (void *)(i+1));
  }
  for (int i = 0; i < nHebras; i++) {
    pthread_join(threads[i], NULL);
  }
}
