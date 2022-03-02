// COVID Tracking
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include "covidTrace.h"
#include <ifaddrs.h>
#include <netpacket/packet.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


#define TEN_SECONDS 1 // NEEDS TO BE 10
#define CLOSE_CONTACT_TIME 13 //1320 seconds for 22 minutes / 10 (timer) 
#define DELETE_CONTACT_TIME 120 //1209600 seconds for 14 days / 10(timer)
#define UPPER_LIMIT 50 // 20 minutes
#define LOWER_LIMIT 4 //4 minutes
#define TEST_TIME 100 // 4 hours
#define ADDRESSES 11
#define PORT 8080


pthread_mutex_t lock, lock2, lock_test;// = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t wait_timer, delete_close_wait, test_wait;
int counter =0, counter2 = 0, counter3=0;//, delete = 0, save =0;

char IPs[ADDRESSES][16] = {"10.0.84.19", "10.0.0.7"};
//long long unsigned
char Macs[ADDRESSES][48] ={"15","20","33", "78","144","54899", "5445645654", "65657687645", "54646" "b8:27:eb:8a:62:62", "94:0c:6d:8b:7e:10"};//{b827eb8a6262, 940c6d8b7e10}; //DEN DOULEUEI KALA

void *client(void *arg){
    close_contact *cont;
    cont = arg;

    int i = cont->contact->position;
    //ara Ips[i]

    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *send_message = "pc I am positive stay away";
    // char buffer[1024] = {0};

       
    // //for (int i =0; i < ADDRESSES; i++) {
    //     //if (IPs[i] != htonl(INADDR_ANY)){

        

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        //return -1;
    }
    
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "10.0.0.7", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        //return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        //return -1;
        close(sock);
    }   


    send(sock , send_message , strlen(send_message) , 0 );
    //printf("pc: notified rasb that i am positive\n");
    //         valread = read( sock , buffer, 1024);
    //         printf("%s\n",buffer );
    //     //}
    // //}
}

void save_server(void *arg, FILE *f){
    close_contact *close_cont;
    close_cont = arg;
    int i = close_cont->head;


    pthread_t client_thread;
    pthread_create(&client_thread, NULL, client, close_cont);
    while(i != close_cont->tail){
        
        //prostetei 6 midenika meta to mac kai ta swzei se hex?  epishs an den ta diagrapseis sinexizoun na grafoun
        fwrite(&close_cont->contact[i], sizeof(contact), 1, f); //auto swzei logika kai to timestamp
                
        i++;
        if (i > CLOSE_CONTACTS){
            i = 0;
        }
    }
    pthread_join(client_thread,NULL);
}

// checks if my covid test is positive or negative
void *test(void *arg) {
    FILE *f;
    while(1) {
        pthread_mutex_lock(&lock_test);
        //printf("Waiting 4 hours for the test \n");
        pthread_cond_wait(&test_wait, &lock_test);
        //printf("Having a covid test\n");
        bool test_covid = rand()%2;
        close_contact *close_cont;
        close_cont = arg;
        if (test_covid == 0) {
            printf("I AM POSITIVE STAY AWAY\n");
            f = fopen("close_contacts.bin","ab");
            if (f == NULL) {
                fprintf(stderr, "failed to make file");
                exit(1);
            }
            save_server(close_cont, f);   
            fclose(f);
            
        } else {
            printf("MY TEST WAS NEGATIVE!\n");
        }
        pthread_mutex_unlock(&lock_test);

    }
}
// server function ready to accept messages
void *server(){
    // INITIALISATION OF SERVER .. get ready to listen to messages
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1; 
    int addrlen = sizeof(address);
    char received_message[2048] = {0}; 
    //char *hellomessage = "pc: thanks for the notification"; //???????

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    //address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_addr.s_addr = inet_addr("10.0.84.19");  //to bind xreiazetai my ip exei kai entoli gia fill my ip
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    
    while(1){
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        valread = read(new_socket , received_message, 1024);
        printf("I received: %s\n",received_message );
        //send(new_socket , hellomessage , strlen(hellomessage) , 0 );
        //printf("pc : I notified them that I got the message\n");
    }
}


