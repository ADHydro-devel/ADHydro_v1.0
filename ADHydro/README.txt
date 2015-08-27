/*
 Copyright 2015 University of Wyoming

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

How to run ADHydro:

The latest version of ADHydro is checked in and pushed to the github repository.  To get a new copy of the repository:

git clone https://github.com/ADHydro-devel/ADHydro_v1.0.git

To build ADHydro you need to have Charm++ installed.

To build ADHydro you first need to build inih and NOAH-MP (in the directory HRLDAS-v3.6), then build ADHydro:

cd inih
make
cd ../HRLDAS-v3.6
make
cd ../ADHydro
make

ADHydro is set up to compile in debug mode with assertions turned on.  To compile in production mode edit all.h to change the following lines to whatever debug level you want to use.  NOTE: change the string DEBUG_LEVEL_DEVELOPMENT.  Do not change the string DEBUG_LEVEL.

// Set this macro to the debug level to use.
#define DEBUG_LEVEL (DEBUG_LEVEL_DEVELOPMENT)

To run ADHydro on one processor:

./adhydro <superfile>

To run ADHydro on more than one processor:

./charmrun +p<number of processors> ./adhydro <superfile>

An example superfile can be found in ADHydro

The input files you need are:

geometry.nc
parameter.nc
state.nc
forcing.nc
GENPARM.TBL
MPTABLE.TBL
SOILPARM.TBL
VEGPARM.TBL

The input files all need to be in the same directory.  The output files will all be put in the same directory.  The output directory must be different than the input directory because the output files have the same names as the input files.  The exception to this is if you are appending your output to your input files.  In that case, ADHydro will use the input directory as the output directory regardless of what is specified for output directory in the superfile.


