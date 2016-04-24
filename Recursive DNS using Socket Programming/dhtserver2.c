/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>



  // the port users will be connecting to
#define MAXDATASIZE 100
#define BACKLOG 10	 // how many pending connections queue will hold
#define BUFSIZE 2048
//#define P23 "8976"




#define SERVER2_TCP_STATIC_PORT "22981"
#define SERVER2_TCP_STATIC_PORT_PRINT 22981
#define SERVER3_TCP_STATIC_PORT "23981"
#define SERVER3_TCP_STATIC_PORT_PRINT 23981



typedef struct node
{
    char key[6];
    char value[8];
    struct node* next;
}Node;

Node* Initialize_Server(Node* head);
Node* Insert(Node* head, char key[], char value[]);
void Print(Node* head);
char * Find_Value(  char buf[], Node* head);
char* client1_server2_server3_communicate(  char key_from_s2[]);
char* client2_server2_server3_communicate(  char key_from_s2[]);



void sigchld_handler(int s)
{
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


in_port_t get_in_port(struct sockaddr *sa)
{
 if(sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}



int check_c1_c2=0;
    
int main(void)
{
    
    /*Referred code from beej's client-server architecture*/
        
	int sockfd, new_fd;                             // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr;            // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;
      char buf[MAXDATASIZE];	                /* receive buffer */
    int numbytes;
    char *value;
    char *value_received_from_server3;
      char buf1[BUFSIZE];	               /* receive buffer */
    struct hostent *Server2_IP = gethostbyname("nunki.usc.edu");

    Node *head;
    head=NULL;                                      //Initially head will be NULL since linklist is empty
   
    head = Initialize_Server(head);                 //Initialize the server
 
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL,SERVER2_TCP_STATIC_PORT, &hints, &servinfo)) != 0) { //PORT where the server 1 will connect to
		fprintf(stderr, "\ngetaddrinfo: %s", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) 
    {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,
				p->ai_protocol)) == -1) {
			perror("\nserver: socket");
			continue;
		}

		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
				sizeof(int)) == -1) {
			perror("\nsetsockopt");
			exit(1);
		}

		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("\nserver: bind");
			continue;
		}

		break;
	}

	if (p == NULL)  
    {
		fprintf(stderr, "\nserver: failed to bind\n");
		return 2;
	}
    
	freeaddrinfo(servinfo); // all done with this structure

	if (listen(sockfd, BACKLOG) == -1) 
    {
		perror("\nlisten");
		exit(1);
	}

	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) 
    {
		perror("\nsigaction");
		exit(1);
	}

    //booting up
    printf("\nThe server 2 has TCP port number %d and IP address %s\n",SERVER2_TCP_STATIC_PORT_PRINT,inet_ntoa( *((struct in_addr *)Server2_IP->h_addr) ));                             


    //listen
	while(1) 
    {
        
		sin_size = sizeof their_addr;
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("\naccept");
			continue;
		}

        //extracting sender's server1's IP
		inet_ntop(their_addr.ss_family,get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
                
         //receive new
        if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1) 
        {
	       perror("recv");
            exit(1);
        }
        

        buf[5]='\0';
       // printf("\n*********NEW REQUEST*********");
        check_c1_c2++;


        printf("\nServer 2 has received a request with key %s from the Server 1 with port number %d and IP address %s",buf,ntohs(get_in_port((struct sockaddr *)&their_addr)),s ); //buf holds key
        
		if (!fork()) 
        {       // this is the child process
                close(sockfd); // child doesn't need the listener
                value = Find_Value(buf,head);                                                  //server 2 will search in his memory to get the value corresponding to the key (buf)
                if(strcmp(value,"invalid") == 0)                                               //server 2 does not have the key
                {
                    if(check_c1_c2%2 == 1)
                    {
                        value_received_from_server3=client1_server2_server3_communicate(buf); //send key to server 3...
                    }
                    if(check_c1_c2%2 == 0)
                    {
                        value_received_from_server3=client2_server2_server3_communicate(buf); //send key to server 3...
                    }
                    
                    strcpy(buf1,value_received_from_server3); //buf1 holds value
                    head = Insert(head, buf, buf1);                                           //save to linklist, key and value sending
      
                
                    if (send(new_fd,buf1, 7, 0) == -1)                                       //send
                        perror("send");
                    printf("\nThe Server 2, sent reply POST %s to Server 1 with port number %d and IP address %s",buf1,ntohs(get_in_port((struct sockaddr *)&their_addr)),s); //buf1 holds value
                    close(new_fd);
                    exit(0);

                }
                else
                {
                  
                    if (send(new_fd,value, 7, 0) == -1)                                      //send
                        perror("send");
                      printf("\nThe Server 2 sends the reply POST %s to Server 1 with port number %d and IP address %s",value,ntohs(get_in_port((struct sockaddr *)&their_addr)),s);
                    close(new_fd);
                    exit(0);
                }
		}
		close(new_fd);  // parent doesn't need this
	
    /*Referred code from beej's client-server architecture*/

    }

	return 0;
}







