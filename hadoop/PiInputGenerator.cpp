/*
 * Pi estimator MapReduce - samples files generator.
 * 
 * Copyright 2014 D. Loghin
 *
 */
#include<stdlib.h>
#include<stdio.h>

#define PREFIX	"pi-part-"

long numMaps = 16;
long numSamples = 1000000000;

int main() {
	char path[128];	
	long i;
	printf("Generating input files for Hadoop PiEstimator\n\twith numMaps = %ld and numSamples = %ld\n", numMaps, numSamples);
	//generate an input file for each map task
	for(i=0; i < numMaps; ++i) {		
		sprintf(path, "%s%ld", PREFIX, i);
		FILE* writeFile = fopen(path, "wt");
    		if(!writeFile) {
		          printf("Failed to open %s for writing!", path);
		          return -2;
		}
		
		fprintf(writeFile, "%ld %ld\n", i * numSamples, numSamples);
		fclose(writeFile);
	}
	printf("Done.\n");
	return 0;
}
