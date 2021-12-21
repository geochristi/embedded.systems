//  // tha eprepe na exei include?
// #define QUEUESIZE 20


#define QUEUESIZE 40
#define CLOSE_CONTACTS 1200
// typedef struct {
//   struct timeval t;
//   int mac;
// } close_contact;
typedef struct {
  int i;
  struct timeval t;
  int mac; // must be 48bit
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


queue *queueInit (void);
void queueDelete (queue *q);
void queueAdd (queue *q, int mac);//, new_contact new);
void queueDel (queue *q);

close_contact *closeContactInit(void);
void closeContactDelete(close_contact *cont);
void closeContactAdd(close_contact *cont, queue *addr, int i);
void closeContactDel (close_contact *cont);

// void *producer (void *args);
// void *consumer (void *args);


// typedef struct {
//   void * (*work)(void *);
//   void * arg;
// } workFunction;

// struct queue{
//   int buf[QUEUESIZE];
//   long head, tail;
//   int full, empty;
//   pthread_mutex_t *mut;
//   pthread_cond_t *notFull, *notEmpty;
//   workFunction work[QUEUESIZE];  //
// } ;

// struct address{
//     unsigned long long macadress : 48;  //gt :? 
//                                         // episis einai 48?
//     //int add;
//     double duration_start;
//     double duration_finish;
// } ;


// struct queue *queueInit (void);
// void queueDelete (struct queue *q);
// void queueAdd (struct queue *q, int in);
// void queueDel (struct queue *q, int *out);



// struct address *BTnearMe(int i);  //macaddress BTnearMe()
// bool testCOVID(); 
// void upladContacts (int* macaddress);
// void timer(struct queue *q);
// void search(struct queue *q);
// void check_contacts();

