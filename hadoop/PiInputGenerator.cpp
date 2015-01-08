/**
 * Copyright 2014 Dumi Loghin
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *** 
 * Pi estimator MapReduce - samples files generator.
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
