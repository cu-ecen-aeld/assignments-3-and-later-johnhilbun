#!/bin/ash
# JDH SUN : original line above is <she><bang>/bin/bash
# JDH created

#echo "Let the weekend begin!"

# check for command line parameters
if [ "$#" != 2 ]; then
   echo "Error: Two command-line parameters required."
   echo "Usage: $0 path_to_directory search_string"
   exit 1
fi

# assign the command line parameters to variable (order specific)
filesdir="$1"
searchstr="$2"

# check if first argument is a directory
if [ -d "$filesdir" ]; then
   echo "will search in $filesdir"
else
   echo "$filesdir is not a directory"
   exit 1
fi

# search in filesdir and count the files
# search in each file and count the number of hits on searchstring
# print out both values

# count the overall number of files. Include symbolic links or 'conf' is missed
X=$(find -L "$filesdir" -type f | wc -l)  # some syntax from Microsoft Copilot

# Count the number of hits to string <searchstr> in all searched files
# -r recursive : no symbolic links
# -R recursive : follow symbolic links
Y=$(grep -R "$searchstr" "$filesdir" | wc -l)

# Print results
echo "The number of files are $X and the number of matching lines are $Y"

#echo "Let the weekend continue!"

