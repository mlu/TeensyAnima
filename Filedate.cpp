// filedate.c
// UNIX version 98/05/23
#include <sys/stat.h>
#include "anisysd.h"
#include <unistd.h>
#include <string.h>

#ifdef __cplusplus
extern "C" int Message(char * msg);
#endif

long getFDate(char * fName,VOLID_TYPE * volID);


long getFDate(char * fName,VOLID_TYPE * volID) {
	return 0;
}

