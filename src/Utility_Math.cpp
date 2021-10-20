// Utility Functions :: Math Functions
//
// Provides Optimized Math Complex Tools

#include "Utility_Math.h"

long GCD_2_L(long A, long B) { if (B) return GCD_2_L(B, A % B); return A; }	// Greatest Common Divisor   between A>=0 and B>=0
long LCM_2_L(long A, long B) { return A * (B / GCD_2_L(A, B)); }				// Lowest   Common Multiple  between A>=0 and B>=0
long GCD_3_L(long A, long B,  long C) { return GCD_2_L(GCD_2_L(A, B), C); }		// Greatest Common Divisor   between A>=0, B>=0 and C>=0
long LCM_3_L(long A, long B,  long C) { return LCM_2_L(LCM_2_L(A, B), C); }		// Lowest   Common Multiple  between A>=0, B>=0 and C>=0 

int LCM_2_L_s(long *LCM, long A, long B)									// Save Lowest Common Multiple between A>=0 and B>=0
{
	long Lvoid_t;

	Lvoid_t = B / GCD_2_L(A, B);

	if (LONG_MAX / Lvoid_t <= A) { *LCM = 0;           return 1; }
	else                         { *LCM = A * Lvoid_t; return 0; }

    return 0;
}

unsigned long GCD_2_UL(unsigned long A, unsigned long B) { if (B) return GCD_2_UL(B, A % B); return A; }		// Greatest Common Divisor   between A and B
unsigned long LCM_2_UL(unsigned long A, unsigned long B) { return A * (B / GCD_2_UL(A, B)); }					// Lowest   Common Multiple  between A and B
unsigned long GCD_3_UL(unsigned long A, unsigned long B, unsigned long C) { return GCD_2_UL(GCD_2_UL(A, B), C); }	// Greatest Common Divisor   between A, B and C
unsigned long LCM_3_UL(unsigned long A, unsigned long B, unsigned long C) { return LCM_2_UL(LCM_2_UL(A, B), C); }	// Lowest   Common Multiple  between A, B an

int LCM_2_UL_s(unsigned long *LCM, unsigned long A, unsigned long B)	// Save Lowest Common Multiple between A and B
{
	unsigned long Lvoid_t;

	Lvoid_t = B / GCD_2_UL(A, B);

	if (ULONG_MAX / Lvoid_t > A) { *LCM = 0;           return 1; }
	else                         { *LCM = A * Lvoid_t; return 0; }

    return 0;
}