char* client1_server2_server3_communicate(  char key_from_s2[])
{
    
        /*Referred code from beej's client-server architecture*/

        int sockfd, numbytes;  
        char *buf=malloc(8 * sizeof(char));
        struct addrinfo hints, *servinfo, *p;
        int rv;
        char s[INET6_ADDRSTRLEN];
        char value_to_get_from_server3[5];
        struct sockaddr_in Server2_Server3_TCP_Dynamic_Port;
        int len;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo("nunki.usc.edu", SERVER3_TCP_STATIC_PORT, &hints, &servinfo)) != 0) //PORT where the server 2 will connect to
        {  
            fprintf(stderr, "\ngetaddrinfo: %s", gai_strerror(rv));
            exit(1);
        }

        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) 
        {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
            {
                perror("\nserver2: socket");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
            {
                close(sockfd);
                perror("\nserver2: connect");
                continue;
            }

            break;
        }
    
       //dynamically assigning the port
        len = sizeof(Server2_Server3_TCP_Dynamic_Port);
        if(getsockname(sockfd, (struct sockaddr *)&Server2_Server3_TCP_Dynamic_Port, (socklen_t*)&len) == -1)
        {
            perror("error");
        }
    

        if (p == NULL) {
            fprintf(stderr, "\nserver2: failed to connect");
            exit(1);
        }

        //extracting sender's server1's IP
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

        freeaddrinfo(servinfo); // all done with this structure

    
        strcpy(value_to_get_from_server3,key_from_s2);
        printf("\nThe Server2 sends the request GET %s to the Server 3",value_to_get_from_server3);
        printf("\nThe TCP port number is %d and IP address is %s",ntohs(Server2_Server3_TCP_Dynamic_Port.sin_port), inet_ntoa(Server2_Server3_TCP_Dynamic_Port.sin_addr));

        if (send(sockfd,value_to_get_from_server3, 5, 0) == -1)    //send
                    perror("send");


        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {   //recv
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';

        printf("\nServer 2 received the value %s from Server 3 with port number %d and IP address %s",buf,SERVER3_TCP_STATIC_PORT_PRINT,s);
        close(sockfd);
        printf("\nThe Server 2 closed the TCP connection with the Server 3");
        return buf;
    
        /*Referred code from beej's client-server architecture*/


}



char* client2_server2_server3_communicate(  char key_from_s2[])
{
        /*Referred code from beej's client-server architecture*/

        int sockfd, numbytes;  
        char *buf=malloc(8 * sizeof(char));
        struct addrinfo hints, *servinfo, *p;
        int rv;
        char s[INET6_ADDRSTRLEN];
        char value_to_get_from_server3[5];
        struct sockaddr_in Server2_Server3_TCP_Dynamic_Port;
        int len;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if ((rv = getaddrinfo("nunki.usc.edu", SERVER3_TCP_STATIC_PORT, &hints, &servinfo)) != 0) //PORT where the server 2 will connect to
        {  
            fprintf(stderr, "\ngetaddrinfo: %s", gai_strerror(rv));
            exit(1);
        }

        // loop through all the results and connect to the first we can
        for(p = servinfo; p != NULL; p = p->ai_next) 
        {
            if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) 
            {
                perror("\nserver2: socket");
                continue;
            }

            if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) 
            {
                close(sockfd);
                perror("\nserver2: connect");
                continue;
            }

            break;
        }
    
    //dynamically assigning the port
        len = sizeof(Server2_Server3_TCP_Dynamic_Port);
        if(getsockname(sockfd, (struct sockaddr *)&Server2_Server3_TCP_Dynamic_Port, (socklen_t*)&len) == -1)
        {
            perror("error");
        }
    

        if (p == NULL) {
            fprintf(stderr, "\nserver2: failed to connect");
            exit(1);
        }

        //extracting sender's server1's IP
        inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

        freeaddrinfo(servinfo); // all done with this structure

    
        strcpy(value_to_get_from_server3,key_from_s2);
        printf("\nThe Server2 sends the request GET %s to the Server 3",value_to_get_from_server3);
        printf("\nThe TCP port number is %d and IP address is %s",ntohs(Server2_Server3_TCP_Dynamic_Port.sin_port), inet_ntoa(Server2_Server3_TCP_Dynamic_Port.sin_addr));

        if (send(sockfd,value_to_get_from_server3, 5, 0) == -1)         //send
                    perror("send");


        if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {   //recv
            perror("recv");
            exit(1);
        }

        buf[numbytes] = '\0';

        printf("\nServer 2 received the value %s from Server 3 with port number %d and IP address %s",buf,SERVER3_TCP_STATIC_PORT_PRINT,s);
        close(sockfd);
        printf("\nThe Server 2 closed the TCP connection with the Server 3");
        return buf; //new...
    
        /*Referred code from beej's client-server architecture*/


}




