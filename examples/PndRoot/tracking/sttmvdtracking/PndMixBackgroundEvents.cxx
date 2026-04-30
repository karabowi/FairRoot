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

#include "PndMixBackgroundEvents.h"

#include "PndSttHit.h"
#include "PndSttTrack.h"
#include "PndSttPoint.h"
#include "PndSttHelixHit.h"
#include "PndSttSingleStraw.h"
#include "PndSttTube.h"
#include "PndSttMapCreator.h"

#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"

#include "PndTrackCand.h"
#include "PndTrackCandHit.h"
#include "PndTrack.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"

#include "TFile.h"
#include "TGeoManager.h"
#include "TClonesArray.h"
#include "TGeoVolume.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TH1F.h"
#include "TMath.h"
#include "TCanvas.h"
#include "TGeoTube.h"

#include <iostream>
#include <cmath>

using namespace std;

const Double_t PndMixBackgroundEvents::MVDTYPICALTIME = 10., // in nsec; time after which the Mvd hit disappears.
  PndMixBackgroundEvents::STRAWRADIUS = 0.5,                 // in cm.
  PndMixBackgroundEvents::STTdriftVEL = 0.0025,              //   in cm/nsec
  PndMixBackgroundEvents::MAXSTTdriftTIME = 200.;            //   in nsec

// -----   Default constructor   -------------------------------------------
PndMixBackgroundEvents::PndMixBackgroundEvents() : FairTask("Mixing bkgrnd hits to Stt-Mvd")
{
  fInteractionRate = 20.;
  fPersistence = kTRUE;
  fVerbose = 0;
  //  IVOLTE=-1;
  //  istampa=2;

  Initialization_ClassVariables();
}
// -------------------------------------------------------------------------

