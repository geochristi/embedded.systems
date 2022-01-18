// COVID Tracking
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "covidTrace.h"
#include <errno.h>

#define MIN_DIST 5
// #define TEN_SECONDS 10
// #define ONE_SECOND 1
// #define ADDRESSES 10

pthread_mutex_t lock, lock2;// = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_timer, delete_close_wait;
int counter =0, counter2 = 0, delete = 0, save =0;

void save_server(void *arg, FILE *f, FILE *f2){
    close_contact *close_cont;
    close_cont = arg;
    int i = close_cont->head;

    f = fopen("close_contacts.bin","ab");
    if (f == NULL) {
        fprintf(stderr, "failed to make file");
        exit(1);
    }
    f2 = fopen("close_contacts2.bin","ab");
    if (f2 == NULL) {
        fprintf(stderr, "failed to make file");
        exit(1);
    }
    while(i != close_cont->tail){
        
        printf("mac = %llu \n", (unsigned long long)close_cont->contact[i].mac);
        //printf("macadress = %llu\n", close_cont->contact[i].macaddress);

        //prostetei 6 midenika meta to mac kai ta swzei se hex?  epishs an den ta diagrapseis sinexizoun na grafoun
        fwrite(&close_cont->contact[i], sizeof(contact), 1, f); //auto swzei logika kai to timestamp
        //fwrite(&close_cont->contact[i].mac, sizeof(unsigned long long), 1, f2);  // swzei mac se hex kai midenika
        //fwrite(&close_cont->contact[i].t.tv_usec/1.0e6, sizeof(float),1,f);
        //fwrite(&close_cont->contact[i],sizeof(contact),1,f);
                    //fread(&j, sizeof(int), 1, f);
                    //printf("%d and \n", j);
        
        //den xerw an xreiazetai gt ana 14 meres diagrafontai automata  
        pthread_mutex_lock(close_cont->mut);
        while (close_cont->empty)
        {
            printf("queue for close contacts empty \n");
            pthread_cond_wait(close_cont->notEmpty, close_cont->mut);
        }
        
        closeContactDel(close_cont);
        pthread_mutex_unlock(close_cont->mut);
        pthread_cond_signal(close_cont->notFull);
                
        i++;
        if (i > CLOSE_CONTACTS){
            i = 0;
        }
    }


    fclose(f);
    fclose(f2);
    save++;
    printf("I have entered save to server %d times \n",save);
}
bool testCOVID(){
    return rand() % 2;
}
void *delete_close_contacts_thread(void *arg){
    while(1){
        pthread_mutex_lock(&lock2);
        printf("Waiting for 14 days to delete my friends...\n");
        pthread_cond_wait(&delete_close_wait, &lock2);
        close_contact *close_cont;
        close_cont = arg;
        long i = close_cont->head;
        struct timeval t1;
        gettimeofday(&t1, NULL);
        while(i != close_cont->tail){
            //if timestamp of close contact is larger than 14 days then delete ...... HERE 8 MINUTES
            if (((t1.tv_usec - close_cont->contact[close_cont->head].t.tv_usec)/1.0e6 + t1.tv_sec - close_cont->contact[close_cont->head].t.tv_sec) < 480) {
                    delete++;
                    printf("I have entered delete contacts %d times but i didnt have to\n", delete);    
                break; //if the first one is less than 14 days then the next one is for sure less than that
            } else {
                printf("It's been 8 minutes mac %llu SALUT AND GOODBYE \n", (unsigned long long)close_cont->contact[close_cont->head].mac);
                pthread_mutex_lock(close_cont->mut);
                while(close_cont->empty){
                    printf("queue for close contacts empty \n");
                    pthread_cond_wait(close_cont->notEmpty,close_cont->mut);
                }
                closeContactDel(close_cont);
                pthread_mutex_unlock(close_cont->mut);
                pthread_cond_signal(close_cont->notFull);

                i++; //check the next contact
                if (i > CLOSE_CONTACTS){
                    i = 0;
                }
            }
       

        }

        pthread_mutex_unlock(&lock2);
    }

}
/*
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

            pthread_mutex_lock(close_cont->mut);
            while(close_cont->empty){
                printf("queue for close contacts empty \n");
                pthread_cond_wait(close_cont->notEmpty,close_cont->mut);
            }
            closeContactDel(close_cont);
            pthread_mutex_unlock(close_cont->mut);
            pthread_cond_signal(close_cont->notFull);
            i++; //check the next contact
            if (i > CLOSE_CONTACTS){
                i = 0;
            }
        }
    }
}
*/
void save_close_contact_thread(void *arg, long i) {
    struct together *args;
    args = arg;
    pthread_mutex_lock(args->contact->mut);
    while(args->contact->full) {
        printf("queue for close contacts full\n");
        pthread_cond_wait(args->contact->notFull, args->contact->mut);
    }
    closeContactAddThread(args, i);
    //printf("mac #%d is a close contact to me \n", close_cont->contact[close_cont->tail-1].mac);  //-1 gt auxanoume thn oura
    pthread_mutex_unlock(args->contact->mut);
    pthread_cond_signal(args->contact->notEmpty);
    

}
/*
void save_close_contact(queue *addr, close_contact *close_cont, long i) {
    
    pthread_mutex_lock(close_cont->mut);
    while(close_cont->full) {
        printf("queue for close contacts full \n");
        pthread_cond_wait(close_cont->notFull,close_cont->mut);
    }
    closeContactAdd(close_cont, addr, i);
    //printf("mac #%d is a close contact to me \n", close_cont->contact[close_cont->tail-1].mac);  //-1 gt auxanoume thn oura
    pthread_mutex_unlock(close_cont->mut);
    pthread_cond_signal(close_cont->notEmpty);
    

}
*/
void *find_close_contacts_thread(void *arg){
    while(1) {
        pthread_mutex_lock(&lock);
        //while(counter < 8) {
        printf("Waiting for (8 minutes) ...\n");
        pthread_cond_wait(&wait_timer, &lock);
        //}            
        //printf("searching for close contacts\n");
        struct together *args;
        args = arg;
        long i = args->addresses->tail;
        //int flag = 0;
        float timestamp;
        struct timeval t;
        gettimeofday(&t, NULL);
        //printf("i %d and head %d\n ",i,addr->head);
        //printf("find close contacts pthread ID - %lu\n",pthread_self());
        while(args->addresses->empty){
            printf("queue for addresses empty \n");
            pthread_cond_wait(args->addresses->notEmpty, args->addresses->mut);
        }
        while(i != args->addresses->head){
            //printf("i %d\n",i);

            if (args->addresses->contact[args->addresses->head].mac == args->addresses->contact[i].mac){
            
                timestamp = (args->addresses->contact[i].t.tv_usec - args->addresses->contact[args->addresses->head].t.tv_usec)/1.0e6 + (args->addresses->contact[i].t.tv_sec - args->addresses->contact[args->addresses->head].t.tv_sec);
                printf("timestamp %fsec of mac %llu\n", timestamp, (unsigned long long)args->addresses->contact[args->addresses->head].mac);
                if (timestamp>100) {
                    printf("more than 100 %f\n",timestamp);
                    pthread_mutex_lock(args->addresses->mut);
                    // while(args->addresses->empty){
                    //     printf("queue for addresses empty \n");
                    //     pthread_cond_wait(args->addresses->notEmpty, args->addresses->mut);
                    // }
                    queueDel(args->addresses);
                    pthread_mutex_unlock(args->addresses->mut);
                    pthread_cond_signal(args->addresses->notFull);
                } else if (timestamp > 4) {
                    printf("edw prepei na swsoume se close contacts kai na fugei to mac %llu\n",(unsigned long long)args->addresses->contact[i].mac);
                    save_close_contact_thread(args, i);
                    pthread_mutex_lock(args->addresses->mut);
                    // while(args->addresses->empty){
                    //     printf("queue for addresses empty \n");

                    //     pthread_cond_wait(args->addresses->notEmpty, args->addresses->mut);
                    // }
                    queueDel(args->addresses);
                    pthread_mutex_unlock(args->addresses->mut);
                    pthread_cond_signal(args->addresses->notFull);
                    
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
        // struct timeval t;
        // gettimeofday(&t, NULL);
        //printf("time passed from head is %f\n", ( t.tv_usec-addr->contact[addr->head].t.tv_usec)/1.0e6 + t.tv_sec-addr->contact[addr->head].t.tv_sec);

        //delete contact if it stays on the queue for more than 20 minutes    (head of the queue)
        if ((( t.tv_usec - args->addresses->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - args->addresses->contact[i].t.tv_sec) > 40){  //afinw perithwrio .. kanonika thelei 20min
            pthread_mutex_lock(args->addresses->mut);
            // while(args->addresses->empty){
            //     printf("queue for addresses empty \n");
            //     pthread_cond_wait(args->addresses->notEmpty, args->addresses->mut);
            // }
            printf("Diagrapsame gt einai panw apo 20 sec edw %f\n",( t.tv_usec - args->addresses->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - args->addresses->contact[i].t.tv_sec);
            queueDel(args->addresses);
            pthread_mutex_unlock(args->addresses->mut);
            pthread_cond_signal(args->addresses->notFull);
            
        }
        
        pthread_mutex_unlock(&lock);

    }

}
/*
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
            if (timestamp>100) {
                //printf("more than 20s %f\n",timestamp);
                pthread_mutex_lock(addr->mut);
                while(addr->empty){
                    printf("queue for addresses empty \n");

                    pthread_cond_wait(addr->notEmpty, addr->mut);
                }
                queueDel(addr);
                pthread_mutex_unlock(addr->mut);
                pthread_cond_signal(addr->notFull);
            } else if (timestamp > 4) {
                printf("edw prepei na swsoume se close contacts kai na fugei to mac %d\n",addr->contact[i].mac);
                save_close_contact(addr, close_cont, i);
                pthread_mutex_lock(addr->mut);
                while(addr->empty){
                    printf("queue for addresses empty \n");
                    pthread_cond_wait(addr->notEmpty, addr->mut);
                }
                queueDel(addr);
                pthread_mutex_unlock(addr->mut);
                pthread_cond_signal(addr->notFull);

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
        pthread_mutex_lock(addr->mut);
        while(addr->empty){
            printf("queue for addresses empty \n");

            pthread_cond_wait(addr->notEmpty, addr->mut);
        }
        queueDel(addr);
        pthread_mutex_unlock(addr->mut);
        pthread_cond_signal(addr->notFull);
        //printf("Diagrapsame gt einai panw apo 20 sec edw %f\n",( t.tv_usec - addr->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - addr->contact[i].t.tv_sec);
    }
}
*/
//find mac address.. fantazomai me bluetooth kai tha einai 48bit
unsigned long long  get_mac(){  
    unsigned long long  mac;// : 48;
    //mac = (rand()%(100-1+1))+1 ;
    mac = rand()%384;
    return mac;
}
/*
void *find_mac(void *addr){
    int mac;
    mac = get_mac();

    //adds a cell in the queue
    queue *addresses;
    addresses = (queue *)addr;
    
    pthread_mutex_lock(addresses->mut);
    while(addresses->full){
        printf("queue for addresses full 1\n");
        pthread_cond_wait(addresses->notFull, addresses->mut);
    }
    queueAdd(addresses,mac);
    pthread_mutex_unlock(addresses->mut);
    pthread_cond_signal(addresses->notEmpty);

}
*/
void *find_mac_thread(void *arg){
    unsigned long long mac;//: 48;
    
    mac = get_mac();

    //adds a cell in the queue
    struct together *args;
    args = arg;
    
    pthread_mutex_lock(args->addresses->mut);
    while(args->addresses->full) {
        printf("queue for addresses full 2\n");
        pthread_cond_wait(args->addresses->notFull, args->addresses->mut);
    }
    queueAdd(args->addresses,mac);
    pthread_mutex_unlock(args->addresses->mut);
    pthread_cond_signal(args->addresses->notEmpty);

}
//ten seconds delay
/*void *timer(void *addr){
    int distance = rand()%10;

    sleep(1);
    i++;
    if (distance < MIN_DIST) {

        find_mac(addr);
    }


}*/

void *thread_timer(void *arg){
   while(1){

    int distance = rand()%10;
    //printf("timer pthread ID - %lu\n",pthread_self());
    sleep(1);
    //printf("Oh it's been a sec\n");
    if (distance < MIN_DIST) {

        find_mac_thread(arg);
    }
    
    counter++;
    //search for close contacts every 22 minutes
    if (counter > 22 ){   //should be 22 minutes
        pthread_mutex_lock(&lock);
        printf("It's time to search for close contacts\n");
        pthread_cond_signal(&wait_timer);
        counter = 0;
        pthread_mutex_unlock(&lock);

    } 

    counter2++;
    //delete close contacts if they are more than 14 days there
    if (counter2 > 100) { //should be 14 days and 1 hour or smth like that
        pthread_mutex_lock(&lock2);
        printf("It's been 14 days dude.. bye\n");
        pthread_cond_signal(&delete_close_wait);
        counter2 =0;
        pthread_mutex_unlock(&lock2);
    }

   }
}

int main(void) {
    
    struct timeval test_time, t1;
    bool test;
    FILE *f, *f2;
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
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock2, NULL);
    pthread_cond_init(&wait_timer,NULL);
    pthread_cond_init(&delete_close_wait, NULL);

    int i=0;
    gettimeofday(&test_time, NULL);
    
    //10 seconds delay and search for mac addresses near you on repeat
    if (pthread_create(&timer_thread,NULL,thread_timer, arg) != 0){
        perror("Failed to create thread");
    }

    //find if a contact is a close one
    if (pthread_create(&find_thread, NULL, find_close_contacts_thread, arg) !=0){  //mporei na mpei mes to timer?
        perror("Failed to create thread");
    }
    
    // delete close contacts after 14 days
    if (pthread_create(&delete_thread, NULL, delete_close_contacts_thread,arg->contact) != 0) {
        perror("Failed to create thread");
    }

    while (1){

       //every 4 hours does a test ....... HERE 6 MINUTES
       gettimeofday(&t1, NULL);
       
        if (((t1.tv_usec - test_time.tv_usec)/1.0e6 + t1.tv_sec - test_time.tv_sec)  > 60){
           test = testCOVID();
           //int j;
            if (test == 0) {
               printf("I AM POSITIVE STAY AWAY\n");
               save_server(arg->contact, f, f2);               
            } else {
               printf("MY TEST WAS NEGATIVE! LET'S HUG\n");
            }
           gettimeofday(&test_time, NULL);  // initialize again

            
        }

    
    }
    pthread_join(timer_thread,NULL);
    pthread_join(delete_thread, NULL);
    pthread_join(find_thread,NULL);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&wait_timer);
    pthread_mutex_destroy(&lock2);
    pthread_cond_destroy(&delete_close_wait);
    queueDelete(addresses);
    closeContactDelete(closeContacts);
    return 0;
}
