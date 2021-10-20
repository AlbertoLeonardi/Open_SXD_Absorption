// Utility Functions :: Statistical Functions
//
// Provides Statistics Tools

#ifndef _INCLUDE_UTILITY_STATISTICS
#define _INCLUDE_UTILITY_STATISTICS 1

#include <App_Environment.h>

#include <Signal_Warning.h>

// RANDOM NUMBER GENERATOR FUNCTIONS

void   Statistics_InitializeGlobalRandomGenerator(long *Seed);				// Initialize the global random generators

#if _UTILITY_STATISTICS_USE_PNG_RNG=='Y'

#include "External_pcg_basic.h"

#define _CONST_RAND_MAX_x32				4294967295
#define _CONST_RAND_MAX_x32_over_x32	0.999999999767169356346130371093750000000 // This is exact! // The PNG random generator defines the bit in a 32byte memory location, therefore the max unsigned integer value is 2^32-1 (the -1 is because of the 0 vlue is included)

long   Statistics_UniformRandomInteger_x32();

#else

#if RAND_MAX==32767
#define _CONST_RAND_MAX_x32   2147483647
#define _CONST_1_RAND_MAX_x32 0.00000000046566128730773925781250// This is exact!
long   Statistics_UniformRandomInteger_x32();
#endif

#endif

int     Statistics_RandomBoolean_01();

double Statistics_UniformRandomReal_In01();									// return [0.0 .. 1.0]
long   Statistics_UniformRandomInteger_InRange(long range);					// return [0 .. range]

// PROBABILITY DISTRIBUTION FUNCTIONS

struct Statistics_Distribution
{
	double  mean;
	double  sd;
	double  variance;
	double  mu;									// uniform distribution: distribution's center
	double  sigma;								// uniform distribution: distribution's range (+-sigma)
	double  max;								// max value (given an integral probability for unlimited distributions, i.e., normal, lognormal, ...)
	double  min;								// min value (given an integral probability for unlimited distributions, i.e., normal, lognormal, ...)
	char    type;								// c. constant, u. uniform, n. normal, l. lognormal
};

double ErrorFunction(double x);
double ErrorFunction_Invers(double x);

int    Statistics_IntegralProbability_Bounds(struct Statistics_Distribution *Data, double Probability);

int    Statistics_DistributionParameters_IdentifyType(struct Statistics_Distribution *Data, const char buffer[]);
const char  *Statistics_DistributionParameters_GetNameType(struct Statistics_Distribution *Data);

int    Statistics_DistributionParameters_MuSigma_to_MeanSDvariance(struct Statistics_Distribution *Data);
int    Statistics_DistributionParameters_MeanSD_to_MuSigmaVariance(struct Statistics_Distribution *Data);

double Statistics_PDF_SkewNormal(double x, double Xi, double Omega, double Alfa);

#endif