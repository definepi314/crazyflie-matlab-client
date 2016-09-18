/* File     : sfun_cflie_simple.cpp
 * Abstract :
 *
 *
 *
 *
 *
 *
 */

#include <iostream>
#include "mex.h"
#include "CCrazyflie.h"
using namespace std;

#define S_FUNCTION_LEVEL 2
#define S_FUNCTION_NAME sfun_cflie_simple

#include "simstruc.h"

#define IS_PARAM_DOUBLE(pVal) (mxIsNumeric(pVal) && !mxIsLogical(pVal) &&\
!mxIsEmpty(pVal) && !mxIsSparse(pVal) && !mxIsComplex(pVal) && mxIsDouble(pVal))

/*===================*
 *S-function methods *
 *===================*/

#define MDL_CHECK_PARAMETERS
#if defined(MDL_CHECK_PARAMETERS)  && defined(MATLAB_MEX_FILE)
/*
 * Check to make sure that each parameter is 1-d and positive
 */
static void mdlCheckParameters(SimStruct *S)
{
    const mxArray *pVal0 = ssGetSFcnParam(S,0);

    if ( !IS_PARAM_DOUBLE(pVal0)) {
        ssSetErrorStatus(S, "Parameter to S-function must be a double scalar");
        return;
    } 
}
#endif

/* Function: mdlInitializeSizes ===========================================
 * Abstract:
 *
 *
 */
static void mdlInitializeSizes(SimStruct *S)
{    
    ssSetNumSFcnParams(S,1); //Numer of expected parameters
    #if defined(MATLAB_MEX_FILE)
    if (ssGetNumSFcnParams(S) == ssGetSFcnParamsCount(S)) {
        mdlCheckParameters(S);
        if (ssGetErrorStatus(S) != NULL) {
            return;
        }
    } else {
        return; /* Parameter mismatch will be reported by Simulink */
    }
    #endif
    
    ssSetSFcnParamTunable(S,0,0);
    ssSetNumContStates(S,0);
    ssSetNumDiscStates(S,0);
    
    int NUM_INPUT_PORTS = 1;
    if(!ssSetNumInputPorts(S,NUM_INPUT_PORTS)) return;
    
    for (int i=0; i<NUM_INPUT_PORTS; i++) {
        ssSetInputPortWidth(S,i,4);
        ssSetInputPortDirectFeedThrough(S, i, 1);
        //ssSetInputPortRequiredContiguous(S,i,1);
    }

    int NUM_OUTPUT_PORTS = 1;
    if(!ssSetNumOutputPorts(S,NUM_OUTPUT_PORTS)) return;
    ssSetOutputPortWidth(S,0,4); 
    ssSetOutputPortDataType(S,0,SS_DOUBLE);
    
    ssSetNumSampleTimes(S,1);
    ssSetNumRWork(S,0);
    ssSetNumIWork(S,0);
    ssSetNumPWork(S,3); // reserve element in the pointers vector to store a C++ object
    ssSetNumModes(S,0);
    ssSetNumNonsampledZCs(S,0);
    
    ssSetSimStateCompliance(S, USE_CUSTOM_SIM_STATE);
    
    ssSetOptions(S,0);
}

/* Function: mdlInitializeSampleTimes =====================================
 * Abstract:
 *
 *
 */ 
static void mdlInitializeSampleTimes(SimStruct *S)
{
    ssSetSampleTime(S,0, mxGetScalar(ssGetSFcnParam(S,0)));
    ssSetOffsetTime(S,0, 0.0);
    ssSetModelReferenceSampleTimeDefaultInheritance(S);
}

#define MDL_START   /* Change to #undef to remove function */
#if defined(MDL_START)
/* Function: mdlStart =====================================================
 * Abstract:
 *
 *
 */
static void mdlStart(SimStruct *S)
{   
    int_T i;
    int_T nInputPorts = ssGetNumInputPorts(S);
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);             
    
    CCrazyRadio *crRadio = new CCrazyRadio("radio://0/80/250K");
    ssGetPWork(S)[0] = crRadio;
    
    bool initDone = false;
 //   ssGetPWork(S)[2] = initDone;
    if(crRadio->startRadio()){
        CCrazyflie *cflieCopter = new CCrazyflie(crRadio);
        cflieCopter->setSendSetpoints(true);
        ssGetPWork(S)[1] = cflieCopter;
         
   //     ssGetPWork(S)[2] = cflieCopter->cycle();
        /* Unlock motors*/
        cflieCopter->setThrust(0); 
    }
    else {
        mexPrintf("\n Radio could not be started!");
        ssSetErrorStatus(S, "Radio could not be started!");
    }
    ssPrintf("Done Initializing");   
   
   // usleep(100);
}
#endif /* MDL_START */

/* Function: mdlOutputs ===================================================
 * Abstract:
 *
 *
 */
static void mdlOutputs(SimStruct *S, int_T tid)
{
    double *output = (double *)ssGetOutputPortSignal(S,0);
    //output[0] = 5;
    UNUSED_ARG(tid);
    
    bool initDone = ssGetPWork(S)[2];
    // Read input port signals
    int_T nInputPorts = ssGetNumInputPorts(S);
    InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
    int_T nu = ssGetInputPortWidth(S,0);
    //ssPrintf("Port width: %d\n", nu);
    
    int_T thrust = *uPtrs[0];
    int_T roll = *uPtrs[1];
    int_T pitch = *uPtrs[2];
    int_T yaw = *uPtrs[3];
    
    CCrazyflie *Copter = (CCrazyflie *) ssGetPWork(S)[1];
    if(Copter->cycle()){
       
        // Send command
       Copter->setThrust(thrust);
       Copter->setRoll(roll);
       Copter->setPitch(pitch);
       Copter->setYaw(yaw);
       Copter->setSendSetpoints(true);
       //ssPrintf("Sending command ---> thrust: %d, roll: %d, pitch: %d, yaw: %d\n",thrust, roll, pitch, yaw);
        
       // Get sensor data
       int tr = Copter->thrust();
       float r = Copter->roll();
       float p = Copter->pitch();
       float y = Copter->yaw();
       //ssPrintf("Sensor data ---> thrust: %d, roll: %F, pitch: %F, yaw: %F\n",tr, r, p, y);   
        //usleep(50);
        
       //output[0] = Copter->thrust();
       output[0] = Copter->roll();
       output[1] = Copter->pitch();
       output[2] = Copter->yaw();
       output[3] = Copter->batteryLevel();
       
      // usleep(100);
             
    }

}

/* Function: mdlTerminate =================================================
 * Abstract:
 *
 *
 */
static void mdlTerminate(SimStruct *S)
{
    CCrazyRadio *radio = (CCrazyRadio *) ssGetPWork(S)[0];
    //CCrazyflie *copter = (CCrazyflie *) ssGetPWork(S)[2];
    
    delete radio;
    //delete copter;
}

/*=============================*
 * Required S-function trailer *
 *=============================*/

#ifdef MATLAB_MEX_FILE      /* Is this file being compiled as a MEX-file? */
#include "simulink.c"       /* MEX-file interface mechanism */
#else
#include "cg_sfun.h"        /* Code generation registration function */
#endif
