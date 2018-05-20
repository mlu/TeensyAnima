/******************************************
DOS and UNIX file opening and buffer allocation

**************************************/

#include <stdlib.h>

#include "movelib.h"
#include "movetext.h"

#include <SD.h>
#include <SPI.h>


#define INLEN 10000
// system dependent directory handling
extern int findFile( char *fName,VOLID_TYPE * fileSpec);


MoveText *  InitFile(char * fname,VOLID_TYPE * fileSpec) {
	File infile; /* present data file */
	long size,filelen;
	char * tb;
	MoveText * mt = NULL;
	char outs[100];

	//  findFile(fname,fileSpec);
	sprintf(outs," Loading file %s",fname);
	Message(outs);
	infile = SD.open(fname, FILE_READ);
	if (!infile) {
	Message("File not found"); 
		return 0;
	}

	filelen=infile.size();
	if (filelen == 0) {
		Message("File length 0"); 
		infile.close();
		return 0;
	}

	tb=(char *)malloc(filelen+2);
	if (tb) {
		size=infile.read(tb,filelen);
		sprintf(outs,"File size read %li on disk %li ",size,filelen);
		Message(outs);
		tb[size]=0;
		mt=new MoveText();
		mt->autoclean=true;
		mt->setBuffer(tb,size);
	}
	infile.close();
	return mt;
	
}




