// Utility Functions :: Test
//

#include "Utility_LinearAbsorption.h"

int    LinearAbsorption_GetElementCoefficients(long Elements_Count, struct LinearAbsorption_Element* Element, char DataBase_Path[])
{
	long i, j;
	long id, length;
	
	bool Test;
	
	char buffer[_Nch500];
	char name[_Nch20];
	char *s_token, *token;
	
	FILE* DAT;
	
	DAT = File_IO_Open(DataBase_Path, "r");
	
	for (id = 0; id < Elements_Count; id++)
	{
		Element[id].s_abs[0] = 0.0;
		Element[id].s_abs[1] = 0.0;
		
		Element[id].s_inc[0] = 0.0;
		Element[id].s_inc[1] = 1.0;
		Element[id].s_inc[2] = 0.0;
		Element[id].s_inc[3] = 1.0;
		
		rewind(DAT);
		
		Test = false;
		do
		{
			File_IO_GetValue("Read line", "-", DAT, NULL, NULL, "\n", 1, 's', &buffer[0], _Nch500, NULL);
			s_token = strtok_s(buffer, " \t\v", &token);
			
			if (!strcmp(s_token, &Element[id].Name[0])) Test = true;
			else
			{
				memset(&name[0], 0, _Nch20 * sizeof(char));
				length = (long)strlen(s_token);
				if (length > _Nch20) length = _Nch20;
				j = 0; i = length;
				while (j < length) { if (s_token[j] != '-') j++; else { i = j; j = length; } }
				j = 0; i++;
				while (i < length)
				{
					name[j] = s_token[i];
					i++; j++;
				}
				
				if (!strcmp(&name[0], &Element[id].Name[0])) Test = true;
			}
		}
		while ((!feof(DAT)) && (!Test));
		
		if(Test)
		{
			s_token = strtok_s(NULL, " \t\v", &token); sscanf_s(s_token, "%lf", &Element[id].s_abs[0]); Element[id].s_abs[0] /= 1.7980;
			s_token = strtok_s(NULL, " \t\v", &token); sscanf_s(s_token, "%lf", &Element[id].s_abs[1]);
			s_token = strtok_s(NULL, " \t\v", &token); sscanf_s(s_token, "%lf", &Element[id].s_inc[0]);
			s_token = strtok_s(NULL, " \t\v", &token); sscanf_s(s_token, "%lf", &Element[id].s_inc[1]);
			s_token = strtok_s(NULL, " \t\v", &token); sscanf_s(s_token, "%lf", &Element[id].s_inc[2]);
			s_token = strtok_s(NULL, " \t\v", &token); sscanf_s(s_token, "%lf", &Element[id].s_inc[3]);
		}
		else { _APP_WARNING_MESSAGE(NULL, "Utility_LinearAbsorption.cpp", "LinearAbsorption_GetElementCoefficients", _WARNING_MESSAGE_KEY_VALUE, "Element not found! Check element name!", id + 1, 0); }
	}
	
	fclose(DAT);
	
	return 0;
}
double LinearAbsorption_GetMu(long Elements_Count, struct LinearAbsorption_Element* Element, double CellVolume_A3, double Lambda)
{
	long i;
	
	double D_void;
	double s_abs, s_inc, Mu;
	
	if (CellVolume_A3 < FLT_EPSILON) return 0.0;
	
	Mu = 0.0;
	for (i = 0; i < Elements_Count; i++)
	{
		s_abs = Lambda * Element[i].s_abs[0] + Element[i].s_abs[1];
		if (fabs(Element[i].s_inc[1]) > FLT_EPSILON)
		{
			D_void = 1.0 + pow(Lambda / Element[i].s_inc[1], Element[i].s_inc[3]);
			if (fabs(D_void) > FLT_EPSILON) s_inc = Element[i].s_inc[0] + Element[i].s_inc[2] / D_void;
			else                            s_inc = Element[i].s_inc[0];
		}
		else s_inc = Element[i].s_inc[0];
		
		Mu += Element[i].Occupancy * (s_abs + s_inc);
	}
	
	return Mu / CellVolume_A3;
}

int    LinearAbsorption_Reflections_Mu(struct Reflection_List_DataSet* Reflection_List, long Elements_Count, struct LinearAbsorption_Element* Element, double LenghtScale)
{
	long i, j;
	
	double CellVolume, ca, cb, cg;
	
	ca = cos(Reflection_List->UnitCell[3]); cb = cos(Reflection_List->UnitCell[5]); cg = cos(Reflection_List->UnitCell[5]);
	CellVolume = Reflection_List->UnitCell[0] * Reflection_List->UnitCell[1] * Reflection_List->UnitCell[2] * sqrt(1.0 - ca * ca - cb * cb - cg * cg + 2.0 * ca * cb * cg);
	
	if (CellVolume < FLT_EPSILON) { _APP_WARNING_MESSAGE(NULL, "Utility_LinearAbsorption.cpp", "LinearAbsorption_Reflections_Mu", _WARNING_MESSAGE_KEY_VALUE, "Negative Unit Cell Volume!", 0, 0); return 1; }
	
	for (i = 0; i < Reflection_List->Reflection_Count; i++) {
	for (j = 0; j < Reflection_List->Reflection[i].Absorption_Count; j++) {
		Reflection_List->Reflection[i].Absorption[j].Mu = LenghtScale * LinearAbsorption_GetMu(Elements_Count, Element, CellVolume, Reflection_List->Reflection[i].Absorption[j].Mu);
	}}
	
	return 0;
}