// Utility Functions :: Statistical Functions
//
// Provides Statistics Tools

#include "Utility_Statistics.h"

// RANDOM NUMBER GENERATOR FUNCTIONS

void   Statistics_InitializeGlobalRandomGenerator(long *Seed)
{
	if (Seed[0] < 0) { Seed[0] = (long)clock(); }							// Seed with current time
	
	srand(Seed[0]);															// Standard c rand
	
#if _UTILITY_STATISTICS_USE_PNG_RNG=='Y'
	if (Seed[1]<0) pcg32_srandom((uint64_t)Seed[0], (uint64_t)&Seed[1]);	// PCG
	else           pcg32_srandom((uint64_t)Seed[0], (uint64_t)Seed[1]);		// PCG
#endif
	
	return;
}

#if _UTILITY_STATISTICS_USE_PNG_RNG=='Y'

long    Statistics_UniformRandomInteger_x32()
{
	return (long)pcg32_random();
}
int     Statistics_RandomBoolean_01()
{
	return pcg32_random() % 2;
}

double  Statistics_UniformRandomReal_In01()					// return [0.0 .. 1.0]
{
	return _CONST_RAND_MAX_x32_over_x32 * (double)ldexp((double)pcg32_random(), -32);
}
long    Statistics_UniformRandomInteger_InRange(long range)	// return [0 .. range]
{
	return (long)pcg32_boundedrand((uint32_t)(range+1));
}

#else

#if RAND_MAX==32767
long    Statistics_UniformRandomInteger_x32()
{
	long r;

	r = rand() % 2;
	r += rand() << 1;
	r += rand() << 16;
	
	return r;
}
#endif
int     Statistics_RandomBoolean_01()
{
	return rand() % 2;
}

double  Statistics_UniformRandomReal_In01()					// return [0.0 .. 1.0]
{
	return _CONST_1_RAND_MAX_x32 * (double)Statistics_UniformRandomInteger_x32();
}
long    Statistics_UniformRandomInteger_InRange(long range) // return [0 .. range]
{
	long r;

	long region = 1 + range;
	long chunk = ((long)RAND_MAX + 1) / region;
	long ChOut = chunk * region;

	do { r = (long)rand(); } while (r >= ChOut);

	return (r / chunk);
}

#endif

// PROBABILITY DISTRIBUTION FUNCTIONS

double ErrorFunction(double x)
{
	double const ERF_AF = (8.0*(_CONST_Pi - 3.0)) / (3.0*_CONST_Pi*(4.0 - _CONST_Pi));
	
	double  x2;
	double  erf;
	
	if ((x<FLT_EPSILON) && (x>-FLT_EPSILON)) return 0.0;
	
	x2 = x*x;
	erf = sqrt(1.0 - exp(-(x2)*(2.0*_CONST_2_Pi + ERF_AF*x2) / (1.0 + ERF_AF*x2)));
	
	if (x<0.0) erf *= -1.0;
	
	return (erf);
}
double ErrorFunction_Invers(double x) // @@@ "A handy approximation for the error function and its inverse" by Sergei Winitzki
{
	double const ERF_AF = (8.0*(_CONST_Pi - 3.0)) / (3.0*_CONST_Pi*(4.0 - _CONST_Pi));
	
	double  x2, Factor;
	double  i_erf;
	
	if ((x<FLT_EPSILON) && (x>-FLT_EPSILON)) return 0.0;
	
	x2 = 1.0 - x*x;
	Factor = _CONST_2_Pi / ERF_AF + 0.50*log(x2);
	i_erf = sqrt(sqrt(Factor*Factor - log(x2) / ERF_AF) - Factor);
	
	if (x<0.0) i_erf *= -1.0;
	
	return (i_erf);
}

int   Statistics_IntegralProbability_Bounds(struct Statistics_Distribution *Data, double Probability)
{
	switch ((*Data).type)
	{
	case 'c':
		(*Data).min = (*Data).mean;
		(*Data).max = (*Data).mean;
		break;
	case 'u':
		(*Data).min = (*Data).mean - (*Data).sigma;
		(*Data).max = (*Data).mean + (*Data).sigma;
		break;
	case 'n':
		if ((Probability<FLT_EPSILON) || (Probability - 1.0>-FLT_EPSILON)) return 1;
		
		(*Data).min = (*Data).mu - (*Data).sigma * _CONST_SQRT2 * ErrorFunction_Invers(2.0*Probability - 1.0);
		(*Data).max = (*Data).mu + (*Data).sigma * _CONST_SQRT2 * ErrorFunction_Invers(2.0*Probability - 1.0);
		break;
	case 'l':
		if ((Probability<FLT_EPSILON) || (Probability - 1.0>-FLT_EPSILON)) return 1;
		
		(*Data).min = 0.0;
		(*Data).max = exp((*Data).mu + (*Data).sigma * _CONST_SQRT2 * ErrorFunction_Invers(2.0*Probability - 1.0));
		break;
	default : return 1;
	}
	
	return 0;
}

