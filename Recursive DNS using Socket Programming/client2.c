#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFLEN 2048
#define MSGS 5	/* number of messages to send */

#define SERVER1_UDP_STATIC_PORT 21981

typedef struct node
{
    char key[6];
    char value[5];
    struct node* next;
}Node;


Node* Initialize_Client(Node* head);
Node* Insert(Node* head, char key[], char value[]);
char * Search_For_Key(char search_string[], Node *head);
void Client2_Server1_Communication(Node *head);

int main()
{
    Node *head;
    head=NULL;                              //Initially head will be NULL since linklist is empty
    head = Initialize_Client(head);         //Initialize the server
    Client2_Server1_Communication(head);
    return 1;
    
}



void Client2_Server1_Communication(Node *head)
{

        /*Code referred from rutgers*/

        struct sockaddr_in myaddr, remaddr;
        int fd, slen=sizeof(remaddr);
        char buf[BUFLEN];	                     /* message buffer */
        int recvlen;		                
        char search_string[10];
        char* searched_key;
        int addrlen;
        addrlen = sizeof(myaddr);
        struct sockaddr_in Client2_UDP_Dynamic_Port;
        struct hostent *Client2_IP = gethostbyname("nunki.usc.edu");
        struct hostent *Server1_IP = gethostbyname("nunki.usc.edu");
        int len;  
    
        /* create a socket */
        if ((fd=socket(AF_INET, SOCK_DGRAM, 0))<0)
        {
            perror("\nclient: socket");
            exit(1);
        }

        /* bind it to all local addresses and pick any port number */

        memset((char *)&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        memcpy(&myaddr.sin_addr, Client2_IP->h_addr_list[0], Client2_IP->h_length);

        if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
            perror("bind failed");
            //return 0;
            exit(1);
        }       

        /* now define remaddr, the address to whom we want to send messages */
        /* For convenience, the host address is expressed as a numeric IP address */
        /* that we will convert to a binary format via inet_aton */

        memset((char *) &remaddr, 0, sizeof(remaddr));
        remaddr.sin_family = AF_INET;
        remaddr.sin_port = htons(SERVER1_UDP_STATIC_PORT);
        memcpy(&remaddr.sin_addr, Server1_IP->h_addr_list[0], Server1_IP->h_length);

    
     if (inet_aton(inet_ntoa(remaddr.sin_addr),&remaddr.sin_addr)==0){
            fprintf(stderr, "inet_aton() failed\n");
            exit(1);
        }

        /* Sending the messages */
        
         printf("\nPlease Enter Your Search (USC, UCLA etc.):");                //booting up
         scanf("%s",search_string);   
         searched_key = Search_For_Key(search_string, head);                    //getting corresponding key to the search string

        
        //Upon receiving a valid keyword
         printf("Client 2 has received a request with search word %s, which maps to %s",search_string,searched_key);

        sprintf(buf, "%s", searched_key);

        if (sendto(fd, buf, strlen(buf), 0, (struct sockaddr *)&remaddr, slen)==-1) 
        {
            perror("sendto"); 
            exit(1);
        }
    
        len = sizeof(Client2_UDP_Dynamic_Port);
        if(getsockname(fd, (struct sockaddr *)&Client2_UDP_Dynamic_Port, (socklen_t*)&len) == -1)
        {
            perror("port unavailable");
            exit(1);
        }
    
        //After sending request to server 1 
        printf("\nThe Client 2 sends the request GET %s to the Server 1 with port number %d and IP address %s.",searched_key,SERVER1_UDP_STATIC_PORT,inet_ntoa(remaddr.sin_addr));
        printf("\nThe Client2's port number is %d and the IP address is %s.",ntohs(Client2_UDP_Dynamic_Port.sin_port),inet_ntoa( *((struct in_addr *)Client2_IP->h_addr) ));

        /* now receive an acknowledgement from the server, receiving reply from server 1 */
        recvlen = recvfrom(fd, buf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
            if (recvlen >= 0) 
            {
                buf[recvlen] = 0;	/* expect a printable string - terminate it */
                printf("\nThe Client 2 received the value POST %s from the Server 1 with port number %d and IP address %s.",buf,SERVER1_UDP_STATIC_PORT,inet_ntoa(remaddr.sin_addr));
                printf("\nThe Client2's port number is %d and IP address is %s\n",ntohs(Client2_UDP_Dynamic_Port.sin_port),inet_ntoa( *((struct in_addr *)Client2_IP->h_addr) ));
            }

        close(fd);
    
        /*Code referred from rutgers*/

}


//Link List operarions

//Seraching for a key word in link list
char * Search_For_Key(char search_string[], Node *head)
{
    
    Node *p;
    p=head;
    char *store_key;
    store_key = malloc(5*sizeof(char));
    
    while(p != NULL)
    {    
        if( strcmp(p->value, search_string) == 0 )
         {
            strcpy(store_key,p->key);
         }
        p=p->next;
    }
  
    return store_key;
}


// Initializing  the client: reading from file and storing in linklist 
Node* Initialize_Client(Node* head)
{
    int i,j; char ch;
    FILE *ifp;
    
    char key[6];
    char value[8];
    ifp = fopen("client2.txt", "r");             //Open client1.txt

        for(i=0;i<12;i++)
        {
                          
          for(j = 0; j < 4; j++)                //Storing the value
            {
                ch = fgetc(ifp);
              if(ch != ' ')
              {
                value[j] = ch;
              }
              else
              {
                value[j] = '\0';                //end the char array with '\0'
                break;                          //If only 3 characters break
              }
               value[4]='\0';                   //end the char array with '\0'
            }
            
            ch = fgetc(ifp);                    //space character between value and key
            
            for(j = 0; j < 5; j++)              //Storing the key
            {
                ch = fgetc(ifp);
                key[j] = ch;
            }

            key[5]='\0';                        //end the char array with '\0'
            
            ch = fgetc(ifp);                    // new line character

            head =  Insert(head,key,value);     //Call Insert function to insert key value pairs in Linklist
        }


        fclose(ifp);                            //Close the file
    
    return head;                                //Return the head of modified Linklist
}



//Inserting into a linklist
Node* Insert(Node* head, char key[], char value[])
{
  

    Node *new; Node *current;
    
    if(head == NULL)                            //If head is NULL the create head with the key-value pair
    {  
     head = malloc(sizeof(Node));
     strcpy(head->key,key);
     strcpy(head->value,value);
     head->next = NULL;
    }
    else
    {
        new = malloc(sizeof(Node));             //Create a new Node with key-value pair
        strcpy(new->key,key);
        strcpy(new->value,value);
        new->next = NULL;
        
        current = head;
            while( current->next != NULL)       //traverse linklist till the end
            {  
                current = current->next;
            }
        current->next = new;                    //Insert the new node at the end
    }    
    return head;
}
