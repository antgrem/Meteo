#ifndef __PROGNOZ_H
#define __PROGNOZ_H

#include "main.h"

typedef enum 
{
	PR_NOTREADY = 0,
	PR_FULLSUN,
	PR_SUN,
	PR_CLOUDS,
	PR_FULRAIN,
	PR_RAIN,
	PR_STORM, 
	PR_ERROR
} PR_RESULT;

PR_RESULT Prognoz(Messure_DataTypeDef Data);

#endif
