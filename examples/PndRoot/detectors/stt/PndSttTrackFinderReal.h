//****************************************************************************
//*                   This file is part of PandaRoot.                        *
//*                                                                          *
//*            PandaRoot is distributed under the terms of the               *
//*              GNU General Public License (GPL) version 3,                 *
//*                 copied verbatim in the file "LICENSE".                   *
//*                                                                          *
//*  Copyright (C) 2006 - 2024 FAIR GmbH and copyright holders of PandaRoot  *
//*     The copyright holders are listed in the file "COPYRIGHTHOLDERS".     *
//*               The authors are listed in the file "AUTHORS".              *
//****************************************************************************

#ifndef PNDSTTTRACKFINDERREAL
#define PNDSTTTRACKFINDERREAL 1

#include "PndSttTrackFinder.h"
#include "PndMCTrack.h"
#include "PndSttTrack.h"

#include "TList.h"
#include "TClonesArray.h"
#include "TH1.h"
//-------------------------------

class FairMCPoint;

struct CalculatedCircles {
  Int_t Ncircles;
  Double_t CX[8];
  Double_t CY[8];
  Double_t R[8];
};

struct CalculatedHelix {
  Int_t Nhelix[3];
  Double_t CX;
  Double_t CY;
  Double_t R;
  Double_t KAPPA[3][16];
  Double_t FI0[3][16];
};

class PndSttTrackFinderReal : public PndSttTrackFinder {

 public:
  /** Default constructor **/
  PndSttTrackFinderReal();

  /** Standard constructor **/
  PndSttTrackFinderReal(Int_t verbose);

  /** Second constructor **/
  PndSttTrackFinderReal(int istamp, bool iplot, bool doMcComparison);

  /** Third constructor **/
  PndSttTrackFinderReal(int istamp, bool iplot, bool doMcComparison, bool doSciTil);

  /** Destructor **/
  virtual ~PndSttTrackFinderReal();

  /** Initialisation **/
  virtual void Init();

  /** Histogramming **/

  void WriteHistograms();

  /** Track finding algorithm **/
  virtual Int_t DoFind(TClonesArray *trackCandArray, TClonesArray *trackArray, TClonesArray *helixHitArray);
  virtual Int_t DoFind(TClonesArray *mHitArray, TClonesArray *mHelixHitArray);

  virtual void AddHitCollection(TClonesArray *mHitArray, TClonesArray *mPointArray)
  {
    fHitCollectionList.Add(mHitArray);
    fPointCollectionList.Add(mPointArray);
  };

  void SetTubeArray(TClonesArray *tubeArray) { fTubeArray = tubeArray; };

  void SetInputBranchName(char *string1)
  {
    sprintf(fSttBranch, "%s", string1);
    return;
  };

  void Finish(){};

 private:
  static const Short_t nmaxHits = 1000, // max hits total.
    nmaxHitsInTrack = 60,
                       nmaxSciTilHits = 200, // max SciTil hits total.
    nmaxSciTilHitsinTrack = 2,               // max SciTil hits in one track.
    MAXMCTRACKS = 10000, MAXTRACKSPEREVENT = 50, MAXHITSINCELL = 50, nmaxinclinationversors = 20, nAdmittedRadia = 3, nbinCX = 100, nbinCY = 100, nbinZ = 100, nbinR = 100,
                       nbinD = 250, nbinFi = 250, nbinKAPPA = 200, nbinFI0 = 200, MINIMUMCOUNTSDFiR = 18, MINIMUMCOUNTSKAPPAFI0 = 5, MAXElementsOverThresholdinHough = 500,
                       nRdivConformal = 10, NHITSINFIT = 15,
                       DELTAnR = 2; //  defines the range of nR in PndSttTrkAssociatedParallelHitsToHelixBis

