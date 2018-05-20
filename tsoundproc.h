#ifndef TSNDPROC_H
#define TSNDPROC_H
/*********************************************************************************
Real time kernel

Teensy Adio (PSJR) Sound player Processes for Anima
*********************************************************************************/
#include "kernel.h"
#include "rtproc.h"
#include "rmovproc.h"


class TeensySoundProc : public RTProcess {

public:
    long 	EventTime          /*LocalEventTime,*/   /* NextLocalEventTime*/;
    TeensySoundProc(char * sndfile);
    ~TeensySoundProc();
    rMoveProc * reader;
    int reset(void);
    int exec(void);
    int activate(void);
    void clean(void);
    void setPlaystate(unsigned char ps);
    unsigned getPlaystate(){return playstate;};
    void waitStateChange();
private:
    char soundfile[256];
    void play(int start);
    void stop();
    bool soundActive();
    char playstate;
};
#endif

