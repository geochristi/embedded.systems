// COVID Tracking
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "covidTrace.h"

#define MIN_DIST 5
// #define TEN_SECONDS 10
// #define ONE_SECOND 1
// #define ADDRESSES 10

void save_server(close_contact *close_cont, FILE *f){

    int i = close_cont->head;

    f = fopen("close_contacts.bin","ab");
    if (f == NULL) {
        fprintf(stderr, "failed to make file");
        exit(1);
    }
    while(i != close_cont->tail){
        
        printf("mac = %d\n", close_cont->contact[i].mac);
                   
        //prostetei 6 midenika meta to mac kai ta swzei se hex?
        fwrite(&close_cont->contact[i], sizeof(int), 1, f);
        //fwrite(&close_cont->contact[i].t.tv_usec/1.0e6, sizeof(float),1,f);
        //fwrite(&close_cont->contact[i],sizeof(contact),1,f);
                    //fread(&j, sizeof(int), 1, f);
                    //printf("%d and \n", j);
        
        //den xerw an xreiazetai gt ana 14 meres diagrafontai automata  
        closeContactDel(close_cont);
                
        i++;
        if (i > CLOSE_CONTACTS){
            i = 0;
        }
    }


    fclose(f);
}


bool testCOVID(){
    return rand() % 2;
}
void *delete_close_contacts_thread(void *arg){
    close_contact *close_cont;
    close_cont = arg;
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
void save_close_contact_thread(void *arg, long i) {
    struct together *args;
    args = arg;
    pthread_mutex_lock(args->contact->mut);
    closeContactAddThread(args, i);
    //printf("mac #%d is a close contact to me \n", close_cont->contact[close_cont->tail-1].mac);  //-1 gt auxanoume thn oura
    pthread_mutex_unlock(args->contact->mut);
    

}

void save_close_contact(queue *addr, close_contact *close_cont, long i) {
    
    pthread_mutex_lock(close_cont->mut);
    closeContactAdd(close_cont, addr, i);
    //printf("mac #%d is a close contact to me \n", close_cont->contact[close_cont->tail-1].mac);  //-1 gt auxanoume thn oura
    pthread_mutex_unlock(close_cont->mut);
    

}

void *find_close_contacts_thread(void *arg){
    struct together *args;
    args = arg;
    long i = args->addresses->tail;
    //int flag = 0;
    float timestamp;
    //printf("i %d and head %d\n ",i,addr->head);
    //printf("find close contacts pthread ID - %lu\n",pthread_self());
    while(i != args->addresses->head){
        //printf("i %d",i);

        if (args->addresses->contact[args->addresses->head].mac == args->addresses->contact[i].mac){
         
            timestamp = (args->addresses->contact[i].t.tv_usec - args->addresses->contact[args->addresses->head].t.tv_usec)/1.0e6 + (args->addresses->contact[i].t.tv_sec - args->addresses->contact[args->addresses->head].t.tv_sec);
            //printf("timestamp %fsec of mac %i\n", timestamp, addr->contact[addr->head].mac);
            if (timestamp>20) {
                //printf("more than 20s %f\n",timestamp);
                queueDel(args->addresses);
            } else if (timestamp > 4) {
                printf("edw prepei na swsoume se close contacts kai na fugei to mac %d\n",args->addresses->contact[i].mac);
                save_close_contact_thread(args, i);
                queueDel(args->addresses);

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
    if ((( t.tv_usec - args->addresses->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - args->addresses->contact[i].t.tv_sec) > 40){  //afinw perithwrio .. kanonika thelei 20min
        queueDel(args->addresses);
        //printf("Diagrapsame gt einai panw apo 20 sec edw %f\n",( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec);
    }
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
            //printf("timestamp %fsec of mac %i\n", timestamp, addr->contact[addr->head].mac);
            if (timestamp>20) {
                //printf("more than 20s %f\n",timestamp);
                queueDel(addr);
            } else if (timestamp > 4) {
                printf("edw prepei na swsoume se close contacts kai na fugei to mac %d\n",addr->contact[i].mac);
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
        //printf("Diagrapsame gt einai panw apo 20 sec edw %f\n",( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec);
    }
}

//find mac address.. fantazomai me bluetooth kai tha einai 48bit
int get_mac(){  
    int mac = (rand()%(140-1+1))+1;
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
void *find_mac_thread(void *arg){
    int mac;
    mac = get_mac();

    //adds a cell in the queue
    struct together *args;
    args = arg;
    
    pthread_mutex_lock(args->addresses->mut);
    queueAdd(args->addresses,mac);
    pthread_mutex_unlock(args->addresses->mut);

}
//ten seconds delay
void *timer(void *addr){
    int distance = rand()%10;

    sleep(1);

    if (distance < MIN_DIST) {

        find_mac(addr);
    }


}
void *thread_timer(void *arg){
    int distance = rand()%10;
    //printf("timer pthread ID - %lu\n",pthread_self());
    sleep(1);

    if (distance < MIN_DIST) {

        find_mac_thread(arg);
    }

}

int main(void) {

    struct timeval test_time, t1;
    bool test;
    FILE *f;
    queue* addresses;  

    close_contact* closeContacts;
    
    struct together *arg = malloc (sizeof (struct together));    
        


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
    arg->addresses = addresses;
    arg->contact = closeContacts;

    //int counter = 0;  // kai auto theoritika den xreiazetai
    pthread_t timer_thread,find_thread, delete_thread, save_thread;
    int i=0;
    gettimeofday(&test_time, NULL);
    while (1){//counter < 1000){   //auto thewritika den tha stamataei
        //pthread_create(&tid, NULL, timer, addresses);
        
        //10 seconds delay and search for mac addresses near you
        //timer(addresses);
        pthread_create(&timer_thread,NULL,thread_timer, arg);

        //find if a contact is a close one
        //find_close_contacts(addresses,closeContacts);
        pthread_create(&find_thread, NULL, find_close_contacts_thread, arg);
        
        // delete close contacts after 14 days
        //delete_close_contacts(closeContacts);
        pthread_create(&delete_thread, NULL, delete_close_contacts_thread,arg->contact);


       //every 4 hours does a test ....... HERE 6 MINUTES
       gettimeofday(&t1, NULL);
       
        if (((t1.tv_usec - test_time.tv_usec)/1.0e6 + t1.tv_sec - test_time.tv_sec)  > 60){
           test = testCOVID();
           //int j;
           if (test == 0) {
               printf("I AM POSITIVE STAY AWAY\n");
               save_server(arg->contact, f);

               
           } else {
               printf("MY TEST WAS NEGATIVE! LET'S HUG\n");

           }
           gettimeofday(&test_time, NULL);  // initialize again
            
            
            // int num_upl = bin_file_size("out/close_contacts.bin")/sizeof(double);
            // contact* up_contact =(contact*)malloc(num_upl*sizeof(double));

            // f=fopen("close_contacts.bin","rb");
            // unsigned char buffer[10];
            // fread(num_upl, sizeof(contact),num_upl,f);
            // //for(int j = 0; j<10; j++)
            //  //   printf("%u ", buffer[i]); // prints a series of bytes
            // fclose(f);
        
        }
        pthread_join(timer_thread,NULL);
        pthread_join(find_thread,NULL);
        pthread_join(delete_thread, NULL);

    }
    
    queueDelete(addresses);
    closeContactDelete(closeContacts);
    return 0;
}
