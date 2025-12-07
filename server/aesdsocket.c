// JDH new file
#include <stdio.h>
#include <syslog.h>	// for logging
#include <signal.h>	// for signal handling
#include <unistd.h>	// JDH DEBUG - remove - Unix Standard - for sleep()

// make a loop variable that is robust
volatile sig_atomic_t keep_listening = 1;

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
}

// aesdsocket 
int main (int argc, char** argv)
{
   printf("Hello ECEA 5305!\n");
   syslog(LOG_INFO, "Welcome to the jungle");	// JDH make this something real

   // setup termination conditions
   signal(SIGINT, signal_handler);
   signal(SIGTERM, signal_handler);

   // only exit this loop if SIGINT or SIGTERM received
   while (keep_listening)
   {
      printf("gibberish\n");	// JDH remove
      sleep(1);			// JDH remove
   }
}
