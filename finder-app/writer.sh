#!/bin/bash
# JDH created

#echo "Let the writing begin!"

# check for command line parameters
if [ "$#" != 2 ]; then
   echo "Error: Two command-line parameters required."
   echo "Usage: $0 full_path_of_file_to_create string_to_inser_in_file"
   exit 1
fi

# assign the command line parameters to variable (order specific)
fullfilename="$1"
writestr="$2"

# create the new directory (including parent directories). -p prevents errors if already exists
# Microsoft CoPilot for syntax
# Get just the directory part
dironly=$(dirname "$fullfilename")
# Create directory if doesn't exist
mkdir -p "$dironly"
if [ $? != 0 ]; then
   echo "Not able to create directory $dironly"
   exit 1
#else
   #echo "Will create $dironly"
   #echo "Will write :$writestr: to :$fullfilename:"
fi

# > creates or contatenates
# fullfilename has the path and file name
echo "$writestr" > "$fullfilename"

#echo "Let the writing continue!"

