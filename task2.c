#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>



struct msg {
    long int type;
    char txt[11];
};

int main() {
    key_t key;
    int msqid;
    pid_t pid;

    
    key = ftok("/tmp", 'a');
    
    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    
    printf("Please enter the workspace name:\n");
    struct msg msg1;

    scanf("%s", msg1.txt);

    if (strcmp(msg1.txt, "cse321") != 0) {
 printf("Invalid workspace name\n");
        exit(1);
    }

    
    strcpy(msg1.txt, "cse321");
    msg1.type = 1; 
    msgsnd(msqid, &msg1, sizeof(struct msg), 0);
    printf("Workspace name sent to otp generator from log in: %s\n", msg1.txt);


    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { 
        msgrcv(msqid, &msg1, sizeof(struct msg), 1, 0);
        printf("OTP generator received workspace name from log in: %s\n", msg1.txt);

      
        pid_t otp = getpid();
        sprintf(msg1.txt, "%d", otp);
        msg1.type = 2;
        msgsnd(msqid, &msg1, sizeof(struct msg), 0);
        printf("OTP sent to log in from OTP generator: %s\n", msg1.txt);

        
        msg1.type = 3; 
        msgsnd(msqid, &msg1, sizeof(struct msg), 0);
        printf("OTP sent to mail from OTP generator: %s\n", msg1.txt);
        
        
        msg1.type =4;
        msgsnd(msqid, &msg1, sizeof(struct msg), 0);
        printf("Mail receieved OTP from OTP generator %s\n", msg1.txt);
        
        
        msg1.type = 5; 
        msgsnd(msqid, &msg1, sizeof(struct msg), 0);
        printf("OTP sent to log in from mail: %s\n", msg1.txt);



        exit(0);
    } else { 
        wait(NULL);

        
        msgrcv(msqid, &msg1, sizeof(struct msg), 2, 0);
printf("Log in received OTP from OTP generator: %s\n", msg1.txt);
        char otp_from_generator[6];
        strcpy(otp_from_generator,msg1.txt);
        
    


      
        msgrcv(msqid, &msg1, sizeof(struct msg), 3, 0);
        printf("Log in received OTP from mail: %s\n", msg1.txt);

        
      
        
        if (strcmp(otp_from_generator,msg1.txt)==0) {
            printf("OTP Verified\n");
        } else {
            printf("OTP Incorrect\n");
        }

        
        msgctl(msqid, IPC_RMID, NULL);
    }

    return 0;
}


