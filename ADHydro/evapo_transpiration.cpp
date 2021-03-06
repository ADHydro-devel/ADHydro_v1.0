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

#include "evapo_transpiration.h"
#include "adhydro.h"
#include "all.h"
#include <math.h>

#ifdef INTEL_COMPILER
#define READ_MP_VEG_PARAMETERS noahmp_veg_parameters_mp_read_mp_veg_parameters_
#define SOIL_VEG_GEN_PARM      module_sf_noahmpdrv_mp_soil_veg_gen_parm_
#define NOAHMP_OPTIONS         noahmp_routines_mp_noahmp_options_
#define NOAHMP_OPTIONS_GLACIER noahmp_glacier_routines_mp_noahmp_options_glacier_
#define REDPRM                 noahmp_routines_mp_redprm_
#define NOAHMP_SFLX            noahmp_routines_mp_noahmp_sflx_
#define NOAHMP_GLACIER         noahmp_glacier_routines_mp_noahmp_glacier_
#define NOAHMP_POROSITY        noahmp_globals_mp_smcmax_
#else // INTEL_COMPILER
#define READ_MP_VEG_PARAMETERS __noahmp_veg_parameters_MOD_read_mp_veg_parameters
#define SOIL_VEG_GEN_PARM      __module_sf_noahmpdrv_MOD_soil_veg_gen_parm
#define NOAHMP_OPTIONS         __noahmp_routines_MOD_noahmp_options
#define NOAHMP_OPTIONS_GLACIER __noahmp_glacier_routines_MOD_noahmp_options_glacier
#define REDPRM                 __noahmp_routines_MOD_redprm
#define NOAHMP_SFLX            __noahmp_routines_MOD_noahmp_sflx
#define NOAHMP_GLACIER         __noahmp_glacier_routines_MOD_noahmp_glacier
#define NOAHMP_POROSITY        __noahmp_globals_MOD_smcmax
#endif // INTEL_COMPILER

extern "C" void READ_MP_VEG_PARAMETERS(const char* landUse, const char* mpTableFile, size_t landUseSize, size_t mpTableFileSize);
extern "C" void SOIL_VEG_GEN_PARM(const char* landUse, const char* soil, const char* vegParmFile, const char* soilParmFile, const char* genParmFile,
                                  int* verbosityLevel, size_t landUseSize, size_t soilSize, size_t vegParmFileSize, size_t soilParmFileSize,
                                  size_t genParmFileSize);
extern "C" void NOAHMP_OPTIONS(int* dveg, int* optCrs, int* optBtr, int* optRun, int* optSfc, int* optFrz, int* optInf, int* optRad, int* optAlb, int* optSnf,
                               int* optTbot, int* optStc);
extern "C" void NOAHMP_OPTIONS_GLACIER(int* dveg, int* optCrs, int* optBtr, int* optRun, int* optSfc, int* optFrz, int* optInf, int* optRad, int* optAlb,
                                       int* optSnf, int* optTbot, int* optStc);
extern "C" void REDPRM(int* vegType, int* soilType, int* slopeType, float* zSoil, int* nSoil, int* isUrban);
extern "C" void NOAHMP_SFLX(int* iLoc, int* jLoc, float* lat, int* yearLen, float* julian, float* cosZ, float* dt, float* dx, float* dz8w, int* nSoil,
                            float* zSoil, int* nSnow, float* shdFac, float* shdMax, int* vegType, int* isUrban, int* ice, int* ist, int* isc, float* smcEq,
                            int* iz0tlnd, float* sfcTmp, float* sfcPrs, float* psfc, float* uu, float* vv, float* q2, float* qc, float* solDn, float* lwDn,
                            float* prcp, float* tBot, float* co2Air, float* o2Air, float* folN, float* fIceOld, float* pblh, float* zLvl, float* albOld,
                            float* snEqvO, float* stc, float* sh2o, float* smc, float* tah, float* eah, float* fWet, float* canLiq, float* canIce, float* tv,
                            float* tg, float* qsfc, float* qSnow, int* iSnow, float* zSnso, float* snowH, float* snEqv, float* snIce, float* snLiq, float* zwt,
                            float* wa, float* wt, float* wsLake, float* lfMass, float* rtMass, float* stMass, float* wood, float* stblCp, float* fastCp,
                            float* lai, float* sai, float* cm, float* ch, float* tauss, float* smcwtd, float* deepRech, float* rech, float* fsa, float* fsr,
                            float* fira, float* fsh, float* sSoil, float* fcev, float* fgev, float* fctr, float* ecan, float* etran, float* eDir, float* trad,
                            float* tgb, float* tgv, float* t2mv, float* t2mb, float* q2v, float* q2b, float* runSrf, float* runSub, float* apar, float* psn,
                            float* sav, float* sag, float* fSno, float* nee, float* gpp, float* npp, float* fVeg, float* albedo, float* qsnBot, float* ponding,
                            float* ponding1, float* ponding2, float* rsSun, float* rsSha, float* bGap, float* wGap, float* chv, float* chb, float* emissi,
                            float* shg, float* shc, float* shb, float* evg, float* evb, float* ghv, float* ghb, float* irg, float* irc, float* irb, float* tr,
                            float* evc, float* chLeaf, float* chuc, float* chv2, float* chb2, float* fpIce);
extern "C" void NOAHMP_GLACIER(int* iLoc, int* jLoc, float* cosZ, int* nSnow, int* nSoil, float* dt, float* sfcTmp, float* sfcPrs, float* uu, float* vv,
                               float* q2, float* solDn, float* prcp, float* lwDn, float* tBot, float* zLvl, float* fIceOld, float* zSoil, float* qSnow,
                               float* snEqvO, float* albOld, float* cm, float* ch, int* iSnow, float* snEqv, float* smc, float* zSnso, float* snowH,
                               float* snIce, float* snLiq, float* tg, float* stc, float* sh2o, float* tauss, float* qsfc, float* fsa, float* fsr, float* fira,
                               float* fsh, float* fgev, float* sSoil, float* trad, float* eDir, float* runSrf, float* runSub, float* sag, float* albedo,
                               float* qsnBot, float* ponding, float* ponding1, float* ponding2, float* t2m, float* q2e, float* emissi, float* fpIce,
                               float* ch2b);
extern float NOAHMP_POROSITY;

