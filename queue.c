#include <pthread.h>
#include <stdlib.h>
#include "covidTrace.h"
#include <stdio.h>
#include <sys/time.h>

queue *queueInit (void)
{
  queue *q;

  q = (queue *)malloc (sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (q->mut, NULL);
  q->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (q->notEmpty, NULL);
	
  return (q);
}

void queueDelete (queue *q)
{
  pthread_mutex_destroy (q->mut);
  free (q->mut);	
  pthread_cond_destroy (q->notFull);
  free (q->notFull);
  pthread_cond_destroy (q->notEmpty);
  free (q->notEmpty);
  free (q);
}

void queueAdd (queue *q, unsigned long long  mac, int pos)//, new_contact new)
{
  //add contact to queue
  q->contact[q->tail].mac = mac;
  gettimeofday(&q->contact[q->tail].t, NULL);
  q->contact[q->tail].position = pos;
  //move queue's tail
  q->tail++;
  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel (queue *q)
{ 
  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;
  return;
}

void closeContactAdd(void *arg, long i){
    struct together *args;
    args = arg;
    args->contact->contact[args->contact->tail] = args->addresses->contact[i];
    args->contact->tail++;
    if (args->contact->tail == CLOSE_CONTACTS)
      args->contact->tail = 0;
    if (args->contact->tail == args->contact->head)
      args->contact->full = 1;
    args->contact->empty = 0;

    return;
}

