#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>




#define BUFSIZE 2048
#define MAXDATASIZE 100 // max number of bytes we can get at once 

#define SERVER1_UDP_STATIC_PORT 21981
#define SERVER2_TCP_STATIC_PORT "22981"
#define SERVER2_TCP_STATIC_PORT_PRINT 22981

typedef struct node
{
    char key[6];
    char value[8];
    struct node* next;
}Node;


Node* Initialize_Server(Node* head);
Node* Insert(Node* head, char key[], char value[]);
void Print(Node* head);
Node * Client_Server1_Communication(Node *head);
char* Find_Value(  char buf[],Node* head);
char*  client1_server1_server2_communicate(  char key_from_s1[]); //new..
char*  client2_server1_server2_communicate(  char key_from_s1[]);

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// get port number
in_port_t get_in_port(struct sockaddr *sa)
{
 if(sa->sa_family == AF_INET) {
        return (((struct sockaddr_in*)sa)->sin_port);
    }

    return (((struct sockaddr_in6*)sa)->sin6_port);
}


int check_c1_c2=0;

int main()
{
    Node *head;
    head=NULL;                                              //Initially head will be NULL since linklist is empty
    head = Initialize_Server(head);                         //Initialize the server
    head = Client_Server1_Communication(head);              //Start the client1 - server1 communication
    return 1;
}


Node * Client_Server1_Communication(Node *head)
{
    
            /*Code referred from rutgers*/

            struct sockaddr_in myaddr;	                    /* our address */
            struct sockaddr_in remaddr;	                    /* remote address */
            socklen_t addrlen = sizeof(remaddr);	        /* length of addresses */
            int recvlen;			                        /* # bytes received */
            int fd;				                            /* our socket */
              char buf[BUFSIZE];	                    /* receive buffer */
              char buf1[BUFSIZE];	                /* receive buffer */
            struct hostent *Server1_IP = gethostbyname("nunki.usc.edu");
            char *value;
            char *value_received_from_server2;
            

            /* create a UDP socket */

            if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
                perror("\ncannot create socket");
                exit(1);
            }

            /* bind the socket to any valid IP address of nunki.usc.edu and a specific port 21981 */

            memset((char *)&myaddr, 0, sizeof(myaddr));
            myaddr.sin_family = AF_INET;
            myaddr.sin_port = htons(SERVER1_UDP_STATIC_PORT);
            memcpy(&myaddr.sin_addr, Server1_IP->h_addr_list[0], Server1_IP->h_length);

    
            if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
                perror("\nbind failed");
                exit(1);
            }

            //booting up
            printf("\nThe server 1 has UDP port number %d and IP address %s",SERVER1_UDP_STATIC_PORT,inet_ntoa( *((struct in_addr *)Server1_IP->h_addr) ));  

            /* now loop, receiving data and printing what we received */

            for (;;)
            {
                recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
                if (recvlen > 0)
                    buf[recvlen] = 0;
                else
                {
                    perror("recv");
                    exit(1);
                }

            //    printf("\n*******NEW REQUEST*******");
                printf("\n");
                check_c1_c2++;
                if(check_c1_c2%2 == 1)
                printf("\nServer 1 has received a request with key %s from client 1 with port number %d and IP address %s",buf,remaddr.sin_port,inet_ntoa(remaddr.sin_addr));
               else if(check_c1_c2%2 ==0)
                printf("\nServer 1 has received a request with key %s from client 2 with port number %d and IP address %s",buf,remaddr.sin_port,inet_ntoa(remaddr.sin_addr));

                value = Find_Value(buf,head);                                                           //buf holds key, find corresponding value

                if(strcmp(value,"invalid") == 0)                                                        //Server 1 does not contain that key (buf)
                {
                    
                    if(check_c1_c2%2==1)
                    {
                        value_received_from_server2=client1_server1_server2_communicate(buf);           //make a call to server 2 to get the value that maps to key (buf)
                    }
                    else if(check_c1_c2%2==0)
                    {
                        value_received_from_server2=client2_server1_server2_communicate(buf);           //make a call to server 2 to get the value that maps to key (buf)
                    }
                    strcpy(buf1,value_received_from_server2);
                    head = Insert(head, buf, buf1);                                                     // add the received value to server 1 data base
                  
                    
                    if (sendto(fd, buf1, strlen(buf1), 0, (struct sockaddr *)&remaddr, addrlen) < 0)    //sending value to client 1
                        perror("\nsendto");

                    if(check_c1_c2%2 == 1)
                    printf("\nThe Server 1, sent reply POST %s to Client 1 with port number %d and IP addrss %s", buf1,remaddr.sin_port,inet_ntoa(remaddr.sin_addr));
                    else if(check_c1_c2%2 == 0)
                    printf("\nThe Server 1, sent reply POST %s to Client 2 with port number %d and IP addrss %s", buf1,remaddr.sin_port,inet_ntoa(remaddr.sin_addr));

                    
                }
                else                                                                                    // Server 1 has the corresponding value to key (buf) 
                {
                    sprintf(buf,"%s",value);
                    if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, addrlen) < 0)      //sending value to client 1
                        perror("sendto");
                    if(check_c1_c2%2 == 1)
                    printf("\nThe Server 1 sends the reply POST %s to Client 1 with port number %d and IP address %s",buf,remaddr.sin_port,inet_ntoa(remaddr.sin_addr)); //Send it to client 1
                    else if(check_c1_c2%2 == 0)
                    printf("\nThe Server 1 sends the reply POST %s to Client 2 with port number %d and IP address %s",buf,remaddr.sin_port,inet_ntoa(remaddr.sin_addr)); //Send it to client 1
                      
                    /*Code referred from rutgers*/


                }
        }

    return head;
}







