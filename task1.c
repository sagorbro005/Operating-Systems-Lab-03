#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

#define SHARED_SIZE sizeof(struct shared)

struct shared {
  char sel[100];
  int b;
};



int main() {
  
  int shmid = shmget(IPC_PRIVATE, SHARED_SIZE, IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("shmget");
    exit(1);
  }

  struct shared* shared = (struct shared*)shmat(shmid, NULL, 0);
  if (shared == (void*)-1) {
    perror("shmat");
    exit(1);
  }

  
  int pipefd[2];
  if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(1);
  }

  pid_t child_pid = fork();
  if (child_pid < 0) {
    perror("fork");
    exit(1);
  }

  if (child_pid == 0) { 
    close(pipefd[1]); 

    while (1) {
      
      while (shared->sel[0] == '\0');

      char selection = shared->sel[0];
      int amount;

      if (selection == 'a') {
        printf("Enter amount to be added: \n");
        if (scanf("%d", &amount) != 1 || amount <= 0) {
          printf("Adding failed, Invalid amount\n");
        } else {
          shared->b += amount;
          printf("Balance added successfully\nUpdated balance after addition:\n%d\n",shared->b);
        }
      } else if (selection == 'w') {
        printf("Enter amount to be withdrawn: \n");
        if (scanf("%d", &amount) != 1 || amount <= 0 || amount > shared->b) {
          printf("Withdrawal failed, Invalid amount\n");
        } else {
          shared->b -= amount;
          printf("Balance withdrawn successfully\nUpdated balance after withdrawal:\n%d\n", shared->b);
        }
      } else if (selection == 'c') {
        printf("Your current balance is:\n%d\n", shared->b);
      } else {
        printf("Invalid selection\n");
      }
      printf("Thankyou for using\n");
      break;
      
      shared->sel[0] = '\0';
    }

    close(pipefd[0]);
    exit(0);
  } else { 
    close(pipefd[0]); 

    shared->b = 1000;

    while (1) {
      printf("Provide Your Input from Given Options:\n");
      printf("1. Type a to Add Money\n");
      printf("2. Type w to Withdraw Money\n");
      printf("3. Type c to Check Balance\n");
      

      char selection;
      if (read(STDIN_FILENO, &selection, 1) != 1) {
        perror("read");
        exit(1);
      }

      strncpy(shared->sel, &selection, 1);
      shared->sel[1] = '\0'; 

      printf("Your selection: %c\n", selection);

      
      wait(NULL);
      break;
    }

    close(pipefd[1]); 
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);

    char message[100];
    if (read(pipefd[0], message, sizeof(message)) > 0) {
      printf("%s\n", message);

}

  }
}
