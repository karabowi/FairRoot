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

#ifndef PNDMIXBACKGROUNDEVENTS_H
#define PNDMIXBACKGROUNDEVENTS_H 1

#include "PndMCTrack.h"
#include "PndSttTrack.h"
#include "FairTask.h"
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairTrackParP.h"
#include "PndGeoSttPar.h"

#include "TFile.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TRandom.h"

class TClonesArray;
class TObjectArray;

class PndMixBackgroundEvents : public FairTask {

 public:
  /** Default constructor **/
  PndMixBackgroundEvents();

  PndMixBackgroundEvents(Int_t verbose);

  /** Destructor **/
  ~PndMixBackgroundEvents();

  Double_t fInteractionRate; //  in MHz, average interaction rate in PANDA.

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void WriteHistograms();

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  void SetParContainers();

  void SetInputBkgFilesName(char *string1)
  {
    sprintf(fSttBkgFilename, "%s", string1);
    return;
  };

  void SetInteractionRate(Double_t rate)
  {
    fInteractionRate = rate;
    return;
  };

 private:
  //  Int_t IVOLTE,
  //        istampa;

  char fSttBkgFilename[1000], fMvdBkgFilename[1000];

  static const UShort_t NMAXBCKGRND = 1000;
  //  static const UShort_t   NMAXBCKGRND ;

  static const Double_t MVDTYPICALTIME, // in nsec; time after which the Mvd hit disappears.
    RATE,                               //  in MHz, average interaction rate in PANDA.
    STRAWRADIUS,                        // in cm.
    STTdriftVEL,                        //   in cm/nsec
    MAXSTTdriftTIME;                    //   in nsec

  Int_t nTotalBkgEvents;

  TFile *filedigirun;

  TFile *filerecorun;

  TTree *treedigibkg; // pointer to root Tree of digi background file.

  TTree *treerecobkg; // pointer to root Tree of reco background file, only for Mvd hits.

  /** Input array of PndSttTube (map of STT tubes) **/
  TClonesArray *fMCTrackArray;
  TClonesArray *fSttTubeArray;

  /** Input array of PndSttHit **/
  TClonesArray *fSttHitArray;

  /** Input array of MvdPixelHitArray **/
  TClonesArray *fMvdPixelHitArray;

  /** Input array of MvdStripHitArray **/
  TClonesArray *fMvdStripHitArray;

  //---------------------  input Arrays for the Background hits  -------------

  /** Input array of PndSttHit **/
  TClonesArray *fSttHitBkgArray;

  /** Input array of MvdPixelHitArray **/
  TClonesArray *fMvdPixelHitBkgArray;

  /** Input array of MvdStripHitArray **/
  TClonesArray *fMvdStripHitBkgArray;

  //-----------------

  /** Output array of Stt Hits + Background  **/
  TClonesArray *fSttHitandBckgrndArray;

  /** Output array of Mvd Pixel Hit + Background **/
  TClonesArray *fMvdPixelHitandBckgrndArray;

  /** Output array of Mvd Strip Hit + Background **/
  TClonesArray *fMvdStripHitandBckgrndArray;

  //---------------------------------------------------------

  /** object persistence **/
  Bool_t fPersistence; //!

  PndGeoSttPar *fSttParameters; //  CHECK added

  void Initialization_ClassVariables();

  void BackgroundNandT(UShort_t *nBkgEventsToAdd, Double_t *times);

  bool ModifyIsochrone(Double_t isochrone,
                       Double_t time, // nanosec
                       Double_t *modified);

  ClassDef(PndMixBackgroundEvents, 1);
};

#endif
