#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <unistd.h>

int randForSleep(){
  srand((time(NULL)));
  int randomNum = ((rand()%5)+1);
  return randomNum;
}
int randForMoney_A(){
  srand((time(NULL)*390));
  int randomNum = ((rand()%100)+1);
  return randomNum;
}

int randForMoney_B(){
  srand((time(NULL)*390));
  int randomNum = ((rand()%50)+1);
  return randomNum;
}


int main(){
  int bankAcct = 0;
  int turn = 0;
  int balance;
  int shmID;
  int *shmPtr;
  
  shmID = shmget(IPC_PRIVATE, sizeof(int), IPC_CREAT | 0666);
  shmPtr = (int *) shmat(shmID, NULL, 0);
    if(*shmPtr == -1){
      printf("*shmat error*\n");
      exit(1);
    }
  shmPtr[0] = bankAcct;
  shmPtr[1] = turn;
  
  pid_t pid;
  pid = fork();
  
  if (pid < 0)
    printf("Fork error");
  // parent
  else if (pid >0){
    int j = 1;
    for(j =1; j <= 25; j++){
      sleep(randForSleep());
      while (shmPtr[1] != 0);
      if (bankAcct<= 100){
        balance = randForMoney_A();
        if (balance % 2 ==0){
          shmPtr[0] += balance;
          printf("Dad: Deposits $%d / Balance = $%d\n", balance, shmPtr[0]);
        }
        if (balance % 2 != 0){
          printf("Dad: No money to give\n");
        }
        shmPtr[1]= 1;
      }
      else{
        printf("Dad: Thinks student has enough money (%d\n)", shmPtr[0]);
      }
    }
    exit(0);
  }
  // child
  else{
    int j = 1;
    for(j =1; j <= 25; j++){
      sleep(randForSleep());
      while (shmPtr[1] != 1);
      balance = randForMoney_B();
      printf("Student needs $%d\n", balance);
      if (balance <= shmPtr[0]){
        shmPtr[0] -= balance;
        printf("Student: withdrwas $%d / balance = $%d\n", balance, shmPtr[0]);
        
      }
      if (balance > shmPtr[0]){
        printf("Student: Not enough cash ($%d\n)", shmPtr[0]);
      }
      shmPtr[1] = 0;
    }
    exit(0);
  }
}

