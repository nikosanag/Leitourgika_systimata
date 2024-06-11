#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <time.h>
#include <ctype.h>




int is_number(const char *str){
if (str == NULL || *str =='\0') {
return 0; //String is null or empty
}
while (*str){
if(!isdigit((unsigned char)*str)){
return 0;//found a non digit character 
}
str++;
}
return 1;//all characters are digits
}

int main(int argc, char *argv[])
{

 bool debug = false;
 char* HOST ;
 HOST = "os4.iot.dslab.ds.open-cloud.xyz";
 int PORT = 20241;
 char buf[1024]; 

 
 if(argc > 6)
 {
    perror("Error-Usage : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
    return 1;
 }
 if(argc >= 1)
 {
    for(int i=1; i<argc; i++)
    {
      if(strcmp(argv[i], "[--host")==0)
      { 
	char *text = argv[i+1];  
        char save = *(text+(strlen(argv[i+1])-1));
        int size = strlen(argv[i+1]); 
	 if(save != ']'){
	perror("Error-Usage : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
	return 1;
	}	
        argv[i+1][strlen(argv[i+1])-1] = '\0';        
	
        
	//if(argv[i+1]!=NULL) HOST = argv[i+1];//
	//else {
	//perror("Error-Usage : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
        //return 1;
	//}
	
        i++;
      }
      else if(strcmp(argv[i], "[--port")==0)
      {
        	
        if(argv[i+1][strlen(argv[i+1])-1]!=']'){
	perror("Error-Usage : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
	return 1;
	} 
        argv[i+1][strlen(argv[i+1])-1] = '\0';
        if(is_number(argv[i+1])==0){
	perror("Error-Usage : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
        return 1;
	}
	PORT = atoi(argv[i+1]); 
	i++;


      }
      else if(strcmp(argv[i], "[--debug]")==0)
        debug = true; 
      else
      {
        perror("Error-Usage : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
        return 1;   
      }
    }
 }


//connect to server
//CLIENT 
 
 
 
 int sock_fd = socket(AF_INET,SOCK_STREAM,0); 
 if (sock_fd < 0) 
 {
  perror("socket has failed to be created!");
  return -1;
 }


/////////////////////////////////////////////////////////////////////////
 
 
 
 struct sockaddr_in addr;//***
 struct hostent *hostp;

 hostp = gethostbyname(HOST); //IP fetching from the  name of the server. 

 addr.sin_family = AF_INET;
 addr.sin_port = htons(PORT);
 bcopy(hostp->h_addr_list[0] , &addr.sin_addr, hostp->h_length);

 if (connect(sock_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
 {
    perror("connect to the server went wrong");
    return -1;
 }




 //in this point we have established communication with server and we take commands from terminal and send them to server
 //and server send us back info that excersice has told us
 
 
 
 
 fd_set read_set;
 int ndfs = sock_fd;                                      //This is the first argument of select(), maximum file descriptor value
while(true)
{ 
    FD_ZERO(&read_set);                                   //We initialize 
    FD_SET(0,&read_set);                                  //Terminal
    FD_SET(sock_fd,&read_set);                            //socket

    int selections = select(ndfs + 1, &read_set, NULL, NULL, NULL);//
    if (selections < 0)
    {
        perror("select");
        return 1;
    }
    if (FD_ISSET(0, &read_set))                 //Data from terminal
    {
        if (fgets(buf, 1024, stdin) == NULL)
        {
            perror("fgets");
            return 1;
        }
        if(strcmp(buf,"exit\n")==0)
        {
            printf("Exiting...!\n");
            close(sock_fd);
            return 1;
        }   
        if(strcmp(buf,"help\n")==0)
        {
            printf("Type 'exit' to exit, 'get' to recover server data, 'N name surname reason' to request access to quarantine.If u do not type something correctly then message 'try again' will appear\n");
            continue;
        }
        if(debug)
        {    
          int i = 0;
          while(buf[i] != '\n')                //We remove \n to print '%s' without changing line
            ++i;
          buf[i] = '\0';
          printf("[DEBUG] sent '%s'\n",buf);       
        }
       if(write(sock_fd,&buf,strlen(buf) + 1) == -1) //writing to the server
       {
            perror("writing to the server went wrong!");
            return 1;
       } 
    }  
    else if(FD_ISSET(sock_fd, &read_set)) //We receive data from the server
    {
        char output[1024]; 
        int nbytes=read(sock_fd,&output,1024-1);

        if( nbytes == -1 )
        {
            perror("read");
            return 1;
        }
        output[nbytes-1] = '\0';                   
        if(debug)
            printf("[DEBUG] read '%s'\n",output);
        if(strcmp(output,"try again")==0 || strcmp(output,"invalid code")==0)
        {
            printf("%s\n",output);
            continue;
        }
        if(output[1] == ' ')      //if the second character is space the server sent a reply 
        {                           
            int flag;
            int light;
            int temp;
            int timestamp;
            char* token;
            const char s[2] = " ";

            token = strtok(output, s);  //strip into tokens
            flag = atoi(token);

            token = strtok(NULL, s);
            light = atoi(token);

            token = strtok(NULL, s);
            temp = atoi(token);

            token = strtok(NULL, s);
            timestamp = atoi(token);
        
            char* message;
            double temperature = (double)temp/100;
            time_t senttime = timestamp;
            struct tm *info;
            time(&senttime);
	    info = localtime( &senttime );
            char buffer[80];
	    strftime(buffer,sizeof(buffer),"%Y-%m-%d %H:%M:%S",info); 



            switch (flag)   //Searching which flag we received
            {
                case 0:
                    message = "boot";
                    break;
                case 1:
                    message = "setup";
                    break;
                case 2:
                    message = "interval";
                    break;
                case 3:
                    message = "button";
                    break;
                case 4:
                    message = "motion";
                    break;
                default:
                    message = "unknown flag";
                    break;
            }
            printf("---------------------------\n");
            printf("Latest event:\n");
            printf("%s (%d)\n", message, flag);
            printf("Temperature is: %.2f\n",temperature);
            printf("Light level is: %d\n",light);
            printf("Timestamp is: %s\n", buffer);
            printf("---------------------------\n");
        }
        else if(output[0] == 'A' && output[1] == 'C' && output[2] == 'K') // ACK reply
        {  
            printf("Response: '%s'\n",output);
        }
        else
            printf("Send verification code : '%s'\n",output);
    }
}
return 0;
}
