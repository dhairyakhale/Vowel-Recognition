//AiRiCi.h : Calculates 12 Ci values from given input
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define p 12

void findRi(double values[], int sample_size, double* Rout)
{
	int i=0;

	for(i=0; i<=p; i++)
	{
		int k=0;
		for(k=1;k<=(sample_size-i);k++)
			Rout[i] += values[k]*values[k+i];
	}
}

void findAi(double Ri[], double* Aout)
{
	int i=0;
	double A[p+1][p+1];

	for(i=0;i<=p;i++)
	{
		A[0][i] = 0;
		A[i][0] = 0;
	}

	double E[p+1] = {0};
	double k[p+1] = {0};

	E[0] = Ri[0];

	for(i=1;i<=p;i++)
	{
		if(i==1)
			k[i] = Ri[i] / E[i-1];
		else
		{
			for(int j=1;j<=(i-1);j++)
			{
				k[i] += A[i-1][j]*Ri[i-j];
			}

			k[i] = Ri[i] - k[i];
			k[i] /= E[i-1];
		}

		A[i][i] = k[i];

		for(int j=1;j<=(i-1);j++)
		{
			A[i][j] = A[i-1][j] - (k[i] * A[i-1][i-j]);
		}

		E[i] = (1 - k[i]*k[i]) * E[i-1];
	}

	for(i=1;i<=p;i++)
		Aout[i] = A[12][i];
}

void findCi(double Ai[], double* Cout)
{
	int i=0;
	
	for(i=1;i<=p;i++)
	{
		int k;
		for(k=1;k<=(i-1);k++)
			Cout[i] += (k/(i*1.0))*Cout[k]*Ai[i-k];

		Cout[i] += Ai[i];
	}
}