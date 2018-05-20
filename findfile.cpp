#include "anisysd.h"
#include <string.h>
#include <unistd.h>
#include <stdio.h>

int findFile( char *fName,VOLID_TYPE * fileSpec){
char dir[128],name[64];
int pos;
   *fileSpec='X';  // Dummy volume ID
   pos=strlen(fName);
   while ( (pos>0) && ( fName[pos-1]!='/' )) pos--;
   strcpy(name,fName+pos);
   strncpy(dir,fName,pos);
   dir[pos]=0;
printf("%s - %s\n",dir,name);
   if (pos>0) {
//      chdir(dir);
      }
return 0																													;
}
