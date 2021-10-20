// Application System
//
// Definition of application system parameters:
// .APPLICATION DEFAULT PARAMETERs
// .SYSTEM CONSTANTs
// .FILE EXTENSIONs
// .STRING FLEGs & LENGHTs

#ifndef _INCLUDE_APP_SYSTEM
#define _INCLUDE_APP_SYSTEM 1

//////////////////////////////////////////////////////////////////////////////// .APPLICATION DOUBLE PRECISON

// #define _USE_LONG_DOUBLE_PRECISON							// redefine "double" as "long double" data type. The "%lf" are then redefined as "%Lf" localy
															// Note: to be usefull the sizeof(double) should be larger than sizeof(long double), which is not ensured

#ifdef _USE_LONG_DOUBLE_PRECISON
 #define double long double
#endif

//////////////////////////////////////////////////////////////////////////////// .APPLICATION GLOBAL VARIABLES

extern FILE *_APP_LOG_FILE_POINTER;

//////////////////////////////////////////////////////////////////////////////// .SYSTEM CONSTANTs

#define _CONST_DegToRad					0.01745329251994329576923690768488	//61271344287 // = Pi/180 // Rad = Deg * DegToRad
#define _CONST_RadToDeg				   57.29577951308232087679815481410517	//03324054725 // = 180/Pi // Deg = Rad * RadToDeg

#define _CONST_Pi						3.14159265358979323846264338327950	//...28841971694
#define _CONST_2Pi						6.28318530717958647692528676655901	//(0)5768394339
#define _CONST_1_Pi						0.31830988618379067153776752674503	//(2)87240689193
#define _CONST_1_2Pi					0.15915494309189533576888376337251	//...43620344596

#define _CONST_1_SQRTPi					0.56418958354775628694807945156077  //25858440506
#define _CONST_1_SQRT2Pi				0.39894228040143267793994605993438	//18684758586

#define _CONST_Pi_2						1.57079632679489661923132169163975	//...1442098585
#define _CONST_2_Pi						0.63661977236758134307553505349006	//(5)74481378386

#define _CONST_Pi_3						1.04719755119659774615421446109317	//(6)76280657231
#define _CONST_3_Pi						9.42477796076937971538793014983851	//(0)86525915081

#define _CONST_SQRT2					1.41421356237309504880168872420970	//(69)807856967
#define _CONST_SQRT3					1.73205080756887729352744634150587	//...2366942805

#define _CONST_1_SQRT2					0.70710678118654752440084436210485	// (4)903928484
#define _CONST_1_SQRT3					0.57735026918962576450914878050196	// (5)745564760

#define _CONST_1_3						(1.0/3.0)

#define _CONST_Rct						287.05         // [J/(K Kg)]
#define _CONST_Na						6.02214179E23  // []
#define _CONST_Kb_JK					1.38064852E-23 // [J/K]
#define _CONST_Kb_eV					8.6173303E-5   // [eV]
#define _CONST_hct						6.62606896E-34 // [Js]
#define _CONST_Ang						1E-10          // [] shift m <--> Angstrom

//////////////////////////////////////////////////////////////////////////////// .FILE SOURCE NAMEs

#define _SN_LOG							"log."

//////////////////////////////////////////////////////////////////////////////// .FILE EXTENSIONs

#define _EXT_TXT						".txt"
#define _EXT_BIN						".bin"

#define _EXT_STL						".stl"
#define _EXT_VTK						".vtk"

#define _EXT_BMP						".bmp"

#define _EXT_IDL						".int"

//////////////////////////////////////////////////////////////////////////////// .STRING FLEGs & LENGHTs

#define _TOKEN_DELIM					" \t\r\n\v\f"		// Sequential key to split values in a line-string of data

#define _Nch_MAX_STRING_SIZE			Nch2000	// Upper limit size of string used to owerlay issue with not defined string size with "_s" functions

#define _Nch2000						2000
#define _Nch1000						1000
#define _Nch800							800
#define _Nch600							600
#define _Nch500							500
#define _Nch300							300
#define _Nch200							200
#define _Nch100							100
#define _Nch50							50
#define _Nch30							30
#define _Nch20							20
#define _Nch10							10

//////////////////////////////////////////////////////////////////////////////// .NEW DATA TYPEs

typedef long long_2[2];
typedef long long_3[3];
typedef long long_4[4];

typedef double double_2[2];
typedef double double_3[3];
typedef double double_4[4];

//////////////////////////////////////////////////////////////////////////////// ...

#endif
