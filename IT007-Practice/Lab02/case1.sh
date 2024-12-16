#!/bin/sh
echo "Are you alright, yes or no?"
read alright

case "$alright" in 
  "yes"|"y"|"Yes"|"YES" )   echo "I'm glad to hear so";;
  [nN]*)   echo "I'm sorry to hear so..." 
           echo "What can I do for you?" ;;
  * )      echo "What do you mean?";;
esac
exit 0 



