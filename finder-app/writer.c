#include <stdio.h>

int main (int argc, char** argv) {
   printf ("writer 1.0\n");
   if (argc != 3 ){
      printf ("writer requires two arguments:\n");
      printf ("   full-path-to-a-file\n");
      printf ("   text-string-to-write\n");
      return 1;
   }

   printf ("write %s in file %s\n", argv[2], argv[1]);

   return 0;
}