void EigenValueVectors_Jacobi_Cyclic_Method(double *eigenvalues, double *eigenvectors,double *A, int n)
 {
 int     i, j, k, m;
 double *pAk, *pAm, *p_r, *p_e;
 double  threshold_norm;
 double  threshold;
 double  tan_phi, sin_phi, cos_phi, tan2_phi, sin2_phi, cos2_phi;
 double  sin_2phi, cot_2phi;
 // @@@ double  cos_2phi;
 double  dum1;
 double  dum2;
 double  dum3;
 double  max;
 
 // Take care of trivial cases
 
 if ( n < 1) return;
 if ( n == 1) { eigenvalues[0] = *A;
                *eigenvectors = 1.0;
                return; }
 
 // Initialize the eigenvalues to the identity matrix.
 
 for (p_e = eigenvectors, i = 0; i < n; i++) {
  for (j = 0; j < n; p_e++, j++) {
   if (i == j) *p_e = 1.0; else *p_e = 0.0; }}
 
 // Calculate the threshold and threshold_norm.
 
 for (threshold = 0.0, pAk = A, i = 0; i < ( n - 1 ); pAk += n, i++) {
  for (j = i + 1; j < n; j++) { threshold += *(pAk + j) * *(pAk + j); }}
  
 threshold = sqrt(threshold + threshold);
 threshold_norm = threshold * DBL_EPSILON;
 max = threshold + 1.0;
 while (threshold > threshold_norm)
    {
    threshold /= 10.0;
    if (max < threshold) continue;
    max = 0.0;
    for (pAk = A, k = 0; k < (n-1); pAk += n, k++) {
     for (pAm = pAk + n, m = k + 1; m < n; pAm += n, m++) {
      if ( fabs(*(pAk + m)) < threshold ) continue;
      
      // Calculate the sin and cos of the rotation angle which
      // annihilates A[k][m].
      
      cot_2phi = 0.5 * ( *(pAk + k) - *(pAm + m) ) / *(pAk + m);
      dum1 = sqrt( cot_2phi * cot_2phi + 1.0);
      if (cot_2phi < 0.0) dum1 = -dum1;
      tan_phi = -cot_2phi + dum1;
      tan2_phi = tan_phi * tan_phi;
      sin2_phi = tan2_phi / (1.0 + tan2_phi);
      cos2_phi = 1.0 - sin2_phi;
      sin_phi = sqrt(sin2_phi);
      if (tan_phi < 0.0) sin_phi = - sin_phi;
      cos_phi = sqrt(cos2_phi); 
      sin_2phi = 2.0 * sin_phi * cos_phi;
      // @@@ cos_2phi = cos2_phi - sin2_phi;
      
      // Rotate columns k and m for both the matrix A 
      //     and the matrix of eigenvectors.
      
      p_r = A;
      dum1 = *(pAk + k);
      dum2 = *(pAm + m);
      dum3 = *(pAk + m);
      *(pAk + k) = dum1 * cos2_phi + dum2 * sin2_phi + dum3 * sin_2phi;
      *(pAm + m) = dum1 * sin2_phi + dum2 * cos2_phi - dum3 * sin_2phi;
      *(pAk + m) = 0.0;
      *(pAm + k) = 0.0;
      for (i = 0; i < n; p_r += n, i++)
         {
         if ( (i == k) || (i == m) ) continue;
         if ( i < k ) dum1 = *(p_r + k); else dum1 = *(pAk + i);
         if ( i < m ) dum2 = *(p_r + m); else dum2 = *(pAm + i);
         dum3 = dum1 * cos_phi + dum2 * sin_phi;
         if ( i < k ) *(p_r + k) = dum3; else *(pAk + i) = dum3;
         dum3 = - dum1 * sin_phi + dum2 * cos_phi;
         if ( i < m ) *(p_r + m) = dum3; else *(pAm + i) = dum3;
         }
      for (p_e = eigenvectors, i = 0; i < n; p_e += n, i++)
         {
         dum1 = *(p_e + k);
         dum2 = *(p_e + m);
         *(p_e + k) = dum1 * cos_phi + dum2 * sin_phi;
         *(p_e + m) = - dum1 * sin_phi + dum2 * cos_phi;
         }
      }
     for (i = 0; i < n; i++) {
      if ( i == k ) continue;
      else if ( max < fabs(*(pAk + i))) max = fabs(*(pAk + i)); }
     }
   }
 for (pAk = A, k = 0; k < n; pAk += n, k++) eigenvalues[k] = *(pAk + k);
 
 return;
 }

int InvertMatrix_3x3(double *A)
{
	double M[3][3], IdetA, detA;
	
	M[0][0] = +(A[4] * A[8] - A[5] * A[7]);
	M[1][0] = -(A[3] * A[8] - A[5] * A[6]);
	M[2][0] = +(A[3] * A[7] - A[4] * A[6]);
	
	M[0][1] = -(A[1] * A[8] - A[2] * A[7]);
	M[1][1] = +(A[0] * A[8] - A[2] * A[6]);
	M[2][1] = -(A[0] * A[7] - A[1] * A[6]);
	
	M[0][2] = +(A[1] * A[5] - A[2] * A[4]);
	M[1][2] = -(A[0] * A[5] - A[2] * A[3]);
	M[2][2] = +(A[0] * A[4] - A[1] * A[3]);
	
	detA = A[0] * M[0][0] + A[1] * M[1][0] + A[2] * M[2][0];
	
	if (fabs(detA) < FLT_EPSILON) return 1;
	IdetA = 1.0 / detA;
	
	A[0] = M[0][0] * IdetA;
	A[1] = M[0][1] * IdetA;
	A[2] = M[0][2] * IdetA;
	
	A[3] = M[1][0] * IdetA;
	A[4] = M[1][1] * IdetA;
	A[5] = M[1][2] * IdetA;
	
	A[6] = M[2][0] * IdetA;
	A[7] = M[2][1] * IdetA;
	A[8] = M[2][2] * IdetA;
	
	return 0;
}
double DeterminantMatrix_3x3(double *A)
{
	return (A[0] * (A[4] * A[8] - A[5] * A[7]) - A[1] * (A[3] * A[8] - A[5] * A[6]) + A[2] * (A[3] * A[7] - A[4] * A[6]));
}

//////////////////////////////////////////////////////////////////////////////// 2D ROTATION MATRIX // Convention: p' = R*p // right-hand rotation