char*  client1_server1_server2_communicate(  char key_from_s1[])
{

    /*Referred code from beej's client-server architecture*/
    
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in Server1_Server2_TCP_Dynamic_Port;
    int rv; int len;
	char s[INET6_ADDRSTRLEN];
    char value_to_get_from_server2[5];
    char *buf=malloc(8 * sizeof(char));
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
    
    /*Referred code from beej's client-server architecture*/

	if ((rv = getaddrinfo("nunki.usc.edu", SERVER2_TCP_STATIC_PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "\ngetaddrinfo: %s", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("\nserver1: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("\nserver1: connect");
			continue;
		}

		break;
	}
    
    len = sizeof(Server1_Server2_TCP_Dynamic_Port);
        if(getsockname(sockfd, (struct sockaddr *)&Server1_Server2_TCP_Dynamic_Port, (socklen_t*)&len) == -1)
        {
            perror("fff");
        }
    
    
	if (p == NULL) {
		fprintf(stderr, "\nserver1: failed to connect");
		exit(1);
	}

    //extracting the IP address of client's program
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

    
	freeaddrinfo(servinfo); // all done with this structure

    strcpy(value_to_get_from_server2,key_from_s1);
    printf("\nThe Server 1 sends the request GET %s to the Server 2.",value_to_get_from_server2);
    printf("\nThe TCP port number is %d and IP address is %s",ntohs(Server1_Server2_TCP_Dynamic_Port.sin_port),inet_ntoa(Server1_Server2_TCP_Dynamic_Port.sin_addr));

    if (send(sockfd,value_to_get_from_server2, 5, 0) == -1)             //send the key from Server 1 to Server 2 
				perror("send");

    
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {      //buf holds the value corresponging to key (value_to_get_from_server2)
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("\nServer 1 received the value %s from server 2 with port number %d and IP address %s",buf,SERVER2_TCP_STATIC_PORT_PRINT,s);
	close(sockfd);                                                     //closing the TCP connection between server1 and server 2
    printf("\nThe Server 1 closed the TCP connection with the Server 2");

    return buf; 
    
    /*Referred code from beej's client-server architecture*/

}



char*  client2_server1_server2_communicate(  char key_from_s1[])
{
    /*Referred code from beej's client-server architecture*/

    int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in Server1_Server2_TCP_Dynamic_Port;
    int rv; int len;
	char s[INET6_ADDRSTRLEN];
    char value_to_get_from_server2[5];
    char *buf=malloc(8 * sizeof(char));
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
    
	if ((rv = getaddrinfo("nunki.usc.edu", SERVER2_TCP_STATIC_PORT, &hints, &servinfo)) != 0) {
		fprintf(stderr, "\ngetaddrinfo: %s", gai_strerror(rv));
		exit(1);
	}

	// loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((sockfd = socket(p->ai_family, p->ai_socktype,p->ai_protocol)) == -1) {
			perror("\nserver1: socket");
			continue;
		}

		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("\nserver1: connect");
			continue;
		}

		break;
	}
    
    len = sizeof(Server1_Server2_TCP_Dynamic_Port);
        if(getsockname(sockfd, (struct sockaddr *)&Server1_Server2_TCP_Dynamic_Port, (socklen_t*)&len) == -1)
        {
            perror("error");
        }
    
    
	if (p == NULL) 
    {
		fprintf(stderr, "\nserver1: failed to connect");
		exit(1);
	}
    //extracting the IP address of client's program
	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),s, sizeof s);

    
	freeaddrinfo(servinfo); // all done with this structure

    strcpy(value_to_get_from_server2,key_from_s1);
    printf("\nThe Server 1 sends the request GET %s to the Server 2.",value_to_get_from_server2);
    printf("\nThe TCP port number is %d and IP address is %s",ntohs(Server1_Server2_TCP_Dynamic_Port.sin_port),inet_ntoa(Server1_Server2_TCP_Dynamic_Port.sin_addr));

    if (send(sockfd,value_to_get_from_server2, 5, 0) == -1)             //send the key from Server 1 to Server 2 
				perror("send");

    
	if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {      //buf holds the value corresponging to key (value_to_get_from_server2)
	    perror("recv");
	    exit(1);
	}

	buf[numbytes] = '\0';

	printf("\nServer 1 received the value %s from server 2 with port number %d and IP address %s",buf,SERVER2_TCP_STATIC_PORT_PRINT,s);
	close(sockfd);                                                     //closing the TCP connection between server1 and server 2
    printf("\nThe Server 1 closed the TCP connection with the Server 2");
    return buf; 
    
    /*Referred code from beej's client-server architecture*/

}



char * Find_Value(  char buf[], Node* head)
{
    Node *curr;
    curr = head ;
    
    while(curr!=NULL)
    {
     if(strcmp(curr->key,buf) == 0)
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
    ifp = fopen("server1.txt", "r");        //Open server1.txt

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
