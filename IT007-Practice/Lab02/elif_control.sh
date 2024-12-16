echo "Are you sleepy yet? Yes or no only"
read check
if [ "$check" = "yes"  ]; then
 echo "Goodnight"
elif [ " $check"  = "no"  ]; then 
 echo "You gotta sleep early"
else "Invalid input, enter yes or no"
exit 1
fi
exit 0 
