// Project -> Create a simple program which will make possible two seperate processes, client and producer to communicate with each other using shared memory
// Producer is responsible for creating Jobs with random priority, and the Id of each job must be equal with child procces forked by current one
// While Consumer is responsible of consuming these jobs, and removing them from the shared memory
// To make this happen, producer program must be ran first edhe create some jobs, then Consumer to remove those jobs
// Two programs must work in synchronisation, each 5 iterations the producer program will ask user if he wishes to continue producing new jobs or terminate
// If producer chooses to terminate, the Consumer will be signaled and it will terminate both processes of programs.
// Project created by: Fisnik Maloku <ffisnikmaloku@gmail.com> 07.JAN.2018 

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <time.h>

#define SIZE 100 // max number of jobs

struct Job {
  int PID;
  int Priority;
};

struct TerminateState {
  int ProducerState;
};

struct JobInfo {
  int JobCount;
  int JobCycle;
};

key_t JobInfoKey = 1000;
key_t key = 3000;
key_t termianteKey = 2000;
