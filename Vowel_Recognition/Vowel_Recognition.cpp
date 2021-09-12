// Vowel_Recognition.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "AiRiCi.h"			//Header file for eventual calculation of Cepstrums for input values

#define pi 3.14159

int main()
{
	char vowels[10] = "a\0e\0i\0o\0u";	//Array for printing vowels

	FILE* fp = NULL;		//General purpose file pointer

	FILE* fp_ref = NULL;	//For creating reference file for each vowel (Cs for each frame)

	//For reading samples from file
	char mystring[150];
	double dat = 0;

	//Iterators used throughout the code in nesting order
	int i=0;
	int j=0;
	int k=0;
	int m=0;
	
	int count = 0;			//Count for dc shift calculation
	
	double max_val = 0;		//Maximum amplitude for normalization

	char usc[2] = "_";
	char ext[5] = ".txt";

	double calc_c[5][p];	//Array for Ci of a vowel (Row is frame no.)

	double vowel_c[5][p];	//Array for Ci (Row is vowel no.)

	//Initialization
	for(i=0;i<5;i++)
	{
		for(j=0;j<p;j++)
			vowel_c[i][j]=0;
	}

	//-------------------TRAINING-------------------------------------------------

	for(i=0;i<5;i++)		//Iterating into training files for each vowel
	{
		//Creating reference file for each vowel

		char ref_pathname[33]="Reference_Files/";
		
		strcat(ref_pathname,&vowels[i*2]);
		strcat(ref_pathname,ext);

		fp_ref = fopen((const char *)ref_pathname, "w");

		//Initializing
		for(j=0;j<5;j++)
		{
			for(k=0;k<p;k++)
				calc_c[j][k] = 0;
		}

		//Looping into each training file for a vowel (1-10)
		for(j=1;j<=10;j++)
		{
			count = 0;

			//Creating pathname to access training file

			char pathname[33] = "Vowel_Files/214101016_";
			strcat(pathname,&vowels[i*2]);

			char str[3] = "";
			sprintf(str,"%d",j);

			strcat(pathname, usc);
			strcat(pathname, str);
			strcat(pathname, ext);

			fp = fopen((const char *)pathname,"r");

			double dc_shift = 0;

			//Reading samples from file
			while(!feof(fp))
			{
				fgets(mystring, 150, fp);	//Storing sample into mystring
				if(feof(fp)) break;

				count++;

				dat = atof(mystring);		//Converting string into double, for each sample

				if(abs(dat)>max_val)		//Maximum amplitude calculation
					max_val = abs(dat);

				dc_shift += dat;
			}

			dc_shift /= count;				//DC shift calculation


			rewind(fp);						//Rewinding fp to find max energy frame

			//Initializing values for frame calculation
			double max_e = 0;
			int max_e_frame = 0;
			double energy_frame = 0;
			int frame_no = 0;	

			while(!feof(fp))
			{
				//Read each frame
				for(k=0;k<320;k++)
				{
					fgets(mystring, 150, fp);	
					if(feof(fp)) break;

					dat = atof(mystring);

					dat -= dc_shift;		//Doing dc shift
					dat *= (5000/max_val);	//Doing normalization

					energy_frame += (dat*dat);
				}

				energy_frame /= k;			//Calculating energy per frame

				//Finding max energy frame
				if(energy_frame > max_e)
				{
					max_e = energy_frame;
					max_e_frame = frame_no;
				}

				frame_no++;
			}

			rewind(fp);		//Rewind fp to get values from stable part

			int start = (max_e_frame - 2)*320;		//Start of stable part

			for(k=0;k<start;k++)			//Skipping non-stable data
				fgets(mystring,150,fp);

			int sample_size = 320;
			double values[320+1] = {0};

			for(k=0;k<5;k++)	//Looping through frames
			{
				for(m=1;m<=320;m++)
				{
					fgets(mystring,150,fp);
					dat = atof(mystring);

					dat -= dc_shift;
					dat *= (5000/max_val);

					values[m] = dat;	//Collect values for a frame
				}

				// Finding Ci for the frame

				double Ri[p+1] = {0};
				findRi(values, sample_size, Ri);

				double Ai[p+1] = {0};
				findAi(Ri, Ai);

				double Ci[p+1] = {0};

				Ci[0] = log(Ri[0]*Ri[0]);

				findCi(Ai, Ci);

				//Found Ci

				//Applying sine window
				for(m=1;m<=p;m++)
				{
					calc_c[k][m-1] += Ci[m]*(1+(p/2)*sin((pi*m)/p));

					if(j==10)
					{
						calc_c[k][m-1] /= 10;
						fprintf(fp_ref,"%lf\n",calc_c[k][m-1]);		//Storing data into reference file
					}
				}
			}
			fclose(fp);
		}

		fclose(fp_ref);

		//Creating average Ci per frame array for further calculations
		for(k=0;k<p;k++)
		{
			for(m=0;m<5;m++)
			{
				vowel_c[i][k] += calc_c[m][k];
			}
			vowel_c[i][k] /= 5;
		}
	}

	//-------------------TESTING--------------------------------------------------

	FILE* fp_acc = fopen("Reference_Files/accuracydata.txt","w+");			//Making file for storing accuracy data

	double wi[p] = {1.0, 3.0, 7.0, 13.0, 19.0, 22.0, 25.0, 33.0, 42.0, 50.0, 56.0, 61.0};	//Wi values for Tokhura's distance

	for(i=0;i<5;i++)	//Looping into each vowel
	{
		int wrongans=0;

		for(j=11;j<=20;j++)	//Looping into each vowel's test file
		{
			count = 0;

			double calc_c_test[p] = {0};	//Stores Ci values of test file, averaged for all samples
			
			//Creating path for test file
			char pathname[33] = "Vowel_Files/214101016_";
			strcat(pathname,&vowels[i*2]);

			char str[3] = "";
			sprintf(str,"%d",j);

			strcat(pathname, usc);
			strcat(pathname, str);
			strcat(pathname, ext);

			fp = fopen((const char *)pathname,"r");

			printf("Matching the %dth test file with references:\n", (10*i)+(j-10));
			
			double tokhura=0;

			double dc_shift = 0;

			//Getting Ci values from a file, same as Training part
			while(!feof(fp))
			{
				fgets(mystring, 150, fp);
				if(feof(fp)) break;

				count++;

				dat = atof(mystring);

				if(abs(dat)>max_val)
					max_val = abs(dat);

				dc_shift += dat;
			}
			dc_shift /= count;

			rewind(fp);

			double max_e = 0;
			int max_e_frame = 0;
			double energy_frame = 0;
			int frame_no = 0;	

			while(!feof(fp))
			{
				for(k=0;k<320;k++)
				{
					fgets(mystring, 150, fp);
					if(feof(fp)) break;

					dat = atof(mystring);

					dat -= dc_shift;
					dat *= (5000/max_val);

					energy_frame += (dat*dat);
				}

				energy_frame /= k;

				if(energy_frame > max_e)
				{
					max_e = energy_frame;
					max_e_frame = frame_no;
				}

				frame_no++;
			}

			rewind(fp);

			int start = (max_e_frame - 2)*320;

			for(k=0;k<start;k++)
				fgets(mystring,150,fp);

			int sample_size = 320;
			double values[320+1] = {0};

			for(k=0;k<5;k++)
			{
				for(m=1;m<=320;m++)
				{
					fgets(mystring,150,fp);
					dat = atof(mystring);

					dat -= dc_shift;
					dat *= (5000/max_val);

					values[m] = dat;
				}

				double Ri[p+1] = {0};
				findRi(values, sample_size, Ri);

				double Ai[p+1] = {0};
				findAi(Ri, Ai);

				double Ci[p+1] = {0};

				Ci[0] = log(Ri[0]*Ri[0]);

				findCi(Ai, Ci);

				//Applying sine window to the Ci values
				for(m=1;m<=p;m++)
				{
					calc_c_test[m-1] += Ci[m]*(1+(p/2)*sin((pi*m)/p));

					if(k==4)	//Averaging for each sample in the end
					{
						calc_c_test[m-1] /= 5;
					}
				}
			}
			fclose(fp);

			int returnvowel = 0;
			double tokhuramin = 100000000000000;	//For calculation of minimum tokhura distance

			printf("Distances: ");

			for(k=0;k<5;k++)
			{
				for(m=0;m<p;m++)
					tokhura += wi[m]*(pow((calc_c_test[m] - vowel_c[k][m]),2));	//Tokhura's distance formula

				printf("%lf\t",tokhura);

				//Finding minimum from the distances
				if(tokhura<tokhuramin)
				{
					tokhuramin = tokhura;
					returnvowel = k;
				}

				tokhura = 0;
			}

			printf("\nVowel detected: %s\n\n",&vowels[i*2]);

			if(i != returnvowel)
				wrongans++;			//Count number of wrong answers
		}
		fprintf(fp_acc,"Accuracy for vowel %s: %d %\n", &vowels[i*2], (10*(10-wrongans)));	//Save accuracy data in file
	}

	rewind(fp_acc);

	//Displaying accuracy data on console
	while(!feof(fp_acc))
	{
		fgets(mystring,150,fp_acc);

		if(!feof(fp_acc))
			printf("%s",mystring);
	}

	fclose(fp_acc);

	system("PAUSE");	//Pause console to display data

	return 0;
}