  static const Double_t PI = 3.141592654,
                        RStrawDetectorMin = 16.119, // minimum radius of the Stt detector in  cm
    ApotemaMaxInnerParStraw = 23.246827,
                        ApotemaMinSkewStraw = 23.246827, // delimitation of the skew area
    ApotemaMaxSkewStraw = 31.517569,                     // delimitation of the skew area
    ApotemaMinOuterParStraw = 31.863369,
                        RStrawDetectorMax = 40.73, // maximum radius of the Stt detector in  cm
    VERTICALGAP = 4.,                              // in cm, the gap between Left and Right sections of the
                                                   //  Central Detector.
    Rmin = 20., Rmax = 700., PMAX = 100., STRAWRADIUS = 0.5, StrawDriftError = 0.02, SKEWinclination_DEGREES = 3., CXmin = -150., CXmax = 150., CYmin = -300., CYmax = 300.,
                        Dmin = -21., Dmax = 21., KAPPAmin = -2., KAPPAmax = 2., Zmin = -75., Zmax = 75., DELTA_R = 5., DELTA_Fi = 0.3, DELTA_D = 2., DELTA_KAPPA = 0.03,
                        DELTA_FI0 = 0.3,
                        RMAXSCITIL = 50., // cm
    DIMENSIONSCITIL = 2.85,               // cm
    BFIELD = 2.,                          // in Tesla
    CVEL = 2.99792;                       //  velocity of light
  static const bool YesClean = false;
  bool YesSciTil, InclusionListSciTil[nmaxSciTilHits];

  static const Short_t nFidivConformal = (Short_t)(3.141592654 * 45. / 0.5);

  static const int nmassimo = 50,
                   TIMEOUT = 60; // timeout in seconds for the GLPK fitting.

  int IVOLTE;

  bool iplotta, doMcComparison, TypeConf[MAXTRACKSPEREVENT];

  int istampa;

  TH1F *hdist, *hdistgoodlast, *hdistbadlast;

  FILE *HANDLE;
  FILE *HANDLE2;
  FILE *HANDLEXYZ;
  FILE *PHANDLEX;
  FILE *PHANDLEY;
  FILE *PHANDLEZ;
  FILE *SHANDLEX;
  FILE *SHANDLEY;
  FILE *SHANDLEZ;
  Double_t veritaMC[nmaxHits][3];

  Short_t MINIMUMHITSPERTRACK, MINIMUMOUTERHITSPERTRACK, nRdivConformalEffective, nSciTilHits, nSttSkewhit, infoparal[nmaxHits], infoskew[nmaxHits],
    nHitsInMCTrack[MAXTRACKSPEREVENT], nSciTilHitsinTrack[MAXTRACKSPEREVENT], nSttSkewhitInMCTrack[MAXTRACKSPEREVENT],
    ListSciTilHitsinTrack[MAXTRACKSPEREVENT][nmaxSciTilHitsinTrack];

  Short_t nMCTracks;

  Double_t Fimin, Fimax, FI0min, FI0max, stepD, stepFi, stepR, stepKAPPA, stepFI0, stepfineKAPPA, stepfineFI0, SEMILENGTH_STRAIGHT, ZCENTER_STRAIGHT, ALFA[MAXTRACKSPEREVENT],
    BETA[MAXTRACKSPEREVENT], GAMMA[MAXTRACKSPEREVENT], radiaConf[nRdivConformal], CxMC[MAXTRACKSPEREVENT], CyMC[MAXTRACKSPEREVENT], R_MC[MAXTRACKSPEREVENT],
    posizSciTil[nmaxSciTilHits][3], S_SciTilHitsinTrack[MAXTRACKSPEREVENT][nmaxSciTilHits];

  TClonesArray *fMCTrackArray, *fSciTPointArray, *fSciTHitArray;

  TClonesArray *fTubeArray;
  PndMCTrack *pMCtr;

  TList fHitCollectionList;
  TList fPointCollectionList;

  PndSttHit *GetHitFromCollections(Int_t hitCounter);
  FairMCPoint *GetPointFromCollections(Int_t hitCounter);
  TClonesArray *fSttHitArray;

  /**  Branch name to be used to fetch the hits of the backgound mixed events  **/
  char fSttBranch[100];

  void Initialization_ClassVariables();

