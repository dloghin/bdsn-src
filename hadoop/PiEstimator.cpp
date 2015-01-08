/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ***
 * Pi estimator MapReduce - Hadoop Pipes
 *
 * Adapted from Pi estimator in Hadoop 1.2.1 examples
 */

#include "hadoop/Pipes.hh"
#include "hadoop/TemplateFactory.hh"
#include "hadoop/StringUtils.hh"

#include <string.h>
#include <iostream>
#include <string>

/* Original Pi Java writes samples files on HDFS. 
 However, writing on HDFS from C++ pipes does not work. 
 The samples files must be generated before Pi execution
 and uploaded on HDFS.
*/
#ifdef WITH_HDFS

#include "hdfs.h"

hdfsFS dfs;

#endif

using namespace std;

#define K_LENGTH	2

/** Bases */
int P[K_LENGTH] = {2, 3};

/** Maximum number of digits allowed */
int K[K_LENGTH] = {63, 40}; 

int numMaps = 0;
int numSamples = 0;

class HaltonSequence {
public: 
    long index;
    double x[K_LENGTH];
    double q[K_LENGTH][63];
    int d[K_LENGTH][63];

    /** Initialize to H(startindex),
     * so the sequence begins with H(startindex+1).
     */
    HaltonSequence(long startindex) {
      index = startindex;

      for(int i = 0; i < K_LENGTH; i++) {
        long k = index;
        x[i] = 0;
        
        for(int j = 0; j < K[i]; j++) {
          q[i][j] = (j == 0? 1.0: q[i][j-1])/P[i];
          d[i][j] = (int)(k % P[i]);
          k = (k - d[i][j])/P[i];
          x[i] += d[i][j] * q[i][j];
        }
      }
    }

    /** Compute next point.
     * Assume the current point is H(index).
     * Compute H(index+1).
     * 
     * @return a 2-dimensional point with coordinates in [0,1)^2
     */
    double* nextPoint() {
      index++;
      for(int i = 0; i < K_LENGTH; i++) {
        for(int j = 0; j < K[i]; j++) {
          d[i][j]++;
          x[i] += q[i][j];
          if (d[i][j] < P[i]) {
            break;
          }
          d[i][j] = 0;
          x[i] -= (j == 0? 1.0: q[i][j-1]);
        }
      }
      return x;
    }
};

class PiMapper : public HadoopPipes::Mapper {
public:
	PiMapper(HadoopPipes::TaskContext& context) {
		
	}
	
	void map(HadoopPipes::MapContext& context) {
		char buff[1024];

#ifdef FDEBUG
		cout << "Key " << context.getInputKey() << "Val " << context.getInputValue() << endl;
#endif

		char* value = (char*)context.getInputValue().c_str();
		char* tok = strtok(value, " ");
		if (tok == NULL)
			return;
		long offset = (long) stoll(tok, NULL, 10);
		tok = strtok(NULL, " ");
		if (tok == NULL)
                        return;
		long size = (long) stoll(tok, NULL, 10);

#ifdef FDEBUG
		cout << "in map size " << size << endl;
#endif

		HaltonSequence* haltonsequence = new HaltonSequence(offset);

		long numInside = 0L;
		long numOutside = 0L;
		long i;

		for(i = 0; i < size; ) {
			//generate points in a unit square
			double* point = haltonsequence->nextPoint();

			//count points inside/outside of the inscribed circle of the square
			double x = point[0] - 0.5;
			double y = point[1] - 0.5;
			if (x*x + y*y > 0.25) {
				numOutside++;
			} else {
				numInside++;
			}

			//report status
			i++;
			if (i % 1000 == 0) {
				sprintf(buff, "Generated %ld samples.", i);
				context.setStatus(string(buff));
			}
		}

		//output map results
		context.emit("true", to_string(numInside));
		context.emit("false", to_string(numOutside));
	}
};

class PiReducer : public HadoopPipes::Reducer {
public:
	long numInside;
	long numOutside;

	PiReducer(HadoopPipes::TaskContext& context) {
		numInside = 0;
		numOutside = 0;
	}
	
