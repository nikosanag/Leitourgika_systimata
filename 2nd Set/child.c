#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h> 



struct sigaction action1,action2,action3,action4;
time_t start_time;
char state;
int id;
pid_t info;



void sigusr1_handler(){

	
if(state=='f')
        printf("[ID=%d/PID=%d/TIME=%ds] The gates are closed!\n",id,getpid(),(int)(time(NULL)-start_time));
else 
        printf("[ID=%d/PID=%d/TIME=%ds] The gates are open!\n",id,getpid(),(int)(time(NULL)-start_time));

}
void sigusr2_handler(){
if(state=='f') {state='t'; printf("ID=%d/PID=%d/TIME=%ds] The gates are open!\n",id,info,(int)(time(NULL)-start_time));} 
else {state = 'f'; printf("ID=%d/PID=%d/TIME=%ds] The gates are closed!\n",id,info,(int)(time(NULL)-start_time));}


}
void sigalrm_handler(){
if(state=='f'){
alarm(15);
        printf("[ID=%d/PID=%d/TIME=%ds] The gates are closed!\n",id,getpid(),(int)(time(NULL)-start_time));
}
else if(state=='t'){
        alarm(15);
        printf("[ID=%d/PID=%d/TIME=%ds] The gates are open!\n",id,getpid(),(int)(time(NULL)-start_time));

}
else perror("sigalrm failed miserably!");
}

void sigterm_handler(){
exit(0);
perror("did not execute");
}



int main(int argc,char *argv[]){

	
info = getpid();
action1.sa_handler=sigusr1_handler; 
action2.sa_handler=sigusr2_handler;
action3.sa_handler=sigalrm_handler;
action4.sa_handler=sigterm_handler;

sigaction(SIGUSR1,&action1,NULL);
sigaction(SIGUSR2,&action2,NULL);
sigaction(SIGALRM,&action3,NULL);
sigaction(SIGTERM,&action4,NULL);

char *read; 
read=argv[1];
id = *read-'0'; 

read+=1;
state = *read ; 
usleep(1000);

if(state=='f')
        printf("[ID=%d/PID=%d/TIME=0s] The gates are closed!\n",id,getpid());
else if(state=='t')
        printf("[ID=%d/PID=%d/TIME=0s] The gates are open!\n",id,getpid());
alarm(15);
start_time=time(NULL);



while(1){
sleep(2);
}








}





