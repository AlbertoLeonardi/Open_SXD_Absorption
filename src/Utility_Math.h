// Utility Functions :: Math Functions
//
// Provides Optimized Math Complex Tools

#ifndef _INCLUDE_UTILITY_MATH
#define _INCLUDE_UTILITY_MATH 1

#include <App_Environment.h>

long GCD_2_L(long A, long B);				// Greatest Common Divisor   between A and B
long LCM_2_L(long A, long B);				// Lowest   Common Multiple  between A and B
long GCD_3_L(long A, long B, long C);		// Greatest Common Divisor   between A, B and C
long LCM_3_L(long A, long B, long C);		// Lowest   Common Multiple  between A, B and C

int LCM_2_L_s(long *LCM, long A, long B);	// Save Lowest Common Multiple between A>=0 and B>=0

unsigned long GCD_2_UL(unsigned long A, unsigned long B);					// Greatest Common Divisor   between A and B
unsigned long LCM_2_UL(unsigned long A, unsigned long B);					// Lowest   Common Multiple  between A and B
unsigned long GCD_3_UL(unsigned long A, unsigned long B, unsigned long C);	// Greatest Common Divisor   between A, B and C
unsigned long LCM_3_UL(unsigned long A, unsigned long B, unsigned long C);	// Lowest   Common Multiple  between A, B and C

int LCM_2_UL_s(unsigned long *LCM, unsigned long A, unsigned long B);		// Save Lowest Common Multiple between A and B

void EigenValueVectors_Jacobi_Cyclic_Method(double *eigenvalues, double *eigenvectors, double *A, int n);

int InvertMatrix_3x3(double *A);
double DeterminantMatrix_3x3(double *A);

//////////////////////////////////////////////////////////////////////////////// 2D ROTATION MATRIX // Convention: p' = R*p // right-hand rotation

void RotationMatrix_2D(double* Theta, double* R);

//////////////////////////////////////////////////////////////////////////////// 3D ROTATION MATRIX // Convention: p' = R*p

void RotationMatrix_Extrinsic_3D_XYZ(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_Extrinsic_3D_XZY(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_Extrinsic_3D_YXZ(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_Extrinsic_3D_YZX(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_Extrinsic_3D_ZXY(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_Extrinsic_3D_ZYX(double Alfa, double Beta, double Gamma, double* R);

#define RotationMatrix_3D_Intrinsic_XYZ(...)	RotationMatrix_Extrinsic_3D_ZYX(__VA_ARGS__)
#define RotationMatrix_3D_Intrinsic_XZY(...)	RotationMatrix_Extrinsic_3D_YZX(__VA_ARGS__)
#define RotationMatrix_3D_Intrinsic_YXZ(...)	RotationMatrix_Extrinsic_3D_ZXY(__VA_ARGS__)
#define RotationMatrix_3D_Intrinsic_YZX(...)	RotationMatrix_Extrinsic_3D_XZY(__VA_ARGS__)
#define RotationMatrix_3D_Intrinsic_ZXY(...)	RotationMatrix_Extrinsic_3D_YXZ(__VA_ARGS__)
#define RotationMatrix_3D_Intrinsic_ZYX(...)	RotationMatrix_Extrinsic_3D_XYZ(__VA_ARGS__)

void RotationMatrix_3D_Euler_XZX(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_3D_Euler_XYX(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_3D_Euler_YXY(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_3D_Euler_YZY(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_3D_Euler_ZYZ(double Alfa, double Beta, double Gamma, double* R);
void RotationMatrix_3D_Euler_ZXZ(double Alfa, double Beta, double Gamma, double* R);

void SolveRotation_3D(double* q, double* p, double* R);
void SolveRotation_3D(double* p, double* R);

//////////////////////////////////////////////////////////////////////////////// Fast, Compact Approximation of the Exponential Function

// Approximate exp adapted from Schraudolph, 1999 - double precision floating point version.
// Based on Schraudolph 1999, A Fast, Compact Approximation of the Exponential Function.
// - Adapted to use 64-bit integer; reduces staircase effect.
// - Valid for x in approx range (-700, 700).
//
// Source:
// https://bduvenhage.me/performance/machine_learning/2019/06/04/fast-exp.html
// Bernardt Duvenhage

inline double fast_exp_64(const double x) noexcept
{
    union { double d_; int64_t i_; } uid;
    
    uid.i_ = int64_t(double((int64_t(1) << 52) / log(2.0)) * x + double((int64_t(1) << 52) * 1023 - 0));    // c=0 for 1.0 at zero.
    
    return uid.d_;
}

#endif