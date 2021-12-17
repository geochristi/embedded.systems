// COVID Tracking
#include <stdio.h>
#include <stdlib.h>
// #include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "covidTrace.h"
// #include <stdbool.h>

#define MIN_DIST 5
// #define TEN_SECONDS 10
// #define ONE_SECOND 1
// #define ADDRESSES 10

// struct timeval t1, t2;
// float t;
// int addr[TEN_SECONDS];

void *search(void *addr){
    
}
//find mac address.. fantazomai me bluetooth kai tha einai 48bit
int get_mac(){  
    int mac = 2;//(rand()%(6-1+1))+1;
    return mac;
}

void *find_mac(void *addr, pthread_t tid){
    int mac;
    mac = get_mac();
    printf("mac %d\n",mac);
    //pthread_create
    //adds a cell in the queue
    queue *addresses;
    addresses = (queue *)addr;
    
    struct timeval t1;
    
    //dokimi
    // new_contact **new;
    // gettimeofday(&new.t, NULL);
    // new.mac=mac;
    // printf("time  %f\n",new.t.tv_usec/1.0e6);

    //if (mac != addresses->contact->mac) {
        pthread_mutex_lock(addresses->mut);
        queueAdd(addresses,mac);
        pthread_mutex_unlock(addresses->mut);
        //gettimeofday(&addresses->contact[addresses->tail].t, NULL);
        //printf("Time of the %i mac %i is %f s\n",addresses->tail, mac, addresses->contact[addresses->tail].t.tv_usec/1.0e6 );
    //}
}

//ten seconds delay
void *timer(void *addr, pthread_t tid){
    int distance = rand()%10;
            struct timeval t1;

    gettimeofday(&t1, NULL);
    printf(" %f s\n",t1.tv_usec/1.0e6);
    sleep(1);

    if (distance < MIN_DIST) {
        //pthread_create(&tid, NULL, search, addr);
        //printf("created thread %zu\n", tid);
        find_mac(addr, tid);
    }


}

int main(void) {

    queue *addresses;  
  
    addresses = queueInit();   // initialize queue
    if (addresses ==  NULL) {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }
    
    int counter = 0;  // kai auto theoritika den xreiazetai
    pthread_t tid;
    while (counter < 5){   //auto thewritika den tha stamataei
        //pthread_create(&tid, NULL, timer, addresses);
        timer(addresses, tid);
        //printf("created thread %zu \n", tid);
        counter++; // kai auto theoritika den xreiazetai
    }
    
    queueDelete(addresses);
    return 0;
}


// void define_addr(void) {
//     for (int i=0; i<10; i++){
//         if (i < 5) {
//             addr[i] = 1;
//         } else {
//             addr[i] = 2;
//         }
//     }
// }

// int main () {

//     define_addr();

//     bool test;

//     struct queue *fifo;  
  
//     fifo = queueInit ();   // initialize queue
//     if (fifo ==  NULL) {
//         fprintf (stderr, "main: Queue Init failed.\n");
//         exit (1);
//     }


//     int done = 10;
//     while (done) {
   
//         timer(fifo);

//         //as to pros to paron
//         if (done == 5) {
//             pthread_t tid;
//             pthread_create(&tid, NULL, &check_contacts,NULL);
//         }
//         // if (distance <= MIN_DIST) {    //logika an macaddress == traceable
//         //     BTnearMe();
//               // test = testCOVID();
//         // }
//         printf("until ending %d\n", done);
//         --done;
//     }
//     if (test == 1) {
//         printf("ITS POSITIVE STAY AWAY\n");
//     } else {
//         printf("ITS NEGATIVE you can hang out with this person\n");
//     }


//     queueDelete (fifo);
//     return 0;
// }


// //function that waits for 1 sec and then calls the search function
// void timer(struct queue *q){

//     sleep(ONE_SECOND);  //we need 10 seconds
//     search(q);  // search if there is a device near you
// }

// //
// void search(struct queue *q) {  // an brei address prepei na ftiaxnei thread? wste na girnaei to programma sto timer
    
//     int distance=rand()%10;  //make a random number for the distance in the range of [0,20]
//     //printf("distance %f \n", distance);
//     if (distance <= MIN_DIST) {    //logika an macaddress == traceable
        
//         pthread_t tid;
//         pthread_create(&tid, NULL, &BTnearMe, distance);
//         printf("created thread %u \n", tid);
//         //pthread_exit(NULL);
//         //BTnearMe();  //find their mac address

//     }
// }


// struct address *BTnearMe(int dist) {

//     printf("there is a traceable device near you\n");
//     struct address* contact = (struct address*)malloc(sizeof(struct address));

//     contact->macadress = addr[dist];//rand();//%ADDRESSES;      //random
//                                 //kanonika epistrefei gnwsti timi apo sinolo timwn des to
//     //printf("the mac is %llu \n", mac_address->macadress);
//     contact->duration_start = gettimeofday(&t1, NULL); 
//     t = (double)(t1.tv_usec/1.0e6);
//     //sleep(3);
//     printf("time of contact %f\n", t );
    
// }

// void check_contacts (void){

// }



// bool testCOVID(){
//     printf("has this person a negative or a positive test?\n");
//     return 1;
// }

// void upladContacts (int* macaddress) {

// }
