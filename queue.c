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

void queueAdd (queue *q, int mac)//, new_contact new)
{
  //add contact to queue
  q->contact[q->tail].mac = mac;
  gettimeofday(&q->contact[q->tail].t, NULL);
  //printf("Time of the %i mac %i is %f s\n",q->tail, mac, q->contact[q->tail].t.tv_usec/1.0e6 + q->contact[q->tail].t.tv_sec );
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


close_contact *closeContactInit(void) {
  close_contact *cont;

  cont = (close_contact *)malloc (sizeof (close_contact));
  if (cont == NULL) return (NULL);

  cont->empty = 1;
  cont->full = 0;
  cont->head = 0;
  cont->tail = 0;
  cont->mut = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
  pthread_mutex_init (cont->mut, NULL);
  cont->notFull = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (cont->notFull, NULL);
  cont->notEmpty = (pthread_cond_t *) malloc (sizeof (pthread_cond_t));
  pthread_cond_init (cont->notEmpty, NULL);
	
  return (cont);
}
void closeContactDelete(close_contact *cont) {
  pthread_mutex_destroy (cont->mut);
  free (cont->mut);	
  pthread_cond_destroy (cont->notFull);
  free (cont->notFull);
  pthread_cond_destroy (cont->notEmpty);
  free (cont->notEmpty);
  free (cont);
}
void closeContactAdd(close_contact *cont) {
  
  
  cont->tail++;
  if (cont->tail == QUEUESIZE)
    cont->tail = 0;
  if (cont->tail == cont->head)
    cont->full = 1;
  cont->empty = 0;

  return;
}