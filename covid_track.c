// COVID Tracking

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#define MIN_DIST 11
#define TEN_SECONDS 10
#define ONE_SECOND 1


void BTnearMe();  //macaddress BTnearMe()
bool testCOVID(); 
void upladContacts (int* macaddress);
void timer();
void search();

int main () {

    bool test;


    //pthread_t tid_timer;
    //pthread_create(&tid_timer, NULL, &timer, NULL);

    int done = 10;
    while (done >0) {
        search();
        timer();

        // if (distance <= MIN_DIST) {    //logika an macaddress == traceable
        //     BTnearMe();
              // test = testCOVID();
        // }
        --done;
    }
    if (test == 1) {
        printf("ITS POSITIVE STAY AWAY\n");
    } else {
        printf("ITS NEGATIVE you can hang out with this person\n");
    }
}

void search() {
    float distance=rand()%20;  //make a random number for the distance in the range of [0,20]
    //printf("%f \n", distance);
    if (distance <= MIN_DIST) {    //logika an macaddress == traceable
        BTnearMe();
    }
}
void timer(){
    sleep(ONE_SECOND);  //we need 10 seconds
}

void BTnearMe() {
    printf("there is a traceable device near you\n");
}


bool testCOVID(){
    printf("has this person a negative or a positive test?\n");
    return 1;
}

void upladContacts (int* macaddress) {

}
