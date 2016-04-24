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

#define MAXDATASIZE 100
#define BACKLOG 10	 // how many pending connections queue will hold


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
char * Find_Value( char buf[], Node* head);

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



int main(void)
{
    
        /*Referred code from beej's client-server architecture*/

        int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
        struct addrinfo hints, *servinfo, *p;
        struct sockaddr_storage their_addr; // connector's address information
        socklen_t sin_size;
        struct sigaction sa;
        int yes=1;
        char s[INET6_ADDRSTRLEN];
        int rv;
         char buf[MAXDATASIZE];	/* receive buffer */
        int numbytes;
        char *value;
        struct hostent *Server3_IP = gethostbyname("nunki.usc.edu");

        Node *head;
        head=NULL;                              //Initially head will be NULL since linklist is empty
        head = Initialize_Server(head);         //Initialize the server
  

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE; // use my IP

        if ((rv = getaddrinfo(NULL,SERVER3_TCP_STATIC_PORT, &hints, &servinfo)) != 0) 
        {
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

            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,sizeof(int)) == -1) {
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
            fprintf(stderr, "\nserver: failed to bind");
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

        printf("\nThe server 3 has TCP port number %d and IP address %s\n",SERVER3_TCP_STATIC_PORT_PRINT,inet_ntoa( *((struct in_addr *)Server3_IP->h_addr) ));                             //booting up

    
    
        while(1) 
        {  

            sin_size = sizeof their_addr;
            new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
            if (new_fd == -1) 
            {
                perror("\naccept");
                continue;
            }

            inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr),s, sizeof s);
            
            if ((numbytes = recv(new_fd, buf, MAXDATASIZE-1, 0)) == -1)  //recev
            {  
                perror("\nrecv");
                exit(1);
            }

            buf[numbytes]='\0';  //buf holds the key
         //   printf("\n*********NEW REQUEST*********");
            printf("\nThe Server 3 has received a request with the key %s from the Server 2 with port number %d and IP address %s",buf,ntohs(get_in_port((struct sockaddr *)&their_addr)),s);

            if (!fork()) 
            { // this is the child process
                close(sockfd); // child doesn't need the listener
                value = Find_Value(buf,head);  //server 2 will search in his memory to get value07 
               
                /* if(strcmp(value,"invalid") == 0) //will never occur
                printf("\nServer 3 does not contain that key"); //will never occur*/
                
                
                if (send(new_fd,value, 7, 0) == -1)                         //send
                    perror("\nsend");
                printf("\nThe Server 3 sends the reply POST %s to Server 2 with port number %d and IP address %s",value,ntohs(get_in_port((struct sockaddr *)&their_addr)),s);
                close(new_fd);
                exit(0);
            }
            close(new_fd);  // parent doesn't need this
        }

        return 0;
    
        /*Referred code from beej's client-server architecture*/

}


char * Find_Value(  char buf[], Node* head)
{
    Node *curr;
    curr = head ;
    
    while(curr!=NULL)
    {
     if(strncmp(curr->key,buf,6) == 0)
         return curr->value;
        
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
    ifp = fopen("server3.txt", "r");        //Open server3.txt

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


