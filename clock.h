#ifndef CLOCK_H
#define CLOCK_H
/***********************************************************/
/* Clock module header for anima move system               */
/* Magnus Lundin aug. 91                                   */
/***********************************************************/
enum {INTERNALCLOCK,MIDIBEAT,MIDITIMECODE,FIXED_RATE};
extern long thisTime;
extern long baseTime;
extern long nextTime;
extern int MM;
extern int framecount;
extern long midioffset;
extern int frameRate;

int doClock(void);

int isRunning();
int doSyncClock(void);
int doStartClock(void);
int doStopClock(void);
int doSetClock(long newTime);
int doSetMetronome(int newMM);
int doSetClockMode(int newMode);
#ifdef Macintosh
void setMIDISync(int newsync);// 0 - MidiManager Internal, 1 MidiManager External
#endif
long getTime(void);
long TickCount();


#endif

