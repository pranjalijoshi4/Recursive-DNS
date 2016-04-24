a. Name:            Pranjali Rajiv Joshi
b. Date:	    April 2015

c. Assignment details
1.) The 2 clients and 3 servers read their respective text files and store the search term-key and key-values respectively in a link list datastructure. The servers will start listening at static ports,server 1: 21981,server 2: 22981, server 3: 23981, where server 1 is the designated server.
2.) A user will enter a search term, the client will search in link list to get the corresponding key. The client will have a dynamic port to make a request to static port of server1, 21981 to find the associated value for the key.
3.) The server1 will search in its link list, if it finds the key, it will return the corresponding value to client’s dynamic port. If not it will create a dynamic port to make a request to the static port of server2, 22981.
4.) The server2 will search in its link list, if it finds the key, it will return the corresponding value to server1’s dynamic port, server1’s static port will further return it to the client’s dynamic port. If not it will make a dynamic port to request to server3, 23981
5.) The server3 will search in its link list, it will find the key, it will return the value to server2’s dynamic port, server2’s static port will further return it to the server1’s dynamic port, server1’s static port will further return it to the client’s dynamic port.
6.) Client to server communication is done using bidirectional UDP connection, whereas the server-server connections use TCP. The servers will keep listening to the static port.
7.) Everytime a server receives a value from another server, a value not present in it’s link list, the key-value is added at the end of the server’s link list.
8.) This assignment thus replicates the working of recursive DNS.
9.) There are 2 clients, client1 will always make the first request.
10.) Server 1, 2 have 2 TCP (Dynamic) ports (for requesting server 2 and 3 respectively) for clients 1 and 2. Server 1 has a UDP port (Static) associated with client. Server 2 has a UDP port (Static) associated with server 1.  Server 3 has only 1 TCP port (Static) associated with server 2. Clients have 2 separate UDP ports (Dynamic)

d. Code file:
1. client1.c        This will find the key from the link list that is mapped to the search key term entered by the user.
2. client2.c        This will find the key from the link list that is mapped to the search key term entered by the user.
3. dhtserver1.c     It is the designated server listening on port 21981, that the client1.c and client2.c  will contact first with the key. This program will return the value     corresponding to the requested key mapped to the search term entered by the user, if present. If it’s not present it will contact dhtserver2.c. Once it gets the value from dhtserver2.c, dhtserver1.c stores key-value in it’s link list. 
4. dhtserver2.c     It listens on port 22981. This will receive the key from dhtserver1.c. This program will return the value corresponding to the requested key received from dhtserver1.c, if present. If it’s not present it will contact dhtserver3.c. Once it gets the value from dhtserver3.c, dhtserver2.c stores key-value in it’s link list.
5. dhtserver3.c     It listens on port 23981. This will receive the key from dhtserver2.c. This program will return the value corresponding to the requested key received from dhtserver2.c. It is assumed that the key is present in at least one of the servers.

e. Running the code files
The following order should be followed
make - to compile all the code files
execute files as - ./dhtserver1.out, ./dhtserver2.out, ./dhtserver3.out, ./client1.out, ./client2.out
Alternatively,
1) Compile dhtserver1.c, dhtserver2.c, dhtserver3.c, client1.c, cient2.c inorder (for example, gcc -o dhtserver1.out dhtserver.c -lsocket -lnsl -lresolv)
2) Execute dhtserver1.c, dhtserver2.c, dhtserver3.c, client1.c, cient2.c inorder (for example, ./dhtserver1.out)
3) client1.c should make a request
4) client2.c should make a request
5) Terminate dhtserver1.c, dhtserver2.c, dhtserver3.c by ctrl^C, client1.c and client2.c will get terminated by themselves.
6) Repeat from step 2 for further testing.

f. The format of all the messages exchanged
A new line is put after every new request.

g. Idiosyncrasy
The clients 1 and 2 will not be able to request a simultaneous request. It is assumed that client 1 will make first request, then will client 2. If a request is made for a key not present in any of the servers then the programs will not work.

h. Reused code
Rutgers and Beej - Client Server architecture, Used code is commented in the code files.