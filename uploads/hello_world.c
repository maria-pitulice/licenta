#include <stdio.h>
int main() {
   float x=10;
   if(0 < x)
   {
   x=x+1;
   x--;
   printf("Hello, World! %f", x);
   x++;
   }
   return 0;
}