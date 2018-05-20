#include <Arduino.h>
#include <Audio.h>
#include "tsoundproc.h"
#include "clock.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>


/*
Different players:

Teensy Audio sound player
*/


AudioPlaySdWav           playWav1;

//AudioOutputAnalog        audioOutput;
AudioOutputI2S           audioOutput;
AudioControlSGTL5000     sgtl5000_1;

AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);



TeensySoundProc::TeensySoundProc(char * sndfile){
	strncpy(soundfile,sndfile,255);
	status = ACTIVE;
	playstate='s';
};

TeensySoundProc::~TeensySoundProc(){

};


int TeensySoundProc::reset(void){
	/* Sound process is autokill, must be restarted from caller */
	clean();
	return 0;
};

int TeensySoundProc::exec(void){
	/* Handle status changes - FIXTHIS */
	if ( reader->status == QUIT) {
		clean(); 
		return 0; 
	};	
	/* Sound process is finished, clean */
	if ( (!playWav1.isPlaying())&&(thisTime-startTime>100)&& (playstate =='r') ){ 
		clean();
	}	
	/* Clock started but not sound, start it */
	else if (isRunning()&& (playstate =='s')) {
		play(thisTime-startTime);
	}
	/* Clock stopped but sound active, stop sound */
	else if (!isRunning()&& (playstate =='r')) {
		stop();
	}
	return 0;
};
	
int TeensySoundProc::activate(void){
	nextTime=EventTime=startTime;//thisTime;
	status=ACTIVE;
	priority=0;
	return 0;
};
	
void TeensySoundProc::clean(void){
	if (status != KILL ) {
		stop();
		status = KILL;
		/* signal quit to play thread */
		setPlaystate('q');
		Message("Exit Sound");
	}
};

/**	Low level sound playing
*		play(starttime)
*		stop()
*/

void TeensySoundProc::play(int dsec){
	char trimstr[64];
	int start,hh, mm, ss, frac;
	start = dsec;
	frac=start%100;
	start=start/100;
	ss=start%60;
	start=start/60;
	mm=start%60;
	hh=start/60;
	sprintf(trimstr,"play:%02i:%02i:%02i.%02i",hh,mm,ss,frac);
	Message(soundfile);
	Message(trimstr);
	playWav1.play(soundfile);
	setPlaystate('r');
};

void TeensySoundProc::stop(){
	playWav1.stop();
	setPlaystate('s');
	Message("Stop Sound");
}

bool TeensySoundProc::soundActive(){
	return playstate!='s';	/* not sleeping */
}

void TeensySoundProc::setPlaystate(unsigned char ps) { 
		playstate=ps;
};


