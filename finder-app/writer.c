#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <errno.h>

int main (int argc, char** argv) {
   printf ("writer 1.0\n");
   FILE *fp;

   // check for required arguments
   if (argc != 3 ){
      printf ("writer requires two arguments:\n");
      printf ("   full-path-to-a-file\n");
      printf ("   text-string-to-write\n");
      return 1;
   }

   // Setup logging
   openlog(NULL, LOG_CONS | LOG_PERROR, LOG_USER);

   // Say what we are going to do
   printf ("writing %s to %s\n", argv[2], argv[1]); // JDH remove
   syslog(LOG_DEBUG, "writing %s in file %s", argv[2], argv[1]);

   // Create/open argv[1] in write mode
   fp = fopen(argv[1], "w");

   // Return error if there was a problem
   if (fp == NULL) {
      syslog(LOG_ERR, "Error opening file: %s", strerror(errno));
      closelog();
      return 1;
   }

   // Write argv[2] to the file
   fprintf (fp, "%s", argv[2]);

   // Close the file
   fclose(fp);
   // Close the log
   closelog();

   return 0;
}
