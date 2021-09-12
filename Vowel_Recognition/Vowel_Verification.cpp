// Vowel_Verification.cpp : Verifies if the functions provided in AiRiCi.h work, using sample data
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "AiRiCi.h"

int main()
{
	FILE *fp = fopen("Verification_Files/test.txt","r");

	int sample_size = 320;
	double values[320+1] = {0};

	char mystring[150];
	double dat=0;
	int i = 0;
	
	for(i=1;i<=sample_size;i++)
	{
		fgets(mystring, 150, fp);
		dat = atof(mystring);

		values[i] = dat;
	}

	fclose(fp);

	double Ri[p+1] = {0};
	findRi(values, sample_size, Ri);

	fp = fopen("Verification_Files/myRi.txt","w");
	
	for(i=0;i<=p;i++)
		fprintf(fp,"%lf\t",Ri[i]);

	fclose(fp);

	double Ai[p+1] = {0};
	findAi(Ri, Ai);

	fp = fopen("Verification_Files/myAi.txt","w");
	
	for(i=1;i<=p;i++)
		fprintf(fp,"%lf\t",Ai[i]);

	fclose(fp);

	double Ci[p+1] = {0};

	Ci[0] = log(Ri[0]*Ri[0]);

	findCi(Ai, Ci);

	fp = fopen("Verification_Files/myCi.txt","w");
	
	for(i=1;i<=p;i++)
		fprintf(fp,"%lf\t",Ci[i]);

	fclose(fp);

	return 0;
}