// delete close contacts that have been in the queue for more than 14 days
void *delete_close_contacts(void *arg){
    while(1){
        pthread_mutex_lock(&lock2);
        //printf("Waiting for 14 days to delete my close contacts...\n");
        pthread_cond_wait(&delete_close_wait, &lock2);
        close_contact *close_cont;
        close_cont = arg;
        long i = close_cont->head;
        struct timeval t1;
        gettimeofday(&t1, NULL);
        while(i != close_cont->tail){
            float timestamp = ((t1.tv_usec - close_cont->contact[close_cont->head].t.tv_usec)/1.0e6 + t1.tv_sec - close_cont->contact[close_cont->head].t.tv_sec) ;
            
            //if timestamp of close contact is larger than 14 days then delete 
            if (timestamp < DELETE_CONTACT_TIME) {
                    //delete++;
                    //printf("I have entered delete contacts %d times but i didnt have to\n", delete);    
                break; //if the first one is less than 14 days then the next one is for sure less than that
            
            // delete close contacts if their timestamp is more than 14 days
            } else {
                //printf("It's been 14 days mac %llu, I am deleting you\n", (unsigned long long)close_cont->contact[close_cont->head].mac);
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

// save my close contacts in a queue
void save_close_contact(void *arg, long i) {
    struct together *args;
    args = arg;
    pthread_mutex_lock(args->contact->mut);
    while(args->contact->full) {
        printf("queue for close contacts full\n");
        pthread_cond_wait(args->contact->notFull, args->contact->mut);
    }
    closeContactAddThread(args, i);
    printf("mac #%llu is a close contact to me \n", args->contact->contact[args->contact->tail-1].mac);  //-1 gt auxanoume thn oura
    pthread_mutex_unlock(args->contact->mut);
    pthread_cond_signal(args->contact->notEmpty);
    
}

// find which contacts have been near me for more than 4 but less than 20 minutes
void *find_close_contacts(void *a){

    unsigned long long history;
    while(1) {

        pthread_mutex_lock(&lock);
        //printf("Waiting for (20+ minutes) ...\n");
        pthread_cond_wait(&wait_timer, &lock);          
        //printf("searching for close contacts\n");

        struct together *args;
        args = a;
        long i = args->addresses->tail;

        float timestamp, time;
        struct timeval t;
        gettimeofday(&t, NULL);

        while(args->addresses->empty){
            printf("queue for addresses is empty \n");
            pthread_cond_wait(args->addresses->notEmpty, args->addresses->mut);
        }
        history = 0;
        
        time = (( t.tv_usec - args->addresses->contact[args->addresses->head].t.tv_usec)/1.0e6 + t.tv_sec - args->addresses->contact[args->addresses->head].t.tv_sec);
        //delete contact if it stays on the queue for more than 20 minutes and doesnt repeat   (head of the queue)
        if (time > UPPER_LIMIT*2+1){  //afinw perithwrio .. kanonika thelei 20min
            pthread_mutex_lock(args->addresses->mut);
            //printf("Delete contact because it's been 40 minutes and haven't seen it%f\n",( t.tv_usec - args->addresses->contact[i].t.tv_usec)/1.0e6 + t.tv_sec - args->addresses->contact[i].t.tv_sec);
            queueDel(args->addresses);
            pthread_mutex_unlock(args->addresses->mut);
            pthread_cond_signal(args->addresses->notFull);
             
        } 

        //search the whole queue
        while(i != args->addresses->head){
            time = (( t.tv_usec - args->addresses->contact[args->addresses->head].t.tv_usec)/1.0e6 + t.tv_sec - args->addresses->contact[args->addresses->head].t.tv_sec);
            if (time < LOWER_LIMIT){
                break;
            }
            // if the same mac has been near us more than once
            if (args->addresses->contact[args->addresses->head].mac == args->addresses->contact[i].mac){
            
                timestamp = (args->addresses->contact[i].t.tv_usec - args->addresses->contact[args->addresses->head].t.tv_usec)/1.0e6 + (args->addresses->contact[i].t.tv_sec - args->addresses->contact[args->addresses->head].t.tv_sec);
                //printf("timestamp %fsec of mac %llu\n", timestamp, (unsigned long long)args->addresses->contact[args->addresses->head].mac);
                
                // delete contacts that have interacted with us for more than 20 minutes
                if (timestamp>UPPER_LIMIT) {
                    //printf("more than 20 minutes %f\n",timestamp);
                    
                    pthread_mutex_lock(args->addresses->mut);

                    queueDel(args->addresses);
                    pthread_mutex_unlock(args->addresses->mut);
                    pthread_cond_signal(args->addresses->notFull);
                    
                //save contacts as close the ones that interacted with us in a period of more than 4 minutes but less than 20
                } else if (timestamp > LOWER_LIMIT) {
                    // if i haven't already saved this mac address in this round of searches
                    if (history != args->addresses->contact[i].mac) {
                        //printf("Save to close contact and delete from the address queue of mac %llu\n",(unsigned long long)args->addresses->contact[i].mac);
                    
                        history = args->addresses->contact[args->addresses->head].mac;
                        save_close_contact(args, i);
                    }
                    pthread_mutex_lock(args->addresses->mut);
                    queueDel(args->addresses);
                    pthread_mutex_unlock(args->addresses->mut);
                    pthread_cond_signal(args->addresses->notFull);
                    
                }
                // if timestamp < 4 minutes stop the search
                break;
            
            //move the tail
            } else {
                i--;
                if (i < 0){
                    i = QUEUESIZE;
                }
            }
        }   
        pthread_mutex_unlock(&lock);

    }

}

//find mac address
void *find_mac(void *arg){
    unsigned long long mac;//: 48;
    
    // random function to decide if a device is in a near enough distance
    if (rand()%8 == 1) {
        int i = rand()%ADDRESSES;   
        mac = (Macs[i]);
        //mac = 855534166;
        /*
            ΠΡΕΠΕΙ ΝΑ ΕΛΕΓΧΩ ΤΙΣ ΔΙΑΦΟΡΕΣ ΜΑΚ ΔΕΝ ΧΡΕΙΑΖΕΤΑΙ ΕΠΙΚΟΙΝΩΝΙΑ ΕΔΩ
            check for mac addresses
        

        // struct ifaddrs *ifaddr=NULL;
        // struct ifaddrs *ifa = NULL;
        // int i = 0;

        // if (getifaddrs(&ifaddr) == -1) {
        //     perror("getifaddrs");
        // } else {
        //     for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        //         if ((ifa->ifa_addr) && (ifa->ifa_addr->sa_family == AF_PACKET)) {
        //             struct sockaddr_ll *s = (struct sockaddr_ll*)ifa->ifa_addr;
        //             printf("%-8s ", ifa->ifa_name);
        //             for (i=0; i <s->sll_halen; i++) {
        //                 mac2 = printf("%02x%c", (s->sll_addr[i]), (i+1!=s->sll_halen)?':':'\n');
        //             }
        //         }
        //     }
        //     printf("mac 2 = %llu\n",mac2);
        //     freeifaddrs(ifaddr);
        // }
        */
        
        //adds a cell in the queue
        struct together *args;
        args = arg;
        
        pthread_mutex_lock(args->addresses->mut);
        while(args->addresses->full) {
            printf("queue for addresses full \n");
            pthread_cond_wait(args->addresses->notFull, args->addresses->mut);
        }
        queueAdd(args->addresses,mac,i);
        pthread_mutex_unlock(args->addresses->mut);
        pthread_cond_signal(args->addresses->notEmpty);
    } else {
        //printf("Didn't find a mac \n");
    }
}

//ten seconds delay
void *timer(void *arg){
   while(1){

    sleep(TEN_SECONDS);

    //printf("It's been 10 seconds, time to search for macs\n");
    find_mac(arg);
        
    counter++;
    //search for close contacts every 22 minutes
    if (counter > CLOSE_CONTACT_TIME ){   
        pthread_mutex_lock(&lock);
        //printf("It's time to search for close contacts\n");
        pthread_cond_signal(&wait_timer);
        counter = 0;
        pthread_mutex_unlock(&lock);

    } 

    counter2++;
    //delete close contacts if they are more than 14 days there
    if (counter2 > DELETE_CONTACT_TIME) { 
        pthread_mutex_lock(&lock2);
        //printf("It's been 14 days, no longer close contacts\n");
        pthread_cond_signal(&delete_close_wait);
        counter2 =0;
        pthread_mutex_unlock(&lock2);
    }

    counter3++;
    if (counter3 > TEST_TIME){
        pthread_mutex_lock(&lock_test);
        //printf("It's been 4 hours, time for my test\n");
        pthread_cond_signal(&test_wait);
        counter3=0;
        pthread_mutex_unlock(&lock_test);
    }

   }
}

int main(void) {
    

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

    pthread_t timer_thread,find_thread, delete_thread, save_thread, server_thread, test_thread;
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&lock2, NULL);
    pthread_mutex_init(&lock_test, NULL);

    pthread_cond_init(&wait_timer,NULL);
    pthread_cond_init(&delete_close_wait, NULL);
    pthread_cond_init(&test_wait, NULL);

        
    if (pthread_create(&server_thread, NULL, server, NULL) != 0){
        perror("Failed to create thread");
    }
    //10 seconds delay and search for mac addresses near you on repeat
    if (pthread_create(&timer_thread,NULL,timer, arg) != 0){
        perror("Failed to create thread");
    }

    //find if a contact is a close one
    if (pthread_create(&find_thread, NULL, find_close_contacts, arg) !=0){  //mporei na mpei mes to timer?
        perror("Failed to create thread");
    }
    
    // delete close contacts after 14 days
    if (pthread_create(&delete_thread, NULL, delete_close_contacts,arg->contact) != 0) {
        perror("Failed to create thread");
    }

    if (pthread_create(&test_thread, NULL, test, arg->contact) != 0){
        perror("Failed to create error");
    }

    pthread_join(server_thread, NULL);
    pthread_join(timer_thread,NULL);
    pthread_join(delete_thread, NULL);
    pthread_join(find_thread,NULL);
    pthread_join(test_thread,NULL);
    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&wait_timer);
    pthread_mutex_destroy(&lock2);
    pthread_cond_destroy(&delete_close_wait);
    pthread_mutex_destroy(&lock_test);
    pthread_cond_destroy(&test_wait);

    queueDelete(addresses);
    closeContactDelete(closeContacts);
    return 0;
}