PndMixBackgroundEvents::PndMixBackgroundEvents(Int_t verbose) : FairTask("STT Stt-Mvd Tracking")
{
  fInteractionRate = 20.;
  fPersistence = kTRUE;
  fVerbose = verbose;
  //  IVOLTE=-1;
  //  istampa=2;

  Initialization_ClassVariables();
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndMixBackgroundEvents::~PndMixBackgroundEvents()
{
  delete filedigirun;
  delete filerecorun;
}
// -------------------------------------------------------------------------

//--------------- begin  PndMixBackgroundEvents::Initialization_ClassVariables

void PndMixBackgroundEvents::Initialization_ClassVariables()
{
  // this is only for initializing the Class Variables.

  size_t len;

  //  Bool_t

  fPersistence = false;

  // char :

  len = sizeof(fSttBkgFilename);
  memset(fSttBkgFilename, 0, len);

  len = sizeof(fMvdBkgFilename);
  memset(fMvdBkgFilename, 0, len);

  // pointers

  filedigirun = nullptr;
  filerecorun = nullptr;
  fMCTrackArray = nullptr;
  fMvdPixelHitandBckgrndArray = nullptr;
  fMvdPixelHitArray = nullptr;
  fMvdPixelHitBkgArray = nullptr;
  fMvdStripHitandBckgrndArray = nullptr;
  fMvdStripHitArray = nullptr;
  fMvdStripHitBkgArray = nullptr;
  fSttHitandBckgrndArray = nullptr;
  fSttHitArray = nullptr;
  fSttHitBkgArray = nullptr;
  fSttParameters = nullptr;
  fSttTubeArray = nullptr;
  treedigibkg = nullptr;
  treerecobkg = nullptr;

  return;
}
//--------------- end  PndMixBackgroundEvents::Initialization_ClassVariables

// -----   Public method Init   --------------------------------------------
InitStatus PndMixBackgroundEvents::Init()
{

  // Get RootManager
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndMixBackgroundEvents::Init: "
         << "RootManager not instantiated, return!" << endl;
    return kFATAL;
  }
  //  -----   maps of STT tubes
  // CHECK added
  PndSttMapCreator *mapper = new PndSttMapCreator(fSttParameters);
  fSttTubeArray = mapper->FillTubeArray();
  //----------------------------------------------------  end map

  //-----------------  input TClones Arrays -----------------------------------------

  // Get input array   hit di STT after digi
  fSttHitArray = (TClonesArray *)ioman->GetObject("STTHit");
  if (!fSttHitArray) {
    cout << "-W- PndMixBackgroundEvents::Init: "
         << "No STTHit array, return!" << endl;
    return kERROR;
  }

  //  -------------------------  get the Mvd hit input Array

  fMvdPixelHitArray = (TClonesArray *)ioman->GetObject("MVDHitsPixel");
  if (!fMvdPixelHitArray) {
    std::cout << "-W- PndMixBackgroundEvents::Init: "
              << "No MVD Pixel hitArray, return!" << std::endl;
    return kERROR;
  }

  fMvdStripHitArray = (TClonesArray *)ioman->GetObject("MVDHitsStrip");

  if (!fMvdStripHitArray) {
    std::cout << "-W- PndMixBackgroundEvents::Init: "
              << "No MVD Strip hitArray, return!" << std::endl;
    return kERROR;
  }

  //  Background input Arrays

  //   opend background digi file for Stt background hits.
  filedigirun = new TFile(fSttBkgFilename);
  treedigibkg = (TTree *)filedigirun->Get("pndsim");
  nTotalBkgEvents = (Int_t)treedigibkg->GetEntriesFast();

  // Background STT hits   -----
  treedigibkg->SetBranchAddress("STTHit", &fSttHitBkgArray);
  if (!fSttHitBkgArray) {
    cout << "-W- PndMixBackgroundEvents::Init: "
         << "No STT Background Hit array, return!" << endl;
    return kERROR;
  }

  treedigibkg->SetBranchAddress("MVDHitsPixel", &fMvdPixelHitBkgArray);

  if (!fMvdPixelHitBkgArray) {
    std::cout << "-W- PndMixBackgroundEvents::Init: "
              << "No MVD Pixel Background hitArray, return!" << std::endl;
    return kERROR;
  }

  treedigibkg->SetBranchAddress("MVDHitsStrip", &fMvdStripHitBkgArray);

  if (!fMvdStripHitBkgArray) {
    std::cout << "-W- PndMixBackgroundEvents::Init: "
              << "No MVD Strip Background hitArray, return!" << std::endl;
    return kERROR;
  }
  int nnn = treedigibkg->GetEntriesFast();

  if (nnn != nTotalBkgEvents) {
    cout << "from PndMixBackgroundEvents : total evts in digi file != total evets in reco file, return!" << endl;
    return kERROR;
  }

  //-----------------  Output TClone Arrays -----------------------------------------

  //  ---------------------- new output array of Stt hits + mixed background

  fSttHitandBckgrndArray = new TClonesArray("PndSttHit"); // PndSttHit is the class tipe.
  ioman->Register("STTHitMix", "SttHitandBckgrnd", fSttHitandBckgrndArray, kTRUE);
  //  ---------------------- new output array of Mvd hits + mixed background
  //   Pixels
  fMvdPixelHitandBckgrndArray = new TClonesArray("PndSdsHit"); // PndSdsHit is the class tipe.
  ioman->Register("MVDHitsPixelMix", "MvdPixelHitandBckgrnd", fMvdPixelHitandBckgrndArray, kTRUE);
  //   Strips
  fMvdStripHitandBckgrndArray = new TClonesArray("PndSdsHit"); // PndSdsHit is the class tipe.
  ioman->Register("MVDHitsStripMix", "MvdStripHitandBckgrnd", fMvdStripHitandBckgrndArray, kTRUE);

  return kSUCCESS;
}

// ---------------  end of      InitStatus PndMixBackgroundEvents::Init  --------------------

// CHECK added
void PndMixBackgroundEvents::SetParContainers()
{
  FairRuntimeDb *rtdb = FairRunAna::Instance()->GetRuntimeDb();
  fSttParameters = (PndGeoSttPar *)rtdb->getContainer("PndGeoSttPar");
}

// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
// -----   Public method Exec   --------------------------------------------
void PndMixBackgroundEvents::Exec(Option_t *)
{

  //---------------- declaration of variables
  bool yesno;

  UShort_t nBkgEventsToAdd;

  Int_t i, iaddPix, iaddStrip, iaddStt, ichosen, iPix, iStrip, iStt, j,
    // k, //[R.K. 01/2017] unused variable?
    k1, k2, k3;

  Double_t modified, times[NMAXBCKGRND];

  // PndSttTube * pSttTube; //[R.K. 01/2017] unused variable?

  PndSttHit *pSttHit;

  PndSdsHit *pMvdPixelHit, *pMvdStripHit;

  TVector3 pp, qq;

  TVector3 &pos = pp, dpos = qq;

  //---------- fetching background info -----------------

  //  IVOLTE++;

  BackgroundNandT(&nBkgEventsToAdd, times);

  //----------- end background info -----------------

  //-----------  Now load the output TClones arrays -----------------------

  fSttHitandBckgrndArray->Delete();

  //	physical event Stt hits --
  for (iStt = 0; iStt < fSttHitArray->GetEntriesFast(); iStt++) {
    pSttHit = (PndSttHit *)fSttHitArray->At(iStt);
    PndSttHit *temp = new ((*fSttHitandBckgrndArray)[iStt]) PndSttHit;
    *temp = *pSttHit;
    temp->SetDetectorID(FairRootManager::Instance()->GetBranchId("STTHitMix"));

  } // end of for(  iStt= 0; iStt< fSttHitArray

  //	physics event Mvd Pixel hits --

  fMvdPixelHitandBckgrndArray->Delete();

  for (iPix = 0; iPix < fMvdPixelHitArray->GetEntriesFast(); iPix++) {
    pMvdPixelHit = (PndSdsHit *)fMvdPixelHitArray->At(iPix);

    pp = pMvdPixelHit->GetPosition();
    pMvdPixelHit->PositionError(dpos);
    new ((*fMvdPixelHitandBckgrndArray)[iPix])
      PndSdsHit(FairRootManager::Instance()->GetBranchId("MVDHitsPixelMix"),
                //			FairRootManager::Instance()->GetBranchId("MVDHitsPixel"),
                pMvdPixelHit->GetSensorID(), pos, dpos, pMvdPixelHit->GetClusterIndex(), pMvdPixelHit->GetCharge(), pMvdPixelHit->GetNDigiHits(), pMvdPixelHit->GetRefIndex());

  } // end of for( iPix= 0; iPix< fMvdPixelHitArray

  fMvdStripHitandBckgrndArray->Delete();
  for (iStrip = 0; iStrip < fMvdStripHitArray->GetEntriesFast(); iStrip++) {
    pMvdStripHit = (PndSdsHit *)fMvdStripHitArray->At(iStrip);
    pp = pMvdStripHit->GetPosition();
    pMvdStripHit->PositionError(dpos);
    new ((*fMvdStripHitandBckgrndArray)[iStrip])
      PndSdsHit(FairRootManager::Instance()->GetBranchId("MVDHitsStripMix"),
                //			FairRootManager::Instance()->GetBranchId("MVDHitsStrip"),
                pMvdStripHit->GetSensorID(), pos, dpos, pMvdStripHit->GetClusterIndex(), pMvdStripHit->GetCharge(), pMvdStripHit->GetNDigiHits(), pMvdStripHit->GetRefIndex());

  } // end of for( i= 0; i< fSttHitArray->Ge

  //	background hits --

  iaddStt = 0;
  iaddPix = 0;
  iaddStrip = 0;
  k1 = fSttHitArray->GetEntriesFast();
  k2 = fMvdPixelHitArray->GetEntriesFast();
  k3 = fMvdStripHitArray->GetEntriesFast();

  // nBkgEventsToAdd=1;
  // cout<<"from PndMixBackgroundEvents : in this evt "<<nBkgEventsToAdd<<" evts of bkg are added, with the following times :\n";
  // for(int ipro=0;ipro<nBkgEventsToAdd;ipro++){
  //	cout<<"\tt = "<<times[ipro]<<endl;
  //}

  for (j = 0; j < nBkgEventsToAdd; j++) {

    ichosen = (Int_t)(nTotalBkgEvents * gRandom->Rndm());
    if (ichosen == nTotalBkgEvents)
      ichosen = ichosen - 2;
    // ichosen=0;
    treedigibkg->GetEntry(ichosen);

    //	background Stt hits --
    for (i = 0; i < fSttHitBkgArray->GetEntriesFast(); i++) {
      pSttHit = (PndSttHit *)fSttHitBkgArray->At(i);
      yesno = ModifyIsochrone(pSttHit->GetIsochrone(), times[j], &modified);

      if (yesno) {
        pSttHit->SetRefIndex(-10);       // because this is background hit.
        pSttHit->SetIsochrone(modified); // because this is background hit.
        PndSttHit *temp = new ((*fSttHitandBckgrndArray)[k1 + iaddStt]) PndSttHit;
        *temp = *pSttHit;
        temp->SetDetectorID(FairRootManager::Instance()->GetBranchId("STTHitMix"));
        iaddStt++;
      }
    } // end of  for( i= 0;

    // times[j]=0;
    if (fabs(times[j]) < MVDTYPICALTIME) { // Mvd hits live only 10 nsec.
      //	background Pixel hits --
      for (i = 0; i < fMvdPixelHitBkgArray->GetEntriesFast(); i++) {
        pMvdPixelHit = (PndSdsHit *)fMvdPixelHitBkgArray->At(i);
        pMvdPixelHit->SetRefIndex(-10); // because this is background hit.

        pp = pMvdPixelHit->GetPosition();
        pMvdPixelHit->PositionError(dpos);
        new ((*fMvdPixelHitandBckgrndArray)[k2 + iaddPix])
          PndSdsHit(FairRootManager::Instance()->GetBranchId("MVDHitsPixelMix"),
                    //							"MVDHitsPixel"),
                    pMvdPixelHit->GetSensorID(), pos, dpos, pMvdPixelHit->GetClusterIndex(), pMvdPixelHit->GetCharge(), pMvdPixelHit->GetNDigiHits(), pMvdPixelHit->GetRefIndex());
        iaddPix++;
      } // end of  for( i= 0;
      //	background Strip hits --
      for (i = 0; i < fMvdStripHitBkgArray->GetEntriesFast(); i++) {
        pMvdStripHit = (PndSdsHit *)fMvdStripHitBkgArray->At(i);
        pMvdStripHit->SetRefIndex(-10); // because this is background hit.

        pp = pMvdStripHit->GetPosition();
        pMvdStripHit->PositionError(dpos);
        new ((*fMvdStripHitandBckgrndArray)[k3 + iaddStrip])
          PndSdsHit(FairRootManager::Instance()->GetBranchId("MVDHitsStripMix"),
                    //							"MVDHitsStrip"),
                    pMvdStripHit->GetSensorID(), pos, dpos, pMvdStripHit->GetClusterIndex(), pMvdStripHit->GetCharge(), pMvdStripHit->GetNDigiHits(), pMvdStripHit->GetRefIndex());
        iaddStrip++;

      } // end of  for( i= 0;
    }   //   end of   if( fabs(times[j])

  } // end of  for(j=0;j<nBkgEventsToAdd;j++)

  // ----------------------------------------------------------------------

  return;
}

