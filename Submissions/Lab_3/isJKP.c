#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[])
{
    char* string1= argv[1];
    char* string2 = argv[2];

    //char string1[]= "happy";
    //char string2[] = "gLad";

    //convert both to lower
    for(int i = 0; i < strlen(string1); i++)
           string1[i] = tolower((char)string1[i]);
      for(int i = 0; i < strlen(string2); i++)
           string2[i] = tolower((char)string2[i]);
    


    //longer word--> kangaroo 
    char* kangaroo;
    char* joey;

    if(strlen(string1)==strlen(string2))
    {
              printf(">>0\n");
              return 0;
    }
    else if(strlen(string1) > strlen(string2))
    {
            kangaroo=string1;
            joey=string2;
    }
    else
    {
           kangaroo=string2;
            joey=string1;
    }
      
    //compare
    int i=0;
    int j=0;
    while(j<strlen(joey) && i<strlen(kangaroo))
    {
        if(kangaroo[i]==joey[j])
        {
            i++;
            j++;
        }
        else 
        {
            i++;
        }
    }
    
    if(j<strlen(joey))
    {
        printf(">>%d\n",0);
    }
    else
    {
        printf(">>%d\n",1);
    }
    return 0;
}