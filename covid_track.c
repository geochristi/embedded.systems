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

void delete_close_contacts(close_contact *close_cont) {
    long i = close_cont->head;
    struct timeval t1;
    gettimeofday(&t1, NULL);
    while(i != close_cont->tail){
        //if timestamp of close contact is larger than 14 days then delete ...... HERE 8 MINUTES
        if (((t1.tv_usec - close_cont->contact[close_cont->head].t.tv_usec)/1.0e6 + t1.tv_sec - close_cont->contact[close_cont->head].t.tv_sec) < 480) {
            break; //if the first one is less than 14 days then the next one is for sure less than that
        } else {
            printf("It's been 8 minutes mac %d SALUT AND GOODBYE \n", close_cont->contact[close_cont->head].mac);

            closeContactDel(close_cont);
            i++; //check the next contact
            if (i > CLOSE_CONTACTS){
                i = 0;
            }
        }
    }
}


void save_close_contact(queue *addr, close_contact *close_cont, long i) {
    
    pthread_mutex_lock(close_cont->mut);
    closeContactAdd(close_cont, addr, i);
    printf("mac #%d is a close contact to me \n", close_cont->contact[close_cont->tail-1].mac);  //-1 gt auxanoume thn oura
    pthread_mutex_unlock(close_cont->mut);
    

}

void find_close_contacts(queue *addr, close_contact *close_cont) {
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
                save_close_contact(addr, close_cont, i);
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
    if ((( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec) > 40){  //afinw perithwrio .. kanonika thelei 20min
        queueDel(addr);
        printf("Diagrapsame gt einai panw apo 20 sec edw %f\n",( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec);
    }
}

//find mac address.. fantazomai me bluetooth kai tha einai 48bit
int get_mac(){  
    int mac = (rand()%(60-1+1))+1;
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

    sleep(1);

    if (distance < MIN_DIST) {

        find_mac(addr);
    }


}

int main(void) {

    struct timeval delete_cont, count;

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

    gettimeofday(&delete_cont, NULL);
    while (1){//counter < 1000){   //auto thewritika den tha stamataei
        //pthread_create(&tid, NULL, timer, addresses);
        
        //10 seconds delay and search for mac addresses near you
        timer(addresses);

        //find if a contact is a close one
        find_close_contacts(addresses,closeContacts);

        //counter++; // kai auto theoritika den xreiazetai
        
        //gettimeofday(&count, NULL);
        //delete close contacts after 14 days (3 minutes here)
        // if( ((count.tv_usec-closeContacts->contact[closeContacts->head].t.tv_usec)/1.0e6 + count.tv_sec-closeContacts->contact[closeContacts->head].t.tv_sec) > 180) {
        //     printf("deleting close contact's mac #%d because it's been a while \n", closeContacts->contact[closeContacts->tail].mac);
        //     closeContactDel(closeContacts);
        // }

        delete_close_contacts(closeContacts);
    }
    
    queueDelete(addresses);
    closeContactDelete(closeContacts);
    return 0;
}
