#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>



bool isNumber(const char *str){
	char *endptr;
	strtol(str,&endptr,10);
	return *str!='\0' && *endptr == '\0';
}//checks if a string is a number


void sigterm_handler_for_child(){	
	exit(0);
}


struct sigaction action ;


 

int main(int argc, char *argv[]){

int method;//0 = round-robin,1 = random
if(argc==2 && isNumber(argv[1]))
	method = 0 ;

else if (argc!=4 || !isNumber(argv[1]) || strcmp(argv[2],"-")!=0 || (strcmp(argv[3],"-random")!=0 && strcmp(argv[3],"-round-robin")!=0)){
	       	printf("Usage:ask3 <nChildren> [--random] [--round-robin]]\n");
	        return 1;
	}
else { 
	if(strcmp(argv[3],"-round-robin")==0)
	        
		method = 0;
	else
		
		method = 1;
}


int n = atoi(argv[1]);


int (*parent_to_child)[2] = malloc(n * sizeof(*parent_to_child));
int (*child_to_parent)[2] = malloc(n * sizeof(*child_to_parent)); 
pid_t *library = malloc(n*sizeof(*library)); 







//dimiourgia pipe kai elegxos
for(int i = 0; i<n; i++){


if(pipe(parent_to_child[i])!=0 || pipe(child_to_parent[i])!=0)//pipe creation
	perror("pipe creation error!");//and error checking
}


pid_t child ; 

//dimiourgia paidion
for(int i=0 ; i<n; i++){
          
	child = fork();


//error with fork

	if (child<0){
 	
	perror("something went wrong with fork");
	exit(1);
}
//child
     else if (child==0){
	
     
	action.sa_handler = sigterm_handler_for_child ; 
  	sigaction(SIGTERM,&action,NULL); //child learns how to terminate when parent asks it via sending SIGTERM
	close(parent_to_child[i][1]);
        close(child_to_parent[i][0]);
	int val;
	while(1){
	
	        if(read(parent_to_child[i][0],&val,sizeof(int))==-1)
			perror("read error"); 
                val = val-1;
		sleep(10);

		if(write(child_to_parent[i][1],&val,sizeof(int))==-1)
			perror("write error");
	}


     }

//parent code

      else {
	close(parent_to_child[i][0]);
        close(child_to_parent[i][1]);
        library[i]=child;//store pid for sending sigterm later from parent to child.Need this so when user type exit everything
			 //exits and the parent exits.
      }
}
//rest of parent code

struct pollfd *pollfds =  malloc((n+1)*sizeof(struct pollfd));

//h 8esi n gia eisodo apo xristi
pollfds[n].fd = 0;
pollfds[n].events = POLLIN;  



for (int i = 0; i<n; i++){
       pollfds[i].fd = child_to_parent[i][0];
       pollfds[i].events = POLLIN; 
}




     
int destination = -1 ;//child that will take the job according to the method


if(strcmp(argv[3],"-random"))
         srand(time(NULL)); //if the method is random then plant the seed of random number genarator
             

while(1){
		
        int rat = poll(pollfds,n+1,-1);
        
	//elegxos gia poll 
	
	
	if(rat==-1){
		perror("poll error!");
	        return 1;
	} //meta ton elegxo...den egine to perror trigger
      	
         
	//anazitisi tou poiou file descriptor egine enable apo ta pipes
        
	for(int i = 0; i<n; i++){
		if(pollfds[i].revents & POLLIN){
		int val;
		read(pollfds[i].fd,&val,sizeof(int));//error checking
		printf("%d\n",val); 
		}
	     }
	
	//elegxos kai gia input xristi
        if(pollfds[n].revents & POLLIN){
	
	
		 char vall[100];

		fgets(vall,sizeof(vall),stdin);//error check
                vall[strcspn(vall, "\n")]='\0' ;//removal of "\n" because it creates problems in strcmp 	
		
	
	        
	
		if(isNumber(vall)==true){
                        
	
		
			int input = atoi(vall); 
        
		
			
	
	
	//ana8esi se poio paidi????
	//-random
			if (method==1){
	                             destination = rand()%n; //means that destination = 0 or... or n-1 
				}
	      	
	//-round-robin
	
		
			else if(method==0){

	
			if(destination == n-1) 
					destination = 0; 
	
			
				else
					destination = destination + 1; 
        
		
			}
                        printf("[PARENT]Assigned %d to child %d\n",input,destination);
                         
	       	
			if(write(parent_to_child[destination][1],&input,sizeof(int))==-1){//send the task
		
			
			
				perror("error sending to child their task!"); //security check for perror
	        
			
				return 1; 
	
		
			}

	continue;
	
	}
	
	else{
	//an zitisei help 

		if (strcmp(vall,"help") == 0) 
		
			printf("Type a number to send a job to a child!\n"); 
	
	
	//termatizei paidia kai eauto tou
	
		else if(strcmp(vall,"exit") == 0){
	               
			for(int i=0; i<n; i++){
			close(parent_to_child[i][1]); 
			close(child_to_parent[i][0]);
			if(kill(library[i],SIGTERM)!=0) 
				perror("sigterm has not been send!");//security check for kill command

			printf("Child %d has exited successfully!\n",i);
			}
	        free(parent_to_child);
		free(child_to_parent);
		free(library);
		
			
	        printf("[PARENT]All children exited.Terminating as well!\n");
		exit(0);
	
		}
	//se opoia alli periptosi
	
		else
		
			printf("Type a number to send a job to a child!\n");
	
	
	continue;
	}
	
	
	}






}//while end
	
return 0;

}//program end




