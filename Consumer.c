#include "header.h"

void initMemoryForTkey(int pState);
void initMemoryForJobs();
void initMemoryForJobInfo();
void DeleteJobs();

struct Job *p;//job pointer
struct Job *basePtr; // backup job pointer from  index 0 of shm
struct JobInfo *jobCountPtr;
struct TerminateState *tPtr;

int shmid;
int shmTerminateID;
int shmJobInfoID;

FILE *logPtr;
FILE *CommonFilePtr;
char timeStr[64];
struct tm *tm;
time_t t;
  

int main () { 

  printf("Welcome to printer jobs program !\n Maximun number of jobs to delete is : %d\n"
  "Number of jobs per cycle isn't limited unless it isnt more than created jobs running on system for that moment !\n",SIZE);
  
  t = time(NULL);
  tm = localtime(&t);
  strftime(timeStr,sizeof(timeStr),"%c",tm);
  
  CommonFilePtr = fopen("Producer&Consumer.txt","a");
  fprintf(CommonFilePtr,"\nConsuming system started ! -- %s\n",timeStr);
  fclose(CommonFilePtr);
  
  logPtr = fopen("Consumer.txt","w");
  fprintf(logPtr,"\nConsumer Activities during : %s",timeStr);
  fclose(logPtr);
  
  initMemoryForJobs();
  initMemoryForJobInfo();
  initMemoryForTkey(0);
  
  basePtr = p;
  int seq = 1;

  DeleteJobs();
  while(seq == 1 && jobCountPtr->JobCount != 0)
  {
    if(tPtr->ProducerState == 1) {//if producer requested termination
        seq = 0;
        initMemoryForTkey(1); // terminate processes
        break;
    }
    else {
     DeleteJobs();
     seq = 1;
    }
  }
  
  //when process is still running but there are no jobs to process
  if(jobCountPtr->JobCount == 0 && tPtr->ProducerState == 0)
  {
    printf("Process will terminate due to shortage of jobs to process !");
    initMemoryForTkey(1);
  }
  return 0;
}


void DeleteJobs() {
    
    printf("\nHow many jobs do you want to delete ? \n");
    int nrOfJobsToDel;
    scanf("%d",&nrOfJobsToDel);
    
    //while number of jobs to delete is greater then actual jobs running
    while(nrOfJobsToDel > jobCountPtr->JobCount){
     printf("\nThere are not %d availabe jobs to delete !",nrOfJobsToDel);
     printf("\nEnter new amount of jobs below %d !",jobCountPtr->JobCount);
     scanf("%d",&nrOfJobsToDel);
    }
    
    printf("What priority of jobs to delete ? \n");
    int jobPriority;
    scanf("%d",&jobPriority);
    
    while(jobPriority > 2 || jobPriority < 0){
        printf("Priority of a job can be from 0 to 2\nPlease enter priority value again : ");
        scanf("%d",&jobPriority);
    }
    
    int jobsDeleted = 0;
    

    for(int i = 0;i<SIZE;i++){
        
        strftime(timeStr,sizeof(timeStr),"%c",tm);
        if(jobsDeleted == nrOfJobsToDel){
            printf("\nAll jobs for this cycle have been deleted !");   
            break;
        }
        if(p->PID != 0 && p->Priority == jobPriority) { // finds the requested jobs to delete
            
            logPtr = fopen("Consumer.txt","a");
            fprintf(logPtr,"\nSuccessfully deleted job , ID = '%d', Priority = '%d'",p->PID,p->Priority);
            fclose(logPtr);
            
            printf("\nJob with PID = '%d' and Priority = '%d' has been deleted!",
            p->PID,p->Priority);
            p->PID = 0;
            p->Priority = 0;
            jobCountPtr->JobCount -= 1;
            jobsDeleted++;
            //kill(p->PID,SIGTERMcle0);//Proces has been killed
            sleep(1);
        }
         if(tPtr->ProducerState == 1) {
            initMemoryForTkey(1);
            break;
        }
        p++;//moves pointer to next job
    }
    printf("\nJobs left : %d \n",jobCountPtr->JobCount);
    logPtr = fopen("Consumer.txt","a");
    fprintf(logPtr,"\nJobs left after deletion cycle : %d",jobCountPtr->JobCount);
    fclose(logPtr);
    p = basePtr; // returns pointer to initial job of shared memory
}


// initializes memory for termination singal
void initMemoryForTkey(int p) {
    if(p == 0 ) {
        logPtr = fopen("Consumer.txt","a");
        shmTerminateID = shmget(termianteKey,sizeof(struct TerminateState),(IPC_CREAT | 0666));
        if(shmTerminateID < 0){
            fprintf(logPtr, "Couldnt allocate memory for termination key !\n");
        }
        tPtr = shmat(shmTerminateID,NULL,0);
        if(tPtr == (struct TerminateState *)-1){
            fprintf(logPtr, "Couldnt initilize your requested memory for termination key !\n");    
        }
        tPtr->ProducerState = p;
        fclose(logPtr);
    }
    else {
      logPtr = fopen("Consumer.txt","a");
      
      tPtr = shmat(shmTerminateID,NULL,0);
       if(tPtr == (struct TerminateState *)-1){
            fprintf(logPtr, "Couldnt initilize your requested memory for temporary key !\n");    
        }
      tPtr->ProducerState = p;
      printf("value : %d , address : %d\n",*tPtr,tPtr);
      printf("\nRequest for termination has been received !\n");
      fprintf(logPtr,"\nRequest for termination has been received !");
      printf("\nTerminating processes ...\n");
      sleep(2);
      
      CommonFilePtr = fopen("Producer&Consumer.txt","a");
      
      t = time(NULL);
      tm = localtime(&t);
      strftime(timeStr,sizeof(timeStr),"%c",tm);
      fprintf(CommonFilePtr,"\nSystem closed  succesfully at : %s \n Counting total of %d job production cycles and %d jobs left unprocessed !\n-----------------------------------------\n",timeStr,jobCountPtr->JobCycle,jobCountPtr->JobCount);
      
      fclose(CommonFilePtr);
      
      shmctl(shmid, IPC_RMID, 0);//deletes memory segments
      jobCountPtr->JobCount = 0;
      jobCountPtr->JobCycle = 0;
      tPtr->ProducerState = 0;// tells produces that the singal ...
      // to terminate arrived, and termination finished
      printf("\nAll processes have been terminated !\n\n");
      fprintf(logPtr,"\nAll processes have been terminated !\n");
      
    } 
}

// allocates memory for job Informations as Job Count and Cycle Count
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


//allocates memory for job structs
void initMemoryForJobs() {

       // ----- Shared memory for Job Info --------//
    shmJobInfoID = shmget(JobInfoKey,sizeof(struct JobInfo),0666 | IPC_CREAT);
    if(shmJobInfoID < 0) 
        fprintf(stderr,"We couldnt allocate memory for Job Info\n");
    
    jobCountPtr = shmat(shmJobInfoID,NULL,0);
    if(jobCountPtr == (struct JobInfo*)-1) 
    fprintf(stderr, "We couldnt initilize your requested memory of Job Info!\n");
}