void RotationMatrix_2D(double *Theta, double *R)
{
	double cc, ss;
	
	cc = cos(*Theta); ss = sin(*Theta);
	
	R[0] = +cc; R[1] = -ss;
	R[2] = +ss; R[3] = +cc;
	
	return;
}

//////////////////////////////////////////////////////////////////////////////// 3D ROTATION MATRIX // Convention: p' = R*p (q = R*p)

void RotationMatrix_Extrinsic_3D_XYZ(double Alfa, double Beta, double Gamma, double *R) // p' = R*p , R=Z*Y*X
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb*Cg;
	R[1] = Cg*Sa*Sb - Ca*Sg;
	R[2] = Ca*Cg*Sb + Sa*Sg;
	
	R[3] = Cb*Sg;
	R[4] = Ca*Cg + Sa*Sb*Sg;
	R[5] = -Cg*Sa + Ca*Sb*Sg;
	
	R[6] = -Sb;
	R[7] = Cb*Sa;
	R[8] = Ca*Cb;
	
	return;
}
void RotationMatrix_Extrinsic_3D_XZY(double Alfa, double Beta, double Gamma, double *R) // p' = R*p , R=Y*Z*X
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb*Cg;
	R[1] = Sa*Sb - Ca*Cb*Sg;
	R[2] = Ca*Sb + Cb*Sa*Sg;
	
	R[3] = Sg;
	R[4] = Ca*Cg;
	R[5] = -Cg*Sa;
	
	R[6] = -Cg*Sb;
	R[7] = Cb*Sa + Ca*Sb*Sg;
	R[8] = Ca*Cb - Sa*Sb*Sg;
	
	return;
}
void RotationMatrix_Extrinsic_3D_YXZ(double Alfa, double Beta, double Gamma, double *R) // p' = R*p , R=Z*X*Y
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb*Cg - Sa*Sb*Sg;
	R[1] = -Ca*Sg;
	R[2] = Cg*Sb + Cb*Sa*Sg;
	
	R[3] = Cg*Sa*Sb + Cb*Sg;
	R[4] = Ca*Cg;
	R[5] = -Cb*Cg*Sa + Sb*Sg;
	
	R[6] = -Ca*Sb;
	R[7] = Sa;
	R[8] = Ca*Cb;
	
	return;
}
void RotationMatrix_Extrinsic_3D_YZX(double Alfa, double Beta, double Gamma, double *R) // p' = R*p , R=X*Z*Y
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] =
	R[1] = Cb*Cg;
	R[2] = -Sg;
	
	R[3] = Sa*Sb + Ca*Cb*Sg;
	R[4] = Ca*Cg;
	R[5] = -Cb*Sa + Ca*Sb*Sg;
	
	R[6] = -Ca*Sb + Cb*Sa*Sg;
	R[7] = Cg*Sa;
	R[8] = Ca*Cb + Sa*Sb*Sg;
	
	return;
}
void RotationMatrix_Extrinsic_3D_ZXY(double Alfa, double Beta, double Gamma, double *R) // p' = R*p , R=Y*X*Z
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb*Cg + Sa*Sb*Sg;
	R[1] = Cg*Sa*Sb - Cb*Sg;
	R[2] = Ca*Sb;
	
	R[3] = Ca*Sg;
	R[4] = Ca*Cg;
	R[5] = -Sa;
	
	R[6] = -Cg*Sb + Cb*Sa*Sg;
	R[7] = Cb*Cg*Sa + Sb*Sg;
	R[8] = Ca*Cb;
	
	return;
}
void RotationMatrix_Extrinsic_3D_ZYX(double Alfa, double Beta, double Gamma, double *R) // p' = R*p , R=X*Y*Z
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb*Cg;
	R[1] = -Cb*Sg;
	R[2] = Sb;
	
	R[3] = Cg*Sa*Sb + Ca*Sg;
	R[4] = Ca*Cg - Sa*Sb*Sg;
	R[5] = -Cb*Sa;
	
	R[6] = -Ca*Cg*Sb + Sa*Sg;
	R[7] = Cg*Sa + Ca*Sb*Sg;
	R[8] = Ca*Cb;
	
	return;
}

