/* NES416 Assignmet 5:  "multithreaded TCP client server program"
(Server_Code)  
 > Jun 6 2021
 > Ayoub Abedalhameed 124052
*/        

/*
Note:  
       > please compile Server.c file with the options: [-lpthread] & [-lm]. 
       > Use Hostnames and Port numbers, Service Names Not Supported.   
*/



#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include<stdbool.h>
#include <netdb.h>
#include <pthread.h> 
#include <time.h>
#include <math.h>

#define MAX_THREADS 3 


pthread_t threadsList[MAX_THREADS] ; 
pthread_t coreThread ; 
struct sockaddr_in  ClientAdd ; 
int NewSock ; 

struct data 
{ long start, end,  number ; int index ; }; 

struct result 
{long Result ; double Time ; };

const int BUFFSIZE = 50 ; 

void* isPrime (void * arg) 
{

    long start = ((struct data*) (arg)) -> start ; 
    long end =   ((struct data*) (arg)) -> end ; long number =  ((struct data*) (arg)) -> number ; 
printf("               @%ld:(%d): Processing the set(%d,%d) \n", number, pthread_self(),  start, end  ) ; 

long* MyStatus = malloc(sizeof(long)) ; 
*MyStatus = -1 ; 
for (int i = start ; i <= end ; i++) 
if (number %  i == 0 ) 
{/*printf("               @%ld: Factor %d returned\n",number ,  i ) ;*/ *MyStatus = i ; break ; }

printf("               @%ld:(%d): Thread %d Terminating ..  \n", number, pthread_self(),  ((struct data*) (arg)) ->index ) ; 

 free(arg) ; 

pthread_exit(MyStatus) ; 
    }  








void* Core (void* Core_arg)
{

       clock_t time ; 

char sendbuff[BUFFSIZE]  ; 

long Number = *(long*) (Core_arg) ; 
printf("               @%ld:  Core Thread Created .. \n", Number) ; 


//free(Core_arg) ;


printf("               @%ld: Creating %d Threads .. \n", Number, MAX_THREADS) ; 

bool flag  = true ; 
/*       ******************      Creating Threads            ******************* */  

struct data* my_data ; long x = 0 ;    long n = sqrt (Number) - 1 ; long sum = 2 ;

time = clock() ; 

for (int i = 0 ; i < MAX_THREADS ;  i++)
{
my_data = malloc(sizeof(struct data )) ; my_data -> number = Number ; 
(my_data-> start) = sum  ;   x = n/(MAX_THREADS-i) ; my_data->end = my_data-> start + x -1 ; sum = sum + x ;  my_data->index = i+1; 

pthread_create(&threadsList[i], NULL , isPrime , my_data ) ;  n -= x ; 
}  



void* status ; 

struct result*  localResult ;  localResult = malloc(sizeof(struct result)) ; 
localResult->Result = -1 ;  

for (int i = 0 ; i < MAX_THREADS ;  i++)
{ pthread_join( threadsList[i], &status ) ;
 // printf("Status %d: %ld\n ", i, * (long*) status) ; 
      
  if(  *(long*) status != -1 && flag) {flag = false ; localResult->Result =   * (long*) status ; } } 


printf("               @%ld: local result = %ld\n", Number, localResult->Result ) ; 
time = clock() - time ; 
localResult->Time= ((double)time)/CLOCKS_PER_SEC;

//printf("@Core_thread:%ld-> result = %ld\n",Number,localResult.Result ) ; 

if(localResult->Result == -1 )
sprintf(sendbuff, "The number (%ld) is Prime.     [in %f s]  \n",Number, localResult->Time) ; 
else 
sprintf(sendbuff, "The number (%ld) is not Prime, the first founded factor is: %d.    [in %f s]",Number, localResult->Result, localResult->Time) ; 

sendbuff[strlen(sendbuff)] = '\0' ; 
printf("               @Corethread:%ld--> Sending the result to the client\n%s\n", Number, sendbuff) ; 


if(write (NewSock, sendbuff , strlen(sendbuff)) == -1 )
{perror("Error While Sending Buffer:"); pthread_exit((void*) -1) ;}

 

printf("               @Corethread:%ld   Terminating ..  \n", Number)  ; 
    pthread_exit(NULL) ;  
}



