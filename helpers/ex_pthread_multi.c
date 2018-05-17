// gcc pthread.c -o pthread -lpthread

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREADS 10

struct ARG
{
  int argv;
  int id;
};

void *multipleRunner(void *param);

int main(int argc, char *argv[])
{
  pthread_t workers[NUM_THREADS];
  pthread_attr_t attrs[NUM_THREADS];

  if(argc != 2)
  {
    fprintf(stderr, "usage: pthread.out <integer value>\n");
    return -1;
  }

  if(atoi(argv[1]) < 0)
  {
    fprintf(stderr, "%d must be >= 0\n", atoi(argv[1]));
    return -1;
  }

  struct ARG args[NUM_THREADS];
  for(int i = 0; i < NUM_THREADS; i++)
  {
    args[i].argv = atoi(argv[1]);
    args[i].id = i;
    pthread_attr_init(&attrs[i]);
    pthread_create(&workers[i], &attrs[i], multipleRunner, &args[i]);
  }

  for(int i = 0; i < NUM_THREADS; i++)
  {
    pthread_join(workers[i], NULL);
  }

  printf("sum = %u\n", sum);

  return 0;
}

void *multipleRunner(void *param)
{
  struct ARG *arg = (struct ARG *)param;
  int i, upper = arg->argv;
  unsigned int sum = 0;

  for(i = 0; i <= upper; i++)
  {
    sum += i;
  }

  printf("thread %d: %u\n", arg->id, sum);

  pthread_exit(0);
}