char * Find_Value(  char buf[], Node* head)
{
    Node *curr;
    curr = head ;
    
    while(curr!=NULL)
    {
     if(strcmp(curr->key,buf) == 0)
     {
         return curr->value;
     }
    curr=curr->next;
    }
    
    return "invalid";
    
}








Node* Initialize_Server(Node* head)
{
    int i,j; char ch;
    FILE *ifp;
    char key[6];
    char value[8];
    ifp = fopen("server2.txt", "r");        //Open server2.txt

        for(i=0;i<4;i++)
        {
                          
          for(j = 0; j < 5; j++)            //Storing the key
            {
                ch = fgetc(ifp);
                key[j] = ch;
            }

            key[5]='\0';                    //end the char array with '\0'

            ch = fgetc(ifp);                //space character between key and value
      
            for(j = 0; j < 7; j++)          //Storing the value
            {
                ch = fgetc(ifp);
                value[j] = ch;   
            }

            value[7]='\0';                  //end the char array with '\0'

            ch = fgetc(ifp);                // new line character

            head =  Insert(head,key,value); //Call Insert function to insert key value pairs in Linklist
        }


        fclose(ifp);                        //Close the file
    
    return head;                            //Return the head of modified Linklist
}

Node* Insert(Node* head, char key[], char value[])
{

    Node *new; Node *current;
    
    if(head == NULL)                        //If head is NULL the create head with the key-value pair
    {  
     head = malloc(sizeof(Node));
     strcpy(head->key,key);
     strcpy(head->value,value);
     head->next = NULL;
    }
    else
    {
        new = malloc(sizeof(Node));         //Create a new Node with key-value pair
        strcpy(new->key,key);
        strcpy(new->value,value);
        new->next = NULL;
        
        current = head;
            while( current->next != NULL)   //traverse linklist till the end
            {  
                current = current->next;
            }
        current->next = new;                //Insert the new node at the end
    }    
    return head;
}

void Print(Node* head)
{
    Node *current = head;
    while(current != NULL)
    {
        printf("\n%s%s",current->key, current->value);
        current=current->next;
    }
}


