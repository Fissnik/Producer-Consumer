# Producer-Consumer
A program which will make possible two seperate processes, client and producer to communicate with each other using shared memory

Producer is responsible for creating Jobs with random priority, and the Id of each job must be equal with child procces forked by current one

While Consumer is responsible of consuming these jobs, and removing them from the shared memory

 To make this happen, producer program must be ran first and create some jobs, then Consumer has to remove those jobs.
 
 Two programs must work in synchronisation with each other, each 5 iterations the producer program will ask user if he wishes to continue producing new jobs or terminate.
 If producer chooses to terminate, the Consumer will be signaled and it will terminate both processes of programs.
 
 Project created by: Fisnik Maloku <ffisnikmaloku@gmail.com> 07.JAN.2018 
