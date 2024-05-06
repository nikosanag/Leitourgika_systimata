#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>

struct sigaction action1,action2,action3,action4,action5;
char state[150];
int len;
bool termcommand=false;
pid_t pid_collection[150];
pid_t birth,parent;




void sigusr1_handler(){
for(int i=0; i<len; i++) 
	kill(pid_collection[i],SIGUSR1);
}
void sigusr2_handler(){
for(int i=0; i<len; i++)
	kill(pid_collection[i],SIGUSR2); 
}
void sigterm_handler(){
termcommand = true;
int status;
int number = len;
for(int i = 0; i<len; i++){
printf("[PARENT/PID = %d] Waiting for %d children to exit\n",getpid(),number);
printf("Child with PID=%d terminated successfully with exit status code 0!\n",pid_collection[i]);
kill(pid_collection[i],SIGTERM);
number=number-1;
if(wait(&status)==-1) 
	printf("There are no child process to wait for!\n");


}
printf("[PARENT/PID = %d] All children exited, terminating as well\n",getpid());
exit(0);
}



void sigchld_handler(){
if(termcommand == false){//simainei oti den prepei akoma na kleiseito programma
int status,save;
pid_t result = waitpid(-1,&status,0);//waiting for any child to be executed

for(int i = 0; i<len; i++) {
if(pid_collection[i]==result) save = i;}//found the critical info for the id of the child that needs to be executed!
if(result>0){//child has been executed succesfully!
if(WIFEXITED(status) || WIFSIGNALED(status)){
printf("[PARENT/PID=%d] Child %d with PID=%d exited\n",getpid(),save,pid_collection[save]);
pid_t new = fork();
if (new<0) {perror("failure in creating a new child"); }
if(new==0) {printf("[PARENT/PID=%d] Created new child for gate %d (PID=%i) and initial state '%c'\n",getppid(),save,getpid(),state[save]);
char buf[20];
char st = state[save];
char num = '0' + save ; 
sprintf(buf,"%c%c",num,st);
char *argss[] = {"./child",buf,NULL};
execv(argss[0],argss);
perror("execv did not work");
}
else if (new>0) pid_collection[save] = new;
else perror("error with  child creation");
}
else if (WIFSTOPPED(status)){
kill(pid_collection[save],SIGCONT);
}

}
else if(result==-1){
if (errno=ECHILD){
printf("Error:no child exists\n");
}
else if(errno ==EINTR){
printf("Error: waitpid() interrupted by a signal\n");
}
else {
perror("waitpid() error");
}
}

}
}



int main(int argc,char* argv[]){

char *input = argv[1];
len = strlen(argv[1]);
if(argc!=2) {
     perror("incorrect input;");
     return 1; 
}

for(int i = 0; i<=len-1; i++){
      if(*input!='f' && *input!='t'){
      perror("only f and t allowed!");
      return 1; 
       }
     state[i]=*input; 
     input+=1;
}

action1.sa_handler = sigusr1_handler;
action2.sa_handler = sigusr2_handler;
action3.sa_handler = sigterm_handler;
action4.sa_handler = sigchld_handler;

sigaction(SIGUSR1,&action1,NULL);
sigaction(SIGUSR2,&action2,NULL);
sigaction(SIGTERM,&action3,NULL);
sigaction(SIGCHLD,&action4,NULL);

//child creation
for(int i=0; i<=len-1; i++){
     birth=fork();
     if(birth<0){
     perror("Failure in creating one child");
     return 1;
   }
     if(birth==0){
     
     char storage[20];
     char id = '0'+i;
     sprintf(storage,"%c%c",id,state[i]);
     char *args[] = {"./child",storage,NULL};
     execv(args[0],args); 
     perror("execv did not work");
     return 1;
   }
     if(birth>0){
     pid_collection[i] = birth;
     parent = getpid();
     printf("[PARENT/PID=%d] Created child %d (PID=%d) and initial state '%c'\n",getpid(),i,pid_collection[i],state[i]); 
   }

}


while(1){
sleep(1);
}

return 0;

}







