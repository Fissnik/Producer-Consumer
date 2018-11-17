#include "header.h"

void initMemoryForTkey(int pState);
void initMemoryForJobs();
void initMemoryForJobInfo();
void createJobs();

struct Job *p;//job pointer
struct Job *basePtr; // backup job pointer from  index 0 of shared memo
struct JobInfo *jobCountPtr;// pointer of JobInfor struct

int shmid;
int shmTerminateID;
int shmJobInfoID;

 FILE *logPtr;
 FILE *CommonFilePtr;
 char timeStr[64];
 struct tm *tm;
 time_t t;
  
int main() {      
  
  printf("Welcome to printer jobs program !\n Maximun number of jobs to create is : %d\n"
  "Number of jobs per cycle isn't limited unless it isnt more than empty slots for jobs!\n",SIZE);
    
  t = time(NULL);
  tm = localtime(&t);
  logPtr = fopen("Producer.txt","w");
  strftime(timeStr,sizeof(timeStr),"%c",tm);
  
  CommonFilePtr = fopen("Producer&Consumer.txt","w");
  fprintf(CommonFilePtr,"\nProduction system started ! -- %s\n",timeStr);
  fclose(CommonFilePtr);
  
  fprintf(logPtr,"Producer Activities during : %s",timeStr);
  fclose(logPtr);

  initMemoryForJobs();
  initMemoryForJobInfo();
  initMemoryForTkey(0);
  
  basePtr = p;//We need the address of the initial pointer when loop repeats
  int seq = 1;
  while(seq == 1)
  {
    if((jobCountPtr->JobCycle % 5 == 0) && (jobCountPtr->JobCycle != 0))
    {
       printf("\nDo you want to terminate process of creating new jobs ? \n"
       " 1 - YES , 0 - NO  ");
       int input;
       scanf("%d",&input);
       
       if(input == 1){
          initMemoryForTkey(1);
          seq = 0;
       }
       else {
         createJobs();
         seq = 1;
       }
    }
    else {
     createJobs();
    }
  }
  return 0;
}


void createJobs() {
  
  printf("\nHow many jobs do you want to produce for this cycle : ");
  int nr;
  scanf("%d",&nr);
  
  while(nr > (SIZE - (jobCountPtr->JobCount))) {
    printf("Not enough space for the required number of jobs !\n"
      "Maximimun number of jobs for the moment is : %d !\n "
      "Please enter available number of jobs !\n",
     (SIZE - (jobCountPtr->JobCount)));
    scanf("%d",&nr);
  }
  
  jobCountPtr->JobCycle += 1;
  logPtr = fopen("Producer.txt","a");
   fprintf(logPtr,"\n------------------- Production Cycle %d ----------------------",jobCountPtr->JobCycle);
   fclose(logPtr);
  
  
  int Count = 0;
  for(int i = 0; i<nr;i++)
  {
      if(p->PID != 0) --i;
      int rNr = rand() % 3;     
      
      if((fork()) == 0) { //child process only
        if(p->PID == 0)//if this is empty
        {
            p->PID = getpid();
            p->Priority = rNr;
            printf("\nJob with ID = '%d' and Priority = '%d' created!",p->PID,p->Priority);
            jobCountPtr->JobCount += 1;
            logPtr = fopen("Producer.txt","a");
            fprintf(logPtr,"\nSuccessfully created job , ID = '%d', Priority = '%d' in memory address : '%d'",p->PID,p->Priority,p);
            fclose(logPtr);
            
            sleep(1);
        } 
        exit(0);
     }
     ++p;
     wait(NULL);
  }  
    p = basePtr;
};


void initMemoryForTkey(int p) {

    struct TerminateState *tPtr;
    if(p == 0 ) {
        logPtr = fopen("Producer.txt","a");
        shmTerminateID = shmget(termianteKey,sizeof(struct TerminateState),(IPC_CREAT | 0666));
        if(shmTerminateID < 0){
            fprintf(logPtr,"\nFailed to allocate memory for termination key !");
        }
        tPtr = shmat(shmTerminateID,NULL,0);
        if(tPtr == (struct TerminateState *)-1){
            fprintf(stderr, "We couldnt initilize your requested memory for termination key !\n");    
        }
        tPtr->ProducerState = p;
        fclose(logPtr);
    }
    else {
        logPtr = fopen("Producer.txt","a");
      tPtr = shmat(shmTerminateID,NULL,0);
       if(tPtr == (struct TerminateState *)-1){
            fprintf(logPtr,"\nFailed to initialize memory for termination key !");
        }
      tPtr->ProducerState = p;
      printf("value : %d , address : %d\n",*tPtr,tPtr);
      printf("Process requested to terminate !\n");
      fprintf(logPtr,"\nProcess request to termiante sent !");
      
      while(tPtr->ProducerState != 0){
       printf("\nWaiting for signal that processes have been shut down !");
       fprintf(logPtr,"\nWaiting for signal that processes have been shut down !");
       sleep(2);
      }
      printf("\nAll procceses have been terminated !\n\n");
      fprintf(logPtr,"\nAll procceses have been terminated !");

    }
    fclose(logPtr); 
}


void initMemoryForJobs() {

    // ----- Shared memory for Job Info --------//
    shmJobInfoID = shmget(JobInfoKey,sizeof(struct JobInfo),0666 | IPC_CREAT);
    if(shmJobInfoID < 0) 
        fprintf(stderr,"We could allocate memory for Job Info\n");
    
    jobCountPtr = shmat(shmJobInfoID,NULL,0);
    if(jobCountPtr == (struct JobInfo*)-1) 
    fprintf(stderr, "We couldnt initilize your requested memory of Job Info!\n");
}


void initMemoryForJobInfo() {
  // ------ Shared memory for jobs ----------//
  shmid  = shmget(key,SIZE * sizeof(struct Job),0666 | IPC_CREAT);
  if(shmid < 0){
     fprintf(stderr, "We couldnt allocate memory for you !\n");
  }
  p =  shmat(shmid,NULL,0);
  if(p == (struct Job*) -1) {
      fprintf(stderr, "We couldnt initilize your requested memory !\n");
  }
}



