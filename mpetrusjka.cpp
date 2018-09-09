/**********************************************************************************

mPetrusjkaProc process class

This is a MOVEBODY structure controlling a Petrusjka RC servo robot
 
*********************************************************************************/

#include "mpetrusjka.h"
#include "aniglue.h"
#include "clock.h"
#include <math.h>
#include <string.h>



/* Relation betwen servo position units, controller dependent, and Anima angles - resolution 2ms for ezusb */
SERVOPROP petr_servoprop2[8]={
   {(char*)"RKNEE V",  590, 940, 590, 1, 11.7},
   {(char*)"RHIP V",   600, 940, 940, -1,11.7},
   {(char*)"LKNEE V",  560, 970, 970, -1,11.7},
   {(char*)"LHIP V",   580, 940, 580, 1, 11.7},
   {(char*)"UPPERBACK",520, 960, 520, 1, 8.66},
   {(char*)"RARM V",   530, 1010, 1010,-1,3.7},
   {(char*)"LARM V",   530, 1010,  530, 1,3.7},
   {(char*)"NONE",  0, 0, 0, 0, 0}
};

/* Relation betwen servo position units, controller dependent, and Anima angles - resolution 1ms */
SERVOPROP petr_servoprop1[8]={
   {(char*)"RKNEE V",  1180, 1880, 1180,  1, 23.4},
   {(char*)"RHIP V",   1200, 1880, 1880, -1, 23.4},
   {(char*)"LKNEE V",  1120, 1940, 1940, -1, 23.4},
   {(char*)"LHIP V",   1160, 1880, 1160,  1, 23.4},
   {(char*)"UPPERBACK",1040, 1920, 1040,  1, 17.3},
   {(char*)"RARM V",   1060, 2020, 2020, -1, 7.4},
   {(char*)"LARM V",   1060, 2020, 1060,  1, 7.4},
   {(char*)"NONE",     0, 0, 0, 0, 0}
};

/* Relation betwen servo position units, controller dependent, and Anima angles - resolution 0.25ms */
/* Version pre 2018 
SERVOPROP petr_servoprop_orig[8]={
   {(char*)"RKNEE V",  4720, 7520, 4720,  1, 93.6},
   {(char*)"RHIP V",   4800, 7520, 7520, -1, 93.6},
   {(char*)"LKNEE V",  4480, 3760, 3760, -1, 93.6},
   {(char*)"LHIP V",   4640, 7520, 4640,  1, 93.6},
   {(char*)"UPPERBACK",4160, 7680, 4160,  1, 69.2},
   {(char*)"RARM V",   4240, 8080, 8080, -1, 29.6},
   {(char*)"LARM V",   4240, 8080, 4240,  1, 29.6},
   {(char*)"NONE",     0, 0, 0, 0, 0}
};
*/

/* Relation betwen servo position units, controller dependent, and Anima angles - resolution 0.25ms */
/* Version september 2018 */
SERVOPROP petr_servoprop[8]={
   {(char*)"RKNEE V",  4720, 7520, 7250, -1, 93.6},
   {(char*)"RHIP V",   4800, 7520, 7520, -1, 93.6},
   {(char*)"LKNEE V",  4480, 7760, 4760,  1, 93.6},
   {(char*)"LHIP V",   4640, 7520, 4640,  1, 93.6},
   {(char*)"UPPERBACK",4160, 7680, 4160,  1, 93.2},
   {(char*)"RARM V",   4240, 8080, 8080, -1, 29.6},
   {(char*)"LARM V",   4240, 8080, 4240,  1, 29.6},
   {(char*)"NONE",     0, 0, 0, 0, 0}
};

// Sensitivity must be adjusted for dutycycle
#define STARTACTIVITY   1.4
#define RESETACTIVITY   0.5

// Constants from original ezusb code
//#define TIMECONSTANT    1.0
//#define SENSITIVITY     100

// Current constants used on Teensy 3.6
#define TIMECONSTANT    2.0
#define SENSITIVITY     75

/***************************************************************************/
long prevtick;   /* For timing of activity */
static float timeconstant = TIMECONSTANT;
static int    pulsectrlsensitivity = SENSITIVITY;
float gPulseActivity;

/* PETRUSKJA Marionett doll axes */
int  petr_nJoints=7;
int  petr_RobJoint[7]   ={ RKNEE, RHIP, LKNEE, LHIP, UPPERBACK, RARM, LARM};
int  petr_servonum[7]   ={    0,    1,    2,     3,      4,      5,    6   };
char petr_RobJointAx[7]={   'V',  'V',  'V',   'V',    'V',    'V',  'V'};

mPetrusjkaProc::mPetrusjkaProc(void){
	nJoints    = petr_nJoints;
	RobJoint   = petr_RobJoint;
	servonum   = petr_servonum;
	RobJointAx = petr_RobJointAx; 
	servoProperties = petr_servoprop;
	servoControl = (rcServoController *)new pololuservo();
	pulsecontrol = 0;
	lastpulsecount = PulseCount();
	activitylevel = 0.0;
	prevtick = TickCount();
	Message("mPetrusjkaProc");
};
 
void mPetrusjkaProc::setPortName(char * fn){
	servoControl->openport(fn);
};

int mPetrusjkaProc::setOption(const char * option,int value) {
  if (strncmp(option,"PULSECONTROL",12)==0) {
     pulsecontrol = value;
     return 0;
  } 
  if (strncmp(option,"TIMECONSTANT",12)==0) {
     timeconstant = value;
     return 0;
  } 
  if (strncmp(option,"SENSITIVITY",11)==0) {
     pulsectrlsensitivity = value;
     return 0;
  } 
  return -1;
}; 


int mPetrusjkaProc::exec(void) {
	// Check activity
	int running = isRunning();
	long tick = TickCount();
	float dt = (tick-prevtick)*0.001/timeconstant;
	bool freeze=false;

	if (pulsecontrol) {
		unsigned int pulsecount=PulseCount();
		activitylevel = activitylevel*exp(-dt);
		if (pulsecount != lastpulsecount) {
			lastpulsecount = pulsecount;
			activitylevel = activitylevel + pulsectrlsensitivity*dt;
			//printf("mPetrusjkaProc::Pulse count is %i,dt %f, activity %f \n",pulsecount,dt,activitylevel);
		}
		gPulseActivity = activitylevel;
		if ((!running) && (activitylevel>STARTACTIVITY)) {
			// Start from the beginning
			doStart();
		}
		if ((running) && (activitylevel<1)) {
			// Stop the dancer but keep music running
			//printf("mPetrusjkaProc::Pulse count is %i,dt %f, activity %f \n",pulsecount,dt,activitylevel);
			freeze = true;
		}
		if ((running) && (activitylevel<RESETACTIVITY)) {
			// Stop the dance
			doStop();
			doRewind();
		}
	}
	prevtick = tick;

	// SET SERVO POSITIONS !!
	if (!freeze) {
		mRCRobotProc::exec();
	}
	return 0;
}
