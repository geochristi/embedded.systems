#define QUEUESIZE 100
#define CLOSE_CONTACTS 50


typedef struct {
  struct timeval t;
  unsigned long long mac : 48;
  int position;

} contact;



typedef struct {
  int buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
  contact contact[QUEUESIZE];  
} queue;

struct together{
  queue *addresses;
  queue *contact;
};

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, unsigned long long mac, int pos);
void queueDel (queue *q);

void closeContactAdd(void *arg, long i);