int servClient (void)
{
  //  printf("@main_thread: ServClient Function is Running! \n") ; 
char rcvBuff [BUFFSIZE] ; 

long* n ; 


while (1)
{
memset(rcvBuff,0, sizeof(rcvBuff));


printf("\n               @main_thread: Waiting for client request at socket #%d\n\n", NewSock) ; 


if(read(NewSock, rcvBuff, sizeof(rcvBuff)) <= 0 )
{printf ("               @servClient: Error while reading socket number or fin received!%d.\n", pthread_self(), NewSock ) ; return -1 ; } 



rcvBuff[strlen(rcvBuff)] = '\0' ; 


if(  strcmp("exit", rcvBuff)  ==0  )   /* checking if client send exit, the server close 
                                              the R/W Socket but keeps the listen socket */ 
 {printf("\nReceived exit, Closing Connection...\n \n"); break ;} 

printf("Client:(%s, %d):", inet_ntoa(ClientAdd.sin_addr) ,(int) ntohs(ClientAdd.sin_port) ) ; 
printf("%s\n", rcvBuff);

n = malloc(sizeof(long)) ; 
*n = atol(rcvBuff) ; 


//printf("Numeric is (%ld)\n", *n) ; 
//Creating Core thread to Serve the received number: 
pthread_create(&coreThread, NULL , Core,  n) ;   

pthread_join(coreThread, NULL) ; 

}


return 0 ; 
}











int main (int argc, char** argv)
{


// Checking command-line arguments count. (Must be 1)
if (argc != 2 ) {printf("Invalid Argument Count, usag: Server <Listining Port>"); return -1 ; } 

if( (atoi(argv[1])) <= 45000) {printf("Please Choose Port Number Greater than 45000\n"); return -1; }
/*
struct servent* myService ; 
myService = getservbyname(argv[1], "tcp" );
if(myService == NULL )
{printf("error in getservbyname Function \n") ; return -1 ; }


printf("The service port# is: (%d)", ntohs( myService->s_port) ) ; 
*/

int ServerSock ; 

// Creating Socket: 
ServerSock = socket(AF_INET, SOCK_STREAM, 0 );
if(ServerSock<0) 
{ perror("Error While Creating Server Socket\n");   return -1 ;}
 printf("Socket Created Successfully\n"); 
int on = 1 ; 
setsockopt(ServerSock, SOL_SOCKET, SO_REUSEADDR  , (void*) &on ,     sizeof(1) )   ;    
setsockopt(ServerSock, SOL_SOCKET, SO_REUSEPORT  , (void*) &on ,     sizeof(1) )   ;    


// Initializing SockAddress Structure: 
struct sockaddr_in  ServerLocal ; 

memset( &ServerLocal, 0, sizeof(ServerLocal));
ServerLocal.sin_family = AF_INET; 
ServerLocal.sin_port=  htons(atoi(argv[1])) ;  
ServerLocal.sin_addr.s_addr= INADDR_ANY; 




//Binding Socket 
if (       bind(ServerSock, (struct sockaddr*) &ServerLocal, sizeof(ServerLocal))      <0)
    { perror("Error while binding address to the listening socket");  return -1; } 

// Switch Socket to the passive-listining State. 
if (listen(ServerSock, 5)<0)
     { perror("Error While Binding the Socket to the local address"); return -1 ; }
printf("Socket State switched To: PASSIVE_LISTENNING\n") ; 


// Declaring SockAdd Structure for the client. (used in accept())
memset( &ClientAdd, 0, sizeof(ClientAdd));
socklen_t ClientAddLength ; 


 
 // Acccepting Connections: 
while(1)
{


     printf("\n  .... Waiting for New Session ....\n") ;     

NewSock = accept(ServerSock, (struct sockaddr * ) &ClientAdd, &ClientAddLength);
if (NewSock<0)
     { perror("While Accepting Connection"); return -1 ; }
printf("Connection Accepted from: %s for Sock#:%d\n", inet_ntoa(ClientAdd.sin_addr), NewSock) ;
printf("\n __________________ NEW SESSION_____________________\n\n") ;


servClient() ; 

close(NewSock) ;  

}
close (ServerSock) ; 


return 0  ; 
}

