### Embedded Systems 

This repository contains the source code of the Final Assignment for the course of "Real Time and Embedded Systems" that took place in the Department of Electrical and Computer Engineering at Aristotle University of Thessaloniki in 2020-2021. 

### Subheader 
The goal of this assignment was to implement an application that traces the people that you have interacted with the last month (close contacts) and inform them when you are positive to covid or you have interacted with a positive to covid person. 
The application was tested on both on a computer running linux mint 20.3, Intel Core i5-8300H with 4 cores (8 logical processors) and a raspberry pi 0.

### Execution
To execute the code you need to compile it using
```sh
gcc -pthread queue.c covid_track.c -o output
```
and then run it using:
```
./output