//---------------------- end of   PndMixBackgroundEvents::Exec

//---------------------- begin of   PndMixBackgroundEvents::BackgroundNandT

void PndMixBackgroundEvents::BackgroundNandT(UShort_t *nBkgEventsToAdd, Double_t *times)
{

  Double_t Trange;

  // negative times
  *nBkgEventsToAdd = 0;
  Trange = gRandom->Exp(1000. / fInteractionRate); //  fInteractionRate is in MHz, dtime is in nsec.
  while (Trange < MAXSTTdriftTIME) {
    times[(*nBkgEventsToAdd)] = -Trange;
    Trange += gRandom->Exp(1000. / fInteractionRate); // fInteractionRate is in MHz.
    (*nBkgEventsToAdd)++;
  }

  // positive times
  Trange = gRandom->Exp(1000. / fInteractionRate); // fInteractionRate is in MHz, dtime is in nsec.
  while (Trange < MAXSTTdriftTIME) {
    times[(*nBkgEventsToAdd)] = Trange;
    Trange += gRandom->Exp(1000. / fInteractionRate); // fInteractionRate is in MHz.
    (*nBkgEventsToAdd)++;
  }

  if (*nBkgEventsToAdd > NMAXBCKGRND)
    *nBkgEventsToAdd = NMAXBCKGRND;
  return;
}

//---------------------- begin of   PndMixBackgroundEvents::ModifyIsochrone

bool PndMixBackgroundEvents::ModifyIsochrone(Double_t isochrone,
                                             Double_t time, // nanosec
                                             Double_t *modified)
{
  //  STTdriftVEL in cm/nsec is  0.0025 cm/nsec;
  //  isochrone here is in reality a drift radius.
  *modified = isochrone + time * STTdriftVEL;
  if (*modified > STRAWRADIUS || *modified < 0.)
    return false;
  else
    return true;
}

//---------------------- end of   PndMixBackgroundEvents::ModifyIsochrone

ClassImp(PndMixBackgroundEvents)
