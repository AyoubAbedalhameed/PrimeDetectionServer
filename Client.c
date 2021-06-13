/* NES416 Assignmet 5:  "multithreaded TCP client server program"
(Client_Code)  
 > Jun 6 2021
 > Ayoub Abedalhameed 124052
  */        


/*
Note:  
       > please compile Client.c file with the options: [-lpthread]. 
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



const int BUFFSIZE = 200 ; 
int MySock ; 
struct sockaddr_in ServerAddrStructure ; 


void* AskServer (void* arg)
{
printf("               @Request:%ld:  Thread Created,   ID=%d  \n", * (long *) arg, pthread_self()) ; 


long number =  * (long*) arg ; 


char sendbuff[BUFFSIZE]  ; 
char rcvbuff[BUFFSIZE]  ; 


memset(sendbuff, 0 , sizeof(sendbuff)) ;
memset(rcvbuff, 0 , sizeof(rcvbuff)) ;

sprintf(sendbuff, "%ld", number) ; 

//printf("@%ld: Send Buff = (%s)\n",pthread_self() ,sendbuff) ; 




if(write (MySock, sendbuff , strlen(sendbuff)) == -1 )
{perror("Error While Sending Buffer:"); pthread_exit((void*) -1) ;}


if(read(MySock, rcvbuff, sizeof(rcvbuff)) <= 0 )
{printf ("@Request(%ld): Error while reading socket number %d.\n", number,  MySock ) ; pthread_exit(NULL) ; } 

rcvbuff[strlen(rcvbuff)  ] = '\0' ; 

printf("(%s, %d):", inet_ntoa(ServerAddrStructure.sin_addr) ,(int) ntohs(ServerAddrStructure.sin_port) ) ; 
printf("%s\n", rcvbuff);



printf("               @Request:%ld:  Thread Terminates,   ID=%d  \n", * (long *) arg, pthread_self()) ; 

    pthread_exit(NULL) ; 
}


int main (int argc, char** argv)
{





// Checking command-line arguments count. (Must be 2)
if (argc != 3) {printf("Invalid Argument Count, usag: Server <Listining Port>"); return -1 ; } 

/*
struct servent* myService ; 
myService = getservbyname(argv[2], "tcp" );
if(myService == NULL )
{printf("error in getservbyname function \n") ; return -1 ; }
*/
struct hostent* hostent_ptr ; 
 hostent_ptr =   gethostbyname(argv[1]) ; 
 if (hostent_ptr == NULL ) {herror("Error while getting host ip") ; return -1 ; }

//printf("Host Name is (%s)address is (%s) \n ", hostent_ptr->h_name ,  inet_ntoa( * (struct in_addr* )  *(hostent_ptr->h_addr_list) ) ) ; 


//struct sockaddr_in ClientAdd ; 



MySock = socket( AF_INET,  SOCK_STREAM, 0); 
if(MySock<0)
{perror("Error While Creating Client Socket%d"); return -1 ;}
printf("Socket Created Successfully, SocketNum= (%d)\n",MySock); 




// Intilaizing Server Socket Address Structure: 

memset(&ServerAddrStructure, 0 , sizeof(ServerAddrStructure)) ;
ServerAddrStructure.sin_family= AF_INET ;
ServerAddrStructure.sin_port=  htons(   atoi(argv[2])   ) ;  
ServerAddrStructure.sin_addr = ( * (struct in_addr* )  *(hostent_ptr->h_addr_list) ) ; 

//( * (struct in_addr* )  *(hostent_ptr->h_addr_list) )


printf ("Built Server Address: (%s)\n", inet_ntoa(ServerAddrStructure.sin_addr)) ; 




// Connecting To the Server: 
if ( connect(MySock, (struct sockaddr*) &ServerAddrStructure, sizeof(ServerAddrStructure)) < 0)
{perror("Error While Connecting to the Server: "); return -1 ; }

printf("Connected successfully to %s on port %d\n", inet_ntoa(ServerAddrStructure.sin_addr), (int) ntohs(ServerAddrStructure.sin_port)  ) ; 

//recv(MySock, rcvbuff, sizeof(rcvbuff), 0 );
//read (MySock, rcvbuff, sizeof(rcvbuff) ); 
//printf("from server:%s\n", rcvbuff) ; 
char bug ; 
long n1 ; 
long n2 ; 



pthread_t thread1 , thread2 ; 

bool flag ; 
while (1)
{

flag = false ; 
while(1) 
{
printf("\n\nPlease Enter Your Number or enter -1 to quit.\n--->"); 
scanf("%ld",&n1) ;  
if (n1 > 0 ) break ; 
if (n1 != -1 ) { printf("Please Enter a Positive Integer or -1 to quit.\n"); continue;}  flag = true ; break ;   } 

if(flag) break ; 

printf("You number is (%ld)\n", n1 ) ;  

n2 =  n1 + (long) 1 ; 

pthread_create(&thread1, NULL, AskServer, &n1) ; 
pthread_create(&thread2, NULL, AskServer, &n2) ; 

pthread_join(thread1, NULL) ; 
pthread_join(thread2, NULL) ; 


void* status1  ;  void* status2 ; 



//read (MySock, rcvBuff, sizeof(rcvBuff)); 
//printf("%s",rcvBuff) ; 





}
close(MySock) ; 
printf("\nClient Terminates ... Good Bye\n\n") ; 


return 0 ;   




}