void RotationMatrix_3D_Euler_XZX(double Alfa, double Beta, double Gamma, double *R) // p' = R*p
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb;
	R[1] = -Cg*Sb;
	R[2] = Sb*Sg;
	
	R[3] = Ca*Sb;
	R[4] = Ca*Cb*Cg - Sa*Sg;
	R[5] = -Cg*Sg - Ca*Cb*Sg;
	
	R[6] = Sa*Sb;
	R[7] = Ca*Sg + Cb*Cg*Sa;
	R[8] = Ca*Cg - Cb*Sa*Sg;
	
	return;
}
void RotationMatrix_3D_Euler_XYX(double Alfa, double Beta, double Gamma, double *R) // p' = R*p
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Cb;
	R[1] = Sb*Sg;
	R[2] = Cg*Sb;
	
	R[3] = Sa*Sb;
	R[4] = Ca*Cg - Cb*Sa*Sg;
	R[5] = -Ca*Sg - Cb*Cg*Sa;
	
	R[6] = -Ca*Sb;
	R[7] = Cg*Sa + Ca*Cb*Sg;
	R[8] = Ca*Cb*Cg - Sa*Sg;
	
	return;
}
void RotationMatrix_3D_Euler_YXY(double Alfa, double Beta, double Gamma, double *R) // p' = R*p
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Ca*Cg - Cb*Sa*Sg;
	R[1] = Sa*Sb;
	R[2] = Ca*Sg + Cb*Cg*Sa;
	
	R[3] = Sb*Sg;
	R[4] = Cb;
	R[5] = -Cg*Sb;
	
	R[6] = -Cg*Sa - Ca*Cb*Sg;
	R[7] = Ca*Sb;
	R[8] = Ca*Cb*Cg - Sa*Sg;
	
	return;
}
void RotationMatrix_3D_Euler_YZY(double Alfa, double Beta, double Gamma, double *R) // p' = R*p
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Ca*Cb*Cg - Sa*Sg;
	R[1] = -Ca*Sb;
	R[2] = Cg*Sa + Ca*Cb*Sg;
	
	R[3] = Cg*Sb;
	R[4] = Cb;
	R[5] = Sb*Sg;
	
	R[6] = -Ca*Sg - Cb*Cg*Sa;
	R[7] = Ca*Cg - Cb*Sa*Sg;
	R[8] = Ca*Cg - Cb*Sa*Sg;
	
	return;
}
void RotationMatrix_3D_Euler_ZYZ(double Alfa, double Beta, double Gamma, double *R) // p' = R*p
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Ca*Cb*Cg - Sa*Sg;
	R[1] = -Cg*Sa - Ca*Cb*Sg;
	R[2] = Ca*Sb;
	
	R[3] = Ca*Sg + Cb*Cg*Sa;
	R[4] = Ca*Cg - Cb*Sa*Sg;
	R[5] = Sa*Sb;
	
	R[6] = -Cg*Sb;
	R[7] = Sb*Sg;
	R[8] = Cb;
	
	return;
}
void RotationMatrix_3D_Euler_ZXZ(double Alfa, double Beta, double Gamma, double *R) // p' = R*p
{
	double Ca, Cb, Cg;
	double Sa, Sb, Sg;
	
	Sa = sin(Alfa);  Ca = cos(Alfa);
	Sb = sin(Beta);  Cb = cos(Beta);
	Sg = sin(Gamma); Cg = cos(Gamma);
	
	R[0] = Ca*Cg - Cb*Sa*Sg;
	R[1] = -Ca*Sg - Cb*Cg*Sa;
	R[2] = Sa*Sb;
	
	R[3] = Cg*Sa + Ca*Cb*Sg;
	R[4] = Ca*Cb*Cg - Sa*Sg;
	R[5] = -Ca*Sb;
	
	R[6] = Sb*Sg;
	R[7] = Cg*Sb;
	R[8] = Cb;
	
	return;
}

void SolveRotation_3D(double *q, double *p, double* R) // q = R * p
{
	q[0] = R[0] * p[0] + R[1] * p[1] + R[2] * p[2];
	q[1] = R[3] * p[0] + R[4] * p[1] + R[5] * p[2];
	q[2] = R[6] * p[0] + R[7] * p[1] + R[8] * p[2];

	//q[0] = p[0] * R[0] + p[1] * R[3] + p[2] * R[6];
	//q[1] = p[0] * R[1] + p[1] * R[4] + p[2] * R[7];
	//q[2] = p[0] * R[2] + p[1] * R[5] + p[2] * R[8];
	
	return;
}
void SolveRotation_3D(double* p, double* R) // p' = p * R
{
	double q[3];

	SolveRotation_3D(&q[0], p, R);
		
	p[0] = q[0];
	p[1] = q[1];
	p[2] = q[2];
	
	return;
}