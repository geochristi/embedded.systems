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

void save_close_contact(queue *addr) {
    
}

void find_close_contacts(queue *addr) {
    long i = addr->tail;
    //int flag = 0;
    float timestamp;
    //printf("i %d and head %d\n ",i,addr->head);
    
    while(i != addr->head){
        //printf("i %d",i);

        if (addr->contact[addr->head].mac == addr->contact[i].mac){
         
            timestamp = (addr->contact[i].t.tv_usec - addr->contact[addr->head].t.tv_usec)/1.0e6 + (addr->contact[i].t.tv_sec - addr->contact[addr->head].t.tv_sec);
            printf("timestamp %fsec of mac %i\n", timestamp, addr->contact[addr->head].mac);
            if (timestamp>20) {
                printf("more than 20s %f\n",timestamp);
                queueDel(addr);
            } else if (timestamp > 4) {
                printf("edw prepei na swsoume se close contacts kai na fugei to mac %i\n",addr->contact[i].mac);
                save_close_contact(addr);
                queueDel(addr);

            }
            break;
        } else {
            i--;
            if (i < 0){
                i = QUEUESIZE;
            }

            //printf("addr->head %d and addr->tail %d and i %d\n", addr->head,addr->tail,i);
        }
    }
    struct timeval t;
    gettimeofday(&t, NULL);
    //printf("time passed from head is %f\n", ( t.tv_usec-addr->contact[addr->head].t.tv_usec)/1.0e6 + t.tv_sec-addr->contact[addr->head].t.tv_sec);

    //delete contact if it stays on the queue for more than 20 minutes
    if ((( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec) > 22){  //afinw perithwrio .. kanonika thelei 20
        queueDel(addr);
        printf("Diagrapsame gt einai panw apo 20 sec edw %f\n",( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec);
    }
}

//find mac address.. fantazomai me bluetooth kai tha einai 48bit
int get_mac(){  
    int mac = (rand()%(70-1+1))+1;
    return mac;
}

void *find_mac(void *addr){
    int mac;
    mac = get_mac();

    //adds a cell in the queue
    queue *addresses;
    addresses = (queue *)addr;
    
    pthread_mutex_lock(addresses->mut);
    queueAdd(addresses,mac);
    pthread_mutex_unlock(addresses->mut);

}

//ten seconds delay
void *timer(void *addr){
    int distance = rand()%10;
    //printf("distance %i\n", distance);
    // struct timeval t1;

    // gettimeofday(&t1, NULL);
    // printf(" %f s\n",t1.tv_usec/1.0e6);
    sleep(1);

    if (distance < MIN_DIST) {
        //pthread_create(&tid, NULL, search, addr);
        //printf("created thread %zu\n", tid);
        find_mac(addr);
    }


}

int main(void) {

    queue *addresses;  
    close_contact *closeContacts;
    
    addresses = queueInit();   // initialize queue
    if (addresses ==  NULL) {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }

    closeContacts = closeContactInit();
    if (closeContacts == NULL) {
        fprintf(stderr, "close Contacts Init failed\n");
        exit(1);
    }
    
    int counter = 0;  // kai auto theoritika den xreiazetai
    //pthread_t tid;
    while (1){//counter < 1000){   //auto thewritika den tha stamataei
        //pthread_create(&tid, NULL, timer, addresses);
        timer(addresses);
        //printf("created thread %zu \n", tid);
        
        //every 30-40 seconds (enough?) we need to check our contacts and add on close contacts
        //if (counter > 10){
            find_close_contacts(addresses);
        //}
        
        //counter++; // kai auto theoritika den xreiazetai
        
    }
    
    queueDelete(addresses);
    closeContactDelete(closeContacts);
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