	void reduce(HadoopPipes::ReduceContext& context) {
#ifdef FDEBUG
		cout << "in reduce" << endl;
#endif
		HadoopPipes::JobConf* conf = (HadoopPipes::JobConf*)context.getJobConf();
		if (conf != NULL) {
			string s1 = conf->get("hadoop.pipes.pi.nummaps");
			numMaps = stol(s1);
			string s2 = conf->get("hadoop.pipes.pi.numsamples");
			numSamples = stol(s2);
		}

		while (context.nextValue()) {
			long val = stol(context.getInputValue(), NULL, 10);
			if (context.getInputKey().compare("true") == 0)
				numInside += val;
			else
				numOutside += val;
		}
	}

	void close() {
#ifdef WITH_HDFS
		char buff[1024];

		//write output to a file
		hdfsFile writeFile = hdfsOpenFile(dfs, TMP_DIR_OUT, O_WRONLY | O_CREAT, 0, 0, 0);
    		if(!writeFile) {
			cerr << "Failed to open " << TMP_DIR_OUT << " for writing!" << endl;
			return;
		}
		
		sprintf(buff, "%ld %ld\n", numInside, numOutside);
		hdfsWrite(dfs, writeFile, (void*)buff, strlen(buff)+1);
   		if (hdfsFlush(dfs, writeFile)) {
           		cerr << "Failed to flush " << TMP_DIR_OUT << endl;
			return;
		}
    		hdfsCloseFile(dfs, writeFile);

#else
#ifdef FDEBUG
		cout << "Reduce close, nMaps " << numMaps << " nSamples " << numSamples << endl;
#endif
		//compute estimated value
	        double epi = 4 * numInside / (double)(numMaps * numSamples);
        	cout << "Estimated value of Pi is " << epi << endl;
#endif			
	}
};

int main(int argc, char** argv) {	
	if (argc < 3) {
		// cerr << "Usage: " << argv[0] << " numMaps numSamples" << endl;
		cout << "Using default numMpas and numSamples ..." << endl;
	}
	else {
		cout << "No. of args " << argc << endl;
		numMaps = strtol(argv[1], NULL, 10);
		numSamples = strtol(argv[2], NULL, 10);
	}

	cout << "Number of Maps " << numMaps << endl;
	cout << "Samples per Map " << numSamples << endl;

#ifdef WITH_HDFS
	dfs = hdfsConnect("default", 0);

	//generate an input file for each map task
	for(i=0; i < numMaps; ++i) {		
		sprintf(path, "%s%d", TMP_DIR_IN, i);
		hdfsFile writeFile = hdfsOpenFile(dfs, path, O_WRONLY | O_CREAT, 0, 0, 0);
    		if(!writeFile) {
		          cerr << "Failed to open " << path << " for writing!" << endl;
		          return -2;
		}
		
		sprintf(buff, "%d %d\n", i * numSamples, numSamples);
		tSize n = hdfsWrite(dfs, writeFile, (void*)buff, strlen(buff)+1);
   		if (hdfsFlush(dfs, writeFile)) {
           		cerr << "Failed to flush " << path << endl;
			return -2;
		}
    		hdfsCloseFile(dfs, writeFile);

		cout << "Wrote input for Map #" << i << endl;
	}
#endif

	//start a map/reduce job
#ifdef FDEBUG
	cout << "Starting Pi" << endl;
#endif	
	int res = HadoopPipes::runTask(HadoopPipes::TemplateFactory<PiMapper, PiReducer>());
#ifdef FDEBUG
	cout << "Pi finished with return code " << res << endl;
#endif

#ifdef WITH_HDFS
	//read outputs
	long numInside = 0;
	long numOutside = 0;
	hdfsFile readFile = hdfsOpenFile(dfs, TMP_DIR_OUT, O_RDONLY, 0, 0, 0);
	int n = hdfsRead(dfs, readFile, buff, 1024);
	sscanf(buff, "%ld %ld", &numInside, &numOutside);
	hdfsCloseFile(dfs, readFile);

	//compute estimated value
	double epi = 4 * numInside / (numMaps * numSamples);
	cout << "Estimated value of Pi is " << epi << endl;

	hdfsDisconnect(dfs);
#endif

	return 0;
}