bool evapoTranspirationInit(const char* directory)
{
  bool        error          = false;                   // Error flag.
  const char* landUse        = "USGS";                  // Land use data set.
  const char* soil           = "STAS";                  // Soil type data set.
  char*       mpTableFile    = NULL;                    // File name for MPTABLE.TBL file.
  char*       vegParmFile    = NULL;                    // File name for VEGPARM.TBL file.
  char*       soilParmFile   = NULL;                    // File name for SOILPARM.TBL file.
  char*       genParmFile    = NULL;                    // File name for GENPARM.TBL file.
  size_t      fileStringSize;                           // Size of buffer allocated for file name strings.
  size_t      numPrinted;                               // Used to check that snprintf printed the correct number of characters.
  int         verbosityLevel = ADHydro::verbosityLevel; // For passing verbosity level into Noah.  I'm not comfortable passing a class static variable by
                                                        // reference into foreign code.
  
  // =====================================options for different schemes================================
  // From module_sf_noahmplsm.F
  
  // options for dynamic vegetation:
  // 1 -> off (use table LAI; use FVEG = SHDFAC from input)
  // 2 -> on (together with OPT_CRS = 1)
  // 3 -> off (use table LAI; calculate FVEG)
  // 4 -> off (use table LAI; use maximum vegetation fraction)

  int dveg = 3;

  // options for canopy stomatal resistance
  // 1-> Ball-Berry; 2->Jarvis

  int optCrs = 1;

  // options for soil moisture factor for stomatal resistance
  // 1-> Noah (soil moisture)
  // 2-> CLM  (matric potential)
  // 3-> SSiB (matric potential)

  int optBtr = 1;

  // options for runoff and groundwater
  // 1 -> TOPMODEL with groundwater (Niu et al. 2007 JGR) ;
  // 2 -> TOPMODEL with an equilibrium water table (Niu et al. 2005 JGR) ;
  // 3 -> original surface and subsurface runoff (free drainage)
  // 4 -> BATS surface and subsurface runoff (free drainage)
  // 5 -> Miguez-Macho&Fan groundwater scheme (Miguez-Macho et al. 2007 JGR, lateral flow: Fan et al. 2007 JGR)

  int optRun = 1;

  // options for surface layer drag coeff (CH & CM)
  // 1->M-O ; 2->original Noah (Chen97); 3->MYJ consistent; 4->YSU consistent.

  int optSfc = 1;

  // options for supercooled liquid water (or ice fraction)
  // 1-> no iteration (Niu and Yang, 2006 JHM); 2: Koren's iteration

  int optFrz = 1;

  // options for frozen soil permeability
  // 1 -> linear effects, more permeable (Niu and Yang, 2006, JHM)
  // 2 -> nonlinear effects, less permeable (old)

  int optInf = 1;

  // options for radiation transfer
  // 1 -> modified two-stream (gap = F(solar angle, 3D structure ...)<1-FVEG)
  // 2 -> two-stream applied to grid-cell (gap = 0)
  // 3 -> two-stream applied to vegetated fraction (gap=1-FVEG)

  int optRad = 1;

  // options for ground snow surface albedo
  // 1-> BATS; 2 -> CLASS

  int optAlb = 2;

  // options for partitioning  precipitation into rainfall & snowfall
  // 1 -> Jordan (1991); 2 -> BATS: when SFCTMP<TFRZ+2.2 ; 3-> SFCTMP<TFRZ

  int optSnf = 1;

  // options for lower boundary condition of soil temperature
  // 1 -> zero heat flux from bottom (ZBOT and TBOT not used)
  // 2 -> TBOT at ZBOT (8m) read from a file (original Noah)

  int optTbot = 2;

  // options for snow/soil temperature time scheme (only layer 1)
  // 1 -> semi-implicit; 2 -> full implicit (original Noah)

  int optStc  = 1;
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != directory))
    {
      CkError("ERROR in evapoTranspirationInit: directory must not be null.\n");
      error = true;
    }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)

  if (!error)
    {
      // Allocate space for file name strings.
      fileStringSize = strlen(directory) + strlen("/SOILPARM.TBL") + 1; // The longest file name is SOILPARM.TBL.  +1 for null terminating character.
      mpTableFile    = new char[fileStringSize];
      vegParmFile    = new char[fileStringSize];
      soilParmFile   = new char[fileStringSize];
      genParmFile    = new char[fileStringSize];

      // Create file name.
      numPrinted = snprintf(mpTableFile, fileStringSize, "%s/MPTABLE.TBL", directory);

#if (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
      if (!(strlen(directory) + strlen("/MPTABLE.TBL") == numPrinted && numPrinted < fileStringSize))
        {
          CkError("ERROR in evapoTranspirationInit: incorrect return value of snprintf when generating mp table file name %s.  "
                  "%d should be equal to %d and less than %d.\n", mpTableFile, numPrinted, strlen(directory) + strlen("/MPTABLE.TBL"), fileStringSize);
          error = true;
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
    }

  // Create file name.
  if (!error)
    {
      numPrinted = snprintf(vegParmFile, fileStringSize, "%s/VEGPARM.TBL", directory);

#if (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
      if (!(strlen(directory) + strlen("/VEGPARM.TBL") == numPrinted && numPrinted < fileStringSize))
        {
          CkError("ERROR in evapoTranspirationInit: incorrect return value of snprintf when generating veg parm file name %s.  "
                  "%d should be equal to %d and less than %d.\n", vegParmFile, numPrinted, strlen(directory) + strlen("/VEGPARM.TBL"), fileStringSize);
          error = true;
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
    }

  // Create file name.
  if (!error)
    {
      numPrinted = snprintf(soilParmFile, fileStringSize, "%s/SOILPARM.TBL", directory);

#if (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
      if (!(strlen(directory) + strlen("/SOILPARM.TBL") == numPrinted && numPrinted < fileStringSize))
        {
          CkError("ERROR in evapoTranspirationInit: incorrect return value of snprintf when generating soil parm file name %s.  "
                  "%d should be equal to %d and less than %d.\n", soilParmFile, numPrinted, strlen(directory) + strlen("/SOILPARM.TBL"), fileStringSize);
          error = true;
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
    }

  // Create file name.
  if (!error)
    {
      numPrinted = snprintf(genParmFile, fileStringSize, "%s/GENPARM.TBL", directory);

#if (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
      if (!(strlen(directory) + strlen("/GENPARM.TBL") == numPrinted && numPrinted < fileStringSize))
        {
          CkError("ERROR in evapoTranspirationInit: incorrect return value of snprintf when generating veg parm file name %s.  "
                  "%d should be equal to %d and less than %d.\n", genParmFile, numPrinted, strlen(directory) + strlen("/GENPARM.TBL"), fileStringSize);
          error = true;
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_LIBRARY_ERRORS)
    }
  
  if (!error)
    {
      READ_MP_VEG_PARAMETERS(landUse, mpTableFile, strlen(landUse), strlen(mpTableFile));
      SOIL_VEG_GEN_PARM(landUse, soil, vegParmFile, soilParmFile, genParmFile, &verbosityLevel, strlen(landUse), strlen(soil), strlen(vegParmFile),
                        strlen(soilParmFile), strlen(genParmFile));
      NOAHMP_OPTIONS(&dveg, &optCrs, &optBtr, &optRun, &optSfc, &optFrz, &optInf, &optRad, &optAlb, &optSnf, &optTbot, &optStc);
      NOAHMP_OPTIONS_GLACIER(&dveg, &optCrs, &optBtr, &optRun, &optSfc, &optFrz, &optInf, &optRad, &optAlb, &optSnf, &optTbot, &optStc);
    }
  
  deleteArrayIfNonNull(&mpTableFile);
  deleteArrayIfNonNull(&vegParmFile);
  deleteArrayIfNonNull(&soilParmFile);
  deleteArrayIfNonNull(&genParmFile);
  
  return error;
}

bool evapoTranspirationSoil(int vegType, int soilType, float lat, int yearLen, float julian, float cosZ, float dt, float dx, float dz8w, float shdFac,
                            float shdMax, float smcEq[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS], float sfcTmp, float sfcPrs, float psfc, float uu, float vv,
                            float q2, float qc, float solDn, float lwDn, float prcp, float tBot, float pblh,
                            float sh2o[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS], float smc[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS], float zwt,
                            float smcwtd, EvapoTranspirationStateStruct* evapoTranspirationState, float* surfacewaterAdd, float* evaporationFromCanopy,
                            float* evaporationFromSnow, float* evaporationFromGround, float* transpiration, float* waterError)
{
  bool  error = false; // Error flag.
  int   ii;            // Loop counter.
  
  // Input parameters for redprm function.  Some are also used for sflx function.
  int   slopeType = 8;                                         // I just arbitrarily chose a slope type with zero slope.  I think slope is only used to
                                                               // calculate runoff, which we ignore.
  float zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];      // Layer bottom depth in meters from soil surface of each soil layer.  Values are set below from
                                                               // zSnso.
  int   nSoil     = EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; // Always use four soil layers.
  int   isUrban   = 1;                                         // USGS vegetation type for urban land.
  
  // Input parameters to sflx function.
  int   iLoc    = 1;                                         // Grid location index, unused.
  int   jLoc    = 1;                                         // Grid location index, unused.
  int   nSnow   = EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; // Maximum number of snow layers.  Always pass 3.
  int   ice     = 0;                                         // Flag to indicate permanent ice cover, 0 for no, 1 for yes.  Always pass 0.  If permanent ice
                                                             // cover call evapoTranspirationGlacier instead.
  int   ist     = 1;                                         // Flag to indicate permanent water cover, 1 for soil, 2 for lake.  Always pass 1.  If permanent
                                                             // water cover call evapoTranspirationWater instead.
  int   isc     = 4;                                         // Soil color type, 1 for lightest to 8 for darkest.  Always pass 4 unless we find a data source
                                                             // for soil color.
  int   iz0tlnd = 0;                                         // Unused.
  float co2Air  = 0.0004f * sfcPrs;                          // CO2 partial pressure in Pascal at surface.  Always pass 400 parts per million of surface
                                                             // pressure.
  float o2Air   = 0.21f * sfcPrs;                            // O2 partial pressure in Pascal at surface.  Always pass 21 percent of surface pressure.
  float folN    = 3.0f;                                      // Foliage nitrogen percentage, 0.0 to 100.0.  Always pass 3.0 unless we find a data source for
                                                             // foliage nitrogen.
  float fIce[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];     // Frozen fraction of each snow layer, unitless.  Values are set below from snIce and snLiq.
  float zLvl    = dz8w;                                      // Thickness in meters of lowest atmosphere layer in forcing data.  Redundant with dz8w.
  
  // Input/output parameters to sflx function.
  float qsfc   = q2;   // Water vapor mixing ratio at middle of lowest atmosphere layer in forcing data, unitless.  Redundant with q2.
  float qSnow  = NAN;  // This is actually an output only variable.  Snowfall rate below the canopy in millimeters of water equivalent per second.
  float wa;            // Water stored in aquifer in millimeters of water.  Value is set below from zwt.
  float wt;            // Water stored in aquifer and saturated soil in millimeters of water.  Value is set below from zwt, zSoil, and NOAHMP_POROSITY.
  float wsLake = 0.0f; // Water stored in lakes in millimeters of water.  Because we separate waterbodies from the mesh, locations with a soil surface have no
                       // included lake storage.
  
  // Output parameters to sflx function.  Set to NAN so we can detect if the values are used before being set.
  float fsa      = NAN; // Unused.
  float fsr      = NAN; // Unused.
  float fira     = NAN; // Unused.
  float fsh      = NAN; // Unused.
  float sSoil    = NAN; // Unused.
  float fcev     = NAN; // Unused.
  float fgev     = NAN; // Unused.
  float fctr     = NAN; // Unused.
  float eCan     = NAN; // Evaporation rate from canopy in millimeters of water equivalent per second.
  float eTran    = NAN; // Evaporation rate from transpiration in millimeters of water equivalent per second.
  float eDir     = NAN; // Evaporation rate from surface in millimeters of water equivalent per second.
  float tRad     = NAN; // Unused.
  float tgb      = NAN; // Unused.
  float tgv      = NAN; // Unused.
  float t2mv     = NAN; // Unused.
  float t2mb     = NAN; // Unused.
  float q2v      = NAN; // Unused.
  float q2b      = NAN; // Unused.
  float runSrf   = NAN; // Infiltration excess runoff in millimeters of water per second.
  float runSub   = NAN; // Saturation excess runoff in millimeters of water per second.
  float apar     = NAN; // Unused.
  float psn      = NAN; // Unused.
  float sav      = NAN; // Unused.
  float sag      = NAN; // Unused.
  float fSno     = NAN; // Unused.
  float nee      = NAN; // Unused.
  float gpp      = NAN; // Unused.
  float npp      = NAN; // Unused.
  float fVeg     = NAN; // Unused.
  float albedo   = NAN; // Unused.
  float qSnBot   = NAN; // Snowmelt rate from the bottom of the snow pack in millimeters of water per second.
  float ponding  = NAN; // Unused.
  float ponding1 = NAN; // Unused.
  float ponding2 = NAN; // Unused.
  float rsSun    = NAN; // Unused.
  float rsSha    = NAN; // Unused.
  float bGap     = NAN; // Unused.
  float wGap     = NAN; // Unused.
  float chv      = NAN; // Unused.
  float chb      = NAN; // Unused.
  float emissi   = NAN; // Unused.
  float shg      = NAN; // Unused.
  float shc      = NAN; // Unused.
  float shb      = NAN; // Unused.
  float evg      = NAN; // Unused.
  float evb      = NAN; // Unused.
  float ghv      = NAN; // Unused.
  float ghb      = NAN; // Unused.
  float irg      = NAN; // Unused.
  float irc      = NAN; // Unused.
  float irb      = NAN; // Unused.
  float tr       = NAN; // Unused.
  float evc      = NAN; // Unused.
  float chLeaf   = NAN; // Unused.
  float chuc     = NAN; // Unused.
  float chv2     = NAN; // Unused.
  float chb2     = NAN; // Unused.
  float fpIce    = NAN; // Fraction of precipitation that is frozen, unitless.
  
  // Derived output variables.
  float canIceOriginal;              // Quantity of canopy ice before timestep in millimeters of water equivalent.
  float changeInCanopyIce;           // Change in canopy ice during timestep in millimeters of water equivalent.  Positive means the amount of canopy ice
                                     // increased.
  float canLiqOriginal;              // Quantity of canopy liquid before timestep in millimeters of water.
  float changeInCanopyLiquid;        // Change in canopy liquid during timestep in millimeters of water.  Positive means the amount of canopy liquid increased.
  int   iSnowOriginal;               // Actual number of snow layers before timestep.
  float evaporationFromSurface;      // Quantity of evaporation from the surface in millimeters of water equivalent.  Positive means water evaporated off of
                                     // the surface.  Negative means water condensed on to the surface.  Surface evaporation sometimes comes from snow and is
                                     // taken out by Noah-MP, but if there is not enough snow we have to take the evaporation from the water in the ADHydro
                                     // state variables.
  float snowfallAboveCanopy;         // Quantity of snowfall above the canopy in millimeters of water equivalent.  Must be non-negative.
  float snowfallInterceptedByCanopy; // Quantity of snowfall intercepted by the canopy in millimeters of water equivalent.  Can be negative if snow is falling
                                     // off of the canopy.
  float snowfallBelowCanopy;         // Quantity of snowfall below the canopy in millimeters of water equivalent.  Must be non-negative.
  float snowmeltOnGround;            // Quantity of water that reaches the ground from the snow layer in millimeters of water.  Must be non-negative.
  float rainfallAboveCanopy;         // Quantity of rainfall above the canopy in millimeters of water.  Must be non-negative.
  float rainfallInterceptedByCanopy; // Quantity of rainfall intercepted by the canopy in millimeters of water.  Can be negative if rain is dripping from the
                                     // canopy.
  float rainfallBelowCanopy;         // Quantity of rainfall below the canopy in millimeters of water.  Must be non-negative.
  float rainfallInterceptedBySnow;   // Quantity of rainfall intercepted by the snow layer in millimeters of water.  Must be non-negative.
  float rainfallOnGround;            // Quantity of rainfall that reaches the ground in millimeters of water.  Must be non-negative.
  float snEqvOriginal;               // Quantity of water in the snow layer(s) before timestep in millimeters of water equivalent.  snEqvO is not always set to
                                     // this value after the timestep.
  float snEqvShouldBe;               // If snEqv falls below 0.001 Noah-MP sets it to zero, or if it rises above 2000.0 Noah-MP sets it to 2000.0.  We don't
                                     // want this behavior so in this variable we calculate what snEqv should be and set it back.  If snEqv is not changed this
                                     // instead performs a mass balance check.
  float runoff;                      // Quantity of water that runs off from the soil in millimeters of water.  We don't use Noah-MP's infiltration and
                                     // groundwater simulation so this is merely for a mass balance check.
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
  // Variables used for assertions.
  int   iLocOriginal         = iLoc;
  int   jLocOriginal         = jLoc;
  float latOriginal          = lat;
  int   yearLenOriginal      = yearLen;
  float julianOriginal       = julian;
  float cosZOriginal         = cosZ;
  float dtOriginal           = dt;
  float dxOriginal           = dx;
  float dz8wOriginal         = dz8w;
  int   nSoilOriginal        = nSoil;
  float zSoilOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];
  int   nSnowOriginal        = nSnow;
  float shdFacOriginal       = shdFac;
  float shdMaxOriginal       = shdMax;
  int   vegTypeOriginal      = vegType;
  int   isUrbanOriginal      = isUrban;
  int   iceOriginal          = ice;
  int   istOriginal          = ist;
  int   iscOriginal          = isc;
  float smcEqOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];
  int   iz0tlndOriginal      = iz0tlnd;
  float sfcTmpOriginal       = sfcTmp;
  float sfcPrsOriginal       = sfcPrs;
  float psfcOriginal         = psfc;
  float uuOriginal           = uu;
  float vvOriginal           = vv;
  float q2Original           = q2;
  float qcOriginal           = qc;
  float solDnOriginal        = solDn;
  float lwDnOriginal         = lwDn;
  float prcpOriginal         = prcp;
  float tBotOriginal         = tBot;
  float co2AirOriginal       = co2Air;
  float o2AirOriginal        = o2Air;
  float folNOriginal         = folN;
  float fIceOldOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];
  float pblhOriginal         = pblh;
  float zLvlOriginal         = zLvl;
  float soilMoistureOriginal = 0.0f; // Total soil moisture before timestep in millimeters of water.
  float soilMoistureNew      = 0.0f; // Total soil moisture after timestep in millimeters of water.
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(1 <= vegType && 27 >= vegType))
    {
      CkError("ERROR in evapoTranspirationSoil: USGS vegetation type must be greater than or equal to 1 and less than or equal to 27.\n");
      error = true;
    }

  if (16 == vegType)
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: vegetation type is 'Water Bodies'.  Call evapoTranspirationWater instead.\n");
        }
    }

  if (24 == vegType)
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: vegetation type is 'Snow or Ice'.  Call evapoTranspirationGlacier instead.\n");
        }
    }

  if (!(1 <= soilType && 19 >= soilType))
    {
      CkError("ERROR in evapoTranspirationSoil: STAS soil type must be greater than or equal to 1 and less than or equal to 19.\n");
      error = true;
    }

  if (14 == soilType)
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: soil type is 'WATER'.  Call evapoTranspirationWater instead.\n");
        }
    }

  if (16 == soilType)
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: soil type is 'OTHER(land-ice)'.  Call evapoTranspirationGlacier instead.\n");
        }
    }
  
  if (!(-M_PI / 2.0f <= lat && M_PI / 2.0f >= lat))
    {
      CkError("ERROR in evapoTranspirationSoil: lat must be greater than or equal to -pi/2 and less than or equal to pi/2.\n");
      error = true;
    }
  
  if (!(365 == yearLen || 366 == yearLen))
    {
      CkError("ERROR in evapoTranspirationSoil: yearLen must be 365 or 366.\n");
      error = true;
    }
  
  if (!(0.0f <= julian && julian <= yearLen))
    {
      CkError("ERROR in evapoTranspirationSoil: julian must be greater than or equal to zero and less than or equal to yearLen.\n");
      error = true;
    }
  
  if (!(0.0f <= cosZ && 1.0f >= cosZ))
    {
      CkError("ERROR in evapoTranspirationSoil: cosZ must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }
  
  if (!(0.0f < dt))
    {
      CkError("ERROR in evapoTranspirationSoil: dt must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f < dx))
    {
      CkError("ERROR in evapoTranspirationSoil: dx must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f < dz8w))
    {
      CkError("ERROR in evapoTranspirationSoil: dz8w must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f <= shdFac && shdFac <= shdMax && 1.0f >= shdMax))
    {
      CkError("ERROR in evapoTranspirationSoil: shdFac must be greater than or equal to zero and less than or equal to shdMax.  shdMax must be less than or "
              "equal to one.\n");
      error = true;
    }

  if (!(NULL != smcEq))
    {
      CkError("ERROR in evapoTranspirationSoil: smcEq must not be NULL.\n");
      error = true;
    }
  
  // Values of smcEq will be error checked later.  They need to be compared against NOAHMP_POROSITY, which is not set until after we call REDPRM.

  if (!(0.0f <= sfcTmp))
    {
      CkError("ERROR in evapoTranspirationSoil: sfcTmp must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= sfcTmp))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: sfcTmp below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= sfcTmp))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: sfcTmp above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= sfcPrs))
    {
      CkError("ERROR in evapoTranspirationSoil: sfcPrs must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(35000.0f <= sfcPrs))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: sfcPrs below 35 kPa.\n");
        }
    }

  if (!(0.0f <= psfc))
    {
      CkError("ERROR in evapoTranspirationSoil: psfc must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(35000.0f <= psfc))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: psfc below 35 kPa.\n");
        }
    }
  
  if (!(100.0f >= fabs(uu)))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: magnitude of uu greater than 100 m/s.\n");
        }
    }
  
  if (!(100.0f >= fabs(vv)))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: magnitude of vv greater than 100 m/s.\n");
        }
    }
  
  if (!(0.0f <= q2 && 1.0f >= q2))
    {
      CkError("ERROR in evapoTranspirationSoil: q2 must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }
  
  if (!(0.0f <= qc && 1.0f >= qc))
    {
      CkError("ERROR in evapoTranspirationSoil: qc must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }

  if (!(0.0f <= solDn))
    {
      CkError("ERROR in evapoTranspirationSoil: solDn must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= lwDn))
    {
      CkError("ERROR in evapoTranspirationSoil: lwDn must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= prcp))
    {
      CkError("ERROR in evapoTranspirationSoil: prcp must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= tBot))
    {
      CkError("ERROR in evapoTranspirationSoil: tBot must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= tBot))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: tBot below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= tBot))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationSoil: tBot above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= pblh))
    {
      CkError("ERROR in evapoTranspirationSoil: pblh must be greater than or equal to zero.\n");
      error = true;
    }
  
  if (!(NULL != sh2o))
    {
      CkError("ERROR in evapoTranspirationSoil: sh2o must not be NULL.\n");
      error = true;
    }
  
  // Values of sh2o will be error checked later.  They need to be compared against NOAHMP_POROSITY, which is not set until after we call REDPRM.
  
  if (!(NULL != smc))
    {
      CkError("ERROR in evapoTranspirationSoil: smc must not be NULL.\n");
      error = true;
    }
  
  // Values of smc will be error checked later.  They need to be compared against NOAHMP_POROSITY, which is not set until after we call REDPRM.

  if (!(0.0f <= zwt))
    {
      CkError("ERROR in evapoTranspirationSoil: zwt must be greater than or equal to zero.\n");
      error = true;
    }
  
  // smcwtd will be error checked later.  It needs to be compared against NOAHMP_POROSITY, which is not set until after we call REDPRM.
  
  if (!(NULL != evapoTranspirationState))
    {
      CkError("ERROR in evapoTranspirationSoil: evapoTranspirationState must not be NULL.\n");
      error = true;
    }

  if (!(NULL != surfacewaterAdd))
    {
      CkError("ERROR in evapoTranspirationSoil: surfacewaterAdd must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *surfacewaterAdd = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromCanopy))
    {
      CkError("ERROR in evapoTranspirationSoil: evaporationFromCanopy must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromCanopy = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromSnow))
    {
      CkError("ERROR in evapoTranspirationSoil: evaporationFromSnow must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromSnow = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromGround))
    {
      CkError("ERROR in evapoTranspirationSoil: evaporationFromGround must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromGround = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != transpiration))
    {
      CkError("ERROR in evapoTranspirationSoil: transpiration must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *transpiration = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != waterError))
    {
      CkError("ERROR in evapoTranspirationSoil: waterError must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *waterError = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_INVARIANTS)
  if (!error)
    {
      error = checkEvapoTranspirationStateStructInvariant(evapoTranspirationState);
    }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_INVARIANTS)

  if (!error)
    {
      // Set variables that require error checking of pointers first.
      canIceOriginal = evapoTranspirationState->canIce;
      canLiqOriginal = evapoTranspirationState->canLiq;
      iSnowOriginal  = evapoTranspirationState->iSnow;
      snEqvOriginal  = evapoTranspirationState->snEqv;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          smcEqOriginal[ii] = smcEq[ii];
        }
      
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          fIceOldOriginal[ii] = evapoTranspirationState->fIceOld[ii];
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Set zSoil from zSnso.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (0 > evapoTranspirationState->iSnow)
            {
              zSoil[ii] = evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS] -
                          evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1];
            }
          else
            {
              zSoil[ii] = evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];
            }

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          zSoilOriginal[ii] = zSoil[ii];
          
          // Calculate soil moisture at the beginning of the timestep.
          if (0 == ii)
            {
              soilMoistureOriginal += smc[ii] * -zSoil[ii] * 1000.0f; // * 1000.0f because zSoil is in meters and soilMoistureOriginal is in millimeters.
            }
          else
            {
              soilMoistureOriginal += smc[ii] * (zSoil[ii - 1] - zSoil[ii]) * 1000.0f;
            }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
        }

      // Calculate fIce at the beginning of the timestep.
      for (ii = evapoTranspirationState->iSnow + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          fIce[ii] = evapoTranspirationState->snIce[ii] / (evapoTranspirationState->snIce[ii] + evapoTranspirationState->snLiq[ii]);
        }
      
      // Set Noah-MP globals.
      REDPRM(&vegType, &soilType, &slopeType, zSoil, &nSoil, &isUrban);
      
      // Calculate water in the aquifer and saturated soil.
      if (zwt > -zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS - 1])
        {
          // If the water table is deeper than the bottom of the lowest soil layer then the amount of water in the aquifer is the total capacity of the
          // aquifer, 5000 mm, minus the dry part, which is the distance of the water table below the bottom of the lowest soil layer times 1000 to convert
          // from meters to millimeters times the specific yield of 0.2.
          wa = 5000.0f - (zwt + zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS - 1]) * 1000.0f * 0.2f;
          
          // Prevent wa form being negative
          if (0.0f > wa)
            {
              wa = 0.0f;
            }
          
          // None of the water in the soil layers is in saturated soil so wt is the same as wa.
          wt = wa;
        }
      else
        {
          // If the water table is not deeper than the bottom of the lowest soil layer then the aquifer is completely full, and saturated soil includes the
          // distance of the water table above the bottom of the lowest soil layer times 1000 to convert from meters to millimeters times the porosity.  The
          // global variable NOAHMP_POROSITY gets set in REDPRM so we can't do this before here.
          wa = 5000.0f;
          wt = wa - (zwt + zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS - 1]) * 1000.0f * NOAHMP_POROSITY;
        }
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      // Include water in the aquifer in the mass balance check.
      soilMoistureOriginal += wa;
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
      // Error check that none of the soil moisture variables are greater than NOAHMP_POROSITY.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (!(0.0f < smcEq[ii] && smcEq[ii] <= NOAHMP_POROSITY))
            {
              CkError("ERROR in evapoTranspirationSoil: smcEq must be greater than zero and less than or equal to NOAHMP_POROSITY.\n");
              error = true;
            }
          
          if (!(0.0f < smc[ii] && smc[ii] <= NOAHMP_POROSITY))
            {
              CkError("ERROR in evapoTranspirationSoil: smc must be greater than zero and less than or equal to NOAHMP_POROSITY.\n");
              error = true;
            }
          
          if (!(0.0f < sh2o[ii] && sh2o[ii] <= smc[ii]))
            {
              CkError("ERROR in evapoTranspirationSoil: sh2o must be greater than zero and less than or equal to smc[ii].\n");
              error = true;
            }
        }
      
      if (!(0.0f < smcwtd && smcwtd <= NOAHMP_POROSITY))
        {
          CkError("ERROR in evapoTranspirationSoil: smcwtd must be greater than zero and less than or equal to NOAHMP_POROSITY.\n");
          error = true;
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
      
      // Run Noah-MP.
      NOAHMP_SFLX(&iLoc, &jLoc, &lat, &yearLen, &julian, &cosZ, &dt, &dx, &dz8w, &nSoil, zSoil, &nSnow, &shdFac, &shdMax, &vegType, &isUrban, &ice, &ist, &isc,
                  smcEq, &iz0tlnd, &sfcTmp, &sfcPrs, &psfc, &uu, &vv, &q2, &qc, &solDn, &lwDn, &prcp, &tBot, &co2Air, &o2Air, &folN,
                  evapoTranspirationState->fIceOld, &pblh, &zLvl, &evapoTranspirationState->albOld, &evapoTranspirationState->snEqvO,
                  evapoTranspirationState->stc, sh2o, smc, &evapoTranspirationState->tah, &evapoTranspirationState->eah, &evapoTranspirationState->fWet,
                  &evapoTranspirationState->canLiq, &evapoTranspirationState->canIce, &evapoTranspirationState->tv, &evapoTranspirationState->tg, &qsfc,
                  &qSnow, &evapoTranspirationState->iSnow, evapoTranspirationState->zSnso, &evapoTranspirationState->snowH, &evapoTranspirationState->snEqv,
                  evapoTranspirationState->snIce, evapoTranspirationState->snLiq, &zwt, &wa, &wt, &wsLake, &evapoTranspirationState->lfMass,
                  &evapoTranspirationState->rtMass, &evapoTranspirationState->stMass, &evapoTranspirationState->wood, &evapoTranspirationState->stblCp,
                  &evapoTranspirationState->fastCp, &evapoTranspirationState->lai, &evapoTranspirationState->sai, &evapoTranspirationState->cm,
                  &evapoTranspirationState->ch, &evapoTranspirationState->tauss, &smcwtd, &evapoTranspirationState->deepRech, &evapoTranspirationState->rech,
                  &fsa, &fsr, &fira, &fsh, &sSoil, &fcev, &fgev, &fctr, &eCan, &eTran, &eDir, &tRad, &tgb, &tgv, &t2mv, &t2mb, &q2v, &q2b, &runSrf, &runSub,
                  &apar, &psn, &sav, &sag, &fSno, &nee, &gpp, &npp, &fVeg, &albedo, &qSnBot, &ponding, &ponding1, &ponding2, &rsSun, &rsSha, &bGap, &wGap,
                  &chv, &chb, &emissi, &shg, &shc, &shb, &evg, &evb, &ghv, &ghb, &irg, &irc, &irb, &tr, &evc, &chLeaf, &chuc, &chv2, &chb2, &fpIce);
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      // Verify that the input variables have not changed.
      CkAssert(iLocOriginal == iLoc && jLocOriginal == jLoc && latOriginal == lat && yearLenOriginal == yearLen && julianOriginal == julian &&
               cosZOriginal == cosZ && dtOriginal == dt && dxOriginal == dx && dz8wOriginal == dz8w && nSoilOriginal == nSoil &&
               zSoilOriginal[0] == zSoil[0] && zSoilOriginal[1] == zSoil[1] && zSoilOriginal[2] == zSoil[2] && zSoilOriginal[3] == zSoil[3] &&
               nSnowOriginal == nSnow && shdFacOriginal == shdFac && shdMaxOriginal == shdMax && vegTypeOriginal == vegType && isUrbanOriginal == isUrban &&
               iceOriginal == ice && istOriginal == ist && iscOriginal == isc && smcEqOriginal[0] == smcEq[0] && smcEqOriginal[1] == smcEq[1] &&
               smcEqOriginal[2] == smcEq[2] && smcEqOriginal[3] == smcEq[3] && iz0tlndOriginal == iz0tlnd && sfcTmpOriginal == sfcTmp &&
               sfcPrsOriginal == sfcPrs && psfcOriginal == psfc && uuOriginal == uu && vvOriginal == vv && q2Original == q2 && qcOriginal == qc &&
               solDnOriginal == solDn && lwDnOriginal == lwDn && prcpOriginal == prcp && tBotOriginal == tBot && co2AirOriginal == co2Air &&
               o2AirOriginal == o2Air && folNOriginal == folN && fIceOldOriginal[0] == evapoTranspirationState->fIceOld[0] &&
               fIceOldOriginal[1] == evapoTranspirationState->fIceOld[1] && fIceOldOriginal[2] == evapoTranspirationState->fIceOld[2] &&
               pblhOriginal == pblh && zLvlOriginal == zLvl);
      
      // Verify that the fraction of the precipitation that falls as snow is between 0 and 1, the snowfall rate below the canopy is not negative, and the
      // snowmelt out the bottom of the snowpack is not negative.
      CkAssert(0.0f <= fpIce && 1.0f >= fpIce && 0.0f <= qSnow && 0.0f <= qSnBot);
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Store fIce from the beginning of the timestep in fIceOld.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          if (ii >= iSnowOriginal + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS)
            {
              evapoTranspirationState->fIceOld[ii] = fIce[ii];
            }
          else
            {
              evapoTranspirationState->fIceOld[ii] = 0.0f;
            }
        }
      
      // Calculate derived output variables.
      changeInCanopyIce      = evapoTranspirationState->canIce - canIceOriginal;
      changeInCanopyLiquid   = evapoTranspirationState->canLiq - canLiqOriginal;
      *evaporationFromCanopy = eCan * dt;
      evaporationFromSurface = eDir * dt;
      
      // Surface condensation is added to the snow layer if any snow exists or otherwise added to the ground as water.  Surface evaporation is taken first from
      // the snow layer up to the amount in snEqvO, and then any remaining is taken from the ground.
      if (0.0f > evaporationFromSurface)
        {
          if (0.0f < evapoTranspirationState->snEqvO)
            {
              *evaporationFromSnow   = evaporationFromSurface;
              *evaporationFromGround = 0.0f;
            }
          else
            {
              *evaporationFromSnow   = 0.0f;
              *evaporationFromGround = evaporationFromSurface;
            }
        }
      else if (evaporationFromSurface <= evapoTranspirationState->snEqvO)
        {
          *evaporationFromSnow   = evaporationFromSurface;
          *evaporationFromGround = 0.0f;
        }
      else
        {
          *evaporationFromSnow   = evapoTranspirationState->snEqvO;
          *evaporationFromGround = evaporationFromSurface - evapoTranspirationState->snEqvO;
        }
      
      snowfallAboveCanopy         = prcp * dt * fpIce;
      snowfallBelowCanopy         = qSnow * dt;
      snowfallInterceptedByCanopy = snowfallAboveCanopy - snowfallBelowCanopy;
      snowmeltOnGround            = qSnBot * dt;
      rainfallAboveCanopy         = prcp * dt - snowfallAboveCanopy;
      rainfallInterceptedByCanopy = changeInCanopyIce + changeInCanopyLiquid + *evaporationFromCanopy - snowfallInterceptedByCanopy;
      rainfallBelowCanopy         = rainfallAboveCanopy - rainfallInterceptedByCanopy;
      
      if (0.0f > rainfallBelowCanopy)
        {
          // FIXLATER There appears to be a mass balance bug.  When the canopy completely empties of water the total outflow of (qSnow * dt + eCan * dt) can be
          // greater than the total water available.  When you calculate snowfallInterceptedByCanopy, rainfallInterceptedByCanopy, and rainfallBelowCanopy from
          // mass conservation it results in a negative value for rainfallBelowCanopy.  It's also possible for rainfallBelowCanopy to be negative at other
          // times due to round off error.  That is not a mass balance bug.  My first thought was to take the missing water back from snowfallBelowCanopy and
          // evaporationFromCanopy.  However, the mass balance check for the snow pack only works with the unaltered value of snowfallBelowCanopy so I would
          // have to take the missing water back from the snowpack too.  And what if the snowpack happened to disappear as well during the exact same timestep
          // as the canopy emptying.  It started to get complicated so I have decided to just create the water and record it in waterError.  The canopy
          // completely emptying should occur infrequently; at most once per storm event.  In the one case where I have seen this it only created 0.1 micron of
          // water.
          *waterError         -= rainfallBelowCanopy;
          rainfallBelowCanopy  = 0.0f;
          
          /* FIXLATER Below are various snippets of code I used to try to fix this situation saved for future reference.
          if (0.0f == evapoTranspirationState->canIce + evapoTranspirationState->canLiq && 0.0f < canIceOriginal + canLiqOriginal)
            {
              // qSnow and eCan are rates at the beginning of the timestep.  However, if they are greater than prcp * fpIce the canopy might completely empty
              // during the timestep.  In that case, the rates are zero for the rest of the timestep, but qSnow and eCan are not updated to be the average
              // rates over the whole timestep.  The consequence of this is that snowfallBelowCanopy and evaporationFromCanopy will be wrong.  This will result
              // in a negative value for rainfallBelowCanopy, which should be taken proportionally from snowfallBelowCanopy and evaporationFromCanopy.
              if (0.0f < *evaporationFromCanopy)
                {
                  snowfallBelowCanopy    += rainfallBelowCanopy *         (snowfallBelowCanopy / (snowfallBelowCanopy + *evaporationFromCanopy));
                  *evaporationFromCanopy += rainfallBelowCanopy * (1.0f - (snowfallBelowCanopy / (snowfallBelowCanopy + *evaporationFromCanopy)));
                }
              else if (0.0f < snowfallBelowCanopy)
                {
                  // If there is condensation on the canopy don't change evaporationFromCanopy because a dry canopy does not stop condensation.
                  snowfallBelowCanopy += rainfallBelowCanopy;
                }
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
              else
                {
                  // This is weird.  There was no snowfall below the canopy, and there was no evaporation from the canopy, but somehow the canopy needs
                  // negative rainfall below the canopy to make up some missing water.
                  CkAssert(false);
                }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
              
              rainfallBelowCanopy  = 0.0f;
              
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
              CkAssert (0.0f <= snowfallBelowCanopy);
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
            }
          else
            {
              // Because rainfallBelowCanopy is a derived value it can be slightly negative due to roundoff error.  If it is try to take the water from canLiq
              // or canIce.
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
              // Because the negative is the result of round off error we want to test that rainfallBelowCanopy is epsilon equal to zero, but it can be the
              // result of subtracting two big numbers that are close to each other so we have to base epsilon on the largest of the numbers used to calculate
              // rainfallBelowCanopy.
              float epsilon = 1.0f; // For near equality testing.

              if (epsilon < prcp * dt)
                {
                  epsilon = prcp * dt;
                }

              if (epsilon < canIceOriginal)
                {
                  epsilon = canIceOriginal;
                }

              if (epsilon < canLiqOriginal)
                {
                  epsilon = canLiqOriginal;
                }

              if (epsilon < evapoTranspirationState->canIce)
                {
                  epsilon = evapoTranspirationState->canIce;
                }

              if (epsilon < evapoTranspirationState->canLiq)
                {
                  epsilon = evapoTranspirationState->canLiq;
                }

              if (epsilon < fabs(*evaporationFromCanopy))
                {
                  epsilon = fabs(*evaporationFromCanopy);
                }

              if (epsilon < snowfallBelowCanopy)
                {
                  epsilon = snowfallBelowCanopy;
                }

              epsilon *= -1.0e-6f;

              CkAssert(epsilon <= rainfallBelowCanopy);
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)

              if (evapoTranspirationState->canLiq >= -rainfallBelowCanopy)
                {
                  evapoTranspirationState->canLiq += rainfallBelowCanopy;
                  rainfallBelowCanopy              = 0.0f;
                }
              else
                {
                  rainfallBelowCanopy             += evapoTranspirationState->canLiq;
                  evapoTranspirationState->canLiq  = 0.0f;

                  if (evapoTranspirationState->canIce >= -rainfallBelowCanopy)
                    {
                      evapoTranspirationState->canIce += rainfallBelowCanopy;
                      rainfallBelowCanopy              = 0.0f;
                    }
                  else
                    {
                      rainfallBelowCanopy             += evapoTranspirationState->canIce;
                      evapoTranspirationState->canIce  = 0.0f;
                      *waterError                     -= rainfallBelowCanopy;
                      rainfallBelowCanopy              = 0.0f;
                    }
                }
            }
            */
        }
      
      // If there is a snow layer at the end of the timestep it intercepts all of the rainfall.
      if (0 > evapoTranspirationState->iSnow)
        {
          rainfallInterceptedBySnow = rainfallBelowCanopy;
          rainfallOnGround          = 0.0f;
        }
      else
        {
          rainfallInterceptedBySnow = 0.0f;
          rainfallOnGround          = rainfallBelowCanopy;
        }
      
      // When the total snow height gets less than 2.5 cm the multi-layer snow simulation turns off.  When this happens all of the liquid water in snLiq
      // becomes snowmelt on the ground and snEqv gets set to just the portion in snIce.  However, melting/freezing between snIce and snLiq also happens during
      // the timestep so we can't use the beginning timestep value of snLiq to determine how much to add to snowmeltOnGround.  We have to use the final value
      // of snEqv to back out the value of snowmeltOnGround.
      if (0 > iSnowOriginal && 0 == evapoTranspirationState->iSnow)
        {
          snowmeltOnGround = snEqvOriginal + snowfallBelowCanopy - *evaporationFromSnow - evapoTranspirationState->snEqv;
        }
      else
        {
          // There is a case where snEqvO does not equal snEqv at the beginning of the timestep.  This appears to happen when the multi-layer snow simulation
          // is turned off, and the snow is melting.  In this case, qSnBot is zero, but some water has disappeared from snEqvO and snEqv and the water shows up
          // in the soil moisture.  The solution to this is to add the difference between snEqvOriginal and snEqvO to snowmeltOnGround.
          snowmeltOnGround += snEqvOriginal - evapoTranspirationState->snEqvO;
        }
      
      // If snEqv falls below 0.001 mm, or snowH falls below 1e-6 m then Noah-MP sets both to zero and the water is lost.  If snEqv grows above 2000 mm then
      // Noah-MP sets it to 2000 and the water is added to runSub as glacier flow.  We are calculating what snEqv should be and putting the water back.
      snEqvShouldBe = snEqvOriginal + snowfallBelowCanopy + rainfallInterceptedBySnow - *evaporationFromSnow - snowmeltOnGround;
      runoff        = (runSrf + runSub) * dt;
      
      if (0.0f == evapoTranspirationState->snEqv)
        {
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonGreaterOrEqual(0.001f, snEqvShouldBe));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          evapoTranspirationState->snEqv = snEqvShouldBe;
          evapoTranspirationState->snowH = snEqvShouldBe / 1000.0f; // Divide by one thousand to convert from millimeters to meters.
        }
      else if (2000.0f < snEqvShouldBe)
        {
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonEqual(2000.0f, evapoTranspirationState->snEqv));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          if (0 > evapoTranspirationState->iSnow)
            {
              evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] = -evapoTranspirationState->snowH;
              
              for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
                {
                  evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS] =
                      evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] + zSoil[ii];
                }
              
              evapoTranspirationState->snIce[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] += snEqvShouldBe - evapoTranspirationState->snEqv;
            }
          
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonGreaterOrEqual(evapoTranspirationState->snEqv + runoff, snEqvShouldBe));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          // Take the glacier flow back from runSub.
          runoff -= snEqvShouldBe - evapoTranspirationState->snEqv;
          
          // Put the water back in snEqv.
          evapoTranspirationState->snEqv = snEqvShouldBe;
        }
      else
        {
          // If canLiq falls below 1e-6 mm then Noah-MP sets it to zero and the water is lost.  It does the same for canIce.  I cannot think of a good way to
          // detect when this happens.  I can't do a should be mass balance check like I do for snEqv because I don't know the correct value of
          // rainfallInterceptedByCanopy or rainfallBelowCanopy.  In fact, I am using the old and new values of canLiq and canIce to calculate
          // rainfallInterceptedByCanopy and rainfallBelowCanopy so they will always be consistent with them.
          //
          // One pernicious aspect of this problem is that our timesteps are smaller than what the Noah-MP code developers expected so we can see accumulations
          // on the canopy of less than 1e-6 mm per timestep.  This water will be thrown away every timestep and nothing will ever accumulate.  This only
          // happens for very light precipitation, and it only throws away a nanometer of water each timestep so it's not too bad, but I wish they hadn't coded
          // it this way.
          //
          // This problem went undetected for a long time because our epsilon for single precision floats is 1e-6 so when Noah-MP threw away less than 1e-6 it
          // was still epsilon equal.  The problem only became visible when canLiq and canIce were both set to zero during the same timestep and the water
          // thrown away added up to more than 1e-6.
          //
          // The solution I have decided on is just to put the difference between snEqv and snEqvShouldBe into waterError.  I also need to change the assertion
          // so that it's okay for the error to be up to 2e-6 if canLiq and canIce are both zero and thus water could have been thrown away from both.
          *waterError += evapoTranspirationState->snEqv - snEqvShouldBe;
          
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonEqual(evapoTranspirationState->snEqv, snEqvShouldBe) ||
                   (0.0f == evapoTranspirationState->canLiq && 0.0f == evapoTranspirationState->canIce &&       // Using std::abs instead of fabs so that the
                    epsilonGreaterOrEqual(2.0e-6f, std::abs(evapoTranspirationState->snEqv - snEqvShouldBe)))); // result is a float and not a double
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
        }

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_INVARIANTS)
      CkAssert(!checkEvapoTranspirationStateStructInvariant(evapoTranspirationState));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_INVARIANTS)
      
      *surfacewaterAdd = snowmeltOnGround + rainfallOnGround;
      *transpiration   = eTran * dt;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      CkAssert(0.0f <= *surfacewaterAdd && 0.0f <= *transpiration);
      
      // Calculate soil moisture at the end of the timestep.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (0 == ii)
            {
              soilMoistureNew += smc[ii] * -zSoil[ii] * 1000.0f; // * 1000.0f because zSoil is in meters and soilMoistureNew is in millimeters.
            }
          else
            {
              soilMoistureNew += smc[ii] * (zSoil[ii - 1] - zSoil[ii]) * 1000.0f;
            }
        }
      
      // Include water in the aquifer in the mass balance check.
      soilMoistureNew += wa;
      
      // Verify that soil moisture balances.
      CkAssert(epsilonEqual(soilMoistureOriginal - *evaporationFromGround - *transpiration + *surfacewaterAdd - runoff, soilMoistureNew));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
    } // End if (!error).
  
  return error;
}

bool evapoTranspirationWater(float lat, int yearLen, float julian, float cosZ, float dt, float dx, float dz8w, float sfcTmp, float sfcPrs, float psfc,
                             float uu, float vv, float q2, float qc, float solDn, float lwDn, float prcp, float tBot, float pblh, float wsLake,
                             EvapoTranspirationStateStruct* evapoTranspirationState, float* surfacewaterAdd, float* evaporationFromSnow,
                             float* evaporationFromGround, float* waterError)
{
  bool error = false; // Error flag.
  int  ii;            // Loop counter.
  
  // Input parameters for redprm function.  Some are also used for sflx function.
  int   vegType   = 16;                                        // 'Water Bodies' from VEGPARM.TBL.
  int   soilType  = 14;                                        // 'WATER' from SOILPARM.TBL.
  int   slopeType = 8;                                         // I just arbitrarily chose a slope type with zero slope.  I think slope is only used to
                                                               // calculate runoff, which we ignore.
  float zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];      // Layer bottom depth in meters from soil surface of each soil layer.  Values are set below from
                                                               // zSnso.
  int   nSoil     = EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; // Always use four soil layers.
  int   isUrban   = 1;                                         // USGS vegetation type for urban land.
  
  // Input parameters to sflx function.
  int   iLoc    = 1;                                         // Grid location index, unused.
  int   jLoc    = 1;                                         // Grid location index, unused.
  int   nSnow   = EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; // Maximum number of snow layers.  Always pass 3.
  float shdFac  = 0.0f;                                      // Fraction of land area shaded by vegetation, 0.0 to 1.0.  Always pass 0.0.
  float shdMax  = 0.0f;                                      // Yearly maximum fraction of land area shaded by vegetation, 0.0 to 1.0.  Always pass 0.0.
  int   ice     = 0;                                         // Flag to indicate permanent ice cover, 0 for no, 1 for yes.  Always pass 0.  If permanent ice
                                                             // cover call evapoTranspirationGlacier instead.
  int   ist     = 2;                                         // Flag to indicate permanent water cover, 1 for soil, 2 for lake.  Always pass 2.  If permanent
                                                             // soil cover call evapoTranspirationSoil instead.
  int   isc     = 4;                                         // Soil color type, 1 for lightest to 8 for darkest.  Always pass 4.
  float smcEq[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];    // Equlibrium water content of each soil layer, unitless.  Values are set below from
                                                             // NOAHMP_POROSITY.
  int   iz0tlnd = 0;                                         // Unused.
  float co2Air  = 0.0004f * sfcPrs;                          // CO2 partial pressure in Pascal at surface.  Always pass 400 parts per million of surface
                                                             // pressure.
  float o2Air   = 0.21f * sfcPrs;                            // O2 partial pressure in Pascal at surface.  Always pass 21 percent of surface pressure.
  float folN    = 0.0f;                                      // Foliage nitrogen percentage, 0.0 to 100.0.  Always pass 0.0.
  float fIce[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];     // Frozen fraction of each snow layer, unitless.  Values are set below from snIce and snLiq.
  float zLvl    = dz8w;                                      // Thickness in meters of lowest atmosphere layer in forcing data.  Redundant with dz8w.
  
  // Input/output parameters to sflx function.
  float sh2o[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS]; // Liquid water content of each soil layer, unitless.  Values are set below from NOAHMP_POROSITY.
  float smc[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];  // Total water content, liquid and solid, of each soil layer, unitless.  Values are set below from
                                                         // NOAHMP_POROSITY.
  float qsfc  = q2;                                      // Water vapor mixing ratio at middle of lowest atmosphere layer in forcing data, unitless.  Redundant
                                                         // with q2.
  float qSnow = NAN;                                     // This is actually an output only variable.  Snowfall rate below the canopy in millimeters of water
                                                         // equivalent per second.
  float zwt   = 0.0f;                                    // Depth in meters to water table.  Specify depth as a positive number.  Always pass 0.0.
  float wa;                                              // Water stored in aquifer in millimeters of water.  Value is set below.
  float wt;                                              // Water stored in aquifer and saturated soil in millimeters of water.  Value is set below from zSoil,
                                                         // and NOAHMP_POROSITY.
  float smcwtd;                                          // Water content between the bottom of the lowest soil layer and water table, unitless.  Value is set
                                                         // below from NOAHMP_POROSITY.
  
  // Output parameters to sflx function.  Set to NAN so we can detect if the values are used before being set.
  float fsa      = NAN; // Unused.
  float fsr      = NAN; // Unused.
  float fira     = NAN; // Unused.
  float fsh      = NAN; // Unused.
  float sSoil    = NAN; // Unused.
  float fcev     = NAN; // Unused.
  float fgev     = NAN; // Unused.
  float fctr     = NAN; // Unused.
  float eCan     = NAN; // Evaporation rate from canopy in millimeters of water equivalent per second.
  float eTran    = NAN; // Evaporation rate from transpiration in millimeters of water equivalent per second.
  float eDir     = NAN; // Evaporation rate from surface in millimeters of water equivalent per second.
  float tRad     = NAN; // Unused.
  float tgb      = NAN; // Unused.
  float tgv      = NAN; // Unused.
  float t2mv     = NAN; // Unused.
  float t2mb     = NAN; // Unused.
  float q2v      = NAN; // Unused.
  float q2b      = NAN; // Unused.
  float runSrf   = NAN; // Infiltration excess runoff in millimeters of water per second.
  float runSub   = NAN; // Saturation excess runoff in millimeters of water per second.
  float apar     = NAN; // Unused.
  float psn      = NAN; // Unused.
  float sav      = NAN; // Unused.
  float sag      = NAN; // Unused.
  float fSno     = NAN; // Unused.
  float nee      = NAN; // Unused.
  float gpp      = NAN; // Unused.
  float npp      = NAN; // Unused.
  float fVeg     = NAN; // Unused.
  float albedo   = NAN; // Unused.
  float qSnBot   = NAN; // Snowmelt rate from the bottom of the snow pack in millimeters of water per second.
  float ponding  = NAN; // Unused.
  float ponding1 = NAN; // Unused.
  float ponding2 = NAN; // Unused.
  float rsSun    = NAN; // Unused.
  float rsSha    = NAN; // Unused.
  float bGap     = NAN; // Unused.
  float wGap     = NAN; // Unused.
  float chv      = NAN; // Unused.
  float chb      = NAN; // Unused.
  float emissi   = NAN; // Unused.
  float shg      = NAN; // Unused.
  float shc      = NAN; // Unused.
  float shb      = NAN; // Unused.
  float evg      = NAN; // Unused.
  float evb      = NAN; // Unused.
  float ghv      = NAN; // Unused.
  float ghb      = NAN; // Unused.
  float irg      = NAN; // Unused.
  float irc      = NAN; // Unused.
  float irb      = NAN; // Unused.
  float tr       = NAN; // Unused.
  float evc      = NAN; // Unused.
  float chLeaf   = NAN; // Unused.
  float chuc     = NAN; // Unused.
  float chv2     = NAN; // Unused.
  float chb2     = NAN; // Unused.
  float fpIce    = NAN; // Fraction of precipitation that is frozen, unitless.
  
  // Derived output variables.
  int   iSnowOriginal;             // Actual number of snow layers before timestep.
  float evaporationFromSurface;    // Quantity of evaporation from the surface in millimeters of water equivalent.  Positive means water evaporated off of the
                                   // surface.  Negative means water condensed on to the surface.  Surface evaporation sometimes comes from snow and is taken
                                   // out by Noah-MP, but if there is not enough snow we have to take the evaporation from the water in the ADHydro state
                                   // variables.
  float snowfall;                  // Quantity of snowfall in millimeters of water equivalent.  Must be non-negative.
  float snowmeltOnGround;          // Quantity of water that reaches the ground from the snow layer in millimeters of water.  Must be non-negative.
  float rainfall;                  // Quantity of rainfall in millimeters of water.  Must be non-negative.
  float rainfallInterceptedBySnow; // Quantity of rainfall intercepted by the snow layer in millimeters of water.  Must be non-negative.
  float rainfallOnGround;          // Quantity of rainfall that reaches the ground in millimeters of water.  Must be non-negative.
  float snEqvOriginal;             // Quantity of water in the snow layer(s) before timestep in millimeters of water equivalent.  snEqvO is not always set to
                                   // this value after the timestep.
  float snEqvShouldBe;             // If snEqv falls below 0.001 Noah-MP sets it to zero, or if it rises above 2000.0 Noah-MP sets it to 2000.0.  We don't want
                                   // this behavior so in this variable we calculate what snEqv should be and set it back.  If snEqv is not changed this
                                   // instead performs a mass balance check.
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
  // Variables used for assertions.
  int   iLocOriginal         = iLoc;
  int   jLocOriginal         = jLoc;
  float latOriginal          = lat;
  int   yearLenOriginal      = yearLen;
  float julianOriginal       = julian;
  float cosZOriginal         = cosZ;
  float dtOriginal           = dt;
  float dxOriginal           = dx;
  float dz8wOriginal         = dz8w;
  int   nSoilOriginal        = nSoil;
  float zSoilOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];
  int   nSnowOriginal        = nSnow;
  float shdFacOriginal       = shdFac;
  float shdMaxOriginal       = shdMax;
  int   vegTypeOriginal      = vegType;
  int   isUrbanOriginal      = isUrban;
  int   iceOriginal          = ice;
  int   istOriginal          = ist;
  int   iscOriginal          = isc;
  float smcEqOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];
  int   iz0tlndOriginal      = iz0tlnd;
  float sfcTmpOriginal       = sfcTmp;
  float sfcPrsOriginal       = sfcPrs;
  float psfcOriginal         = psfc;
  float uuOriginal           = uu;
  float vvOriginal           = vv;
  float q2Original           = q2;
  float qcOriginal           = qc;
  float solDnOriginal        = solDn;
  float lwDnOriginal         = lwDn;
  float prcpOriginal         = prcp;
  float tBotOriginal         = tBot;
  float co2AirOriginal       = co2Air;
  float o2AirOriginal        = o2Air;
  float folNOriginal         = folN;
  float fIceOldOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];
  float pblhOriginal         = pblh;
  float zLvlOriginal         = zLvl;
  float soilMoistureOriginal = 0.0f; // Total soil moisture before timestep in millimeters of water.
  float soilMoistureNew      = 0.0f; // Total soil moisture after timestep in millimeters of water.
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(-M_PI / 2.0f <= lat && M_PI / 2.0f >= lat))
    {
      CkError("ERROR in evapoTranspirationWater: lat must be greater than or equal to -pi/2 and less than or equal to pi/2.\n");
      error = true;
    }
  
  if (!(365 == yearLen || 366 == yearLen))
    {
      CkError("ERROR in evapoTranspirationWater: yearLen must be 365 or 366.\n");
      error = true;
    }
  
  if (!(0.0f <= julian && julian <= yearLen))
    {
      CkError("ERROR in evapoTranspirationWater: julian must be greater than or equal to zero and less than or equal to yearLen.\n");
      error = true;
    }
  
  if (!(0.0f <= cosZ && 1.0f >= cosZ))
    {
      CkError("ERROR in evapoTranspirationWater: cosZ must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }
  
  if (!(0.0f < dt))
    {
      CkError("ERROR in evapoTranspirationWater: dt must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f < dx))
    {
      CkError("ERROR in evapoTranspirationWater: dx must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f < dz8w))
    {
      CkError("ERROR in evapoTranspirationWater: dz8w must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f <= sfcTmp))
    {
      CkError("ERROR in evapoTranspirationWater: sfcTmp must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= sfcTmp))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: sfcTmp below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= sfcTmp))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: sfcTmp above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= sfcPrs))
    {
      CkError("ERROR in evapoTranspirationWater: sfcPrs must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(35000.0f <= sfcPrs))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: sfcPrs below 35 kPa.\n");
        }
    }

  if (!(0.0f <= psfc))
    {
      CkError("ERROR in evapoTranspirationWater: psfc must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(35000.0f <= psfc))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: psfc below 35 kPa.\n");
        }
    }
  
  if (!(100.0f >= fabs(uu)))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: magnitude of uu greater than 100 m/s.\n");
        }
    }
  
  if (!(100.0f >= fabs(vv)))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: magnitude of vv greater than 100 m/s.\n");
        }
    }
  
  if (!(0.0f <= q2 && 1.0f >= q2))
    {
      CkError("ERROR in evapoTranspirationWater: q2 must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }
  
  if (!(0.0f <= qc && 1.0f >= qc))
    {
      CkError("ERROR in evapoTranspirationWater: qc must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }

  if (!(0.0f <= solDn))
    {
      CkError("ERROR in evapoTranspirationWater: solDn must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= lwDn))
    {
      CkError("ERROR in evapoTranspirationWater: lwDn must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= prcp))
    {
      CkError("ERROR in evapoTranspirationWater: prcp must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= tBot))
    {
      CkError("ERROR in evapoTranspirationWater: tBot must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= tBot))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: tBot below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= tBot))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationWater: tBot above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= pblh))
    {
      CkError("ERROR in evapoTranspirationWater: pblh must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= wsLake))
    {
      CkError("ERROR in evapoTranspirationWater: wsLake must be greater than or equal to zero.\n");
      error = true;
    }
  
  if (!(NULL != evapoTranspirationState))
    {
      CkError("ERROR in evapoTranspirationWater: evapoTranspirationState must not be NULL.\n");
      error = true;
    }
  else
    {
      if (!(0.0 == evapoTranspirationState->canLiq))
        {
          CkError("ERROR in evapoTranspirationWater: canLiq must be zero.\n");
          error = true;
        }

      if (!(0.0 == evapoTranspirationState->canIce))
        {
          CkError("ERROR in evapoTranspirationWater: canIce must be zero.\n");
          error = true;
        }
    }

  if (!(NULL != surfacewaterAdd))
    {
      CkError("ERROR in evapoTranspirationWater: surfacewaterAdd must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *surfacewaterAdd = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromSnow))
    {
      CkError("ERROR in evapoTranspirationWater: evaporationFromSnow must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromSnow = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromGround))
    {
      CkError("ERROR in evapoTranspirationWater: evaporationFromGround must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromGround = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != waterError))
    {
      CkError("ERROR in evapoTranspirationWater: waterError must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *waterError = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_INVARIANTS)
  if (!error)
    {
      error = checkEvapoTranspirationStateStructInvariant(evapoTranspirationState);
    }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_INVARIANTS)

  if (!error)
    {
      // Set variables that require error checking of pointers first.
      iSnowOriginal  = evapoTranspirationState->iSnow;
      snEqvOriginal  = evapoTranspirationState->snEqv;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          fIceOldOriginal[ii] = evapoTranspirationState->fIceOld[ii];
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Set zSoil from zSnso.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (0 > evapoTranspirationState->iSnow)
            {
              zSoil[ii] = evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS] -
                          evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1];
            }
          else
            {
              zSoil[ii] = evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];
            }

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          zSoilOriginal[ii] = zSoil[ii];
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
        }

      // Calculate fIce at the beginning of the timestep.
      for (ii = evapoTranspirationState->iSnow + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          fIce[ii] = evapoTranspirationState->snIce[ii] / (evapoTranspirationState->snIce[ii] + evapoTranspirationState->snLiq[ii]);
        }

      // Set Noah-MP globals.
      REDPRM(&vegType, &soilType, &slopeType, zSoil, &nSoil, &isUrban);
      
      // Set water content of the soil layers.  In waterbodies the watertable is always at the surface and everything is saturated.  The global variable
      // NOAHMP_POROSITY gets set in REDPRM so we can't do this before here.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          smcEq[ii] = NOAHMP_POROSITY;
          sh2o[ii]  = NOAHMP_POROSITY;
          smc[ii]   = NOAHMP_POROSITY;

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          smcEqOriginal[ii] = smcEq[ii];
          
          // Calculate soil moisture at the beginning of the timestep.
          if (0 == ii)
            {
              soilMoistureOriginal += smc[ii] * -zSoil[ii] * 1000.0f; // * 1000.0f because zSoil is in meters and soilMoistureOriginal is in millimeters.
            }
          else
            {
              soilMoistureOriginal += smc[ii] * (zSoil[ii - 1] - zSoil[ii]) * 1000.0f;
            }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
        }
      
      // Calculate water in the aquifer and saturated soil.
      wa     = 5000.0f;
      wt     = wa - (zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS - 1]) * 1000.0f * NOAHMP_POROSITY;
      smcwtd = NOAHMP_POROSITY;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      // Include water in the aquifer in the mass balance check.
      soilMoistureOriginal += wa;
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Run Noah-MP.
      NOAHMP_SFLX(&iLoc, &jLoc, &lat, &yearLen, &julian, &cosZ, &dt, &dx, &dz8w, &nSoil, zSoil, &nSnow, &shdFac, &shdMax, &vegType, &isUrban, &ice, &ist, &isc,
                  smcEq, &iz0tlnd, &sfcTmp, &sfcPrs, &psfc, &uu, &vv, &q2, &qc, &solDn, &lwDn, &prcp, &tBot, &co2Air, &o2Air, &folN,
                  evapoTranspirationState->fIceOld, &pblh, &zLvl, &evapoTranspirationState->albOld, &evapoTranspirationState->snEqvO,
                  evapoTranspirationState->stc, sh2o, smc, &evapoTranspirationState->tah, &evapoTranspirationState->eah, &evapoTranspirationState->fWet,
                  &evapoTranspirationState->canLiq, &evapoTranspirationState->canIce, &evapoTranspirationState->tv, &evapoTranspirationState->tg, &qsfc,
                  &qSnow, &evapoTranspirationState->iSnow, evapoTranspirationState->zSnso, &evapoTranspirationState->snowH, &evapoTranspirationState->snEqv,
                  evapoTranspirationState->snIce, evapoTranspirationState->snLiq, &zwt, &wa, &wt, &wsLake, &evapoTranspirationState->lfMass,
                  &evapoTranspirationState->rtMass, &evapoTranspirationState->stMass, &evapoTranspirationState->wood, &evapoTranspirationState->stblCp,
                  &evapoTranspirationState->fastCp, &evapoTranspirationState->lai, &evapoTranspirationState->sai, &evapoTranspirationState->cm,
                  &evapoTranspirationState->ch, &evapoTranspirationState->tauss, &smcwtd, &evapoTranspirationState->deepRech, &evapoTranspirationState->rech,
                  &fsa, &fsr, &fira, &fsh, &sSoil, &fcev, &fgev, &fctr, &eCan, &eTran, &eDir, &tRad, &tgb, &tgv, &t2mv, &t2mb, &q2v, &q2b, &runSrf, &runSub,
                  &apar, &psn, &sav, &sag, &fSno, &nee, &gpp, &npp, &fVeg, &albedo, &qSnBot, &ponding, &ponding1, &ponding2, &rsSun, &rsSha, &bGap, &wGap,
                  &chv, &chb, &emissi, &shg, &shc, &shb, &evg, &evb, &ghv, &ghb, &irg, &irc, &irb, &tr, &evc, &chLeaf, &chuc, &chv2, &chb2, &fpIce);
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      // Verify that the input variables have not changed.
      CkAssert(iLocOriginal == iLoc && jLocOriginal == jLoc && latOriginal == lat && yearLenOriginal == yearLen && julianOriginal == julian &&
               cosZOriginal == cosZ && dtOriginal == dt && dxOriginal == dx && dz8wOriginal == dz8w && nSoilOriginal == nSoil &&
               zSoilOriginal[0] == zSoil[0] && zSoilOriginal[1] == zSoil[1] && zSoilOriginal[2] == zSoil[2] && zSoilOriginal[3] == zSoil[3] &&
               nSnowOriginal == nSnow && shdFacOriginal == shdFac && shdMaxOriginal == shdMax && vegTypeOriginal == vegType && isUrbanOriginal == isUrban &&
               iceOriginal == ice && istOriginal == ist && iscOriginal == isc && smcEqOriginal[0] == smcEq[0] && smcEqOriginal[1] == smcEq[1] &&
               smcEqOriginal[2] == smcEq[2] && smcEqOriginal[3] == smcEq[3] && iz0tlndOriginal == iz0tlnd && sfcTmpOriginal == sfcTmp &&
               sfcPrsOriginal == sfcPrs && psfcOriginal == psfc && uuOriginal == uu && vvOriginal == vv && q2Original == q2 && qcOriginal == qc &&
               solDnOriginal == solDn && lwDnOriginal == lwDn && prcpOriginal == prcp && tBotOriginal == tBot && co2AirOriginal == co2Air &&
               o2AirOriginal == o2Air && folNOriginal == folN && fIceOldOriginal[0] == evapoTranspirationState->fIceOld[0] &&
               fIceOldOriginal[1] == evapoTranspirationState->fIceOld[1] && fIceOldOriginal[2] == evapoTranspirationState->fIceOld[2] &&
               pblhOriginal == pblh && zLvlOriginal == zLvl);
      
      // Verify that the fraction of the precipitation that falls as snow is between 0 and 1, the snowfall rate below the canopy is not negative, and the
      // snowmelt out the bottom of the snowpack is not negative.
      CkAssert(0.0f <= fpIce && 1.0f >= fpIce && 0.0f <= qSnow && 0.0f <= qSnBot);
      
      // Verify that there is no water, no evaporation, no transpiration, and no snowfall interception in the non-existant canopy.
      CkAssert(0.0 == evapoTranspirationState->canLiq && 0.0 == evapoTranspirationState->canIce && 0.0 == eCan && 0.0 == eTran && prcp * fpIce == qSnow);
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Store fIce from the beginning of the timestep in fIceOld.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          if (ii >= iSnowOriginal + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS)
            {
              evapoTranspirationState->fIceOld[ii] = fIce[ii];
            }
          else
            {
              evapoTranspirationState->fIceOld[ii] = 0.0f;
            }
        }
      
      // Calculate derived output variables.
      evaporationFromSurface = eDir * dt;
      
      // Surface condensation is added to the snow layer if any snow exists or otherwise added to the ground as water.  Surface evaporation is taken first from
      // the snow layer up to the amount in snEqvO, and then any remaining is taken from the ground.
      if (0.0f > evaporationFromSurface)
        {
          if (0.0f < evapoTranspirationState->snEqvO)
            {
              *evaporationFromSnow   = evaporationFromSurface;
              *evaporationFromGround = 0.0f;
            }
          else
            {
              *evaporationFromSnow   = 0.0f;
              *evaporationFromGround = evaporationFromSurface;
            }
        }
      else if (evaporationFromSurface <= evapoTranspirationState->snEqvO)
        {
          *evaporationFromSnow   = evaporationFromSurface;
          *evaporationFromGround = 0.0f;
        }
      else
        {
          *evaporationFromSnow   = evapoTranspirationState->snEqvO;
          *evaporationFromGround = evaporationFromSurface - evapoTranspirationState->snEqvO;
        }
      
      snowfall         = prcp * dt * fpIce;
      snowmeltOnGround = qSnBot * dt;
      rainfall         = prcp * dt - snowfall;
      
      // If there is a snow layer at the end of the timestep it intercepts all of the rainfall.
      if (0 > evapoTranspirationState->iSnow)
        {
          rainfallInterceptedBySnow = rainfall;
          rainfallOnGround          = 0.0f;
        }
      else
        {
          rainfallInterceptedBySnow = 0.0f;
          rainfallOnGround          = rainfall;
        }
      
      // When the total snow height gets less than 2.5 cm the multi-layer snow simulation turns off.  When this happens all of the liquid water in snLiq
      // becomes snowmelt on the ground and snEqv gets set to just the portion in snIce.  However, melting/freezing between snIce and snLiq also happens during
      // the timestep so we can't use the beginning timestep value of snLiq to determine how much to add to snowmeltOnGround.  We have to use the final value
      // of snEqv to back out the value of snowmeltOnGround.
      if (0 > iSnowOriginal && 0 == evapoTranspirationState->iSnow)
        {
          snowmeltOnGround = snEqvOriginal + snowfall - *evaporationFromSnow - evapoTranspirationState->snEqv;
        }
      else
        {
          // There is a case where snEqvO does not equal snEqv at the beginning of the timestep.  This appears to happen when the multi-layer snow simulation
          // is turned off, and the snow is melting.  In this case, qSnBot is zero, but some water has disappeared from snEqvO and snEqv and the water shows up
          // in the soil moisture.  The solution to this is to add the difference between snEqvOriginal and snEqvO to snowmeltOnGround.
          snowmeltOnGround += snEqvOriginal - evapoTranspirationState->snEqvO;
        }
      
      // If snEqv falls below 0.001 mm, or snowH falls below 1e-6 m then Noah-MP sets both to zero and the water is lost.  If snEqv grows above 2000 mm then
      // Noah-MP sets it to 2000 and the water is lost.  We are calculating what snEqv should be and putting the water back.
      snEqvShouldBe = snEqvOriginal + snowfall + rainfallInterceptedBySnow - *evaporationFromSnow - snowmeltOnGround;
      
      if (0.0f == evapoTranspirationState->snEqv)
        {
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonGreaterOrEqual(0.001f, snEqvShouldBe));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          evapoTranspirationState->snEqv = snEqvShouldBe;
          evapoTranspirationState->snowH = snEqvShouldBe / 1000.0f; // Divide by one thousand to convert from millimeters to meters.
        }
      else if (epsilonEqual(2000.0f, evapoTranspirationState->snEqv))
        {
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonLessOrEqual(2000.0f, snEqvShouldBe));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          if (0 > evapoTranspirationState->iSnow)
            {
              evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] = -evapoTranspirationState->snowH;
              
              for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
                {
                  evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS] =
                      evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] + zSoil[ii];
                }
              
              evapoTranspirationState->snIce[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] += snEqvShouldBe - evapoTranspirationState->snEqv;
            }
          
          evapoTranspirationState->snEqv = snEqvShouldBe;
        }
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      else
        {
          CkAssert(epsilonEqual(evapoTranspirationState->snEqv, snEqvShouldBe));
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_INVARIANTS)
      CkAssert(!checkEvapoTranspirationStateStructInvariant(evapoTranspirationState));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_INVARIANTS)
      
      *surfacewaterAdd = snowmeltOnGround + rainfallOnGround;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      CkAssert(0.0f <= *surfacewaterAdd);
      
      // Calculate soil moisture at the end of the timestep.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (0 == ii)
            {
              soilMoistureNew += smc[ii] * -zSoil[ii] * 1000.0f; // * 1000.0f because zSoil is in meters and soilMoistureNew is in millimeters.
            }
          else
            {
              soilMoistureNew += smc[ii] * (zSoil[ii - 1] - zSoil[ii]) * 1000.0f;
            }
        }
      
      // Include water in the aquifer in the mass balance check.
      soilMoistureNew += wa;
      
      // Verify that soil moisture balances.
      CkAssert(epsilonEqual(soilMoistureOriginal - *evaporationFromGround + *surfacewaterAdd - runSrf * dt - runSub * dt, soilMoistureNew));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
    } // End if (!error).
  
  return error;
}

bool evapoTranspirationGlacier(float cosZ, float dt, float sfcTmp, float sfcPrs, float uu, float vv, float q2, float solDn, float lwDn, float prcp, float tBot,
                               float zLvl, EvapoTranspirationStateStruct* evapoTranspirationState, float* surfacewaterAdd, float* evaporationFromSnow,
                               float* evaporationFromGround, float* waterError)
{
  bool error = false; // Error flag.
  int  ii;            // Loop counter.
  
  // Input parameters for redprm function.  Some are also used for sflx function.
  int   vegType   = 24;                                        // 'Snow or Ice' from VEGPARM.TBL.
  int   soilType  = 16;                                        // 'OTHER(land-ice)' from SOILPARM.TBL.
  int   slopeType = 8;                                         // I just arbitrarily chose a slope type with zero slope.  I think slope is only used to
                                                               // calculate runoff, which we ignore.
  float zSoil[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];      // Layer bottom depth in meters from soil surface of each soil layer.  Values are set below from
                                                               // zSnso.
  int   nSoil     = EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; // Always use four soil layers.
  int   isUrban   = 1;                                         // USGS vegetation type for urban land.
  
  // Input parameters to sflx function.
  int   iLoc    = 1;                                         // Grid location index, unused.
  int   jLoc    = 1;                                         // Grid location index, unused.
  int   nSnow   = EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; // Maximum number of snow layers.  Always pass 3.
  float fIce[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];     // Frozen fraction of each snow layer, unitless.  Values are set below from snIce and snLiq.
  
  // Input/output parameters to sflx function.
  float qSnow = NAN;                                     // This is actually an output only variable.  Snowfall rate below the canopy in millimeters of water
                                                         // equivalent per second.
  float smc[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];  // Total water content, liquid and solid, of each soil layer, unitless.  Values are set below from
                                                         // NOAHMP_POROSITY.
  float sh2o[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS]; // Liquid water content of each soil layer, unitless.  Values are set below.
  float qsfc  = q2;                                      // Water vapor mixing ratio at middle of lowest atmosphere layer in forcing data, unitless.  Redundant
                                                         // with q2.

  // Output parameters to sflx function.  Set to NAN so we can detect if the values are used before being set.
  float fsa      = NAN; // Unused.
  float fsr      = NAN; // Unused.
  float fira     = NAN; // Unused.
  float fsh      = NAN; // Unused.
  float fgev     = NAN; // Unused.
  float sSoil    = NAN; // Unused.
  float tRad     = NAN; // Unused.
  float eDir     = NAN; // Evaporation rate from surface in millimeters of water equivalent per second.
  float runSrf   = NAN; // Infiltration excess runoff in millimeters of water per second.
  float runSub   = NAN; // Saturation excess runoff in millimeters of water per second.
  float sag      = NAN; // Unused.
  float albedo   = NAN; // Unused.
  float qSnBot   = NAN; // Snowmelt rate from the bottom of the snow pack in millimeters of water per second.
  float ponding  = NAN; // Unused.
  float ponding1 = NAN; // Unused.
  float ponding2 = NAN; // Unused.
  float t2m      = NAN; // Unused.
  float q2e      = NAN; // Unused.
  float emissi   = NAN; // Unused.
  float fpIce    = NAN; // Fraction of precipitation that is frozen, unitless.
  float ch2b     = NAN; // Unused.
  
  // Derived output variables.
  int   iSnowOriginal;             // Actual number of snow layers before timestep.
  float evaporationFromSurface;    // Quantity of evaporation from the surface in millimeters of water equivalent.  Positive means water evaporated off of the
                                   // surface.  Negative means water condensed on to the surface.  Surface evaporation sometimes comes from snow and is taken
                                   // out by Noah-MP, but if there is not enough snow we have to take the evaporation from the water in the ADHydro state
                                   // variables.
  float snowfall;                  // Quantity of snowfall in millimeters of water equivalent.  Must be non-negative.
  float snowmeltOnGround;          // Quantity of water that reaches the ground from the snow layer in millimeters of water.  Must be non-negative.
  float rainfall;                  // Quantity of rainfall in millimeters of water.  Must be non-negative.
  float rainfallInterceptedBySnow; // Quantity of rainfall intercepted by the snow layer in millimeters of water.  Must be non-negative.
  float rainfallOnGround;          // Quantity of rainfall that reaches the ground in millimeters of water.  Must be non-negative.
  float snEqvOriginal;             // Quantity of water in the snow layer(s) before timestep in millimeters of water equivalent.  snEqvO is not always set to
                                   // this value after the timestep.
  float snEqvShouldBe;             // If snEqv falls below 0.001 Noah-MP sets it to zero, or if it rises above 2000.0 Noah-MP sets it to 2000.0.  We don't want
                                   // this behavior so in this variable we calculate what snEqv should be and set it back.  If snEqv is not changed this
                                   // instead performs a mass balance check.
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
  // Variables used for assertions.
  int   iLocOriginal         = iLoc;
  int   jLocOriginal         = jLoc;
  float cosZOriginal         = cosZ;
  int   nSnowOriginal        = nSnow;
  int   nSoilOriginal        = nSoil;
  float dtOriginal           = dt;
  float sfcTmpOriginal       = sfcTmp;
  float sfcPrsOriginal       = sfcPrs;
  float uuOriginal           = uu;
  float vvOriginal           = vv;
  float q2Original           = q2;
  float solDnOriginal        = solDn;
  float prcpOriginal         = prcp;
  float lwDnOriginal         = lwDn;
  float tBotOriginal         = tBot;
  float zLvlOriginal         = zLvl;
  float fIceOldOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];
  float zSoilOriginal[EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS];
  float soilMoistureOriginal = 0.0f; // Total soil moisture before timestep in millimeters of water.
  float soilMoistureNew      = 0.0f; // Total soil moisture after timestep in millimeters of water.
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
                                                    
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(0.0f <= cosZ && 1.0f >= cosZ))
    {
      CkError("ERROR in evapoTranspirationGlacier: cosZ must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }
  
  if (!(0.0f < dt))
    {
      CkError("ERROR in evapoTranspirationGlacier: dt must be greater than zero.\n");
      error = true;
    }
  
  if (!(0.0f <= sfcTmp))
    {
      CkError("ERROR in evapoTranspirationGlacier: sfcTmp must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= sfcTmp))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: sfcTmp below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= sfcTmp))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: sfcTmp above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= sfcPrs))
    {
      CkError("ERROR in evapoTranspirationGlacier: sfcPrs must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(35000.0f <= sfcPrs))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: sfcPrs below 35 kPa.\n");
        }
    }
  
  if (!(100.0f >= fabs(uu)))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: magnitude of uu greater than 100 m/s.\n");
        }
    }
  
  if (!(100.0f >= fabs(vv)))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: magnitude of vv greater than 100 m/s.\n");
        }
    }
  
  if (!(0.0f <= q2 && 1.0f >= q2))
    {
      CkError("ERROR in evapoTranspirationGlacier: q2 must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }
  
  if (!(0.0f <= solDn))
    {
      CkError("ERROR in evapoTranspirationGlacier: solDn must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= lwDn))
    {
      CkError("ERROR in evapoTranspirationGlacier: lwDn must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= prcp))
    {
      CkError("ERROR in evapoTranspirationGlacier: prcp must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= tBot))
    {
      CkError("ERROR in evapoTranspirationGlacier: tBot must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= tBot))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: tBot below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= tBot))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in evapoTranspirationGlacier: tBot above 70 degrees C.\n");
        }
    }
  
  if (!(0.0f < zLvl))
    {
      CkError("ERROR in evapoTranspirationGlacier: zLvl must be greater than zero.\n");
      error = true;
    }

  if (!(NULL != evapoTranspirationState))
    {
      CkError("ERROR in evapoTranspirationGlacier: evapoTranspirationState must not be NULL.\n");
      error = true;
    }
  else
    {
      if (!(0.0 == evapoTranspirationState->canLiq))
        {
          CkError("ERROR in evapoTranspirationGlacier: canLiq must be zero.\n");
          error = true;
        }

      if (!(0.0 == evapoTranspirationState->canIce))
        {
          CkError("ERROR in evapoTranspirationGlacier: canIce must be zero.\n");
          error = true;
        }
    }
  
  if (!(NULL != surfacewaterAdd))
    {
      CkError("ERROR in evapoTranspirationGlacier: surfacewaterAdd must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *surfacewaterAdd = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromSnow))
    {
      CkError("ERROR in evapoTranspirationGlacier: evaporationFromSnow must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromSnow = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != evaporationFromGround))
    {
      CkError("ERROR in evapoTranspirationGlacier: evaporationFromGround must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *evaporationFromGround = 0.0f;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
  if (!(NULL != waterError))
    {
      CkError("ERROR in evapoTranspirationGlacier: water error must not be NULL.\n");
      error = true;
    }
  else
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_SIMPLE)
    {
      *waterError = 0.0;
    }
  
#if (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_INVARIANTS)
  if (!error)
    {
      error = checkEvapoTranspirationStateStructInvariant(evapoTranspirationState);
    }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_PUBLIC_FUNCTIONS_INVARIANTS)

  if (!error)
    {
      // Set variables that require error checking of pointers first.
      iSnowOriginal  = evapoTranspirationState->iSnow;
      snEqvOriginal  = evapoTranspirationState->snEqv;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          fIceOldOriginal[ii] = evapoTranspirationState->fIceOld[ii];
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Set zSoil from zSnso.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (0 > evapoTranspirationState->iSnow)
            {
              zSoil[ii] = evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS] -
                          evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1];
            }
          else
            {
              zSoil[ii] = evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS];
            }
          
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          zSoilOriginal[ii] = zSoil[ii];
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
        }

      // Calculate fIce at the beginning of the timestep.
      for (ii = evapoTranspirationState->iSnow + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          fIce[ii] = evapoTranspirationState->snIce[ii] / (evapoTranspirationState->snIce[ii] + evapoTranspirationState->snLiq[ii]);
        }

      // Set Noah-MP globals.
      REDPRM(&vegType, &soilType, &slopeType, zSoil, &nSoil, &isUrban);
      
      // Set water content of the soil layers.  In glaciers the watertable is always at the surface and everything is saturated and frozen.  The global
      // variable NOAHMP_POROSITY gets set in REDPRM so we can't do this before here.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          sh2o[ii] = 0.0f;
          smc[ii]  = NOAHMP_POROSITY;

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          // Calculate soil moisture at the beginning of the timestep.
          if (0 == ii)
            {
              soilMoistureOriginal += smc[ii] * -zSoil[ii] * 1000.0f; // * 1000.0f because zSoil is in meters and soilMoistureOriginal is in millimeters.
            }
          else
            {
              soilMoistureOriginal += smc[ii] * (zSoil[ii - 1] - zSoil[ii]) * 1000.0f;
            }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
        }
      
      // Run Noah-MP.
      NOAHMP_GLACIER(&iLoc, &jLoc, &cosZ, &nSnow, &nSoil, &dt, &sfcTmp, &sfcPrs, &uu, &vv, &q2, &solDn, &prcp, &lwDn, &tBot, &zLvl,
                     evapoTranspirationState->fIceOld, zSoil, &qSnow, &evapoTranspirationState->snEqvO, &evapoTranspirationState->albOld,
                     &evapoTranspirationState->cm, &evapoTranspirationState->ch, &evapoTranspirationState->iSnow, &evapoTranspirationState->snEqv, smc,
                     evapoTranspirationState->zSnso, &evapoTranspirationState->snowH, evapoTranspirationState->snIce, evapoTranspirationState->snLiq,
                     &evapoTranspirationState->tg, evapoTranspirationState->stc, sh2o, &evapoTranspirationState->tauss, &qsfc, &fsa, &fsr, &fira, &fsh, &fgev,
                     &sSoil, &tRad, &eDir, &runSrf, &runSub, &sag, &albedo, &qSnBot, &ponding, &ponding1, &ponding2, &t2m, &q2e, &emissi, &fpIce, &ch2b);

#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      // Verify that the input variables have not changed.
      CkAssert(iLocOriginal == iLoc && jLocOriginal == jLoc && cosZOriginal == cosZ && nSnowOriginal == nSnow && nSoilOriginal == nSoil && dtOriginal == dt &&
               sfcTmpOriginal == sfcTmp && sfcPrsOriginal == sfcPrs && uuOriginal == uu && vvOriginal == vv && q2Original == q2 && solDnOriginal == solDn &&
               prcpOriginal == prcp && lwDnOriginal == lwDn && tBotOriginal == tBot && zLvlOriginal == zLvl &&
               fIceOldOriginal[0] == evapoTranspirationState->fIceOld[0] && fIceOldOriginal[1] == evapoTranspirationState->fIceOld[1] &&
               fIceOldOriginal[2] == evapoTranspirationState->fIceOld[2] && zSoilOriginal[0] == zSoil[0] && zSoilOriginal[1] == zSoil[1] &&
               zSoilOriginal[2] == zSoil[2] && zSoilOriginal[3] == zSoil[3]);
      
      // Verify that the fraction of the precipitation that falls as snow is between 0 and 1, the snowfall rate below the canopy is not negative, and the
      // snowmelt out the bottom of the snowpack is not negative.
      CkAssert(0.0f <= fpIce && 1.0f >= fpIce && 0.0f <= qSnow && 0.0f <= qSnBot);
      
      // Verify that there is no snowfall interception in the non-existant canopy.
      CkAssert(prcp * fpIce == qSnow);
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
      // Store fIce from the beginning of the timestep in fIceOld.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
        {
          if (ii >= iSnowOriginal + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS)
            {
              evapoTranspirationState->fIceOld[ii] = fIce[ii];
            }
          else
            {
              evapoTranspirationState->fIceOld[ii] = 0.0;
            }
        }
         
      // Calculate derived output variables.
      evaporationFromSurface = eDir * dt;

      // Surface condensation is added to the snow layer if any snow exists or otherwise added to the ground as water.  Surface evaporation is taken first from
      // the snow layer up to the amount in snEqvO, and then any remaining is taken from the ground.
      if (0.0f > evaporationFromSurface)
        {
          if (0.0f < evapoTranspirationState->snEqvO)
            {
              *evaporationFromSnow   = evaporationFromSurface;
              *evaporationFromGround = 0.0f;
            }
          else
            {
              *evaporationFromSnow   = 0.0f;
              *evaporationFromGround = evaporationFromSurface;
            }
        }
      else if (evaporationFromSurface <= evapoTranspirationState->snEqvO)
        {
          *evaporationFromSnow   = evaporationFromSurface;
          *evaporationFromGround = 0.0f;
        }
      else
        {
          *evaporationFromSnow   = evapoTranspirationState->snEqvO;
          *evaporationFromGround = evaporationFromSurface - evapoTranspirationState->snEqvO;
        }
      
      snowfall         = prcp * dt * fpIce;
      snowmeltOnGround = qSnBot * dt;
      rainfall         = prcp * dt - snowfall;
      
      // If there is a snow layer at the end of the timestep it intercepts all of the rainfall.
      if (0 > evapoTranspirationState->iSnow)
        {
          rainfallInterceptedBySnow = rainfall;
          rainfallOnGround          = 0.0;
        }
      else
        {
          rainfallInterceptedBySnow = 0.0;
          rainfallOnGround          = rainfall;
        }
      
      // When the total snow height gets less than 2.5 cm the multi-layer snow simulation turns off.  When this happens all of the liquid water in snLiq
      // becomes snowmelt on the ground and snEqv gets set to just the portion in snIce.  However, melting/freezing between snIce and snLiq also happens during
      // the timestep so we can't use the beginning timestep value of snLiq to determine how much to add to snowmeltOnGround.  We have to use the final value
      // of snEqv to back out the value of snowmeltOnGround.
      if (0 > iSnowOriginal && 0 == evapoTranspirationState->iSnow)
        {
          snowmeltOnGround = snEqvOriginal + snowfall - *evaporationFromSnow - evapoTranspirationState->snEqv;
        }
      else
        {
          // There is a case where snEqvO does not equal snEqv at the beginning of the timestep.  This appears to happen when the multi-layer snow simulation
          // is turned off, and the snow is melting.  In this case, qSnBot is zero, but some water has disappeared from snEqvO and snEqv and the water shows up
          // in the soil moisture.  The solution to this is to add the difference between snEqvOriginal and snEqvO to snowmeltOnGround.
          snowmeltOnGround += snEqvOriginal - evapoTranspirationState->snEqvO;
        }
      
      // If snEqv falls below 0.001 mm, or snowH falls below 1e-6 m then Noah-MP sets both to zero and the water is lost.  If snEqv grows above 2000 mm then
      // Noah-MP sets it to 2000 and the water is lost.  We are calculating what snEqv should be and putting the water back.
      snEqvShouldBe = snEqvOriginal + snowfall + rainfallInterceptedBySnow - *evaporationFromSnow - snowmeltOnGround;
      
      if (0.0 == evapoTranspirationState->snEqv)
        {
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonGreaterOrEqual(0.001f, snEqvShouldBe));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          evapoTranspirationState->snEqv = snEqvShouldBe;
          evapoTranspirationState->snowH = snEqvShouldBe / 1000.0f; // Divide by one thousand to convert from millimeters to meters.
        }
      else if (epsilonEqual(2000.0f, evapoTranspirationState->snEqv))
        {
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          CkAssert(epsilonLessOrEqual(2000.0f, snEqvShouldBe));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
          
          if (0 > evapoTranspirationState->iSnow)
            {
              evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] = -evapoTranspirationState->snowH;
              
              for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
                {
                  evapoTranspirationState->zSnso[ii + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS] =
                      evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] + zSoil[ii];
                }
              
              evapoTranspirationState->snIce[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1] += snEqvShouldBe - evapoTranspirationState->snEqv;
            }
          
          evapoTranspirationState->snEqv = snEqvShouldBe;
        }
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      else
        {
          CkAssert(epsilonEqual(evapoTranspirationState->snEqv, snEqvShouldBe));
        }
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_INVARIANTS)
      CkAssert(!checkEvapoTranspirationStateStructInvariant(evapoTranspirationState));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_INVARIANTS)
      
      *surfacewaterAdd = snowmeltOnGround + rainfallOnGround;
      
#if (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
      CkAssert(0.0f <= *surfacewaterAdd);
      
      // Calculate soil moisture at the end of the timestep.
      for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SOIL_LAYERS; ii++)
        {
          if (0 == ii)
            {
              soilMoistureNew += smc[ii] * -zSoil[ii] * 1000.0f; // * 1000.0f because zSoil is in meters and soilMoistureNew is in millimeters.
            }
          else
            {
              soilMoistureNew += smc[ii] * (zSoil[ii - 1] - zSoil[ii]) * 1000.0f;
            }
        }
      
      // Verify that soil moisture balances.
      CkAssert(epsilonEqual(soilMoistureOriginal - *evaporationFromGround + *surfacewaterAdd - runSrf * dt - runSub * dt, soilMoistureNew));
#endif // (DEBUG_LEVEL & DEBUG_LEVEL_INTERNAL_SIMPLE)
    } // End if (!error).
  
  return error;
}

bool checkEvapoTranspirationStateStructInvariant(EvapoTranspirationStateStruct* evapoTranspirationState)
{
  bool  error          = false; // Error flag.
  int   ii;                     // Loop counter.
  float totalSnowWater = 0.0f;  // Total water in all snow layers in millimeters of water equivalent.

  for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
    {
      if (!(0.0f <= evapoTranspirationState->fIceOld[ii] && evapoTranspirationState->fIceOld[ii] <= 1.0f))
        {
          CkError("ERROR in checkEvapoTranspirationStateStructInvariant, snow layer %d: fIceOld must be greater than or equal to zero and less than or equal "
                  "to one.\n", ii);
          error = true;
        }
    }

  if (!(0.0f <= evapoTranspirationState->albOld && evapoTranspirationState->albOld <= 1.0f))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: albOld must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->snEqvO))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: snEqvO must be greater than or equal to zero.\n");
      error = true;
    }

  for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_ALL_LAYERS; ii++)
    {
      if (ii < evapoTranspirationState->iSnow + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS)
        {
          // Snow layer does not exist.
          if (!(0.0f == evapoTranspirationState->stc[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, non-existant snow layer %d: stc must be equal to zero.\n", ii);
              error = true;
            }
        }
      else
        {
          // Snow or soil layer does exist.
          if (!(0.0f <= evapoTranspirationState->stc[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, snow/soil layer %d: stc must be greater than or equal to zero.\n", ii);
              error = true;
            }
          else if (!(-70.0f + ZERO_C_IN_KELVIN <= evapoTranspirationState->stc[ii]))
            {
              if (2 <= ADHydro::verbosityLevel)
                {
                  CkError("WARNING in checkEvapoTranspirationStateStructInvariant, snow/soil layer %d: stc below -70 degrees C.\n", ii);
                }
            }
          else if (!(70.0f + ZERO_C_IN_KELVIN >= evapoTranspirationState->stc[ii]))
            {
              if (2 <= ADHydro::verbosityLevel)
                {
                  CkError("WARNING in checkEvapoTranspirationStateStructInvariant, snow/soil layer %d: stc above 70 degrees C.\n", ii);
                }
            }
        }
    }

  if (!(0.0f <= evapoTranspirationState->tah))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: tah must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= evapoTranspirationState->tah))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: tah below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= evapoTranspirationState->tah))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: tah above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= evapoTranspirationState->eah))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: eah must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->fWet && evapoTranspirationState->fWet <= 1.0f))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: fWet must be greater than or equal to zero and less than or equal to one.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->canLiq))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: canLiq must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->canIce))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: canIce must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->tv))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: tv must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= evapoTranspirationState->tv))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: tv below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= evapoTranspirationState->tv))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: tv above 70 degrees C.\n");
        }
    }

  if (!(0.0f <= evapoTranspirationState->tg))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: tg must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(-70.0f + ZERO_C_IN_KELVIN <= evapoTranspirationState->tg))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: tg below -70 degrees C.\n");
        }
    }
  else if (!(70.0f + ZERO_C_IN_KELVIN >= evapoTranspirationState->tg))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: tg above 70 degrees C.\n");
        }
    }

  if (!(-EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS <= evapoTranspirationState->iSnow && evapoTranspirationState->iSnow <= 0))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: iSnow must be greater than or equal to minus three and less than or equal to zero.\n");
      error = true;
    }

  for (ii = 0; ii < EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii++)
    {
      if (ii < evapoTranspirationState->iSnow + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS)
        {
          // Snow layer does not exist.
          if (!(0.0f == evapoTranspirationState->zSnso[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, non-existant snow layer %d: zSnso must be equal to zero.\n", ii);
              error = true;
            }

          if (!(0.0f == evapoTranspirationState->snIce[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, non-existant snow layer %d: snIce must be equal to zero.\n", ii);
              error = true;
            }

          if (!(0.0f == evapoTranspirationState->snLiq[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, non-existant snow layer %d: snLiq must be equal to zero.\n", ii);
              error = true;
            }
        }
      else
        {
          // Snow layer does exist.
          if (ii == evapoTranspirationState->iSnow + EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS)
            {
              // Top snow layer.
              if (!(0.0f > evapoTranspirationState->zSnso[ii]))
                {
                  CkError("ERROR in checkEvapoTranspirationStateStructInvariant, snow layer %d: zSnso must be less than zero.\n", ii);
                  error = true;
                }
            }
          else
            {
              // Not top snow layer.
              if (!(evapoTranspirationState->zSnso[ii - 1] > evapoTranspirationState->zSnso[ii]))
                {
                  CkError("ERROR in checkEvapoTranspirationStateStructInvariant, snow layer %d: zSnso must be less than the layer above it.\n", ii);
                  error = true;
                }
            }

          if (!(0.0f < evapoTranspirationState->snIce[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, snow layer %d: snIce must be greater than zero.\n", ii);
              error = true;
            }

          if (!(0.0f <= evapoTranspirationState->snLiq[ii]))
            {
              CkError("ERROR in checkEvapoTranspirationStateStructInvariant, snow layer %d: snLiq must be greater than or equal to zero.\n", ii);
              error = true;
            }

          totalSnowWater += evapoTranspirationState->snIce[ii] + evapoTranspirationState->snLiq[ii];
        }
    }

  for (ii = EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS; ii < EVAPO_TRANSPIRATION_NUMBER_OF_ALL_LAYERS; ii++)
    {
      if (!(evapoTranspirationState->zSnso[ii - 1] > evapoTranspirationState->zSnso[ii]))
        {
          CkError("ERROR in checkEvapoTranspirationStateStructInvariant, soil layer %d: zSnso must be less than the layer above it.\n",
                  ii - EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS);
          error = true;
        }
    }

  if (!(0.0f <= evapoTranspirationState->snowH))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: snowH must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->snEqv))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: snEqv must be greater than or equal to zero.\n");
      error = true;
    }

  // If multi-layer snow simulation is turned on snowH and snEqv must be consistent with zSnso, snIce, and snLiq.
  if (0 > evapoTranspirationState->iSnow)
    {
      if (!epsilonEqual(-evapoTranspirationState->zSnso[EVAPO_TRANSPIRATION_NUMBER_OF_SNOW_LAYERS - 1], evapoTranspirationState->snowH))
        {
          CkError("ERROR in checkEvapoTranspirationStateStructInvariant: snowH must be consistent with zSnso.\n");
          error = true;
        }

      if (!epsilonEqual(totalSnowWater, evapoTranspirationState->snEqv))
        {
          CkError("ERROR in checkEvapoTranspirationStateStructInvariant: snEqv must be consistent with snIce and snLiq.\n");
          error = true;
        }
    }

  if (!(0.0f <= evapoTranspirationState->lfMass))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: lfMass must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->rtMass))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: rtMass must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->stMass))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: stMass must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->wood))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: wood must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->stblCp))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: stblCp must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->fastCp))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: fastCp must be greater than or equal to zero.\n");
      error = true;
    }

  if (!(0.0f <= evapoTranspirationState->lai))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: lai must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(6.0f >= evapoTranspirationState->lai))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: lai above six.\n");
        }
    }

  if (!(0.0f <= evapoTranspirationState->sai))
    {
      CkError("ERROR in checkEvapoTranspirationStateStructInvariant: sai must be greater than or equal to zero.\n");
      error = true;
    }
  else if (!(2.0f >= evapoTranspirationState->sai))
    {
      if (2 <= ADHydro::verbosityLevel)
        {
          CkError("WARNING in checkEvapoTranspirationStateStructInvariant: sai above two.\n");
        }
    }

  return error;
}
