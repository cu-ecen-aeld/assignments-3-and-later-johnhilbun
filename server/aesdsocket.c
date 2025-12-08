// JDH new file
#include <stdio.h>
#include <stdlib.h>	// for exit()
#include <syslog.h>	// for logging
#include <signal.h>	// for signal handling
#include <unistd.h>	// JDH DEBUG - remove - Unix Standard - for sleep()
#include <netdb.h>	// getaddrinfo
#include <string.h>	// memset
#include <arpa/inet.h>	// inet_ntop

// make a loop variable that is robust
volatile sig_atomic_t keep_listening = 1;

//#define BUFFER_SIZE 1024	// JDH pulled from a hat, keep an eye on this
#define BUFFER_SIZE 20480	// JDH long_string is 19,999 on disk : try it
int server_socket_fd;		// JDH made global - signal handler involved
int client_socket_fd;		// JDH made global - signal handler involved
FILE *outfile_writing = NULL;		// /var/tmp/aesdsocketdata
FILE *outfile_reading = NULL;		// /var/tmp/aedsocketdata

// handle SIGTERM or SIGINT signals and stop the program
void signal_handler(int the_signal)
{
   // JDH TODO:
   // - complete any open connection operations - ?huh?
   // - close any open sockets
   // - delete the file /var/tmp/aesdsocketdata

   if (the_signal == SIGINT)
   {
      syslog(LOG_INFO,"Caught signal %d, exiting", the_signal);
   }
   else if (the_signal == SIGTERM)
   {
      syslog(LOG_INFO,"Caught signal %d, exiting", the_signal);
   }
   keep_listening = 0; // the key here is we only register for SIGTERM and SIGINT
   if (server_socket_fd != -1)
      close(server_socket_fd);
   if (client_socket_fd != -1)
      close(client_socket_fd);
   if (outfile_writing != NULL)
      fclose(outfile_writing);
   if (outfile_reading != NULL)
      fclose(outfile_reading);
}

// aesdsocket 
int main (int argc, char** argv)
{
   printf("Hello ECEA 5305!\n");
   syslog(LOG_INFO, "Welcome to the jungle");	// JDH make this something real

   // setup termination conditions
   //signal(SIGINT, signal_handler);
   //signal(SIGTERM, signal_handler);
   struct sigaction sa;
   sa.sa_handler = signal_handler;
   sigaction(SIGINT, &sa, NULL);
   sigaction(SIGINT, &sa, NULL);

   // from Sockets video ~10:30 
   int status;
   //int server_socket_fd;
   //int client_socket_fd;
   struct addrinfo hints;
   struct addrinfo *servinfo;

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;	// video shows AF_UNSPEC
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;

   // some preliminary getaddrinfo details
   if ((status = getaddrinfo(NULL, "9000", &hints, &servinfo)) != 0)
   {
      printf ("\n### getaddrinfo() error:%s:\n", gai_strerror(status));
      exit (1); // JDH exit (-1 instead?)
   }

   // create the socket
   server_socket_fd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
   if (server_socket_fd == -1)
   {
      perror("socket");
      freeaddrinfo(servinfo);
      exit (1); // JDH exit (-1 instead?)
   }

   // bind
   if (bind (server_socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
   {
      perror("bind");
      close(server_socket_fd);
      freeaddrinfo(servinfo);
      exit (1); // JDH exit (-1 instead?)
   }

   // open a file, truncating if it exists
   FILE *outfile_writing;   // /var/tmp/aesdsocketdata
   FILE *outfile_reading; // /var/tmp/aedsocketdata
   outfile_writing = fopen ("/var/tmp/aesdsocketdata", "w"); // w : truncate if it exists
   if (!outfile_writing)
   {
      perror("fopen");
      close(server_socket_fd);
      freeaddrinfo(servinfo);
      exit (1); // JDH exit (-1 instead?)
   }


   // only exit this loop if SIGINT or SIGTERM received
   while (keep_listening)
   {
      // printf("gibberish\n");
      // sleep(1);
 
      // listen
      if ( listen (server_socket_fd, 1) == -1)	// backlog set to 1 pending allowed
      {
         perror("listen");
         close(server_socket_fd);
         freeaddrinfo(servinfo);
         exit (1); // JDH exit (-1 instead?)
      }

      // accept - blocks here waiting on a connection
      struct sockaddr_storage received_addr;
      socklen_t received_addr_size;
      received_addr_size = sizeof received_addr;
      client_socket_fd = accept(server_socket_fd, (struct sockaddr *)&received_addr, &received_addr_size); // JDH arg guesses
      if (client_socket_fd == -1)
      {
         perror("accept");
         close(server_socket_fd);
         freeaddrinfo(servinfo);
         exit (1); // JDH exit (-1 instead?)
      }

      // do something
      void *address;
      char client_ip[INET6_ADDRSTRLEN];
      int client_port;
      struct sockaddr_in *ipv4 = (struct sockaddr_in *)&received_addr;
      address = &(ipv4->sin_addr);
      client_port = ntohs(ipv4->sin_port); // network to host order
      // JDH SUN FILE *outfile; // /var/tmp/aesdsocketdata

      // inet_ntop(received_addr.ss_family, address, client_ip, sizeof client_ip);
      printf("Accepted connection from %d\n", client_port);
      syslog(LOG_INFO,"Accepted connection from %d", client_port);
  

      /* JDH SUN Move this outside the loop
      // open a file, truncating if it exists
      outfile = fopen ("/var/tmp/aesdsocketdata", "w"); // w : truncate if it exists
      if (!outfile)
      { 
         perror("fopen");
         close(server_socket_fd);
         freeaddrinfo(servinfo);
         exit (1); // JDH exit (-1 instead?)
      }
      */

      // read packets until we see a carriage return
      int cr = 0;
      int still_sending = 0;
      char buffer[BUFFER_SIZE];	// JDH pulled 1024 out of a hat. Keep an eye on this
      while (!cr)
      {
         int bytes_read = read(client_socket_fd, buffer, BUFFER_SIZE-1);
         if (bytes_read <= 0)
         {
            printf("--> read 0 or less bytes");
            break;
         }
         buffer[bytes_read] = '\0'; // null terminate
         fwrite(buffer, 1, bytes_read, outfile_writing);
	 fflush(outfile_writing); // JDH SUN make sure the data goes to disk

         if (strchr(buffer, '\n') != NULL)
         {
            printf("--> found <cr>");
	    cr = 1;
         }
         // JDH SUN fclose(outfile); // to make sure all is written to disk

         outfile_reading = fopen ("/var/tmp/aesdsocketdata", "r"); // JDHJDH open for read
         if (!outfile_reading)
         { 
            perror("fopen for sending");
            close(server_socket_fd);
            freeaddrinfo(servinfo);
            exit (1); // JDH exit (-1 instead?)
         }

         // send
         printf ("--> going to send !%s!\n", buffer);
         while ((still_sending = fread(buffer, 1, BUFFER_SIZE, outfile_reading)) > 0)
         {
            printf ("--> write() still_sending %d", still_sending);
            write(client_socket_fd, buffer, still_sending);
         }
         printf ("--> after write()");
         fclose(outfile_reading);
      } // huh?
   }

   freeaddrinfo(servinfo);	// JDH at some point, free up servinfo - but what about signals?
}
