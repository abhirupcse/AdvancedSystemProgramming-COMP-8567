#include <stdio.h>
 #include <stdlib.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <unistd.h>
 
int main(int argc, char *argv[]) {
   int fd = open("/home/kumar2d/asp/check2.txt",O_RDWR);
 
  int buf[20];
   lseek(fd, 2, SEEK_SET);
   int long n = lseek(fd, -5, SEEK_CUR);
 
  printf("offset %d\n",n); //-3                                                                      
 
  n = read(fd, buf, 10); //no char read because at -3 position nothing available
   printf("%s\n", buf);//NOTHING PRINT
   printf("%d\n",n);//0
 
  n = lseek(fd, 0, SEEK_CUR);
   printf("offset %d\n",n);//0
 
  n = write(fd, "buf", 2); //no char write because at -3 position nothing available
   printf("%ld\n",n);//0
 

   close(fd);
 }


