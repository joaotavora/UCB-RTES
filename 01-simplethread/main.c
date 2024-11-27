#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 64

typedef struct
{
    int tid;
} tparams_t;

void* count_locally(void* arg) {
  tparams_t* a = (tparams_t*)arg;
  int sum = 0;
  int tid = a->tid;
  for (int i = 1; i<=tid; ++i) sum += i;
  printf("Thread[%d] sum[1..%d]=%d\n", tid, tid, sum);
  return NULL;
}

int main(int argc, char* argv[]) {
  pthread_t threads[NUM_THREADS];
  tparams_t params[NUM_THREADS];

  for (int i = 0; i<NUM_THREADS; ++i) {
    params[i].tid = i;
    if (pthread_create(&threads[i], NULL, &count_locally, &params[i]) != 0) {
      perror("pthread_create");
      exit(-1);
    }
  }

  for (int i = 0; i<NUM_THREADS; ++i) pthread_join(threads[i], NULL);
  printf("Done!\n");
  return 0;
}