  void PndSttFromXYtoConformal(Double_t trajectory_vertex[3], Double_t info[][7], Int_t Nparal, Double_t infoparalConformal[][5], Int_t *status);

  void PndSttFromXYtoConformal2(Double_t trajectory_vertex[3], Short_t nHitsinTrack, Short_t iExclude, Short_t *ListHits, Double_t info[][7], Double_t auxinfoparalConformal[][5],
                                Int_t *status);

  void PndSttBoxConformalFilling(bool ExclusionList[nmaxHits], Double_t infoparalConformal[][5], Int_t Nparal, Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                 Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal], Short_t RConformalIndex[nmaxHits], Short_t FiConformalIndex[nmaxHits]);

  void Merge_Sort(Short_t n_ele, Double_t *array, Short_t *ind);

  void Merge(Short_t nl, Double_t *left, Short_t *ind_left, Short_t nr, Double_t *right, Short_t *ind_right, Double_t *result, Short_t *ind);

  Short_t PndSttFindTrackPatterninBoxConformal(Short_t NRCELLDISTANCE, Short_t NFiCELLDISTANCE, Short_t Nparal,
                                               Short_t ihit,    // seed hit;
                                               Short_t nRcell,  // R cell of the seed hit;
                                                                // can be negative beacuse of SciTil hits;
                                               Short_t nFicell, // Fi cell of the seed hit;
                                               Double_t info[][7], bool Exclusion_List[nmaxHits], Short_t RConformalIndex[nmaxHits], Short_t FiConformalIndex[nmaxHits],
                                               Short_t nBoxConformal[nRdivConformal][nFidivConformal], Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal],
                                               Short_t *ListHitsinTrack);

  Short_t PndSttFindTrackPatterninBoxConformalSpecial(Short_t NRCELLDISTANCE, Short_t NFiCELLDISTANCE, Short_t Nparal, Short_t NparallelToSearch, Short_t iSeed,
                                                      Short_t *ListHitsinTrackinWhichToSearch, Double_t info[][7], bool InclusionList[nmaxHits], Short_t RConformalIndex[nmaxHits],
                                                      Short_t FiConformalIndex[nmaxHits], Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                      Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal], Short_t *OutputListHitsinTrack);

  Short_t PndSttFindTrackStrictCollection(Short_t NFiCELLDISTANCE,
                                          Short_t iSeed,             //  seed track (parallel notation) as fa as the Fi angle is concerned
                                          Short_t NParallelToSearch, //  n. of hits to search in ListHitsinTrackinWhichToSearch
                                          Short_t *ListHitsinTrackinWhichToSearch, bool ExclusionList[nmaxHits], Short_t FiConformalIndex[nmaxHits],
                                          Short_t *OutputListHitsinTrack);

  //----   questa funzione e'
  //  identica a quella di sttmvd che funziona sia con soli STT hits che con
  //  Stt+Mvd hits.
  Short_t FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal, Double_t *ErrorDriftRadiusconformal,
                           Double_t rotationangle, Double_t *trajectory_vertex, Short_t NMAX, Double_t *m, Double_t *q, Double_t *ALFA, Double_t *BETA, Double_t *GAMMA,
                           bool *TypeConf);

  Short_t PndSttFitSZspacebis(Short_t nSttSkewhitinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t FInot, Short_t NMAX, Double_t *m);

  // rimpiazza la precedente
  Short_t FitSZspace(Short_t nSkewHitsinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t *ErrorDriftRadius, Double_t FInot, Short_t NMAX, Double_t *emme);

  Short_t PndSttTrkAssociatedParallelHitsToHelix(Double_t Ox, Double_t Oy, Double_t R, Int_t Nhits, Double_t info[][7],
                                                 Short_t *auxListHitsinTrack //  this is the output
  );

  Short_t PndSttTrkAssociatedParallelHitsToHelixQuater(bool ExclusionList[nmaxHits], Double_t m, Double_t q, Short_t Status, Short_t nHitsinTrack, Short_t *ListHitsinTrack,
                                                       Int_t NhitsParallel, Double_t Ox, Double_t Oy, Double_t R, Double_t info[][7], Double_t infoparalConformal[][5],
                                                       Short_t *RConformalIndex, Short_t *FiConformalIndex, Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                       Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal], Short_t *auxListHitsinTrack);

  Short_t PndSttTrkAssociatedParallelHitsToHelix5(bool ExclusionList[nmaxHits], Int_t NhitsParallel, Double_t Ox, Double_t Oy, Double_t R, Double_t info[][7], Double_t Fi_low,
                                                  Double_t Fi_up, Short_t *auxListHitsinTrack);

  bool PndSttAcceptHitsConformal(Double_t distance, Double_t DriftConfR, Double_t StrawConfR);

  CalculatedCircles PndSttTrkFindCircles(Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t, Double_t);

  CalculatedHelix PndSttTrkFindHelix(Double_t Ox, Double_t Oy, Double_t R, Double_t Zcenter1, Double_t Zcenter2, Double_t Zcenter3, Double_t semilengthStraight1,
                                     Double_t semilengthStraight2, Double_t semilengthStraight3, Double_t C0x1, Double_t C0y1, Double_t C0z1, Double_t semilengthSkew1, Double_t r1,
                                     Double_t vx1, Double_t vy1, Double_t vz1, Double_t C0x2, Double_t C0y2, Double_t C0z2, Double_t semilengthSkew2, Double_t r2, Double_t vx2,
                                     Double_t vy2, Double_t vz2, Int_t *STATUS);

  void calculateintersections(Double_t Ox, Double_t Oy, Double_t R, Double_t C0x, Double_t C0y, Double_t C0z, Double_t r, Double_t vx, Double_t vy, Double_t vz, Int_t *STATUS,
                              Double_t *POINTS);

  void plottamentiParalleleGenerali(Int_t Nremaining, Float_t *RemainingR, Float_t *RemainingD, Float_t *RemainingFi, Float_t *RemainingCX, Float_t *RemainingCY, bool *Goodflag);

  void plottamentiParalleleconMassimo(char *tipo, Int_t nMaxima, Int_t Nremaining, Float_t *RemainingR, Float_t *RemainingD, Float_t *RemainingFi, Float_t *RemainingCX,
                                      Float_t *RemainingCY, Double_t Rup, Double_t Rlow, Double_t Dup, Double_t Dlow, Double_t Fiup, Double_t Filow);

  bool iscontiguous(int ncomponents, Short_t *vec1, Short_t *vec2);

  void clustering2(Short_t vec1[2],                                               // input
                   int nListElements, Short_t List[][2],                          // input
                   int &nClusterElementsFound, Short_t ClusterElementsFound[][2], // output
                   int &nRemainingElements, Short_t RemainingElements[][2]        // output
  );

  void clustering3(Short_t vec1[3],                                               // input
                   int nListElements, Short_t List[][3],                          // input
                   int &nClusterElementsFound, Short_t ClusterElementsFound[][3], // output
                   int &nRemainingElements, Short_t RemainingElements[][3]        // output
  );

  void WriteMacroParallelAssociatedHits(Double_t Ox, Double_t Oy, Double_t R, Short_t Nhits, Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Double_t info[][7],
                                        Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3], Short_t imaxima, Int_t sequencial, Short_t nscitilhitsintrack,
                                        Short_t *listscitilhitsintrack);

  void WriteMacroParallelAssociatedHitswithMC(Double_t Ox, Double_t Oy, Double_t R, Short_t TrackFoundaTrackMC, Short_t Nhits,
                                              Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Double_t info[][7], Short_t ifoundtrack, Int_t sequentialNTrack,
                                              Short_t nscitilhitsintrack, Short_t *listscitilhitsintrack, Short_t nParalCommon[MAXTRACKSPEREVENT],
                                              Short_t ParalCommonList[MAXTRACKSPEREVENT][nmaxHits], Short_t nSpuriParinTrack[MAXTRACKSPEREVENT],
                                              Short_t ParSpuriList[MAXTRACKSPEREVENT][nmaxHits], Short_t nMCParalAlone[MAXTRACKSPEREVENT],
                                              Short_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxHits]);
  void WriteMacroParallelHitsGeneral(bool *keepit, Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3], Short_t nTracksFoundSoFar);

  void WriteMacroParallelHitsGeneralConformalwithMC(bool *keepit, Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                                                    Short_t nTracksFoundSoFar);

  void WriteMacroParallelHitsConformalwithMCspecial(Int_t Nhits, Double_t auxinfoparalConformal[][5], Short_t nTracksFoundSoFar, Short_t Status, Double_t *trajectory_vertex);

  void WriteMacroSkewAssociatedHits(bool goodskewfit, Double_t KAPPA, Double_t FI0, Double_t D, Double_t Fi, Double_t R, Double_t info[][7], Int_t Nincl, Int_t Minclinations[],
                                    Double_t inclination[][3], Int_t imaxima, Int_t sequentialNTrack, Short_t nSttSkewhitinTrack,
                                    Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t nscitilhits, Double_t *ESSE, Double_t *ZETA);

  void WriteMacroSkewAssociatedHitswithMC(bool goodskewfit, Double_t KAPPA, Double_t FI0, Double_t D, Double_t Fi, Double_t R, Double_t info[][7], Int_t Nincl,
                                          Int_t Minclinations[], Double_t inclination[][3], Int_t imaxima, Int_t sequentialNTrack, Short_t nSttSkewhitinTrack,
                                          Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t nSkewCommon, Short_t SkewCommonList[MAXTRACKSPEREVENT][nmaxHits],
                                          Short_t daTrackFoundaTrackMC, Short_t nMCSkewAlone[MAXTRACKSPEREVENT], Short_t MCSkewAloneList[MAXTRACKSPEREVENT][nmaxHits],
                                          Short_t nscitilhits, Double_t *ESSE, Double_t *ZETA

  );

  Short_t AssociateSkewHitsToXYTrack(bool *ExclusionListSkew, Double_t Ox, Double_t Oy, Double_t R, Double_t info[][7], Double_t inclination[][3], Double_t Fi_low_limit,
                                     Double_t Fi_up_limit,
                                     //                   Double_t Fi_allowedforskew_low,
                                     //                   Double_t Fi_allowedforskew_up,
                                     Short_t Charge, Double_t Fi_initial_helix_referenceframe, Double_t Fi_final_helix_referenceframe,
                                     Short_t SkewList[nmaxHits][2], // output,  list of selected skew hits (in skew numbering)
                                     Double_t *S,                   //  output,  S coordinate of selected Skew hit
                                     Double_t *Z,                   //  output,  Z coordinate of selected Skew hit
                                     Double_t *ZDrift,              //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                                     Double_t *ZRadiusafterTilt     //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
  );

  Short_t AssociateBetterAfterFitSkewHitsToXYTrack(Short_t TemporarynSttSkewhitinTrack,
                                                   Short_t SkewList[nmaxHits][2], // input,  list of selected skew hits (in skew numbering)
                                                   Double_t *S,                   //  input,  S coordinate of selected Skew hit
                                                   Double_t *Z,                   //  input,  Z coordinate of selected Skew hit
                                                   Double_t *ZDrift,              //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
                                                   Double_t *ZRadiusafterTilt,    //  input,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
                                                   Double_t KAPPA,                // input, KAPPA result of fit
                                                   Double_t FI0,                  // input, FI0 result of fit
                                                   Short_t *tempore,
                                                   Double_t *temporeS,               //  output, associated skew hit  S
                                                   Double_t *temporeZ,               //  output, associated skew hits Z
                                                   Double_t *temporeZDrift,          //  output, associated skew hit Z drift
                                                   Double_t *temporeZErrorafterTilt, //  output, associated skew hits Z error after tilt
                                                   Int_t *STATUS                     // output
  );

  void PndSttOrderingParallel(Double_t oX, Double_t oY, Double_t info[][7], Short_t nParallelHits, Short_t *ListParallelHits, Short_t *Infoparal, Short_t Charge,
                              Double_t *Fi_initial_helix_referenceframe, Double_t *Fi_final_helix_referenceframe, Double_t *U, Double_t *V);

  void PndSttOrderingSkewandParallel(Short_t *Infoparal, Short_t *Infoskew, Double_t oX, Double_t oY, Double_t Rr, Short_t nSttSkewhit, Short_t *ListSkewHits, Double_t *SList,
                                     Short_t Charge, Short_t nParHits, Short_t *ListParHits, Double_t *U, Double_t *V, Short_t *BigList

  );

  void PndSttOrdering(Double_t oX, Double_t oY, Double_t info[][7], Short_t nParallelHits, Short_t *ListParallelHits, Short_t nSttSkewhit, Short_t *ListSkewHits, Double_t *S,
                      Short_t *Infoparal, Short_t *Infoskew, Short_t *nTotal, Short_t *BigList, Short_t *Charge);

  void PndSttFindingParallelTrackAngularRange(Double_t oX, Double_t oY, Double_t r, Short_t Charge,
                                              Double_t *Fi_low_limit, // Fi (in XY Helix frame) lower limit using
                                                                      // the Stt detector minimum/maximum radius
                                                                      // Fi_low_limit is ALWAYS between 0. and 2PI
                                              Double_t *Fi_up_limit,  // Fi (in XY Helix frame) upper limit using
                                                                      // the Stt detector maximum/minimum radius
                                                                      // Fi_up_limit is ALWAYS > Fi_low_limit and
                                                                      // possibly > 2PI.
                                              Short_t *status,
                                              Double_t Rmin, // Rmin of cylindrical volume intersected by track;
                                              Double_t Rmax  // Rmax of cylindrical volume intersected by track;
  );

  void WriteMacroParallelHitswithRfromMC(Int_t Nhits, Double_t info[][7], Short_t nTracksFoundSoFar,
                                         //                   bool *TypeConf,
                                         Double_t *Ox, Double_t *Oy, Short_t *daParTrackFoundaTrackMC);

  void WriteMacroSkewAssociatedHitswithRfromMC(Double_t KAPPA, Double_t FI0, Double_t D, Double_t Fi, Double_t R, Int_t Nhits, Double_t info[][7], Int_t Nincl,
                                               Int_t Minclinations[], Double_t inclination[][3], Int_t imaxima, Int_t nMaxima);

  void AssociateFoundTrackstoMC(Double_t info[][7], Short_t nTracksFoundSoFar, Short_t *nHitsinTrack, Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                                Short_t *nSttSkewhitinTrack, Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits], Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]);

  void AssociateFoundTrackstoMCbis(bool *keepit, Double_t info[][7], Short_t nTracksFoundSoFar, Short_t nHitsinTrack[MAXTRACKSPEREVENT],
                                   Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t nSttSkewhitinTrack[MAXTRACKSPEREVENT],
                                   Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]);

  void PndSttInfoXYZParal(Double_t info[][7], Short_t infopar, Double_t Ox, Double_t Oy, Double_t R, Double_t KAPPA, Double_t FI0, Short_t Charge,
                          Double_t *Posiz //  output
  );

  void PndSttInfoXYZSkew(Double_t Z,      //  Z coordinate of selected Skew hit
                         Double_t ZDrift, // drift distance IN Z DIRECTION only, of Skew hit
                         Double_t S, Double_t Ox, Double_t Oy, Double_t R, Double_t KAPPA, Double_t FI0, Short_t Charge,
                         Double_t *Posiz //  output
  );

  void FixDiscontinuitiesFiangleinSZplane(Short_t TemporarynSttSkewhitinTrack, Double_t *S, Double_t *Fi_initial_helix_referenceframe, Short_t Charge);

  void FindCharge(Double_t oX, Double_t oY, Short_t nParallelHits, Double_t *X, Double_t *Y, Short_t *Charge);

  bool SttParalCleanup(Double_t GAP, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3], Double_t FI0, Double_t FiLimitAdmissible, Short_t nHits,
                       Short_t *Listofhits, Double_t info[][7], Double_t RStrawDetMin, Double_t RStrawDetInnerParMax, Double_t RStrawDetOuterParMin, Double_t RStrawDetMax);

  bool SttSkewCleanup(Double_t GAP, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3], Double_t FI0, Double_t FiLimitAdmissible, Short_t nHits,
                      Short_t *Listofhits, Double_t *S, Double_t info[][7], Double_t RminStrawSkew, Double_t RmaxStrawSkew,
                      Double_t cut,  // cut distance (in cm).
                      Short_t maxnum // max number allowed of failures to pass the cut.
  );

  bool BadTrack_ParStt(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge,
                       Double_t Xcross[2], // Xcross[0]=point of entrance;
                                           //  Xcross[1]=point of exit.
                       Double_t Ycross[2], Short_t nHits, Short_t *ListHits, Double_t info[][7], Double_t cut, Short_t maxnum,
                       Short_t islack // uncertainty allowed as far as
                                      // the n. of hits that should be present.
  );

  Short_t IntersectionsWithClosedPolygon(Double_t Ox, Double_t Oy, Double_t R,
                                         Double_t Rmi, // Rmin of cylindrical volume intersected by track;
                                         Double_t Rma, // Rmax of cylindrical volume intersected by track;

                                         //-------- outputs
                                         Short_t nIntersections[2], Double_t XintersectionList[][2], Double_t YintersectionList[][2]);
  Short_t IntersectionsWithOpenPolygon(Double_t Ox,    // Track parameter
                                       Double_t Oy,    // Track parameter
                                       Double_t R,     // Track parameter
                                       Short_t nSides, // input, n. of Sides of open Polygon.
                                       Double_t *a,    //  coefficient of formula :  aX + bY + c = 0 defining
                                       Double_t *b,    //  the Polygon sides.
                                       Double_t *c,
                                       Double_t *side_x, // X,Y coordinate of the Sides vertices (in sequence, following
                                       Double_t *side_y, // the Polygon along.
                                       //-------- outputs
                                       Double_t *XintersectionList, // XintersectionList
                                       Double_t *YintersectionList  // YintersectionList.
  );
  Short_t IntersectionsWithClosedbiHexagonLeft(Double_t vgap, Double_t Ox, Double_t Oy, Double_t R,
                                               Double_t Ami, // Apotema min of inner Hexagon;
                                               Double_t Ama, // Apotema max of outer Hexagon;

                                               //-------- outputs
                                               Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList);

  Short_t IntersectionsWithClosedbiHexagonRight(Double_t vgap, Double_t Ox, Double_t Oy, Double_t R,
                                                Double_t Ami, // Apotema min of inner Hexagon;
                                                Double_t Ama, // Apotema max of outer Hexagon;

                                                //-------- outputs
                                                Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList);

  bool IntersectionCircle_Segment(Double_t a, // coefficients implicit equation.
                                  Double_t b, // of segment : a*x + b*y + c =0.
                                  Double_t c,
                                  Double_t P1x, // point delimiting the segment.
                                  Double_t P2x, // point delimiting the segment.
                                  Double_t P1y, // point delimiting the segment.
                                  Double_t P2y, // point delimiting the segment.
                                  Double_t Ox,  // center of circle.
                                  Double_t Oy,
                                  Double_t R, // Radius of circle.
                                  Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2], Double_t *distance);

  bool IntersectionSciTil_Circle(Double_t posizSciTilx, Double_t posizSciTily,
                                 Double_t Oxx, // center of circle.
                                 Double_t Oyy,
                                 Double_t Rr, // Radius of circle.
                                 Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2]);

  Short_t IntersectionsWithGapSemicircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t gap, bool left, Double_t Rma, Double_t *XintersectionList, Double_t *YintersectionList);

  bool IsInternal(Double_t Px, // point
                  Double_t Py, Double_t Xtraslation, Double_t Ytraslation, Double_t Theta);

  void ChooseEntranceExit(Double_t Oxx, Double_t Oyy, Short_t flag, Short_t Charge, Double_t FiStart, Short_t nIntersections[2], Double_t XintersectionList[][2],
                          Double_t YintersectionList[][2],
                          Double_t Xcross[2], // output
                          Double_t Ycross[2]  // output
  );

  void ChooseEntranceExitbis(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t FiStart, Short_t nIntersections, Double_t *XintersectionList, Double_t *YintersectionList,
                             Double_t Xcross[2], // output
                             Double_t Ycross[2]  // output
  );

  Short_t FindTrackEntranceExitbiHexagon(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                         Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                         Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitbiHexagonLeft(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                             Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                             Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2]);

  Short_t FindTrackEntranceExitbiHexagonRight(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                              Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                              Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2]);

  void SeparateInnerOuterParallel(

    // input
    Short_t nHits, Short_t *ListHits, Double_t info[][7], Double_t RStrawDetInnerParMax,

    // output
    Short_t *nInnerHits, Short_t *ListInnerHits, Short_t *nOuterHits, Short_t *ListOuterHits,

    Short_t *nInnerHitsLeft, Short_t *ListInnerHitsLeft, Short_t *nInnerHitsRight, Short_t *ListInnerHitsRight,

    Short_t *nOuterHitsLeft, Short_t *ListOuterHitsLeft, Short_t *nOuterHitsRight, Short_t *ListOuterHitsRight

  );

  Short_t FindTrackEntranceExitHexagonCircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                             Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                             Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2]);
  Short_t FindTrackEntranceExitHexagonCircleLeft(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                 Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                 Double_t ApotemaMax, Double_t GAP, Double_t Xcross[2], Double_t Ycross[2]);
  Short_t FindTrackEntranceExitHexagonCircleRight(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                  Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                  Double_t ApotemaMax, Double_t GAP, Double_t Xcross[2], Double_t Ycross[2]);
  Short_t FindIntersectionsOuterCircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t RMax, Double_t Xcross[2], Double_t Ycross[2]);

  bool IsInsideArc(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t Xcross[2], Double_t Ycross[2], Double_t Spoint);

  bool IsInTargetPipe(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t fi0, Double_t kappa, Short_t charge, Double_t gap);

  Double_t CalculateArcLength(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge,
                              Double_t Xcross[2], // entrance-exit point
                              Double_t Ycross[2]  // entrance-exit point
  );

  void OrderingUsingConformal(Double_t oX, Double_t oY, Int_t nHits, Double_t XY[][2],
                              Int_t Charge, // input
                              Short_t *ListHits);

  bool FindTrackInXYProjection(Short_t iHit,    // seed hit; it is negative for SciTil Hits.
                               Short_t nRcell,  // R cell of seed hit;negative for SciTil hits.
                               Short_t nFicell, // Fi cell of the seed hit;
                               Int_t *Minclinations, Double_t info[nmaxHits][7], bool *ExclusionList, Short_t *RConformalIndex, Short_t *FiConformalIndex,
                               Short_t nBoxConformal[nRdivConformal][nFidivConformal], Short_t HitsinBoxConformal[MAXHITSINCELL][nRdivConformal][nFidivConformal],
                               Short_t nTracksFoundSoFar, Short_t *nHitsinTrack, Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Double_t *trajectory_vertex,
                               Double_t infoparalConformal[nmaxHits][5], Double_t posizSciTilx, Double_t posizSciTily, Double_t *S, Double_t *Ox, Double_t *Oy, Double_t *R,
                               Double_t *Fi_low_limit, Double_t *Fi_up_limit, Double_t *Fi_initial_helix_referenceframe, Double_t *Fi_final_helix_referenceframe, Short_t *Charge,
                               Double_t *U, Double_t *V);

  Short_t AssociateSciTilHit(Double_t Oxx, Double_t Oyy, Double_t Rr,
                             Short_t *List, // output, list of SciTil hits associated (max. 2);
                             Double_t *esse // output, list of  S of the SciTil hits associated.
  );

  void disegnaSciTilHit(FILE *MACRO, int ScitilHit, double posx, double posy, int tipo);

  void disegnaAssiXY(FILE *MACRO, double xmin, double xmax, double ymin, double ymax);

  //----------------------------------------------

  ClassDef(PndSttTrackFinderReal, 1);
};

#endif