int   Statistics_DistributionParameters_IdentifyType(struct Statistics_Distribution *Data, const char buffer[])
{
	if      (!strcmp(&buffer[0], "Constant"))	(*Data).type = 'c';
	else if (!strcmp(&buffer[0], "Uniform"))	(*Data).type = 'u';
	else if (!strcmp(&buffer[0], "Normal"))		(*Data).type = 'n';
	else if (!strcmp(&buffer[0], "Gausian"))	(*Data).type = 'n';
	else if (!strcmp(&buffer[0], "LogNormal"))	(*Data).type = 'l';
	else										return 1;
	return 0;
}
const char *Statistics_DistributionParameters_GetNameType(struct Statistics_Distribution *Data)
{
	switch ((*Data).type)
	{
	case 'c': return "Constant";
	case 'u': return "Uniform";
	case 'n': return "Normal";
	case 'l': return "LogNormal";
	default : return "None";
	}
}

int   Statistics_DistributionParameters_MuSigma_to_MeanSDvariance(struct Statistics_Distribution *Data)
{
	(*Data).mean     = 0.0;
	(*Data).variance = 0.0;
	(*Data).sd       = 0.0;
	
	switch ((*Data).type)
	{
	case 'c':
		(*Data).mean     = (*Data).mu;
		(*Data).variance = 0.0;
		(*Data).sd       = 0.0;
		return 0;
	case 'u':
		(*Data).mean     = (*Data).mu;
		(*Data).variance = (*Data).sigma*(*Data).sigma / 3.0;
		(*Data).sd       = sqrt((*Data).variance);
		return 0;
	case 'n':
		(*Data).mean     = (*Data).mu;
		(*Data).sd       = (*Data).sigma;
		(*Data).variance = (*Data).sd*(*Data).sd;
		return 0;
	case 'l':
		(*Data).mean     = exp((*Data).mu + 0.50*(*Data).sigma*(*Data).sigma);
		(*Data).variance = (exp((*Data).sigma*(*Data).sigma) - 1.0)*exp(2.0*(*Data).mu + (*Data).sigma*(*Data).sigma);
		(*Data).sd       = sqrt((*Data).variance);
		return 0;
	default :
		_APP_WARNING_MESSAGE(NULL, "Utility_Statistics.cpp", "Statistics_DistributionParameters_MuSigma_to_MeanSDvariance", _WARNING_MESSAGE_KEY_VALUE, "Distribution type unknown!", (long)(*Data).type, 0);
		return 1;
	}
}
int   Statistics_DistributionParameters_MeanSD_to_MuSigmaVariance(struct Statistics_Distribution *Data)
{
	(*Data).variance = 0.0;
	(*Data).sigma    = 0.0;
	(*Data).mu       = 0.0;
	
	switch ((*Data).type)
	{
	case 'c':
		(*Data).variance = 0.0;
		(*Data).sigma    = 0.0;
		(*Data).mu       = (*Data).mean;
		return 0;
	case 'u':
		(*Data).variance = (*Data).sd*(*Data).sd;
		(*Data).sigma    = sqrt(3.0*(*Data).variance);
		(*Data).mu       = (*Data).mean;
		return 0;
	case 'n':
		(*Data).variance = (*Data).sd*(*Data).sd;
		(*Data).sigma    = (*Data).sd;
		(*Data).mu       = (*Data).mean;
		return 0;
	case 'l':
		if ((*Data).mean < FLT_EPSILON) return 1;
		(*Data).variance = (*Data).sd*(*Data).sd;
		(*Data).sigma    = sqrt(log(((*Data).variance / ((*Data).mean * (*Data).mean)) + 1.0));
		(*Data).mu       = log((*Data).mean) - 0.50 * (*Data).sigma * (*Data).sigma;
		return 0;
	default :
		_APP_WARNING_MESSAGE(NULL, "Utility_Statistics.cpp", "Statistics_DistributionParameters_MeanSD_to_MuSigmaVariance", _WARNING_MESSAGE_KEY_VALUE, "Distribution type unknown!", (long)(*Data).type, 0);
		return 1;
	}
}


double Statistics_PDF_SkewNormal(double x, double Xi, double Omega, double Alfa)
{
	double y = (x - Xi) / Omega;
	double z = Alfa * y;

	return (2.0 / Omega) * (_CONST_1_SQRT2 * _CONST_1_Pi * exp(-0.50 * y * y)) * (0.50 * (1.0 + ErrorFunction(z * _CONST_1_SQRT2)));
}
