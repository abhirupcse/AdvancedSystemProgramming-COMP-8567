#include <stdio.h>
#include <stdlib.h> 
void main() {
int *ar,n;
printf("\nEnter the number of elements in the array\n");
scanf("%d",&n);
ar=malloc(n*sizeof(int));
for(int i=0;i<n;i++)
{
printf("Enter element %d\n", i);
scanf("%d",&ar[i]);
}
printf("\nThe elements of the array are\n");
for(int i=n - 1;i>=0;i--)
{
printf("\n The elements at index %d is %d",i ,*(ar+i));
}
}