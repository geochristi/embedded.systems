//  // tha eprepe na exei include?
// #define QUEUESIZE 20


#define QUEUESIZE 400000
#define CLOSE_CONTACTS 1200

typedef struct {
  //int i;
  struct timeval t;
  //int mac; // must be 48bit
  //unit64_t macaddress: 48;
  unsigned long long mac : 48;

} contact;


typedef struct {
  int buf[CLOSE_CONTACTS];
  long head, tail;
  int full, empty, mac;
  struct timeval t;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
  contact contact[CLOSE_CONTACTS];  //
} close_contact;


typedef struct {
  int buf[QUEUESIZE];
  long head, tail;
  int full, empty;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
  contact contact[QUEUESIZE];  //
} queue;

struct together{
  queue *addresses;
  close_contact *contact;
};

queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, unsigned long long mac);//, new_contact new);
void queueDel (queue *q);

close_contact *closeContactInit(void);
void closeContactDelete(close_contact *cont);
void closeContactAdd(close_contact *cont, queue *addr, int i);
void closeContactDel (close_contact *cont);
void closeContactAddThread(void *arg, long i);
