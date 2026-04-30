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

// -------------------------------------------------------------------------
// -----		PndSttTrackFinderReal source file	-----
// -----		by Gianluigi Boca		-----
// -------------------------------------------------------------------------

#include "glpk.h"

// Pnd includes
#include "PndSttTrackFinderReal.h"

#include "PndSttHit.h"
#include "PndSciTHit.h"
#include "PndSttPoint.h"
#include "PndSttHelixHit.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndDetectorList.h"
#include "PndSttTube.h"
#include <cmath>
#include "FairTrackParP.h"
#include "FairMCPoint.h"
#include "FairRootManager.h"
#include "FairRootFileSink.h"

// ROOT includes
#include "TClonesArray.h"
#include "TDatabasePDG.h"
#include "TRandom.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"

// C++ includes
#include <iostream>
#include <map>

using std::cin;
using std::cout;
using std::endl;
using std::map;

// -----   Default constructor   -------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal()
{
  doMcComparison = false;
  Fimin = 0.;
  Fimax = 2. * PI;
  FI0min = 0.;
  FI0max = 2. * PI;
  iplotta = false;
  istampa = 0;
  MINIMUMOUTERHITSPERTRACK = 5;
  nSciTilHits = 0;
  stepD = (Dmax - Dmin) / nbinD;
  stepFi = (Fimax - Fimin) / nbinFi;
  stepR = (Rmax - Rmin) / nbinR;
  stepKAPPA = (KAPPAmax - KAPPAmin) / nbinKAPPA;
  stepFI0 = (FI0max - FI0min) / nbinFI0;
  stepfineKAPPA = 2. * DELTA_KAPPA / nbinKAPPA;
  stepfineFI0 = 2. * DELTA_FI0 / nbinFI0;
  YesSciTil = false;

  Initialization_ClassVariables();
  sprintf(fSttBranch, "STTHit");
}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal(int verbose)
{

  doMcComparison = false;
  Fimin = 0.;
  Fimax = 2. * PI;
  FI0min = 0.;
  FI0max = 2. * PI;
  iplotta = false;
  istampa = verbose;
  MINIMUMOUTERHITSPERTRACK = 5;
  nSciTilHits = 0;
  stepD = (Dmax - Dmin) / nbinD;
  stepFi = (Fimax - Fimin) / nbinFi;
  stepR = (Rmax - Rmin) / nbinR;
  stepKAPPA = (KAPPAmax - KAPPAmin) / nbinKAPPA;
  stepFI0 = (FI0max - FI0min) / nbinFI0;
  stepfineKAPPA = 2. * DELTA_KAPPA / nbinKAPPA;
  stepfineFI0 = 2. * DELTA_FI0 / nbinFI0;
  YesSciTil = false;
  Initialization_ClassVariables();
  sprintf(fSttBranch, "STTHit");
}
// -------------------------------------------------------------------------

// -----   Second constructor   ------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal(int istamp, bool iplott, bool imc)
{
  doMcComparison = imc;
  Fimin = 0.;
  Fimax = 2. * PI;
  FI0min = 0.;
  FI0max = 2. * PI;
  iplotta = iplott;
  istampa = istamp;
  MINIMUMOUTERHITSPERTRACK = 5;
  nSciTilHits = 0;
  stepD = (Dmax - Dmin) / nbinD;
  stepFi = (Fimax - Fimin) / nbinFi;
  stepR = (Rmax - Rmin) / nbinR;
  stepKAPPA = (KAPPAmax - KAPPAmin) / nbinKAPPA;
  stepFI0 = (FI0max - FI0min) / nbinFI0;
  stepfineKAPPA = 2. * DELTA_KAPPA / nbinKAPPA;
  stepfineFI0 = 2. * DELTA_FI0 / nbinFI0;
  YesSciTil = false;
  Initialization_ClassVariables();
  sprintf(fSttBranch, "STTHit");
}
// -------------------------------------------------------------------------

// -----   Third constructor   ------------------------------------------
PndSttTrackFinderReal::PndSttTrackFinderReal(int istamp, bool iplott, bool imc, bool doSciTil)
{

  doMcComparison = imc;
  Fimin = 0.;
  Fimax = 2. * PI;
  FI0min = 0.;
  FI0max = 2. * PI;
  iplotta = iplott;
  istampa = istamp;
  MINIMUMOUTERHITSPERTRACK = 5;
  nSciTilHits = 0;
  stepD = (Dmax - Dmin) / nbinD;
  stepFi = (Fimax - Fimin) / nbinFi;
  stepR = (Rmax - Rmin) / nbinR;
  stepKAPPA = (KAPPAmax - KAPPAmin) / nbinKAPPA;
  stepFI0 = (FI0max - FI0min) / nbinFI0;
  stepfineKAPPA = 2. * DELTA_KAPPA / nbinKAPPA;
  stepfineFI0 = 2. * DELTA_FI0 / nbinFI0;
  YesSciTil = doSciTil;
  Initialization_ClassVariables();
  sprintf(fSttBranch, "STTHit");
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndSttTrackFinderReal::~PndSttTrackFinderReal() {}
// -------------------------------------------------------------------------

// -------------------- begin PndSttTrackFinderReal::Initialization_ClassVariables

void PndSttTrackFinderReal::Initialization_ClassVariables()
{

  // this is only for initializing the Class Variables.
  char zero;
  size_t len;
  // booleans :
  len = sizeof(InclusionListSciTil);
  memset(InclusionListSciTil, 0, len);
  len = sizeof(TypeConf);
  memset(TypeConf, 0, len);
  //  int  :
  IVOLTE = -1;

  //  Short_t :

  nRdivConformalEffective = 0;
  nSciTilHits = 0;
  nSttSkewhit = 0;
  nMCTracks = 0;
  len = sizeof(infoparal);
  memset(infoparal, 0, len);
  len = sizeof(infoskew);
  memset(infoskew, 0, len);
  len = sizeof(nHitsInMCTrack);
  memset(nHitsInMCTrack, 0, len);
  len = sizeof(nSciTilHitsinTrack);
  memset(nSciTilHitsinTrack, 0, len);
  len = sizeof(nSttSkewhitInMCTrack);
  memset(nSttSkewhitInMCTrack, 0, len);
  len = sizeof(ListSciTilHitsinTrack);
  memset(ListSciTilHitsinTrack, 0, len);
  //  Double_t :
  Fimax = 0.;
  FI0min = 0.;
  FI0max = 0.;
  stepD = 0.;
  stepFi = 0.;
  stepR = 0.;
  stepKAPPA = 0.;
  stepFI0 = 0.;
  stepfineKAPPA = 0.;
  stepfineFI0 = 0.;
  SEMILENGTH_STRAIGHT = 0.;
  ZCENTER_STRAIGHT = 0.;
  len = sizeof(veritaMC);
  memset(veritaMC, 0, len);

  len = sizeof(ALFA);
  memset(ALFA, 0, len);
  len = sizeof(BETA);
  memset(BETA, 0, len);
  len = sizeof(GAMMA);
  memset(GAMMA, 0, len);
  len = sizeof(radiaConf);
  memset(radiaConf, 0, len);
  len = sizeof(CxMC);
  memset(CxMC, 0, len);
  len = sizeof(CyMC);
  memset(CyMC, 0, len);
  len = sizeof(R_MC);
  memset(R_MC, 0, len);
  len = sizeof(posizSciTil);
  memset(posizSciTil, 0, len);
  len = sizeof(S_SciTilHitsinTrack);
  memset(S_SciTilHitsinTrack, 0, len);

  // pointers :

  hdist = nullptr;
  hdistgoodlast = nullptr, hdistbadlast = nullptr;
  HANDLE = nullptr;
  HANDLE2 = nullptr;
  HANDLEXYZ = nullptr;
  PHANDLEX = nullptr;
  PHANDLEY = nullptr;
  PHANDLEZ = nullptr;
  SHANDLEX = nullptr;
  SHANDLEY = nullptr;
  SHANDLEZ = nullptr;

  fMCTrackArray = nullptr;
  fSciTPointArray = nullptr;
  fSciTHitArray = nullptr;

  pMCtr = nullptr;

  fSttHitArray = nullptr;
}

// -------------------- end of PndSttTrackFinderReal::Initialization_ClassVariables

//----------begin of function PndSttTrackFinderReal::WriteHistograms

void PndSttTrackFinderReal::WriteHistograms()
{

  //  TFile *file = FairRootManager::Instance()->GetOutFile();
  //  file->cd();
  //  file->mkdir("PndSttTrackFinderReal");
  //  file->cd("PndSttTrackFinderReal");

  FairSink *sink = FairRootManager::Instance()->GetSink();

  if (sink->GetSinkType() == kFILESINK) {
    TDirectory::TContext restorecwd{};
    TFile *outfile = dynamic_cast<FairRootFileSink *>(sink)->GetRootFile();
    outfile->mkdir("PndSttTrackFinderReal");
    outfile->cd("PndSttTrackFinderReal");

    if (iplotta) {
      outfile->WriteTObject(hdist);
      outfile->WriteTObject(hdistgoodlast);
      outfile->WriteTObject(hdistbadlast);
      delete hdist;
      hdist = nullptr;
      delete hdistgoodlast;
      hdistgoodlast = nullptr;
      delete hdistbadlast;
      hdistbadlast = nullptr;
    }
  }
}

//----------end of function PndSttTrackFinderReal::WriteHistograms

// -----   Public method Init   --------------------------------------------
void PndSttTrackFinderReal::Init()
{

  Short_t i;
  Double_t r1, r2, A, tempRadiaConf[nRdivConformal];

  MINIMUMHITSPERTRACK = 3;

  //  --------------------------- opening files for special purposes

  if (istampa >= 1) {
    //---- apertura file con info su Found tracce su cui si fa Helix fit dopo
    HANDLE2 = fopen("info_da_PndTrackFinderReal.txt", "w");

    //  ---- open filehandle per statistica sugli hits etc.
    HANDLE = fopen("statistichePndTrackFinderReal.txt", "w");
    //  ---------------
    if (istampa >= 3)
      HANDLEXYZ = fopen("infoPndTrackFinderRealXYZ.txt", "w");

    //  ---------------  open file delle info su deltaX, Y, Z  degli hits in comune tra tracce trovate e MC

    PHANDLEX = fopen("deltaParXmio.txt", "w");
    PHANDLEY = fopen("deltaParYmio.txt", "w");
    PHANDLEZ = fopen("deltaParZmio.txt", "w");
    SHANDLEX = fopen("deltaSkewXmio.txt", "w");
    SHANDLEY = fopen("deltaSkewYmio.txt", "w");
    SHANDLEZ = fopen("deltaSkewZmio.txt", "w");

  } //  end of if(istampa >=1)

  // -------------------------

  fHelixHitProduction = true;

  //   IVOLTE=-1;

  // Get and check FairRootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndSttTrackFinderReal::Init: "
         << "RootManager not instantiated, return!" << endl;
    return;
  }

  //    get   the MCTrack  array

  fMCTrackArray = (TClonesArray *)ioman->GetObject("MCTrack");

  //    get   the SciTil point  array
  //  fSciTPointArray = (TClonesArray*) ioman->GetObject("SciTPoint");

  //    get   the SciTil hit  array
  if (YesSciTil) {
    fSciTHitArray = (TClonesArray *)ioman->GetObject("SciTHit");
  } else {
    fSciTHitArray = nullptr;
  }

  //   -------------------------------------------------------------------

  if (iplotta) {
    hdist = new TH1F("hdist", "distance (cm)", 20, 0., 10.);
    hdistgoodlast = new TH1F("hDistanceTrulyInnerLast", "distance (cm)", 20, 0., 10.);
    hdistbadlast = new TH1F("hDistanceNonLastInner", "distance (cm)", 20, 0., 10.);
  }

  //   calculate the boundaries of the Box in Conformal Space, see Gianluigi logbook on pag. 210-211

  radiaConf[0] = 1. / RStrawDetectorMax;
  r1 = RStrawDetectorMin;
  A = (RStrawDetectorMax - r1) / nRdivConformal;
  if (nRdivConformal > 1) {
    for (i = 1; i < nRdivConformal; i++) {
      r2 = r1 + A;
      //        tempRadiaConf[nRdivConformal-i] = 1./r2;
      radiaConf[nRdivConformal - i] = 1. / r2;
      r1 = r2;
    }
  }

  nRdivConformalEffective = nRdivConformal;

  //--------------------  end of method Init   --------------------------------------------
}
// -------------------- starting PndSttTrackFinderReal::GetHitFromCollections

PndSttHit *PndSttTrackFinderReal::GetHitFromCollections(Int_t hitCounter)
{
  PndSttHit *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      retval = (PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter);
      break;
    } else {
      relativeCounter -= size;
    }
  }
  return retval;
}

// -------------------- end of PndSttTrackFinderReal::GetHitFromCollections

// -------------------- starting PndSttTrackFinderReal::GetPointFromCollections

FairMCPoint *PndSttTrackFinderReal::GetPointFromCollections(Int_t hitCounter)
{
  FairMCPoint *retval = nullptr;

  Int_t relativeCounter = hitCounter;

  for (Int_t collectionCounter = 0; collectionCounter < fHitCollectionList.GetEntries(); collectionCounter++) {
    Int_t size = ((TClonesArray *)fHitCollectionList.At(collectionCounter))->GetEntriesFast();

    if (relativeCounter < size) {
      Int_t tmpHit = ((PndSttHit *)((TClonesArray *)fHitCollectionList.At(collectionCounter))->At(relativeCounter))->GetRefIndex();

      retval = (FairMCPoint *)((TClonesArray *)fPointCollectionList.At(collectionCounter))->At(tmpHit);

      break;
    } else {
      relativeCounter -= size;
    }
  }
  return retval;
}

// -------------------- end of PndSttTrackFinderReal::GetPointFromCollections

Int_t PndSttTrackFinderReal::DoFind(TClonesArray *, TClonesArray *)
{
  return 0;
} // CHECK da cancellare

// -----------------   start of method DoFind
Int_t PndSttTrackFinderReal::DoFind(TClonesArray *trackCandArray, TClonesArray *trackArray, TClonesArray *helixHitArray)
{

  bool flaggo, outcome;
  Int_t i, j, iaccept, ii, jj, k, kk, n1, n2, n3, i1, j1, k1, imaxima, jmaxima, iofmax, jofmax, kofmax, index[nmaxHits], integer, Ncirc, nSkew1, nSkew2, n_K, n_FI0, n_R, n_D, n_Fi,
    STATUS, Nremaining, Nremaining2, NumberofMaximaDFiR, NumberofMaximaKFI0, nAssociatedParallelHits;

  Short_t emme, exitstatus, inclination_type, istep,
    //	nSciTilHitsinTrack[MAXTRACKSPEREVENT],
    auxIndex[nmaxHits], OLDinfoparal[nmaxHits];

  Short_t Charge[MAXTRACKSPEREVENT], Status[MAXTRACKSPEREVENT], daTrackFoundaTrackMC[MAXTRACKSPEREVENT], enne[MAXTRACKSPEREVENT][nmaxHits];

  Double_t aaa, ddd, delta, deltabis, deltaZ, mindis, distanza, fi_hit, ap1, ap2, ap3, carica, cross1, cross2, cross3, dummy, lowlimit[MAXTRACKSPEREVENT],
    uplimit[MAXTRACKSPEREVENT], info[nmaxHits][7], WDX, WDY, WDZ, auxRvalues[nmaxHits], inclination[nmaxinclinationversors][3];

  PndSttHit *ListPointer_to_Hit[nmaxHits];

  inclination[0][0] = inclination[0][1] = 0., inclination[0][2] = 1.;
  Int_t Nincl = 1, Ninclinate, iHit;
  Int_t Minclinations[nmaxinclinationversors];

  bool InclusionList[nmaxHits],     // list of parallel hits  already assigned to a track or with multiple hits.
    InclusionListSkew[nmaxHits],    //  list of skew hits with multiple hits
    InclusionListbis[nmaxHits],     //  list of || hits ONLY with multiple hits
    InclusionListSkewbis[nmaxHits], //  list of skew hits ONLY with multiple hits (duplicate of
                                    //		InclusionListSkew)

    //	TypeConf[MAXTRACKSPEREVENT],   //  if TypeConf[]=false --> the track is a line in the Conformal space,
    //   if TypeConf[]=true it is a crf;
    TypeConfSkew[MAXTRACKSPEREVENT],
    GoodTrack[MAXTRACKSPEREVENT]; //  yes = track found passes minimal requirements

  Short_t iParHit, nFicell, nRcell;

  Short_t exphit, iExclude, imc, jexp, mchit, nTracksFoundSoFar, NNN, Nouter, nTotalHits[MAXTRACKSPEREVENT], TemporarynSttSkewhitinTrack,
    BigList[MAXTRACKSPEREVENT][nmaxHitsInTrack], TemporarySkewList[nmaxHits][2], nHitsinTrack[MAXTRACKSPEREVENT], nSttSkewhitinTrack[MAXTRACKSPEREVENT], tempore[nmaxHits],
    ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], nMCParalAlone[MAXTRACKSPEREVENT], nMCSkewAlone[MAXTRACKSPEREVENT],
    MCParalAloneList[MAXTRACKSPEREVENT][nmaxHits], MCSkewAloneList[MAXTRACKSPEREVENT][nmaxHits], nParalCommon[MAXTRACKSPEREVENT], ParalCommonList[MAXTRACKSPEREVENT][nmaxHits],
    nSpuriParinTrack[MAXTRACKSPEREVENT], ParSpuriList[MAXTRACKSPEREVENT][nmaxHits], nSkewCommon[MAXTRACKSPEREVENT], SkewCommonList[MAXTRACKSPEREVENT][nmaxHits],
    nSpuriSkewinTrack[MAXTRACKSPEREVENT], SkewSpuriList[MAXTRACKSPEREVENT][nmaxHits],
    RConformalIndex[nmaxHits],                      //  given a Hit number it gives its radial box number
    FiConformalIndex[nmaxHits],                     //  given a Hit number it gives its azimuthal box number
    nBoxConformal[nRdivConformal][nFidivConformal]; //  first index -> radial divisions, 2nd index -> azimuthal divisions; n. of
                                                    //  hits falling in this cell

  Short_t HitsinBoxConformal[MAXHITSINCELL][nRdivConformal][nFidivConformal];
  //  first index -> radial divisions, 2nd index -> azimuthal divisions;

  Int_t status;

  int iimax, jjmax, ncount;

  Double_t angle, Distance, max1, HoughR, HoughD, HoughFi, HoughKAPPA, HoughFI0, Px, Py, tempR, tempD, tempFi, tempKAPPA, tempFI0, Rlow, Rup, Dlow, Dup, Filow, Fiup, KAPPAlow,
    KAPPAup, FI0low, FI0up, gamma;

  Double_t D, Fi, ptotal, Fi_low_limit[MAXTRACKSPEREVENT], Fi_up_limit[MAXTRACKSPEREVENT], Fi_initial_helix_referenceframe[MAXTRACKSPEREVENT],
    Fi_final_helix_referenceframe[MAXTRACKSPEREVENT], R[MAXTRACKSPEREVENT], Ox[MAXTRACKSPEREVENT], Oy[MAXTRACKSPEREVENT], KAPPA[MAXTRACKSPEREVENT], FI0[MAXTRACKSPEREVENT],
    trajectory_vertex[3], infoparalConformal[nmaxHits][5], S[2 * nmaxHits], tmpErrorZDrift[nmaxHitsInTrack], tmpS[nmaxHitsInTrack], tmpZ[nmaxHitsInTrack],
    tmpZDrift[nmaxHitsInTrack], Z[2 * nmaxHits], temporeS[nmaxHits], temporeZ[nmaxHits], ZDrift[2 * nmaxHits], ZErrorafterTilt[2 * nmaxHits], temporeZDrift[nmaxHits],
    temporeZErrorafterTilt[nmaxHits], Posiz[3], Posiz1[3], Posiz2[3], versor[3],
    //	       U[MAXTRACKSPEREVENT][nmaxHits],
    //	       V[MAXTRACKSPEREVENT][nmaxHits],
    Xpos_for_LHeTrack[nmaxHits], Ypos_for_LHeTrack[nmaxHits], Zpos_for_LHeTrack[nmaxHits], Sfinal[MAXTRACKSPEREVENT][nmaxHits], Zfinal[MAXTRACKSPEREVENT][nmaxHits],
    ZDriftfinal[MAXTRACKSPEREVENT][nmaxHits], ZErrorafterTiltfinal[MAXTRACKSPEREVENT][nmaxHits];

  Float_t RemainingR[nmaxHits], RemainingFi[nmaxHits], RemainingD[nmaxHits], RemainingCX[nmaxHits], RemainingCY[nmaxHits], RemainingKAPPA[nmaxHits], RemainingFI0[nmaxHits];
  bool GoodSkewFit[nmaxHits];

  bool temporflag[8], IFLAG;

  CalculatedCircles Result;
  CalculatedHelix HelixResult;
  //   AssociatedHitsToHelix ResultAssociatedHits ;

  char nomef[100], nome[30], titolo[100];

  //------------------------------------ modifiche Gianluigi, 9-7-08

  IVOLTE++;

  if (istampa > 0)
    cout << "\nEntering in PndSttTrackFinderReal : evt (starting from 0)  n. " << IVOLTE << endl;

  //------------------------------------ fine modifiche Gianluigi, 9-7-08

  // Check pointers
  if (fHitCollectionList.GetEntries() == 0) {
    cout << "-E- PndSttTrackFinderReal::DoFind: "
         << "No hit arrays present, call AddHitCollection() first (at least once), return -1! " << endl;
    return -1;
  }

  if (fPointCollectionList.GetEntries() == 0) {
    cout << "-E- PndSttTrackFinderReal::DoFind: "
         << "No point arrays present, call AddHitCollection() first (at least once), return -1! " << endl;
    return -1;
  }

  if (!trackCandArray) {
    cout << "-E- PndSttTrackFinderReal::DoFind: "
         << "Track array missing, return -1!" << endl;
    return -1;
  }

  //       PndMCTrack*      pMCtr = nullptr;     //  questo e' gia' definito in PndSttTrackFinderReal.h
  nMCTracks = fMCTrackArray->GetEntriesFast(); // num. tracce/evento
  if (nMCTracks == 0) {
    cout << "da PndSttTrackFinderReal  :  N. di MC truth tracks = 0, return -1!\n" << endl;
    return -1;
  } else if (nMCTracks > MAXMCTRACKS) {
    cout << "da PndSttTrackFinderReal  :  N. di MC truth tracks = " << nMCTracks << " and it is > MAXMCTRACKS = " << MAXMCTRACKS << ", setting it equal to MAXMCTRACKS.\n";
    nMCTracks = MAXMCTRACKS;
  }

  if (istampa >= 1)
    cout << "Gianluigi, evento n. " << IVOLTE << ", n. tracce MC = " << nMCTracks << endl;

  // Number of STT hits
  Int_t Nhits = 0;

  for (Int_t hitListCounter = 0; hitListCounter < fHitCollectionList.GetEntries(); hitListCounter++) {
    Nhits += ((TClonesArray *)fHitCollectionList.At(hitListCounter))->GetEntriesFast();
  }

  if (Nhits < MINIMUMHITSPERTRACK) {
    cout << "from PndSttTrackFinderReal :  # Stt hits (|| + //) = " << Nhits << " and it is < MINIMUMHITSPERTRACK = " << MINIMUMHITSPERTRACK << ", return -10!" << endl;
    return -10;
  }

  if (Nhits > nmaxHits) {

    cout << "da PndSttTrackFinderReal  :  N. di Stt Hits = " << Nhits << " and it is > nmaxHits (=" << nmaxHits << "), therefore consider only the first " << nmaxHits << " hits\n"
         << endl;
    Nhits = nmaxHits;
  }

  //------------------------ loading SciTil hits.
  nSciTilHits = 0;
  if (YesSciTil) {
    if (fSciTHitArray != nullptr) {
      // number SciTil hits/event
      nSciTilHits = fSciTHitArray->GetEntriesFast();
      if (istampa > 0)
        cout << "da PndSttTrackFinderReal, event " << IVOLTE << ", " << nSciTilHits << " SciTil hits presenti inizialmente.\n";
    }
    if (nSciTilHits > 0) {
      PndSciTHit *pPndSciTHit;
      TVector3 posiz;

      // the first SciTil hit; this cannot be duplicate hit by definition.
      pPndSciTHit = (PndSciTHit *)fSciTHitArray->At(0);
      posiz = pPndSciTHit->GetPosition();

      if (istampa > 0)
        cout << "da PndSttTrackFinderReal, evt " << IVOLTE << ",  SciTil non purgati, Xpos " << posiz.X() << ", Ypos " << posiz.Y() << ", Zpos " << posiz.Z() << endl;

      posizSciTil[0][0] = posiz.X();
      posizSciTil[0][1] = posiz.Y();
      posizSciTil[0][2] = posiz.Z();
      InclusionListSciTil[0] = true;
      iaccept = 1;
      // the other SciTil hits; purge them if they are duplicate.
      for (j = 1; j < nSciTilHits; j++) {
        pPndSciTHit = (PndSciTHit *)fSciTHitArray->At(j);
        posiz = pPndSciTHit->GetPosition();

        if (istampa > 0)
          cout << "da PndSttTrackFinderReal, evt. " << IVOLTE << ",  SciTil non purgati, Xpos " << posiz.X() << ", Ypos " << posiz.Y() << ", Zpos " << posiz.Z() << endl;

        // purging the duplicate SciTil hits.

        flaggo = true;
        for (k = 0; k < iaccept; k++) {
          if ((fabs(posiz.X() - posizSciTil[k][0]) < 1.e-20) && (fabs(posiz.Y() - posizSciTil[k][1]) < 1.e-20) && (fabs(posiz.Z() - posizSciTil[k][2]) < 1.e-20)) {
            flaggo = false;
            break;
          } // end of if((fabs(posiz.X() - old...
        }   // end of for(k=0; k<iaccept; k++)
        if (flaggo) {
          posizSciTil[iaccept][0] = posiz.X();
          posizSciTil[iaccept][1] = posiz.Y();
          posizSciTil[iaccept][2] = posiz.Z();
          InclusionListSciTil[iaccept] = true;
          iaccept++;
        }

      } // end of for(j=0; j<nSciTilHits; j++)

      nSciTilHits = iaccept;

    } // end of if( nSciTilHits>0){

    //-----------stampe.
    if (istampa > 0) {
      cout << "da PndSttTrackFinderReal, dopo purga di SciTil; evt. " << IVOLTE << ", n. hits = " << nSciTilHits << endl;
      for (j = 0; j < nSciTilHits; j++) {
        cout << "da PndSttTrackFinderReal, evt. " << IVOLTE << ", SciTil Xpos " << posizSciTil[j][0] << ", Ypos " << posizSciTil[j][1] << ", Zpos " << posizSciTil[j][2]
             << ", loro InclusionList [0=false] " << InclusionListSciTil[j] << endl;
      }
    }
    //---------- fine stampe.

  } // end of if(YesSciTil)

  //--------------------------------

  // Initialise control counters
  Int_t nNoTrack = 0;
  Int_t nNoSttPoint = 0;
  Int_t nNoSttHit = 0;

  // Create pointers to hit and SttPoint
  PndSttHit *pMhit = nullptr;
  FairMCPoint *pMCpt = nullptr;
  //   PndSttTrack*     pTrck[MAXTRACKSPEREVENT] ; // this is for the hits found by pattern recognition

  // Declare some variables outside the loops
  Int_t trackIndex = 0; // STTTrack index

  // Create STL map from MCtrack index to number of valid SttHits
  map<Int_t, map<Double_t, Int_t>> hitMap;

  for (j = 0; j < nmaxinclinationversors; j++) {
    Minclinations[j] = 0;
  }

  nSttSkewhit = Ninclinate = 0;

  if (istampa >= 1)
    cout << "Gianluigi : da PndSttTrackFinderReal::DoFind : Nhits=" << Nhits << endl;

  //   generated momenta and starting position of each track

  // Loop over hits
  for (iHit = 0; iHit < Nhits; iHit++) {
    // hit point
    pMhit = GetHitFromCollections(iHit); // <== PndSttHit
    ListPointer_to_Hit[iHit] = pMhit;
    if (!pMhit) {
      cout << "from PndSttTrackFinderReal :  # Stt hits pointer missing, skip this hit!\n";
      continue;
    }

    // MC point
    Int_t ptIndex = pMhit->GetRefIndex();
    if (ptIndex >= 0) {
      pMCpt = GetPointFromCollections(iHit); // <== FairMCPoint
    }

    // tubeID  CHECK added
    Int_t tubeID = pMhit->GetTubeID();
    PndSttTube *tube = (PndSttTube *)fTubeArray->At(tubeID);
    // real hit center of tube coordinates
    TVector3 center = tube->GetPosition();

    // drift radius
    Double_t dradius = pMhit->GetIsochrone();

    // wire direction
    TVector3 wiredirection = tube->GetWireDirection();

    // "real" MC coordinates (in + out)/2.
    //      TVector3 mcpoint;
    //      pMCpt->Position(mcpoint);

    if (wiredirection.Z() >= 0.) {
      WDX = wiredirection.X();
      WDY = wiredirection.Y();
      WDZ = wiredirection.Z();
    } else {
      WDX = -wiredirection.X();
      WDY = -wiredirection.Y();
      WDZ = -wiredirection.Z();
    }

    // stampe di controllo

    info[iHit][0] = tube->GetPosition().X();
    info[iHit][1] = tube->GetPosition().Y();
    info[iHit][2] = tube->GetPosition().Z();
    info[iHit][3] = dradius;
    info[iHit][4] = tube->GetHalfLength();
    if (ptIndex >= 0) {
      info[iHit][6] = pMCpt->GetTrackID();
    } else {
      info[iHit][6] = -20.;
    }

    if (fabs(WDX) < 0.00001 && fabs(WDY) < 0.00001) {
      info[iHit][5] = 1.;
      infoparal[Minclinations[0]] = iHit;
      Minclinations[0]++;
      ZCENTER_STRAIGHT = info[iHit][2];    //    this works because just few lines below there is the
      SEMILENGTH_STRAIGHT = info[iHit][4]; //    requirement that Minclinations[0] > 2 (= at least 3 parallel straws)
    } else {
      flaggo = true;
      for (i = 2; i <= Nincl; i++) {
        if (fabs(WDX - inclination[i - 1][0]) < 0.00001 && fabs(WDY - inclination[i - 1][1]) < 0.00001 && fabs(WDZ - inclination[i - 1][2]) < 0.00001) {
          info[iHit][5] = i;
          infoskew[Ninclinate] = iHit;
          Ninclinate++;
          Minclinations[i - 1]++;
          flaggo = false;
          break;
        }
      }
      if (flaggo) {
        Nincl++;
        inclination[Nincl - 1][0] = (Double_t)WDX;
        inclination[Nincl - 1][1] = (Double_t)WDY;
        inclination[Nincl - 1][2] = (Double_t)WDZ;
        info[iHit][5] = Nincl;
        infoskew[Ninclinate] = iHit;
        Ninclinate++;
        Minclinations[Nincl - 1]++;
      } // end if(flaggo)
    }

    nSttSkewhit = Ninclinate;

    //   calcoli validi solo per il MC   -----------------------------

    if (ptIndex >= 0) {
      veritaMC[iHit][0] = pMCpt->GetX();
      veritaMC[iHit][1] = pMCpt->GetY();
      veritaMC[iHit][2] = pMCpt->GetZ();
    }

    //	FromHitToMCTrack[iHit] = (Short_t) ( info[iHit][6] + 0.001 );

    //--------------- inizio stampaggi,  stampe di controllo
    if (istampa >= 2 && IVOLTE <= nmassimo) {
      cout << "iHit " << iHit << endl;
      if (ptIndex < 0) {
        cout << "from PndSttTrackFinderReal...this hit must be noise (RefIndex = " << ptIndex << " )\n\t\t\tnot associate to any MC Track\n";
        //         cout<<"             this hit belongs to MC track n. "<<pMCpt->GetTrackID()<<endl;
      } else {
        cout << "             MC point X, Y, Z space position " << veritaMC[iHit][0] << " " << veritaMC[iHit][1] << " " << veritaMC[iHit][2] << endl;
      }
      cout << "             hit wire pos. in middle " << tube->GetPosition().X() << " " << tube->GetPosition().Y() << " " << tube->GetPosition().Z()
           << "; R = " << sqrt(tube->GetPosition().X() * tube->GetPosition().X() + tube->GetPosition().Y() * tube->GetPosition().Y()) << endl;
      cout << "             wire direction, X, Y, Z (Z direction set always positive)" << WDX << "  " << WDY << "  " << WDZ << endl;
    } //  end of   if(istampa >=

    //--------  fine stampaggi

  } //   end  of  for (Int_t iHit = 0;

  //--------------- inizio stampaggi
  //  if (istampa >= 2  && IVOLTE<= nmassimo) {
  if (istampa >= 2) {
    cout << "Gianluigi : da PndSttTrackFinderReal::DoFind : Nhits totali =" << Nhits << ",  n Hits ||  = " << Minclinations[0] << ",  n Hits  skew = " << nSttSkewhit << endl;
  } //  end of   if(istampa >=
    //--------  fine stampaggi

  //   ordering the parallel hits by INCREASING CONFORMAL RADIUS or equivalently, decreasing spatial radius.

  for (j = 0; j < Minclinations[0]; j++) {
    auxIndex[j] = j;
    auxRvalues[j] = info[infoparal[j]][0] * info[infoparal[j]][0] + info[infoparal[j]][1] * info[infoparal[j]][1];
    OLDinfoparal[j] = infoparal[j];
  }

  Merge_Sort(Minclinations[0], auxRvalues, auxIndex);
  for (j = 0; j < Minclinations[0]; j++) {
    infoparal[Minclinations[0] - 1 - j] = OLDinfoparal[auxIndex[j]];
  }

  //----------------------------

  if (Minclinations[0] < MINIMUMHITSPERTRACK) {
    cout << "from PndSttTrackFinderReal :  # Stt || hits = " << Minclinations[0] << " and it is < MINIMUMHITSPERTRACK = " << MINIMUMHITSPERTRACK << ", return 0!" << endl;
    return 0;
  }

  for (i = 0; i < Minclinations[0]; i++) {
    InclusionList[infoparal[i]] = true;
    InclusionListbis[infoparal[i]] = true;
  }
  for (i = 0; i < nSttSkewhit; i++) {
    InclusionListSkew[infoskew[i]] = true;
    InclusionListSkewbis[infoskew[i]] = true;
  }

  //-----------------------------------   Inclusion of straws with multiple hits

  //   first the parallel straws
  for (i = 0; i < Nhits - 1; i++) {
    if (info[i][5] == 1.) {
      if (InclusionList[i]) {
        for (j = i + 1; j < Nhits; j++) {
          if (InclusionList[j] && info[j][5] == 1. && fabs(info[i][0] - info[j][0]) < 1.e-20 && fabs(info[i][1] - info[j][1]) < 1.e-20) {
            InclusionList[j] = false;
            InclusionListbis[j] = false;
          }
        }    //  end of  for(j=i+1; j< Nhits;; j++)
      }      //   end of if( InclusionList[ i ] )
    } else { //  continuation of  if( info[i][5]==1.)

      if (InclusionListSkew[i]) {
        for (j = i + 1; j < Nhits; j++) {
          if (InclusionListSkew[j] && info[j][5] != 1. && fabs(info[i][0] - info[j][0]) < 1.e-20 && fabs(info[i][1] - info[j][1]) < 1.e-20) {
            InclusionListSkew[j] = false;
            InclusionListSkewbis[j] = false;
          }
        } //  end of  for(j=i+1; j< Nhits;; j++)
      }   //   end of if( InclusionList[ i ] )

    } //	//  end of  if( info[i][5]==1.)

  } //   end of for(i=0; i< Nhits-1; i++)

  //-----------------------------------  end of Inclusion of straws with multiple hits

  trajectory_vertex[0] = trajectory_vertex[1] = trajectory_vertex[2] = 0.;

  PndSttFromXYtoConformal(trajectory_vertex, info, Minclinations[0], infoparalConformal, &status);

  PndSttBoxConformalFilling(InclusionList, infoparalConformal, Minclinations[0], nBoxConformal, HitsinBoxConformal, RConformalIndex, FiConformalIndex);

  //     start the track finding procedure

  Double_t U[MAXTRACKSPEREVENT][Minclinations[0]], V[MAXTRACKSPEREVENT][Minclinations[0]];
  nTracksFoundSoFar = 0; // # tracks found

  //----- loop over the SciTil hits first;
  if (YesSciTil) {

    for (i = 0; i < nSciTilHits; i++) {
      if (nTracksFoundSoFar > MAXTRACKSPEREVENT)
        continue;

      nRcell = -1; // because SciTil hit is outside of the Stt system.
      Fi = atan2(posizSciTil[i][1], posizSciTil[i][0]);
      if (Fi < 0.)
        Fi += 2. * PI;
      nFicell = (Short_t)(0.5 * nFidivConformal * Fi / PI);
      if (nFicell > nFidivConformal) {
        nFicell = nFidivConformal;
      } else if (nFicell < 0) {
        nFicell = 0;
      }
      outcome = FindTrackInXYProjection(-i - 1, // seed hit; it is negative for SciTil Hits.
                                        nRcell, nFicell, Minclinations, info, InclusionList, RConformalIndex, FiConformalIndex, nBoxConformal, HitsinBoxConformal,
                                        nTracksFoundSoFar, nHitsinTrack, ListHitsinTrack, trajectory_vertex, infoparalConformal, posizSciTil[i][0], posizSciTil[i][1],
                                        &S_SciTilHitsinTrack[nTracksFoundSoFar][0], Ox, Oy, R, Fi_low_limit, Fi_up_limit, Fi_initial_helix_referenceframe,
                                        Fi_final_helix_referenceframe, Charge, &U[nTracksFoundSoFar][0], &V[nTracksFoundSoFar][0]);
      if (!outcome) {
        continue;
      }
      for (j = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
        InclusionList[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]] = false;
      }

      //---------stampe.
      if (istampa > 0) {
        cout << "PndTrackFinderReal, evt. " << IVOLTE << ", cand. " << nTracksFoundSoFar << ", partendo da SciTil, traccia n. " << nTracksFoundSoFar << ",n SciTilhitsintrack  "
             << nSciTilHitsinTrack[nTracksFoundSoFar] << " e loro stampa :\n";
        for (j = 0; j < nSciTilHitsinTrack[nTracksFoundSoFar]; j++) {
          cout << "\t hit || n. " << ListSciTilHitsinTrack[nTracksFoundSoFar][j] << ", S " << S_SciTilHitsinTrack[nTracksFoundSoFar][j] << endl;
        }
        cout << "\t\tora stampa hit || :\n";
        for (j = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
          cout << "\t hit || n. " << infoparal[ListHitsinTrack[nTracksFoundSoFar][j]] << endl;
        }
      }
      //-------fine stampe.

      nTracksFoundSoFar++;

      if (nTracksFoundSoFar >= MAXTRACKSPEREVENT) {
        cout << "from PndSttTrackFinderReal :  # n. Tracks found so far = " << nTracksFoundSoFar << " and it is >= MAXTRACKSPEREVENT ( = " << MAXTRACKSPEREVENT
             << "; rejecting this event and returning -15!\n";
        return -15;
      }

    } // end of  for(i=0; i<nSciTilHits ; i++)

  } // end of  if(YesSciTil)

  //----- end loop over the SciTil hits.

  //----- loop over the parallel hits

  //   begins the first iteration with more severe cuts on the # hits in track candidate

  for (iParHit = 0; iParHit < Minclinations[0] + 1 - MINIMUMHITSPERTRACK; iParHit++) {
    if (nTracksFoundSoFar > MAXTRACKSPEREVENT)
      continue;
    if (!InclusionList[infoparal[iParHit]])
      continue;

    nRcell = RConformalIndex[infoparal[iParHit]];
    nFicell = FiConformalIndex[infoparal[iParHit]];

    outcome = FindTrackInXYProjection(iParHit, // seed hit; it is positive for STT Hits.
                                      nRcell, nFicell, Minclinations, info, InclusionList, RConformalIndex, FiConformalIndex, nBoxConformal, HitsinBoxConformal, nTracksFoundSoFar,
                                      nHitsinTrack, ListHitsinTrack, trajectory_vertex, infoparalConformal,
                                      1., // dummy value, there is no SciTil info in this case;
                                      1., // dummy value, there is no SciTil info in this case
                                      &dummy, Ox, Oy, R, Fi_low_limit, Fi_up_limit, Fi_initial_helix_referenceframe, Fi_final_helix_referenceframe, Charge,
                                      &U[nTracksFoundSoFar][0], &V[nTracksFoundSoFar][0]);

    if (!outcome)
      continue;

    // --------  here the track and its hits were found, filling the Inclusion list

    for (j = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
      InclusionList[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]] = false;
    }
    //---------stampe.
    if (istampa > 0) {
      cout << "PndTrackFinderReal, evt. " << IVOLTE << ", cand. " << nTracksFoundSoFar << ", partendo da Stt || hits, traccia n. " << nTracksFoundSoFar << ",n SciTilhitsintrack  "
           << nSciTilHitsinTrack[nTracksFoundSoFar] << " e loro stampa :\n";
      for (j = 0; j < nSciTilHitsinTrack[nTracksFoundSoFar]; j++) {
        cout << "\t hit || n. " << ListSciTilHitsinTrack[nTracksFoundSoFar][j] << ", S " << S_SciTilHitsinTrack[nTracksFoundSoFar][j] << endl;
      }
      cout << "\t\tora stampa hit || :\n";
      for (j = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
        cout << "\t hit || n. " << infoparal[ListHitsinTrack[nTracksFoundSoFar][j]] << endl;
      }
    }
    //-------fine stampe.

    nTracksFoundSoFar++;

    if (nTracksFoundSoFar >= MAXTRACKSPEREVENT) {
      cout << "from PndSttTrackFinderReal :  # n. Tracks found so far = " << nTracksFoundSoFar << " and it is >= MAXTRACKSPEREVENT ( = " << MAXTRACKSPEREVENT
           << "; rejecting this event and returning -15!\n";
      return -15;
    }

  } // end  of   for(iParHit=0; iParHit<Minclinations[0]+1-MINIMUMHITSPERTRACK; iParHit++)

  //------------ stampe.
  if (istampa >= 2) {
    for (j = 0; j < nTracksFoundSoFar; j++) {
      for (int jc = 0; jc < nSciTilHitsinTrack[j]; jc++) {
        cout << "from PndTrackFinderReal evt " << IVOLTE << ", cand " << jc << ", SciTil hit n. " << ListSciTilHitsinTrack[j][jc] << ", Z "
             << posizSciTil[ListSciTilHitsinTrack[j][jc]][2] << ", S  " << S_SciTilHitsinTrack[j][jc] << endl;
      }
    }
  }
  //---------------fine stampe.

  //-----------------------
  //-----------------------
  //-----------------------
  //-----------------------  doing the fit with the skew hits for each XY plane track found
  //-----------------------
  //-----------------------
  //-----------------------

  bool keepit[nTracksFoundSoFar];

  for (i = 0; i < nTracksFoundSoFar; i++) {

    keepit[i] = true; // initialization.

    GoodSkewFit[i] = false; //  flag indicating if the skew sector info has completed the parameter info;
                            //  a priori this is set false.

    nSttSkewhitinTrack[i] = 0;

    if (Fi_low_limit[i] < -99998.)
      continue; // this is when in XY the Helix circle is not in the
                // STT region; this in principle should never happen.

    //-----  finding the skew hits intersecting this XY trajectory circle

    TemporarynSttSkewhitinTrack = AssociateSkewHitsToXYTrack(InclusionListSkew, // excluded only if it is a double hit
                                                             Ox[i],             //  input : X of center of XY plane circle
                                                             Oy[i],             //  input : Y of center of XY plane circle
                                                             R[i],              //  input : Radius of XY plane circle
                                                             info, inclination,
                                                             Fi_low_limit[i], // in the Helix XY frame, taking into account the minimum/maximum
                                                             Fi_up_limit[i],  // radius of the STT  detector.
                                                             Charge[i], Fi_initial_helix_referenceframe[i], Fi_final_helix_referenceframe[i],
                                                             TemporarySkewList, // output,  list of selected skew hits (in skew numbering)
                                                             S,                 //  output,  S coordinate of selected Skew hit
                                                             Z,                 //  output,  Z coordinate of center wire of selected Skew hit
                                                             ZDrift,            //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                                                             ZErrorafterTilt    //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
    );

    nSttSkewhitinTrack[i] = TemporarynSttSkewhitinTrack; // it can be also zero!
                                                         // limit the total # hits to nmaxHitsInTrack
    if (nSttSkewhitinTrack[i] + nHitsinTrack[i] > nmaxHitsInTrack) {
      if (nmaxHitsInTrack - nHitsinTrack[i] > 0)
        nSttSkewhitinTrack[i] = nmaxHitsInTrack - nHitsinTrack[i];
      else
        nSttSkewhitinTrack[i] = 0;
    }

    // here there are up to 2 SciTil hits in track.
    if (nSciTilHitsinTrack[i] > 0) {
      for (j = 0; j < nSciTilHitsinTrack[i]; j++) {
        tmpS[j] = S_SciTilHitsinTrack[i][j]; // this is already between 0 and 2PI.
        tmpZ[j] = posizSciTil[ListSciTilHitsinTrack[i][j]][2],
        tmpZDrift[j] = -1., // conventional, to signal that this is not a STT hit.
                            // error is intentionally overestimated for later use in SZ fit.
                            //	tmpErrorZDrift[j] = DIMENSIONSCITIL/sqrt(12.);
          tmpErrorZDrift[j] = DIMENSIONSCITIL / 2.;
      }
    }
    for (j = 0; j < nSttSkewhitinTrack[i]; j++) {
      ListSkewHitsinTrack[i][j] = TemporarySkewList[j][0];
      tmpS[j + nSciTilHitsinTrack[i]] = S[j], tmpZ[j + nSciTilHitsinTrack[i]] = Z[j], tmpZDrift[j + nSciTilHitsinTrack[i]] = ZDrift[j],
               // error is intentionally overestimated for later use in SZ fit.
        tmpErrorZDrift[j + nSciTilHitsinTrack[i]] = 3. * STRAWRADIUS;
    }

    if (nSttSkewhitinTrack[i] + nSciTilHitsinTrack[i] < 2) {
      for (j = 0; j < nSttSkewhitinTrack[i]; j++) {
        Sfinal[i][infoskew[ListSkewHitsinTrack[i][j]]] = S[j];
      }
      continue;
    }

    //  finding if there are discontinuity at 0 for fi value of the Skew Straws Hit.
    //  In case of discontinuity at 0, add 2*PI to fi of those hits with fi in the 1st quadrant.
    //  This is necessary because the discontinuities would make the fit
    //  in the SZ plane fail.
    //  In this discontinuity fixing, the value FI0 of the vertex (0,0) is also included.
    //  If there is discontinuity fixing, the values of S[i] AND POSSIBLY
    //  Fi_initial_helix_referenceframe[i] might be modified (+2.*PI) from  now on.

    FixDiscontinuitiesFiangleinSZplane(nSttSkewhitinTrack[i], S, &Fi_initial_helix_referenceframe[i], Charge[i]);

    Status[i] = FitSZspace(nSttSkewhitinTrack[i] + nSciTilHitsinTrack[i], tmpS, tmpZ,
                           tmpZDrift, // drift radius onto the SZ projection; if negative --> SciTil hit.
                           tmpErrorZDrift,
                           Fi_initial_helix_referenceframe[i], //   this is an input;
                           NHITSINFIT,                         // maximum n. hits in fit.
                           &KAPPA[i]);
    //    Status[i] is negative (-99) when m = 0. and (-100) as result when the fit with glpk
    //		failed.

    if (Status[i] < 0 || fabs(KAPPA[i]) > 1.e10) {
      //  necessary to load here the Sfinal  vector anyway.
      for (j = 0; j < nSttSkewhitinTrack[i]; j++) {
        Sfinal[i][infoskew[ListSkewHitsinTrack[i][j]]] = S[j];
      }
      continue;
    }

    FI0[i] = Fi_initial_helix_referenceframe[i]; //  therefore, FI0[i] has an extra +2*PI or -2*PI added
                                                 // in case of tracks
                                                 //  crossing the X axis

    //-----  finding a better association of the skew hits intersecting this XY trajectory circle

    //    this means discarding those skew hits that are too far away from the fitted straight line
    //    found in the SZ fit.

    NNN = AssociateBetterAfterFitSkewHitsToXYTrack(nSttSkewhitinTrack[i],
                                                   TemporarySkewList,      // input, list of selected skew hits (in skew numbering)
                                                   S,                      //  input,  S coordinate of selected Skew hit
                                                   Z,                      //  input,  Z coordinate of center wire of selected Skew hit
                                                   ZDrift,                 //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
                                                   ZErrorafterTilt,        //  input,  Radius taking into account the tilt,
                                                                           // IN Z DIRECTION only, of selected Skew hit
                                                   KAPPA[i],               // input, KAPPA result of fit
                                                   FI0[i],                 // input, FI0 result of fit,
                                                   tempore,                //  output, associated skew hits
                                                   temporeS,               //  output, associated skew hit  S
                                                   temporeZ,               //  output, associated skew hits Zcoordinate of center wire
                                                   temporeZDrift,          //  output, associated skew hit Z drift
                                                   temporeZErrorafterTilt, //  output, associated skew hits Z error after tilt
                                                   &STATUS                 // output
    );
    //    out of this function  STATUS  is zero signals only that KAPPA  is zero.

    if (STATUS >= 0) {

      nSttSkewhitinTrack[i] = NNN;
      // limit the total # of hits in track to nmaxHitsInTrack.
      if (nSttSkewhitinTrack[i] + nHitsinTrack[i] > nmaxHitsInTrack) {
        if (nmaxHitsInTrack - nHitsinTrack[i] > 0)
          nSttSkewhitinTrack[i] = nmaxHitsInTrack - nHitsinTrack[i];
        else
          nSttSkewhitinTrack[i] = 0;
      }
      for (j = 0; j < nSttSkewhitinTrack[i]; j++) {
        ListSkewHitsinTrack[i][j] = tempore[j];
        Sfinal[i][infoskew[ListSkewHitsinTrack[i][j]]] = temporeS[j];
        S[j] = temporeS[j];
        Z[j] = temporeZ[j];
        ZDrift[j] = temporeZDrift[j];
        ZErrorafterTilt[j] = temporeZErrorafterTilt[j];
      }

      if (NNN < 2)
        continue;

      GoodSkewFit[i] = true;

    } else { //   continuation of   if( STATUS >=0 )

      nSttSkewhitinTrack[i] = TemporarynSttSkewhitinTrack;
      // limit the total # of hits in track to nmaxHitsInTrack.
      if (nSttSkewhitinTrack[i] + nHitsinTrack[i] > nmaxHitsInTrack) {
        if (nmaxHitsInTrack - nHitsinTrack[i] > 0)
          nSttSkewhitinTrack[i] = nmaxHitsInTrack - nHitsinTrack[i];
        else
          nSttSkewhitinTrack[i] = 0;
      }
      for (j = 0; j < nSttSkewhitinTrack[i]; j++) {
        ListSkewHitsinTrack[i][j] = TemporarySkewList[j][0];
      }
      GoodSkewFit[i] = true;
    } //  end of     if( STATUS >=0 )

    if (nHitsinTrack[i] + nSttSkewhitinTrack[i] < MINIMUMHITSPERTRACK || nHitsinTrack[i] + nSttSkewhitinTrack[i] > nmaxHitsInTrack) {
      keepit[i] = false;
      continue;
    }

    // ---------    numbering according to the ORIGINAL hit number
    for (i1 = 0; i1 < nSttSkewhitinTrack[i]; i1++) {
      Sfinal[i][infoskew[ListSkewHitsinTrack[i][i1]]] = S[i1];
      Zfinal[i][infoskew[ListSkewHitsinTrack[i][i1]]] = Z[i1];
      ZDriftfinal[i][infoskew[ListSkewHitsinTrack[i][i1]]] = ZDrift[i1];
      ZErrorafterTiltfinal[i][infoskew[ListSkewHitsinTrack[i][i1]]] = ZErrorafterTilt[i1];
    }
    // ---------

    //     -------------------------------------------------------------

  } //  end of     for(i=0; i<nTracksFoundSoFar;i++)
    //------------------------------------------------------  end of skew hits section

  // now the ordering the parallel and skew hits.

  for (i = 0; i < nTracksFoundSoFar; i++) {
    nTotalHits[i] = nHitsinTrack[i] + nSttSkewhitinTrack[i];

    // the BigList array must be loaded also when there are no Skew hits.
    PndSttOrderingSkewandParallel(infoparal, infoskew, Ox[i], Oy[i], R[i],
                                  nSttSkewhitinTrack[i],      // input
                                  &ListSkewHitsinTrack[i][0], // input, but this gets ordered
                                  &Sfinal[i][0],              // input from Skew Straws
                                  Charge[i],                  // input
                                  nHitsinTrack[i],            // input, # parallel Hits in the current track
                                  &ListHitsinTrack[i][0],     // this was already ordered
                                  &U[i][0],                   // U conformal parallel hits; input, this was already ordered
                                  &V[i][0],                   // V conformal parallel hits; input, this was already ordered
                                  &BigList[i][0]              // this is the final ordered Parallel+Skew list; already
                                                              // in NATIVE hit number.
    );

    //	if ( nSttSkewhitinTrack[i]==0) continue;

    if (istampa >= 2) {
      cout << "Evt. n. " << IVOLTE << ", Traccia n. " << i << ",  list dei " << nTotalHits[i] << "   hits come stanno in BigList (original notation) :\n";

      for (int ig = 0; ig < nTotalHits[i]; ig++) {
        cout << "          hit n.  " << BigList[i][ig] << endl;
      }
      cout << "e ora gli skew hits ordinati (original notation) :\n";
      for (int ig = 0; ig < nSttSkewhitinTrack[i]; ig++) {
        cout << "          hit n.  " << infoskew[ListSkewHitsinTrack[i][ig]] << endl;
      }
    }

  } //  end of     for(i=0; i<nTracksFoundSoFar;i++)

  //-----------------------------------

  for (i = 0; i < nTracksFoundSoFar; i++) {

    //--------stampe.
    if (istampa >= 2) {
      cout << "\tPndSttTrackFinderReal, fine di procedura, IVOLTE = " << IVOLTE << ",  traccia n. " << i << ", lista degli hit || :" << endl;
      for (int ic = 0; ic < nHitsinTrack[i]; ic++) {
        cout << "\thit || (nativo) n. " << infoparal[ListHitsinTrack[i][ic]] << endl;
      }
      cout << "\tlista degli Stt // hits :" << endl;
      for (int ic = 0; ic < nSciTilHitsinTrack[i]; ic++) {
        cout << "\thit // Stt (nativo) n. " << infoskew[ListSkewHitsinTrack[i][ic]] << endl;
      }

      cout << "\tlista degli hit SciTil :" << endl;
      for (int ic = 0; ic < nSciTilHitsinTrack[i]; ic++) {
        cout << "\thit SciTil n. " << ListSciTilHitsinTrack[i][ic] << ", Z " << posizSciTil[ListSciTilHitsinTrack[i][ic]][2] << ", S " << S_SciTilHitsinTrack[i][ic] << endl;
      }
    }
    //-------------- fine stampe.

  } //  end of  for(i=0; i<nTracksFoundSoFar;i++)

  //--------------------  inizio della sezione sul confronto tra MC truth e tracce trovate

  //    associate the tracks found with Pattern Recognition to the MC tracks

  for (int ig = 0; ig < nTracksFoundSoFar; ig++) {
    daTrackFoundaTrackMC[ig] = -1; // inizialization for protection agains possible
                                   // weird values; this is necessary because later
                                   // this is used in the PndTrackCand array.
  }

  if (nMCTracks > 0 && nTracksFoundSoFar > 0 && doMcComparison) {

    AssociateFoundTrackstoMCbis(keepit, info, nTracksFoundSoFar, nHitsinTrack, ListHitsinTrack, nSttSkewhitinTrack, ListSkewHitsinTrack, daTrackFoundaTrackMC);

    for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
      if (!keepit[jexp])
        continue;
      nParalCommon[jexp] = 0;
      nSkewCommon[jexp] = 0;
      nMCParalAlone[jexp] = 0;
      nMCSkewAlone[jexp] = 0;
      nSpuriParinTrack[jexp] = 0;
      nSpuriSkewinTrack[jexp] = 0;

      // --- parallel hits

      for (exphit = 0; exphit < nHitsinTrack[jexp]; exphit++) {
        iHit = infoparal[ListHitsinTrack[jexp][exphit]];
        enne[jexp][exphit] = (Short_t)(info[iHit][6] + 0.01);
        if (enne[jexp][exphit] == daTrackFoundaTrackMC[jexp]) {
          ParalCommonList[jexp][nParalCommon[jexp]] = iHit;
          nParalCommon[jexp]++;
        } else {
          ParSpuriList[jexp][nSpuriParinTrack[jexp]] = iHit;
          nSpuriParinTrack[jexp]++;
        }
      }
      //--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
      for (i = 0; i < Minclinations[0]; i++) {
        if (!InclusionListbis[infoparal[i]])
          continue;
        emme = (Short_t)(info[infoparal[i]][6] + 0.01);
        if (emme == daTrackFoundaTrackMC[jexp]) {
          flaggo = true;
          for (exphit = 0; exphit < nHitsinTrack[jexp]; exphit++) {
            if (ListHitsinTrack[jexp][exphit] == i) {
              flaggo = false;
              break;
            }
          }
          if (flaggo) {
            MCParalAloneList[jexp][nMCParalAlone[jexp]] = infoparal[i];
            nMCParalAlone[jexp]++;
          }
        }
      } //  end of  for(i=0; i<Minclinations[0]; i++)

      nHitsInMCTrack[jexp] = nMCParalAlone[jexp] + nParalCommon[jexp];
      // --- skew hits

      for (exphit = 0; exphit < nSttSkewhitinTrack[jexp]; exphit++) {
        iHit = infoskew[ListSkewHitsinTrack[jexp][exphit]];
        enne[jexp][exphit] = (Short_t)(info[iHit][6] + 0.01);
        if (enne[jexp][exphit] == daTrackFoundaTrackMC[jexp]) {
          SkewCommonList[jexp][nSkewCommon[jexp]] = iHit;
          nSkewCommon[jexp]++;
        } else {
          SkewSpuriList[jexp][nSpuriSkewinTrack[jexp]] = iHit;
          nSpuriSkewinTrack[jexp]++;
        }
      }

      //--- ricerca degli hits non mecciati, della traccia MC associata a questa traccia trovata.
      for (i = 0; i < nSttSkewhit; i++) {
        if (!InclusionListSkewbis[infoskew[i]])
          continue;
        emme = (Short_t)(info[infoskew[i]][6] + 0.01);
        if (emme == daTrackFoundaTrackMC[jexp]) {
          flaggo = true;
          for (exphit = 0; exphit < nSttSkewhitinTrack[jexp]; exphit++) {
            if (i == ListSkewHitsinTrack[jexp][exphit]) {
              flaggo = false;
              break;
            }
          }
          if (flaggo) {
            MCSkewAloneList[jexp][nMCSkewAlone[jexp]] = infoskew[i];
            nMCSkewAlone[jexp]++;
          }
        }
      }

      nSttSkewhitInMCTrack[jexp] = nMCSkewAlone[jexp] + nSkewCommon[jexp];

    } //   end of  for(jexp=0; jexp<nTracksFoundSoFar;jexp++)

    //---------- conteggio delle tracce MC accettabili!!
    int citata;
    int nMCTracksaccettabili = 0;
    int ListaMCTracksaccettabili[nMCTracks];
    for (i = 0; i < nMCTracks; i++) {
      citata = 0;
      pMCtr = (PndMCTrack *)fMCTrackArray->At(i);
      if (!pMCtr)
        continue;
      Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Pxx, Pyy;
      Int_t icode;
      icode = pMCtr->GetPdgCode();       //   PDG code of track
      Oxx = pMCtr->GetStartVertex().X(); //   X of starting point track
      Oyy = pMCtr->GetStartVertex().Y(); //   Y of starting point track
      Pxx = pMCtr->GetMomentum().X();
      Pyy = pMCtr->GetMomentum().Y();
      aaa = sqrt(Pxx * Pxx + Pyy * Pyy);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla

      if (istampa > 2) {
        TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
        TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
        if (icode > 1000000000)
          carica = 1.;
        else
          carica = fParticle->Charge() / 3.; //   charge of track
        if (fabs(carica) < 1.e-5)
          continue;
        Cx = Oxx + Pyy * 1000. / (BFIELD * CVEL * carica);
        Cy = Oyy - Pxx * 1000. / (BFIELD * CVEL * carica);
        cout << "da PndSttTrackFinderReal, evento (cominciando da 0) n. " << IVOLTE << ",  traccia MC n. " << i << ",  R MC = " << Rr << ", Centro X = " << Cx
             << ", Centro Y = " << Cy << endl;
      }

      for (int ic = 0; ic < Nhits; ic++) {
        if (((int)(info[ic][6] + 0.1)) == i && info[ic][5] < 2.) {
          citata++;
        }
      }
      if (citata > 2 && fabs(Oxx) < 1. && fabs(Oyy) < 1.) {
        ListaMCTracksaccettabili[nMCTracksaccettabili] = i;
        nMCTracksaccettabili++;
      }
    }

    if (istampa >= 1) {
      cout << "da PndSttTrackFinderReal, MC comparison; evt. " << IVOLTE << ", nMCTracks " << nMCTracks << ", n. MC tracce accettabili " << nMCTracksaccettabili
           << " e loro lista :\n";
      for (int g = 0; g < nMCTracksaccettabili; g++) {
        cout << "\ttraccia MC n. " << ListaMCTracksaccettabili[g] << endl;
      }
      cout << "Total track trovate " << nTracksFoundSoFar << endl;
    }

    //----------- fine conteggio delle tracce MC accettabili

    //----------------
    if (istampa >= 1) {
      fprintf(HANDLE, "\n Evento %d  NTotaleTracceMC %d ------\n", IVOLTE, nMCTracksaccettabili);
      int ibene = 0;
      if (nMCTracksaccettabili > 0) {
        for (ii = 0; ii < nTracksFoundSoFar; ii++) {
          // il controllo su keepit e' gia' incluso in daTrackFoundaTrackMC
          // (che in tal caso e' -1).
          // GoodSkewFit invece non c'entra, perche' daTrackFoundaTrackMC
          // e' caricata in funzione degli hit paralleli solamente.
          for (i = 0; i < nMCTracksaccettabili; i++) {
            if (daTrackFoundaTrackMC[ii] == ListaMCTracksaccettabili[i]) {
              ibene++;
            }
          }
        }
      }
      if (ibene > 0)
        fprintf(HANDLE, "\tn. volte almeno 1 traccia MC accettabile e' ricostruita %d\n", ibene);
    } // end of  if(istampa>=1 )
    //----------------

    for (ii = 0; ii < nTracksFoundSoFar && istampa >= 1; ii++) {
      if (!keepit[ii])
        continue;
      fprintf(HANDLE, "----------------------------------------------------------\n");
      i = daTrackFoundaTrackMC[ii];

      if (i < 0) {
        fprintf(HANDLE, "   No TracciaMC associated to found track n. %d in pattern recognition, with %d Hits ||, %d skew hits, %f Radius \n ", ii, nHitsinTrack[ii],
                nSttSkewhitinTrack[ii], R[ii]);
        continue;
      }
      if (!(GoodSkewFit[ii])) {
        fprintf(HANDLE, "       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' in Z-S KAPPA e' risultata || asse S\n", i, ii);
        continue;
      }
      if (fabs(KAPPA[ii]) < 1.e-10) {
        fprintf(HANDLE, "       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' KAPPA troppo piccolo; KAPPA = %g\n", i, ii, KAPPA[ii]);
        continue;
        //   } else if (fabs(KAPPA[ii])>1.e-10 ){
        //    fprintf(HANDLE,
        //"       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' KAPPA troppo grande; KAPPA = %g\n",
        //          i,ii,KAPPA[ii]);
        //           continue;
      }
      Double_t dista = sqrt(Ox[ii] * Ox[ii] + Oy[ii] * Oy[ii]);
      if (fabs(dista) < 1.e-20) {
        fprintf(HANDLE, "       TracciaMC %d; sua FoundTrack associata (n. %d) NONsoddisfaRequisitiMinimi perche' centro Helix Cilinder trovato dista solo %g da (0,0)\n", i, ii,
                dista);
        continue;
      }
      pMCtr = (PndMCTrack *)fMCTrackArray->At(i);
      if (!pMCtr) {
        fprintf(HANDLE, "       MC track n. %d doesn't have pointer to MC Track TClones Array\n", i);
        continue;
      }

      //   controllo che la traccia associata MC sia una delle tracce MC 'ragionevoli'.

      flaggo = true;
      for (int g = 0; g < nMCTracksaccettabili; g++) {
        if (i == ListaMCTracksaccettabili[g]) {
          flaggo = false;
          break;
        }
      }
      if (flaggo)
        continue;

      fprintf(HANDLE, "       TracciaMC %d ParHitsMC %d ParMecc %d ParMeccSpuri %d SkewHitsMC %d  SkewMecc %d SkewMeccSpuri %d\n", i, nHitsInMCTrack[ii], nParalCommon[ii],
              nSpuriParinTrack[ii], nSttSkewhitInMCTrack[ii], nSkewCommon[ii], nSpuriSkewinTrack[ii]

      );
      fprintf(HANDLE, "       e corrisponde a track found n. %d\n", ii);
      fprintf(HANDLE, "       AVENDO %d hits paralleli e %d hits skew non mecciati dalla corrisponde track found\n", nMCParalAlone[ii], nMCSkewAlone[ii]);

      HoughFi = atan2(Oy[ii], Ox[ii]);
      if (HoughFi < 0.)
        HoughFi += 2. * PI;

      Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy;
      Int_t icode;
      icode = pMCtr->GetPdgCode();       //   PDG code of track
      Oxx = pMCtr->GetStartVertex().X(); //   X of starting point track
      Oyy = pMCtr->GetStartVertex().Y(); //   Y of starting point track
      Px = pMCtr->GetMomentum().X();
      Py = pMCtr->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
      Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
      Fifi = atan2(Cy, Cx); // MC truth Fifi angle of circle of Helix trajectory
      if (Fifi < 0.)
        Fifi += 2. * PI;
      Double_t Kakka;
      if (fabs(pMCtr->GetMomentum().Z()) < 1.e-20)
        Kakka = 99999999.;
      else
        Kakka = -carica * 0.001 * BFIELD * CVEL / pMCtr->GetMomentum().Z();

      fprintf(HANDLE, "       R_MC %g R %g Fi_MC %g Fi %g KAPPA_MC %g KAPPA %g FI0_MC %g FI0 %g\n", Rr, R[ii], Fifi, HoughFi, Kakka, KAPPA[ii],
              fmod(Fifi + PI, 2. * PI), //  FI0  da MC truth
              FI0[ii]);

    } //   end of  for (ii=0; ii<nTracksFoundSoFar && istampa>=2 ;ii++)

    //--------------ghosts

    // fa il conto delle ghost solo sugli eventi che hanno almeno 1 traccia MC accettabile.
    int NParghost = 0, NParhitsghost = 0, icc;
    if (istampa >= 1 && nMCTracksaccettabili > 0) {
      for (icc = 0; icc < nTracksFoundSoFar; icc++) {
        if (!keepit[icc])
          continue;
        if (daTrackFoundaTrackMC[icc] == -1) {
          NParghost++;
          NParhitsghost += nHitsinTrack[icc] + nSttSkewhitinTrack[icc];

          fprintf(HANDLE, "          tracce Trovata n. %d e' Ghost\n", icc);
        }
      }
      fprintf(HANDLE, "          tracceGhostTrovate %d TotaleHitsGhost %d  ----\n", NParghost, NParhitsghost);

      fprintf(HANDLE, "----------------------------------------------------------\n");

    } //   end of    if( istampa>=2)

  } //  end of  if( nMCTracks >0 && nTracksFoundSoFar > 0  && doMcComparison )

  //--------------------  fine della sezione sul confronto tra MC truth e tracce trovate

  //----------------------------------------------------------------------------------
  //   loading the hits found and associated to a track in a  PndTrackCand  class;
  //   a class per each track

  int ipinco = 0, ipanco = 0;
  Int_t flag;
  Double_t Ptras, Pxini, Pyini, Pzini, dista, qop;
  PndTrackCand *pTrckCand;
  PndTrack *pTrck;
  TVector3 Momentum, ErrMomentum, Position, ErrPosition;
  for (i = 0; i < nTracksFoundSoFar; i++) {
    if (!keepit[i])
      continue;
    dista = sqrt(Ox[i] * Ox[i] + Oy[i] * Oy[i]);
    Ptras = R[i] * 0.003 * BFIELD;
    Pxini = -Charge[i] * Ptras * Oy[i] / dista;
    Pyini = Charge[i] * Ptras * Ox[i] / dista;
    TVector3 posSeed(0., 0., 0.); //  the starting point of the trajectory

    if (GoodSkewFit[i]) {
      new ((*trackCandArray)[ipinco]) PndTrackCand;
      pTrckCand = (PndTrackCand *)trackCandArray->At(ipinco);

      if (fabs(KAPPA[i]) > 1.e-20) {
        Pzini = -Charge[i] * 0.003 * BFIELD / KAPPA[i];
        // PMAX is the maximum Pz tolerable; it is set at 100 for now.
        if (fabs(Pzini) < PMAX)
          flag = 0;
        else
          flag = -1;
        TVector3 dirSeed(Pxini, Pyini,
                         Pzini); // momentum direction in starting point
        qop = Charge[i] / dirSeed.Mag();
        dirSeed.SetMag(1.);
        pTrckCand->setTrackSeed(posSeed, dirSeed, qop);
        if (doMcComparison) {
          pTrckCand->setMcTrackId(daTrackFoundaTrackMC[i]);
        } else {
          pTrckCand->setMcTrackId(-1);
        }
        for (j = 0; j < nTotalHits[i]; j++) {
          pTrckCand->AddHit(FairRootManager::Instance()->GetBranchId(fSttBranch), (Int_t)BigList[i][j], j);
        }

        //  add the SciTil hit(s).
        if (nSciTilHitsinTrack[i] > 0) {
          for (j = 0; j < nSciTilHitsinTrack[i]; j++) {
            pTrckCand->AddHit(1001, // my personal hit type for SciTil.
                              (Int_t)ListSciTilHitsinTrack[i][j], nTotalHits[i] + j);
          }
        }

        //--------  do relevant calculation for this track and load the PndTrack  class
        //---- first hit
        if (fabs(info[BigList[i][0]][5] - 1.) < 1.e-10) { // it is a parallel straw
          PndSttInfoXYZParal(info, BigList[i][0], Ox[i], Oy[i], R[i], KAPPA[i], FI0[i], Charge[i], Posiz1);
        } else {                                           //   it is a skew straw
          PndSttInfoXYZSkew(Zfinal[i][BigList[i][0]],      //  Z coordinate of selected Skew hit
                            ZDriftfinal[i][BigList[i][0]], // drift distance IN Z DIRECTION only, of Skew hit
                            Sfinal[i][BigList[i][0]], Ox[i], Oy[i], R[i], KAPPA[i], FI0[i], Charge[i], Posiz1);
        }
        //  if all X, Y, Z positions were found for the first hit, go on
        if (Posiz1[0] > -777777776.) {
          //---- last hit
          if (fabs(info[BigList[i][nTotalHits[i] - 1]][5] - 1.) < 1.e-10) { // it is a parallel straw
            PndSttInfoXYZParal(info, BigList[i][nTotalHits[i] - 1], Ox[i], Oy[i], R[i], KAPPA[i], FI0[i], Charge[i], Posiz2);
          } else {                                                           //   it is a skew straw
            PndSttInfoXYZSkew(Zfinal[i][BigList[i][nTotalHits[i] - 1]],      //  Z coordinate of selected Skew hit
                              ZDriftfinal[i][BigList[i][nTotalHits[i] - 1]], // drift distance IN Z DIRECTION only, of Skew hit
                              Sfinal[i][BigList[i][nTotalHits[i] - 1]], Ox[i], Oy[i], R[i], KAPPA[i], FI0[i], Charge[i], Posiz2);
          }
          if (Posiz2[0] > -777777776.) {

            // load in   FairTrackParP first  the relevant quantities
            // of the first hit
            Position.SetX(Posiz1[0]);
            Position.SetY(Posiz1[1]);
            Position.SetZ(Posiz1[2]);
            ErrPosition.SetX(0.02); // 200 microns
            ErrPosition.SetY(0.02); // 200 microns
            ErrPosition.SetZ(1.);   // 1 cm
            // calculate Px and Py
            versor[0] = Ox[i] - Posiz1[0];
            versor[1] = Oy[i] - Posiz1[1];
            Distance = sqrt(versor[0] * versor[0] + versor[1] * versor[1]);
            // I already know from PndSttInfoXYZ... that versor is not zero.
            versor[0] /= Distance;
            versor[1] /= Distance;
            Px = -Charge[i] * Ptras * versor[1];
            Py = Charge[i] * Ptras * versor[0];
            Momentum.SetX(Px);
            Momentum.SetY(Py);
            Momentum.SetZ(Pzini);
            ErrMomentum.SetX(0.05 * Ptras); //  set at 5% all the times.
            ErrMomentum.SetY(0.05 * Ptras); //  set at 5% all the times.
            ErrMomentum.SetZ(0.05 * Pzini); //  set at 5% all the times.
            //  the plane of this FairTrackParP better is perpendicular to
            //  the momentum direction
            ddd = Ptras * sqrt(Ptras * Ptras + Pzini * Pzini);
            FairTrackParP first(Position, Momentum, ErrPosition, ErrMomentum, Charge[i], Position, TVector3(Py / Ptras, -Px / Ptras, 0.),
                                TVector3(Pzini * Px / ddd, Pzini * Py / ddd, -Ptras * Ptras / ddd));
            // load in   FairTrackParP first  the relevant quantities
            // of the last hit
            Position.SetX(Posiz2[0]);
            Position.SetY(Posiz2[1]);
            Position.SetZ(Posiz2[2]);
            ErrPosition.SetX(0.02); // 200 microns
            ErrPosition.SetY(0.02); // 200 microns
            ErrPosition.SetZ(1.);   // 1 cm
            // calculate Px and Py
            versor[0] = Ox[i] - Posiz2[0];
            versor[1] = Oy[i] - Posiz2[1];
            Distance = sqrt(versor[0] * versor[0] + versor[1] * versor[1]);
            // I already know from PndSttInfoXYZ... that versor is not zero.
            versor[0] /= Distance;
            versor[1] /= Distance;
            Px = -Charge[i] * Ptras * versor[1];
            Py = Charge[i] * Ptras * versor[0];
            Momentum.SetX(Px);
            Momentum.SetY(Py);
            // Momentum.SetZ(Pzini);
            ErrMomentum.SetX(0.05 * Ptras); //  set at 5% all the times.
            ErrMomentum.SetY(0.05 * Ptras); //  set at 5% all the times.
            // ErrMomentum.SetZ(0.05*Pzini); //  set at 5% all the times.
            //  the plane of this FairTrackParP better is perpendicular to
            //  the momentum direction
            FairTrackParP last(Position, Momentum, ErrPosition, ErrMomentum, Charge[i], Position, TVector3(Py / Ptras, -Px / Ptras, 0.),
                               TVector3(Pzini * Px / ddd, Pzini * Py / ddd, -Ptras * Ptras / ddd));
            //--------   load PndTrack object
            pTrck = new ((*trackArray)[ipanco]) PndTrack(first, last, *pTrckCand);
            pTrck->SetRefIndex(ipanco);
            pTrck->SetFlag(flag); // at this point flag = 0 for Pz<PMAX, -1 otherwise.
            ipanco++;

          } else { // continuation  of  if( Posiz2[0]>-777777776.)
            // case of something wrong with last hit
            FairTrackParP first(TVector3(-99999., -99999., -99999.), //  dummy Position
                                TVector3(-99999., -99999., -99999.), //  dummy Momentum
                                TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                                TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                                0,                                   //  dummy Charge
                                TVector3(-99999., -99999., -99999.), //  dummy Position again
                                TVector3(1., 0., 0.),                //  dummy direction versor
                                TVector3(0., 1., 0.)                 //  dummy direction versor
            );
            FairTrackParP last(TVector3(-99999., -99999., -99999.), //  dummy Position
                               TVector3(-99999., -99999., -99999.), //  dummy Momentum
                               TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                               TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                               0,                                   //  dummy Charge
                               TVector3(-99999., -99999., -99999.), //  dummy Position again
                               TVector3(1., 0., 0.),                //  dummy direction versor
                               TVector3(0., 1., 0.)                 //  dummy direction versor
            );
            //--------   load PndTrack object
            pTrck = new ((*trackArray)[ipanco]) PndTrack(first, last, *pTrckCand);
            pTrck->SetRefIndex(ipanco);
            pTrck->SetFlag(-1);
            ipanco++;

          } // end of if( Posiz2[0]>-777777776. )

        } else { // continuation of  if( Posiz1[0] > -777777776.)
          // case of something wrong with first hit
          FairTrackParP first(TVector3(-99999., -99999., -99999.), //  dummy Position
                              TVector3(-99999., -99999., -99999.), //  dummy Momentum
                              TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                              TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                              0,                                   //  dummy Charge
                              TVector3(-99999., -99999., -99999.), //  dummy Position again
                              TVector3(1., 0., 0.),                //  dummy direction versor
                              TVector3(0., 1., 0.)                 //  dummy direction versor
          );
          FairTrackParP last(TVector3(-99999., -99999., -99999.), //  dummy Position
                             TVector3(-99999., -99999., -99999.), //  dummy Momentum
                             TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                             TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                             0,                                   //  dummy Charge
                             TVector3(-99999., -99999., -99999.), //  dummy Position again
                             TVector3(1., 0., 0.),                //  dummy direction versor
                             TVector3(0., 1., 0.)                 //  dummy direction versor
          );
          //--------   load PndTrack object
          pTrck = new ((*trackArray)[ipanco]) PndTrack(first, last, *pTrckCand);
          pTrck->SetRefIndex(ipanco);
          pTrck->SetFlag(-1);
          ipanco++;
        } // end of  if( Posiz1[0] > -777777776.)

      } else { //   continuation of   if(fabs(KAPPA[i])>1.e-20  )

        Pzini = 999999.;
        TVector3 dirSeed(Pxini, Pyini,
                         Pzini); // momentum direction in starting point
        qop = Charge[i] / dirSeed.Mag();
        dirSeed.SetMag(1.);

        pTrckCand->setTrackSeed(posSeed, dirSeed, qop);
        pTrckCand->setMcTrackId(daTrackFoundaTrackMC[i]);
        for (j = 0; j < nTotalHits[i]; j++) {
          pTrckCand->AddHit(
            //			FairRootManager::Instance()->GetBranchId("STTHit"),
            FairRootManager::Instance()->GetBranchId(fSttBranch), (Int_t)BigList[i][j], j);
        }

        // load dummy quantities in PndTrack
        FairTrackParP first(TVector3(-99999., -99999., -99999.), //  dummy Position
                            TVector3(-99999., -99999., -99999.), //  dummy Momentum
                            TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                            TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                            0,                                   //  dummy Charge
                            TVector3(-99999., -99999., -99999.), //  dummy Position again
                            TVector3(1., 0., 0.),                //  dummy direction versor
                            TVector3(0., 1., 0.)                 //  dummy direction versor
        );
        FairTrackParP last(TVector3(-99999., -99999., -99999.), //  dummy Position
                           TVector3(-99999., -99999., -99999.), //  dummy Momentum
                           TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                           TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                           0,                                   //  dummy Charge
                           TVector3(-99999., -99999., -99999.), //  dummy Position again
                           TVector3(1., 0., 0.),                //  dummy direction versor
                           TVector3(0., 1., 0.)                 //  dummy direction versor
        );
        //--------   load PndTrack object
        pTrck = new ((*trackArray)[ipanco]) PndTrack(first, last, *pTrckCand);
        pTrck->SetRefIndex(ipanco);
        pTrck->SetFlag(-1);
        ipanco++;

      } // end of  if(fabs(KAPPA[i])>1.e-20  )

    } else { //   continuation of    if(   GoodSkewFit[i]  )   //  case in which there is no
             //  skew hits in this track.
             //  This fact is signalled by
      Pzini = 9999.;
      new ((*trackCandArray)[ipinco]) PndTrackCand;
      pTrckCand = (PndTrackCand *)trackCandArray->At(ipinco);
      TVector3 dirSeed(Pxini, Pyini,
                       Pzini); // momentum direction in starting point
      qop = Charge[i] / Ptras; //  as if Pz=0

      //          dirSeed.SetMag(1.);
      pTrckCand->setTrackSeed(posSeed, dirSeed, qop);
      pTrckCand->setMcTrackId(daTrackFoundaTrackMC[i]);
      for (j = 0; j < nTotalHits[i]; j++) {

        pTrckCand->AddHit(
          //		FairRootManager::Instance()->GetBranchId("STTHit"),
          FairRootManager::Instance()->GetBranchId(fSttBranch), (Int_t)BigList[i][j], j);
      }

      // load dummy quantities in PndTrack
      FairTrackParP first(TVector3(-99999., -99999., -99999.), //  dummy Position
                          TVector3(-99999., -99999., -99999.), //  dummy Momentum
                          TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                          TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                          0,                                   //  dummy Charge
                          TVector3(-99999., -99999., -99999.), //  dummy Position again
                          TVector3(1., 0., 0.),                //  dummy direction versor
                          TVector3(0., 1., 0.)                 //  dummy direction versor
      );
      FairTrackParP last(TVector3(-99999., -99999., -99999.), //  dummy Position
                         TVector3(-99999., -99999., -99999.), //  dummy Momentum
                         TVector3(-99999., -99999., -99999.), //  dummy ErrPosition
                         TVector3(-99999., -99999., -99999.), //  dummy ErrMomentum
                         0,                                   //  dummy Charge
                         TVector3(-99999., -99999., -99999.), //  dummy Position again
                         TVector3(1., 0., 0.),                //  dummy direction versor
                         TVector3(0., 1., 0.)                 //  dummy direction versor
      );
      //--------   load PndTrack object
      pTrck = new ((*trackArray)[ipanco]) PndTrack(first, last, *pTrckCand);
      pTrck->SetRefIndex(ipanco);
      pTrck->SetFlag(-1);
      ipanco++;

    } //   end of      if(   GoodSkewFit[i]  )

    //--- now increment the : number of PndTrackCand counter

    ipinco++;

  } //  end of     for(i=0; i<nTracksFoundSoFar;i++)

  //---------------   printouts of comparison with MC for judging algorithm performance

  if (nMCTracks > 0 && nTracksFoundSoFar > 0 && doMcComparison) {

    for (i = 0; i < nTracksFoundSoFar; i++) {
      if (!GoodSkewFit[i])
        continue;
      if (!keepit[i])
        continue;

      for (j = 0; j < nParalCommon[i]; j++) {

        Posiz[0] = Posiz[1] = Posiz[2] = 0.;
        PndSttInfoXYZParal(info, ParalCommonList[i][j], Ox[i], Oy[i], R[i], KAPPA[i], FI0[i], Charge[i],
                           Posiz // position of hit n. ParalCommonList[i][j]
                                 // in 'Parallel' hits notation.
        );

        if (istampa >= 2)
          cout << "PndTrackFinderReal::DoFind, paralleli, n. hits (original notation) = " << ParalCommonList[i][j] << ", X = " << Posiz[0] << ", Y = " << Posiz[1]
               << ", Z = " << Posiz[2] << endl;

        if (istampa >= 2) {
          if (info[ParalCommonList[i][j]][6] < 0.) {
            fprintf(PHANDLEX, "Noise hit\n");
            fprintf(PHANDLEY, "Noise hit\n");
            fprintf(PHANDLEZ, "Noise hit\n");
          } else {
            fprintf(PHANDLEX, "%g\n", veritaMC[ParalCommonList[i][j]][0] - Posiz[0]);
            fprintf(PHANDLEY, "%g\n", veritaMC[ParalCommonList[i][j]][1] - Posiz[1]);
            fprintf(PHANDLEZ, "%g\n", veritaMC[ParalCommonList[i][j]][2] - Posiz[2]);
          }
        }

      } //   end of for( j=0; j<nParalCommon[i]; j++)

      for (j = 0; j < nSkewCommon[i]; j++) {

        PndSttInfoXYZSkew(Zfinal[i][SkewCommonList[i][j]],      //  Z coordinate of selected Skew hit
                          ZDriftfinal[i][SkewCommonList[i][j]], // drift distance IN Z DIRECTION only, of Skew hit
                          Sfinal[i][SkewCommonList[i][j]], Ox[i], Oy[i], R[i], KAPPA[i], FI0[i], Charge[i], Posiz);
        if (istampa >= 2)
          cout << "DoFind, skew, n. hits (original notation) = " << SkewCommonList[i][j] << ", X = " << Posiz[0] << ", Y = " << Posiz[1] << ", Z = " << Posiz[2] << endl;

        if (istampa >= 2) {
          if (info[SkewCommonList[i][j]][6] < 0.) {
            fprintf(SHANDLEX, "Noise hit\n");
            fprintf(SHANDLEY, "Noise hit\n");
            fprintf(SHANDLEZ, "Noise hit\n");
          } else {
            fprintf(SHANDLEX, "%g\n", veritaMC[SkewCommonList[i][j]][0] - Posiz[0]);
            fprintf(SHANDLEY, "%g\n", veritaMC[SkewCommonList[i][j]][1] - Posiz[1]);
            fprintf(SHANDLEZ, "%g\n", veritaMC[SkewCommonList[i][j]][2] - Posiz[2]);
          }
        }

      } //   end of for( j=0; j<nSkewCommon[i]; j++)

    } //  end of     for(i=0; i<nTracksFoundSoFar;i++)

  } //  end of  if( nMCTracks >0 && nTracksFoundSoFar > 0  && doMcComparison )

  //---------------  end of printouts of comparison with MC for judging algorithm performance

  //---------------------------------------------------------------------------------------------------------

  //------------------------------------- macro di display.

  if (iplotta && IVOLTE <= nmassimo) {

    //--------------------------------------------------    macro for display

    //------
    //----------------fine stampa
    WriteMacroParallelHitsGeneral(keepit, Nhits, info, Nincl, Minclinations, inclination, nTracksFoundSoFar);

    //----------------------------------- end macro for display

    if (doMcComparison) {
      WriteMacroParallelHitsGeneralConformalwithMC(keepit, Nhits, info, Nincl, Minclinations, inclination, nTracksFoundSoFar);
    }

    for (i = 0, ii = -1; i < nTracksFoundSoFar; i++) {
      if (!keepit[i])
        continue;
      ii++;

      WriteMacroParallelAssociatedHits(Ox[i], Oy[i], R[i], nHitsinTrack[i], ListHitsinTrack, info, Nincl, Minclinations, inclination, i, ii, nSciTilHitsinTrack[i],
                                       &ListSciTilHitsinTrack[i][0]);

      if (doMcComparison) {
        WriteMacroParallelAssociatedHitswithMC(Ox[i], Oy[i], R[i], daTrackFoundaTrackMC[i], nHitsinTrack[i], ListHitsinTrack, info, i, ii, nSciTilHitsinTrack[i],
                                               &ListSciTilHitsinTrack[i][0], nParalCommon, ParalCommonList, nSpuriParinTrack, ParSpuriList, nMCParalAlone, MCParalAloneList

        );
      } // end of if(doMcComparison)
    }   // end of for(i=0, ii=-1; i<nTracksFoundSoFar;i++)

    for (i = 0, ii = -1; i < nTracksFoundSoFar; i++) {
      if (!keepit[i])
        continue;
      ii++;
      if (iplotta) {

        HoughR = R[i];
        HoughD = sqrt(Ox[i] * Ox[i] + Oy[i] * Oy[i]) - R[i];
        HoughFi = atan2(Oy[i], Ox[i]);
        if (HoughFi < 0.)
          HoughFi += 2. * PI;

        //  if there are SciTil hits, calculate S.
        Double_t ESSE[nSciTilHitsinTrack[i]], ZETA[nSciTilHitsinTrack[i]];
        // here nSciTilHitsinTrack[i] is up to 2.
        if (nSciTilHitsinTrack[i] > 0) {
          for (j = 0; j < nSciTilHitsinTrack[i]; j++) {

            ESSE[j] = S_SciTilHitsinTrack[i][j];
            ZETA[j] = posizSciTil[ListSciTilHitsinTrack[i][j]][2];
          }
        }

        WriteMacroSkewAssociatedHits(GoodSkewFit[i], KAPPA[i], FI0[i], HoughD, HoughFi, HoughR, info, Nincl, Minclinations, inclination, i, ii, nSttSkewhitinTrack[i],
                                     ListSkewHitsinTrack, nSciTilHitsinTrack[i], ESSE, ZETA);
        if (doMcComparison) {
          if (daTrackFoundaTrackMC[i] >= 0) {
            WriteMacroSkewAssociatedHitswithMC(GoodSkewFit[i], KAPPA[i], FI0[i], HoughD, HoughFi, HoughR, info, Nincl, Minclinations, inclination, i, ii, nSttSkewhitinTrack[i],
                                               ListSkewHitsinTrack, nSkewCommon[i], SkewCommonList, daTrackFoundaTrackMC[i], nMCSkewAlone, MCSkewAloneList, nSciTilHitsinTrack[i],
                                               ESSE, ZETA);
          }
        }

      } //  end of   if (iplotta)
    }   //   end of  for(i=0; i<nTracksFoundSoFar;i++)

  } // end of if(iplotta && IVOLTE <= nmassimo)

  //------------------------------------- fine macro di display delle skew

  return ((Int_t)nTracksFoundSoFar);

}; //-------------------------------------------------  end of function  PndSttTrackFinderReal::DoFind

CalculatedCircles PndSttTrackFinderReal::PndSttTrkFindCircles(Double_t x1, Double_t y1, Double_t r1, Double_t x2, Double_t y2, Double_t r2, Double_t x3, Double_t y3, Double_t r3)
{
  Double_t a, b, c, d, ap, bp, cp, dp, radix, solution, Radius, A, B, C, D, AAA, BBB, CCC, DELTA, S, T, X;
  Int_t Nsol = 0;
  Double_t R[8], CX[8], CY[8];
  CalculatedCircles Ris;

  // cout<<"---------------------------------- inizio printout da find_circles \n";

  /*
    x1 = X center crf 1;
    y1 = Y center crf 1;
    r1 = radius crf 1;
    x2 = X center crf 2;
    y2 = Y center crf 2;
    r2 = radius crf 2;
    x3 = X center crf 3;
    y3 = Y center crf 3;
    r3 = radius crf 3;
  */

  a = 2. * (x1 - x2);
  b = 2. * (y1 - y2);
  d = x1 * x1 + y1 * y1 - r1 * r1 - x2 * x2 - y2 * y2 + r2 * r2;
  ap = 2. * (x1 - x3);
  bp = 2. * (y1 - y3);
  dp = x1 * x1 + y1 * y1 - r1 * r1 - x3 * x3 - y3 * y3 + r3 * r3;

  /*
  cout<<"printout iniziale :\nx1="<<x1<<";\nx2="<<x2<<";\nx3="<<x3<<";\ny1="<<y1<<";\ny2="<<y2<<";\ny3="
         <<y3<<";\nr1="<<r1<<";\nr2="<<r2<<";\nr3="<<r3<<endl;
  cout<<" a= "<<a<<";  b="<<b<<";  d="<<d<<endl;
  cout<<" ap= "<<ap<<";  bp="<<bp<<";  dp="<<dp<<endl;
  cout<<"fine printout iniziale\n";
  */

  Double_t aaa = a * bp - b * ap;
  // cout<<"aaa  : "<<aaa<<endl;
  if (aaa == 0.) {
    for (Int_t i1 = -1; i1 <= 1; i1 += 2) {
      for (Int_t i2 = -1; i2 <= 1; i2 += 2) {
        c = -2. * (i1 * r1 - i2 * r2);
        for (Int_t i3 = -1; i3 <= 1; i3 += 2) {
          cp = -2. * (i1 * r1 - i3 * r3);
          if (bp * c - b * cp == 0. && b == 0. && bp == 0. && a != 0. && ap != 0. && a != ap && ap * c - a * cp != 0.) {
            Radius = (a * dp - ap * d) / (ap * c - a * cp);
            if (Radius > 0.) {
              S = d / a;
              T = -c / a;
              BBB = y1;
              CCC = (S - T * Radius) * (S - T * Radius) - Radius * Radius - 2. * x1 * (S - T * Radius) - 2. * i1 * Radius * r1 + x1 * x1 + y1 * y1 - r1 * r1;
              DELTA = BBB * BBB - CCC;
              if (DELTA < 0.) {
                continue;
              } else if (DELTA == 0.) {
                Nsol++;
                R[Nsol - 1] = Radius;
                CX[Nsol - 1] = S - T * Radius;
                CY[Nsol - 1] = -BBB;
              } else {
                Nsol += 2;
                R[Nsol - 2] = Radius;
                CX[Nsol - 2] = S - T * Radius;
                CY[Nsol - 2] = -BBB + sqrt(DELTA);
                R[Nsol - 1] = Radius;
                CX[Nsol - 1] = S - T * Radius;
                CY[Nsol - 1] = -BBB - sqrt(DELTA);
              }

            } //  end  if ( Radius >0.)
          } else if (bp * c - b * cp != 0.) {

            Radius = (-bp * d + b * dp) / (bp * c - b * cp);
            if (Radius > 0. && b != 0.) {
              S = (d + c * Radius) / b;
              T = a / b;
              AAA = 1 + T * T;
              BBB = -(T * S + x1 - T * y1);
              CCC = S * S - Radius * Radius - 2. * y1 * S - 2. * i1 * r1 * Radius + x1 * x1 + y1 * y1 - r1 * r1;
              DELTA = BBB * BBB - AAA * CCC;
              // cout<<"AAA ="<<AAA<<",  BBB= "<<BBB<<", CCC ="<<CCC<<", DELTA= "<<DELTA<<endl;
              // cout<<"S = "<<S<<", T= "<<T<<endl<<"DELTA "<<DELTA<<endl;
              if (DELTA < 0.) {
                continue;
              } else if (DELTA == 0.) {
                Nsol++;
                R[Nsol - 1] = Radius;
                CX[Nsol - 1] = -BBB / AAA;
                CY[Nsol - 1] = S - T * CX[Nsol - 1];
                // cout<<"CX[Nsol-1] ="<<CX[Nsol-1]<<",  CY[Nsol-1]= "<<CY[Nsol-1]<<endl;
              } else {
                Nsol += 2;
                R[Nsol - 2] = Radius;
                CX[Nsol - 2] = (-BBB + sqrt(DELTA)) / AAA;
                CY[Nsol - 2] = S - T * CX[Nsol - 2];
                R[Nsol - 1] = Radius;
                CX[Nsol - 1] = (-BBB - sqrt(DELTA)) / AAA;
                CY[Nsol - 1] = S - T * CX[Nsol - 1];
                // cout<<"CX[Nsol-2] ="<<CX[Nsol-2]<<",  CY[Nsol-2]= "<<CY[Nsol-2]<<endl;
                // cout<<"CX[Nsol-1] ="<<CX[Nsol-1]<<",  CY[Nsol-1]= "<<CY[Nsol-1]<<endl;
              } // end  if(DELTA<0.)
            }   //  end  if( Radius > 0. && b != 0. )
          }     //   end if( bp*c-b*cp == 0.)
        }       //  end for (Int_t i3
      }         //  end for(Int_t i2
    }           //  end for(Int_t i1

  } else { // of if(aaa == 0. )

    A = (bp * d - b * dp) / aaa;
    C = (-ap * d + a * dp) / aaa;
    // cout<<"aaa "<<aaa<<endl<<"A "<<A<<endl<<"C "<<C<<endl;
    for (Int_t i1 = -1; i1 <= 1; i1 += 2) {
      for (Int_t i2 = -1; i2 <= 1; i2 += 2) {
        c = -2. * (i1 * r1 - i2 * r2);
        for (Int_t i3 = -1; i3 <= 1; i3 += 2) {
          cp = -2. * (i1 * r1 - i3 * r3);
          // cout<<"----------------------------------------\n";
          // cout<<"c="<<c<<";  cp="<<cp<<endl;

          B = (bp * c - b * cp) / aaa;
          D = (-ap * c + a * cp) / aaa;

          AAA = B * B + D * D - 1.;
          BBB = A * B + C * D - B * x1 - D * y1 - i1 * r1;
          CCC = A * A + C * C - 2. * x1 * A - 2. * y1 * C + x1 * x1 + y1 * y1 - r1 * r1;
          DELTA = BBB * BBB - CCC * AAA;
          //        cout<<"caso con i1= "<<i1<<", i2, i3 = "<<" "<<i2<<" "<<i3<<endl
          //              <<"A "<<A<<";  B = "<<B<<"; C= "<<C<<"; D = "<<D<<endl;
          //        cout<<"AAA = "<<AAA<<";  BBB = "<<BBB<<" DELTA "<<DELTA<<endl;
          //        cout<<"sqrt(DELTA) = "<<sqrt(DELTA)<<endl;

          if (DELTA < 0.) {
            continue;
          } else if (DELTA == 0.) {
            if (BBB == 0.) {
              continue;
            } else {
              solution = -BBB / AAA;
              if (solution > 0.) {
                Nsol++;
                R[Nsol - 1] = solution;
                CX[Nsol - 1] = A + B * R[Nsol - 1];
                CY[Nsol - 1] = C + D * R[Nsol - 1];
              }
              continue;
            }
          }

          //  this is the case when DELTA > 0.

          radix = sqrt(DELTA);

          if (AAA == 0.) {
            cout << "AAA== 0.";
            continue;
          } else {

            solution = (-BBB + radix) / AAA;
            // cout<<"-BBB[j]  radix  AAA[j] : "<<-BBB <<";  "<< radix <<"; "<< AAA<<"; solution=+ :"<<solution<<endl;
            if (solution > 0.) {
              Nsol++;
              R[Nsol - 1] = solution;
              CX[Nsol - 1] = A + B * R[Nsol - 1];
              CY[Nsol - 1] = C + D * R[Nsol - 1];
            }

            solution = (-BBB - radix) / AAA;
            // cout<<"-BBB[j]  radix  AAA[j] : "<<-BBB <<";  "<< radix <<"; "<< AAA<<"; solution=- :"<<solution<<endl;
            if (solution > 0.) {
              Nsol++;
              R[Nsol - 1] = solution;
              CX[Nsol - 1] = A + B * R[Nsol - 1];
              CY[Nsol - 1] = C + D * R[Nsol - 1];
            }

          } //  end of   if(AAA == 0.)

        } //  end  for(i3
      }   //  end  for(i2
    }     //  end  for(i1

  } // end if ( aaa == 0.)

  // cout<<"----------------------------------------\n";
  //  cout<<"n. soluzioni : "<<Nsol<<endl;
  Ris.Ncircles = Nsol;
  for (Int_t i = 0; i < Nsol; i++) {
    //  cout<<"Soluzione n. "<<i+1<<"; R = "<<R[i]<<", CX = "<<CX[i]<<", CY = "<<CY[i]<<endl;
    Ris.CX[i] = CX[i];
    Ris.CY[i] = CY[i];
    Ris.R[i] = R[i];
  }

  return Ris;
}

//----------------------------------------------------   end of function  PndSttTrackFinderReal::PndSttTrkFindCircles

//--------------------------------  begin of function         PndSttTrackFinderReal::calculateintersections

void PndSttTrackFinderReal::calculateintersections(Double_t Ox, Double_t Oy, Double_t R, Double_t C0x, Double_t C0y, Double_t C0z, Double_t r, Double_t vx, Double_t vy,
                                                   Double_t vz, Int_t *STATUS, Double_t *POINTS)
{

  //-------------------------------------------

  Double_t P1x, P1y, P1z, P2x, P2y, P2z;

  Double_t AAA, DELTA, ax, ay, aaa;

  /*
   INPUTS :

    Ox, Oy        = abscissa and ordinate of the center of the circular trajectory of
                    the particle;
    R             = radius of such trajectory;
    C0x, C0y, Coz = x, y, z coordinates of a point belonging to the axis of the
                    skewed straw;
    r  = radius of equidrift of such skewed straw;
    vx, vy, vz    =  versor of the direction along which the skewed straw lies.


   OUTPUTS :

    P1x, P1y, P1z  =  x, y, z coordinates of the point intersection between the
                      particle trajectory circle and the equidrift cylinder of
                      the skewed straw calculated as a function of theta (first
                      solution);
    P2x, P2y, P2z  =  x, y, z coordinates of the point intersection between the
                      particle trajectory circle and the equidrift cylinder of
                      the skewed straw calculated as a function of theta (second
                      solution);

   P1x, P1y, .... , P2z  are stored in the array POINTS[0-5];  the status is stored
   in *STATUS.

  */

  //--------------------------------------------------------------------------

  // from the resolving formula on page 23 of Gianluigi's notes, setting  r=0.

  ax = C0x - Ox;
  ay = C0y - Oy;
  DELTA = R * R * (vx * vx + vy * vy) - (vx * ay - vy * ax) * (vx * ay - vy * ax);
  AAA = vx * vx + vy * vy;

  if (DELTA < 0.) {
    *STATUS = -2;
  } else if (AAA == 0.) {
    *STATUS = -3;
  } else if (DELTA == 0.) {
    *STATUS = 1;
    POINTS[0] = C0x - vx * (vx * ax + vy * ay) / AAA;
    POINTS[1] = C0y - vy * (vx * ax + vy * ay) / AAA;
    POINTS[2] = C0z - vz * (vx * ax + vy * ay) / AAA;
  } else {
    *STATUS = 0;
    DELTA = sqrt(DELTA);
    POINTS[0] = C0x - vx * (vx * ax + vy * ay - DELTA) / AAA;
    POINTS[1] = C0y - vy * (vx * ax + vy * ay - DELTA) / AAA;
    POINTS[2] = C0z - vz * (vx * ax + vy * ay - DELTA) / AAA;
    POINTS[3] = C0x - vx * (vx * ax + vy * ay + DELTA) / AAA;
    POINTS[4] = C0y - vy * (vx * ax + vy * ay + DELTA) / AAA;
    POINTS[5] = C0z - vz * (vx * ax + vy * ay + DELTA) / AAA;
  }

  return;
}

//----------end of function PndSttTrackFinderReal::calculateintersections

//----------begin function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix

Short_t PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix(Double_t Ox, Double_t Oy, Double_t R, Int_t Nhits, Double_t info[][7],
                                                                      Short_t *auxListHitsinTrack //  this is the output
)
{

  Int_t i;
  Short_t Nassociatedhits;

  Double_t dx, dy, distance;

  //    Ox = (D+R)*cos(Fi);
  //    Oy = (D+R)*sin(Fi);

  //  association of hits in the XY plane

  Nassociatedhits = 0;

  for (i = 0; i < Nhits; i++) {

    //          Kincl = (int) info[i][5] - 1;

    // --------------------    association of the hits from parallel straws
    //         if( info[i][5] == 1. ) {     // parallel straws

    dx = -Ox + info[infoparal[i]][0];
    dy = -Oy + info[infoparal[i]][1];
    distance = sqrt(dx * dx + dy * dy);
    // cout<<"nuov, R "<<R<<",  distance  "<<distance<<endl;
    //          if( distance < 1.e-10)  continue;
    //          angle = atan2(dy,dx);

    if (fabs(R - distance) > 2. * STRAWRADIUS)
      continue;
    auxListHitsinTrack[Nassociatedhits] = i;
    Nassociatedhits++;

    //         }    //  end of   if( info[i][5] == 1 )  else

  } //   end of   for( i=1; i< Nhits; i++)

  return Nassociatedhits;
}

//----------end of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix

//----------start of function PndSttTrackFinderReal::plottamentiParalleleGenerali

void PndSttTrackFinderReal::plottamentiParalleleGenerali(Int_t Nremaining, Float_t *RemainingR, Float_t *RemainingD, Float_t *RemainingFi, Float_t *RemainingCX,
                                                         Float_t *RemainingCY, bool *Goodflag)
{

  int itemp, i, j, ii, jj;

  Double_t D, Fi;

  char nome[300], titolo[300];

  sprintf(nome, "HoughGeneralPlotsEvent%d.root", IVOLTE);
  TFile hfile(nome, "RECREATE", "STT pattern recognition");

  //----------
  sprintf(titolo, "Cxofcircle");
  TH1F hCX(titolo, titolo, nbinCX, CXmin, CXmax);
  //----------
  sprintf(titolo, "Cyofcircle");
  TH1F hCY(titolo, titolo, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "Radiusofcircle");
  TH1F hR(titolo, titolo, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CxabscissavsCy");
  TH2F hCX_CY(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "CxabscissavsRadius");
  TH2F hCX_R(titolo, titolo, nbinCX, CXmin, CXmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CyabscissavsRadius");
  TH2F hCY_R(titolo, titolo, nbinCY, CYmin, CYmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CxabscissavsCyordinatevsRadius");
  TH3F hCX_CY_R(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "Distanceofclosestapproachofcircle");
  TH1F hD(titolo, titolo, nbinD, Dmin, Dmax);
  //----------
  sprintf(titolo, "Firadofcircle");
  TH1F hFi(titolo, titolo, nbinFi, Fimin, Fimax);
  //-----
  sprintf(titolo, "DabscissavsFi");
  TH2F hD_Fi(titolo, titolo, nbinD, Dmin, Dmax, nbinFi, Fimin, Fimax);
  //-----
  sprintf(titolo, "DabscissavsRadius");
  TH2F hD_R(titolo, titolo, nbinD, Dmin, Dmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "FiabscissavsRadius");
  TH2F hFi_R(titolo, titolo, nbinFi, Fimin, Fimax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "DabscissavsFiordinatevsRadius");
  TH3F hD_Fi_R(titolo, titolo, nbinD, Dmin, Dmax, nbinFi, Fimin, Fimax, nbinR, Rmin, Rmax);
  //-----

  //---------------------------------------------------
  //   gli stessi plots ma per le soluzioni che sono le piu' vicine alla verita' MC
  //----------
  sprintf(titolo, "Cxofcircle_truth");
  TH1F hCXverita(titolo, titolo, nbinCX, CXmin, CXmax);
  //----------
  sprintf(titolo, "Cyofcircle_truth");
  TH1F hCYverita(titolo, titolo, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "Radiusofcircle_truth");
  TH1F hRverita(titolo, titolo, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CxabscissavsCy_truth");
  TH2F hCX_CYverita(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "CxabscissavsRadius_truth");
  TH2F hCX_Rverita(titolo, titolo, nbinCX, CXmin, CXmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CyabscissavsRadius_truth");
  TH2F hCY_Rverita(titolo, titolo, nbinCY, CYmin, CYmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CxabscissavsCyordinatevsRadius_truth");
  TH3F hCX_CY_Rverita(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "Distanceofclosestapproachofcircle_truth");
  TH1F hDverita(titolo, titolo, nbinD, Dmin, Dmax);
  //----------
  sprintf(titolo, "Firadofcircle_truth");
  TH1F hFiverita(titolo, titolo, nbinFi, Fimin, Fimax);
  //-----
  sprintf(titolo, "DabscissavsFi_truth");
  TH2F hD_Fiverita(titolo, titolo, nbinD, Dmin, Dmax, nbinFi, Fimin, Fimax);
  //-----
  sprintf(titolo, "DabscissavsRadius_truth");
  TH2F hD_Rverita(titolo, titolo, nbinD, Dmin, Dmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "FiabscissavsRadius_truth");
  TH2F hFi_Rverita(titolo, titolo, nbinFi, Fimin, Fimax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "DabscissavsFiordinatevsRadius_truth");
  TH3F hD_Fi_Rverita(titolo, titolo, nbinD, Dmin, Dmax, nbinFi, Fimin, Fimax, nbinR, Rmin, Rmax);
  //-----

  //---------------------------------------------------
  //   gli stessi plots ma per le soluzioni che NON sono le piu' vicine alla verita' MC
  //----------
  sprintf(titolo, "Cxofcircle_nontruth");
  TH1F hCXnonverita(titolo, titolo, nbinCX, CXmin, CXmax);
  //----------
  sprintf(titolo, "Cyofcircle_nontruth");
  TH1F hCYnonverita(titolo, titolo, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "Radiusofcircle_nontruth");
  TH1F hRnonverita(titolo, titolo, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CxabscissavsCy_nontruth");
  TH2F hCX_CYnonverita(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "CxabscissavsRadius_nontruth");
  TH2F hCX_Rnonverita(titolo, titolo, nbinCX, CXmin, CXmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CyabscissavsRadius_nontruth");
  TH2F hCY_Rnonverita(titolo, titolo, nbinCY, CYmin, CYmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "CxabscissavsCyordinatevsRadius_nontruth");
  TH3F hCX_CY_Rnonverita(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "Distanceofclosestapproachofcircle_nontruth");
  TH1F hDnonverita(titolo, titolo, nbinD, Dmin, Dmax);
  //----------
  sprintf(titolo, "Firadofcircle_nontruth");
  TH1F hFinonverita(titolo, titolo, nbinFi, Fimin, Fimax);
  //-----
  sprintf(titolo, "DabscissavsFi_nontruth");
  TH2F hD_Finonverita(titolo, titolo, nbinD, Dmin, Dmax, nbinFi, Fimin, Fimax);
  //-----
  sprintf(titolo, "DabscissavsRadius_nontruth");
  TH2F hD_Rnonverita(titolo, titolo, nbinD, Dmin, Dmax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "FiabscissavsRadius_nontruth");
  TH2F hFi_Rnonverita(titolo, titolo, nbinFi, Fimin, Fimax, nbinR, Rmin, Rmax);
  //-----
  sprintf(titolo, "DabscissavsFiordinatevsRadius_nontruth");
  TH3F hD_Fi_Rnonverita(titolo, titolo, nbinD, Dmin, Dmax, nbinFi, Fimin, Fimax, nbinR, Rmin, Rmax);
  //-----

  //---------------------------------------------------

  for (itemp = 0; itemp < Nremaining; itemp++) {

    D = RemainingD[itemp];
    Fi = RemainingFi[itemp];

    hCX.Fill(RemainingCX[itemp]);
    hCY.Fill(RemainingCY[itemp]);
    hR.Fill(RemainingR[itemp]);
    hCX_CY.Fill(RemainingCX[itemp], RemainingCY[itemp]);
    hCX_R.Fill(RemainingCX[itemp], RemainingR[itemp]);
    hCY_R.Fill(RemainingCY[itemp], RemainingR[itemp]);
    hCX_CY_R.Fill(RemainingCX[itemp], RemainingCY[itemp], RemainingR[itemp]);

    hD.Fill(D);
    hFi.Fill(Fi);
    hD_Fi.Fill(D, Fi);
    hD_R.Fill(D, RemainingR[itemp]);
    hFi_R.Fill(Fi, RemainingR[itemp]);
    hD_Fi_R.Fill(D, Fi, RemainingR[itemp]);

    if (Goodflag[itemp]) {

      hCXverita.Fill(RemainingCX[itemp]);
      hCYverita.Fill(RemainingCY[itemp]);
      hRverita.Fill(RemainingR[itemp]);
      hCX_CYverita.Fill(RemainingCX[itemp], RemainingCY[itemp]);
      hCX_Rverita.Fill(RemainingCX[itemp], RemainingR[itemp]);
      hCY_Rverita.Fill(RemainingCY[itemp], RemainingR[itemp]);
      hCX_CY_Rverita.Fill(RemainingCX[itemp], RemainingCY[itemp], RemainingR[itemp]);

      hDverita.Fill(D);
      hFiverita.Fill(Fi);
      hD_Fiverita.Fill(D, Fi);
      hD_Rverita.Fill(D, RemainingR[itemp]);
      hFi_Rverita.Fill(Fi, RemainingR[itemp]);
      hD_Fi_Rverita.Fill(D, Fi, RemainingR[itemp]);

    } else {

      hCXnonverita.Fill(RemainingCX[itemp]);
      hCYnonverita.Fill(RemainingCY[itemp]);
      hRnonverita.Fill(RemainingR[itemp]);
      hCX_CYnonverita.Fill(RemainingCX[itemp], RemainingCY[itemp]);
      hCX_Rnonverita.Fill(RemainingCX[itemp], RemainingR[itemp]);
      hCY_Rnonverita.Fill(RemainingCY[itemp], RemainingR[itemp]);
      hCX_CY_Rnonverita.Fill(RemainingCX[itemp], RemainingCY[itemp], RemainingR[itemp]);

      hDnonverita.Fill(D);
      hFinonverita.Fill(Fi);
      hD_Finonverita.Fill(D, Fi);
      hD_Rnonverita.Fill(D, RemainingR[itemp]);
      hFi_Rnonverita.Fill(Fi, RemainingR[itemp]);
      hD_Fi_Rnonverita.Fill(D, Fi, RemainingR[itemp]);
    }

  } //  end of    for(Int_t itemp = 0; itemp<Nremaining; itemp++)

  hfile.Write(nome);
  hfile.Close();
}

//----------end of function PndSttTrackFinderReal::plottamentiParalleleGenerali

//----------start of function PndSttTrackFinderReal::plottamentiParalleleconMassimo

void PndSttTrackFinderReal::plottamentiParalleleconMassimo(char *tipo, Int_t nMaxima, Int_t Nremaining, Float_t *RemainingR, Float_t *RemainingD, Float_t *RemainingFi,
                                                           Float_t *RemainingCX, Float_t *RemainingCY, Double_t Rup, Double_t Rlow, Double_t Dup, Double_t Dlow, Double_t Fiup,
                                                           Double_t Filow)
{

  int itemp, i, j, ii, jj;

  Double_t D, Fi;

  char nome[300], titolo[300];

  sprintf(nome, "HoughParallele%sMaximumN%devent%d.root", tipo, nMaxima, IVOLTE);
  TFile hfile(nome, "RECREATE", "STT pattern recognition");

  //----------
  sprintf(titolo, "CxofcircleSelected");
  TH1F hCXsel(titolo, titolo, nbinCX, CXmin, CXmax);
  //-----
  sprintf(titolo, "CyofcircleSelected");
  TH1F hCYsel(titolo, titolo, nbinCY, CYmin, CYmax);
  //-----
  sprintf(titolo, "RadiusofcircleSelected");
  TH1F hRsel(titolo, titolo, nbinR, Rlow, Rup);
  //-----
  sprintf(titolo, "CxabscissavsCyordinatevsRadiusSelected");
  TH3F hCX_CY_Rsel(titolo, titolo, nbinCX, CXmin, CXmax, nbinCY, CYmin, CYmax, nbinR, Rlow, Rup);
  //----------
  sprintf(titolo, "DistanceofclosestapproachofcircleSelected");
  TH1F hDsel(titolo, titolo, nbinD, Dlow, Dup);
  //-----
  sprintf(titolo, "FiradofcircleSselected");
  TH1F hFisel(titolo, titolo, nbinFi, Filow, Fiup);
  //-----
  sprintf(titolo, "DabscissavsFiSelected");
  TH2F hD_Fisel(titolo, titolo, nbinD, Dlow, Dup, nbinFi, Filow, Fiup);
  //-----
  sprintf(titolo, "DabscissavsRadiusSelected");
  TH2F hD_Rsel(titolo, titolo, nbinD, Dlow, Dup, nbinR, Rlow, Rup);
  //-----
  sprintf(titolo, "FiabscissavsRadiusSelected");
  TH2F hFi_Rsel(titolo, titolo, nbinFi, Filow, Fiup, nbinR, Rlow, Rup);
  //-----
  sprintf(titolo, "DabscissavsFiordinatevsRadiusSelected");
  TH3F hD_Fi_Rsel(titolo, titolo, nbinD, Dlow, Dup, nbinFi, Filow, Fiup, nbinR, Rlow, Rup);

  for (itemp = 0; itemp < Nremaining; itemp++) {

    D = RemainingD[itemp];
    Fi = RemainingFi[itemp];

    if (RemainingR[itemp] > Rlow && RemainingR[itemp] < Rup && RemainingD[itemp] > Dlow && RemainingD[itemp] < Dup && RemainingFi[itemp] > Filow && RemainingFi[itemp] < Fiup) {
      hRsel.Fill(RemainingR[itemp]);
      hFisel.Fill(Fi);
      hDsel.Fill(D);
      hCXsel.Fill(RemainingCX[itemp]);
      hCYsel.Fill(RemainingCY[itemp]);
      hD_Fisel.Fill(D, Fi);
      hD_Rsel.Fill(D, RemainingR[itemp]);
      hFi_Rsel.Fill(Fi, RemainingR[itemp]);
      hD_Fi_Rsel.Fill(D, Fi, RemainingR[itemp]);
      hCX_CY_Rsel.Fill(RemainingCX[itemp], RemainingCY[itemp], RemainingR[itemp]);
    }

  } //  end of    for(Int_t itemp = 0; itemp<Nremaining; itemp++)

  hfile.Write(nome);
  hfile.Close();
}

//----------end of function PndSttTrackFinderReal::plottamentiParalleleconMassimo

bool PndSttTrackFinderReal::iscontiguous(int ncomponents, Short_t *vec1, Short_t *vec2)
{

  for (int i = 0; i < ncomponents; i++) {
    if (vec1[i] - vec2[i] > 3 || vec1[i] - vec2[i] < -3)
      return false;
  }

  return true;
}

//----------end of function PndSttTrackFinderReal::iscontiguous

void PndSttTrackFinderReal::clustering2(Short_t vec1[2],                                               // input
                                        int nListElements, Short_t List[][2],                          // input
                                        int &nClusterElementsFound, Short_t ClusterElementsFound[][2], // output
                                        int &nRemainingElements, Short_t RemainingElements[][2]        // output
)
{
  int i;
  Short_t vec2[2];

  nClusterElementsFound = 0;
  nRemainingElements = 0;
  for (i = 0; i < nListElements; i++) {
    vec2[0] = List[i][0];
    vec2[1] = List[i][1];
    if (iscontiguous(2, vec1, vec2)) {
      ClusterElementsFound[nClusterElementsFound][0] = vec2[0];
      ClusterElementsFound[nClusterElementsFound][1] = vec2[1];
      nClusterElementsFound++;
    } else {
      RemainingElements[nRemainingElements][0] = vec2[0];
      RemainingElements[nRemainingElements][1] = vec2[1];
      nRemainingElements++;
    }
  }

  return;
}

//----------end of function PndSttTrackFinderReal::clustering2

void PndSttTrackFinderReal::clustering3(Short_t vec1[3],                                               // input
                                        int nListElements, Short_t List[][3],                          // input
                                        int &nClusterElementsFound, Short_t ClusterElementsFound[][3], // output
                                        int &nRemainingElements, Short_t RemainingElements[][3]        // output
)
{
  int i;
  Short_t vec2[3];

  nClusterElementsFound = 0;
  nRemainingElements = 0;
  for (i = 0; i < nListElements; i++) {
    vec2[0] = List[i][0];
    vec2[1] = List[i][1];
    vec2[2] = List[i][2];
    if (iscontiguous(3, vec1, vec2)) {
      ClusterElementsFound[nClusterElementsFound][0] = vec2[0];
      ClusterElementsFound[nClusterElementsFound][1] = vec2[1];
      ClusterElementsFound[nClusterElementsFound][2] = vec2[2];
      nClusterElementsFound++;
    } else {
      RemainingElements[nRemainingElements][0] = vec2[0];
      RemainingElements[nRemainingElements][1] = vec2[1];
      RemainingElements[nRemainingElements][2] = vec2[2];
      nRemainingElements++;
    }
  }

  return;
}

//----------end of function PndSttTrackFinderReal::clustering3

//----------start of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneral

void PndSttTrackFinderReal::WriteMacroParallelHitsGeneral(bool *keepit, Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3],
                                                          Short_t nTracksFoundSoFar)
{

  Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

  Double_t xmin, xmax, ymin, ymax, xl, xu, yl, yu, Ox[nmaxHits], Oy[nmaxHits], Radius[nmaxHits], gamma, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, ff,
    zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, x1, x2, vx1, vy1, vz1, C0x1, C0y1, C0z1, aaa, bbb, ccc, rrr, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1,
    fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2, Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  char nome[300], nome2[300];

  //---------- parallel straws Macro now
  sprintf(nome, "MacroSttParEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //--- SciTil  info
  for (i = 0; i < nSciTilHits; i++) {
    if (posizSciTil[i][0] < xmin)
      xmin = posizSciTil[i][0];
    if (posizSciTil[i][0] > xmax)
      xmax = posizSciTil[i][0];
    if (posizSciTil[i][1] < ymin)
      ymin = posizSciTil[i][1];
    if (posizSciTil[i][1] > ymax)
      ymax = posizSciTil[i][1];
  }
  //------

  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
      if (info[i][0] - info[i][3] < xmin)
        xmin = info[i][0] - info[i][3];
      if (info[i][0] + info[i][3] > xmax)
        xmax = info[i][0] + info[i][3];
      if (info[i][1] - info[i][3] < ymin)
        ymin = info[i][1] - info[i][3];
      if (info[i][1] + info[i][3] > ymax)
        ymax = info[i][1] + info[i][3];
    }
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  //---- disegna gli Scitil.

  for (i = 0; i < nSciTilHits; i++) {

    disegnaSciTilHit(MACRO, i, posizSciTil[i][0], posizSciTil[i][1], 0);
  }
  //------------------------

  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, info[i][0], info[i][1], info[i][3], info[i][3], i, i);
    }
  }

  //-------------------------------   plotting all the tracks found

  for (i = 0; i < nTracksFoundSoFar; i++) {
    if (!keepit[i])
      continue;
    if (TypeConf[i]) {
      aaa = -0.5 * ALFA[i];
      bbb = -0.5 * BETA[i];
      //  Questa riga perche' so che la crf passa per origine
      rrr = sqrt(aaa * aaa + bbb * bbb);
      //       rrr = sqrt( aaa*aaa+bbb*bbb-GAMMA[i]);
      fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);

    } else {
      if (fabs(BETA[i]) < 1.e-10) {
        //         fprintf(MACRO,"TLine* ris%d = new TLine(%f,%f,%f,%f);\n",i,- GAMMA[i]/ALFA[i],ymin,- GAMMA[i]/ALFA[i],ymax);
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, 0., ymin, 0., ymax);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      } else {
        //        yl = -xmin*ALFA[i]/BETA[i] - GAMMA[i]/BETA[i];
        //         yu = -xmax*ALFA[i]/BETA[i] - GAMMA[i]/BETA[i];
        yl = -xmin * ALFA[i] / BETA[i];
        yu = -xmax * ALFA[i] / BETA[i];
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      }
    }

    if (istampa >= 3 && IVOLTE <= nmassimo) {
      cout << "stampa da WriteMacroParallelHitsGeneral , for trackfoundsofar n. " << i << ",  typconf " << TypeConf[i] << ",  nel normale piano XY;  alfa " << ALFA[i] << ",  beta "
           << BETA[i] << ", gamma " << GAMMA[i] << "  a cui corrisponde  Cx = " << -0.5 * ALFA[i] << ",  Cy = " << -0.5 * BETA[i]
           << " ed  R = "
           //       << sqrt( aaa*aaa+bbb*bbb-GAMMA[i]) <<endl;
           << sqrt(aaa * aaa + bbb * bbb) << endl;
    }
  }

  // -----------

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  //------------------------------------------------------------------------------------------------------------

  //---------- parallel straws Macro now con anche le tracce MC

  if (doMcComparison) {

    sprintf(nome, "MacroSttParwithMCEvent%d", IVOLTE);
    sprintf(nome2, "%s.C", nome);
    MACRO = fopen(nome2, "w");
    fprintf(MACRO, "void %s()\n{\n", nome);
    xmin = 1.e20;
    xmax = -1.e20;
    ymin = 1.e20;
    ymax = -1.e20;

    //--- SciTil  info
    for (i = 0; i < nSciTilHits; i++) {
      if (posizSciTil[i][0] < xmin)
        xmin = posizSciTil[i][0];
      if (posizSciTil[i][0] > xmax)
        xmax = posizSciTil[i][0];
      if (posizSciTil[i][1] < ymin)
        ymin = posizSciTil[i][1];
      if (posizSciTil[i][1] > ymax)
        ymax = posizSciTil[i][1];
    }
    //------

    for (i = 0; i < Nhits; i++) {
      if (info[i][5] == 1) { // parallel straws
        if (info[i][0] - info[i][3] < xmin)
          xmin = info[i][0] - info[i][3];
        if (info[i][0] + info[i][3] > xmax)
          xmax = info[i][0] + info[i][3];
        if (info[i][1] - info[i][3] < ymin)
          ymin = info[i][1] - info[i][3];
        if (info[i][1] + info[i][3] > ymax)
          ymax = info[i][1] + info[i][3];
      }
    }

    if (xmin > 0.)
      xmin = 0.;
    if (xmax < 0.)
      xmax = 0.;
    if (ymin > 0.)
      ymin = 0.;
    if (ymax < 0.)
      ymax = 0.;

    deltax = xmax - xmin;
    deltay = ymax - ymin;

    if (deltax > deltay) {
      ymin -= 0.5 * (deltax - deltay);
      ymax = ymin + deltax;
      delta = deltax;
    } else {
      xmin -= 0.5 * (deltay - deltax);
      xmax = xmin + deltay;
      delta = deltay;
    }

    xmax = xmax + delta * 0.15;
    xmin = xmin - delta * 0.15;

    ymax = ymax + delta * 0.15;
    ymin = ymin - delta * 0.15;

    fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

    disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

    //---- disegna gli Scitil.

    for (i = 0; i < nSciTilHits; i++) {
      /*
          fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
            i,posizSciTil[i][0],posizSciTil[i][1],30);
          fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
          fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
              ,i,i);
      */
      disegnaSciTilHit(MACRO, i, posizSciTil[i][0], posizSciTil[i][1], 0);
    }
    //------------------------

    for (i = 0; i < Nhits; i++) {
      if (info[i][5] == 1) { // parallel straws
        fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, info[i][0], info[i][1], info[i][3], info[i][3], i, i);
      }
    }

    //---------------------------  ora le tracce MC
    Int_t icode;
    Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica;
    PndMCTrack *pMC;
    for (int im = 0; im < nMCTracks; im++) {
      pMC = (PndMCTrack *)fMCTrackArray->At(im);
      if (!pMC)
        continue;
      icode = pMC->GetPdgCode();       //   PDG code of track
      Oxx = pMC->GetStartVertex().X(); //   X of starting point track
      Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) < 0.1)
        continue;
      Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
      Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
      fprintf(MACRO, "TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw();\n", im, Cx, Cy, Rr, Rr, im, im, im);
    } //  end of for(int im=0;im<nMCTracks; im++)

    //----------- fine parte del MC

    //-------------------------------   plotting all the tracks found

    for (i = 0; i < nTracksFoundSoFar; i++) {
      if (!keepit[i])
        continue;

      if (TypeConf[i]) {
        aaa = -0.5 * ALFA[i];
        bbb = -0.5 * BETA[i];
        rrr = sqrt(aaa * aaa + bbb * bbb - GAMMA[i]);
        fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
      } else {
        if (fabs(BETA[i]) < 1.e-10) {
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, -GAMMA[i] / ALFA[i], ymin, -GAMMA[i] / ALFA[i], ymax);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        } else {
          yl = -xmin * ALFA[i] / BETA[i] - GAMMA[i] / BETA[i];
          yu = -xmax * ALFA[i] / BETA[i] - GAMMA[i] / BETA[i];
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      }
    }

    // -----------

    fprintf(MACRO, "}\n");
    fclose(MACRO);

  } // end of  if(doMcComparison)
    //------------------------------------------------------------------------------------------------------------

  //   ora riplotto tutto nello spazio conforme usando la trasformazione u= x/(x**2+y**2) e   v = y/(x**2+y**2)
  //
  //   aggiungo anche la grigliatura dello spazio conforme usata per la box del pattern recognition

  sprintf(nome, "MacroSttParConformeEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //--- SciTil  info
  Double_t USciTil[nmaxSciTilHits], VSciTil[nmaxSciTilHits];
  for (i = 0; i < nSciTilHits; i++) {
    Double_t erre = posizSciTil[i][0] * posizSciTil[i][0] + posizSciTil[i][1] * posizSciTil[i][1];
    USciTil[i] = posizSciTil[i][0] / erre;
    VSciTil[i] = posizSciTil[i][1] / erre;
    if (USciTil[i] < xmin)
      xmin = USciTil[i];
    if (USciTil[i] > xmax)
      xmax = USciTil[i];
    if (VSciTil[i] < ymin)
      ymin = VSciTil[i];
    if (VSciTil[i] > ymax)
      ymax = VSciTil[i];
  }
  //------

  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
                           //   centro sfera in sistema conforme
      gamma = info[i][0] * info[i][0] + info[i][1] * info[i][1] - info[i][3] * info[i][3];
      Ox[i] = info[i][0] / gamma;
      Oy[i] = info[i][1] / gamma;
      Radius[i] = info[i][3] / gamma;
      if (Ox[i] - Radius[i] < xmin)
        xmin = Ox[i] - Radius[i];
      if (Ox[i] + Radius[i] > xmax)
        xmax = Ox[i] + Radius[i];
      if (Oy[i] - Radius[i] < ymin)
        ymin = Oy[i] - Radius[i];
      if (Oy[i] + Radius[i] > ymax)
        ymax = Oy[i] + Radius[i];
    }
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  // ora la grigliatura  con i cerchi

  fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", nRdivConformalEffective, 1. / RStrawDetectorMin,
          1. / RStrawDetectorMin, nRdivConformalEffective, nRdivConformalEffective);

  for (i = nRdivConformalEffective - 1; i >= 0; i--) {
    fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", i, radiaConf[i], radiaConf[i], i, i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti blu per delimitare la zona degli Stt.

  for (i = 0; i < nFidivConformal; i++) {
    ff = i * 2. * PI / nFidivConformal;
    x1 = cos(ff) / RStrawDetectorMax;
    y1 = sin(ff) / RStrawDetectorMax;
    x2 = cos(ff) / RStrawDetectorMin;
    y2 = sin(ff) / RStrawDetectorMin;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(4);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti magenta per comprendere la zona degli SciTil.

  for (i = 0; i < nFidivConformal; i++) {
    ff = i * 2. * PI / nFidivConformal;
    x1 = cos(ff) / RMAXSCITIL;
    y1 = sin(ff) / RMAXSCITIL;
    x2 = cos(ff) / RStrawDetectorMax;
    y2 = sin(ff) / RStrawDetectorMax;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(6);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->SetTitle(\"U    \");\n");
  fprintf(MACRO, "Assex->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->SetTitle(\"V    \");\n");
  fprintf(MACRO, "Assey->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assey->Draw();\n");

  // plot degli Hits Stt.
  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, Ox[i], Oy[i], Radius[i], Radius[i], i, i);
    }
  }

  // plot degli Hit SciTil
  for (i = 0; i < nSciTilHits; i++) {

    disegnaSciTilHit(MACRO, i, posizSciTil[i][0], posizSciTil[i][1],
                     2 // 2--> disegno SciTil in conforme.
    );
    /*
        fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
          i,USciTil[i],VSciTil[i],30);
        fprintf(MACRO,"SciT%d->SetMarkerSize(1.1);\n",i);
        fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
            ,i,i);
    */
  }

  //------------------   plotting all the tracks found

  for (i = 0; i < nTracksFoundSoFar; i++) {
    if (!keepit[i])
      continue;

    // cout<<" da writeparallelhitsgeneral, traccia found n. "<<i<<", typeconf "<<TypeConf[i]
    //  <<",  alfa "<<ALFA[i]<<",  beta  "<<BETA[i]<<",  gamma "<<GAMMA[i]<<endl;

    //  assumo che tutte le traiettorie siano rette nello spazio conforme, perche' le tracce vengono dal vertice primario

    if (TypeConf[i]) {

      if (fabs(BETA[i]) < 1.e-10) {
        if (fabs(ALFA[i]) < 1.e-10) {
          continue;
        } else {
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, -1. / ALFA[i], ymin, -1. / ALFA[i], ymax);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      } else {
        yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
        yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
        fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "ris%d->Draw();\n", i);
      }

    } else { // in this case TypConf = 0 -->  straight line in XY

      if (fabs(GAMMA[i]) > 1.e-10) {
        aaa = -0.5 * ALFA[i] / GAMMA[i];
        bbb = -0.5 * BETA[i] / GAMMA[i];
        rrr = sqrt(aaa * aaa + bbb * bbb);
        fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
      } else {
        if (fabs(BETA[i]) > 1.e-10) {
          yl = -xmin * ALFA[i] / BETA[i];
          yu = -xmax * ALFA[i] / BETA[i];
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        } else {
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, 0., ymin, 0., ymax);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      }
    }
  }

  // ------------------------------

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneral

//----------start of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneralConformalwithMC

void PndSttTrackFinderReal::WriteMacroParallelHitsGeneralConformalwithMC(bool *keepit, Int_t Nhits, Double_t info[][7], Int_t Nincl, Int_t Minclinations[],
                                                                         Double_t inclination[][3], Short_t nTracksFoundSoFar)
{

  Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

  Double_t xmin, xmax, ymin, ymax, xl, xu, yl, yu, rrr, Ox[nmaxHits], Oy[nmaxHits], Radius[nmaxHits], gamma, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor,
    ff, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, x1, x2, vx1, vy1, vz1, C0x1, C0y1, C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1,
    fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2, zl[200], zu[200];

  Double_t USciTil[nmaxSciTilHits], VSciTil[nmaxSciTilHits];

  //---------- parallel straws Macro now
  char nome[300], nome2[300];
  FILE *MACRO;

  //   ora riplotto tutto nello spazio conforme usando la trasformazione u= x/(x**2+y**2) e   v = y/(x**2+y**2)
  //
  //   aggiungo anche la grigliatura dello spazio conforme usata per la box del pattern recognition

  sprintf(nome, "MacroSttParConformewithMCEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //--- SciTil  info
  for (i = 0; i < nSciTilHits; i++) {
    Double_t erre = posizSciTil[i][0] * posizSciTil[i][0] + posizSciTil[i][1] * posizSciTil[i][1];
    USciTil[i] = posizSciTil[i][0] / erre;
    VSciTil[i] = posizSciTil[i][1] / erre;
    if (USciTil[i] < xmin)
      xmin = USciTil[i];
    if (USciTil[i] > xmax)
      xmax = USciTil[i];
    if (VSciTil[i] < ymin)
      ymin = VSciTil[i];
    if (VSciTil[i] > ymax)
      ymax = VSciTil[i];
  }
  //------

  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
                           //   centro sfera in sistema conforme
      gamma = info[i][0] * info[i][0] + info[i][1] * info[i][1] - info[i][3] * info[i][3];
      Ox[i] = info[i][0] / gamma;
      Oy[i] = info[i][1] / gamma;
      Radius[i] = info[i][3] / gamma;
      if (Ox[i] - Radius[i] < xmin)
        xmin = Ox[i] - Radius[i];
      if (Ox[i] + Radius[i] > xmax)
        xmax = Ox[i] + Radius[i];
      if (Oy[i] - Radius[i] < ymin)
        ymin = Oy[i] - Radius[i];
      if (Oy[i] + Radius[i] > ymax)
        ymax = Oy[i] + Radius[i];
    }
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  // ora la grigliatura  con i cerchi

  fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", nRdivConformalEffective, 1. / RStrawDetectorMin,
          1. / RStrawDetectorMin, nRdivConformalEffective, nRdivConformalEffective);

  for (i = nRdivConformalEffective - 1; i >= 0; i--) {
    fprintf(MACRO, "TEllipse* Griglia%d = new TEllipse(0.,0.,%f,%f,0.,360.);\nGriglia%d->SetLineColor(4);\nGriglia%d->Draw();\n", i, radiaConf[i], radiaConf[i], i, i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti blu per delimitare la zona degli Stt.

  for (i = 0; i < nFidivConformal; i++) {
    ff = i * 2. * PI / nFidivConformal;
    x1 = cos(ff) / RStrawDetectorMax;
    y1 = sin(ff) / RStrawDetectorMax;
    x2 = cos(ff) / RStrawDetectorMin;
    y2 = sin(ff) / RStrawDetectorMin;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(4);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------
  // ora la grigliatura  con i segmenti magenta per comprendere la zona degli SciTil.

  for (i = 0; i < nFidivConformal; i++) {
    ff = i * 2. * PI / nFidivConformal;
    x1 = cos(ff) / RMAXSCITIL;
    y1 = sin(ff) / RMAXSCITIL;
    x2 = cos(ff) / RStrawDetectorMax;
    y2 = sin(ff) / RStrawDetectorMax;
    fprintf(MACRO, "TLine* Seg%d = new TLine(%f,%f,%f,%f);\nSeg%d->SetLineColor(6);\nSeg%d->Draw();\n", i, x1, y1, x2, y2, i, i);
  }
  //---------------------

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->SetTitle(\"U    \");\n");
  fprintf(MACRO, "Assex->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->SetTitle(\"V    \");\n");
  fprintf(MACRO, "Assey->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assey->Draw();\n");

  // plot degli Hits Stt
  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetLineWidth(2);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, Ox[i], Oy[i], Radius[i], Radius[i], i,
              i, i);
    }
  }

  // plot degli Hit SciTil
  for (i = 0; i < nSciTilHits; i++) {

    disegnaSciTilHit(MACRO, i, posizSciTil[i][0], posizSciTil[i][1],
                     2 // 2--> disegno SciTil in conforme.
    );
    /*
        fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
          i,USciTil[i],VSciTil[i],30);
        fprintf(MACRO,"SciT%d->SetMarkerSize(1.1);\n",i);
        fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
            ,i,i);
    */
  }

  //------------------   plotting all the tracks found

  for (i = 0; i < nTracksFoundSoFar; i++) {
    if (!keepit[i])
      continue;

    if (TypeConf[i]) {

      if (fabs(GAMMA[i]) > 1.e-10) {
        aaa = -0.5 * ALFA[i] / GAMMA[i];
        bbb = -0.5 * BETA[i] / GAMMA[i];
        rrr = sqrt(aaa * aaa + bbb * bbb - 1. / GAMMA[i]);
        if (fabs(rrr / GAMMA[i]) < 30.) {
          fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
        } else {

          yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
          yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      } else {

        if (fabs(BETA[i]) < 1.e-10) {
          if (fabs(ALFA[i]) < 1.e-10) {
            continue;
          } else {
            fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, -1. / ALFA[i], ymin, -1. / ALFA[i], ymax);
            fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
            fprintf(MACRO, "ris%d->Draw();\n", i);
          }
        } else {
          yl = -xmin * ALFA[i] / BETA[i] - 1. / BETA[i];
          yu = -xmax * ALFA[i] / BETA[i] - 1. / BETA[i];
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      }

    } else { // in this case TypConf = 0 -->  straight line in XY

      if (fabs(GAMMA[i]) > 1.e-10) {
        aaa = -0.5 * ALFA[i] / GAMMA[i];
        bbb = -0.5 * BETA[i] / GAMMA[i];
        rrr = sqrt(aaa * aaa + bbb * bbb);
        fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
      } else {
        if (fabs(BETA[i]) > 1.e-10) {
          yl = -xmin * ALFA[i] / BETA[i];
          yu = -xmax * ALFA[i] / BETA[i];
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        } else {
          fprintf(MACRO, "TLine* ris%d = new TLine(%f,%f,%f,%f);\n", i, 0., ymin, 0., ymax);
          fprintf(MACRO, "ris%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "ris%d->Draw();\n", i);
        }
      }
    }
  }

  // ------------------------------

  //   plotting all the tracks MC generated

  Int_t icode;
  Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica;
  PndMCTrack *pMC;
  for (i = 0; i < nMCTracks; i++) {
    pMC = (PndMCTrack *)fMCTrackArray->At(i);
    if (!pMC)
      continue;
    icode = pMC->GetPdgCode();       //   PDG code of track
    Oxx = pMC->GetStartVertex().X(); //   X of starting point track
    Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
    Px = pMC->GetMomentum().X();
    Py = pMC->GetMomentum().Y();
    aaa = sqrt(Px * Px + Py * Py);
    Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
    if (icode > 1000000000)
      carica = 1.;
    else
      carica = fParticle->Charge() / 3.; //   charge of track
    if (fabs(carica) < 0.1)
      continue;
    Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
    Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
    gamma = -Rr * Rr + Cx * Cx + Cy * Cy;
    if (fabs(gamma) < 0.001) {
      if (Cy != 0.) {
        yl = xmin * (-Cx / Cy) + 0.5 / Cy;
        yu = xmax * (-Cx / Cy) + 0.5 / Cy;
        xl = xmin;
        xu = xmax;
      } else {
        yl = ymin;
        yu = ymax;
        xu = xl = 0.5 / Cx;
      }
      fprintf(MACRO, "TLine* MCris%d = new TLine(%f,%f,%f,%f);\n", i, xl, yl, xu, yu);
      fprintf(MACRO, "MCris%d->SetLineStyle(2);\n", i);
      fprintf(MACRO, "MCris%d->SetLineColor(3);\n", i);
      fprintf(MACRO, "MCris%d->SetLineWidth(1);\n", i);
      fprintf(MACRO, "MCris%d->Draw();\n", i);

    } else {
      if (fabs(Rr / gamma) > 1.) {
        if (fabs(Cy) > 0.001) {
          yl = -xmin * Cx / Cy + 0.5 / Cy;
          yu = -xmax * Cx / Cy + 0.5 / Cy;
          fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "MCline%d->SetLineColor(3);\n", i);
          fprintf(MACRO, "MCline%d->Draw();\n", i);
        } else {
          fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, 2. * CxMC[i], ymin, 2. * CxMC[i], ymax);
          fprintf(MACRO, "MCline%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "MCline%d->Draw();\n", i);
        }
      } else {
        fprintf(MACRO, "TEllipse* MCcerchio%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMCcerchio%d->SetLineColor(3);\n", i, Cx / gamma, Cy / gamma, Rr / fabs(gamma), Rr / fabs(gamma),
                i);

        fprintf(MACRO, "MCcerchio%d->SetFillStyle(0);\nMCcerchio%d->SetLineStyle(2);\nMCcerchio%d->SetLineWidth(1);\nMCcerchio%d->Draw();\n", i, i, i, i);
      }
    }
  } // end of for(i=0;i<nMCTracks; i++)

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneralConformalwithMC

//----------start of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneralConformalwithMCspecial

void PndSttTrackFinderReal::WriteMacroParallelHitsConformalwithMCspecial(Int_t Nhits,
                                                                         //                   Short_t iExclude,
                                                                         Double_t auxinfoparalConformal[][5], Short_t nTracksFoundSoFar,
                                                                         //                   Double_t * ALFA,
                                                                         //                   Double_t * BETA,
                                                                         //                   Double_t * GAMMA,
                                                                         Short_t Status, Double_t *trajectory_vertex)
{

  Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

  Double_t xmin, xmax, ymin, ymax, xl, xu, yl, yu, rrr, cx, cy, Ox[nmaxHits], Oy[nmaxHits], Radius[nmaxHits], alfa, beta, gamma, dx, dy, diff, d1, d2, delta, deltax, deltay,
    deltaz, deltaS, factor, ff, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, x1, x2, vx1, vy1, vz1, C0x1, C0y1, C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL,
    Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2, zl[200], zu[200];

  //---------- parallel straws Macro now
  char nome[300], nome2[300];
  FILE *MACRO;

  //   ora riplotto tutto nello spazio conforme usando la trasformazione u= x/(x**2+y**2) e   v = y/(x**2+y**2)
  //

  sprintf(nome, "MacroSttParConformewithMCspecialEvent%dTrack%d", IVOLTE, nTracksFoundSoFar);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;
  for (i = 0; i < Nhits; i++) {
    //             if( i== iExclude)  continue;
    //   centro sfera in sistema conforme
    Ox[i] = auxinfoparalConformal[i][0];
    Oy[i] = auxinfoparalConformal[i][1];
    Radius[i] = auxinfoparalConformal[i][2];
    if (Ox[i] - Radius[i] < xmin)
      xmin = Ox[i] - Radius[i];
    if (Ox[i] + Radius[i] > xmax)
      xmax = Ox[i] + Radius[i];
    if (Oy[i] - Radius[i] < ymin)
      ymin = Oy[i] - Radius[i];
    if (Oy[i] + Radius[i] > ymax)
      ymax = Oy[i] + Radius[i];
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->Draw();\n");

  for (i = 0; i < Nhits; i++) {
    //         if( i== iExclude)  continue;
    fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetLineWidth(2);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, Ox[i], Oy[i], Radius[i], Radius[i], i, i,
            i);
  }

  //------------------   plotting the track found

  i = nTracksFoundSoFar;
  if (Status != 99) {

    rrr = sqrt(0.25 * ALFA[nTracksFoundSoFar] * ALFA[nTracksFoundSoFar] + 0.25 * BETA[nTracksFoundSoFar] * BETA[nTracksFoundSoFar] - GAMMA[nTracksFoundSoFar]);
    alfa = ALFA[nTracksFoundSoFar] + 2. * trajectory_vertex[0];
    beta = BETA[nTracksFoundSoFar] + 2. * trajectory_vertex[1];
    cx = -0.5 * alfa;
    cy = -0.5 * beta;
    gamma = cx * cx + cy * cy - rrr * rrr;

    if (fabs(gamma) > 1.e-8) {
      fprintf(MACRO, "TEllipse* FoundTrack%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nFoundTrack%d->SetLineColor(2);\n", i, cx / gamma, cy / gamma, rrr / fabs(gamma),
              rrr / fabs(gamma), i);
      fprintf(MACRO, "FoundTrack%d->SetFillStyle(0);\nFoundTrack%d->SetLineStyle(2);\nFoundTrack%d->SetLineWidth(1);\nFoundTrack%d->Draw();\n", i, i, i, i);
    } else {
      if (fabs(beta) > 1.e-8) {
        yl = -xmin * alfa / beta - 1. / beta;
        yu = -xmax * alfa / beta - 1. / beta;
        fprintf(MACRO, "TLine* FoundTrack%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
        fprintf(MACRO, "FoundTrack%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "FoundTrack%d->Draw();\n", i);
      } else {
        fprintf(MACRO, "TLine* FoundTrack%d = new TLine(%f,%f,%f,%f);\n", i, -1. / alfa, ymin, -1. / alfa, ymax);
        fprintf(MACRO, "FoundTrack%d->SetLineColor(2);\n", i);
        fprintf(MACRO, "FoundTrack%d->Draw();\n", i);
      }
    }

  } else {

    alfa = ALFA[nTracksFoundSoFar];
    beta = BETA[nTracksFoundSoFar];
    gamma = GAMMA[nTracksFoundSoFar] + ALFA[nTracksFoundSoFar] * trajectory_vertex[0] + BETA[nTracksFoundSoFar] * trajectory_vertex[1];

    if (fabs(beta) > 1.e-8) {
      yl = -xmin * alfa / beta - 1. / beta;
      yu = -xmax * alfa / beta - 1. / beta;
      fprintf(MACRO, "TLine* FoundTrack%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
      fprintf(MACRO, "FoundTrack%d->SetLineColor(2);\n", i);
      fprintf(MACRO, "FoundTrack%d->Draw();\n", i);
    } else {
      fprintf(MACRO, "TLine* FoundTrack%d = new TLine(%f,%f,%f,%f);\n", i, -1. / alfa, ymin, -1. / alfa, ymax);
      fprintf(MACRO, "FoundTrack%d->SetLineColor(2);\n", i);
      fprintf(MACRO, "FoundTrack%d->Draw();\n", i);
    }
  }

  // ------------------------------

  //   plotting all the tracks MC generated

  // cout<<" TRASLAZIONE IN "<<trajectory_vertex[0]<<",  "<<trajectory_vertex[1]<<endl;

  Int_t icode;
  Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica;
  PndMCTrack *pMC;
  for (i = 0; i < nMCTracks; i++) {
    pMC = (PndMCTrack *)fMCTrackArray->At(i);
    if (!pMC)
      continue;
    icode = pMC->GetPdgCode();       //   PDG code of track
    Oxx = pMC->GetStartVertex().X(); //   X of starting point track
    Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
    Px = pMC->GetMomentum().X();
    Py = pMC->GetMomentum().Y();
    aaa = sqrt(Px * Px + Py * Py);
    Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
    if (icode > 1000000000)
      carica = 1.;
    else
      carica = fParticle->Charge() / 3.; //   charge of track
    if (fabs(carica) < 0.1)
      continue;
    Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
    Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);

    //    for(i=0; i<nMCTracks; i++){
    cx = Cx - trajectory_vertex[0];
    cy = Cy - trajectory_vertex[1];
    gamma = -Rr * Rr + (cx * cx + cy * cy);

    if (fabs(gamma) < 0.001) {

      if (cy != 0.) {
        yl = xmin * (-cx / cy) + 0.5 / cy;
        yu = xmax * (-cx / cy) + 0.5 / cy;
        xl = xmin;
        xu = xmax;
      } else {
        yl = ymin;
        yu = ymax;
        xu = xl = 0.5 / cx;
      }
      fprintf(MACRO, "TLine* MCris%d = new TLine(%f,%f,%f,%f);\n", i, xl, yl, xu, yu);
      fprintf(MACRO, "MCris%d->SetLineStyle(2);\n", i);
      fprintf(MACRO, "MCris%d->SetLineColor(3);\n", i);
      fprintf(MACRO, "MCris%d->SetLineWidth(1);\n", i);
      fprintf(MACRO, "MCris%d->Draw();\n", i);

    } else {

      if (fabs(Rr / gamma) > 1.) {
        if (fabs(Cy) > 0.001) {
          yl = -xmin * Cx / Cy + 0.5 / Cy;
          yu = -xmax * Cx / Cy + 0.5 / Cy;
          fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, xmin, yl, xmax, yu);
          fprintf(MACRO, "MCline%d->SetLineColor(3);\n", i);
          fprintf(MACRO, "MCline%d->Draw();\n", i);
        } else {
          fprintf(MACRO, "TLine* MCline%d = new TLine(%f,%f,%f,%f);\n", i, 2. * CxMC[i], ymin, 2. * CxMC[i], ymax);
          fprintf(MACRO, "MCline%d->SetLineColor(2);\n", i);
          fprintf(MACRO, "MCline%d->Draw();\n", i);
        }
      } else {

        fprintf(MACRO, "TEllipse* MCcerchio%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMCcerchio%d->SetLineColor(3);\n", i, cx / gamma, cy / gamma, Rr / fabs(gamma), Rr / fabs(gamma),
                i);
        fprintf(MACRO, "MCcerchio%d->SetFillStyle(0);\nMCcerchio%d->SetLineStyle(2);\nMCcerchio%d->SetLineWidth(1);\nMCcerchio%d->Draw();\n", i, i, i, i);
      }
    }
  }

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndSttTrackFinderReal::WriteMacroParallelHitsGeneralConformalwithMCspecial

//----------start of function PndSttTrackFinderReal::WriteMacroParallelAssociatedHits

void PndSttTrackFinderReal::WriteMacroParallelAssociatedHits(Double_t Ox, Double_t Oy, Double_t R, Short_t Nhits, Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack],
                                                             Double_t info[][7], Int_t Nincl, Int_t Minclinations[], Double_t inclination[][3], Short_t imaxima, Int_t sequencial,
                                                             Short_t nscitilhitsintrack, Short_t *listscitilhitsintrack)
{

  Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

  Double_t xmin, xmax, ymin, ymax, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, vx1, vy1, vz1, C0x1, C0y1,
    C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2,
    Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  //    Ox = (D+R)*cos(Fi);
  //    Oy = (D+R)*sin(Fi);

  // cout<<"da MacroTrackparalleletc. Ox, Oy "<<Ox<<",  "<<Oy<<endl;

  //---------- parallel straws Macro now
  char nome[300], nome2[300];
  sprintf(nome, "MacroSttParEvent%dT%d", IVOLTE, sequencial);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //---- Scitil hits.
  if (nscitilhitsintrack > 0) {
    for (j = 0; j < nscitilhitsintrack; j++) {
      i = listscitilhitsintrack[j];
      if (posizSciTil[i][0] < xmin)
        xmin = posizSciTil[i][0];
      if (posizSciTil[i][0] > xmax)
        xmax = posizSciTil[i][0];
      if (posizSciTil[i][1] < ymin)
        ymin = posizSciTil[i][1];
      if (posizSciTil[i][1] > ymax)
        ymax = posizSciTil[i][1];
    }
  }
  //-------------

  for (ii = 0; ii < Nhits; ii++) {
    i = infoparal[ListHitsinTrack[imaxima][ii]];
    if (info[i][0] - info[i][3] < xmin)
      xmin = info[i][0] - info[i][3];
    if (info[i][0] + info[i][3] > xmax)
      xmax = info[i][0] + info[i][3];
    if (info[i][1] - info[i][3] < ymin)
      ymin = info[i][1] - info[i][3];
    if (info[i][1] + info[i][3] > ymax)
      ymax = info[i][1] + info[i][3];
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  fprintf(MACRO, "TEllipse* TC = new TEllipse(%f,%f,%f,%f,0.,360.);\n", Ox, Oy, R, R);
  fprintf(MACRO, "TC->SetLineColor(2);\nTC->SetFillStyle(0);\nTC->Draw();\n");

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  for (ii = 0; ii < Nhits; ii++) {
    i = infoparal[ListHitsinTrack[imaxima][ii]];
    fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, info[i][0], info[i][1], info[i][3], info[i][3], i, i);
  }

  //---- disegna gli Scitil.

  if (nscitilhitsintrack > 0) {
    for (j = 0; j < nscitilhitsintrack; j++) {
      i = listscitilhitsintrack[j];
      /*
          fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
            i,posizSciTil[i][0],posizSciTil[i][1],30);
          fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
          fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
              ,i,i);
      */
      disegnaSciTilHit(MACRO, i, posizSciTil[i][0], posizSciTil[i][1], 0);
    }
  }
  //------------------------

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndSttTrackFinderReal::WriteMacroParallelAssociatedHits

//----------start of function PndSttTrackFinderReal::WriteMacroParallelAssociatedHitswithMC

void PndSttTrackFinderReal::WriteMacroParallelAssociatedHitswithMC(Double_t Ox, Double_t Oy, Double_t R, Short_t TrackFoundaTrackMC, Short_t Nhits,
                                                                   Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Double_t info[][7], Short_t ifoundtrack,
                                                                   Int_t sequentialNTrack, Short_t nscitilhitsintrack, Short_t *listscitilhitsintrack,
                                                                   Short_t nParalCommon[MAXTRACKSPEREVENT], Short_t ParalCommonList[MAXTRACKSPEREVENT][nmaxHits],
                                                                   Short_t nSpuriParinTrack[MAXTRACKSPEREVENT], Short_t ParSpuriList[MAXTRACKSPEREVENT][nmaxHits],
                                                                   Short_t nMCParalAlone[MAXTRACKSPEREVENT], Short_t MCParalAloneList[MAXTRACKSPEREVENT][nmaxHits])
{

  Int_t i, j, i1, ii, index, imaxima, Kincl, nlow, nup, STATUS;

  Double_t xmin, xmax, ymin, ymax, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, vx1, vy1, vz1, C0x1, C0y1,
    C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2,
    Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  imaxima = ifoundtrack;

  //---------- parallel straws Macro now
  char nome[300], nome2[300];
  sprintf(nome, "MacroSttParwithMCEvent%dT%d", IVOLTE, sequentialNTrack);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;

  //---- Scitil hits.
  if (nscitilhitsintrack > 0) {
    for (j = 0; j < nscitilhitsintrack; j++) {
      i = listscitilhitsintrack[j];
      if (posizSciTil[i][0] < xmin)
        xmin = posizSciTil[i][0];
      if (posizSciTil[i][0] > xmax)
        xmax = posizSciTil[i][0];
      if (posizSciTil[i][1] < ymin)
        ymin = posizSciTil[i][1];
      if (posizSciTil[i][1] > ymax)
        ymax = posizSciTil[i][1];
    }
  }
  //-------------

  for (ii = 0; ii < Nhits; ii++) {
    i = infoparal[ListHitsinTrack[imaxima][ii]];
    if (info[i][0] - info[i][3] < xmin)
      xmin = info[i][0] - info[i][3];
    if (info[i][0] + info[i][3] > xmax)
      xmax = info[i][0] + info[i][3];
    if (info[i][1] - info[i][3] < ymin)
      ymin = info[i][1] - info[i][3];
    if (info[i][1] + info[i][3] > ymax)
      ymax = info[i][1] + info[i][3];
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  fprintf(MACRO, "TEllipse* TC = new TEllipse(%f,%f,%f,%f,0.,360.);\n", Ox, Oy, R, R);
  fprintf(MACRO, "TC->SetLineColor(2);\nTC->SetFillStyle(0);\nTC->Draw();\n");

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  for (ii = 0; ii < nParalCommon[ifoundtrack]; ii++) {
    i = ParalCommonList[ifoundtrack][ii];
    fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);", i, info[i][0], info[i][1], info[i][3], info[i][3], i);
    fprintf(MACRO, "E%d->Draw();\n", i);
  }

  for (ii = 0; ii < nSpuriParinTrack[ifoundtrack]; ii++) {
    i = ParSpuriList[ifoundtrack][ii];
    fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);", i, info[i][0], info[i][1], info[i][3], info[i][3], i);
    fprintf(MACRO, "E%d->SetLineColor(2);\n", i);
    fprintf(MACRO, "E%d->Draw();\n", i);
  }

  for (ii = 0; ii < nMCParalAlone[ifoundtrack]; ii++) {
    i = MCParalAloneList[ifoundtrack][ii];
    fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);", i, info[i][0], info[i][1], info[i][3], info[i][3], i);
    fprintf(MACRO, "E%d->SetLineColor(4);\n", i);
    fprintf(MACRO, "E%d->Draw();\n", i);
  }

  //---- disegna gli Scitil.

  if (nscitilhitsintrack > 0) {
    for (j = 0; j < nscitilhitsintrack; j++) {
      i = listscitilhitsintrack[j];
      /*
          fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
            i,posizSciTil[i][0],posizSciTil[i][1],30);
          fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",i);
          fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
              ,i,i);
      */
      disegnaSciTilHit(MACRO, i, posizSciTil[i][0], posizSciTil[i][1], 0);
    }
  }
  //------------------------

  //---------------------------  ora le tracce MC
  Int_t icode, im;
  Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica;
  PndMCTrack *pMC;
  if (TrackFoundaTrackMC > -1) {
    im = TrackFoundaTrackMC;
    pMC = (PndMCTrack *)fMCTrackArray->At(im);
    if (pMC) {
      icode = pMC->GetPdgCode();       //   PDG code of track
      Oxx = pMC->GetStartVertex().X(); //   X of starting point track
      Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) >= 0.1) {
        Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
        Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
        fprintf(MACRO, "TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw();\n", im, Cx, Cy, Rr, Rr, im, im, im);
      } // end of if( fabs(carica)>=0.1)
    }   //  end of if ( pMC )

  } //  end of if( TrackFoundaTrackMC > -1){

  //----------- fine parte del MC

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  return;
}

//----------end of function PndSttTrackFinderReal::WriteMacroParallelAssociatedHitswithMC

//----------start of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHits

void PndSttTrackFinderReal::WriteMacroSkewAssociatedHits(bool goodskewfit, Double_t KAPPA, Double_t FI0, Double_t D, Double_t Fi, Double_t R, Double_t info[][7], Int_t Nincl,
                                                         Int_t Minclinations[], Double_t inclination[][3], Int_t imaxima, Int_t sequentialNTrack, Short_t nSttSkewhitinTrack,
                                                         Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t nscitilhits, Double_t *ESSE, Double_t *ZETA)

{

  Int_t i, j, i1, ii, iii, index, Kincl, nlow, nup, STATUS, imc, Nmin, Nmax;

  Double_t xmin, xmax, ymin, ymax, Ox, Oy, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, vx1, vy1, vz1, C0x1,
    C0y1, C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2,
    Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  //-------------------  skew straws hits Macro now

  char nome2[300], nome[300];
  FILE *MACRO;
  sprintf(nome, "MacroSttSkewEvent%dT%d", IVOLTE, sequentialNTrack);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  // KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

  Smin = zmin = 1.e10;
  Smax = zmax = -zmin;
  index = 0;

  // prima lo  hits SciTil
  for (i = 0; i < nscitilhits; i++) {
    if (ESSE[i] > Smax)
      Smax = ESSE[i];
    if (ESSE[i] < Smin)
      Smin = ESSE[i];
    if (ZETA[i] > zmax)
      zmax = ZETA[i];
    if (ZETA[i] < zmin)
      zmin = ZETA[i];
  }
  //-------------------------

  for (iii = 0; iii < nSttSkewhitinTrack; iii++) {
    i = infoskew[ListSkewHitsinTrack[imaxima][iii]];

    Kincl = (int)info[i][5] - 1;

    aaa = sqrt(inclination[Kincl][0] * inclination[Kincl][0] + inclination[Kincl][1] * inclination[Kincl][1] + inclination[Kincl][2] * inclination[Kincl][2]);
    vx1 = inclination[Kincl][0] / aaa;
    vy1 = inclination[Kincl][1] / aaa;
    vz1 = inclination[Kincl][2] / aaa;
    C0x1 = info[i][0];
    C0y1 = info[i][1];
    C0z1 = info[i][2];
    Ox = (R + D) * cos(Fi);
    Oy = (R + D) * sin(Fi);

    calculateintersections(Ox, Oy, R, C0x1, C0y1, C0z1, info[i][3], vx1, vy1, vz1, &STATUS, POINTS1);

    if (STATUS < 0)
      continue;

    for (ii = 0; ii < 2; ii++) {
      j = 3 * ii;
      distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

      Rx = POINTS1[j] - Ox;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= R;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
      if (bbb > 1.e-10) {
        Tiltdirection1[0] = vz1 / bbb;
        Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
      } else {
        Tiltdirection1[0] = 1.;
        Tiltdirection1[1] = 0.;
      }

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;
      Aellipsis1 = info[i][3] * aaa / LL;

      Bellipsis1 = info[i][3] / R;

      if (distance >= info[i][4] + Aellipsis1)
        continue;

      // checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

      //--------------------------

      fi1 = atan2(POINTS1[j + 1] - Oy, POINTS1[j] - Ox); // atan2 returns radians in (-pi and +pi]
      if (fi1 < 0.)
        fi1 += 2. * PI;

      if (zmin > POINTS1[j + 2] - Aellipsis1)
        zmin = POINTS1[j + 2] - Aellipsis1;
      if (zmax < POINTS1[j + 2] + Aellipsis1)
        zmax = POINTS1[j + 2] + Aellipsis1;

      if (Smin > fi1 - Bellipsis1)
        Smin = fi1 - Bellipsis1;
      if (Smax < fi1 + Bellipsis1)
        Smax = fi1 + Bellipsis1;

      Double_t rotation1 = 180. * atan2(Tiltdirection1[1], Tiltdirection1[0]) / PI;
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nE%d->SetFillStyle(0);\n", index, POINTS1[j + 2], fi1, Aellipsis1, Bellipsis1, rotation1, index);

      // ------ se lo hit e' spurio marcalo in rosso

      index++;

    } //  end of    for( ii=0; ii<2; ii++)

  } //   end of  for( i=1; i< Nhits; i++)

  if (!(index + nscitilhits == 0 || zmax < zmin || Smax < Smin)) {
    aaa = Smax - Smin;
    Smin -= aaa * 1.;
    Smax += aaa * 1.;

    aaa = zmax - zmin;
    zmin -= aaa * 0.05;
    zmax += aaa * 0.05;

    if (Smax > 2. * PI)
      Smax = 2. * PI;
    if (Smin < 0.)
      Smin = 0.;

    //  Smin=0.;
    //  Smax=2.*PI;

    fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", zmin, Smin, zmax, Smax);
    for (ii = 0; ii < index; ii++) {
      fprintf(MACRO, "E%d->Draw();\n", ii);
    }

    deltaz = zmax - zmin;
    deltaS = Smax - Smin;
    fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Smin + 0.05 * deltaS, zmax - 0.05 * deltaz, Smin + 0.05 * deltaS,
            zmin + 0.05 * deltaz, zmax - 0.05 * deltaz);
    fprintf(MACRO, "Assex->Draw();\n");
    fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Smin + 0.05 * deltaS, zmin + 0.05 * deltaz, Smax - 0.05 * deltaS,
            Smin + 0.05 * deltaS, Smax - 0.05 * deltaS);
    fprintf(MACRO, "Assey->Draw();\n");

    //------------
    //  plot di eventuali hits  SciTil;
    for (i = 0; i < nscitilhits; i++) {
      //		fprintf(MACRO,"SciT%d->Draw();\n",i);

      disegnaSciTilHit(MACRO, i, ZETA[i], ESSE[i], 1);
    }
    //------------

    // --------------------------------

    //  plot della traccia trovata dal finder
    if (goodskewfit) {

      //  if ( fabs(KAPPA) > 1.e-10 && fabs(KAPPA) < 1.e10  ) {

      if (fabs(KAPPA) <= 1.e-10 || fabs(KAPPA) >= 1.e10) {
        cout << "PndSttTrackFinderReal::WriteMacroSkewAssociatedHits,"
             << " this track found by PR not plotted"
             << "\n\t because KAPPA = " << KAPPA << endl;
      } else {

        fmin = KAPPA * zmin + FI0;
        fmax = KAPPA * zmax + FI0;

        if (KAPPA >= 0.) {
          fmin = KAPPA * zmin + FI0;
          fmax = KAPPA * zmax + FI0;
        } else {
          fmax = KAPPA * zmin + FI0;
          fmin = KAPPA * zmax + FI0;
        }

        if (fmax >= 0.) {
          Nmax = (int)(0.5 * fmax / PI);
        } else {
          Nmax = ((int)(0.5 * fmax / PI)) - 1;
        }
        if (fmin >= 0.) {
          Nmin = (int)(0.5 * fmin / PI);
        } else {
          Nmin = ((int)(0.5 * fmin / PI)) - 1;
        }
        for (i = Nmin; i <= Nmax; i++) {
          offset = 2. * PI * i;
          z1 = (i * 2. * PI - FI0) / KAPPA;
          z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
          fprintf(MACRO, "TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n", i - Nmin, z1, 0., z2, 2. * PI, i - Nmin, i - Nmin);

        } //  end of  for(i=Nmin; i<= Nmax;++)

      } // end of   if ( fabs(KAPPA) <= 1.e-10 || fabs(KAPPA) >= 1.e10  )
    }   // end of if(goodskewfit)
  }     // end of  if(!(index+nscitilhits==0||zmax < zmin||Smax < Smin))

  fprintf(MACRO, "}\n");
  fclose(MACRO);
}

//----------end of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHits

//----------start of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithMC

void PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithMC(bool goodskewfit, Double_t KAPPA, Double_t FI0, Double_t D, Double_t Fi, Double_t R, Double_t info[][7], Int_t Nincl,
                                                               Int_t Minclinations[], Double_t inclination[][3], Int_t imaxima, Int_t sequentialNTrack, Short_t nSttSkewhitinTrack,
                                                               Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t nSkewCommon,
                                                               Short_t SkewCommonList[MAXTRACKSPEREVENT][nmaxHits], Short_t daTrackFoundaTrackMC,
                                                               Short_t nMCSkewAlone[MAXTRACKSPEREVENT], Short_t MCSkewAloneList[MAXTRACKSPEREVENT][nmaxHits], Short_t nscitilhits,
                                                               Double_t *ESSE, Double_t *ZETA)
{

  bool flaggo;
  Int_t i, j, i1, ii, iii, index, Kincl, nlow, nup, STATUS, imc, Nmin, Nmax;

  Short_t Lista[nmaxHits];

  Double_t xmin, xmax, ymin, ymax, Ox, Oy, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, vx1, vy1, vz1, C0x1,
    C0y1, C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2,
    Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  TDatabasePDG *fdbPDG;
  TParticlePDG *fParticle;

  //-------------------  skew straws hits Macro now

  char nome2[300], nome[300];
  FILE *MACRO;
  sprintf(nome, "MacroSttSkewwithMCEvent%dT%d", IVOLTE, sequentialNTrack);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  // KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

  Smin = zmin = 1.e10;
  Smax = zmax = -zmin;
  index = 0;

  // prima lo (gli) hits SciTil
  for (i = 0; i < nscitilhits; i++) {
    if (ESSE[i] > Smax)
      Smax = ESSE[i];
    if (ESSE[i] < Smin)
      Smin = ESSE[i];
    if (ZETA[i] > zmax)
      zmax = ZETA[i];
    if (ZETA[i] < zmin)
      zmin = ZETA[i];
  }
  //----------------------------
  for (iii = 0; iii < nSttSkewhitinTrack; iii++) {
    i = infoskew[ListSkewHitsinTrack[imaxima][iii]];

    Kincl = (int)info[i][5] - 1;

    aaa = sqrt(inclination[Kincl][0] * inclination[Kincl][0] + inclination[Kincl][1] * inclination[Kincl][1] + inclination[Kincl][2] * inclination[Kincl][2]);
    vx1 = inclination[Kincl][0] / aaa;
    vy1 = inclination[Kincl][1] / aaa;
    vz1 = inclination[Kincl][2] / aaa;
    C0x1 = info[i][0];
    C0y1 = info[i][1];
    C0z1 = info[i][2];
    Ox = (R + D) * cos(Fi);
    Oy = (R + D) * sin(Fi);

    calculateintersections(Ox, Oy, R, C0x1, C0y1, C0z1, info[i][3], vx1, vy1, vz1, &STATUS, POINTS1);

    if (STATUS < 0)
      continue;

    for (ii = 0; ii < 2; ii++) {
      j = 3 * ii;
      distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

      Rx = POINTS1[j] - Ox;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= R;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
      if (bbb > 1.e-10) {
        Tiltdirection1[0] = vz1 / bbb;
        Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
      } else {
        Tiltdirection1[0] = 1.;
        Tiltdirection1[1] = 0.;
      }

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;
      Aellipsis1 = info[i][3] * aaa / LL;

      Bellipsis1 = info[i][3] / R;

      if (distance >= info[i][4] + Aellipsis1)
        continue;

      // checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

      //--------------------------

      fi1 = atan2(POINTS1[j + 1] - Oy, POINTS1[j] - Ox); // atan2 returns radians in (-pi and +pi]
      if (fi1 < 0.)
        fi1 += 2. * PI;

      if (zmin > POINTS1[j + 2] - Aellipsis1)
        zmin = POINTS1[j + 2] - Aellipsis1;
      if (zmax < POINTS1[j + 2] + Aellipsis1)
        zmax = POINTS1[j + 2] + Aellipsis1;

      if (Smin > fi1 - Bellipsis1)
        Smin = fi1 - Bellipsis1;
      if (Smax < fi1 + Bellipsis1)
        Smax = fi1 + Bellipsis1;

      Lista[index] = i + ii * 10000; // do la possibilita' di plottare 2 hits skew che vengono dalla
                                     // stessa skew straw.

      Double_t rotation1 = 180. * atan2(Tiltdirection1[1], Tiltdirection1[0]) / PI;
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nE%d->SetFillStyle(0);\n", i + ii * 10000, POINTS1[j + 2], fi1, Aellipsis1, Bellipsis1, rotation1,
              i + ii * 10000);
      //                     index,POINTS1[j+2],fi1,Aellipsis1,Bellipsis1,rotation1,index);

      // ------ se lo hit e' spurio marcalo in rosso
      flaggo = true;
      for (i1 = 0; i1 < nSkewCommon; i1++) {
        if (SkewCommonList[imaxima][i1] == i) {
          flaggo = false;
          break;
        }
      }
      if (flaggo)
        fprintf(MACRO, "E%d->SetLineColor(2);\n", i + ii * 10000);

      index++;

    } //  end of    for( ii=0; ii<2; ii++)

  } //   end of  for( iii=0; iii< nSttSkewhitinTrack; iii++)

  //-------------------------------

  //------ aggiungo in blu eventuali punti della traccia MC che sono non mecciati

  for (iii = 0; iii < nMCSkewAlone[imaxima]; iii++) {
    i = MCSkewAloneList[imaxima][iii];

    Kincl = (int)info[i][5] - 1;

    aaa = sqrt(inclination[Kincl][0] * inclination[Kincl][0] + inclination[Kincl][1] * inclination[Kincl][1] + inclination[Kincl][2] * inclination[Kincl][2]);
    vx1 = inclination[Kincl][0] / aaa;
    vy1 = inclination[Kincl][1] / aaa;
    vz1 = inclination[Kincl][2] / aaa;
    C0x1 = info[i][0];
    C0y1 = info[i][1];
    C0z1 = info[i][2];
    Ox = (R + D) * cos(Fi);
    Oy = (R + D) * sin(Fi);

    calculateintersections(Ox, Oy, R, C0x1, C0y1, C0z1, info[i][3], vx1, vy1, vz1, &STATUS, POINTS1);

    if (STATUS < 0)
      continue;

    for (ii = 0; ii < 2; ii++) {
      j = 3 * ii;
      distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

      Rx = POINTS1[j] - Ox;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= R;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
      if (bbb > 1.e-10) {
        Tiltdirection1[0] = vz1 / bbb;
        Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
      } else {
        Tiltdirection1[0] = 1.;
        Tiltdirection1[1] = 0.;
      }

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;
      Aellipsis1 = info[i][3] * aaa / LL;

      Bellipsis1 = info[i][3] / R;

      if (distance >= info[i][4] + Aellipsis1)
        continue;

      //--------------------------

      fi1 = atan2(POINTS1[j + 1] - Oy, POINTS1[j] - Ox); // atan2 returns radians in (-pi and +pi]
      if (fi1 < 0.)
        fi1 += 2. * PI;

      if (zmin > POINTS1[j + 2] - Aellipsis1)
        zmin = POINTS1[j + 2] - Aellipsis1;
      if (zmax < POINTS1[j + 2] + Aellipsis1)
        zmax = POINTS1[j + 2] + Aellipsis1;

      if (Smin > fi1 - Bellipsis1)
        Smin = fi1 - Bellipsis1;
      if (Smax < fi1 + Bellipsis1)
        Smax = fi1 + Bellipsis1;

      Double_t rotation1 = 180. * atan2(Tiltdirection1[1], Tiltdirection1[0]) / PI;
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nE%d->SetFillStyle(0);\n", i + ii * 10000, POINTS1[j + 2], fi1, Aellipsis1, Bellipsis1, rotation1,
              i + ii * 10000);

      // ------  marca lo hit in blu
      fprintf(MACRO, "E%d->SetLineColor(4);\n", i + ii * 10000);
      Lista[index] = i + ii * 10000; // do la possibilita' di plottare 2 hits skew che vengono dalla
                                     // stessa skew straw.
      index++;

    } //  end of    for( ii=0; ii<2; ii++)

  } //   end of  for( iii=0; iii< nMCSkewAlone[imaxima]; iii++)

  //-------------------------------
  //------ fine aggiunta in blu eventuali punti della traccia MC che sono non mecciati

  if (!(index + nscitilhits == 0 || zmax < zmin || Smax < Smin)) {
    aaa = Smax - Smin;
    Smin -= aaa * 1.;
    Smax += aaa * 1.;

    aaa = zmax - zmin;
    zmin -= aaa * 0.05;
    zmax += aaa * 0.05;

    if (Smax > 2. * PI)
      Smax = 2. * PI;
    if (Smin < 0.)
      Smin = 0.;

    //  Smin=0.;
    //  Smax=2.*PI;

    fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", zmin, Smin, zmax, Smax);
    for (ii = 0; ii < index; ii++) {
      //       fprintf(MACRO,"E%d->Draw();\n",ii);
      fprintf(MACRO, "E%d->Draw();\n", Lista[ii]);
    }

    deltaz = zmax - zmin;
    deltaS = Smax - Smin;
    fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Smin + 0.05 * deltaS, zmax - 0.05 * deltaz, Smin + 0.05 * deltaS,
            zmin + 0.05 * deltaz, zmax - 0.05 * deltaz);
    fprintf(MACRO, "Assex->Draw();\n");
    fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Smin + 0.05 * deltaS, zmin + 0.05 * deltaz, Smax - 0.05 * deltaS,
            Smin + 0.05 * deltaS, Smax - 0.05 * deltaS);
    fprintf(MACRO, "Assey->Draw();\n");

    //------------
    //  plot di eventuali hits  SciTil;
    for (i = 0; i < nscitilhits; i++) {
      //		fprintf(MACRO,"SciT%d->Draw();\n",i);

      disegnaSciTilHit(MACRO, i, ZETA[i], ESSE[i], 1);
    }
    //------------

    // --------------------------------

    //  plot della traccia trovata dal finder

    if (goodskewfit) {
      if (fabs(KAPPA) > 1.e-10 && fabs(KAPPA) < 1.e10) {
        fmin = KAPPA * zmin + FI0;
        fmax = KAPPA * zmax + FI0;

        if (KAPPA >= 0.) {
          fmin = KAPPA * zmin + FI0;
          fmax = KAPPA * zmax + FI0;
        } else {
          fmax = KAPPA * zmin + FI0;
          fmin = KAPPA * zmax + FI0;
        }

        if (fmax >= 0.) {
          Nmax = (int)(0.5 * fmax / PI);
        } else {
          Nmax = ((int)(0.5 * fmax / PI)) - 1;
        }
        if (fmin >= 0.) {
          Nmin = (int)(0.5 * fmin / PI);
        } else {
          Nmin = ((int)(0.5 * fmin / PI)) - 1;
        }
        for (i = Nmin; i <= Nmax; i++) {
          offset = 2. * PI * i;
          z1 = (i * 2. * PI - FI0) / KAPPA;
          z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
          fprintf(MACRO, "TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n", i - Nmin, z1, 0., z2, 2. * PI, i - Nmin, i - Nmin);

        } //  end of  for(i=Nmin; i<= Nmax;++)

      } else { // continuation of if ( fabs(KAPPA) > 1.e-10 && fabs(KAPPA) < 1.e10)
        cout << "PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithMC,"
             << " this track found by PR not plotted"
             << "\n\t because KAPPA = " << KAPPA << endl;
      }

    } // end of if(goodskewfit)
      //---------------------------------------------  qui ci aggiungo le traccie MC

    imc = daTrackFoundaTrackMC;

    Int_t icode;
    Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica;
    PndMCTrack *pMC;
    pMC = (PndMCTrack *)fMCTrackArray->At(imc);
    if (pMC) {
      icode = pMC->GetPdgCode();       //   PDG code of track
      Oxx = pMC->GetStartVertex().X(); //   X of starting point track
      Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      fdbPDG = TDatabasePDG::Instance();
      fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) >= 0.1) {

        Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
        Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
        Fifi = atan2(Cy, Cx); // MC truth Fifi angle of circle of Helix trajectory
        if (Fifi < 0.)
          Fifi += 2. * PI;
        if (fabs(pMC->GetMomentum().Z()) < 1.e-20)
          KAPPA = 99999999.;
        else
          KAPPA = -carica * 0.001 * BFIELD * CVEL / pMC->GetMomentum().Z();

        FI0 = fmod(Fifi + PI, 2. * PI);

        if (fabs(KAPPA) > 1.e-10 && fabs(KAPPA) < 1.e10) {
          fmin = KAPPA * zmin + FI0;
          fmax = KAPPA * zmax + FI0;
          if (fmax >= 0.) {
            Nmax = (int)(0.5 * fmax / PI);
          } else {
            Nmax = ((int)(0.5 * fmax / PI)) - 1;
          }
          if (fmin >= 0.) {
            Nmin = (int)(0.5 * fmin / PI);
          } else {
            Nmin = ((int)(0.5 * fmin / PI)) - 1;
          }

          for (i = Nmin; i <= Nmax; i++) {
            offset = 2. * PI * i;
            z1 = (i * 2. * PI - FI0) / KAPPA;
            z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
            fprintf(MACRO, "TLine* MC%d_%d = new TLine(%f,%f,%f,%f);\nMC%d_%d->SetLineColor(3);\nMC%d_%d->Draw();\n", imc, i - Nmin, z1, 0., z2, 2. * PI, imc, i - Nmin, imc,
                    i - Nmin);

          } //  end of  for(i=Nmin; i<= Nmax;++)

        } else { // end of  if ( fabs(KAPPA) > 1.e-10  && fabs(KAPPA) < 1.e10)
          cout << "PndSttTrackFinderReal::WriteMacroSkewAssociatedHits, this track found by PR not plotted"
               << "\n\t because KAPPA = " << KAPPA << endl;
        }

      } // end of if( fabs(carica)>=0.1)
    }   // end of  if ( pMC )

  } // end of  if( !(index+nscitilhits==0|| zmax < zmin || Smax < Smin ) )

  fprintf(MACRO, "}\n");
  fclose(MACRO);
}

//----------end of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithMC

//----------begin of function PndSttTrackFinderReal::PndSttfromXYtoConformal

void PndSttTrackFinderReal::PndSttFromXYtoConformal(Double_t trajectory_vertex[3], Double_t info[][7], Int_t Nparal, Double_t infoparalConformal[][5], Int_t *status)
{

  //   do the transformation in the conformal space :  u= x/(x**2+y**2), v= y/(x**2+y**2) for each hit from parallel
  //   straws;  also the equidrift radius changes.

  //

  Double_t gamma, x, y, r;

  for (int i = 0; i < Nparal; i++) {
    x = info[infoparal[i]][0] - trajectory_vertex[0];
    y = info[infoparal[i]][1] - trajectory_vertex[1];
    r = info[infoparal[i]][3];
    gamma = x * x + y * y - r * r;
    if (fabs(gamma) < 1.e-10) {
      *status = -1;
      continue;
    }
    infoparalConformal[i][0] = x / gamma;
    infoparalConformal[i][1] = y / gamma;
    infoparalConformal[i][2] = r / fabs(gamma);
    infoparalConformal[i][3] = infoparal[i]; //  n. of the Hit (in the original order)
    infoparalConformal[i][4] = STRAWRADIUS / fabs(gamma);
  }

  *status = 0;
  return;
}

//----------end of function PndSttTrackFinderReal::PndSttfromXYtoConformal

//----------start function PndSttTrackFinderReal::PndSttfromXYtoConformal2

void PndSttTrackFinderReal::PndSttFromXYtoConformal2(Double_t trajectory_vertex[3], Short_t nHitsinTrack, Short_t iExclude, Short_t *ListHits, Double_t info[][7],
                                                     Double_t auxinfoparalConformal[][5], Int_t *status)
{

  //   do the transformation in the conformal space :  u= x/(x**2+y**2), v= y/(x**2+y**2) for each hit from parallel
  //   straws;  also the equidrift radius changes.

  //

  Double_t gamma, x, y, r;

  for (int i = 0; i < nHitsinTrack; i++) {
    if (i == iExclude)
      continue;
    x = info[infoparal[ListHits[i]]][0] - trajectory_vertex[0];
    y = info[infoparal[ListHits[i]]][1] - trajectory_vertex[1];
    r = info[infoparal[ListHits[i]]][3];
    gamma = x * x + y * y - r * r;
    if (fabs(gamma) < 1.e-10) {
      *status = -1;
      continue;
    }
    auxinfoparalConformal[i][0] = x / gamma;
    auxinfoparalConformal[i][1] = y / gamma;
    auxinfoparalConformal[i][2] = r / fabs(gamma);
    auxinfoparalConformal[i][3] = infoparal[ListHits[i]]; //  n. of the Hit (in the original order)
    auxinfoparalConformal[i][4] = STRAWRADIUS / fabs(gamma);
  }

  *status = 0;
  return;
}

//----------end of function PndSttTrackFinderReal::PndSttfromXYtoConformal2

//----------begin of function PndSttTrackFinderReal::PndSttBoxConformalFilling

void PndSttTrackFinderReal::PndSttBoxConformalFilling(bool InclusionList[nmaxHits], Double_t infoparalConformal[][5], Int_t Nparal,
                                                      Short_t nBoxConformal[nRdivConformal][nFidivConformal], Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal],
                                                      Short_t RConformalIndex[nmaxHits], Short_t FiConformalIndex[nmaxHits])
{

  Short_t iR, iFi, i, j;
  Double_t Fi;

  for (i = 0; i < nRdivConformalEffective; i++) {
    for (j = 0; j < nFidivConformal; j++) {
      nBoxConformal[i][j] = 0;
    }
  }

  for (i = 0; i < Nparal; i++) {
    if (!InclusionList[infoparal[i]])
      continue;
    Fi = atan2(infoparalConformal[i][1], infoparalConformal[i][0]);
    if (Fi < 0.)
      Fi += 2. * PI;
    iFi = (Short_t)(0.5 * nFidivConformal * Fi / PI);
    if (iFi > nFidivConformal) {
      iFi = nFidivConformal;
    } else if (iFi < 0) {
      iFi = 0;
    }

    Double_t RRR = sqrt(infoparalConformal[i][0] * infoparalConformal[i][0] + infoparalConformal[i][1] * infoparalConformal[i][1]);
    //   cout<<"from Fillng : hit parallel n. "<<i<<"  con raggio conforme "<<RRR<<endl;

    for (j = nRdivConformalEffective - 1, iR = 0; j > 0; j--) {
      if (RRR > radiaConf[j]) {
        iR = j;
        break;
      }
    }
    if (nBoxConformal[iR][iFi] >= MAXHITSINCELL) {
      cout << "Warning from PndSttTrackFinderReal::PndSttBoxConformalFilling     :"
           << "\n\tcontent in nBoxConformal[" << iR << "][" << iFi << "] has reached the Max allowed value = " << MAXHITSINCELL << endl;
      continue;
    }
    HitsinBoxConformal[nBoxConformal[iR][iFi]][iR][iFi] = (Short_t)i;
    nBoxConformal[iR][iFi]++;
    RConformalIndex[infoparal[i]] = iR;
    FiConformalIndex[infoparal[i]] = iFi;
  } // end of for(i = 0; i< Nparal ; i++)

  return;
}
//----------end of function PndSttTrackFinderReal::PndSttBoxConformalFilling

//----------begin of function PndSttTrackFinderReal::Merge_Sort

void PndSttTrackFinderReal::Merge_Sort(Short_t n_ele, Double_t *array, Short_t *ind)
{

  //  this method orders a set of numbers from smaller to biggers; smaller numbers come first.
  //  The vector :    array     is going to be changed, and also the vector   ind  will be changed
  //  accordingly.

  Short_t nr, nl, middle, i, ind_left[n_ele], ind_right[n_ele];

  Double_t left[n_ele], right[n_ele], result[n_ele];

  if (n_ele <= 1)
    return;

  middle = n_ele / 2;
  for (i = 0; i < middle; i++) {
    left[i] = array[i];
    ind_left[i] = ind[i];
  }
  for (i = middle; i < n_ele; i++) {
    right[i - middle] = array[i];
    ind_right[i - middle] = ind[i];
  }

  Merge_Sort(middle, left, ind_left);
  Merge_Sort(n_ele - middle, right, ind_right);

  if (left[middle - 1] > right[0]) {
    Merge(middle, left, ind_left, n_ele - middle, right, ind_right, array, ind);
  } else {
    //  do the appending
    for (i = 0; i < middle; i++) {
      array[i] = left[i];
      ind[i] = ind_left[i];
    }
    for (i = middle; i < n_ele; i++) {
      array[i] = right[i - middle];
      ind[i] = ind_right[i - middle];
    }
  }
}

//----------end of function PndSttTrackFinderReal::Merge_Sort

//----------begin of function PndSttTrackFinderReal::Merge

void PndSttTrackFinderReal::Merge(Short_t nl, Double_t *left, Short_t *ind_left, Short_t nr, Double_t *right, Short_t *ind_right, Double_t *result, Short_t *ind)
{
  Short_t i = 0, j, nl_curr = 0, nr_curr = 0;

  while (nl > 0 && nr > 0) {
    if (left[nl_curr] <= right[nr_curr]) {
      result[i] = left[nl_curr];
      ind[i] = ind_left[nl_curr];
      nl--;
      nl_curr++;
    } else {
      result[i] = right[nr_curr];
      ind[i] = ind_right[nr_curr];
      nr--;
      nr_curr++;
    }
    i++;
  }
  //--------------------
  if (nl == 0) {
    for (j = 0; j < nr; j++) {
      result[i + j] = right[nr_curr + j];
      ind[i + j] = ind_right[nr_curr + j];
    }
  } else {
    for (j = 0; j < nl; j++) {
      result[i + j] = left[nl_curr + j];
      ind[i + j] = ind_left[nl_curr + j];
    }
  }
}

//----------end of function PndSttTrackFinderReal::Merge

//----------begin of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformal

Short_t PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformal(Short_t NRCELLDISTANCE, Short_t NFiCELLDISTANCE, Short_t Nparal,
                                                                    Short_t ihit,    // seed hit; if it is negative it is a SciTil hit.
                                                                    Short_t nRcell,  // R cell of the seed hit; can be negative beacuse of SciTil hits;
                                                                    Short_t nFicell, // Fi cell of the seed hit;
                                                                    Double_t info[][7], bool InclusionList[nmaxHits], Short_t RConformalIndex[nmaxHits],
                                                                    Short_t FiConformalIndex[nmaxHits], Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                                    Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal], Short_t *ListHitsinTrack)
{

  bool status, TemporaryInclusionList[nmaxHits];

  Short_t i;
  Short_t j, iFi, iR, nRmin, nRmax, nRemainingHits, nHitsinTrack, auxIndex[nmaxHits], Remaining[nmaxHits];

  Short_t iFi2;

  Double_t auxRvalues[nmaxHits];

  //   ihit        is the hit number in the PARALLEL number scheme

  for (i = 0, nRemainingHits = 0; i < Nparal; i++) {

    if (i != ihit && InclusionList[infoparal[i]]) { //  Inclusion of the
      //  parallel hit straws already used in other tracks
      //  remember the index of InclusionList is in the
      //  ORIGINAL scheme of hits
      TemporaryInclusionList[infoparal[i]] = true;
      Remaining[nRemainingHits] = i; //  index of the PARALLEL hit
      nRemainingHits++;
    } else {
      TemporaryInclusionList[infoparal[i]] = false;
    }
  }

  if (nRemainingHits < MINIMUMHITSPERTRACK)
    return 0;

  //  cells of the seed hit

  if (ihit >= 0) {
    nHitsinTrack = 1;
    ListHitsinTrack[0] = ihit;
    i = 0;
  } else {
    nHitsinTrack = 0;
    i = -1;
  }

  status = true;
  while (nRemainingHits > 0 && i < nHitsinTrack && status) {

    if (nRcell - NRCELLDISTANCE < 0) {
      nRmin = 0;
    } else {
      nRmin = nRcell - NRCELLDISTANCE;
    }
    if (nRcell + NRCELLDISTANCE >= nRdivConformalEffective) {
      nRmax = nRdivConformalEffective - 1;
    } else {
      nRmax = nRcell + NRCELLDISTANCE;
    }
    if (istampa > 0) {
      cout << "\tin FindTrackPatterninBoxConformal, nRmin = " << nRmin << ", nRmax = " << nRmax << endl;
      cout << "\tin FindTrackPatterninBoxConformal, nFicell = " << nFicell << endl;
    }
    for (iR = nRmin; iR <= nRmax && status; iR++) {
      for (iFi2 = nFicell - NFiCELLDISTANCE; iFi2 <= nFicell + NFiCELLDISTANCE && status; iFi2++) {
        if (iFi2 < 0) {
          iFi = nFidivConformal + iFi2;
        } else if (iFi2 >= nFidivConformal) {
          iFi = iFi2 - nFidivConformal;
        } else {
          iFi = iFi2;
        }
        for (j = 0; j < nBoxConformal[iR][iFi]; j++) {
          if (InclusionList[infoparal[HitsinBoxConformal[j][iR][iFi]]] && TemporaryInclusionList[infoparal[HitsinBoxConformal[j][iR][iFi]]]) {
            // hit number in the PARALLEL straws scheme
            ListHitsinTrack[nHitsinTrack] = HitsinBoxConformal[j][iR][iFi];
            nHitsinTrack++;
            if (nHitsinTrack >= nmaxHitsInTrack) {
              // finish the search
              status = false; // finish all outer loops as well.
              break;
            }
            TemporaryInclusionList[infoparal[HitsinBoxConformal[j][iR][iFi]]] = false;
            nRemainingHits--;
          } // end of if( InclusionList[  infoparal[...
        }   // end of  for (j = 0; j< nBoxConformal[iR][iFi]; j++)
      }     // end of  for( iFi2 = nFicell - NFiCELLDISTANCE ;
    }       // end of  for( iR= nRmin ; iR<= nRmax ; iR++)
            //----------------
    i++;
    if (i < nmaxHitsInTrack) {
      nRcell = RConformalIndex[infoparal[ListHitsinTrack[i]]];
      nFicell = FiConformalIndex[infoparal[ListHitsinTrack[i]]];
    }
  } //  end      while ( nRemainingHits > 0 && i < nHitsinTrack)

  return nHitsinTrack;
}

//----------end of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformal

//----------begin of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformalSpecial

//  The difference with PndSttFindTrackPatterninBoxConformal is that the search is performed
//  in the selected list    ListHitsinTrackinWhichToSearch   instead of all the parallel list.

Short_t PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformalSpecial(Short_t NRCELLDISTANCE, Short_t NFiCELLDISTANCE, Short_t Nparal, Short_t NparallelToSearch,
                                                                           Short_t iSeed, Short_t *ListHitsinTrackinWhichToSearch, Double_t info[][7], bool InclusionList[nmaxHits],
                                                                           Short_t RConformalIndex[nmaxHits], Short_t FiConformalIndex[nmaxHits],
                                                                           Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                                           Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal], Short_t *OutputListHitsinTrack)
{

  bool TemporaryInclusionList[nmaxHits];

  Short_t i, i2, j, iFi, iR, nFicell, nHitsinTrack, nRcell, nRemainingHits, nRmax, nRmin, auxIndex[nmaxHits], Remaining[nmaxHits];

  Short_t iFi2;

  Double_t auxRvalues[nmaxHits];

  //   iSeed        is the hit number in the PARALLEL number scheme

  //--------    the following initialization is essential for the algorithm to work
  for (i = 0; i < Nparal; i++) {
    TemporaryInclusionList[infoparal[i]] = false;
  }
  //-------------

  for (i2 = 0, nRemainingHits = 0; i2 < NparallelToSearch; i2++) {
    i = ListHitsinTrackinWhichToSearch[i2];
    //  Inclusion of the parallel hit straws already used in other tracks;
    //  remember the index of InclusionList is in the ORIGINAL scheme of hits.
    if (i != iSeed && InclusionList[infoparal[i]]) {
      TemporaryInclusionList[infoparal[i]] = true;
      Remaining[nRemainingHits] = i; //  index of the PARALLEL hit
      nRemainingHits++;
    }
  }

  if (nRemainingHits < MINIMUMHITSPERTRACK)
    return 0;

  //  cells of the seed hit

  nHitsinTrack = 1;
  OutputListHitsinTrack[0] = iSeed;
  i = 0;
  while (nRemainingHits > 0 && i < nHitsinTrack) {

    nRcell = RConformalIndex[infoparal[OutputListHitsinTrack[i]]];
    nFicell = FiConformalIndex[infoparal[OutputListHitsinTrack[i]]];

    //---------------

    if (nRcell - NRCELLDISTANCE < 0) {
      nRmin = 0;
    } else {
      nRmin = nRcell - NRCELLDISTANCE;
    }
    if (nRcell + NRCELLDISTANCE >= nRdivConformalEffective) {
      nRmax = nRdivConformalEffective - 1;
    } else {
      nRmax = nRcell + NRCELLDISTANCE;
    }

    for (iR = nRmin; iR <= nRmax; iR++) {
      for (iFi2 = nFicell - NFiCELLDISTANCE; iFi2 <= nFicell + NFiCELLDISTANCE; iFi2++) {
        if (iFi2 < 0) {
          iFi = nFidivConformal + iFi2;
        } else if (iFi2 >= nFidivConformal) {
          iFi = iFi2 - nFidivConformal;
        } else {
          iFi = iFi2;
        }
        for (j = 0; j < nBoxConformal[iR][iFi]; j++) {
          if (InclusionList[infoparal[HitsinBoxConformal[j][iR][iFi]]] && TemporaryInclusionList[infoparal[HitsinBoxConformal[j][iR][iFi]]]) {
            //  hit number in the PARALLEL straws scheme
            OutputListHitsinTrack[nHitsinTrack] = HitsinBoxConformal[j][iR][iFi];
            nHitsinTrack++;
            TemporaryInclusionList[infoparal[HitsinBoxConformal[j][iR][iFi]]] = false;
            nRemainingHits--;
          }
        } // end of for (j = 0; j< nBoxConformal[iR][iFi]; j++)
      }
    }
    //----------------
    i++;

  } //  end      while ( nRemainingHits > 0 && i < nHitsinTrack)

  return nHitsinTrack;
}

//----------end of function PndSttTrackFinderReal::PndSttFindTrackPatterninBoxConformalSpecial

//----------begin of function PndSttTrackFinderReal::PndSttFindTrackStrictCollection

Short_t PndSttTrackFinderReal::PndSttFindTrackStrictCollection(Short_t NFiCELLDISTANCE,
                                                               Short_t iSeed,             //  seed track (parallel notation) as fa as the Fi angle is concerned
                                                               Short_t NParallelToSearch, //  n. of hits to search in ListHitsinTrackinWhichToSearch
                                                               Short_t *ListHitsinTrackinWhichToSearch, bool InclusionList[nmaxHits], Short_t FiConformalIndex[nmaxHits],
                                                               Short_t *OutputListHitsinTrack)
{

  Short_t i, j, iR, iFi, iFiseed, nHitsinTrack;

  Double_t auxRvalues[nmaxHits];

  if (istampa >= 3 && IVOLTE <= nmassimo) {
    cout << "Da strictcollection, n. elementi delle search list " << NParallelToSearch << endl;
    for (i = 0; i < NParallelToSearch; i++) {
      cout << "Da strictcollection,  hit n. (parallel notation ) " << ListHitsinTrackinWhichToSearch[i] << endl;
    }
  }

  //   iSeed        is the hit number in the PARALLEL number scheme

  iFiseed = FiConformalIndex[infoparal[iSeed]];
  if (istampa >= 3 && IVOLTE <= nmassimo)
    cout << "Da strictcollection, iFiseed " << iFiseed << endl;

  nHitsinTrack = 0;
  for (i = 0; i < NParallelToSearch; i++) {
    if (InclusionList[infoparal[ListHitsinTrackinWhichToSearch[i]]]) { //  Inclusion of the parallel hit straws already used in other tracks
                                                                       //  remember the index of InclusionList is in the ORIGINAL scheme of hits

      iFi = FiConformalIndex[infoparal[ListHitsinTrackinWhichToSearch[i]]];
      if (iFi == iFiseed) {
        OutputListHitsinTrack[nHitsinTrack] = ListHitsinTrackinWhichToSearch[i];
        nHitsinTrack++;
      } else if (iFi < iFiseed) {
        if (iFiseed - iFi <= NFiCELLDISTANCE) {
          OutputListHitsinTrack[nHitsinTrack] = ListHitsinTrackinWhichToSearch[i];
          nHitsinTrack++;
        } else {
          if (iFi + nFidivConformal - iFiseed <= NFiCELLDISTANCE) {
            OutputListHitsinTrack[nHitsinTrack] = ListHitsinTrackinWhichToSearch[i];
            nHitsinTrack++;
          }
        }
      } else { //  iFi > iFiseed
        if (-iFiseed + iFi <= NFiCELLDISTANCE) {
          OutputListHitsinTrack[nHitsinTrack] = ListHitsinTrackinWhichToSearch[i];
          nHitsinTrack++;
        } else {
          if (-iFi + nFidivConformal + iFiseed <= NFiCELLDISTANCE) {
            OutputListHitsinTrack[nHitsinTrack] = ListHitsinTrackinWhichToSearch[i];
            nHitsinTrack++;
          }
        }

      } //  end of   if( iFi == iFiseed )

    } //  end of    if( InclusionList[  infoparal[ ListHitsinTrackinWhichToSearch[i] ]   ] )

  } //  end of        for(i=0; i<NparallelToSearch; i++)

  if (istampa >= 3 && IVOLTE <= nmassimo) {
    cout << "Da strictcollection, n. elementi aggiunti " << nHitsinTrack << endl;
    for (i = 0; i < nHitsinTrack; i++) {
      cout << "Da strictcollection,  hit n. (parallel notaion ) " << OutputListHitsinTrack[i] << endl;
    }
  }

  return nHitsinTrack;
}

//----------end of function PndSttTrackFinderReal::PndSttFindTrackStrictCollection

//----------begin of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixQuater

Short_t PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixQuater(bool InclusionList[nmaxHits], Double_t m, Double_t q, Short_t Status, Short_t nHitsinTrack,
                                                                            Short_t *ListHitsinTrack, Int_t NhitsParallel, Double_t Ox, Double_t Oy, Double_t R, Double_t info[][7],
                                                                            Double_t infoparalConformal[][5], Short_t *RConformalIndex, Short_t *FiConformalIndex,
                                                                            Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                                            Short_t HitsinBoxConformal[][nRdivConformal][nFidivConformal], Short_t *auxListHitsinTrack)
{
  bool passamin, passamax, Unselected[nmaxHits];

  Short_t i, i2, j, k, l, l2, l3, itemp, kstart, kend, iFi0, FFimin, FFimax;
  Short_t Nextra = 8, nFi, Fi, nR, nAssociatedHits, nHit_original;
  Double_t maxFi, minFi, dist, xx, yy, aaa, angle, r, erre1, erre2, Rin, Rout, Fi0, ddd, fi1, fi2, dx, dy, distance,
    NTIMES = 1.5; //   number of Straw radia allowed in association

  nAssociatedHits = 0;
  for (i = 0; i < NhitsParallel; i++) {
    Unselected[i] = true;
  }

  //   find the range in Fi spanned  by the candidate track

  FFimin = 10000;
  FFimax = 0;
  for (j = 0; j < nHitsinTrack; j++) {
    i = (Short_t)infoparalConformal[ListHitsinTrack[j]][3];

    if (FiConformalIndex[i] < FFimin)
      FFimin = FiConformalIndex[i];
    if (FiConformalIndex[i] > FFimax)
      FFimax = FiConformalIndex[i];
  }

  if (FFimax > 3. * nFidivConformal / 4. && FFimin < nFidivConformal / 4.) {
    FFimin = 10000;
    FFimax = 0;
    for (j = 0; j < nHitsinTrack; j++) {
      i = (Short_t)infoparalConformal[ListHitsinTrack[j]][3];
      Fi = FiConformalIndex[i];
      if (Fi < nFidivConformal / 4.)
        Fi = FiConformalIndex[i] + nFidivConformal;
      if (Fi < FFimin)
        FFimin = Fi;
      if (Fi > FFimax)
        FFimax = Fi;
    }
  }

  //  finding the boundaries in the Conformal plane. The basic assumption is that the range
  // in Fi is much less that 180 degrees.

  FFimin -= (Short_t)nFidivConformal / Nextra;
  FFimax += (Short_t)nFidivConformal / Nextra;
  if (FFimax - FFimin > nFidivConformal / 2) {
    cout << "something fishy is going on in PndSttTrkAssociatedParallelHitsToHelixQuater!"
         << "Range in Fi (rad) is  " << (FFimax - FFimin) * 2. * PI / nFidivConformal << endl;
    return 0;
  }

  //   use the equation of a line in polar coordinates

  if (Status == 99) { //  case in which   0 = x + q

    if (fabs(q) > 1.e-10) {
      passamax = false;
      passamin = false;
      for (itemp = FFimin; itemp <= FFimax; itemp++) {
        i = itemp;
        if (i < 0) {
          i += nFidivConformal;
        } else if (i >= nFidivConformal) {
          i -= nFidivConformal * (i / nFidivConformal);
          //         i -= nFidivConformal;
        }
        angle = (i + 0.5) * 2. * PI / nFidivConformal;
        aaa = cos(angle);
        if (fabs(cos(angle)) < 1.e-10)
          continue;
        r = -q / aaa;
        if (r < radiaConf[0] || r >= 1. / RStrawDetectorMin)
          continue;
        for (j = nRdivConformalEffective - 1; j >= 0; j--) {
          if (r >= radiaConf[j]) {
            nR = j;
            break;
          }
        }

        for (l = -DELTAnR; l < DELTAnR + 1; l++) {
          l2 = nR + l;
          if (l2 < 0 || l2 >= nRdivConformalEffective)
            continue;
          for (k = 0; k < nBoxConformal[l2][i]; k++) {
            nHit_original = (Short_t)infoparalConformal[HitsinBoxConformal[k][l2][i]][3];
            if (!InclusionList[nHit_original])
              continue;
            // check if the hit position is near the circle of the Helix found by the fit
            dx = -Ox + info[nHit_original][0];
            dy = -Oy + info[nHit_original][1];
            distance = sqrt(dx * dx + dy * dy);
            // cout<<"nuov, R "<<R<<",  distance  "<<distance<<endl;
            if (fabs(R - distance) > NTIMES * STRAWRADIUS)
              continue;

            //-------------------
            xx = infoparalConformal[HitsinBoxConformal[k][l2][i]][0];
            dist = fabs(xx + q);
            if (PndSttAcceptHitsConformal(dist, infoparalConformal[HitsinBoxConformal[k][l2][i]][2], infoparalConformal[HitsinBoxConformal[k][l2][i]][4])) {
              auxListHitsinTrack[nAssociatedHits] = HitsinBoxConformal[k][l2][i];
              nAssociatedHits++;
            }
          } //  end of  for( k=0;k<nBoxConformal[l2][i];k++)
        }   //   end of for(l=-DELTAnR; l<DELTAnR+1;l++)

        // ------- special cases
        if ((nR == nRdivConformalEffective - 1 && passamin && !passamax) || (nR == 0 && passamax && !passamin)) { //  do the last two Fi columns
          if (nR == nRdivConformalEffective - 1)
            passamax = true;
          if (nR == 0)
            passamin = true;

          for (l2 = 1; l2 < 3; l2++) {
            i2 = i + l2;
            if (i2 >= nFidivConformal)
              i2 -= nFidivConformal;
            for (l = -2; l < 3; l++) {
              l3 = nR + l;
              if (l3 < 0 || l3 >= nRdivConformalEffective)
                continue;
              for (k = 0; k < nBoxConformal[l3][i2]; k++) {
                nHit_original = (Short_t)infoparalConformal[HitsinBoxConformal[k][l3][i2]][3];
                if (!InclusionList[nHit_original])
                  continue;
                // check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox + info[nHit_original][0];
                dy = -Oy + info[nHit_original][1];
                distance = sqrt(dx * dx + dy * dy);
                if (fabs(R - distance) > NTIMES * STRAWRADIUS)
                  continue;

                //-------------------
                xx = infoparalConformal[HitsinBoxConformal[k][l3][i2]][0];
                dist = fabs(xx + q);
                //                if(dist < 3.*infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2]){
                if (PndSttAcceptHitsConformal(dist, infoparalConformal[HitsinBoxConformal[k][l3][i2]][2], infoparalConformal[HitsinBoxConformal[k][l3][i2]][4])) {
                  auxListHitsinTrack[nAssociatedHits] = HitsinBoxConformal[k][l3][i2];
                  nAssociatedHits++;
                }
              } //  end of   for( k=0;k<nBoxConformal[l3][i2];k++)
            }   //   end of for(l=-2; l<3;l++)
          }     //   end of for(l2=0;l2<2;l2++)
          return nAssociatedHits;
        } else if ((nR == nRdivConformalEffective - 1 && !passamin && !passamax) || (nR == 0 && !passamax && !passamin)) {
          if (nR == nRdivConformalEffective - 1)
            passamax = true;
          if (nR == 0)
            passamin = true;

          for (l2 = 1; l2 < 3; l2++) {
            i2 = i - l2;
            if (i2 < nFidivConformal)
              i2 += nFidivConformal;
            for (l = -2; l < 3; l++) {
              l3 = nR + l;
              if (l3 < 0 || l3 >= nRdivConformalEffective)
                continue;
              for (k = 0; k < nBoxConformal[l3][i2]; k++) {
                nHit_original = (Short_t)infoparalConformal[HitsinBoxConformal[k][l3][i2]][3];
                if (!InclusionList[nHit_original])
                  continue;
                // check if the hit position is near the circle of the Helix found by the fit
                dx = -Ox + info[nHit_original][0];
                dy = -Oy + info[nHit_original][1];
                distance = sqrt(dx * dx + dy * dy);
                if (fabs(R - distance) > NTIMES * STRAWRADIUS)
                  continue;

                //-------------------
                xx = infoparalConformal[HitsinBoxConformal[k][l3][i2]][0];
                dist = fabs(xx + q);
                //                if(dist < 3.*infoparalConformal[  HitsinBoxConformal[k][l3][i2]  ][2]){
                if (PndSttAcceptHitsConformal(dist, infoparalConformal[HitsinBoxConformal[k][l3][i2]][2], infoparalConformal[HitsinBoxConformal[k][l3][i2]][4])) {
                  auxListHitsinTrack[nAssociatedHits] = HitsinBoxConformal[k][l3][i2];
                  nAssociatedHits++;
                }
              } //  end of  for( k=0;k<nBoxConformal[l3][i2];k++)
            }   //   end of for(l=-2; l<3;l++)
          }     //   end of for(l2=0;l2<2;l2++)
        }       //   end of if((nR == nRdivConformalEffective-1 && passamin) ||  (nR==0 && passamax)  )

      } //  end of     for(itemp=Fimin; itemp<=FFimax;itemp++)

    } else { //  q=0 --> x=0

      if (FFimax > nRdivConformal / 4 && FFimin < nRdivConformal / 4) {
        iFi0 = (Short_t)(nRdivConformal / 4);
      } else if (FFimax > 3 * nRdivConformal / 4 && FFimin < 3 * nRdivConformal / 4) {
        iFi0 = (Short_t)(3 * nRdivConformal / 4);
      } else {
        cout << "From PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixQuater  :"
             << "  inconsistency, 0 associated hits to this track candidate\n";
        return 0;
      }

      for (itemp = iFi0 - 5; itemp <= iFi0 + 5; itemp++) {
        i = itemp;
        if (i < 0) {
          i += nFidivConformal;
        } else if (i >= nFidivConformal) {
          i -= nFidivConformal * (i / nFidivConformal);
          //         i -= nFidivConformal;
        }
        for (l = 0; l < nRdivConformalEffective; l++) {
          for (k = 0; k < nBoxConformal[l][i]; k++) {
            nHit_original = (Short_t)infoparalConformal[HitsinBoxConformal[k][l][i]][3];
            if (!InclusionList[nHit_original])
              continue;
            // check if the hit position is near the circle of the Helix found by the fit
            dx = -Ox + info[nHit_original][0];
            dy = -Oy + info[nHit_original][1];
            distance = sqrt(dx * dx + dy * dy);
            if (fabs(R - distance) > NTIMES * STRAWRADIUS)
              continue;

            //-------------------
            xx = infoparalConformal[HitsinBoxConformal[k][l][i]][0];
            dist = fabs(xx);

            if (PndSttAcceptHitsConformal(dist, infoparalConformal[HitsinBoxConformal[k][l][i]][2], infoparalConformal[HitsinBoxConformal[k][l][i]][4])) {
              auxListHitsinTrack[nAssociatedHits] = HitsinBoxConformal[k][l][i];
              nAssociatedHits++;
            }
          } //  end of for( k=0;k<nBoxConformal[l][i];k++)
        }   //  end of for(l=0; l<nRdivConformalEffective;l++)
      }     //  end of for(itemp=iFi0-5; itemp<=iFi0+5;itemp++)

    } //   end  of if(fabs(q) > 1.e-10 )

  } else if (fabs(q) > 1.e-10) { //   second part of    if( Status ==99),  in this case y = m*x +q

    Fi0 = atan(m); // Fi0 belongs to (-PI/2, PI/2] .
    aaa = atan2(q, -m * q);
    if (Fi0 < 0.) {
      Fi0 += PI;
      if (Fi0 < 0.)
        Fi0 = 0.; // this is between 0. and PI.
      if (Fi0 > PI)
        Fi0 = PI; // this is between 0. and PI.
    };
    ddd = fabs(q) / sqrt(1. + m * m);

    for (itemp = FFimin; itemp <= FFimax; itemp++) {
      i = itemp;
      if (i < 0) {
        i += nFidivConformal;
      } else if (i >= nFidivConformal) {
        i -= nFidivConformal * (i / nFidivConformal);
      }

      // erre1 is the distance from origin of point of intersection of the straight
      // line of equation  y= m*x+q  with  line of equation  y = x*tan(fi1);
      // when erre1 is < 0 it means the intersection is on the opposite side of the
      // versor defined by  [cos(fi1); sin(fi1)].
      // Here we are working in the conformal plane U,V.

      fi1 = i * 2. * (PI / nFidivConformal);
      if (fabs(sin(fi1) - m * cos(fi1)) > 1.e-10) {
        erre1 = q / (sin(fi1) - m * cos(fi1));
      } else {
        erre1 = 99999999999.;
      }

      fi2 = (i + 1) * 2. * (PI / nFidivConformal);
      if (fabs(sin(fi2) - m * cos(fi2)) > 1.e-10) {
        erre2 = q / (sin(fi2) - m * cos(fi2));
      } else {
        erre2 = 99999999999.;
      }

      for (j = 0; j < nRdivConformal; j++) {
        Rin = radiaConf[j];
        if (j != nRdivConformal - 1) {
          Rout = radiaConf[j + 1];
        } else {
          Rout = 1. / RStrawDetectorMin;
        }

        //  note that the following algorithm works also for negative erre1  and   erre2

        if (erre1 < -1.e-10) {
          if (erre2 < 0. || erre2 > Rout) {
            continue;
          }
        } else if (fabs(erre1) < 1.e-10) {
          if (Fi0 > fi2 || Fi0 < fi1) {
            continue;
          }
        } else if (erre1 < Rin) {
          if (erre2 < Rin && erre2 > 0.) {
            continue;
          }
        } else if (erre1 > Rout && erre2 > Rout && !(fi1 <= aaa && aaa <= fi2 && ddd <= Rout)) {
          continue;
        }

        for (l = itemp - 2; l <= itemp + 2; l++) {
          if (l < 0) {
            l2 = l + nFidivConformal;
          } else if (l >= nFidivConformal) {
            l2 = l - nFidivConformal * (l / nFidivConformal);
          } else {
            l2 = l;
          }
          if (j - 1 < 0) {
            kstart = 0;
          } else {
            kstart = j - 1;
          }
          if (j + 1 >= nRdivConformal) {
            kend = nRdivConformal;
          } else {
            kend = j + 2;
          }

          for (k = kstart; k < kend; k++) {

            for (l3 = 0; l3 < nBoxConformal[k][l2]; l3++) {
              if (!Unselected[HitsinBoxConformal[l3][k][l2]])
                continue;
              nHit_original = (Short_t)infoparalConformal[HitsinBoxConformal[l3][k][l2]][3];
              if (!InclusionList[nHit_original])
                continue;
              // check if the hit position is near the circle of the Helix found by the fit
              dx = -Ox + info[nHit_original][0];
              dy = -Oy + info[nHit_original][1];
              distance = sqrt(dx * dx + dy * dy);
              if (fabs(R - distance) > NTIMES * STRAWRADIUS)
                continue;

              //-------------------
              xx = infoparalConformal[HitsinBoxConformal[l3][k][l2]][0];
              yy = infoparalConformal[HitsinBoxConformal[l3][k][l2]][1];
              dist = fabs(-yy + m * xx + q) / sqrt(m * m + 1.);
              if (PndSttAcceptHitsConformal(dist, infoparalConformal[HitsinBoxConformal[l3][k][l2]][2], infoparalConformal[HitsinBoxConformal[l3][k][l2]][4])) {

                auxListHitsinTrack[nAssociatedHits] = HitsinBoxConformal[l3][k][l2];
                Unselected[HitsinBoxConformal[l3][k][l2]] = false;
                nAssociatedHits++;
              }

            } //   end of  for( l3=0;l3<nBoxConformal[k][l2];l3++)
          }   //   end of  for(k=kstart;k<kend;k++)
        }     //   end of  for(l=itemp-1; l<itemp+2; l++)

      } //  end of for(j=0; j<nRdivConformal; j++)

    } //   end of    for(itemp=FFimin; itemp<=FFimax;itemp++)

  } else { //  case in which    y= m*x ,  m can be zero    ,  third part of if( Status ==99)

    iFi0 = (Short_t)(atan(m) * nRdivConformal / (2. * PI));
    for (itemp = iFi0 - 5; itemp <= iFi0 + 5; itemp++) {
      i = itemp;
      if (i < 0) {
        i += nFidivConformal;
      } else if (i >= nFidivConformal) {
        i -= nFidivConformal * (i / nFidivConformal);
        //          i -= nFidivConformal;
      }
      for (l = 0; l < nRdivConformalEffective; l++) {
        for (k = 0; k < nBoxConformal[l][i]; k++) {
          nHit_original = (Short_t)infoparalConformal[HitsinBoxConformal[k][l][i]][3];
          if (!InclusionList[nHit_original])
            continue;
          // check if the hit position is near the circle of the Helix found by the fit
          dx = -Ox + info[nHit_original][0];
          dy = -Oy + info[nHit_original][1];
          distance = sqrt(dx * dx + dy * dy);
          if (fabs(R - distance) > NTIMES * STRAWRADIUS)
            continue;

          //-------------------
          xx = infoparalConformal[HitsinBoxConformal[k][l][i]][0];
          yy = infoparalConformal[HitsinBoxConformal[k][l][i]][1];
          dist = fabs(m * xx - yy) / sqrt(m * m + 1.);
          if (PndSttAcceptHitsConformal(dist, infoparalConformal[HitsinBoxConformal[k][l][i]][2], infoparalConformal[HitsinBoxConformal[k][l][i]][4])) {
            auxListHitsinTrack[nAssociatedHits] = HitsinBoxConformal[k][l][i];
            nAssociatedHits++;
          }
        } //  end of for( k=0;k<nBoxConformal[l][i];k++)
      }
    } //  end of for(itemp=FFimin; itemp<=FFimax;itemp++)

  } //   end of if ( Status ==99)

  if (istampa >= 3) {
    cout << "from PndSttTrkAssociatedParallelHitsToHelixQuater, before exiting; nAssociatedHits = " << nAssociatedHits << endl;
  }

  return nAssociatedHits;
}

//----------end of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelixQuater

//----------begin of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix5

Short_t PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix5(bool InclusionList[nmaxHits], Int_t NhitsParallel, Double_t Ox, Double_t Oy, Double_t R, Double_t info[][7],
                                                                       Double_t Fi_low, Double_t Fi_up, Short_t *auxListHitsinTrack)
{

  Short_t i;

  Short_t nAssociatedHits;

  Double_t angle, dx, dy, distance,
    NTIMES = 5.; //   number of Straw radia allowed in association.

  nAssociatedHits = 0;
  //   find the Hits belonging to this Track.

  for (i = 0; i < NhitsParallel; i++) {
    if (!InclusionList[infoparal[i]])
      continue;
    // check if the hit position is near the circle of the Helix found by the fit

    dx = -Ox + info[infoparal[i]][0];
    dy = -Oy + info[infoparal[i]][1];
    angle = atan2(dy, dx);
    if (angle < 0.)
      angle += 2. * PI;
    if (angle < 0.)
      angle = 0.;
    distance = sqrt(dx * dx + dy * dy);
    if (fabs(R - distance) > NTIMES * STRAWRADIUS)
      continue;
    if (angle < Fi_low)
      angle += 2. * PI;
    if (angle > Fi_up)
      continue;
    auxListHitsinTrack[nAssociatedHits] = i;
    nAssociatedHits++;
  } // end for(i=0; i<NhitsParallel;i++)

  return nAssociatedHits;
}

//----------end of function PndSttTrackFinderReal::PndSttTrkAssociatedParallelHitsToHelix5

//----------begin of function PndSttTrackFinderReal::PndSttAcceptHitsConformal

bool PndSttTrackFinderReal::PndSttAcceptHitsConformal(Double_t distance,
                                                      Double_t DriftConfR, // drift radius in conformal space
                                                      Double_t StrawConfR  // straw radius in conformal space
)
{
  if (fabs(distance - DriftConfR) < 2. * StrawConfR)
    return true;
  return false;
}

//----------end of function PndSttTrackFinderReal::PndSttAcceptHitsConformal

//----------begin of function PndSttTrackFinderReal::AssociateSkewHitsToXYTrack

Short_t PndSttTrackFinderReal::AssociateSkewHitsToXYTrack(bool *InclusionListSkew, Double_t Ox, Double_t Oy, Double_t R, Double_t info[][7], Double_t inclination[][3],
                                                          Double_t Fi_low_limit, Double_t Fi_up_limit, Short_t Charge, Double_t Fi_initial_helix_referenceframe,
                                                          Double_t Fi_final_helix_referenceframe,
                                                          Short_t SkewList[nmaxHits][2], // output,  list of selected skew hits (in skew numbering)
                                                          Double_t *S,                   //  output,  S coordinate of selected Skew hit
                                                          Double_t *Z,                   //  output,  Z coordinate of selected Skew hit
                                                          Double_t *ZDrift,              //  output,  drift distance IN Z DIRECTION only, of selected Skew hit
                                                          Double_t *ZErrorafterTilt      //  output,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
)
{

  Int_t i, j, i1, ii, iii, NAssociated, Kincl, nlow, nup, STATUS, Nmin, Nmax;

  Double_t xmin, xmax, ymin, ymax, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, vx1, vy1, vz1, C0x1, C0y1,
    C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2,
    Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  //   calculate the Fi range allowed to the skew hits, with Fi calculated in the TRACK CYLINDER REFERENCE FRAME.

  //      Smin=zmin = 1.e10;
  //      Smax=zmax = -zmin;
  NAssociated = 0;

  for (iii = 0; iii < nSttSkewhit; iii++) {
    i = infoskew[iii];
    if (!InclusionListSkew[i])
      continue;

    Kincl = (int)info[i][5] - 1;

    aaa = sqrt(inclination[Kincl][0] * inclination[Kincl][0] + inclination[Kincl][1] * inclination[Kincl][1] + inclination[Kincl][2] * inclination[Kincl][2]);
    vx1 = inclination[Kincl][0] / aaa;
    vy1 = inclination[Kincl][1] / aaa;
    vz1 = inclination[Kincl][2] / aaa;
    C0x1 = info[i][0];
    C0y1 = info[i][1];
    C0z1 = info[i][2];

    calculateintersections(Ox, Oy, R, C0x1, C0y1, C0z1, info[i][3], vx1, vy1, vz1, &STATUS, POINTS1);

    if (STATUS < 0)
      continue;

    for (ii = 0; ii < 2; ii++) {

      j = 3 * ii;
      distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

      Rx = POINTS1[j] - Ox;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= R;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
      if (bbb > 1.e-10) {
        Tiltdirection1[0] = vz1 / bbb;
        Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
      } else {
        Tiltdirection1[0] = 1.;
        Tiltdirection1[1] = 0.;
      }

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;

      Aellipsis1 = info[i][3] * aaa / LL;

      Bellipsis1 = info[i][3] / R;

      if (distance >= info[i][4] + Aellipsis1)
        continue;

      // checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

      //        if(
      //             fabs(POINTS1[j+2]-info[i][2]) > SEMILENGTH_STRAIGHT- Aellipsis1 ||
      //             distance + bbb > info[i][4]        //  the ellipsis goes out of the boundaries of the skew straw
      //          ) {
      // if( istampa)  cout<<"the ellipsis goes out of the boundaries of the skew straw, hit n. "<<i<<endl
      //     <<"dis. from center "<<distance+bbb<<",  length of the straw "<<info[i][4]<<endl;
      //           continue;
      //          }
      //--------------------------

      S[NAssociated] = atan2(POINTS1[j + 1] - Oy, POINTS1[j] - Ox); // atan2 returns radians in (-pi and +pi]
      if (S[NAssociated] < 0.)
        S[NAssociated] += 2. * PI;

      //  check if the S of this intersection is compatible with information coming from the parallel fit of this track
      //        Double_t Sprime = atan2(POINTS1[j+1], POINTS1[j]) ;
      //        if( Sprime < 0.) Sprime += 2.*PI;

      //        if(  Sprime < Fi_low_limit) {
      //           if(  Sprime+2.*PI > Fi_up_limit)  continue;
      //        }  else {
      //           if(  Sprime > Fi_up_limit)  continue;
      //        }

      if (S[NAssociated] < Fi_low_limit) {
        if (S[NAssociated] + 2. * PI > Fi_up_limit)
          continue;
      } else if (S[NAssociated] > Fi_up_limit) {
        if (S[NAssociated] - 2. * PI < Fi_low_limit)
          continue;
      }

      //  second check, to see if this hits are in the allowed Fi range in the Helix reference frame
      /*
              if ( S[NAssociated] <  Fi_allowedforskew_low) {
                    if (S[NAssociated] + 2.*PI >  Fi_allowedforskew_up)  continue;
              } else {
                    if (S[NAssociated] >  Fi_allowedforskew_up)  continue;
              }
      */

      //---------------------------   end check

      Z[NAssociated] = POINTS1[j + 2];
      ZDrift[NAssociated] = Aellipsis1 * Tiltdirection1[0];
      ZErrorafterTilt[NAssociated] = StrawDriftError * aaa * Tiltdirection1[0] / LL;
      SkewList[NAssociated][0] = iii; // n. skew hit in skew hit numbering
      SkewList[NAssociated][1] = ii;  //  solution 0 or solution 1 were accepted

      // check if this skew hit doesn't "push out"  the most external parallel hit
      // (see Gianluigi's logbook on page 251)

      Double_t Zh1 = Z[NAssociated] - ZDrift[NAssociated];
      Double_t Zh2 = Z[NAssociated] + ZDrift[NAssociated];
      Double_t Sh1 = S[NAssociated] - Aellipsis1 * Tiltdirection1[1];
      Double_t Sh2 = S[NAssociated] + Aellipsis1 * Tiltdirection1[1];
      Double_t Zlast1 = (Fi_final_helix_referenceframe - Fi_initial_helix_referenceframe) * Zh1 / (Sh1 - Fi_initial_helix_referenceframe);
      Double_t Zlast2 = (Fi_final_helix_referenceframe - Fi_initial_helix_referenceframe) * Zh2 / (Sh2 - Fi_initial_helix_referenceframe);

      if (fabs(Zlast1 - ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT && fabs(Zlast2 - ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT)
        continue;

      NAssociated++;

    } //  end of    for( ii=0; ii<2; ii++)

  } //   for( iii=0; iii< nSttSkewhit; iii++)

  return NAssociated;
}

//----------end of function PndSttTrackFinderReal::AssociateSkewHitsToXYTrack

//----------begin of function PndSttTrackFinderReal::AssociateBetterAfterFitSkewHitsToXYTrack

Short_t PndSttTrackFinderReal::AssociateBetterAfterFitSkewHitsToXYTrack(
  Short_t TemporarynSttSkewhitinTrack, //  input
  Short_t SkewList[nmaxHits][2],       // input,  list of selected skew hits (in skew numbering)
  Double_t *S,                         //  input,  S coordinate of selected Skew hit
  Double_t *Z,                         //  input,  Z coordinate of selected Skew hit
  Double_t *ZDrift,                    //  input,  drift distance IN Z DIRECTION only, of selected Skew hit
  Double_t *ZErrorafterTilt,           //  input,  Radius taking into account the tilt, IN Z DIRECTION only, of selected Skew hit
  Double_t KAPPA,                      // input, KAPPA result of fit
  Double_t FI0,                        // input, FI0 result of fit
  Short_t *tempore,                    //  output result, associated skew hits
  Double_t *temporeS,                  //  output, associated skew hit  S
  Double_t *temporeZ,                  //  output, associated skew hits Z
  Double_t *temporeZDrift,             //  output, associated skew hit Z drift
  Double_t *temporeZErrorafterTilt,    //  output, associated skew hits Z error after tilt
  Int_t *STATUS                        // output
)
{

  Short_t NAssociated;
  Short_t sign;
  Int_t i, j, i1, ii, iii, Kincl, nlow, nup;

  Double_t bbb, tempZ[2], zmin, zmax, deltaz, zdist[2], zdist1, zdist2;

  Double_t allowed_distance = 4. * STRAWRADIUS / sin(SKEWinclination_DEGREES * PI / 180.);

  if (fabs(KAPPA) < 1.e-20) {
    *STATUS = -1;
    return 0;
  }

  NAssociated = 0;

  if (KAPPA > 0) {
    zmin = -FI0 / KAPPA;
    zmax = (2. * PI - FI0) / KAPPA;
  } else {
    zmax = -FI0 / KAPPA;
    zmin = (2. * PI - FI0) / KAPPA;
  }
  deltaz = zmax - zmin;

  for (i = 0; i < TemporarynSttSkewhitinTrack; i++) {
    bbb = (S[i] - FI0) / KAPPA;
    for (sign = 0; sign <= 1; sign++) {
      tempZ[sign] = Z[i] + (2 * sign - 1) * ZDrift[i];
      if (tempZ[sign] > zmax) {
        tempZ[sign] = fmod(tempZ[sign] - zmax, deltaz) + zmin;
      } else if (tempZ[sign] < zmin) {
        tempZ[sign] = fmod(tempZ[sign] - zmin, deltaz) + zmax;
      }

      zdist1 = fabs(bbb - tempZ[sign]);
      zdist2 = deltaz - zdist1;
      if (zdist2 < 0.)
        zdist2 = 0.; // protect against rounding errors.
      zdist[sign] = zdist1 < zdist2 ? zdist1 : zdist2;
    } //  end of for(sign=0;sign<=1; sign++)

    zdist1 = zdist[0] < zdist[1] ? zdist[0] : zdist[1];

    if (zdist1 < allowed_distance) {

      //       if(  zdist < 4.*ZErrorafterTilt[i] ){

      tempore[NAssociated] = SkewList[i][0];
      temporeS[NAssociated] = S[i];
      temporeZ[NAssociated] = Z[i];
      temporeZDrift[NAssociated] = ZDrift[i];
      temporeZErrorafterTilt[NAssociated] = ZErrorafterTilt[i];
      NAssociated++;
    }

  } //  end of for(i=0; i<TemporarynSttSkewhitinTrack; i++)

  *STATUS = 0;

  return NAssociated;
}

//----------end of function PndSttTrackFinderReal::AssociateBetterAfterFitSkewHitsToXYTrack

//----------begin of function PndSttTrackFinderReal::FitHelixCylinder

Short_t PndSttTrackFinderReal::FitHelixCylinder(Short_t nHitsinTrack, Double_t *Xconformal, Double_t *Yconformal, Double_t *DriftRadiusconformal,
                                                Double_t *ErrorDriftRadiusconformal, Double_t rotationangle, Double_t *trajectory_vertex, Short_t NMAX, Double_t *emme,
                                                Double_t *qu, Double_t *pAlfa, Double_t *pBeta, Double_t *pGamma, bool *Type)
{

  //   definition of variables for the glpsol  solver
  //    ROWS (for read_rows  function)
  //
  Short_t NpointsInFit = nHitsinTrack - NMAX < 0 ? nHitsinTrack : NMAX;

  bool mvdhit[NpointsInFit];

  Double_t M = 1., m_result, q_result, A, alfetta, angle, offsety, Delta[NpointsInFit], Ox[NpointsInFit], Oy[NpointsInFit];

  Short_t i, j, ii, iii, nSttHits, nMvdHits;
  Short_t Status;

  char nome[300], stringa[300], stringa2[300];
  float m1_result, m2_result, q1_result, q2_result, A1_result, A2_result;

  // --

  //-------------- stampaggi
  if (istampa >= 3) {
    cout << "from FitHelixCylinder,prima di rotazione,  Evento " << IVOLTE << ", nHitsinTrack = " << nHitsinTrack << "\nfrom FitHelixCylinder, nPointsinFit = " << NpointsInFit
         << endl;
    for (i = 0; i < NpointsInFit; i++) {
      cout << "  Xconformal[" << i << "] = " << Xconformal[i] << ";   Yconformal[" << i << "] = " << Yconformal[i] << ",  drift radius conformal " << DriftRadiusconformal[i]
           << endl
           << "\tErrordiriftradiusconformal = " << ErrorDriftRadiusconformal[i] << endl;
    }
  }
  //------------ end stampaggi

  if (nHitsinTrack < 2) {
    return -1;
  }
  //  use the trick of increasing the rotation angle by 10 degrees in order to obtain always a positive m
  rotationangle -= PI / 18.;

  Double_t cose = cos(rotationangle), sine = sin(rotationangle);

  nSttHits = nMvdHits = 0;
  for (i = 0; i < NpointsInFit; i++) {
    Ox[i] = Xconformal[i] * cose + Yconformal[i] * sine;
    Oy[i] = -Xconformal[i] * sine + Yconformal[i] * cose;
    Delta[i] = 3. * ErrorDriftRadiusconformal[i]; //   3 times the Drift Radius

    if (DriftRadiusconformal[i] < 0.) {
      mvdhit[i] = true;
      nMvdHits++;
    } else {
      mvdhit[i] = false;
      nSttHits++;
    }
  }

  //-------------- stampaggi
  if (istampa >= 3) {
    cout << "from FitHelixCylinder, dopo rotazione, Evento " << IVOLTE << ", nHitsinTrack = " << nHitsinTrack << "\nfrom FitHelixCylinder, nPointsinFit = " << NpointsInFit << endl;
    for (i = 0; i < NpointsInFit; i++) {
      cout << "  Ox[" << i << "] = " << Ox[i] << ";   Oy[" << i << "] = " << Oy[i] << ",  Delta " << Delta[i] << endl;
    }
  }
  //------------ end stampaggi
  //--------- calculation of # structural variables (see Gianluigi's logbook pag. 236) etc.etc.

  int NStructVar = 4 + 1 + nMvdHits * 2 + nSttHits * 4;
  //               m1,m2,q1,q2     DUMMY     lam & sigma      lamp & lamm & sigmap & sigmam

  int nRows = 1 + nMvdHits * 4 + nSttHits * 9;
  //	         OBJECT     A,B,C,D        Ap,Bp,Cp,Dp,Am,Bm,Cm,Dm,LAMBDA

  //----  creating the various service arrays
  int typeRows[nRows];
  char *nameRows[nRows];
  char auxnameRows[nRows][20];

  int NStructRowsMax = 8 * NpointsInFit; //  maximum number of ROWS in which a
                                         //  structural variable (for instance M ) can be found
  double final_values[NStructVar];
  int NRowsInWhichStructVarArePresent[NStructVar];
  char *StructVarName[NStructVar];
  char auxStructVarName[NStructVar][20];
  char *NameRowsInWhichStructVarArePresent[NStructVar * NStructRowsMax];
  char aux[NStructVar * NStructRowsMax][20];
  double Coefficients[NStructVar * NStructRowsMax];

  //--------for RHS information
  double ValueB[nRows - 1]; // -1 because OBJECT dowsn't have a RHS boundary.
  //--------for RANGES information
  int nRanges = nSttHits;
  double ValueRanges[nRanges];
  char *NameRanges[nRanges];
  char auxNameRanges[nRanges][20];

  //--------for BOUNDS information
  int nBounds = NpointsInFit + nSttHits + 1;
  double BoundValue[nBounds];
  char *BoundStructVarName[nBounds];
  char auxBoundStructVarName[nBounds][20];
  char *TypeofBound[nBounds];
  char auxTypeofBound[nBounds][20];
  //--------end BOUNDS information

  //---------------------------------------------------

  //--- calculate array      NRowsInWhichStructVarArePresent
  NRowsInWhichStructVarArePresent[0] =                                //  this is for m1
    NRowsInWhichStructVarArePresent[1] =                              //  this is for m2
    NRowsInWhichStructVarArePresent[2] =                              //  this is for q1
    NRowsInWhichStructVarArePresent[3] = nMvdHits * 2 + nSttHits * 4; //  this is for q2
                                                                      //--- the following is for the  lam* (Mvd hits) or lamp* (Stt hits) structural variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 4;
    } else {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
    }
    ii++;
  }
  //--- the following is for the  lamm* (Mvd Hits, if any)  structural variables
  for (i = 0; i < NpointsInFit; i++) {
    if (!mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
      ii++;
    }
  }

  //--- the following is for the  sigma   structural variables
  for (i = 0; i < nMvdHits + 2 * nSttHits; i++) {
    NRowsInWhichStructVarArePresent[4 + ii + i] = 5;
  }
  //--- the following is for the    DUMMY    structural variable
  NRowsInWhichStructVarArePresent[4 + ii + nMvdHits + 2 * nSttHits] = nMvdHits * 4 + nSttHits * 8;

  //-----------------  write the ROWS  section

  sprintf(&(auxnameRows[0][0]), "OBJECT");
  nameRows[0] = &auxnameRows[0][0];
  typeRows[0] = GLP_FR;
  for (i = 0, ii = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "A%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "B%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "C%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "D%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      ii += 4;

    } else {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_UP;
      typeRows[6 + ii] = GLP_UP;
      typeRows[7 + ii] = GLP_UP;
      typeRows[8 + ii] = GLP_UP;
      typeRows[9 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "Ap%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "Bp%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "Cp%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "Dp%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      sprintf(&(auxnameRows[5 + ii][0]), "Am%d", i);
      nameRows[5 + ii] = &auxnameRows[5 + ii][0];
      sprintf(&(auxnameRows[6 + ii][0]), "Bm%d", i);
      nameRows[6 + ii] = &auxnameRows[6 + ii][0];
      sprintf(&(auxnameRows[7 + ii][0]), "Cm%d", i);
      nameRows[7 + ii] = &auxnameRows[7 + ii][0];
      sprintf(&(auxnameRows[8 + ii][0]), "Dm%d", i);
      nameRows[8 + ii] = &auxnameRows[8 + ii][0];
      sprintf(&(auxnameRows[9 + ii][0]), "LAMBDA%d", i);
      nameRows[9 + ii] = &auxnameRows[9 + ii][0];
      ii += 9;
    }
  }

  //-----------------  write the COLUMNS  section

  //      fprintf(FMCS,"COLUMNS\n");  //--------stampaggi

  //  Column variable  m1

  ii = 0;
  for (i = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      //---stampaggi
      //          fprintf(FMCS,"  m1 A%d   %g\n  m1 B%d  %g\n",i,Ox[i],i,-Ox[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Ox[i];
      Coefficients[ii + 1] = -Ox[i];
      ii += 2;
    } else {
      //---stampaggi
      //          fprintf(FMCS,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
      //                                  i,Ox[i],i,Ox[i],i,-Ox[i],i,-Ox[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Ox[i];
      Coefficients[ii + 1] = Ox[i];
      Coefficients[ii + 2] = -Ox[i];
      Coefficients[ii + 3] = -Ox[i];
      ii += 4;
    }
  }

  //  Column variable  m2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  m2 A%d  %g\n  m2 B%d  %g\n",//---stampaggi
      //                                  i,-Ox[i],i,Ox[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Ox[i];
      Coefficients[NStructRowsMax + ii + 1] = Ox[i];
      ii += 2;
    } else {
      //          fprintf(FMCS,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",//---stampaggi
      //                                  i,-Ox[i],i,-Ox[i],i,Ox[i],i,Ox[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Ox[i];
      Coefficients[NStructRowsMax + ii + 1] = -Ox[i];
      Coefficients[NStructRowsMax + ii + 2] = Ox[i];
      Coefficients[NStructRowsMax + ii + 3] = Ox[i];
      ii += 4;
    }
  }

  //  Column variable  q1
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q1 A%d   1.\n  q1 B%d  -1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = -1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 2] = -1.;
      Coefficients[2 * NStructRowsMax + ii + 3] = -1.;
      ii += 4;
    }
  }

  //  Column variable  q2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q2 A%d   -1.\n  q2 B%d   1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = 1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 2] = 1.;
      Coefficients[3 * NStructRowsMax + ii + 3] = 1.;
      ii += 4;
    }
  }

  //  Column variable  lambdap(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii = (4 + i) * NStructRowsMax;
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    //	if( mvdhit[i]){
    //  fprintf(FMCS,"  lam%d  A%d  %g  B%d  %g\n  lam%d  C%d  %g  D%d   %g\n",//---stampaggi
    //                      i,i,-M,i,-M, i , i,-M, i, M);//---stampaggi
    //	} else {
    //  fprintf(FMCS,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LAMBDA%d  1.\n",//---stampaggi
    //                      i,i,-M,i,-M, i , i,-M, i, M, i,i);//---stampaggi
    //	}

    if (!mvdhit[i])
      Coefficients[ii + 4] = 1.;
  }

  //  Column variable  lambdam(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    //         fprintf(FMCS,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LAMBDA%d  1.\n",//---stampaggi
    //                                                i,i,-M,i,-M, i,i , -M, i, M, i,i);//---stampaggi
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    Coefficients[ii + 4] = 1.;
  }
  //  Column variable  sigmap(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii += NStructRowsMax;

    /*
      if( mvdhit[i]){
          fprintf(FMCS,"  sigma%d  OBJECT  %g  A%d  -1.\n  sigma%d  B%d    -1. C%d  1.\n  sigma%d  D%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi

      } else {
          fprintf(FMCS,"  sigmap%d  OBJECT  %g  Ap%d  -1.\n  sigmap%d  Bp%d    -1. Cp%d  1.\n  sigmap%d  Dp%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
      }
    */

    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }
  //  Column variable  sigmam(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    //         fprintf(FMCS,"  sigmam%d  OBJECT %g  Am%d  -1.\n  sigmam%d  Bm%d   -1. Cm%d   1.\n  sigmam%d  Dm%d  -1.\n",//---stampaggi
    //                                                i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }

  //  Column variable  DUMMY
  //-----------stampaggi
  /*
        for(i=0 ; i< NpointsInFit ; i++) {
    if( mvdhit[i]){
           fprintf(FMCS,"  DUMMY     A%d  1.\n  DUMMY   B%d   1.\n  DUMMY    C%d   1.\n",i,i,i);
           fprintf(FMCS,"  DUMMY     D%d  1.\n",i);
    } else {
           fprintf(FMCS,"  DUMMY     Ap%d  1.      Am%d       1.\n  DUMMY   Bp%d   1.    Bm%d   1.\n  DUMMY    Cp%d   1.   Cm%d   1\n",
                                                  i,i,i,i,i,i);
           fprintf(FMCS,"  DUMMY     Dp%d  1.      Dm%d       1.\n",i,i);
    }
        }
  */
  //---fine stampaggi
  ii += NStructRowsMax;
  for (i = 0, iii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      iii += 4;
    } else {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      Coefficients[ii + iii + 4] = 1.;
      Coefficients[ii + iii + 5] = 1.;
      Coefficients[ii + iii + 6] = 1.;
      Coefficients[ii + iii + 7] = 1.;
      iii += 8;
    }
  }

  //-----------

  //--- give the names to the Structural Variables

  sprintf(&auxStructVarName[0][0], "m1", i);
  StructVarName[0] = &auxStructVarName[0][0];
  sprintf(&auxStructVarName[1][0], "m2", i);
  StructVarName[1] = &auxStructVarName[1][0];

  sprintf(&auxStructVarName[2][0], "q1", i);
  StructVarName[2] = &auxStructVarName[2][0];

  sprintf(&auxStructVarName[3][0], "q2", i);
  StructVarName[3] = &auxStructVarName[3][0];
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&auxStructVarName[4 + i][0], "lam%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "sigma%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];
    } else {
      sprintf(&auxStructVarName[4 + i][0], "lamp%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + ii][0], "lamm%d", i);
      StructVarName[4 + NpointsInFit + ii] = &auxStructVarName[4 + NpointsInFit + ii][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "sigmap%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0], "sigmam%d", i);
      StructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii] = &auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0];
      ii++;
    }
  }

  sprintf(&auxStructVarName[NStructVar - 1][0], "DUMMY", i);
  StructVarName[NStructVar - 1] = &auxStructVarName[NStructVar - 1][0];

  //--- give the names of those Rows in which the Structural Variables are present

  //  for m1, m2, q1, q2
  for (i = 0; i < 4; i++) {
    for (j = 0, ii = 0; j < NpointsInFit; j++) {
      if (mvdhit[j]) {
        sprintf(&aux[i * NStructRowsMax + ii][0], "A%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "B%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        ii += 2;
      } else {
        sprintf(&aux[i * NStructRowsMax + ii][0], "Ap%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "Am%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        sprintf(&aux[i * NStructRowsMax + ii + 2][0], "Bp%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 2] = &aux[i * NStructRowsMax + ii + 2][0];
        sprintf(&aux[i * NStructRowsMax + ii + 3][0], "Bm%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 3] = &aux[i * NStructRowsMax + ii + 3][0];
        ii += 4;
      }
    }
  }

  //  now for the    lamp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
    } else {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 4][0], "LAMBDA%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 4] = &aux[(i + 4) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    lamm*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 1] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 2] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 3] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0], "LAMBDA%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 4] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    sigmap*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0], "OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    } else {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0], "OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    sigmam*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //         sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax][0], "OBJECT");
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    DUMMY   variable
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      ii += 4;
    } else {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "Am%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "Bm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 4] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0], "Cm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 5] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 6] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0], "Dm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 7] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0];
      ii += 8;
    }
  }

  //-----------------  write the RHS  section

  //      fprintf(FMCS,"RHS\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  A%d  %g  B%d  %g\n  BOUND  C%d  %g  D%d  %g\n",
                    i, Oy[i]+2.*M,i,
                      -Oy[i]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oy[i] + 2. * M;
      ValueB[ii + 1] = -Oy[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;
      ii += 4;
    } else {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
                    i, Oy[i]+DriftRadiusconformal[ i ]+2.*M,i,
                      -Oy[i]-DriftRadiusconformal[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oy[i] + DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 1] = -Oy[i] - DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;

      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Am%d  %g  Bm%d  %g\n  BOUND  Cm%d  %g  Dm%d %g\n",
                    i, Oy[i]-DriftRadiusconformal[ i ]+2.*M,i,
                      -Oy[i]+DriftRadiusconformal[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
                fprintf(FMCS,"  BOUND  LAMBDA%d   1.\n",i);
      */
      //---fine stampaggi
      ValueB[ii + 4] = Oy[i] - DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 5] = -Oy[i] + DriftRadiusconformal[i] + 2. * M;
      ValueB[ii + 6] = Delta[i] + 2. * M;
      ValueB[ii + 7] = M - Delta[i] + 2. * M;
      ValueB[ii + 8] = 1.;
      ii += 9;
    }
  }

  //-----------------  write the RANGES  section

  //      fprintf(FMCS,"RANGES\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS,"  RANGE  LAMBDA%d  1.\n",i);//---stampaggi
    //---
    ValueRanges[ii] = 1.;
    sprintf(&auxNameRanges[ii][0], "LAMBDA%d", i);
    NameRanges[ii] = &auxNameRanges[ii][0];
    ii++;
  }

  //-----------------  write the BOUNDS  section
  //      fprintf(FMCS,"BOUNDS\n");//---stampaggi

  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS," BV  Bounds  lam%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lam%d", i);
    } else {
      //          fprintf(FMCS," BV  Bounds  lamp%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lamp%d", i);
    }

    BoundStructVarName[i] = &auxBoundStructVarName[i][0];
    BoundValue[i] = 0.;
  }

  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS," BV  Bounds  lamm%d\n", i);//---stampaggi
    sprintf(&auxTypeofBound[ii + NpointsInFit][0], "BV");
    TypeofBound[ii + NpointsInFit] = &auxTypeofBound[ii + NpointsInFit][0];
    sprintf(&auxBoundStructVarName[ii + NpointsInFit][0], "lamm%d", i);
    BoundStructVarName[ii + NpointsInFit] = &auxBoundStructVarName[ii + NpointsInFit][0];
    BoundValue[ii + NpointsInFit] = 0.;
    ii++;
  }

  //          fprintf(FMCS," FX  Bounds  DUMMY  %g\n",2.*M);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits][0], "DUMMY");
  BoundStructVarName[NpointsInFit + nSttHits] = &auxBoundStructVarName[NpointsInFit + nSttHits][0];
  BoundValue[NpointsInFit + nSttHits] = 2. * M;
  //-----

  //------------------------------------------------------------stampaggi
  /*
        fprintf(FMCS,"ENDATA\n");
        fclose(FMCS);
  */

  if (istampa >= 4) {

    cout << "n.  punti nel fit " << NpointsInFit << endl;

    cout << "nRows " << nRows << endl;
    for (int ic = 0; ic < nRows; ic++) {
      cout << "n.  Row  " << ic << ", nameRows " << nameRows[ic] << ",  typeRows " << typeRows[ic] << endl;
    }

    cout << "NStructRowsMax, NStructVar " << NStructRowsMax << ", " << NStructVar << endl;
    for (int ic = 0; ic < NStructVar; ic++) {
      cout << "NRowsInWhichStructVarArePresent  " << NRowsInWhichStructVarArePresent[ic] << ", nome var. strut. n." << ic << "  = " << StructVarName[ic] << endl;

      for (int jc = 0; jc < NRowsInWhichStructVarArePresent[ic]; jc++) {
        cout << "n. " << jc << "  NameRowsInWhichStructVarArePresent  " << NameRowsInWhichStructVarArePresent[ic * NStructRowsMax + jc] << endl;
      }
    }

    cout << "n Coefficient " << 21 * nMvdHits + 24 * nSttHits << endl;
    iii = 0;
    for (int ic = 0; ic < NStructVar; ic++) {
      cout << "Struct. Var." << StructVarName[ic] << " e' presente in " << NRowsInWhichStructVarArePresent[ic] << "  Rows;" << endl;
      for (ii = 0; ii < NRowsInWhichStructVarArePresent[ic]; ii++) {

        cout << "\tin Row " << NameRowsInWhichStructVarArePresent[ic * NStructRowsMax + ii] << ", ha  Coefficient   " << Coefficients[ic * NStructRowsMax + ii]
             << " (n. sequenziale = " << iii << ")" << endl;
        iii++;
      }
    }

    cout << "n valuesB " << nRows - 1 << endl;
    for (int ic = 0; ic < nRows - 1; ic++) {
      cout << "n. " << ic << ",  valuesB   " << ValueB[ic] << endl;
    }
    cout << "n ranges " << nRanges << endl;
    for (int ic = 0; ic < nRanges; ic++) {
      cout << "n. " << ic << ",  RANGES   " << ValueRanges[ic] << endl;
    }
    cout << "n Bounds " << nBounds << endl;
    for (int ic = 0; ic < nBounds; ic++) {
      cout << "n. " << ic << ",  Bounds   " << BoundValue[ic] << endl;
      cout << "n. " << ic << ",  Bound Type   " << TypeofBound[ic] << endl;
      cout << "n. " << ic << ",  Bound Name   " << BoundStructVarName[ic] << endl;
    }
  } // end of if(istampa

  //-------fine stampaggi

  //-----------------------  funzioni chiamate direttamente
  /*
  cout<<"cavolo, da sttmvdtracking : nRows = "<<nRows<<", NStructVar = "<<
    NStructVar<<", NStructRowsMax = "<<NStructRowsMax<<
    ", NRowsInWhichStructVarArePresent = "<<
    NRowsInWhichStructVarArePresent<<", nRanges = "<<nRanges
    <<", nBounds = "<<nBounds<<endl;
  */
  int status = glp_main(nRows, nameRows, typeRows,                                   //  ROWS info
                        NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent, //  COLUMNS info
                        StructVarName, NameRowsInWhichStructVarArePresent,           //  COLUMNS info
                        Coefficients,                                                //  COLUMNS info
                        ValueB,                                                      // RHS  info
                        nRanges, ValueRanges, NameRanges,                            //  RANGES  info
                        nBounds, BoundValue, BoundStructVarName,
                        TypeofBound //  BOUNDS info
                                    //      ,final_values,TIMEOUT
                        ,
                        final_values);
  if (status != 0)
    return -5;

  //--------stampaggi
  if (istampa >= 3) {
    printf("from FitHelixCylinder  printout dopo glp_main -------------------------------\n");
    printf("      number of structural variables %d\n", NStructVar);
    int ica;
    for (ica = 0; ica < NStructVar; ica++) {
      printf("name of structural variable %s and its final value %g\n", StructVarName[ica], final_values[ica]);
    }
    printf("from FitHelixCylinder  printout dopo glp_main  -------------------------------\n");
  } // end of if(istampa
    //--------fine stampaggi

  //-----------------------  fine funzioni chiamate direttamente

  /*
     if(istampa>=3 && IVOLTE <= 20){
       sprintf(stringa,
  "/home/boca/panda/glpk/glpk-4.39/examples/glpsol --min -o soluztrack%dEvent%dstep%d    GeneralParallelHitsConformeTraccia%dEvent%d.mcs",
                                  0,IVOLTE,1,0,IVOLTE);
     }  else {
       sprintf(stringa,
  "/home/boca/panda/glpk/glpk-4.39/examples/glpsol --min -o soluztrack%dEvent%dstep%d    GeneralParallelHitsConformeTraccia%dEvent%d.mcs >& /dev/null",
                                  0, IVOLTE,1,0,IVOLTE,1);
     }
  */

  m1_result = final_values[0];
  m2_result = final_values[1];
  q1_result = final_values[2];
  q2_result = final_values[3];

  if (istampa > 2)
    cout << "Results : m1 = " << m1_result << ", m2= " << m2_result << ", q1 = " << q1_result << ", q2 = " << q2_result << endl;

  //---------  case in which the fit failed
  if (final_values[0] == 0. && final_values[1] == 0. && final_values[2] == 0. && final_values[3] == 0.)
    return -10;

  //------------------------  transformation of the result in terms of ALFA, BETA, GAMMA

  *qu = q1_result - q2_result;
  *emme = m1_result - m2_result;

  *pGamma = 0.;
  if (fabs(*qu) > 1.e-10) { //  trajectory is a circle in XY space
    *pAlfa = *emme / (*qu);
    *pBeta = -1. / (*qu);
    *Type = true;
    //  now take into account the rotation and correct; the only affected quantities are ALFA and BETA
    alfetta = *pAlfa;
    *pAlfa = *pAlfa * cose - *pBeta * sine;
    *pBeta = alfetta * sine + *pBeta * cose;

  } else if (fabs(*emme) > 1.e-10) { //  trajectory is a straight line in XY space of equation y= m*x
    //  the rotation first
    angle = atan(*emme) + rotationangle;
    if (fabs(cos(angle)) > 1.e-10) {
      *pAlfa = 999999.;
      *pBeta = -(*pAlfa) / tan(angle);
      *Type = false;

    } else { //  in this case the equation in XY plane is y = 0.
      *pAlfa = 0.;
      *pBeta = 999999.;
      *Type = false;
    }
  } else { //  in this case also the equation in XY plane is  y = 0.
    *pAlfa = 0.;
    *pBeta = 999999.;
    *Type = false;
  } // end of 	if( fabs( *qu ) > 1.e-10)

  //------------------

  // now take into account the displacement and correct
  *pGamma += (trajectory_vertex[0] * trajectory_vertex[0] + trajectory_vertex[1] * trajectory_vertex[1] - *pAlfa * trajectory_vertex[0] - *pBeta * trajectory_vertex[1]);
  *pAlfa -= 2. * trajectory_vertex[0];
  *pBeta -= 2. * trajectory_vertex[1];

  if (fabs(cose - *emme * sine) > 1.e-10) {
    *qu = *qu / (cose - *emme * sine);
    *emme = (*emme * cose + sine) / (cose - *emme * sine);
    return 1;
  } else { //  in this case the equation is   0 = x+*qu .
    if (fabs(sine + *emme * cose) < 1.e-10) {
      cout << " From FitHelixCylinder, equation of XY circle : X**2 + Y**2 =0,"
           << " situation impossible in principle! Returning -1" << endl;
      return -1;
    }

    *emme = 1.;
    *qu = *qu / (sine + *emme * cose);
    return 99; //  in this case the equation is   0 = x+*qu .
  }
}

//----------end of function PndSttTrackFinderReal::FitHelixCylinder

//----------begin of function PndSttTrackFinderReal::FitSZspace

Short_t PndSttTrackFinderReal::FitSZspace(Short_t nSkewHitsinTrack, Double_t *S,
                                          Double_t *Z, //
                                          Double_t *DriftRadius,
                                          Double_t *ErrorDriftRadius, //
                                          Double_t FInot, Short_t NMAX, Double_t *emme)
{

  //   definition of variables for the glpsol  solver
  //    ROWS (for read_rows  function)
  //
  Short_t NpointsInFit = nSkewHitsinTrack - NMAX < 0 ? nSkewHitsinTrack : NMAX;

  bool mvdhit[NpointsInFit];

  Double_t ave, avex, avey, cose, sine, M = 50., m_result, q_result, A, alfetta, angle, offsety, rotationangle, Ox[NpointsInFit], Oy[NpointsInFit], Delta[NpointsInFit];

  Short_t i, j, ii, iii, n, nSttHits, nMvdHits;
  Short_t Status;

  char nome[300], stringa[300], stringa2[300];
  float m1_result, m2_result, q1_result, q2_result, A1_result, A2_result;

  // --

  if (nSkewHitsinTrack == 0) {
    cout << "from FitSZspace, Evento " << IVOLTE << endl;
    cout << "from PndSttTrackFinderReal::FitSZspace  :  no points in fit, return!\n";
    return -10;
  }

  //  use the trick of increasing the rotation angle by 10 degrees in order to
  //  obtain always a positive m
  //      rotationangle -= PI/18.;
  rotationangle = PI / 2.;

  cose = cos(rotationangle);
  sine = sin(rotationangle);

  nSttHits = nMvdHits = 0;

  for (i = 0; i < NpointsInFit; i++) {
    Ox[i] = Z[i] * cose + (S[i] - FInot) * sine;
    Oy[i] = -Z[i] * sine + (S[i] - FInot) * cose;
    Delta[i] = ErrorDriftRadius[i];

    if (DriftRadius[i] < 0.) //  not a STT hit.
    {
      mvdhit[i] = true;
      nMvdHits++;
    } else {
      mvdhit[i] = false;
      nSttHits++;
    }
  } // end of  for(i=0;i<NpointsInFit; i++)

  //--------- calculation of # structural variables (see Gianluigi's logbook pag. 236) etc.etc.

  int NStructVar = 4 + 1 + nMvdHits * 2 + nSttHits * 4;
  //                  m1,m2,q1,q2     DUMMY      lam & sigma            lamp & lamm & sigmap & sigmam

  int nRows = 1 + nMvdHits * 4 + nSttHits * 9;
  //	         OBJECT     A,B,C,D        Ap,Bp,Cp,Dp,Am,Bm,Cm,Dm,LAMBDA

  //----  creating the various service arrays
  int typeRows[nRows];
  char *nameRows[nRows];
  char auxnameRows[nRows][20];

  int NStructRowsMax = 8 * NpointsInFit; //  maximum number of ROWS in which a
                                         //  structural variable (for instance M ) can be found
  double final_values[NStructVar];
  int NRowsInWhichStructVarArePresent[NStructVar];
  char *StructVarName[NStructVar];
  char auxStructVarName[NStructVar][20];
  char *NameRowsInWhichStructVarArePresent[NStructVar * NStructRowsMax];
  char aux[NStructVar * NStructRowsMax][20];
  double Coefficients[NStructVar * NStructRowsMax];

  //--------for RHS information
  double ValueB[nRows - 1]; // -1 because OBJECT dowsn't have a RHS boundary.
  //--------for RANGES information
  int nRanges = nSttHits;
  double ValueRanges[nRanges];
  char *NameRanges[nRanges];
  char auxNameRanges[nRanges][20];

  //--------for BOUNDS information
  int nBounds = NpointsInFit + nSttHits + 1 + 2; // +2  because q1 =  q2 = 0 fixed.
  double BoundValue[nBounds];
  char *BoundStructVarName[nBounds];
  char auxBoundStructVarName[nBounds][20];
  char *TypeofBound[nBounds];
  char auxTypeofBound[nBounds][20];
  //--------end BOUNDS information

  //---------------------------------------------------

  //--- calculate array      NRowsInWhichStructVarArePresent
  NRowsInWhichStructVarArePresent[0] =                                //  this is for m1
    NRowsInWhichStructVarArePresent[1] =                              //  this is for m2
    NRowsInWhichStructVarArePresent[2] =                              //  this is for q1
    NRowsInWhichStructVarArePresent[3] = nMvdHits * 2 + nSttHits * 4; //  this is for q2
                                                                      //--- the following is for the  lam* (Mvd Hits) and lamp* (Stt hits) structural variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 4;
    } else {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
    }
    ii++;
  }

  //  the lamm* (Stt hits) if any.
  for (i = 0; i < NpointsInFit; i++) {
    if (!mvdhit[i]) {
      NRowsInWhichStructVarArePresent[4 + ii] = 5;
      ii++;
    }
  }

  //--- the following is for the  sigma   structural variables
  for (i = 0; i < nMvdHits + 2 * nSttHits; i++) {
    NRowsInWhichStructVarArePresent[4 + ii + i] = 5;
  }
  //--- the following is for the    DUMMY    structural variable
  NRowsInWhichStructVarArePresent[4 + ii + nMvdHits + 2 * nSttHits] = nMvdHits * 4 + nSttHits * 8;

  //-----------------  write the ROWS  section

  //-------stampaggi
  /*
        sprintf(nome,"GeneralSkewEvent%d.mcs", IVOLTE);
        FILE * FMCS = fopen(nome,"w");
        fprintf(FMCS,"NAME    FIT\n");


        fprintf(FMCS,"ROWS\n");
        fprintf(FMCS," N OBJECT\n");
        for(i=0 ; i< NpointsInFit ; i++) {
    if( mvdhit[i]){
             fprintf(FMCS," L A%d\n L B%d\n L C%d\n L D%d\n",i,i,i,i);
    }else{
             fprintf(FMCS," L Ap%d\n L Bp%d\n L Cp%d\n L Dp%d\n",i,i,i,i);
             fprintf(FMCS," L Am%d\n L Bm%d\n L Cm%d\n L Dm%d\n G LAMBDA%d\n",i,i,i,i,i);
    }
        }
  */
  //--------------------------fine stampaggi

  sprintf(&(auxnameRows[0][0]), "OBJECT");
  nameRows[0] = &auxnameRows[0][0];
  typeRows[0] = GLP_FR;
  for (i = 0, ii = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "A%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "B%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "C%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "D%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      ii += 4;

    } else {
      typeRows[1 + ii] = GLP_UP;
      typeRows[2 + ii] = GLP_UP;
      typeRows[3 + ii] = GLP_UP;
      typeRows[4 + ii] = GLP_UP;
      typeRows[5 + ii] = GLP_UP;
      typeRows[6 + ii] = GLP_UP;
      typeRows[7 + ii] = GLP_UP;
      typeRows[8 + ii] = GLP_UP;
      typeRows[9 + ii] = GLP_LO;

      sprintf(&(auxnameRows[1 + ii][0]), "Ap%d", i);
      nameRows[1 + ii] = &auxnameRows[1 + ii][0];
      sprintf(&(auxnameRows[2 + ii][0]), "Bp%d", i);
      nameRows[2 + ii] = &auxnameRows[2 + ii][0];
      sprintf(&(auxnameRows[3 + ii][0]), "Cp%d", i);
      nameRows[3 + ii] = &auxnameRows[3 + ii][0];
      sprintf(&(auxnameRows[4 + ii][0]), "Dp%d", i);
      nameRows[4 + ii] = &auxnameRows[4 + ii][0];
      sprintf(&(auxnameRows[5 + ii][0]), "Am%d", i);
      nameRows[5 + ii] = &auxnameRows[5 + ii][0];
      sprintf(&(auxnameRows[6 + ii][0]), "Bm%d", i);
      nameRows[6 + ii] = &auxnameRows[6 + ii][0];
      sprintf(&(auxnameRows[7 + ii][0]), "Cm%d", i);
      nameRows[7 + ii] = &auxnameRows[7 + ii][0];
      sprintf(&(auxnameRows[8 + ii][0]), "Dm%d", i);
      nameRows[8 + ii] = &auxnameRows[8 + ii][0];
      sprintf(&(auxnameRows[9 + ii][0]), "LAMBDA%d", i);
      nameRows[9 + ii] = &auxnameRows[9 + ii][0];
      ii += 9;
    }
  }

  //-----------------  write the COLUMNS  section

  //      fprintf(FMCS,"COLUMNS\n");  //--------stampaggi

  //  Column variable  m1

  ii = 0;
  for (i = 0; i < NpointsInFit; i++) {

    if (mvdhit[i]) {
      //---stampaggi
      //          fprintf(FMCS,"  m1 A%d   %g\n  m1 B%d  %g\n",i,Ox[i],i,-Ox[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Ox[i];
      Coefficients[ii + 1] = -Ox[i];
      ii += 2;
    } else {
      //---stampaggi
      //          fprintf(FMCS,"  m1 Ap%d  %g  Am%d  %g\n  m1 Bp%d  %g   Bm%d  %g\n",
      //                                  i,Ox[i],i,Ox[i],i,-Ox[i],i,-Ox[i]);
      //-----stampaggi, fine

      Coefficients[ii] = Ox[i];
      Coefficients[ii + 1] = Ox[i];
      Coefficients[ii + 2] = -Ox[i];
      Coefficients[ii + 3] = -Ox[i];
      ii += 4;
    }
  }

  //  Column variable  m2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  m2 A%d  %g\n  m2 B%d  %g\n",//---stampaggi
      //                                  i,-Ox[i],i,Ox[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Ox[i];
      Coefficients[NStructRowsMax + ii + 1] = Ox[i];
      ii += 2;
    } else {
      //          fprintf(FMCS,"  m2 Ap%d  %g  Am%d  %g\n  m2 Bp%d  %g   Bm%d  %g\n",//---stampaggi
      //                                  i,-Ox[i],i,-Ox[i],i,Ox[i],i,Ox[i]);//---stampaggi
      Coefficients[NStructRowsMax + ii] = -Ox[i];
      Coefficients[NStructRowsMax + ii + 1] = -Ox[i];
      Coefficients[NStructRowsMax + ii + 2] = Ox[i];
      Coefficients[NStructRowsMax + ii + 3] = Ox[i];
      ii += 4;
    }
  }

  //  Column variable  q1
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q1 A%d   1.\n  q1 B%d  -1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = -1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q1 Ap%d   1.  Am%d   1.\n  q1 Bp%d  -1.  Bm%d  -1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[2 * NStructRowsMax + ii] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 1] = 1.;
      Coefficients[2 * NStructRowsMax + ii + 2] = -1.;
      Coefficients[2 * NStructRowsMax + ii + 3] = -1.;
      ii += 4;
    }
  }

  //  Column variable  q2
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS,"  q2 A%d   -1.\n  q2 B%d   1.\n",//---stampaggi
      //                                  i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = 1.;
      ii += 2;
    } else {
      //          fprintf(FMCS,"  q2 Ap%d   -1.  Am%d   -1.\n  q2 Bp%d   1.   Bm%d   1.\n",//---stampaggi
      //                                  i,i,i,i);//---stampaggi
      Coefficients[3 * NStructRowsMax + ii] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 1] = -1.;
      Coefficients[3 * NStructRowsMax + ii + 2] = 1.;
      Coefficients[3 * NStructRowsMax + ii + 3] = 1.;
      ii += 4;
    }
  }

  //  Column variable  lambdap(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii = (4 + i) * NStructRowsMax;
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    /*
      if( mvdhit[i]){
      fprintf(FMCS,"  lam%d  A%d  %g  B%d  %g\n  lam%d  C%d  %g  D%d   %g\n",//---stampaggi
                          i,i,-M,i,-M, i , i,-M, i, M);//---stampaggi
      } else {
      fprintf(FMCS,"  lamp%d  Ap%d  %g  Bp%d  %g\n  lamp%d  Cp%d  %g  Dp%d   %g\n  lamp%d  LAMBDA%d  1.\n",//---stampaggi
                          i,i,-M,i,-M, i , i,-M, i, M, i,i);//---stampaggi
      }
    */
    if (!mvdhit[i])
      Coefficients[ii + 4] = 1.;
  }

  //  Column variable  lambdam(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    //         fprintf(FMCS,"  lamm%d  Am%d  %g  Bm%d  %g\n  lamm%d  Cm%d  %g  Dm%d %g\n  lamm%d  LAMBDA%d  1.\n",//---stampaggi
    //                     i,i,-M,i,-M, i,i , -M, i, M, i,i);//---stampaggi
    Coefficients[ii] = -M;
    Coefficients[ii + 1] = -M;
    Coefficients[ii + 2] = -M;
    Coefficients[ii + 3] = M;
    Coefficients[ii + 4] = 1.;
  }
  //  Column variable  sigmap(i)
  for (i = 0; i < NpointsInFit; i++) {
    ii += NStructRowsMax;

    /*
      if( mvdhit[i]){
          fprintf(FMCS,"  sigma%d  OBJECT  %g  A%d  -1.\n  sigma%d  B%d    -1. C%d  1.\n  sigma%d  D%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi

      } else {
          fprintf(FMCS,"  sigmap%d  OBJECT  %g  Ap%d  -1.\n  sigmap%d  Bp%d    -1. Cp%d  1.\n  sigmap%d  Dp%d -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
      }
    */

    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }
  //  Column variable  sigmam(i)
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    ii += NStructRowsMax;
    /*
      fprintf(FMCS,"  sigmam%d  OBJECT %g  Am%d  -1.\n  sigmam%d  Bm%d   -1. Cm%d   1.\n  sigmam%d  Dm%d  -1.\n",//---stampaggi
                                                    i,1./Delta[i],i,i,i,i,i,i);//---stampaggi
    */
    Coefficients[ii] = 1. / Delta[i];
    Coefficients[ii + 1] = -1.;
    Coefficients[ii + 2] = -1.;
    Coefficients[ii + 3] = 1.;
    Coefficients[ii + 4] = -1.;
  }

  //  Column variable  DUMMY
  //-----------stampaggi
  /*
        for(i=0 ; i< NpointsInFit ; i++) {
    if( mvdhit[i]){
           fprintf(FMCS,"  DUMMY     A%d  1.\n  DUMMY   B%d   1.\n  DUMMY    C%d   1.\n",i,i,i);
           fprintf(FMCS,"  DUMMY     D%d  1.\n",i);
    } else {
           fprintf(FMCS,"  DUMMY     Ap%d  1.      Am%d       1.\n  DUMMY   Bp%d   1.    Bm%d   1.\n  DUMMY    Cp%d   1.   Cm%d   1\n",
                                                  i,i,i,i,i,i);
           fprintf(FMCS,"  DUMMY     Dp%d  1.      Dm%d       1.\n",i,i);
    }
        }
  */
  //---fine stampaggi
  ii += NStructRowsMax;
  for (i = 0, iii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      iii += 4;
    } else {
      Coefficients[ii + iii] = 1.;
      Coefficients[ii + iii + 1] = 1.;
      Coefficients[ii + iii + 2] = 1.;
      Coefficients[ii + iii + 3] = 1.;
      Coefficients[ii + iii + 4] = 1.;
      Coefficients[ii + iii + 5] = 1.;
      Coefficients[ii + iii + 6] = 1.;
      Coefficients[ii + iii + 7] = 1.;
      iii += 8;
    }
  }

  //-----------

  //--- give the names to the Structural Variables

  sprintf(&auxStructVarName[0][0], "m1", i);
  StructVarName[0] = &auxStructVarName[0][0];
  sprintf(&auxStructVarName[1][0], "m2", i);
  StructVarName[1] = &auxStructVarName[1][0];

  sprintf(&auxStructVarName[2][0], "q1", i);
  StructVarName[2] = &auxStructVarName[2][0];

  sprintf(&auxStructVarName[3][0], "q2", i);
  StructVarName[3] = &auxStructVarName[3][0];
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&auxStructVarName[4 + i][0], "lam%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "sigma%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];
    } else {
      sprintf(&auxStructVarName[4 + i][0], "lamp%d", i);
      StructVarName[4 + i] = &auxStructVarName[4 + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + ii][0], "lamm%d", i);
      StructVarName[4 + NpointsInFit + ii] = &auxStructVarName[4 + NpointsInFit + ii][0];

      sprintf(&auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0], "sigmap%d", i);
      StructVarName[4 + nMvdHits + 2 * nSttHits + i] = &auxStructVarName[4 + nMvdHits + 2 * nSttHits + i][0];

      sprintf(&auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0], "sigmam%d", i);
      StructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii] = &auxStructVarName[4 + NpointsInFit + nMvdHits + 2 * nSttHits + ii][0];
      ii++;
    }
  }

  sprintf(&auxStructVarName[NStructVar - 1][0], "DUMMY", i);
  StructVarName[NStructVar - 1] = &auxStructVarName[NStructVar - 1][0];

  //--- give the names of those Rows in which the Structural Variables are present

  //  for m1, m2, q1, q2
  for (i = 0; i < 4; i++) {
    for (j = 0, ii = 0; j < NpointsInFit; j++) {
      if (mvdhit[j]) {
        sprintf(&aux[i * NStructRowsMax + ii][0], "A%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "B%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        ii += 2;
      } else {
        sprintf(&aux[i * NStructRowsMax + ii][0], "Ap%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii] = &aux[i * NStructRowsMax + ii][0];
        sprintf(&aux[i * NStructRowsMax + ii + 1][0], "Am%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 1] = &aux[i * NStructRowsMax + ii + 1][0];
        sprintf(&aux[i * NStructRowsMax + ii + 2][0], "Bp%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 2] = &aux[i * NStructRowsMax + ii + 2][0];
        sprintf(&aux[i * NStructRowsMax + ii + 3][0], "Bm%d", j);
        NameRowsInWhichStructVarArePresent[i * NStructRowsMax + ii + 3] = &aux[i * NStructRowsMax + ii + 3][0];
        ii += 4;
      }
    }
  }

  //  now for the    lamp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
    } else {
      sprintf(&aux[(i + 4) * NStructRowsMax + 0][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 0] = &aux[(i + 4) * NStructRowsMax + 0][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 1][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 1] = &aux[(i + 4) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 2][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 2] = &aux[(i + 4) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 3][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 3] = &aux[(i + 4) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4) * NStructRowsMax + 4][0], "LAMBDA%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4) * NStructRowsMax + 4] = &aux[(i + 4) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    lamm*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 1] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 1][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 2] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 2][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 3] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 3][0];
    sprintf(&aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0], "LAMBDA%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + NpointsInFit) * NStructRowsMax + 4] = &aux[(ii + 4 + NpointsInFit) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    sigmap*   variables
  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0], "OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    } else {
      //         sprintf(&aux[(i+4+NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0], "OBJECT");
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 1][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 2][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 3][0];
      sprintf(&aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(i + 4 + NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    }
  }

  //  now for the    sigmam*   variables
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //        sprintf(&aux[(ii+4+2*NpointsInFit+nSttHits)*NStructRowsMax][0],"OBJECT",i);
    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax][0], "OBJECT");
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 1][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 2][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 3][0];

    sprintf(&aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4] = &aux[(ii + 4 + 2 * NpointsInFit + nSttHits) * NStructRowsMax + 4][0];
    ii++;
  }

  //  now for the    DUMMY   variable
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "A%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "B%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "C%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "D%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      ii += 4;
    } else {
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii][0], "Ap%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii] = &aux[(NStructVar - 1) * NStructRowsMax + ii][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0], "Am%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 1] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 1][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0], "Bp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 2] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 2][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0], "Bm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 3] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 3][0];
      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0], "Cp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 4] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 4][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0], "Cm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 5] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 5][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0], "Dp%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 6] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 6][0];

      sprintf(&aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0], "Dm%d", i);
      NameRowsInWhichStructVarArePresent[(NStructVar - 1) * NStructRowsMax + ii + 7] = &aux[(NStructVar - 1) * NStructRowsMax + ii + 7][0];
      ii += 8;
    }
  }

  //-----------------  write the RHS  section

  //      fprintf(FMCS,"RHS\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  A%d  %g  B%d  %g\n  BOUND  C%d  %g  D%d  %g\n",
                    i, Oy[i]+2.*M,i,
                      -Oy[i]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oy[i] + 2. * M;
      ValueB[ii + 1] = -Oy[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;
      ii += 4;
    } else {
      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Ap%d  %g  Bp%d  %g\n  BOUND  Cp%d  %g  Dp%d  %g\n",
                    i, Oy[i]+DriftRadius[ i ]+2.*M,i,
                      -Oy[i]-DriftRadius[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);
      */
      //---fine stampaggi
      ValueB[ii] = Oy[i] + DriftRadius[i] + 2. * M;
      ValueB[ii + 1] = -Oy[i] - DriftRadius[i] + 2. * M;
      ValueB[ii + 2] = Delta[i] + 2. * M;
      ValueB[ii + 3] = M - Delta[i] + 2. * M;

      //---stampaggi
      /*
                fprintf(FMCS,"  BOUND  Am%d  %g  Bm%d  %g\n  BOUND  Cm%d  %g  Dm%d %g\n",
                    i, Oy[i]-DriftRadius[ i ]+2.*M,i,
                      -Oy[i]+DriftRadius[ i ]+2.*M,i,
                       Delta[i]+2.*M,i,M-Delta[i]+2.*M);f
                fprintf(FMCS,"  BOUND  LAMBDA%d   1.\n",i);
      */
      //---fine stampaggi
      ValueB[ii + 4] = Oy[i] - DriftRadius[i] + 2. * M;
      ValueB[ii + 5] = -Oy[i] + DriftRadius[i] + 2. * M;
      ValueB[ii + 6] = Delta[i] + 2. * M;
      ValueB[ii + 7] = M - Delta[i] + 2. * M;
      ValueB[ii + 8] = 1.;
      ii += 9;
    }
  }

  //-----------------  write the RANGES  section

  //      fprintf(FMCS,"RANGES\n");//---stampaggi
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS,"  RANGE  LAMBDA%d  1.\n",i);//---stampaggi
    //---
    ValueRanges[ii] = 1.;
    sprintf(&auxNameRanges[ii][0], "LAMBDA%d", i);
    NameRanges[ii] = &auxNameRanges[ii][0];
    ii++;
  }

  //-----------------  write the BOUNDS  section
  //      fprintf(FMCS,"BOUNDS\n");//---stampaggi

  for (i = 0; i < NpointsInFit; i++) {
    if (mvdhit[i]) {
      //          fprintf(FMCS," BV  Bounds  lam%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lam%d", i);
    } else {
      //          fprintf(FMCS," BV  Bounds  lamp%d\n",  i);//---stampaggi
      sprintf(&auxTypeofBound[i][0], "BV");
      TypeofBound[i] = &auxTypeofBound[i][0];
      sprintf(&auxBoundStructVarName[i][0], "lamp%d", i);
    }

    BoundStructVarName[i] = &auxBoundStructVarName[i][0];
    BoundValue[i] = 0.;
  }

  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    if (mvdhit[i])
      continue;
    //          fprintf(FMCS," BV  Bounds  lamm%d\n", i);//---stampaggi
    sprintf(&auxTypeofBound[ii + NpointsInFit][0], "BV");
    TypeofBound[ii + NpointsInFit] = &auxTypeofBound[ii + NpointsInFit][0];
    sprintf(&auxBoundStructVarName[ii + NpointsInFit][0], "lamm%d", i);
    BoundStructVarName[ii + NpointsInFit] = &auxBoundStructVarName[ii + NpointsInFit][0];
    BoundValue[ii + NpointsInFit] = 0.;
    ii++;
  }

  //          fprintf(FMCS," FX  Bounds  DUMMY  %g\n",2.*M);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxTypeofBound[NpointsInFit + nSttHits][0], "FX");
  TypeofBound[NpointsInFit + nSttHits] = &auxTypeofBound[NpointsInFit + nSttHits][0];

  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits][0], "DUMMY");
  BoundStructVarName[NpointsInFit + nSttHits] = &auxBoundStructVarName[NpointsInFit + nSttHits][0];
  BoundValue[NpointsInFit + nSttHits] = 2. * M;

  //   fixing q1
  //          fprintf(FMCS," FX  Bounds  q1  %g\n",0.);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits + 1][0], "FX");
  TypeofBound[NpointsInFit + nSttHits + 1] = &auxTypeofBound[NpointsInFit + nSttHits + 1][0];
  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits + 1][0], "q1");
  BoundStructVarName[NpointsInFit + nSttHits + 1] = &auxBoundStructVarName[NpointsInFit + nSttHits + 1][0];
  BoundValue[NpointsInFit + nSttHits + 1] = 0.;
  //   fixing q2
  //          fprintf(FMCS," FX  Bounds  q2  %g\n",0.);//--stampaggi
  sprintf(&auxTypeofBound[NpointsInFit + nSttHits + 2][0], "FX");
  TypeofBound[NpointsInFit + nSttHits + 2] = &auxTypeofBound[NpointsInFit + nSttHits + 2][0];
  sprintf(&auxBoundStructVarName[NpointsInFit + nSttHits + 2][0], "q2");
  BoundStructVarName[NpointsInFit + nSttHits + 2] = &auxBoundStructVarName[NpointsInFit + nSttHits + 2][0];
  BoundValue[NpointsInFit + nSttHits + 2] = 0.;

  //-----

  //------------------------------------------------------------stampaggi
  /*
        fprintf(FMCS,"ENDATA\n");
        fclose(FMCS);
  */

  /*
  cout<<"n.  punti nel fit "<<NpointsInFit<<endl;


  cout<<"nRows "<<nRows<<endl;
  for(int ic =0;ic<nRows; ic++){
     cout<<"n.  Row  "<<ic<<", nameRows "<<nameRows[ic]<<",  typeRows "<<typeRows[ic]<<endl;
  }

  cout<<"NStructRowsMax = "<<NStructRowsMax<<endl;
  cout<<"NStructVar "<<NStructVar<<" e loro elenco "<<endl;
  for(int ic =0;ic<NStructVar; ic++){
    cout<<"\tvar. n. "<<ic<<", nome = "<<StructVarName[ic]<<endl;
  }



  for(int ic =0;ic<NStructVar; ic++){
     cout<<"NRowsInWhichStructVarArePresent  "<<NRowsInWhichStructVarArePresent[ic]
       <<", nome var. strut. n."<<ic<<"  = "
       <<StructVarName[ic]<<endl;

    for(int jc=0; jc<NRowsInWhichStructVarArePresent[ic];jc++){
     cout<<"n. "<<jc<<"  NameRowsInWhichStructVarArePresent  "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+jc]<<endl;
    }
  }


  cout<<"n Coefficient "<<21*nMvdHits+24*nSttHits<<endl;
  iii=0;
  for(int ic =0;ic<NStructVar; ic++){
     cout<<"Struct. Var."<< StructVarName[ic] <<" e' presente in "<< NRowsInWhichStructVarArePresent[ic]
    <<"  Rows;"<<endl;
    for(ii=0;ii<NRowsInWhichStructVarArePresent[ic];ii++){

     cout<<"\tin Row "<<NameRowsInWhichStructVarArePresent[ic*NStructRowsMax+ii]
           <<", ha  Coefficient   "<<Coefficients[ic*NStructRowsMax+ii]<<
    " (n. sequenziale = "<<iii<<")"<<endl;
    iii++;
    }
  }

  cout<<"n valuesB "<<nRows-1<<endl;
  for(int ic =0;ic<nRows-1; ic++){
     cout<<"n. "<<ic<<",  valuesB   "<<ValueB[ic]<<endl;
  }
  cout<<"n ranges "<<nRanges<<endl;
  for(int ic =0;ic<nRanges; ic++){
     cout<<"n. "<<ic<<",  RANGES   "<<ValueRanges[ic]<<endl;
  }
  cout<<"n Bounds "<<nBounds<<endl;
  for(int ic =0;ic<nBounds; ic++){
     cout<<"n. "<<ic<<",  Bounds   "<<BoundValue[ic]<<endl;
     cout<<"n. "<<ic<<",  Bound Type   "<<TypeofBound[ic]<<endl;
     cout<<"n. "<<ic<<",  Bound Name   "<<BoundStructVarName[ic]<<endl;
  }
  */

  //-------fine stampaggi

  //-----------------------  funzioni chiamate direttamente
  /*
  cout<<"cavolo2, da sttmvdtracking : nRows = "<<nRows<<", NStructVar = "<<
    NStructVar<<", NStructRowsMax = "<<NStructRowsMax<<
    ", NRowsInWhichStructVarArePresent = "<<
    NRowsInWhichStructVarArePresent<<", nRanges = "<<nRanges
    <<", nBounds = "<<nBounds<<endl;
  */
  int status = glp_main(nRows, nameRows, typeRows,                                   //  ROWS info
                        NStructVar, NStructRowsMax, NRowsInWhichStructVarArePresent, //  COLUMNS info
                        StructVarName, NameRowsInWhichStructVarArePresent,           //  COLUMNS info
                        Coefficients,                                                //  COLUMNS info
                        ValueB,                                                      // RHS  info
                        nRanges, ValueRanges, NameRanges,                            //  RANGES  info
                        nBounds, BoundValue, BoundStructVarName,
                        TypeofBound //  BOUNDS info
                                    //      ,final_values, TIMEOUT
                        ,
                        final_values);

  if (status != 0)
    return -5; // fit failed

  //--------stampaggi
  if (istampa >= 3) {
    printf("from PndSttTrackFinderReal:FitSZ, final printout dopo glpmain -------------------------------\n");
    printf("      number of structural variables %d\n", NStructVar);
    int ica;
    for (ica = 0; ica < NStructVar; ica++) {
      printf("name of structural variable %s and its final value %g\n", StructVarName[ica], final_values[ica]);
    }
    printf("from main, end of final printout  con routines chiamate direttamente -------------------------------\n");
  }
  //--------fine stampaggi

  //-----------------------  fine funzioni chiamate direttamente

  m1_result = final_values[0];
  m2_result = final_values[1];
  //     q1_result=final_values[2];
  //     q2_result=final_values[3];

  *emme = m1_result - m2_result;
  // taking into account the rotation + traslation that was performed and calculate emme and qu

  if (fabs(cose - *emme * sine) > 1.e-10) {
    *emme = ((*emme) * cose + sine) / (cose - (*emme) * sine);
    return 1;
  } else { //  in this case the equation is   0 = U in the Conformal plane --> x=0 in the XY plane.
    return -99;
  }
}

//----------end of function PndSttTrackFinderReal::FitSZspace

//----------begin of function PndSttTrackFinderReal::PndSttFitSZspacebis

Short_t PndSttTrackFinderReal::PndSttFitSZspacebis(Short_t nSttSkewhitinTrack, Double_t *S, Double_t *Z, Double_t *DriftRadius, Double_t FInot, Short_t NMAX, Double_t *emme)
{

  //   definition of variables for the glpsol  solver
  //    ROWS (for read_rows  function)
  //
  Short_t NpointsInFit = nSttSkewhitinTrack - NMAX < 0 ? nSttSkewhitinTrack : NMAX;
  int nRows = NpointsInFit * 9 + 1;
  int typeRows[nRows];
  char *nameRows[nRows];
  char auxnameRows[nRows][20];
  //-------  end ROWS information
  //--------begin COLUMNS information
  int NStructVar = 5 + NpointsInFit * 4; //  number of  structural variables
  int NStructRows = 8 * NpointsInFit;    //  maximum number of ROWS in which a structural variable can be found
  double final_values[NStructVar];
  int NRowsInWhichStructVarArePresent[NStructVar];
  char *StructVarName[NStructVar];
  char auxStructVarName[NStructVar][20];
  char *NameRowsInWhichStructVarArePresent[NStructVar * NStructRows];
  char aux[NStructVar * NStructRows][20];
  //      double Coefficients[NStructVar][NStructRows];
  double Coefficients[NStructVar * NStructRows];
  //--------end COLUMNS information
  //--------begin RHS information
  double ValueB[9 * NpointsInFit];
  //--------end RHS information
  //--------begin RANGES information
  int nRanges = NpointsInFit;
  double ValueRanges[nRanges];
  char *NameRanges[nRanges];
  char auxNameRanges[nRanges][20];
  //--------end RANGES information
  //--------start BOUNDS information
  int nBounds = 2 * NpointsInFit + 3;
  //      int nBounds=2*NpointsInFit+1;
  double BoundValue[nBounds];
  char *BoundStructVarName[nBounds];
  char auxBoundStructVarName[nBounds][20];
  char *TypeofBound[nBounds];
  char auxTypeofBound[nBounds][20];
  //--------end BOUNDS information

  //----------------------------------------------------

  Double_t M = 50., m_result, q_result, A, alfetta, angle, offsety, Ox[nmaxHits], Oy[nmaxHits], Delta[nmaxHits];

  Short_t i, ii;
  Short_t Status;

  char nome[300], stringa[300], stringa2[300];

  //     FILE * MACRO ;

  float m1_result, m2_result, q1_result, q2_result, A1_result, A2_result;

  // --

  //  rotation of 90 degrees

  for (i = 0; i < nSttSkewhitinTrack; i++) {

    Ox[i] = S[i] - FInot;
    Oy[i] = -Z[i];

    Delta[i] = 3. * STRAWRADIUS; //  STRAWRADIUS now is 0.5 cm
  }

  //-----------------  write the ROWS  section

  //--------
  sprintf(&auxnameRows[0][0], "OBJECT");
  nameRows[0] = &auxnameRows[0][0];
  typeRows[0] = GLP_FR;
  for (i = 0; i < NpointsInFit; i++) {
    ii = 9 * i;
    typeRows[1 + ii] = GLP_UP;
    typeRows[2 + ii] = GLP_UP;
    typeRows[3 + ii] = GLP_UP;
    typeRows[4 + ii] = GLP_UP;
    typeRows[5 + ii] = GLP_UP;
    typeRows[6 + ii] = GLP_UP;
    typeRows[7 + ii] = GLP_UP;
    typeRows[8 + ii] = GLP_UP;
    typeRows[9 + ii] = GLP_LO;

    sprintf(&(auxnameRows[1 + ii][0]), "Ap%d", i);
    nameRows[1 + ii] = &auxnameRows[1 + ii][0];
    sprintf(&(auxnameRows[2 + ii][0]), "Bp%d", i);
    nameRows[2 + ii] = &auxnameRows[2 + ii][0];
    sprintf(&(auxnameRows[3 + ii][0]), "Cp%d", i);
    nameRows[3 + ii] = &auxnameRows[3 + ii][0];
    sprintf(&(auxnameRows[4 + ii][0]), "Dp%d", i);
    nameRows[4 + ii] = &auxnameRows[4 + ii][0];
    sprintf(&(auxnameRows[5 + ii][0]), "Am%d", i);
    nameRows[5 + ii] = &auxnameRows[5 + ii][0];
    sprintf(&(auxnameRows[6 + ii][0]), "Bm%d", i);
    nameRows[6 + ii] = &auxnameRows[6 + ii][0];
    sprintf(&(auxnameRows[7 + ii][0]), "Cm%d", i);
    nameRows[7 + ii] = &auxnameRows[7 + ii][0];
    sprintf(&(auxnameRows[8 + ii][0]), "Dm%d", i);
    nameRows[8 + ii] = &auxnameRows[8 + ii][0];
    sprintf(&(auxnameRows[9 + ii][0]), "LAMBDA%d", i);
    nameRows[9 + ii] = &auxnameRows[9 + ii][0];
  }

  //-----------------  write the COLUMNS  section

  //  Column variable  m1
  for (i = 0, ii = 0; i < NpointsInFit; i++) {
    ii++;
    Coefficients[i * 4] = Ox[i];
    Coefficients[i * 4 + 1] = Ox[i];
    Coefficients[i * 4 + 2] = -Ox[i];
    Coefficients[i * 4 + 3] = -Ox[i];
  }

  //  Column variable  m2
  for (i = 0; i < NpointsInFit; i++) {
    Coefficients[NStructRows + i * 4] = -Ox[i];
    Coefficients[NStructRows + i * 4 + 1] = -Ox[i];
    Coefficients[NStructRows + i * 4 + 2] = Ox[i];
    Coefficients[NStructRows + i * 4 + 3] = Ox[i];
  }

  //  Column variable  q1
  for (i = 0; i < NpointsInFit; i++) {
    Coefficients[2 * NStructRows + i * 4] = 1.;
    Coefficients[2 * NStructRows + i * 4 + 1] = 1.;
    Coefficients[2 * NStructRows + i * 4 + 2] = -1.;
    Coefficients[2 * NStructRows + i * 4 + 3] = -1.;
  }

  //  Column variable  q2
  for (i = 0; i < NpointsInFit; i++) {
    Coefficients[3 * NStructRows + i * 4] = -1.;
    Coefficients[3 * NStructRows + i * 4 + 1] = -1.;
    Coefficients[3 * NStructRows + i * 4 + 2] = 1.;
    Coefficients[3 * NStructRows + i * 4 + 3] = 1.;
  }

  //  Column variable  lambdap(i)
  for (i = 0; i < NpointsInFit; i++) {
    Coefficients[(4 + i) * NStructRows + 0] = -M;
    Coefficients[(4 + i) * NStructRows + 1] = -M;
    Coefficients[(4 + i) * NStructRows + 2] = -M;
    Coefficients[(4 + i) * NStructRows + 3] = M;
    Coefficients[(4 + i) * NStructRows + 4] = 1.;
  }
  //  Column variable  lambdam(i)
  for (i = 0; i < NpointsInFit; i++) {
    Coefficients[(4 + i + NpointsInFit) * NStructRows + 0] = -M;
    Coefficients[(4 + i + NpointsInFit) * NStructRows + 1] = -M;
    Coefficients[(4 + i + NpointsInFit) * NStructRows + 2] = -M;
    Coefficients[(4 + i + NpointsInFit) * NStructRows + 3] = M;
    Coefficients[(4 + i + NpointsInFit) * NStructRows + 4] = 1.;
  }
  //  Column variable  sigmap(i)
  for (i = 0; i < NpointsInFit; i++) {

    Coefficients[(4 + i + 2 * NpointsInFit) * NStructRows + 0] = 1. / Delta[i];
    Coefficients[(4 + i + 2 * NpointsInFit) * NStructRows + 1] = -1.;
    Coefficients[(4 + i + 2 * NpointsInFit) * NStructRows + 2] = -1.;
    Coefficients[(4 + i + 2 * NpointsInFit) * NStructRows + 3] = 1.;
    Coefficients[(4 + i + 2 * NpointsInFit) * NStructRows + 4] = -1.;
  }
  //  Column variable  sigmam(i)
  for (i = 0; i < NpointsInFit; i++) {
    Coefficients[(4 + i + 3 * NpointsInFit) * NStructRows + 0] = 1. / Delta[i];
    Coefficients[(4 + i + 3 * NpointsInFit) * NStructRows + 1] = -1.;
    Coefficients[(4 + i + 3 * NpointsInFit) * NStructRows + 2] = -1.;
    Coefficients[(4 + i + 3 * NpointsInFit) * NStructRows + 3] = 1.;
    Coefficients[(4 + i + 3 * NpointsInFit) * NStructRows + 4] = -1.;
  }

  //  Column variable  DUMMY
  for (i = 0; i < NStructRows; i++) {
    Coefficients[(4 + 4 * NpointsInFit) * NStructRows + i] = 1.;
  }
  //--------------------
  sprintf(&auxStructVarName[0][0], "m1");
  StructVarName[0] = &auxStructVarName[0][0];
  NRowsInWhichStructVarArePresent[0] = 4 * NpointsInFit;

  sprintf(&auxStructVarName[1][0], "m2");
  StructVarName[1] = &auxStructVarName[1][0];
  NRowsInWhichStructVarArePresent[1] = 4 * NpointsInFit;

  sprintf(&auxStructVarName[2][0], "q1");
  StructVarName[2] = &auxStructVarName[2][0];
  NRowsInWhichStructVarArePresent[2] = 4 * NpointsInFit;

  sprintf(&auxStructVarName[3][0], "q2");
  StructVarName[3] = &auxStructVarName[3][0];
  NRowsInWhichStructVarArePresent[3] = 4 * NpointsInFit;
  for (i = 0; i < NpointsInFit; i++) {
    sprintf(&auxStructVarName[3 + i + 1][0], "lamp%d", i);
    StructVarName[4 + i] = &auxStructVarName[4 + i][0];
    NRowsInWhichStructVarArePresent[4 + i] = 5;

    sprintf(&auxStructVarName[4 + NpointsInFit + i][0], "lamm%d", i);
    StructVarName[4 + NpointsInFit + i] = &auxStructVarName[4 + NpointsInFit + i][0];
    NRowsInWhichStructVarArePresent[4 + NpointsInFit + i] = 5;

    sprintf(&auxStructVarName[4 + 2 * NpointsInFit + i][0], "sigmap%d", i);
    StructVarName[4 + 2 * NpointsInFit + i] = &auxStructVarName[4 + 2 * NpointsInFit + i][0];
    NRowsInWhichStructVarArePresent[4 + 2 * NpointsInFit + i] = 5;

    sprintf(&auxStructVarName[4 + 3 * NpointsInFit + i][0], "sigmam%d", i);
    StructVarName[4 + 3 * NpointsInFit + i] = &auxStructVarName[4 + 3 * NpointsInFit + i][0];
    NRowsInWhichStructVarArePresent[4 + 3 * NpointsInFit + i] = 5;
  }
  sprintf(&auxStructVarName[4 + 4 * NpointsInFit][0], "DUMMY");
  StructVarName[4 + 4 * NpointsInFit] = &auxStructVarName[4 + 4 * NpointsInFit][0];
  NRowsInWhichStructVarArePresent[4 + 4 * NpointsInFit] = NStructRows;
  //  for m1, m2, q1, q2
  for (i = 0; i < 4; i++) {
    for (ii = 0; ii < NpointsInFit; ii++) {
      sprintf(&aux[i * NStructRows + ii * 4][0], "Ap%d", ii);
      NameRowsInWhichStructVarArePresent[i * NStructRows + ii * 4] = &aux[i * NStructRows + ii * 4][0];
      sprintf(&aux[i * NStructRows + ii * 4 + 1][0], "Am%d", ii);
      NameRowsInWhichStructVarArePresent[i * NStructRows + ii * 4 + 1] = &aux[i * NStructRows + ii * 4 + 1][0];
      sprintf(&aux[i * NStructRows + ii * 4 + 2][0], "Bp%d", ii);
      NameRowsInWhichStructVarArePresent[i * NStructRows + ii * 4 + 2] = &aux[i * NStructRows + ii * 4 + 2][0];
      sprintf(&aux[i * NStructRows + ii * 4 + 3][0], "Bm%d", ii);
      NameRowsInWhichStructVarArePresent[i * NStructRows + ii * 4 + 3] = &aux[i * NStructRows + ii * 4 + 3][0];
    }
  }

  //  now for the    lamp*   variables
  for (i = 0; i < NpointsInFit; i++) {
    sprintf(&aux[(i + 4) * NStructRows + 0][0], "Ap%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4) * NStructRows + 0] = &aux[(i + 4) * NStructRows + 0][0];
    sprintf(&aux[(i + 4) * NStructRows + 1][0], "Bp%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4) * NStructRows + 1] = &aux[(i + 4) * NStructRows + 1][0];
    sprintf(&aux[(i + 4) * NStructRows + 2][0], "Cp%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4) * NStructRows + 2] = &aux[(i + 4) * NStructRows + 2][0];
    sprintf(&aux[(i + 4) * NStructRows + 3][0], "Dp%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4) * NStructRows + 3] = &aux[(i + 4) * NStructRows + 3][0];
    sprintf(&aux[(i + 4) * NStructRows + 4][0], "LAMBDA%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4) * NStructRows + 4] = &aux[(i + 4) * NStructRows + 4][0];
  }

  //  now for the    lamm*   variables
  for (i = 0; i < NpointsInFit; i++) {
    sprintf(&aux[(i + 4 + NpointsInFit) * NStructRows + 0][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit) * NStructRows + 0] = &aux[(i + 4 + NpointsInFit) * NStructRows + 0][0];
    sprintf(&aux[(i + 4 + NpointsInFit) * NStructRows + 1][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit) * NStructRows + 1] = &aux[(i + 4 + NpointsInFit) * NStructRows + 1][0];
    sprintf(&aux[(i + 4 + NpointsInFit) * NStructRows + 2][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit) * NStructRows + 2] = &aux[(i + 4 + NpointsInFit) * NStructRows + 2][0];
    sprintf(&aux[(i + 4 + NpointsInFit) * NStructRows + 3][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit) * NStructRows + 3] = &aux[(i + 4 + NpointsInFit) * NStructRows + 3][0];
    sprintf(&aux[(i + 4 + NpointsInFit) * NStructRows + 4][0], "LAMBDA%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + NpointsInFit) * NStructRows + 4] = &aux[(i + 4 + NpointsInFit) * NStructRows + 4][0];
  }

  //  now for the    sigmap*   variables
  for (i = 0; i < NpointsInFit; i++) {
    //         sprintf(&aux[(i+4+2*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
    sprintf(&aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 0][0], "OBJECT");
    NameRowsInWhichStructVarArePresent[(i + 4 + 2 * NpointsInFit) * NStructRows + 0] = &aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 0][0];
    sprintf(&aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 1][0], "Ap%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 2 * NpointsInFit) * NStructRows + 1] = &aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 1][0];
    sprintf(&aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 2][0], "Bp%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 2 * NpointsInFit) * NStructRows + 2] = &aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 2][0];
    sprintf(&aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 3][0], "Cp%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 2 * NpointsInFit) * NStructRows + 3] = &aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 3][0];
    sprintf(&aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 4][0], "Dp%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 2 * NpointsInFit) * NStructRows + 4] = &aux[(i + 4 + 2 * NpointsInFit) * NStructRows + 4][0];
  }

  //  now for the    sigmam*   variables
  for (i = 0; i < NpointsInFit; i++) {
    //         sprintf(&aux[(i+4+3*NpointsInFit)*NStructRows+0][0],"OBJECT",i);
    sprintf(&aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 0][0], "OBJECT");
    NameRowsInWhichStructVarArePresent[(i + 4 + 3 * NpointsInFit) * NStructRows + 0] = &aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 0][0];
    sprintf(&aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 1][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 3 * NpointsInFit) * NStructRows + 1] = &aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 1][0];
    sprintf(&aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 2][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 3 * NpointsInFit) * NStructRows + 2] = &aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 2][0];
    sprintf(&aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 3][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 3 * NpointsInFit) * NStructRows + 3] = &aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 3][0];
    sprintf(&aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 4][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(i + 4 + 3 * NpointsInFit) * NStructRows + 4] = &aux[(i + 4 + 3 * NpointsInFit) * NStructRows + 4][0];
  }

  //  now for the    DUMMY   variable
  for (i = 0; i < NpointsInFit; i++) {
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 8 * i][0], "Ap%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + i * 8] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 8 * i][0];

    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 1 + 8 * i][0], "Am%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 1 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 1 + 8 * i][0];
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 2 + 8 * i][0], "Bp%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 2 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 2 + 8 * i][0];
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 3 + 8 * i][0], "Bm%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 3 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 3 + 8 * i][0];
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 4 + 8 * i][0], "Cp%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 4 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 4 + 8 * i][0];
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 5 + 8 * i][0], "Cm%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 5 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 5 + 8 * i][0];
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 6 + 8 * i][0], "Dp%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 6 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 6 + 8 * i][0];
    sprintf(&aux[(4 + 4 * NpointsInFit) * NStructRows + 7 + 8 * i][0], "Dm%d", i);
    NameRowsInWhichStructVarArePresent[(4 + 4 * NpointsInFit) * NStructRows + 7 + 8 * i] = &aux[(4 + 4 * NpointsInFit) * NStructRows + 7 + 8 * i][0];
  }

  //-----------------  write the RHS  section

  for (i = 0; i < NpointsInFit; i++) {
    ValueB[i * 9] = Oy[i] + DriftRadius[i] + 2. * M;
    ValueB[i * 9 + 1] = -Oy[i] - DriftRadius[i] + 2. * M;
    ValueB[i * 9 + 2] = Delta[i] + 2. * M;
    ValueB[i * 9 + 3] = M - Delta[i] + 2. * M;
    ValueB[i * 9 + 4] = Oy[i] - DriftRadius[i] + 2. * M;
    ValueB[i * 9 + 5] = -Oy[i] + DriftRadius[i] + 2. * M;
    ValueB[i * 9 + 6] = Delta[i] + 2. * M;
    ValueB[i * 9 + 7] = M - Delta[i] + 2. * M;
    ValueB[i * 9 + 8] = 1.;
  }

  //-----------------  write the RANGES  section

  for (i = 0; i < NpointsInFit; i++) {
    ValueRanges[i] = 1.;
    sprintf(&auxNameRanges[i][0], "LAMBDA%d", i);
    NameRanges[i] = &auxNameRanges[i][0];
  }

  //-----------------  write the BOUNDS  section

  for (i = 0; i < NpointsInFit; i++) {
    sprintf(&auxTypeofBound[i][0], "BV");
    TypeofBound[i] = &auxTypeofBound[i][0];
    sprintf(&auxBoundStructVarName[i][0], "lamp%d", i);
    BoundStructVarName[i] = &auxBoundStructVarName[i][0];
    BoundValue[i] = 0.;
  }

  for (i = 0; i < NpointsInFit; i++) {
    sprintf(&auxTypeofBound[i + NpointsInFit][0], "BV");
    TypeofBound[i + NpointsInFit] = &auxTypeofBound[i + NpointsInFit][0];
    sprintf(&auxBoundStructVarName[i + NpointsInFit][0], "lamm%d", i);
    BoundStructVarName[i + NpointsInFit] = &auxBoundStructVarName[i + NpointsInFit][0];
    BoundValue[i + NpointsInFit] = 0.;
  }

  sprintf(&auxTypeofBound[2 * NpointsInFit][0], "FX");
  TypeofBound[2 * NpointsInFit] = &auxTypeofBound[2 * NpointsInFit][0];
  sprintf(&auxBoundStructVarName[2 * NpointsInFit][0], "DUMMY");
  BoundStructVarName[2 * NpointsInFit] = &auxBoundStructVarName[2 * NpointsInFit][0];
  BoundValue[2 * NpointsInFit] = 2. * M;

  //   fixing q1
  sprintf(&auxTypeofBound[2 * NpointsInFit + 1][0], "FX");
  TypeofBound[2 * NpointsInFit + 1] = &auxTypeofBound[2 * NpointsInFit + 1][0];
  sprintf(&auxBoundStructVarName[2 * NpointsInFit + 1][0], "q1");
  BoundStructVarName[2 * NpointsInFit + 1] = &auxBoundStructVarName[2 * NpointsInFit + 1][0];
  BoundValue[2 * NpointsInFit + 1] = 0.;
  //   fixing q2
  sprintf(&auxTypeofBound[2 * NpointsInFit + 2][0], "FX");
  TypeofBound[2 * NpointsInFit + 2] = &auxTypeofBound[2 * NpointsInFit + 2][0];
  sprintf(&auxBoundStructVarName[2 * NpointsInFit + 2][0], "q2");
  BoundStructVarName[2 * NpointsInFit + 2] = &auxBoundStructVarName[2 * NpointsInFit + 2][0];
  BoundValue[2 * NpointsInFit + 2] = 0.;

  //-----

  //----------------------  calling the minimizer

  //  WHEN THE FIT WENT WELL, STATUS = 0

  int status = glp_main(nRows, nameRows, typeRows,                                //  ROWS info
                        NStructVar, NStructRows, NRowsInWhichStructVarArePresent, //  COLUMNS info
                        StructVarName, NameRowsInWhichStructVarArePresent,        //  COLUMNS info
                        Coefficients,                                             //  COLUMNS info
                        ValueB,                                                   // RHS  info
                        nRanges, ValueRanges, NameRanges,                         //  RANGES  info
                        nBounds, BoundValue, BoundStructVarName,
                        TypeofBound //  BOUNDS info
                                    //      ,final_values, TIMEOUT  //  timeout is in seconds.
                        ,
                        final_values);

  if (status != 0)
    return -100;

  //------------------------------------------

  m1_result = final_values[0];
  m2_result = final_values[1];
  //     q1_result=final_values[2];
  //     q2_result=final_values[3];

  //------------------------  transformation of the result in terms of ALFA, BETA, GAMMA

  // taking into account the rotation + traslation that was performed and calculate emme and qu

  *emme = m1_result - m2_result;
  //     *qu = FInot;

  if (fabs((*emme)) > 1.e-10) {
    *emme = -1. / (*emme);
    return 1;
  } else {

    return -99;
  }
}

//----------end of function PndSttTrackFinderReal::PndSttFitSZspacebis

//----------begin of function PndSttTrackFinderReal::PndSttOrderingParallel

void PndSttTrackFinderReal::PndSttOrderingParallel(Double_t oX, Double_t oY, Double_t info[][7], Short_t nParallelHits, Short_t *ListParallelHits, Short_t *Infoparal,
                                                   Short_t Charge,                            // input
                                                   Double_t *Fi_initial_helix_referenceframe, // output
                                                   Double_t *Fi_final_helix_referenceframe,   // output
                                                   Double_t *U,                               // output
                                                   Double_t *V                                // output
)
{

  Short_t i, j, tmp[nParallelHits];
  Double_t aaa, b1, firstR2, lastR2, aux[nParallelHits];

  //  here there is the ordering of the hits, under the assumption that the circumference
  //  in XY goes through (0,0).
  //  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
  //  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
  //  The scheme for the ordering of the hit is as follows :
  //  1)  order hits by increasing U or V of the conformal mapping; see Gianluigi's Logbook page 283;
  //  2)  find the charge of the track by checking if it is closest to the center in XY
  //	the first or the last of the ordered hits.
  //  3)  in case, invert the ordering of U, V and ListParallelHits such that the first hits in the
  //	list are alway those closer to the (0,0).

  //   ordering of the hits

  aaa = atan2(oY, oX); // atan2 defined between -PI and PI.

  // the following statement is necessary since for unknown reason the root interpreter
  // gives a weird error when using PI directly in the if statement below!!!!!!! I lost
  // 2 hours trying to figure this out!
  b1 = PI / 4.;

  if ((aaa > b1 && aaa < 3. * b1) || (aaa > -3. * b1 && aaa < -b1)) { // use U as ordering variable;
    //[case 1 or 3 Gianluigi's Logbook page 285].
    for (j = 0; j < nParallelHits; j++) {
      U[j] = info[Infoparal[ListParallelHits[j]]][0] / (info[Infoparal[ListParallelHits[j]]][0] * info[Infoparal[ListParallelHits[j]]][0] +
                                                        info[Infoparal[ListParallelHits[j]]][1] * info[Infoparal[ListParallelHits[j]]][1]);
    }
    Merge_Sort(nParallelHits, U, ListParallelHits);

    if ((aaa > b1 && aaa < 3. * b1)) { //  case #1;
      if (Charge == -1) {
        // inverting the order of the hits.
        for (i = 0; i < nParallelHits; i++) {
          tmp[i] = ListParallelHits[nParallelHits - 1 - i];
          aux[i] = U[nParallelHits - 1 - i];
        }
        for (i = 0; i < nParallelHits; i++) {
          ListParallelHits[i] = tmp[i];
          U[i] = aux[i];
        }
      }
      for (j = 0; j < nParallelHits; j++) {
        V[j] = info[Infoparal[ListParallelHits[j]]][1] / (info[Infoparal[ListParallelHits[j]]][0] * info[Infoparal[ListParallelHits[j]]][0] +
                                                          info[Infoparal[ListParallelHits[j]]][1] * info[Infoparal[ListParallelHits[j]]][1]);
      }
    } else { //  case # 3.
      if (Charge == 1) {
        // inverting the order of the hits.
        for (i = 0; i < nParallelHits; i++) {
          tmp[i] = ListParallelHits[nParallelHits - 1 - i];
          aux[i] = U[nParallelHits - 1 - i];
        }
        for (i = 0; i < nParallelHits; i++) {
          ListParallelHits[i] = tmp[i];
          U[i] = aux[i];
        }
      } // end of  if( Charge ==1)
      for (j = 0; j < nParallelHits; j++) {
        V[j] = info[Infoparal[ListParallelHits[j]]][1] / (info[Infoparal[ListParallelHits[j]]][0] * info[Infoparal[ListParallelHits[j]]][0] +
                                                          info[Infoparal[ListParallelHits[j]]][1] * info[Infoparal[ListParallelHits[j]]][1]);
      }
    } // end of  if((aaa>b1&&aaa<3.*b1))

  } else { // use V as ordering variable [case 2 or 4 Gianluigi's Logbook page 285].
    for (j = 0; j < nParallelHits; j++) {
      V[j] = info[Infoparal[ListParallelHits[j]]][1] / (info[Infoparal[ListParallelHits[j]]][0] * info[Infoparal[ListParallelHits[j]]][0] +
                                                        info[Infoparal[ListParallelHits[j]]][1] * info[Infoparal[ListParallelHits[j]]][1]);
    }
    Merge_Sort(nParallelHits, V, ListParallelHits);

    if ((aaa <= -3. * b1 || aaa >= 3. * b1)) { //  case #2;
      if (Charge == -1) {
        // inverting the order of the hits.
        for (i = 0; i < nParallelHits; i++) {
          tmp[i] = ListParallelHits[nParallelHits - 1 - i];
          aux[i] = V[nParallelHits - 1 - i];
        }
        for (i = 0; i < nParallelHits; i++) {
          ListParallelHits[i] = tmp[i];
          V[i] = aux[i];
        }
      }
      for (j = 0; j < nParallelHits; j++) {
        U[j] = info[Infoparal[ListParallelHits[j]]][0] / (info[Infoparal[ListParallelHits[j]]][0] * info[Infoparal[ListParallelHits[j]]][0] +
                                                          info[Infoparal[ListParallelHits[j]]][1] * info[Infoparal[ListParallelHits[j]]][1]);
      }
    } else { //  case # 4.
      if (Charge == 1) {
        // inverting the order of the hits.
        for (i = 0; i < nParallelHits; i++) {
          tmp[i] = ListParallelHits[nParallelHits - 1 - i];
          aux[i] = V[nParallelHits - 1 - i];
        }
        for (i = 0; i < nParallelHits; i++) {
          ListParallelHits[i] = tmp[i];
          V[i] = aux[i];
        }
      }
      for (j = 0; j < nParallelHits; j++) {
        U[j] = info[Infoparal[ListParallelHits[j]]][0] / (info[Infoparal[ListParallelHits[j]]][0] * info[Infoparal[ListParallelHits[j]]][0] +
                                                          info[Infoparal[ListParallelHits[j]]][1] * info[Infoparal[ListParallelHits[j]]][1]);
      }
    }

  } //  end of   if((aaa>b1&& ....

  //  FI initial value (at 0,0  vertex) in the Helix reference frame

  *Fi_initial_helix_referenceframe = atan2(-oY, -oX); //  this is in order to be coherent
                                                      //  with the calculatation of Fi, which is atan2(oY,oX).
                                                      //  atan2  is defined in [-PI,PI)
  if (*Fi_initial_helix_referenceframe < 0.)
    *Fi_initial_helix_referenceframe += 2. * PI;

  //  FI of the last parallel hit in the Helix reference frame

  *Fi_final_helix_referenceframe = atan2(info[Infoparal[ListParallelHits[nParallelHits - 1]]][1] - oY, info[Infoparal[ListParallelHits[nParallelHits - 1]]][0] - oX);
  if (*Fi_final_helix_referenceframe < 0.)
    *Fi_final_helix_referenceframe += 2. * PI;

  if (Charge > 0) {
    if (*Fi_final_helix_referenceframe > *Fi_initial_helix_referenceframe)
      *Fi_final_helix_referenceframe -= 2. * PI;
    if (*Fi_final_helix_referenceframe > *Fi_initial_helix_referenceframe)
      *Fi_final_helix_referenceframe = *Fi_initial_helix_referenceframe;
  } else {
    if (*Fi_final_helix_referenceframe < *Fi_initial_helix_referenceframe)
      *Fi_final_helix_referenceframe += 2. * PI;
    if (*Fi_final_helix_referenceframe < *Fi_initial_helix_referenceframe)
      *Fi_final_helix_referenceframe = *Fi_initial_helix_referenceframe;
  }

  return;
}
//----------end of function PndSttTrackFinderReal::PndSttOrderingParallel

//----------begin of function PndSttTrackFinderReal::PndSttOrderingSkewandParallel

void PndSttTrackFinderReal::PndSttOrderingSkewandParallel(Short_t *Infoparal, Short_t *Infoskew, Double_t oX, Double_t oY, Double_t Rr, Short_t nSkewhit, Short_t *ListSkewHits,
                                                          Double_t *SList, // this is rekated to the skew hits. IMPORTANT :
                                                                           // the index must be the ORIGINAL skew hit number,
                                                                           // therefore SList[Infoskew[ListSkewHits[*]]].
                                                          Short_t Charge, Short_t nParHits, Short_t *ListParHits, Double_t *U, Double_t *V,
                                                          Short_t *BigList // this is the final ordered Parallel+Skew list;
                                                                           // already in NATIVE hit number.
)
{

  Short_t i, j, index[nSkewhit + nParHits], tmp[nSkewhit + nParHits], tmpList[nSkewhit];
  Double_t aaa, b1, sign, aux[nSkewhit + nParHits];

  //  here there is the ordering of the hits, under the assumption that the circumference
  //  in XY goes through (0,0).
  //  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
  //  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
  //  The scheme for the ordering of the hit is as follows :
  //  1)  order hits by increasing U of the conformal mapping; see Gianluigi's Logbook page 283;
  //  2)  find the charge of the track by checking if it is closest to the center in XY
  //	the first or the last of the ordered hits.

  //   ordering of the hits

  aaa = atan2(oY, oX); // atan2 defined between -PI and PI.

  // the following statement is necessary since for unknown reason the root interpreter
  // gives a weird error when using PI directly in the if statement below!!!!!!! I lost
  // 2 hours trying to figure this out!
  b1 = PI / 4.;

  if (aaa > b1 && aaa < 3. * b1 || (aaa > -3. * b1 && aaa < -b1)) {                                    //  case #1 or #3;see Gianluigi's Logbook page 285.
    if ((aaa > b1 && aaa < 3. * b1 && Charge == -1) || (aaa > -3. * b1 && aaa < -b1 && Charge == 1)) { // for speeding up the ordering taking advantage
                                                                                                       // that the parallel hits were earlier ordered and
                                                                                                       //  apply the trick of multiplying by   -1.
      sign = -1.;
    } else { //  normal calculation
      sign = 1.;
    }

    for (j = 0; j < nParHits; j++) {
      aux[j] = sign * U[j];
      //			BigList[j]=Infoparal[ListParHits[j]];
      //			index[j] = j;
    }
    for (j = 0; j < nSkewhit; j++) {
      // this is U in conformal space
      aux[j + nParHits] = sign * (oX + Rr * cos(SList[Infoskew[ListSkewHits[j]]])) /
                          (oX * oX + oY * oY + Rr * Rr + 2. * Rr * (oX * cos(SList[Infoskew[ListSkewHits[j]]]) + oY * sin(SList[Infoskew[ListSkewHits[j]]])));
      //			BigList[j+nParHits]=Infoskew[ListSkewHits[j]];
      //			index[j+nParHits] = j+nParHits;
    }

  } else { // use V as ordering variable
           // [case 2 and 4 Gianluigi's Logbook page 285].

    if (((aaa <= -3. * b1 || aaa >= 3. * b1) && Charge == -1) || (-b1 <= aaa && aaa <= b1 && Charge == 1)) {
      sign = -1.;
    } else {
      sign = 1.;
    }
    for (j = 0; j < nParHits; j++) {
      aux[j] = sign * V[j];
      //			BigList[j]=Infoparal[ListParHits[j]];
      //			index[j] = j;
    }
    for (j = 0; j < nSkewhit; j++) {
      // this is V in conformal space.
      aux[j + nParHits] = sign * (oY + Rr * sin(SList[Infoskew[ListSkewHits[j]]])) /
                          (oX * oX + oY * oY + Rr * Rr + 2. * Rr * (oX * cos(SList[Infoskew[ListSkewHits[j]]]) + oY * sin(SList[Infoskew[ListSkewHits[j]]])));
      //			BigList[j+nParHits]=Infoskew[ListSkewHits[j]];
      //			index[j+nParHits] = j+nParHits;
    }

  } //  end of  if((aaa>b1&& ....

  for (j = 0; j < nParHits; j++) {
    BigList[j] = Infoparal[ListParHits[j]];
    index[j] = j;
  }
  for (j = 0; j < nSkewhit; j++) {
    BigList[j + nParHits] = Infoskew[ListSkewHits[j]];
    index[j + nParHits] = j + nParHits;
  }

  Merge_Sort(nSkewhit + nParHits, aux, index);

  for (i = 0, j = 0; i < nSkewhit + nParHits; i++) {
    tmp[i] = BigList[index[i]];
    //  reorder the ListSkewHits also.
    if (index[i] >= nParHits) {
      tmpList[j] = ListSkewHits[index[i] - nParHits];
      j++;
    }
  }
  for (i = 0; i < nSkewhit + nParHits; i++) {
    BigList[i] = tmp[i];
  }
  //  reorder the ListSkewHits also.
  for (i = 0; i < nSkewhit; i++) {
    ListSkewHits[i] = tmpList[i];
  }

  return;
}
//----------end of function PndSttTrackFinderReal::PndSttOrderingSkewandParallel

//----------begin of function PndSttTrackFinderReal::PndSttOrdering

void PndSttTrackFinderReal::PndSttOrdering(Double_t oX, Double_t oY, Double_t info[][7], Short_t nParallelHits, Short_t *ListParallelHits, Short_t nSkewHit, Short_t *ListSkewHits,
                                           Double_t *S, Short_t *Infoparal, Short_t *Infoskew, Short_t *nTotal, Short_t *BigList, Short_t *Charge)
{

  *nTotal = nParallelHits + nSttSkewhit;

  Short_t i, j, flag, aux[*nTotal];
  Double_t old, auxFivalues[nmaxHits], auxFiSkewvalues[nmaxHits], auxRvalues[nmaxHits], BigListFi[*nTotal];

  //  here there is the ordering of the hits

  //   ordering of the parallel hits is not necessary; already done previously in
  //   PndSttOrderingParallel, also taking care of the Charge (when positive, the
  //   ordering must be reversed).

  for (j = 0; j < nParallelHits; j++) {
    auxFivalues[j] = atan2(info[Infoparal[ListParallelHits[j]]][1] - oY, info[Infoparal[ListParallelHits[j]]][0] - oX);
    if (auxFivalues[j] < 0.)
      auxFivalues[j] += 2. * PI;
  }

  //  fixing possible discontinuity between fi<2*PI and fi>0.
  for (old = auxFivalues[0], flag = 0, j = 1; j < nParallelHits; j++) {
    if (fabs(old - auxFivalues[j]) > PI) {
      flag = 1;
      break;
    } else {
      old = auxFivalues[j];
    }
  }
  if (flag == 1) {
    for (j = 0; j < nParallelHits; j++) {
      if (auxFivalues[j] < PI)
        auxFivalues[j] += 2. * PI;
    }
  }

  //     now ordering of the skew hits
  if (nSkewHit > 0) {
    if (flag == 1) {
      for (j = 0; j < nSkewHit; j++) {
        if (S[Infoskew[ListSkewHits[j]]] < PI) {
          auxFiSkewvalues[j] = S[Infoskew[ListSkewHits[j]]] + 2. * PI;
        } else {
          auxFiSkewvalues[j] = S[Infoskew[ListSkewHits[j]]];
        }
      }
    } else {
      for (j = 0; j < nSkewHit; j++) {
        auxFiSkewvalues[j] = S[Infoskew[ListSkewHits[j]]];
      }
    }

    Merge_Sort(nSkewHit, auxFiSkewvalues, ListSkewHits);
  } //  end of   if(nSkewHit>0)

  //    merge the parallel and skew hits
  for (j = 0; j < nParallelHits; j++) {
    BigListFi[j] = auxFivalues[j];
    BigList[j] = Infoparal[ListParallelHits[j]];
  }
  for (j = 0; j < nSkewHit; j++) {
    BigListFi[j + nParallelHits] = auxFiSkewvalues[j];
    BigList[j + nParallelHits] = Infoskew[ListSkewHits[j]];
  }

  if (nSkewHit > 0)
    Merge_Sort(*nTotal, BigListFi, BigList);

  //---------   end ordering

  return;
}
//----------end of function PndSttTrackFinderReal::PndSttOrdering

//----------start  function PndSttTrackFinderReal::PndSttFindingParallelTrackAngularRange

void PndSttTrackFinderReal::PndSttFindingParallelTrackAngularRange(Double_t oX, Double_t oY, Double_t R, Short_t Charge,
                                                                   Double_t *Fi_low_limit, // Fi (in XY Helix frame) lower limit using
                                                                                           // the Stt detector minimum/maximum radius
                                                                                           // Fi_low_limit is ALWAYS between 0. and 2PI
                                                                   Double_t *Fi_up_limit,  // Fi (in XY Helix frame) upper limit using
                                                                                           // the Stt detector maximum/minimum radius
                                                                                           // Fi_up_limit is ALWAYS > Fi_low_limit and
                                                                                           // possibly > 2PI.
                                                                   Short_t *status,        // *status =0, all well; =1,
                                                                                           // track contained completely between RMin
                                                                                           // and RMax; = -1 track contained within RMin;
                                                                                           // =-2 track outside RMax.
                                                                   Double_t Rmi,           // Rmin of cylindrical volume intersected by track;
                                                                   Double_t Rma            // Rmax of cylindrical volume intersected by track;
)
{
  //  The hits ALWAYS are contained between Fi_low_limit and Fi_up_limit;
  //  the Point at (0,0) is NEVER contained between Fi_low_limit and Fi_up_limit.

  // -------------- calculate the maximum fi and minimum fi spanned by this track,

  // see logbook pag.270; by using the Rmin and Rmax of the straw detector.

  //  working in the hypothesis that the starting point of the track is near (0,0) so that
  //  R_vertex < RStrawDetectorMin

  bool intersection_inner, intersection_outer;
  Double_t teta1, teta2, tetavertex, a, cosT, cost, cosFi, cosfi, Fi, fi, FI0, Px, Py, tmp;

  //	Rma += 1. ; // add a safety margin.
  //	Rmi -= 1. ; // add a safety margin.

  a = sqrt(oX * oX + oY * oY);

  //  preliminary condition
  if (a + R <= Rmi) // in this case there might be hits at radius < Rmi.
  {
    *status = -1;
    return;
  }
  if (a >= R + Rma || R >= a + Rma) // in this case there can be no hits with radius < Rma.
  {
    *status = -2;
    return;
  }

  if (a - R >= Rmi)
    intersection_inner = false;
  else
    intersection_inner = true;

  if (a + R <= Rma || a - R >= Rma)
    intersection_outer = false;
  else
    intersection_outer = true;

  if ((!intersection_inner) && (!intersection_outer)) {
    *Fi_low_limit = 0.;
    *Fi_up_limit = 2. * PI;
    *status = 1;
    return;
  }

  //	now the calculation

  FI0 = atan2(-oY, -oX);
  if (intersection_outer) {
    cosFi = (a * a + R * R - Rma * Rma) / (2. * R * a);
    if (cosFi < -1.)
      cosFi = -1.;
    else if (cosFi > 1.)
      cosFi = 1.;
    Fi = acos(cosFi);
  }

  if (intersection_inner) {
    cosfi = (a * a + R * R - Rmi * Rmi) / (2. * R * a);
    if (cosfi < -1.)
      cosfi = -1.;
    else if (cosfi > 1.)
      cosfi = 1.;
    fi = acos(cosfi);
  }

  if (Charge < 0.) { // this particle rotates counterclockwise when looking into the beam
    if (intersection_outer && intersection_inner) {
      *Fi_low_limit = FI0 + fi;
      *Fi_up_limit = FI0 + Fi;

    } else if (intersection_inner) {
      *Fi_low_limit = FI0 + fi;
      *Fi_up_limit = FI0 - fi;
    } else {
      *Fi_low_limit = FI0 - Fi;
      *Fi_up_limit = FI0 + Fi;
    } // end of    if( intersection_outer && intersection_inner

  } else { // continuation of   if( Charge < 0.)

    if (intersection_outer && intersection_inner) {
      *Fi_low_limit = FI0 - Fi;
      *Fi_up_limit = FI0 - fi;

    } else if (intersection_inner) {
      *Fi_low_limit = FI0 + fi; // must invert because low limit must be < up limit
      *Fi_up_limit = FI0 - fi;
    } else {
      *Fi_low_limit = FI0 - Fi;
      *Fi_up_limit = FI0 + Fi;
    } // end of    if( intersection_outer && intersection_inner

  } // end of  if( Charge < 0.)

  if (*Fi_low_limit < 0.) {
    *Fi_low_limit = fmod(*Fi_low_limit, 2. * PI);
    *Fi_low_limit += 2. * PI;
  } else if (*Fi_low_limit >= 2. * PI) {
    *Fi_low_limit = fmod(*Fi_low_limit, 2. * PI);
  }
  if (*Fi_up_limit < 0.) {
    *Fi_up_limit = fmod(*Fi_up_limit, 2. * PI);
    *Fi_up_limit += 2. * PI;
  } else if (*Fi_up_limit >= 2. * PI) {
    *Fi_up_limit = fmod(*Fi_up_limit, 2. * PI);
  }

  //	Modify *Fi_up_limit by adding
  //	2PI if it is the case, in order to make *Fi_up_limit > *Fi_low_limit.
  if (*Fi_up_limit < *Fi_low_limit)
    *Fi_up_limit += 2. * PI;
  if (*Fi_up_limit < *Fi_low_limit)
    *Fi_up_limit = *Fi_low_limit;

  *status = 0;

  return;
}

//---------- end of  function PndSttTrackFinderReal::PndSttFindingParallelTrackAngularRange

//----------start of function PndSttTrackFinderReal::WriteMacroParallelHitswithRfromMC

void PndSttTrackFinderReal::WriteMacroParallelHitswithRfromMC(Int_t Nhits, Double_t info[][7], Short_t nTracksFoundSoFar,
                                                              //                   bool *TypeConf,
                                                              Double_t *Ox, Double_t *Oy, Short_t *daTrackFoundaTrackMC)
{

  Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS;

  Double_t xmin, xmax, ymin, ymax, xl, xu, yl, yu, gamma, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, ff, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2,
    x1, x2, vx1, vy1, vz1, C0x1, C0y1, C0z1, aaa, bbb, ccc, rrr, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step,
    SkewInclWithRespectToS, zpos, zpos1, zpos2, Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  char nome[300], nome2[300];

  //---------- parallel straws Macro now con anche le tracce MC

  sprintf(nome, "MacroSttwithRfromMCParallelHitswithMCEvent%d", IVOLTE);
  sprintf(nome2, "%s.C", nome);
  FILE *MACRO = fopen(nome2, "w");
  //      MACRO = fopen(nome2,"w");
  fprintf(MACRO, "void %s()\n{\n", nome);
  xmin = 1.e20;
  xmax = -1.e20;
  ymin = 1.e20;
  ymax = -1.e20;
  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
      if (info[i][0] - info[i][3] < xmin)
        xmin = info[i][0] - info[i][3];
      if (info[i][0] + info[i][3] > xmax)
        xmax = info[i][0] + info[i][3];
      if (info[i][1] - info[i][3] < ymin)
        ymin = info[i][1] - info[i][3];
      if (info[i][1] + info[i][3] > ymax)
        ymax = info[i][1] + info[i][3];
    }
  }

  if (xmin > 0.)
    xmin = 0.;
  if (xmax < 0.)
    xmax = 0.;
  if (ymin > 0.)
    ymin = 0.;
  if (ymax < 0.)
    ymax = 0.;

  deltax = xmax - xmin;
  deltay = ymax - ymin;

  if (deltax > deltay) {
    ymin -= 0.5 * (deltax - deltay);
    ymax = ymin + deltax;
    delta = deltax;
  } else {
    xmin -= 0.5 * (deltay - deltax);
    xmax = xmin + deltay;
    delta = deltay;
  }

  xmax = xmax + delta * 0.15;
  xmin = xmin - delta * 0.15;

  ymax = ymax + delta * 0.15;
  ymin = ymin - delta * 0.15;

  fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", xmin, ymin, xmax, ymax);

  disegnaAssiXY(MACRO, xmin, xmax, ymin, ymax);

  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1) { // parallel straws
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nE%d->SetFillStyle(0);\nE%d->Draw();\n", i, info[i][0], info[i][1], info[i][3], info[i][3], i, i);
    }
  }

  for (i = 0; i < nMCTracks; i++) {
    Int_t icode;
    Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica, KAPPA, FI0mc;
    PndMCTrack *pMC;
    pMC = (PndMCTrack *)fMCTrackArray->At(i);
    if (!pMC)
      continue;
    icode = pMC->GetPdgCode();       //   PDG code of track
    Oxx = pMC->GetStartVertex().X(); //   X of starting point track
    Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
    Px = pMC->GetMomentum().X();
    Py = pMC->GetMomentum().Y();
    aaa = sqrt(Px * Px + Py * Py);
    Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
    if (icode > 1000000000)
      carica = 1.;
    else
      carica = fParticle->Charge() / 3.; //   charge of track
    if (fabs(carica) < 0.1)
      continue;
    Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
    Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
    if (fabs(pMC->GetMomentum().Z()) < 1.e-20)
      KAPPA = 99999999.;
    else
      KAPPA = -carica * 0.001 * BFIELD * CVEL / pMC->GetMomentum().Z();
    FI0mc = fmod(Fifi + PI, 2. * PI);

    fprintf(MACRO, "TEllipse* MC%d = new TEllipse(%f,%f,%f,%f,0.,360.);\nMC%d->SetFillStyle(0);\nMC%d->SetLineColor(3);\nMC%d->Draw();\n", i, Cx, Cy, Rr, Rr, i, i, i);
  }

  //-------------------------------   plotting all the tracks found, with R from corresponding MC track

  for (i = 0; i < nTracksFoundSoFar; i++) {

    if (daTrackFoundaTrackMC[i] == -1)
      continue;

    Int_t icode;
    Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica, KAPPA, FI0mc;
    PndMCTrack *pMC;
    pMC = (PndMCTrack *)fMCTrackArray->At(daTrackFoundaTrackMC[i]);
    if (!pMC)
      continue;
    icode = pMC->GetPdgCode();       //   PDG code of track
    Oxx = pMC->GetStartVertex().X(); //   X of starting point track
    Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
    Px = pMC->GetMomentum().X();
    Py = pMC->GetMomentum().Y();
    aaa = sqrt(Px * Px + Py * Py);
    Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
    TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
    TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
    if (icode > 1000000000)
      carica = 1.;
    else
      carica = fParticle->Charge() / 3.; //   charge of track
    if (fabs(carica) < 0.1)
      continue;
    Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
    Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
    if (fabs(pMC->GetMomentum().Z()) < 1.e-20)
      KAPPA = 99999999.;
    else
      KAPPA = -carica * 0.001 * BFIELD * CVEL / pMC->GetMomentum().Z();
    FI0mc = fmod(Fifi + PI, 2. * PI);

    rrr = Rr;
    double angolo = atan2(Oy[i], Ox[i]);
    aaa = rrr * cos(angolo);
    bbb = rrr * sin(angolo);
    fprintf(MACRO, "TEllipse* ris%d=new TEllipse(%f,%f,%f,%f,0.,360.);\nris%d->SetFillStyle(0);\nris%d->SetLineColor(2);\nris%d->Draw();\n", i, aaa, bbb, rrr, rrr, i, i, i);
  }

  // -----------

  fprintf(MACRO, "}\n");
  fclose(MACRO);

  //------------------------------------------------------------------------------------------------------------

  return;
}

//----------end of function PndSttTrackFinderReal::WriteMacroParallelHitswithRfromMC

//----------start of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithRfromMC

void PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithRfromMC(Double_t KAPPA, Double_t FI0, Double_t D, Double_t Fi, Double_t R, Int_t Nhits, Double_t info[][7], Int_t Nincl,
                                                                    Int_t Minclinations[], Double_t inclination[][3], Int_t imaxima, Int_t nMaxima)
{

  Int_t i, j, i1, ii, index, Kincl, nlow, nup, STATUS, imc, Nmin, Nmax;

  Double_t xmin, xmax, ymin, ymax, Ox, Oy, dx, dy, diff, d1, d2, delta, deltax, deltay, deltaz, deltaS, factor, zmin, zmax, Smin, Smax, S1, S2, z1, z2, y1, y2, vx1, vy1, vz1, C0x1,
    C0y1, C0z1, aaa, bbb, ccc, angle, minor, major, distance, Rx, Ry, LL, Aellipsis1, Bellipsis1, fi1, fmin, fmax, offset, step, SkewInclWithRespectToS, zpos, zpos1, zpos2,
    Tiltdirection1[2], zl[200], zu[200], POINTS1[6];

  //-------------------  skew straws hits Macro now

  char nome2[300], nome[300];
  FILE *MACRO;
  sprintf(nome, "MacroSttParTrack%dSkewTrack%dSkewHitswithRfromMCEvent%d", imaxima, nMaxima, IVOLTE);
  sprintf(nome2, "%s.C", nome);
  MACRO = fopen(nome2, "w");
  fprintf(MACRO, "void %s()\n{\n", nome);

  // KAPPA = 1./166.67 ;  FI0 = 1.5*PI;

  Smin = zmin = 1.e10;
  Smax = zmax = -zmin;
  index = 0;

  for (i = 0; i < Nhits; i++) {
    if (info[i][5] == 1.)
      continue; // exclude parallel straws

    Kincl = (int)info[i][5] - 1;

    aaa = sqrt(inclination[Kincl][0] * inclination[Kincl][0] + inclination[Kincl][1] * inclination[Kincl][1] + inclination[Kincl][2] * inclination[Kincl][2]);
    vx1 = inclination[Kincl][0] / aaa;
    vy1 = inclination[Kincl][1] / aaa;
    vz1 = inclination[Kincl][2] / aaa;
    C0x1 = info[i][0];
    C0y1 = info[i][1];
    C0z1 = info[i][2];
    Ox = (R + D) * cos(Fi);
    Oy = (R + D) * sin(Fi);

    calculateintersections(Ox, Oy, R, C0x1, C0y1, C0z1, info[i][3], vx1, vy1, vz1, &STATUS, POINTS1);

    if (STATUS < 0)
      continue;

    for (ii = 0; ii < 2; ii++) {
      j = 3 * ii;
      distance = sqrt((POINTS1[j] - C0x1) * (POINTS1[j] - C0x1) + (POINTS1[1 + j] - C0y1) * (POINTS1[1 + j] - C0y1) + (POINTS1[2 + j] - C0z1) * (POINTS1[2 + j] - C0z1));

      Rx = POINTS1[j] - Ox;     //  x component Radial vector of cylinder of trajectory
      Ry = POINTS1[1 + j] - Oy; //  y direction Radial vector of cylinder of trajectory

      aaa = sqrt(Rx * Rx + Ry * Ry);
      SkewInclWithRespectToS = (-Ry * vx1 + Rx * vy1) / aaa;
      SkewInclWithRespectToS /= R;
      bbb = sqrt(SkewInclWithRespectToS * SkewInclWithRespectToS + vz1 * vz1);
      //  the tilt direction of this ellipse is (1,0)  when major axis along Z direction
      if (bbb > 1.e-10) {
        Tiltdirection1[0] = vz1 / bbb;
        Tiltdirection1[1] = SkewInclWithRespectToS / bbb;
      } else {
        Tiltdirection1[0] = 1.;
        Tiltdirection1[1] = 0.;
      }

      LL = fabs(vx1 * Rx + vy1 * Ry);
      if (LL < 1.e-10)
        continue;
      Aellipsis1 = info[i][3] * aaa / LL;

      Bellipsis1 = info[i][3] / R;

      if (distance >= info[i][4] + Aellipsis1)
        continue;

      // checks that the projected ellipsis doesn't go out the boundaries of both the skew straw and the trajectory cylinder

      if (fabs(POINTS1[j + 2] - ZCENTER_STRAIGHT) > SEMILENGTH_STRAIGHT - Aellipsis1 || distance + bbb > info[i][4] //  the ellipsis goes out of the boundaries of the skew straw
      ) {
        cout << "the ellipsis goes out of the boundaries of the skew straw, hit n. " << i << endl
             << "dis. from center " << distance + bbb << ",  length of the straw " << info[i][4] << endl;
        continue;
      }
      //--------------------------

      fi1 = atan2(POINTS1[j + 1] - Oy, POINTS1[j] - Ox); // atan2 returns radians in (-pi and +pi]
      if (fi1 < 0.)
        fi1 += 2. * PI;

      if (zmin > POINTS1[j + 2] - Aellipsis1)
        zmin = POINTS1[j + 2] - Aellipsis1;
      if (zmax < POINTS1[j + 2] + Aellipsis1)
        zmax = POINTS1[j + 2] + Aellipsis1;

      if (Smin > fi1 - Bellipsis1)
        Smin = fi1 - Bellipsis1;
      if (Smax < fi1 + Bellipsis1)
        Smax = fi1 + Bellipsis1;

      Double_t rotation1 = 180. * atan2(Tiltdirection1[1], Tiltdirection1[0]) / PI;
      fprintf(MACRO, "TEllipse* E%d = new TEllipse(%f,%f,%f,%f,0.,360.,%f);\nE%d->SetFillStyle(0);\n", index, POINTS1[j + 2], fi1, Aellipsis1, Bellipsis1, rotation1, index);

      index++;

    } //  end of    for( ii=0; ii<2; ii++)

  } //   end of  for( i=1; i< Nhits; i++)

  if (index != 0 && zmax >= zmin && Smax >= Smin) {
    aaa = Smax - Smin;
    Smin -= aaa * 1.;
    Smax += aaa * 1.;

    aaa = zmax - zmin;
    zmin -= aaa * 0.05;
    zmax += aaa * 0.05;

    if (Smax > 2. * PI)
      Smax = 2. * PI;
    if (Smin < 0.)
      Smin = 0.;

    //  Smin=0.;
    //  Smax=2.*PI;

    fprintf(MACRO, "TCanvas* my= new TCanvas();\nmy->Range(%f,%f,%f,%f);\n", zmin, Smin, zmax, Smax);
    for (ii = 0; ii < index; ii++) {
      fprintf(MACRO, "E%d->Draw();\n", ii);
    }

    deltaz = zmax - zmin;
    deltaS = Smax - Smin;
    fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Smin + 0.05 * deltaS, zmax - 0.05 * deltaz, Smin + 0.05 * deltaS,
            zmin + 0.05 * deltaz, zmax - 0.05 * deltaz);
    fprintf(MACRO, "Assex->Draw();\n");
    fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", zmin + 0.05 * deltaz, Smin + 0.05 * deltaS, zmin + 0.05 * deltaz, Smax - 0.05 * deltaS,
            Smin + 0.05 * deltaS, Smax - 0.05 * deltaS);
    fprintf(MACRO, "Assey->Draw();\n");

    // --------------------------------

    //  plot della traccia trovata dal finder

    if (KAPPA >= 0.) {
      fmin = KAPPA * zmin + FI0;
      fmax = KAPPA * zmax + FI0;
    } else {
      fmax = KAPPA * zmin + FI0;
      fmin = KAPPA * zmax + FI0;
    }
    if (fmax >= 0.) {
      Nmax = (int)(0.5 * fmax / PI);
    } else {
      Nmax = ((int)(0.5 * fmax / PI)) - 1;
    }
    if (fmin >= 0.) {
      Nmin = (int)(0.5 * fmin / PI);
    } else {
      Nmin = ((int)(0.5 * fmin / PI)) - 1;
    }
    for (i = Nmin; i <= Nmax; i++) {
      offset = 2. * PI * i;
      z1 = (i * 2. * PI - FI0) / KAPPA;
      z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
      fprintf(MACRO, "TLine* FOUND%d = new TLine(%f,%f,%f,%f);\nFOUND%d->SetLineColor(2);\nFOUND%d->Draw();\n", i - Nmin, z1, 0., z2, 2. * PI, i - Nmin, i - Nmin);

    } //  end of  for(i=Nmin; i<= Nmax;++)

    //---------------------------------------------  qui ci aggiungo le traccie MC

    for (imc = 0; imc < nMCTracks; imc++) {

      Int_t icode;
      Double_t Rr, Dd, Fifi, Oxx, Oyy, Cx, Cy, Px, Py, carica, FI0mc;
      PndMCTrack *pMC;
      pMC = (PndMCTrack *)fMCTrackArray->At(imc);
      if (!pMC)
        continue;
      icode = pMC->GetPdgCode();       //   PDG code of track
      Oxx = pMC->GetStartVertex().X(); //   X of starting point track
      Oyy = pMC->GetStartVertex().Y(); //   Y of starting point track
      Px = pMC->GetMomentum().X();
      Py = pMC->GetMomentum().Y();
      aaa = sqrt(Px * Px + Py * Py);
      Rr = aaa * 1000. / (BFIELD * CVEL); //   R (cm) of Helix of track projected in XY plane; B = 2 Tesla
      TDatabasePDG *fdbPDG = TDatabasePDG::Instance();
      TParticlePDG *fParticle = fdbPDG->GetParticle(icode);
      if (icode > 1000000000)
        carica = 1.;
      else
        carica = fParticle->Charge() / 3.; //   charge of track
      if (fabs(carica) < 0.1)
        continue;
      Cx = Oxx + Py * 1000. / (BFIELD * CVEL * carica);
      Cy = Oyy - Px * 1000. / (BFIELD * CVEL * carica);
      if (fabs(pMC->GetMomentum().Z()) < 1.e-20)
        KAPPA = 99999999.;
      else
        KAPPA = -carica * 0.001 * BFIELD * CVEL / pMC->GetMomentum().Z();
      FI0mc = fmod(Fifi + PI, 2. * PI);
      //     KAPPA=MCtruthTrkInfo[12][imc];
      FI0 = FI0mc;

      if (KAPPA >= 0.) {
        fmin = KAPPA * zmin + FI0;
        fmax = KAPPA * zmax + FI0;
      } else {
        fmax = KAPPA * zmin + FI0;
        fmin = KAPPA * zmax + FI0;
      }
      if (fmax >= 0.) {
        Nmax = (int)(0.5 * fmax / PI);
      } else {
        Nmax = ((int)(0.5 * fmax / PI)) - 1;
      }
      if (fmin >= 0.) {
        Nmin = (int)(0.5 * fmin / PI);
      } else {
        Nmin = ((int)(0.5 * fmin / PI)) - 1;
      }
      for (i = Nmin; i <= Nmax; i++) {
        offset = 2. * PI * i;
        z1 = (i * 2. * PI - FI0) / KAPPA;
        z2 = ((i + 1) * 2. * PI - FI0) / KAPPA;
        fprintf(MACRO, "TLine* MC%d_%d = new TLine(%f,%f,%f,%f);\nMC%d_%d->SetLineColor(3);\nMC%d_%d->Draw();\n", imc, i - Nmin, z1, 0., z2, 2. * PI, imc, i - Nmin, imc, i - Nmin);

      } //  end of  for(i=Nmin; i<= Nmax;++)

    } // end of for(imc=0; imc<nMCTracks ; imc++)

  } // end of   if(index!=0 &&  zmax >= zmin && Smax >= Smin )

  fprintf(MACRO, "}\n");
  fclose(MACRO);
}

//----------end of function PndSttTrackFinderReal::WriteMacroSkewAssociatedHitswithRfromMC

//----------begin of function PndSttTrackFinderReal::AssociateFoundTrackstoMC

void PndSttTrackFinderReal::AssociateFoundTrackstoMC(Double_t info[][7], Short_t nTracksFoundSoFar, Short_t *nHitsinTrack, Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                                                     Short_t *nSttSkewhitinTrack, Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHits],
                                                     Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT]
                                                     //                  Short_t *daMCTrackaTrackFound
)
{

  bool firstime, flaggo, inclusionMC[nTracksFoundSoFar][nmaxHits], inclusionExp[nTracksFoundSoFar];

  Short_t ntoMCtrack[nTracksFoundSoFar], toMCtracklist[nTracksFoundSoFar][nmaxHits], toMCtrackfrequency[nTracksFoundSoFar][nmaxHits];

  Short_t i, j, jtemp, jexp;

  Short_t enne, itemp, massimo;

  for (i = 0; i < nTracksFoundSoFar; i++) {
    daTrackFoundaTrackMC[i] = -1;
    inclusionExp[i] = true;
    for (j = 0; j < nHitsinTrack[i] + nSttSkewhitinTrack[i]; j++) {
      inclusionMC[i][j] = true;
    }
  }

  for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {

    firstime = true;
    ntoMCtrack[jexp] = 0;

    // prima  gli hits paralleli ---------------------

    for (i = 0; i < nHitsinTrack[jexp]; i++) {
      enne = (Short_t)(info[infoparal[ListHitsinTrack[jexp][i]]][6] + 0.01);
      if (enne < 0)
        continue; //  hit not associated to any MC track; noise hit.
      if (firstime) {
        ntoMCtrack[jexp] = 1;
        toMCtracklist[jexp][0] = enne;
        toMCtrackfrequency[jexp][0] = 1;
        firstime = false;
      } else {

        flaggo = true;
        for (j = 0; j < ntoMCtrack[jexp]; j++) {
          if (enne == toMCtracklist[jexp][j]) {
            toMCtrackfrequency[jexp][j]++;
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          toMCtracklist[jexp][ntoMCtrack[jexp]] = enne;
          toMCtrackfrequency[jexp][ntoMCtrack[jexp]] = 1;
          ntoMCtrack[jexp]++;
        }
      }

    } //  end of for(i=0; i<nHitsinTrack[jexp]; i++)

    // poi gli hits skew ---------------------
    for (i = 0; i < nSttSkewhitinTrack[jexp]; i++) {
      enne = (Short_t)(info[infoskew[ListSkewHitsinTrack[jexp][i]]][6] + 0.01);
      if (enne < 0)
        continue; //  hit not associated to any MC track; noise hit.
      if (firstime) {
        ntoMCtrack[jexp] = 1;
        toMCtracklist[jexp][0] = enne;
        toMCtrackfrequency[jexp][0] = 1;
        firstime = false;
      } else {
        for (j = 0, flaggo = true; j < ntoMCtrack[jexp]; j++) {
          if (enne == toMCtracklist[jexp][j]) {
            toMCtrackfrequency[jexp][j]++;
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          toMCtracklist[jexp][ntoMCtrack[jexp]] = enne;
          toMCtrackfrequency[jexp][ntoMCtrack[jexp]] = 1;
          ntoMCtrack[jexp]++;
        }
      }
    } //  end of for(i=0; j<nHitsinTrack[jexp]; i++)

  } // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)

  itemp = 0;
  while (itemp > -1) {
    itemp = -1;
    massimo = -1;
    for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
      if (!inclusionExp[jexp])
        continue;
      for (i = 0; i < ntoMCtrack[jexp]; i++) {
        if (!inclusionMC[jexp][i])
          continue;
        if (toMCtrackfrequency[jexp][i] > massimo) {
          massimo = toMCtrackfrequency[jexp][i];
          itemp = toMCtracklist[jexp][i];
          jtemp = jexp;
        }
      }
    }
    if (itemp > -1) {
      daTrackFoundaTrackMC[jtemp] = itemp;
      inclusionExp[jtemp] = false;
      inclusionMC[jtemp][itemp] = false;
    }
  } //    end while ( itemp > -1)

  return;
}

//----------end of function PndSttTrackFinderReal::AssociateFoundTrackstoMC

//----------begin of function PndSttTrackFinderReal::AssociateFoundTrackstoMCbis

void PndSttTrackFinderReal::AssociateFoundTrackstoMCbis(bool *keepit, Double_t info[][7], Short_t nTracksFoundSoFar, Short_t nHitsinTrack[MAXTRACKSPEREVENT],
                                                        Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t nSttSkewhitinTrack[MAXTRACKSPEREVENT],
                                                        Short_t ListSkewHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Short_t daTrackFoundaTrackMC[MAXTRACKSPEREVENT])
{

  bool firstime, flaggo, inclusionMC[nTracksFoundSoFar][nmaxHits], inclusionExp[nTracksFoundSoFar];

  Short_t ntoMCtrack[nTracksFoundSoFar], toMCtrackfrequency[nTracksFoundSoFar][nmaxHits];

  Short_t i, j, jtemp, jexp;

  Short_t enne, itemp, massimo, toMCtracklist[nTracksFoundSoFar][nmaxHits];

  for (i = 0; i < nTracksFoundSoFar; i++) {
    //     daTrackFoundaTrackMC[i]=-1;  // initialization already done before!
    if (!keepit[i]) {
      inclusionExp[i] = false;
      continue;
    }
    inclusionExp[i] = true;
    for (j = 0; j < nHitsinTrack[i] + nSttSkewhitinTrack[i]; j++) {
      inclusionMC[i][j] = true;
    }
  }

  for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {

    if (!keepit[jexp])
      continue;
    firstime = true;
    ntoMCtrack[jexp] = 0;

    // ------ only the parallel hits are taken into consideration
    for (i = 0; i < nHitsinTrack[jexp]; i++) {

      enne = (Short_t)(info[infoparal[ListHitsinTrack[jexp][i]]][6] + 0.01);
      if (enne < 0)
        continue; //  hit not associated to any MC track; noise hit.
      if (firstime) {
        ntoMCtrack[jexp] = 1;
        toMCtracklist[jexp][0] = enne;
        toMCtrackfrequency[jexp][0] = 1;
        firstime = false;
      } else {
        for (j = 0, flaggo = true; j < ntoMCtrack[jexp]; j++) {
          if (enne == toMCtracklist[jexp][j]) {
            toMCtrackfrequency[jexp][j]++;
            flaggo = false;
            break;
          }
        }
        if (flaggo) {
          toMCtracklist[jexp][ntoMCtrack[jexp]] = enne;
          toMCtrackfrequency[jexp][ntoMCtrack[jexp]] = 1;
          ntoMCtrack[jexp]++;
        }
      }
    } //  end of for(i=0; i<nHitsinTrack[jexp]; i++)

  } // end of  for(jexp=0; jexp< nTracksFoundSoFar ;jexp++)

  itemp = 0;
  while (itemp > -1) {
    itemp = -1;
    massimo = -1;
    for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
      if (!inclusionExp[jexp])
        continue;
      for (i = 0; i < ntoMCtrack[jexp]; i++) {
        if (!inclusionMC[jexp][i])
          continue;
        if (toMCtrackfrequency[jexp][i] > massimo) {
          massimo = toMCtrackfrequency[jexp][i];
          itemp = toMCtracklist[jexp][i];
          jtemp = jexp;
        }
      }
    }
    if (itemp > -1) {
      daTrackFoundaTrackMC[jtemp] = itemp;
      inclusionExp[jtemp] = false;
      for (jexp = 0; jexp < nTracksFoundSoFar; jexp++) {
        if (!inclusionExp[jexp])
          continue;
        for (int jk = 0; jk < ntoMCtrack[jexp]; jk++) {
          if (itemp == toMCtracklist[jexp][jk]) {
            inclusionMC[jexp][jk] = false;
          }
        }
      }
    }
  } //    end while ( itemp > -1)

  return;
}

//----------end of function PndSttTrackFinderReal::AssociateFoundTrackstoMCbis

//---------- begin of function PndSttTrackFinderReal::PndSttInfoXYZParal

void PndSttTrackFinderReal::PndSttInfoXYZParal(Double_t info[][7], Short_t infopar, Double_t Ox, Double_t Oy, Double_t R, Double_t KAPPA, Double_t FI0, Short_t Charge,
                                               Double_t *Posiz //  output.
)
{

  //	Posiz = position (on the drift radius) of the hit whose 'parallel' scheme
  //	number is  infopar.

  Double_t fi, norm, vers[2];

  vers[0] = Ox - info[infopar][0];
  vers[1] = Oy - info[infopar][1];
  norm = sqrt(vers[0] * vers[0] + vers[1] * vers[1]);

  if (norm < 1.e-20) {
    Posiz[0] = -999999999.;
    return;
  }

  if (fabs(R - fabs(norm - info[infopar][3])) // distance trajectory-drift radius
      < fabs(R - (norm + info[infopar][3]))) {

    Posiz[0] = info[infopar][0] + info[infopar][3] * vers[0] / norm;
    Posiz[1] = info[infopar][1] + info[infopar][3] * vers[1] / norm;

  } else {

    Posiz[0] = info[infopar][0] - info[infopar][3] * vers[0] / norm;
    Posiz[1] = info[infopar][1] - info[infopar][3] * vers[1] / norm;

  } // end of if ( fabs( R - fabs( Distance - info[infopar][3] ) ).....

  //   Posiz[0] = info[infopar][0] + info[infopar][3]*vers[0]/norm;
  //   Posiz[1] = info[infopar][1] + info[infopar][3]*vers[1]/norm;

  if (fabs(KAPPA) < 1.e-10) {
    Posiz[2] = -888888888.;
    return;
  } else if (fabs(KAPPA) > 1.e10) {
    Posiz[2] = -777777777.;
    return;
  }

  fi = atan2(-vers[1], -vers[0]);
  if (fi < 0.)
    fi += 2. * PI;

  if (Charge > 0) {
    if (fi > FI0)
      FI0 += 2. * PI;
    //    Posiz[2] = (FI0-fi)/KAPPA;
  } else {
    if (fi < FI0)
      fi += 2. * PI;
  }
  Posiz[2] = (fi - FI0) / KAPPA;

  return;
}

//----------end of function PndSttTrackFinderReal::PndSttInfoXYZParal

//---------- begin of function PndSttTrackFinderReal::PndSttInfoXYZSkew

void PndSttTrackFinderReal::PndSttInfoXYZSkew(Double_t Z,      //  Z coordinate (center wire) of selected Skew hit
                                              Double_t ZDrift, // drift distance IN Z DIRECTION only, of selected Skew hit
                                              Double_t S, Double_t Ox, Double_t Oy, Double_t R, Double_t KAPPA, Double_t FI0, Short_t Charge,
                                              Double_t *Posiz //  output
)
{
  Short_t sign;

  Double_t bbb, tempZ[2], zmin, zmax, deltaz, zdist[2], zdist1, zdist2;

  Double_t Zline;

  if (fabs(KAPPA) < 1.e-10) {
    Posiz[0] = -999999999.;
    return;
  } else if (fabs(KAPPA) > 1.e10) {
    Posiz[0] = -888888888.;
    return;
  }

  Posiz[0] = Ox + R * cos(S);
  Posiz[1] = Oy + R * sin(S);

  if (Charge > 0) {
    if (S > FI0) {
      if (istampa >= 3) {
        cout << "from PndSttInfoXYZSkew : inconsistency, FI0 is not the maximum for this track " << endl;
        cout << "  stampa da PndSttInfoXYZSkew,  "
             << ", Z hit = " << Z << ", Zrift = " << ZDrift << ", S = " << S << ",  FI0 = " << FI0 << endl;
      }
      Posiz[0] = -777777777.;
      return;
    }
  } else {
    if (S < FI0) {
      if (istampa >= 3) {
        cout << "from PndSttInfoXYZSkew : inconsistency, FI0 is not the minimum for this track " << endl;
        cout << "  stampa da PndSttInfoXYZSkew,  "
             << ", Z hit = " << Z << ", Zrift = " << ZDrift << ", S = " << S << ",  FI0 = " << FI0 << endl;
      }
      Posiz[0] = -777777777.;
      return;
    }
  }

  if (KAPPA > 0) {
    zmin = -FI0 / KAPPA;
    zmax = (2. * PI - FI0) / KAPPA;
  } else {
    zmax = -FI0 / KAPPA;
    zmin = (2. * PI - FI0) / KAPPA;
  }
  deltaz = zmax - zmin;

  bbb = (S - FI0) / KAPPA;
  for (sign = 0; sign <= 1; sign++) {
    tempZ[sign] = Z + (2 * sign - 1) * ZDrift;
    if (tempZ[sign] > zmax) {
      tempZ[sign] = fmod(tempZ[sign] - zmax, deltaz) + zmin;
    } else if (tempZ[sign] < zmin) {
      tempZ[sign] = fmod(tempZ[sign] - zmin, deltaz) + zmax;
    }

    zdist1 = fabs(bbb - tempZ[sign]);
    zdist2 = deltaz - zdist1;
    if (zdist2 < 0.)
      zdist2 = 0.; // protect against rounding errors.
    zdist[sign] = zdist1 < zdist2 ? zdist1 : zdist2;

  } //  end of for(sign=0;sign<=1; sign++)

  zdist[0] < zdist[1] ? Posiz[2] = Z - ZDrift : Posiz[2] = Z + ZDrift;

  return;
}

//----------end of function PndSttTrackFinderReal::PndSttInfoXYZSkew

//----------start of function PndSttTrackFinderReal::FixDiscontinuitiesFiangleinSZplane

void PndSttTrackFinderReal::FixDiscontinuitiesFiangleinSZplane(Short_t TemporarynSttSkewhitinTrack, Double_t *S, Double_t *Fi_initial_helix_referenceframe, Short_t Charge)
{

  Short_t i;
  Double_t max, min;

  for (i = 0, min = 9999., max = -9999.; i < TemporarynSttSkewhitinTrack; i++) {
    if (S[i] > max)
      max = S[i];
    if (S[i] < min)
      min = S[i];
  }

  if (max - min > PI) {
    for (i = 0, min = 9999., max = -9999.; i < TemporarynSttSkewhitinTrack; i++) {
      if (S[i] < PI)
        S[i] += 2. * PI;
      if (S[i] > max)
        max = S[i];
      if (S[i] < min)
        min = S[i];
    }
  }

  if (Charge > 0) {
    if (*Fi_initial_helix_referenceframe < max)
      *Fi_initial_helix_referenceframe += 2. * PI;
  } else {
    if (*Fi_initial_helix_referenceframe > min)
      *Fi_initial_helix_referenceframe -= 2. * PI;
  }

  return;
}
//----------end of function PndSttTrackFinderReal::FixDiscontinuitiesFiangleinSZplane

//----------begin of function PndSttTrackFinderReal::FindCharge

void PndSttTrackFinderReal::FindCharge(Double_t oX, Double_t oY, Short_t nParallelHits, Double_t *X, Double_t *Y, Short_t *Charge)
{

  Short_t ihit, nleft, nright;

  Double_t cross, disq, minl, minr;

  // this methods works with the hypothesis that this track comes
  //  from (0,0)

  for (ihit = 0, nleft = 0, nright = 0, minr = 9999999., minl = 9999999.; ihit < nParallelHits; ihit++) {
    // find the Z component of the cross product between the vector from (0,0) to center of
    // circular trajectory [namely, (oX,oY) ]  and the Position vector of the center of the
    // parallel Hits [namely, (x,y)].

    cross = oX * Y[ihit] - oY * X[ihit];

    // if  cross >0  hits stays 'on the left' (which means clockwise to go from the origin
    // to the hit following the smaller path) otherwise it stays 'on the right'.

    if (cross > 0.) {
      disq = X[ihit] * X[ihit] + Y[ihit] * Y[ihit];
      nleft++;
    } else {
      nright++;
    }
  } // end of   for(ihit=0, nleft=0, nright=0;....

  if (nright > nleft) {
    *Charge = -1;
  } else if (nleft > nright) {
    *Charge = 1;
  } else { // then choose according the closest hit ti the center
    if (minr < minl)
      *Charge = -1;
    else
      *Charge = 1;
  }
}
//----------end of function PndSttTrackFinderReal::FindCharge

//----------start  function PndSttTrackFinderReal::IntersectionsWithClosedPolygon

Short_t PndSttTrackFinderReal::IntersectionsWithClosedPolygon(
  //-------- inputs
  Double_t Ox, Double_t Oy, Double_t R,
  Double_t Rmi, // min Apotema of hexagonal  volume intersected by track;
  Double_t Rma, // max Apotema of hexagonal volume intersected by track;
  //-------- outputs
  Short_t nIntersections[2], Double_t XintersectionList[][2], Double_t YintersectionList[][2])
{

  // return integer convention :
  // -2 -->  track outside outer polygon;
  // -1 -->  track contained completely within inner polygon;
  // 0 -->  at least 1 intersection with inner polygon, at least 1 with outer polygon;
  // 1 -->  track contained completely between the two polygons;
  // 2 -->  track contained completely by larger polygons, with intersections in the smaller;
  // 3 -->  track completely outsiede the small polygon with intersections in the bigger.

  //  inner Hexagon --> 0
  //  outer Hexagon --> 1

  bool internal[2], AtLeast1[2];

  Short_t i, is, j, Nintersections;

  Double_t mindist[2], distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the six sides of the Hexagon
    // centered at 0 :   a*x + b*y +c =0; see Gianluigi's logbook on page 277;
    // the coefficient  c  has to be multiplied by Erre.
    // The first side is
    a[] = {1. / sqrt(3.), 1., -1. / sqrt(3.), 1. / sqrt(3.), 1., -1. / sqrt(3.)}, b[] = {1., 0., 1., 1., 0., 1.},
    c[] = {-2. / sqrt(3.), -1., 2. / sqrt(3.), 2. / sqrt(3.), 1., -2. / sqrt(3.)},
    //----------------------

    Erre[] = {Rmi, Rma}, //  this is the distance from (0,0)
                         // of the Verteces of the Hexagon delimiting the Skew area
    tempX[2], tempY[2];

  // both hexagon_side_xlow and hexagon_side_xup must be multiplied by appropriate Erre;
  // sides of Hexagon ordered as a, b, c ..... in Gianluigi's logbook on page 280.
  Double_t hexagon_side_x[] = {0., 1., 1., 0., -1., -1., 0.},
           hexagon_side_y[] = {2. / sqrt(3.), 1. / sqrt(3.), -1. / sqrt(3.), -2. / sqrt(3.), -1. / sqrt(3.), 1. / sqrt(3.), 2. / sqrt(3.)};

  //-----------------------

  //   find intersection with the 6 sides of the small exhagon delimiting the skew straws zone
  //   see on page 277 of Gianluigi's logbook.

  // status  =0, at least 1 intersection with inner Hexagon, at least 1 intersection
  // with the outer Hexagon; =1, track contained completely between the
  // two Hexagons; = -1 track contained within inner Hexagon;
  // =-2 track outside outer Hexagon.

  for (i = 0; i < 2; i++) { // i=0 --> inner Hexagon, i= 1 --> outer Hexagon.
    AtLeast1[i] = false;
    nIntersections[i] = 0;
    internal[i] = true;
    mindist[i] = 999999.;
    for (is = 0; is < 6; is++) {
      if (IntersectionCircle_Segment(a[is], b[is], c[is] * Erre[i], hexagon_side_x[is] * Erre[i], hexagon_side_x[is + 1] * Erre[i], hexagon_side_y[is] * Erre[i],
                                     hexagon_side_y[is + 1] * Erre[i], Ox, Oy, R, &Nintersections, tempX, tempY,
                                     &distance // distance of (Ox,Oy) from line
                                               // defined by  a*x+b*y+c=0.
                                     )) {
        AtLeast1[i] = true;
        for (j = 0; j < Nintersections; j++) {
          XintersectionList[nIntersections[i]][i] = tempX[j];
          YintersectionList[nIntersections[i]][i] = tempY[j];
          nIntersections[i]++;
        }
      } // end of if ( IntersectionCircle_Segment( .....

      if (mindist[i] > distance)
        mindist[i] = distance;

      // the definition of 'internal' here is when the given Point
      // stays at the same side of the origin (0,0) with respect to
      // the given line of equation   a*x+b*y+c=0.
      internal[i] = internal[i] && IsInternal(Ox, Oy, a[is], b[is], c[is] * Erre[i]);

    } // end of  for(is=0; is<6; is++)
  }   // end of  for(i=0;i<2;i++)

  if ((!AtLeast1[0]) && (!AtLeast1[1])) {
    if (!internal[1])
      return -2; // trajectory outside outer Hexagon.
    if (R > mindist[1])
      return -2;
    if (!internal[0])
      return 1; // trajectory contained between inner and outer Hexagon.
    if (mindist[0] >= R)
      return -1;                           //  trajectory contained in inner Hexagon.
    return 1;                              // trajectory contained between inner and outer Hexagon.
  } else if (AtLeast1[0] && AtLeast1[1]) { // continuation of  if( (!AtLeast1[0]) && ...
    return 0;
  } else if (AtLeast1[0]) {
    return 2;
  }

  return 3;
}

//---------- end of  function PndSttTrackFinderReal::IntersectionsWithClosedPolygon

//----------start  function PndSttTrackFinderReal::IntersectionsWithOpenPolygon

Short_t PndSttTrackFinderReal::IntersectionsWithOpenPolygon(
  //-------- inputs
  Double_t Ox,    // Track parameter
  Double_t Oy,    // Track parameter
  Double_t R,     // Track parameter
  Short_t nSides, // input, n. of Sides of open Polygon.
  Double_t *a,    //  coefficient of formula :  aX + bY + c = 0 defining
  Double_t *b,    //  the Polygon sides.
  Double_t *c,
  Double_t *Side_x, // X,Y coordinate of the Sides vertices (in sequence, following
  Double_t *Side_y, // the Polygon along.
  //-------- outputs
  Double_t *XintersectionList, // XintersectionList
  Double_t *YintersectionList  // YintersectionList.
)
{

  // this methods returns the n. of intersections.

  Short_t i, is, j, nIntersections, Nintersections;

  Double_t mindist, distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the six sides of the Hexagon
    // centered at 0 :   a*x + b*y +c =0; see Gianluigi's logbook on page 277;
    // the coefficient  c  has to be multiplied by Erre.

    tempX[2], tempY[2];

  //-----------------------

  nIntersections = 0;
  for (is = 0; is < nSides; is++) {
    if (IntersectionCircle_Segment(a[is], b[is], c[is], Side_x[is], Side_x[is + 1], Side_y[is], Side_y[is + 1], Ox, Oy, R, &Nintersections, tempX, tempY,
                                   &distance // distance of (Ox,Oy) from line
                                             // defined by  a*x+b*y+c=0.
                                   )) {
      for (j = 0; j < Nintersections; j++) {
        XintersectionList[nIntersections] = tempX[j];
        YintersectionList[nIntersections] = tempY[j];
        nIntersections += Nintersections;
      }
    } // end of if ( IntersectionCircle_Segment( .....

  } // end of  for(is=0; is<nSides; is++)
    //	}  // end of  for(i=0;i<2;i++)

  return nIntersections;
}

//---------- end of  function PndSttTrackFinderReal::IntersectionsWithOpenPolygon

//----------start  function PndSttTrackFinderReal::IntersectionsWithClosedbiHexagonLeft

Short_t PndSttTrackFinderReal::IntersectionsWithClosedbiHexagonLeft(
  //-------- inputs
  Double_t vgap, Double_t Ox, Double_t Oy, Double_t R,
  Double_t Ami, // min Apotema of hexagonal  volume intersected by track;
  Double_t Ama, // max Apotema of hexagonal volume intersected by track;
  //-------- outputs
  Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList)
{

  // return integer convention :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon;
  // 1 -->  track contained completely between the two polygons;

  //  inner Hexagon --> 0
  //  outer Hexagon --> 1

  bool internal, AtLeast1;

  Short_t i, is, j, Nintersections;

  Double_t aaa, maxdistq, distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the 3 sides of the half inner Hexagon
    // plus 3 sides of the half outer Hexagon plus 2 vertical sides corresponding to the Gap :
    //    a*x + b*y +c =0; the numbering of these 8 sides foolows the convention of Gianluigi's
    // logbook on page 286.
    a[] = {-1. / sqrt(3.), 1., 1. / sqrt(3.), 1., 1. / sqrt(3.), 1., -1. / sqrt(3.), 1.}, b[] = {1., 0., 1., 0., 1., 0., 1., 0.},
    c[] = {-2. * Ama / sqrt(3.), Ama, 2. * Ama / sqrt(3.), vgap / 2., 2. * Ami / sqrt(3.), Ami, -2. * Ami / sqrt(3.), vgap / 2.},
    //----------------------

    tempX[2], tempY[2];

  // side_x and side_y are ordered as side1, side2, ..... in Gianluigi's logbook on page 286.
  Double_t side_x[] = {-vgap / 2., -Ama, -Ama, -vgap / 2., -vgap / 2., -Ami, -Ami, -vgap / 2., -vgap / 2.},
           side_y[] = {(-0.5 * vgap + 2. * Ama) / sqrt(3.),  Ama / sqrt(3.),  -Ama / sqrt(3.), -(-0.5 * vgap + 2. * Ama) / sqrt(3.),
                       -(-0.5 * vgap + 2. * Ami) / sqrt(3.), -Ami / sqrt(3.), Ami / sqrt(3.),  (-0.5 * vgap + 2. * Ami) / sqrt(3.),
                       (-0.5 * vgap + 2. * Ama) / sqrt(3.)};

  //-----------------------

  //   find intersections (maximum 16) with the 8 sides.

  AtLeast1 = false;
  *nIntersections = 0;
  internal = true;
  maxdistq = -9999.;
  for (is = 0; is < 8; is++) {
    aaa = (side_x[is] - Ox) * (side_x[is] - Ox) + (side_y[is] - Oy) * (side_y[is] - Oy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    aaa = (side_x[is + 1] - Ox) * (side_x[is + 1] - Ox) + (side_y[is + 1] - Oy) * (side_y[is + 1] - Oy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    if (IntersectionCircle_Segment(a[is], b[is], c[is], side_x[is], side_x[is + 1], side_y[is], side_y[is + 1], Ox, Oy, R, &Nintersections, tempX, tempY,
                                   &distance // distance of (Ox,Oy) from line
                                             // defined by  a*x+b*y+c=0.
                                   )) {
      AtLeast1 = true;
      for (j = 0; j < Nintersections; j++) {
        XintersectionList[*nIntersections] = tempX[j];
        YintersectionList[*nIntersections] = tempY[j];
        (*nIntersections)++;
      }
    } // end of if ( IntersectionCircle_Segment( .....

    // the definition of 'internal' here is when the given Point
    // stays at the same side of the origin (0,0) with respect to
    // the given line of equation   a*x+b*y+c=0.
    if (is < 3) {
      internal = internal && IsInternal(Ox, Oy, a[is], b[is], c[is]);
    } else {
      internal = internal && (!IsInternal(Ox, Oy, a[is], b[is], c[is]));
    }

  } // end of  for(is=0; is<8; is++)

  if (!AtLeast1) {
    if (!internal)
      return -1; // trajectory outside polygon.
    if (maxdistq < R * R)
      return -1; // trajectory outside polygon.
    return 1;    // trajectory completely contained inside this Polygon.
  }
  return 0;
}

//---------- end of  function PndSttTrackFinderReal::IntersectionsWithClosedbiHexagonLeft

//----------start  function PndSttTrackFinderReal::IntersectionsWithClosedbiHexagonRight

Short_t PndSttTrackFinderReal::IntersectionsWithClosedbiHexagonRight(
  //-------- inputs
  Double_t vgap, Double_t Ox, Double_t Oy, Double_t R,
  Double_t Ami, // min Apotema of hexagonal  volume intersected by track;
  Double_t Ama, // max Apotema of hexagonal volume intersected by track;
  //-------- outputs
  Short_t *nIntersections, Double_t *XintersectionList, Double_t *YintersectionList)
{

  // return integer convention :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon;
  // 1 -->  track contained completely between the two polygons;

  //  inner Hexagon --> 0
  //  outer Hexagon --> 1

  bool internal, AtLeast1;

  Short_t i, is, j, Nintersections;

  Double_t aaa, maxdistq, distance,
    //-------------------
    // a,b,c == coefficients of the implicit equations of the 3 sides of the half inner Hexagon
    // plus 3 sides of the half outer Hexagon plus 2 vertical sides corresponding to the Gap :
    //    a*x + b*y +c =0; the numbering of these 8 sides foolows the convention of Gianluigi's
    // logbook on page 286.
    a[] = {1. / sqrt(3.), 1., -1. / sqrt(3.), 1., -1. / sqrt(3.), 1., 1. / sqrt(3.), 1.}, b[] = {1., 0., 1., 0., 1., 0., 1., 0.},
    c[] = {-2. * Ama / sqrt(3.), -Ama, 2. * Ama / sqrt(3.), -vgap / 2., 2. * Ami / sqrt(3.), -Ami, -2. * Ami / sqrt(3.), -vgap / 2.},
    //----------------------

    tempX[2], tempY[2];

  // side_x and side_y are ordered as side1, side2, ..... in Gianluigi's logbook on page 286.
  Double_t side_x[] = {vgap / 2., Ama, Ama, vgap / 2., vgap / 2., Ami, Ami, vgap / 2., vgap / 2.},
           side_y[] = {(-0.5 * vgap + 2. * Ama) / sqrt(3.),  Ama / sqrt(3.),  -Ama / sqrt(3.), -(-0.5 * vgap + 2. * Ama) / sqrt(3.),
                       -(-0.5 * vgap + 2. * Ami) / sqrt(3.), -Ami / sqrt(3.), Ami / sqrt(3.),  (-0.5 * vgap + 2. * Ami) / sqrt(3.),
                       (-0.5 * vgap + 2. * Ama) / sqrt(3.)};

  //-----------------------

  //   find intersections (maximum 16) with the 8 sides.

  AtLeast1 = false;
  *nIntersections = 0;
  internal = true;
  maxdistq = -9999.;
  for (is = 0; is < 8; is++) {
    aaa = (side_x[is] - Ox) * (side_x[is] - Ox) + (side_y[is] - Oy) * (side_y[is] - Oy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    aaa = (side_x[is + 1] - Ox) * (side_x[is + 1] - Ox) + (side_y[is + 1] - Oy) * (side_y[is + 1] - Oy);
    if (aaa > maxdistq)
      maxdistq = aaa;
    if (IntersectionCircle_Segment(a[is], b[is], c[is], side_x[is], side_x[is + 1], side_y[is], side_y[is + 1], Ox, Oy, R, &Nintersections, tempX, tempY,
                                   &distance // distance of (Ox,Oy) from line
                                             // defined by  a*x+b*y+c=0.
                                   )) {
      AtLeast1 = true;
      for (j = 0; j < Nintersections; j++) {
        XintersectionList[*nIntersections] = tempX[j];
        YintersectionList[*nIntersections] = tempY[j];
        (*nIntersections)++;
      }
    } // end of if ( IntersectionCircle_Segment( .....

    // the definition of 'internal' here is when the given Point
    // stays at the same side of the origin (0,0) with respect to
    // the given line of equation   a*x+b*y+c=0.
    if (is < 3) {
      internal = internal && IsInternal(Ox, Oy, a[is], b[is], c[is]);
    } else {
      internal = internal && (!IsInternal(Ox, Oy, a[is], b[is], c[is]));
    }

  } // end of  for(is=0; is<8; is++)

  if (!AtLeast1) {
    if (!internal)
      return -1; // trajectory outside polygon.
    if (maxdistq < R * R)
      return -1; // trajectory outside polygon.
    return 1;    // trajectory completely contained inside this Polygon.
  }
  return 0;
}

//---------- end of  function PndSttTrackFinderReal::IntersectionsWithClosedbiHexagonRight

//----------begin of function PndSttTrackFinderReal::IntersectionCircle_Segment

bool PndSttTrackFinderReal::IntersectionCircle_Segment(Double_t a, // coefficients implicit equation.
                                                       Double_t b, // of segment : a*x + b*y + c =0.
                                                       Double_t c,
                                                       Double_t P1x, // point delimiting the segment.
                                                       Double_t P2x, // point delimiting the segment.
                                                       Double_t P1y, // point delimiting the segment.
                                                       Double_t P2y, // point delimiting the segment.
                                                       Double_t Ox,  // center of circle.
                                                       Double_t Oy,
                                                       Double_t R, // Radius of circle.
                                                       Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2], Double_t *distance)
{

  // this method finds the intersection of a circle with a segment. If the circle
  // passes through an endpoint of the segment, that is considered an intersection also.

  bool status;

  Short_t ipossibility;

  Double_t aperp, bperp, cperp, det, distq, dist1, dist2, length, length_segmentq, Rq, x, y, Xintersection, Yintersection;

  *Nintersections = 0;
  det = a * a + b * b;
  if (det < 1.e-20) {
    cout << "from  PndSttTrackFinderReal::IntersectionCircle_Segment :"
         << " this is not the equation of a segment, return!\n";
    return false;
  }
  //  find if intersection circle - segment is possible.

  // check if there is an intersection (with the squares, it's the same!).
  distq = (a * Ox + b * Oy + c) * (a * Ox + b * Oy + c) / det;
  *distance = sqrt(distq);
  Rq = R * R;
  length = Rq - distq;
  if (length <= 0.)
    return false; // no intersection between trajectory and this
                  // segment.
  length = sqrt(length);
  // coefficients of line perpendicular to input segment and passing for (Ox, Oy).
  aperp = -b;
  bperp = a;
  cperp = -aperp * Ox - bperp * Oy;

  // find intersection of segment with perpendicular : no need to check if
  // det is different from 0.
  Xintersection = (-bperp * c + b * cperp) / det;
  Yintersection = (-a * cperp + aperp * c) / det;
  det = sqrt(det);
  length_segmentq = (P1x - P2x) * (P1x - P2x) + (P1y - P2y) * (P1y - P2y);

  status = false;
  for (ipossibility = -1; ipossibility < 2; ipossibility += 2) {
    x = Xintersection + ipossibility * length * b / det;
    y = Yintersection - ipossibility * length * a / det;
    if ((x - P1x) * (x - P1x) + (y - P1y) * (y - P1y) > length_segmentq || (x - P2x) * (x - P2x) + (y - P2y) * (y - P2y) > length_segmentq)
      continue;
    status = true;
    XintersectionList[*Nintersections] = x;
    YintersectionList[*Nintersections] = y;
    (*Nintersections)++;
  } //  end of  for(ipossibility=-1; ...

  return status;
}

//----------end of function PndSttTrackFinderReal::IntersectionCircle_Segment

//----------begin of function PndSttTrackFinderReal::IntersectionsWithGapSemicircle

Short_t PndSttTrackFinderReal::IntersectionsWithGapSemicircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Double_t GAP,
                                                              bool left, // if true --> Left semicircle; false --> Right semicircle.
                                                              Double_t Rma, Double_t *XintersectionList, Double_t *YintersectionList)
{

  Short_t nIntersectionsCircle;

  Double_t cosFi, theta1, theta2, Theta1, Theta2, aaa, Fi, FI0, x1, x2, y1, y2;

  nIntersectionsCircle = 0;
  aaa = sqrt(Oxx * Oxx + Oyy * Oyy);

  //  preliminary condition for having intersections between trajectory and  Circle.

  if (!(aaa >= Rr + Rma || Rr >= aaa + Rma) && !(aaa + Rr <= Rma || aaa - Rr >= Rma)) {

    //	now the calculation

    FI0 = atan2(-Oyy, -Oxx);
    cosFi = (aaa * aaa + Rr * Rr - Rma * Rma) / (2. * Rr * aaa);
    if (cosFi < -1.)
      cosFi = -1.;
    else if (cosFi > 1.)
      cosFi = 1.;
    Fi = acos(cosFi);

    //  (x1, y1) and (x2,y2) are the intersections between the trajectory
    //  and the circle, in the laboratory reference frame.
    x1 = Oxx + Rr * cos(FI0 - Fi);
    y1 = Oyy + Rr * sin(FI0 - Fi);
    theta1 = atan2(y1, x1); // in this way theta1 is between -PI and PI.
    x2 = Oxx + Rr * cos(FI0 + Fi);
    y2 = Oyy + Rr * sin(FI0 + Fi);
    theta2 = atan2(y2, x2); // in this way theta2 is between -PI and PI.
    //  Theta1, Theta2 = angle of the edges of the outer circle + Gap in the laboratory frame.
    //  Theta1, Theta2 are angles between -PI/2 and +PI/2.
    if (!left) { //  Right (looking into the beam) Semicircle.
      Theta2 = atan2(sqrt(Rma * Rma - GAP * GAP / 4.), GAP / 2.);
      Theta1 = atan2(-sqrt(Rma * Rma - GAP * GAP / 4.), GAP / 2.);
      if (Theta1 <= theta1 && theta1 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x1;
        YintersectionList[nIntersectionsCircle] = y1;
        nIntersectionsCircle++;
      }
      if (Theta1 <= theta2 && theta2 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x2;
        YintersectionList[nIntersectionsCircle] = y2;
        nIntersectionsCircle++;
      }
    } else { //  Left (looking into the beam) Semicircle.
      Theta2 = atan2(-sqrt(Rma * Rma - GAP * GAP / 4.), -GAP / 2.);
      Theta1 = atan2(sqrt(Rma * Rma - GAP * GAP / 4.), -GAP / 2.);
      if (Theta1 <= theta1 || theta1 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x1;
        YintersectionList[nIntersectionsCircle] = y1;
        nIntersectionsCircle++;
      }
      if (Theta1 <= theta2 || theta2 <= Theta2) {
        XintersectionList[nIntersectionsCircle] = x2;
        YintersectionList[nIntersectionsCircle] = y2;
        nIntersectionsCircle++;
      }
    }

  } // end of   if (!( a >= Rr + Rma || .....

  //---------------------------- end of calculation intersection with outer circle.

  return nIntersectionsCircle;
}
//----------end of function PndSttTrackFinderReal::IntersectionsWithGapSemicircle

//----------star of function PndSttTrackFinderReal::IsInternal
bool PndSttTrackFinderReal::IsInternal(Double_t Px, // point
                                       Double_t Py, Double_t Xtraslation, Double_t Ytraslation, Double_t Theta)
{

  // for explanations see Gianluigi's logbook on page 278-280.

  if ((Xtraslation - Px) * sin(Theta) + (Py - Ytraslation) * cos(Theta) >= 0.)
    return true;
  else
    return false;
}
//----------end of function PndSttTrackFinderReal::IsInternal

//----------star of function PndSttTrackFinderReal::ChooseEntranceExit
void PndSttTrackFinderReal::ChooseEntranceExit(Double_t Oxx, Double_t Oyy, Short_t flag, Short_t Charge, Double_t FiStart, Short_t nIntersections[2],
                                               Double_t XintersectionList[][2], //  second index =1 -->inner polygon;
                                               Double_t YintersectionList[][2], //  second index =2 -->outer polygon.
                                               Double_t Xcross[2],              // output
                                               Double_t Ycross[2]               // output
)
{

  Short_t i, j;
  // this method works under the hypothesis that flag=0 or 2 only.

  if (flag == 0) {
    if (Charge > 0) {
      for (j = 0; j < 2; j++) { // j=0 --> inner polygon; j=1 --> outer polygon.
        Short_t auxIndex[nIntersections[j]];
        Double_t fi[nIntersections[j]];
        for (i = 0; i < nIntersections[j]; i++) {
          fi[i] = atan2(YintersectionList[i][j] - Oyy, XintersectionList[i][j] - Oxx);
          if (fi[i] > FiStart)
            fi[i] -= 2. * PI;
          if (fi[i] > FiStart)
            fi[i] = FiStart;
          auxIndex[i] = i;
        } // end of for( i=0;i<nIntersections[j];i++)
        Merge_Sort(nIntersections[j], fi, auxIndex);
        Xcross[j] = XintersectionList[auxIndex[nIntersections[j] - 1]][j];
        Ycross[j] = YintersectionList[auxIndex[nIntersections[j] - 1]][j];
      } //   end of for(j=0;j<2;j++)

    } else {
      for (j = 0; j < 2; j++) { // j=0 --> inner polygon; j=1 --> outer polygon.
        Short_t auxIndex[nIntersections[j]];
        Double_t fi[nIntersections[j]];
        for (i = 0; i < nIntersections[j]; i++) {
          fi[i] = atan2(YintersectionList[i][j] - Oyy, XintersectionList[i][j] - Oxx);
          if (fi[i] < FiStart)
            fi[i] += 2. * PI;
          if (fi[i] < FiStart)
            fi[i] = FiStart;
          auxIndex[i] = i;
        } // end of for( i=0;i<nIntersections[j];i++)
        Merge_Sort(nIntersections[j], fi, auxIndex);
        Xcross[j] = XintersectionList[auxIndex[0]][j];
        Ycross[j] = YintersectionList[auxIndex[0]][j];
      } //   end of for(j=0;j<2;j++)
    }

  } else { // therefore it means that flag = 2

    Short_t auxIndex[nIntersections[0]];
    Double_t fi[nIntersections[0]];
    if (Charge > 0) {
      for (i = 0; i < nIntersections[0]; i++) {
        fi[i] = atan2(YintersectionList[i][0] - Oyy, XintersectionList[i][0] - Oxx);
        if (fi[i] > FiStart)
          fi[i] -= 2. * PI;
        if (fi[i] > FiStart)
          fi[i] = FiStart;
        auxIndex[i] = i;
      } // end of for( i=0;i<nIntersections[0];i++)
      Merge_Sort(nIntersections[0], fi, auxIndex);
      Xcross[0] = XintersectionList[auxIndex[nIntersections[0] - 1]][0];
      Ycross[0] = YintersectionList[auxIndex[nIntersections[0] - 1]][0];
      Xcross[1] = XintersectionList[auxIndex[nIntersections[0] - 2]][0];
      Ycross[1] = YintersectionList[auxIndex[nIntersections[0] - 2]][0];

    } else {
      for (i = 0; i < nIntersections[0]; i++) {
        fi[i] = atan2(YintersectionList[i][0] - Oyy, XintersectionList[i][0] - Oxx);
        if (fi[i] < FiStart)
          fi[i] += 2. * PI;
        if (fi[i] < FiStart)
          fi[i] = FiStart;
        auxIndex[i] = i;
      } // end of for( i=0;i<nIntersections[0];i++)
      Merge_Sort(nIntersections[0], fi, auxIndex);
      Xcross[0] = XintersectionList[auxIndex[0]][0];
      Ycross[0] = YintersectionList[auxIndex[0]][0];
      Xcross[1] = XintersectionList[auxIndex[1]][0];
      Ycross[1] = YintersectionList[auxIndex[1]][0];
    }
  } // end of if(flag == 0)
}
//----------end of function PndSttTrackFinderReal::ChooseEntranceExit

//----------star of function PndSttTrackFinderReal::ChooseEntranceExitbis
void PndSttTrackFinderReal::ChooseEntranceExitbis(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t FiStart, Short_t nIntersections,
                                                  Double_t *XintersectionList, //  second index =1 -->inner polygon;
                                                  Double_t *YintersectionList, //  second index =2 -->outer polygon.
                                                  Double_t Xcross[2],          // output
                                                  Double_t Ycross[2]           // output
)
{

  Short_t i, j;
  // this method works under the hypothesis that there are at least 2 intersections.
  if (nIntersections < 2)
    return;

  if (Charge > 0) {
    Short_t auxIndex[nIntersections];
    Double_t fi[nIntersections];
    for (i = 0; i < nIntersections; i++) {
      fi[i] = atan2(YintersectionList[i] - Oyy, XintersectionList[i] - Oxx);
      if (fi[i] > FiStart)
        fi[i] -= 2. * PI;
      if (fi[i] > FiStart)
        fi[i] = FiStart;
      auxIndex[i] = i;
    } // end of for( i=0;i<nIntersections[j];i++)
    Merge_Sort(nIntersections, fi, auxIndex);
    Xcross[0] = XintersectionList[auxIndex[nIntersections - 1]];
    Ycross[0] = YintersectionList[auxIndex[nIntersections - 1]];
    Xcross[1] = XintersectionList[auxIndex[nIntersections - 2]];
    Ycross[1] = YintersectionList[auxIndex[nIntersections - 2]];

  } else {
    Short_t auxIndex[nIntersections];
    Double_t fi[nIntersections];
    for (i = 0; i < nIntersections; i++) {
      fi[i] = atan2(YintersectionList[i] - Oyy, XintersectionList[i] - Oxx);
      if (fi[i] < FiStart)
        fi[i] += 2. * PI;
      if (fi[i] < FiStart)
        fi[i] = FiStart;
      auxIndex[i] = i;
    } // end of for( i=0;i<nIntersections;i++)
    Merge_Sort(nIntersections, fi, auxIndex);
    Xcross[0] = XintersectionList[auxIndex[0]];
    Ycross[0] = YintersectionList[auxIndex[0]];
    Xcross[1] = XintersectionList[auxIndex[1]];
    Ycross[1] = YintersectionList[auxIndex[1]];
  }
}
//----------end of function PndSttTrackFinderReal::ChooseEntranceExitbis

//----------begin of function PndSttTrackFinderReal::SeparateInnerOuterParallel

void PndSttTrackFinderReal::SeparateInnerOuterParallel(

  // input
  Short_t nHits, Short_t *ListHits, Double_t info[][7], Double_t RStrawDetInnerParMax,

  // output
  Short_t *nInnerHits, Short_t *ListInnerHits, Short_t *nOuterHits, Short_t *ListOuterHits,

  Short_t *nInnerHitsLeft, Short_t *ListInnerHitsLeft, Short_t *nInnerHitsRight, Short_t *ListInnerHitsRight,

  Short_t *nOuterHitsLeft, Short_t *ListOuterHitsLeft, Short_t *nOuterHitsRight, Short_t *ListOuterHitsRight)
{

  Short_t ihit;

  Double_t r;

  //   separation of inner Parallel Stt hits from outer Parallel Stt hits.

  *nInnerHits = 0;
  *nInnerHitsLeft = 0;
  *nInnerHitsRight = 0;
  *nOuterHits = 0;
  *nOuterHitsLeft = 0;
  *nOuterHitsRight = 0;
  for (ihit = 0; ihit < nHits; ihit++) {
    r = sqrt(info[ListHits[ihit]][0] * info[ListHits[ihit]][0] + info[ListHits[ihit]][1] * info[ListHits[ihit]][1]);
    // the value 2.*RStrawDetectorParMax/sqrt(3.) is because RStrawDetectorParMax
    // is an Apotema !
    if (r > 2. * ApotemaMaxInnerParStraw / sqrt(3.)) { // outer Parallel hit.
      ListOuterHits[*nOuterHits] = ListHits[ihit];
      (*nOuterHits)++;
      if (info[ListHits[ihit]][0] < 0.) {
        ListOuterHitsLeft[*nOuterHitsLeft] = ListHits[ihit];
        (*nOuterHitsLeft)++;
      } else {
        ListOuterHitsRight[*nOuterHitsRight] = ListHits[ihit];
        (*nOuterHitsRight)++;
      }
    } else {
      ListInnerHits[*nInnerHits] = ListHits[ihit];
      (*nInnerHits)++;
      if (info[ListHits[ihit]][0] < 0.) {
        ListInnerHitsLeft[*nInnerHitsLeft] = ListHits[ihit];
        (*nInnerHitsLeft)++;
      } else {
        ListInnerHitsRight[*nInnerHitsRight] = ListHits[ihit];
        (*nInnerHitsRight)++;
      }
    }
  }
}

//----------end of function PndSttTrackFinderReal::SeparateInnerOuterParallel

//----------begin of function PndSttTrackFinderReal::FindTrackEntranceExitbiHexagon

Short_t PndSttTrackFinderReal::FindTrackEntranceExitbiHexagon(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                              Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                              Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2])
{
  Short_t flag;
  Short_t nIntersections[2];
  Double_t FiStart,
    XintersectionList[12][2], // first index =0 --> inner Hexagon, =1 --> outer.
    YintersectionList[12][2]; // second index : all the possible intersections
                              // (up to 12 intersections).

  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -2 -->  track outside outer polygon;
  // -1 -->  track contained completely within inner polygon;
  // 0 -->  at least 1 intersection with inner polygon, at least 1 with outer polygon;
  // 1 -->  track contained completely between the two polygons;
  // 2 -->  track contained completely by larger polygons, with intersections in the smaller;
  // 3 -->  track completely outsiede the small polygon with intersections in the bigger.

  flag = IntersectionsWithClosedPolygon(Oxx, Oyy, Rr,
                                        ApotemaMin, // Rmin of the inner part of parallele straws,
                                        ApotemaMax, // max Apotema of the inner part of parallele straws.
                                        nIntersections,
                                        XintersectionList, // XintersectionList[..][0] --> inner polygon,
                                                           // XintersectionList[..][1] --> outer polygon.
                                        YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0 || flag == 2))
    return flag;
  if (flag == 2 && nIntersections[0] < 2) {
    cout << "PndSttTrackFinderReal::FindTrackEntranceExitbiHexagon,"
         << " contraddiction, nIntersections[0]=" << nIntersections[0] << "<2, returning -99!\n";
    return -99;
  }

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  ChooseEntranceExit(Oxx, Oyy, flag, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                     Xcross, // output
                     Ycross  // output
  );

  return flag;
}

//----------end of function PndSttTrackFinderReal::FindTrackEntranceExitbiHexagon

//----------begin of function PndSttTrackFinderReal::FindTrackEntranceExitbiHexagonLeft

Short_t PndSttTrackFinderReal::FindTrackEntranceExitbiHexagonLeft(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                  Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                  Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2])
{
  Short_t flag;
  Short_t nIntersections;
  Double_t FiStart,
    XintersectionList[16], // all the possible intersections
    YintersectionList[16]; // (up to 16 intersections).

  // The following is the form of the Left (looking from downstream into the beam) biHexagon
  // geometrical shape considered in this method :
  //
  /*

          /|
         / |
        /  |
       /  /
      /  /
     /  /
    /  /
    |  |
    |  |
    |  |
    |  |
    \  \
     \  \
      \  \
       \  \
        \  |
         \ |
          \|

  */
  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons;

  flag = IntersectionsWithClosedbiHexagonLeft(vgap, Oxx, Oyy, Rr,
                                              ApotemaMin, // Apotema of the inner Hexagon,
                                              ApotemaMax, // Apotema of the outer Hexagon.
                                              &nIntersections,
                                              XintersectionList, // XintersectionList[..][0] --> inner polygon,
                                                                 // XintersectionList[..][1] --> outer polygon.
                                              YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0))
    return flag;
  if (nIntersections < 2)
    return -1;

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  // so at this point, the intersections are at least 2.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  //----------------------

  return flag;
}

//----------end of function PndSttTrackFinderReal::FindTrackEntranceExitbiHexagonLeft

//----------begin of function PndSttTrackFinderReal::FindTrackEntranceExitbiHexagonRight

Short_t PndSttTrackFinderReal::FindTrackEntranceExitbiHexagonRight(Double_t vgap, Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                   Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                   Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2])
{
  Short_t flag;
  Short_t nIntersections;
  Double_t FiStart,
    XintersectionList[16], // all the possible intersections
    YintersectionList[16]; // (up to 16 intersections).

  // The following is the form of the Left (looking from downstream into the beam) biHexagon
  // geometrical shape considered in this method :
  //
  /*
    |\
    | \
    |  \
     \  \
      \  \
       |  |
       |  |
       /  /
      /  /
     /  /
    |  /
    | /
    |/
  */

  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -1 -->  track outside outer perimeter;
  // 0 -->  at least 1 intersection with polygon, therefore a possible entry and an exit;
  // 1 -->  track contained completely between the two polygons;

  flag = IntersectionsWithClosedbiHexagonRight(vgap, Oxx, Oyy, Rr,
                                               ApotemaMin, // Apotema of the inner Hexagon,
                                               ApotemaMax, // Apotema of the outer Hexagon.
                                               &nIntersections,
                                               XintersectionList, // XintersectionList[..][0] --> inner polygon,
                                                                  // XintersectionList[..][1] --> outer polygon.
                                               YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0))
    return flag;
  if (nIntersections < 2)
    return -1;

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  // so at this point, the intersections are at least 2.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  //----------------------

  return flag;
}

//----------end of function PndSttTrackFinderReal::FindTrackEntranceExitbiHexagonRight

//----------begin of function PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircle

Short_t PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircle(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                  Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                  Double_t ApotemaMax, Double_t Xcross[2], Double_t Ycross[2])
{
  Short_t flag;
  Short_t nIntersections[2];
  Double_t FiStart,
    XintersectionList[12][2], // second index =0 --> inner Hexagon, =1 --> outer.
    YintersectionList[12][2]; // first index : all the possible intersections
                              // (up to 12 intersections).

  // finding all possible intersections with inner parallel straw region.
  // The inner parallel straw region is delimited by two hexagons.

  // flag meaning :
  // -2 -->  track outside outer polygon;
  // -1 -->  track contained completely within inner polygon;
  // 0 -->  at least 1 intersection with inner polygon, at least 1 with outer polygon;
  // 1 -->  track contained completely between the two polygons;
  // 2 -->  track contained completely by larger polygons, with intersections in the smaller;
  // 3 -->  track completely outsiede the small polygon with intersections in the bigger.

  flag = IntersectionsWithClosedPolygon(Oxx, Oyy, Rr,
                                        ApotemaMin, // Rmin of the inner part of parallele straws,
                                        ApotemaMax, // max Apotema of the inner part of parallele straws.
                                        nIntersections,
                                        XintersectionList, // XintersectionList[..][0] --> inner polygon,
                                                           // XintersectionList[..][1] --> outer polygon.
                                        YintersectionList);

  // IMPORTANT :
  // this is true because here it is assumed that the track comes from (0,0,0)
  // otherwise the code must be changed!

  if (!(flag == 0 || flag == 2))
    return flag;
  if (flag == 2 && nIntersections[0] < 2) {
    cout << "PndSttTrackFinderReal::FindTrackEntranceExitbiHexagon,"
         << " contraddiction, nIntersections[0]=" << nIntersections[0] << "<2, returning -99!\n";
    return -99;
  }

  //-------  among all  possible intersection find the entrance point (Xcross[0], Ycross[0])
  //	   and the exit point (Xcross[1], Ycross[1])  of this track.

  //-------- the starting point of the track.
  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  ChooseEntranceExit(Oxx, Oyy, flag, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                     Xcross, // output
                     Ycross  // output
  );

  //----------------------

  return flag;
}

//----------end of function PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircle

//----------begin of function PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircleLeft

Short_t PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircleLeft(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                      Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                      Double_t Rma,        // outer radius of the Circle.
                                                                      Double_t GAP, Double_t Xcross[2], Double_t Ycross[2])
{

  //  This methods finds the intersections between a trajectory coming from (0,0) and
  //  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
  //  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
  //  the pellet target target.
  //  It returns -1 if there are 0 or 1 intersections, 0 if there are at least 2 intersections.

  Double_t cosFi, theta1, theta2, Theta1, Theta2, aaa, Fi, FI0, x1, x2, y1, y2;
  //------------------

  Short_t nIntersectionsCircle, nIntersections;
  Double_t FiStart, XintersectionList[12],
    YintersectionList[12]; // all the possible intersections (up to 12 intersections).

  // finding all possible intersections with inner parallel straw region.

  Double_t Side_x[] = {-GAP / 2., -GAP / 2., -ApotemaMin, -ApotemaMin, -GAP / 2., -GAP / 2.},
           Side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - GAP) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - GAP) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)},
           a[] = {1., -1. / sqrt(3.), 1., 1. / sqrt(3.), 1.}, b[] = {0., 1., 0., 1., 0.},
           c[] = {GAP / 2., -2. * ApotemaMin / sqrt(3.), ApotemaMin, 2. * ApotemaMin / sqrt(3.), GAP / 2.};

  nIntersections = IntersectionsWithOpenPolygon(Oxx, Oyy, Rr,
                                                5, //  n. Sides of open Polygon.
                                                a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
                                                b, c,
                                                Side_x,            // X,Y coordinate of the Sides vertices (in sequence, following
                                                Side_y,            // the Polygon along.
                                                XintersectionList, // XintersectionList
                                                YintersectionList  // YintersectionList.
  );

  //-------------------------------------------------------------------------
  // finding intersections of trajectory [assumed to originate from (0,0) ]
  // with outer semicircle, the Left part.

  nIntersectionsCircle = IntersectionsWithGapSemicircle(Oxx,                                // input from trajectory
                                                        Oyy,                                // input from trajectory
                                                        Rr,                                 // input from trajectory
                                                        GAP,                                // input, vertical gap in XY plane of STT detector.
                                                        true,                               // true --> Left semicircle, false --> Right semicircle.
                                                        Rma,                                // radius of external Stt containment.
                                                        &XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
                                                        &YintersectionList[nIntersections]);
  nIntersections += nIntersectionsCircle;

  //-------- the starting point of the track.

  if (nIntersections < 2)
    return -1;

  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  return 0;
}

//----------end of function PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircleLeft

//----------begin of function PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircleRight

Short_t PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircleRight(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t Charge, Double_t Start[3],
                                                                       Double_t ApotemaMin, // Apotema=distance Hexagon side from (0,0).
                                                                       Double_t Rma,        // outer radius of the Circle.
                                                                       Double_t GAP, Double_t Xcross[2], Double_t Ycross[2])
{

  //  This methods finds the intersections between a trajectory coming from (0,0) and
  //  parameters  Oxx, Oyy, Rr   with the closed geometrical figure (in XY) formed by the STT
  //  external Left semicircle and the Outer STT parallel Left straw semi-Hexagon + Gap for
  //  the pellet target target.
  //  It returns -1 if there are 0 or 1 intersections, 0 if there are at least 2 intersections.

  Double_t cosFi, theta1, theta2, Theta1, Theta2, aaa, Fi, FI0, x1, x2, y1, y2;
  //------------------

  Short_t nIntersectionsCircle, nIntersections;
  Double_t FiStart, XintersectionList[12],
    YintersectionList[12]; // all the possible intersections (up to 10 intersections).

  // finding all possible intersections with inner parallel straw region.

  Double_t Side_x[] = {GAP / 2., GAP / 2., ApotemaMin, ApotemaMin, GAP / 2., GAP / 2.},
           Side_y[] = {sqrt(Rma * Rma - GAP * GAP / 4.), (2. * ApotemaMin - GAP) / sqrt(3.),  ApotemaMin / sqrt(3.),
                       -ApotemaMin / sqrt(3.),           -(2. * ApotemaMin - GAP) / sqrt(3.), -sqrt(Rma * Rma - GAP * GAP / 4.)},
           a[] = {1., 1. / sqrt(3.), 1., -1. / sqrt(3.), 1.}, b[] = {0., 1., 0., 1., 0.},
           c[] = {-GAP / 2., -2. * ApotemaMin / sqrt(3.), -ApotemaMin, 2. * ApotemaMin / sqrt(3.), -GAP / 2.};

  nIntersections = IntersectionsWithOpenPolygon(Oxx, Oyy, Rr,
                                                5, //  n. Sides of open Polygon.
                                                a, //  coefficient of formula :  aX + bY + c = 0 defining the Polygon sides.
                                                b, c,
                                                Side_x,            // X,Y coordinate of the Sides vertices (in sequence, following
                                                Side_y,            // the Polygon along.
                                                XintersectionList, // XintersectionList
                                                YintersectionList  // YintersectionList.
  );

  //-------------------------------------------------------------------------
  // finding intersections of trajectory [assumed to originate from (0,0) ]
  // with outer semicircle, the Left part.

  nIntersectionsCircle = IntersectionsWithGapSemicircle(Oxx,                                // input from trajectory
                                                        Oyy,                                // input from trajectory
                                                        Rr,                                 // input from trajectory
                                                        GAP,                                // input, vertical gap in XY plane of STT detector.
                                                        false,                              // true --> Left semicircle, false --> Right semicircle.
                                                        Rma,                                // radius of external Stt containment.
                                                        &XintersectionList[nIntersections], // output, X list of intersections (maximal 2).
                                                        &YintersectionList[nIntersections]);

  nIntersections += nIntersectionsCircle;

  //-------- the starting point of the track.

  if (nIntersections < 2)
    return -1;

  FiStart = atan2(Start[1] - Oyy, Start[0] - Oxx);

  // this method selects the entrance and exit points of the trajectory among all
  // geometrical intersections of the circular trajectory with the straw particular
  // volume.

  ChooseEntranceExitbis(Oxx, Oyy, Charge, FiStart, nIntersections, XintersectionList, YintersectionList,
                        Xcross, // output
                        Ycross  // output
  );

  return 0;
}
//----------end of function PndSttTrackFinderReal::FindTrackEntranceExitHexagonCircleRight

//----------begin of function PndSttTrackFinderReal::FindIntersectionsOuterCircle

Short_t PndSttTrackFinderReal::FindIntersectionsOuterCircle(Double_t oX, Double_t oY, Double_t R, Double_t Rma, Double_t Xcross[2], Double_t Ycross[2])
{

  // return -1 --> non intersection;
  // return 0  --> 2 intersections.

  Double_t a, cosFi, Fi, FI0;
  a = sqrt(oX * oX + oY * oY);

  // case with no intersections.
  if (a >= R + Rma || R >= a + Rma || a + R <= Rma)
    return -1;

  FI0 = atan2(-oY, -oX);
  cosFi = (a * a + R * R - Rma * Rma) / (2. * R * a);
  if (cosFi < -1.)
    cosFi = -1.;
  else if (cosFi > 1.)
    cosFi = 1.;
  Fi = acos(cosFi);

  Xcross[0] = oX + R * cos(FI0 + Fi);
  Ycross[0] = oY + R * sin(FI0 + Fi);
  Xcross[1] = oX + R * cos(FI0 - Fi);
  Ycross[1] = oY + R * sin(FI0 - Fi);

  return 0;
}
//----------end of function PndSttTrackFinderReal::FindIntersectionsOuterCircle

//----------begin of function PndSttTrackFinderReal::CalculateArcLength

Double_t PndSttTrackFinderReal::CalculateArcLength(Double_t Oxx, Double_t Oyy, Double_t Rr, Short_t charge,
                                                   Double_t Xcross[2], // entrance-exit point
                                                   Double_t Ycross[2]  // entrance-exit point
)
{
  Double_t dis, theta1, theta2;

  theta1 = atan2(Ycross[0] - Oyy, Xcross[0] - Oxx);
  theta2 = atan2(Ycross[1] - Oyy, Xcross[1] - Oxx);

  if (charge > 0) { // the rotation was clockwise.
    dis = theta1 - theta2;
  } else { // the rotation was counterclockwise.
    dis = theta2 - theta1;
  }

  if (dis < 0.)
    dis += 2. * PI;
  if (dis < 0.)
    dis = 0.;

  dis *= Rr;

  return dis;
}

//----------end of function PndSttTrackFinderReal::CalculateArcLength

//----------begin of function PndSttTrackFinderReal::IsInsideCircle

bool PndSttTrackFinderReal::IsInsideArc(Double_t Oxx, Double_t Oyy, Short_t Charge, Double_t Xcross[2], Double_t Ycross[2],
                                        Double_t f // f should be between 0 and 2PI.
)
{

  Double_t f1, f2;

  // Xcross[0],Ycross[0] is the point of entrance.

  f1 = atan2(Ycross[0] - Oyy, Xcross[0] - Oxx);
  if (f1 < 0.)
    f1 += 2. * PI;
  if (f1 < 0.)
    f1 = 0.;
  f2 = atan2(Ycross[1] - Oyy, Xcross[1] - Oxx);
  if (f2 < 0.)
    f2 += 2. * PI;
  if (f2 < 0.)
    f2 = 0.;

  if (Charge < 0) {
    if (f1 > f2)
      f2 += 2. * PI;
    if (f1 > f2)
      f2 = f1;
    if (f > f1) {
      if (f < f2)
        return true;
      else
        return false;
    } else {
      f += 2. * PI;
      if (f < f1)
        f = f1;
      if (f < f2)
        return true;
      else
        return false;
    }
  } else { // Charge > 0.
    if (f1 < f2)
      f1 += 2. * PI;
    if (f1 < f2)
      f1 = f2;
    if (f > f2) {
      if (f < f1)
        return true;
      else
        return false;
    } else {
      f += 2. * PI;
      if (f < f2)
        f = f2;
      if (f < f1)
        return true;
      else
        return false;
    }
  } // end of if(Charge<0)
}

//----------end of function PndSttTrackFinderReal::IsInsideCircle

//----------begin of function PndSttTrackFinderReal::OrderingUsingConformal

void PndSttTrackFinderReal::OrderingUsingConformal(Double_t oX, Double_t oY, Int_t nHits, Double_t XY[][2],
                                                   Int_t Charge, // input
                                                   Short_t *ListHits)
{

  Short_t i, j, tmp[nHits];
  Double_t aaa, b1, firstR2, lastR2, aux[nHits], U[nHits], V[nHits];

  //  here there is the ordering of the hits, under the assumption that the circumference
  //  in XY goes through (0,0).
  //  Moreover, the code before is supposed to have selected trajectories in XY with (Ox,Oy)
  //  farther from (0,0) by > 0.9 * RminStrawDetector/2 and consequently Ox and Oy are not both 0.
  //  The scheme for the ordering of the hit is as follows :
  //  1)  order hits by increasing U or V of the conformal mapping; see Gianluigi's Logbook page 283;
  //  2)  find the charge of the track by checking if it is closest to the center in XY
  //	the first or the last of the ordered hits.
  //  3)  in case, invert the ordering of U, V and ListHits such that the first hits in the
  //	list are alway those closer to the (0,0).

  //   ordering of the hits

  aaa = atan2(oY, oX); // atan2 defined between -PI and PI.

  // the following statement is necessary since for unknown reason the root interpreter
  // gives a weird error when using PI directly in the if statement below!!!!!!! I lost
  // 2 hours trying to figure this out!
  b1 = PI / 4.;

  if ((aaa > b1 && aaa < 3. * b1) || (aaa > -3. * b1 && aaa < -b1)) { // use U as ordering variable;
    //[case 1 or 3 Gianluigi's Logbook page 285].
    for (j = 0; j < nHits; j++) {
      U[j] = XY[j][0] / (XY[j][0] * XY[j][0] + XY[j][1] * XY[j][1]);
    }
    Merge_Sort(nHits, U, ListHits);

    if ((aaa > b1 && aaa < 3. * b1)) { //  case #1;
      if (Charge == -1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      }
    } else { //  case # 3.
      if (Charge == 1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      } // end of  if( Charge ==1)
    }   // end of  if((aaa>b1&&aaa<3.*b1))

  } else { // use V as ordering variable [case 2 or 4 Gianluigi's Logbook page 285].
    for (j = 0; j < nHits; j++) {
      V[j] = XY[j][1] / (XY[j][0] * XY[j][0] + XY[j][1] * XY[j][1]);
    }
    Merge_Sort(nHits, V, ListHits);

    if ((aaa <= -3. * b1 || aaa >= 3. * b1)) { //  case #2;
      if (Charge == -1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      }
    } else { //  case # 4.
      if (Charge == 1) {
        // inverting the order of the hits.
        for (i = 0; i < nHits; i++) {
          tmp[i] = ListHits[nHits - 1 - i];
        }
        for (i = 0; i < nHits; i++) {
          ListHits[i] = tmp[i];
        }
      }
    }

  } //  end of   if((aaa>b1&& ....

  return;
}
//----------end of function PndSttTrackFinderReal::OrderingUsingConformal

//----------begin of function PndSttTrackFinderReal::FindTrackInXYProjection

bool PndSttTrackFinderReal::FindTrackInXYProjection(Short_t iHit,    // seed hit; if it is negative it is a SciTil hit.
                                                    Short_t nRcell,  // R cell of the seed hit; can be negative when SciTil hit.
                                                    Short_t nFicell, // Fi cell of the seed hit;
                                                    Int_t *Minclinations, Double_t info[nmaxHits][7], bool *InclusionList, Short_t *RConformalIndex, Short_t *FiConformalIndex,
                                                    Short_t nBoxConformal[nRdivConformal][nFidivConformal],
                                                    Short_t HitsinBoxConformal[MAXHITSINCELL][nRdivConformal][nFidivConformal], Short_t nTracksFoundSoFar, Short_t *nHitsinTrack,
                                                    Short_t ListHitsinTrack[MAXTRACKSPEREVENT][nmaxHitsInTrack], Double_t *trajectory_vertex,
                                                    Double_t infoparalConformal[nmaxHits][5], Double_t posizSciTilx, Double_t posizSciTily, Double_t *S, Double_t *Ox, Double_t *Oy,
                                                    Double_t *R, Double_t *Fi_low_limit, Double_t *Fi_up_limit, Double_t *Fi_initial_helix_referenceframe,
                                                    Double_t *Fi_final_helix_referenceframe, Short_t *Charge, Double_t *U, Double_t *V)
{

  //---------------

  Short_t i, j, Naux, Nbaux, NN, Nouter, auxListHitsinTrack[nmaxHits], OutputListHitsinTrack[nmaxHits], OutputList2HitsinTrack[nmaxHits], ListHitsinTrackinWhichToSearch[nmaxHits];

  Short_t flagStt, status;

  Double_t aaa, m, q, rotationangle, rotationcos, rotationsin, auxinfoparalConformal[nmaxHits + 1][5];

  //----------------
  nHitsinTrack[nTracksFoundSoFar] = PndSttFindTrackPatterninBoxConformal(1, //  distance in R cells allowed
                                                                         2, //  distance in Fi cells allowed
                                                                         Minclinations[0],
                                                                         iHit, // seed hit; if it is negative it is a SciTil hit.
                                                                         nRcell, nFicell, info, InclusionList, RConformalIndex, FiConformalIndex, nBoxConformal, HitsinBoxConformal,
                                                                         &ListHitsinTrack[nTracksFoundSoFar][0]);
  //---------stampe.
  if (istampa > 0) {
    cout << "PndSttTrackFinderReal, evt. " << IVOLTE << ", dopo PndSttFindTrackPatterninBoxConformal; nTracksFoundSoFar  " << nTracksFoundSoFar << ", nHitsinTrack "
         << nHitsinTrack[nTracksFoundSoFar] << ", ;loro lista :\n";
    for (int iz = 0; iz < nHitsinTrack[nTracksFoundSoFar]; iz++) {
      cout << "\thit n. (parallel numbering) " << ListHitsinTrack[nTracksFoundSoFar][iz] << endl;
    }
  }
  //------------------------------fine stampe.

  if (nHitsinTrack[nTracksFoundSoFar] < MINIMUMHITSPERTRACK || nHitsinTrack[nTracksFoundSoFar] > nmaxHitsInTrack) {
    return false;
  }
  //-----------------------

  //   find among the ListHitsinTrack  if there are at least
  //   a minimum # of hits belonging to the outer part of the STT  system.
  //   At this point of the code the Stt hits are already ordered from
  //   the outermost to the innermost.

  for (j = 0, Nouter = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
    if (info[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]][0] * info[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]][0] +
          info[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]][1] * info[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]][1] <
        ApotemaMaxSkewStraw * ApotemaMaxSkewStraw)
      break;
    Nouter++;
  }

  if (Nouter >= MINIMUMOUTERHITSPERTRACK) {
    for (i = 0; i < Nouter; i++) {
      ListHitsinTrackinWhichToSearch[i] = ListHitsinTrack[nTracksFoundSoFar][i];
    }

    for (i = 0; i < Nouter; i++) {
      Naux = PndSttFindTrackPatterninBoxConformalSpecial(3, // NRCELLDISTANCE
                                                         1, // NFiCELLDISTANCE
                                                         Minclinations[0], Nouter,
                                                         ListHitsinTrackinWhichToSearch[i], // seed hit.
                                                         ListHitsinTrackinWhichToSearch, info, InclusionList, RConformalIndex, FiConformalIndex, nBoxConformal, HitsinBoxConformal,
                                                         OutputListHitsinTrack);
      if (Naux >= MINIMUMOUTERHITSPERTRACK && Naux > 0.7 * Nouter)
        break;
      if (Naux >= MINIMUMOUTERHITSPERTRACK) {

        //   further collection of hits in the inner region but this time strictly connected
        //   to the outer ones

        //  first the list of non outer hits
        for (j = Nouter; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
          ListHitsinTrackinWhichToSearch[j - Nouter] = ListHitsinTrack[nTracksFoundSoFar][j];
        }

        Nbaux = PndSttFindTrackStrictCollection(1,                                 // NFiCELLDISTANCE
                                                ListHitsinTrackinWhichToSearch[i], //  seed hit
                                                nHitsinTrack[nTracksFoundSoFar] - Nouter, ListHitsinTrackinWhichToSearch, InclusionList, FiConformalIndex, OutputList2HitsinTrack);
        //   add the new hits found to the list

        nHitsinTrack[nTracksFoundSoFar] = Naux + Nbaux;
        if (nHitsinTrack[nTracksFoundSoFar] >= MINIMUMHITSPERTRACK && nHitsinTrack[nTracksFoundSoFar] <= nmaxHitsInTrack) {
          for (j = 0; j < Naux; j++) {
            ListHitsinTrack[nTracksFoundSoFar][j] = OutputListHitsinTrack[j];
          }
          for (j = 0; j < Nbaux; j++) {
            ListHitsinTrack[nTracksFoundSoFar][Naux + j] = OutputList2HitsinTrack[j];
          }
          break;

        } // end of  if( nHitsinTrack[nTracksFoundSoFar] >= ....
      }   // end of  if( Naux >= MINIMUMOUTERHITSPERTRACK)
    }     // end of for(i=0; i< Nouter;i++)
  }       // end of if( Nouter >= MINIMUMOUTERHITSPERTRACK)

  if (nHitsinTrack[nTracksFoundSoFar] < MINIMUMHITSPERTRACK || nHitsinTrack[nTracksFoundSoFar] > nmaxHitsInTrack) {
    return false;
  }

  //---------------------------

  //  finding the rotation angle for best utilization of the MILP procedure

  for (j = 0, rotationcos = 0., rotationsin = 0.; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
    rotationcos += cos((0.5 + FiConformalIndex[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]]) * 2. * PI / nFidivConformal);

    rotationsin += sin((0.5 + FiConformalIndex[infoparal[ListHitsinTrack[nTracksFoundSoFar][j]]]) * 2. * PI / nFidivConformal);
  }
  rotationcos /= nHitsinTrack[nTracksFoundSoFar];
  rotationsin /= nHitsinTrack[nTracksFoundSoFar];
  rotationangle = atan2(rotationsin, rotationcos);

  //  fitting with superfast MILP code

  //  loading the conformal infos necessary in the fit :
  //  auxinfoparalConformal[j][0] = U;
  //  auxinfoparalConformal[j][1] = V;
  //  auxinfoparalConformal[j][2] = drift radius in conformal space; for the SciTil
  //				  the SciTil dimension (2.85 cm)/Rmiddle**2 with
  //				  Rmiddle = distance middle of SciTil from (0,0), so
  //				  = DIMENSIONSCITIL/RMAXSCITIL**2.

  bool Type;
  int nFitPoints, offset;
  Double_t Xconformal[1 + nHitsinTrack[nTracksFoundSoFar]], Yconformal[1 + nHitsinTrack[nTracksFoundSoFar]], DriftRadiusconformal[1 + nHitsinTrack[nTracksFoundSoFar]],
    ErrorDriftRadiusconformal[1 + nHitsinTrack[nTracksFoundSoFar]];

  if (iHit < 0) { // case with a hit in the SciTil
    aaa = posizSciTilx * posizSciTilx + posizSciTily * posizSciTily;
    Xconformal[0] = posizSciTilx / aaa;
    Yconformal[0] = posizSciTily / aaa;
    ErrorDriftRadiusconformal[0] = DIMENSIONSCITIL / aaa;
    DriftRadiusconformal[0] = -1.; // treat it like it is a Mvd hit.

    // +1 comes from one SciTil hit.
    offset = 1;
    nFitPoints = nHitsinTrack[nTracksFoundSoFar] + 1;

  } else { // no SciTil hit.
    offset = 0;
    nFitPoints = nHitsinTrack[nTracksFoundSoFar];
  } // end of  if(iHit<0)

  for (j = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
    Xconformal[j + offset] = infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][0];
    Yconformal[j + offset] = infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][1];
    ErrorDriftRadiusconformal[j + offset] = infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][2];
    DriftRadiusconformal[j + offset] = infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][2];
  }

  //------------------------
  if (istampa > 0) {

    cout << "PndSttTrackFinderReal, in FindTrackInXYProjection,  evt. " << IVOLTE << ", nTracksFoundSoFar " << nTracksFoundSoFar << " nHits " << nHitsinTrack[nTracksFoundSoFar]
         << ", iseed " << iHit << endl;
    cout << "\tLista Stt Hits :\n";
    for (int iz = 0; iz < nHitsinTrack[nTracksFoundSoFar]; iz++) {
      cout << "\tStt hit n. " << ListHitsinTrack[nTracksFoundSoFar][iz] << endl;
    }
    cout << "\tLista info in Conformal :\n";
    int nummm;
    if (iHit < 0) {
      nummm = nHitsinTrack[nTracksFoundSoFar] + 1;
    } else {
      nummm = nHitsinTrack[nTracksFoundSoFar];
    };
    for (int iz = 0; iz < nummm; iz++) {
      cout << "\t" << Xconformal[iz] << ",  " << Yconformal[iz] << ",  " << ErrorDriftRadiusconformal[iz] << endl;
    }
  }
  //--------------------

  status = FitHelixCylinder(nFitPoints, // +1 comes from one SciTil hit.
                            Xconformal, Yconformal, DriftRadiusconformal, ErrorDriftRadiusconformal,
                            rotationangle,     //  rotationangle, da mettere
                            trajectory_vertex, //  vertex in (X,Y) of this trajectory
                            NHITSINFIT,        //  maximum n. of hits allowed in fast fit
                            &m, &q, &ALFA[nTracksFoundSoFar], &BETA[nTracksFoundSoFar], &GAMMA[nTracksFoundSoFar], &TypeConf[nTracksFoundSoFar]);

  if (status < 0)
    return false;

  //  this trasformation is valid even if the equation is a straight line from the fit

  Ox[nTracksFoundSoFar] = -0.5 * ALFA[nTracksFoundSoFar];
  Oy[nTracksFoundSoFar] = -0.5 * BETA[nTracksFoundSoFar];
  R[nTracksFoundSoFar] = Ox[nTracksFoundSoFar] * Ox[nTracksFoundSoFar] + Oy[nTracksFoundSoFar] * Oy[nTracksFoundSoFar] - GAMMA[nTracksFoundSoFar];

  // some obvious preliminary cuts
  if (R[nTracksFoundSoFar] < 0.)
    return false;
  R[nTracksFoundSoFar] = sqrt(R[nTracksFoundSoFar]);
  aaa = sqrt(Ox[nTracksFoundSoFar] * Ox[nTracksFoundSoFar] + Oy[nTracksFoundSoFar] * Oy[nTracksFoundSoFar]);

  // the following is because the circumference is supposed to come from (0,0);
  //   here the factor 0.9 is used in order to be conservative.
  if (aaa < 0.9 * RStrawDetectorMin / 2.)
    return false;

  //   here the factor 0.9 is used in order to be conservative.
  if (R[nTracksFoundSoFar] + aaa < RStrawDetectorMin * 0.9)
    return false;

  //---------------------------

  //  check again if the SciTil hit (if present) is compatible with this circle trajectory
  //  in  XY by finding if it has intersection (in the XY plane) with Helix circle

  //  equation of the SciTil segment :  y0 * y + x0 * x - x0**2 - y0**2 = 0
  //  where  (x0,y0) = position of center of the SciTil.

  //  delimiting points of the SciTil segment :  define L = length of the SciTil,
  //  and RR = sqrt(x0**2+y0**2), SIGN = the sign of (-x0*y0) or SIGN=1 when y0=0,
  //  SIGN=irrelevant when x0=0;   then :
  //  P1 =  [ x0- abs{(L/2)*y0/RR}; y0-SIGN*abs{(L/2)*x0/RR} ],
  //  P2 =  [ x0+abs{(L/2)*y0/RR}; y0+SIGN*abs{(L/2)*x0/RR} ].

  bool intersect;
  Short_t Nint, nSciT;
  Double_t distance, QQ, sqrtRR, SIGN, XintersectionList[2], YintersectionList[2];

  // whether or not the seed hit was a Stt hit try if any SciTil hits are
  // associated to this track cand.

  if (YesSciTil) {
    // nScit is the n. of SciTil hit associated to this track.
    nSciT = AssociateSciTilHit(Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], R[nTracksFoundSoFar], &ListSciTilHitsinTrack[nTracksFoundSoFar][0],
                               S // output; S on the lateral face of the Helix
                                 // of the SciTil hit (if present).
    );
    if (nSciT > 0) {
      nSciTilHitsinTrack[nTracksFoundSoFar] = nSciT;
      for (j = 0; j < nSciTilHitsinTrack[nTracksFoundSoFar]; j++) {
        InclusionListSciTil[ListSciTilHitsinTrack[nTracksFoundSoFar][0]] = false;
        S_SciTilHitsinTrack[nTracksFoundSoFar][j] = S[j];
      }
    } else {
      nSciTilHitsinTrack[nTracksFoundSoFar] = 0;
    }
  } else {
    nSciTilHitsinTrack[nTracksFoundSoFar] = 0;
  } // end of if(YesSciTil)

  //---------------------  better association of the hits in the track candidate
  // treat differently the case in which the track has radius < RStrawDetectorMax/2
  // and the other case.

  if (R[nTracksFoundSoFar] < RStrawDetectorMax / 2) {
    PndSttFindingParallelTrackAngularRange(Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], R[nTracksFoundSoFar],
                                           1, /// this is supposed to be the charge, irrelevant here if it is +1 or -1.
                                           &Fi_low_limit[nTracksFoundSoFar], &Fi_up_limit[nTracksFoundSoFar], &flagStt, RStrawDetectorMin, RStrawDetectorMax);

    NN = PndSttTrkAssociatedParallelHitsToHelix5(InclusionList, Minclinations[0], Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], R[nTracksFoundSoFar], info,
                                                 Fi_low_limit[nTracksFoundSoFar], Fi_up_limit[nTracksFoundSoFar],
                                                 auxListHitsinTrack //  this is the output
    );

  } else {

    NN = PndSttTrkAssociatedParallelHitsToHelixQuater(InclusionList, m, q, status, nHitsinTrack[nTracksFoundSoFar], &ListHitsinTrack[nTracksFoundSoFar][0], Minclinations[0],
                                                      Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], R[nTracksFoundSoFar], info, infoparalConformal, RConformalIndex,
                                                      FiConformalIndex, nBoxConformal, HitsinBoxConformal,
                                                      auxListHitsinTrack //  this is the output
    );
  } // end of  if( R[nTracksFoundSoFar] < RStrawDetectorMax/2)

  if (NN < MINIMUMHITSPERTRACK || NN > nmaxHitsInTrack)
    return false;

  nHitsinTrack[nTracksFoundSoFar] = NN;

  for (i = 0; i < nHitsinTrack[nTracksFoundSoFar]; i++) {
    ListHitsinTrack[nTracksFoundSoFar][i] = auxListHitsinTrack[i];
  }

  //----------------------------- Finding the Charge
  // The charge is calculated first by dividing the hits in two categories by using the Perpendicular
  // to the tangent to the trajectory in (0,0). Then simply the majority of the hits decides the
  // sign of the  charge. See Gianluigi's Logbook page 290.

  Double_t X[nHitsinTrack[nTracksFoundSoFar]], Y[nHitsinTrack[nTracksFoundSoFar]];

  for (i = 0; i < nHitsinTrack[nTracksFoundSoFar]; i++) {
    X[i] = info[infoparal[ListHitsinTrack[nTracksFoundSoFar][i]]][0];
    Y[i] = info[infoparal[ListHitsinTrack[nTracksFoundSoFar][i]]][1];
  }
  FindCharge(Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], nHitsinTrack[nTracksFoundSoFar], X, Y, &Charge[nTracksFoundSoFar]);

  //----------------------------- Ordering.
  // The ordering reflects the angle Fi in the Helix reference frame because
  // the hits are ordered by going around the trajectory cclockwise for
  // positive tracks or counterclockwise for negative particles; in other words
  //  it is not used simply the distance of the hit from (0,0) as ordering parameter
  //  but the track length of the circle.
  // this method finds also Fi_initial_helix_referenceframe and Fi_final_helix_referenceframe.
  // The former is simply the fi angle of the point (0,0) with respect to the center
  // of this Helix. Important : this angle has to be > 0 always and it is between 0. and
  // 2 PI here (later,  FixDiscontinuitiesFiangleinSZplane may change it adding +2PI or
  // -2PI if necessary).
  // Fi_final_helix_referenceframe is made such that  it is < Fi_initial_helix_referenceframe
  // when the track is  positive, and it is > Fi_initial_helix_referenceframe for negative
  // tracks.

  PndSttOrderingParallel(Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], info, nHitsinTrack[nTracksFoundSoFar], &ListHitsinTrack[nTracksFoundSoFar][0], infoparal,
                         Charge[nTracksFoundSoFar],
                         &Fi_initial_helix_referenceframe[nTracksFoundSoFar], // output
                         &Fi_final_helix_referenceframe[nTracksFoundSoFar],   // output
                         U, V);

  //   finding the FI angular range (in the laboratory frame) spanned by this parallel track

  PndSttFindingParallelTrackAngularRange(Ox[nTracksFoundSoFar], Oy[nTracksFoundSoFar], R[nTracksFoundSoFar], Charge[nTracksFoundSoFar], &Fi_low_limit[nTracksFoundSoFar],
                                         &Fi_up_limit[nTracksFoundSoFar], &flagStt, RStrawDetectorMin, RStrawDetectorMax);

  if (flagStt == -2)
    return false; // track outside cylinder RStrawDetectorMax.
  if (flagStt == -1)
    return false; // track inside cylinder RStrawDetectorMin.
  if (flagStt == 1)
    return false; // track comprised in cylinder : discard,
                  // because at this stage only tracks from
                  // vertex are searched.

  //--------------------------------------------------    macro for display

  for (j = 0; j < nHitsinTrack[nTracksFoundSoFar]; j++) {
    for (i = 0; i < 5; i++) {
      auxinfoparalConformal[j][i] = infoparalConformal[ListHitsinTrack[nTracksFoundSoFar][j]][i];
    }
  }

  /*
  if(iplotta && IVOLTE <= nmassimo){
          WriteMacroParallelHitsConformalwithMCspecial(
                     nHitsinTrack[nTracksFoundSoFar],
                     auxinfoparalConformal,
                     nTracksFoundSoFar,
                     status,
         trajectory_vertex
                                                       );
  }
  */

  //----------------------------------- end macro for display

  return true;
};

//----------end of function PndSttTrackFinderReal::FindTrackInXYProjection

//----------begin of function PndSttTrackFinderReal::AssociateSciTilHit

Short_t PndSttTrackFinderReal::AssociateSciTilHit(Double_t Oxx, Double_t Oyy, Double_t Rr,
                                                  Short_t *List, // output
                                                  Double_t *esse // output
)
{

  bool intersect;

  Short_t igoodScit, iScitHit, Nint;

  Double_t distance, QQ, sqrtRR, SIGN, XintersectionList[2], YintersectionList[2];

  igoodScit = 0;
  for (iScitHit = 0; iScitHit < nSciTilHits; iScitHit++) {
    if (!InclusionListSciTil[iScitHit])
      continue;

    intersect = IntersectionSciTil_Circle(posizSciTil[iScitHit][0], posizSciTil[iScitHit][1], Oxx, Oyy, Rr,
                                          &Nint,             // output
                                          XintersectionList, // output
                                          YintersectionList  // output
    );

    if (intersect) {
      List[igoodScit] = iScitHit;

      // calculate S on the lateral face of the Helix.
      if (Nint == 1) { // the majority of the cases
        esse[igoodScit] = atan2(YintersectionList[0] - Oyy, XintersectionList[0] - Oxx);
      } else { // in this case Nint=2 (it should be a very rare case).
        // do an average of the two positions.
        esse[igoodScit] = atan2(0.5 * (YintersectionList[0] + YintersectionList[1]) - Oyy, 0.5 * (XintersectionList[0] + XintersectionList[1]) - Oxx);
      } // end of  if ( Nint==1)
      if (esse[igoodScit] < 0.)
        esse[igoodScit] += 2. * PI;
      igoodScit++;
      if (igoodScit == nmaxSciTilHitsinTrack)
        break;

    } // end of  if(intersect && distance<olddist)
  }   // end of  for(iScitHit=0; iScitHit<nScitHits; iScitHit++)

  return igoodScit;
}

//----------end of function PndSttTrackFinderReal::AssociateSciTilHit

//----------begin of function PndSttTrackFinderReal::IntersectionSciTil_Circle
bool PndSttTrackFinderReal::IntersectionSciTil_Circle(Double_t posizSciTilx, Double_t posizSciTily,
                                                      Double_t Oxx, // center of circle.
                                                      Double_t Oyy,
                                                      Double_t Rr, // Radius of circle.
                                                      Short_t *Nintersections, Double_t XintersectionList[2], Double_t YintersectionList[2])
{

  bool intersect;

  Double_t distance, QQ, sqrtRR, SIGN;

  QQ = posizSciTilx * posizSciTilx + posizSciTily * posizSciTily;
  sqrtRR = sqrt(QQ);

  if (posizSciTily < 0.)
    SIGN = -1.;
  else
    SIGN = 1.;

  intersect = IntersectionCircle_Segment(posizSciTilx, posizSciTily, -QQ, posizSciTilx - SIGN * 0.5 * DIMENSIONSCITIL * posizSciTily / sqrtRR,
                                         posizSciTilx + SIGN * 0.5 * DIMENSIONSCITIL * posizSciTily / sqrtRR, posizSciTily - SIGN * posizSciTilx * 0.5 * DIMENSIONSCITIL / sqrtRR,
                                         posizSciTily + SIGN * posizSciTilx * 0.5 * DIMENSIONSCITIL / sqrtRR, Oxx, Oyy, Rr,
                                         Nintersections,    // output
                                         XintersectionList, // output
                                         YintersectionList, // output
                                         &distance          // output
  );

  return intersect;
}
//----------end of function PndSttTrackFinderReal::IntersectionSciTil_Circle

//----------begin of function PndSttTrackFinderReal::disegnaSciTilHit

void PndSttTrackFinderReal::disegnaSciTilHit(FILE *MACRO, int ScitilHit, double posx, double posy,
                                             int tipo // 0 --> disegna in XY, 1 --> in SZ, altro --> in UV.
)
{
  double x1, x2, y1, y2, L, R, RR;

  L = DIMENSIONSCITIL / 2.;

  if (tipo == 0) { // SciTil disegnate in XY.
    R = sqrt(posx * posx + posy * posy);
    x1 = posx + posy * L / R;
    x2 = posx - posy * L / R;
    y1 = posy - posx * L / R;
    y2 = posy + posx * L / R;
  } else if (tipo == 1) { // SciTil disegnate in SZ.
    x1 = posx - L;
    x2 = posx + L;
    y1 = posy;
    y2 = posy;
  } else { // SciTil disegnate in UV.
    RR = posx * posx + posy * posy;
    R = sqrt(RR);
    x1 = posx + posy * L / R;
    x1 /= RR;
    x2 = posx - posy * L / R;
    x2 /= RR;
    y1 = posy - posx * L / R;
    y1 /= RR;
    y2 = posy + posx * L / R;
    y2 /= RR;
  }

  fprintf(MACRO, "TLine *Tile%d = new TLine(%f,%f,%f,%f);\n", ScitilHit, x1, y1, x2, y2);
  fprintf(MACRO, "Tile%d->SetLineColor(1);\n", ScitilHit);
  if (tipo == 0) {
    // disegna in XY.
    fprintf(MACRO, "Tile%d->SetLineWidth(2);\n", ScitilHit);
  } else if (tipo == 1) {
    // disegna in SZ.
    fprintf(MACRO, "Tile%d->SetLineWidth(3);\n", ScitilHit);
  } else {
    // disegna in UV.
    fprintf(MACRO, "Tile%d->SetLineWidth(3);\n", ScitilHit);
  }
  fprintf(MACRO, "Tile%d->Draw();\n", ScitilHit);
  /*
    fprintf(MACRO,"TMarker* SciT%d = new TMarker(%f,%f,%d);\n",
        ScitilHit,posx,posy,30);
    fprintf(MACRO,"SciT%d->SetMarkerSize(1.5);\n",ScitilHit);
    fprintf(MACRO,"SciT%d->SetMarkerColor(1);\nSciT%d->Draw();\n"
          ,ScitilHit,ScitilHit);
  */
}

//----------end of function PndSttTrackFinderReal::disegnaSciTilHit

//----------begin of function PndSttTrackFinderReal::disegnaassiXY
void PndSttTrackFinderReal::disegnaAssiXY(FILE *MACRO, double xmin, double xmax, double ymin, double ymax)
{

  fprintf(MACRO, "TGaxis *Assex = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", xmin, 0., xmax, 0., xmin, xmax);
  fprintf(MACRO, "Assex->SetTitle(\"X\");\n");
  fprintf(MACRO, "Assex->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assex->Draw();\n");
  fprintf(MACRO, "TGaxis *Assey = new  TGaxis(%f,%f,%f,%f,%f,%f,510);\n", 0., ymin, 0., ymax, ymin, ymax);
  fprintf(MACRO, "Assey->SetTitle(\"Y\");\n");
  fprintf(MACRO, "Assey->SetTitleOffset(1.5);\n");
  fprintf(MACRO, "Assey->Draw();\n");
}

//----------end of function PndSttTrackFinderReal::disegnaassiXY

ClassImp(PndSttTrackFinderReal)
