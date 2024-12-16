/*######################################
# University of Information Technology #
# IT007 Operating System #
# <Your name>, <your Student ID> #
# File: factorial.c #
######################################*/
#include <stdio.h>
int main()
{
long long int i, num, j;
j=1;
printf("Enter the number: ");
scanf ("%d", &num );
for (i=1; i<=num; i++)
j=j*i;
printf("The factorial of %d is %d\n", num, j);
return 0;
}