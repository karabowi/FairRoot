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
// -----                    PndSttFitTracks header file                -----
// -----                  Created 18/02/05  by V. Friese               -----
// -------------------------------------------------------------------------

/** PndSttFitTracks
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for track fitting in the STT.
 ** Input: TClonesArray of PndSttTrack
 ** Parameters of these objects are updated
 **
 ** Uses as track fitting algorithm classes derived from PndSttTrackFitter.
 **/

#ifndef PNDSTTFITTRACKS
#define PNDSTTFITTRACKS 1

#include "FairTask.h"
#include "PndGeoSttPar.h"

#include <string>
#include <vector>

class PndSttTrackFitter;
class TClonesArray;

class PndSttFitTracks : public FairTask {

 public:
  /** Default constructor **/
  PndSttFitTracks();

  /** Standard constructor
   **
   *@param name   Name of class
   *@param title  Task title
   *@param fitter Pointer to STT track fitter concrete class
   **/
  PndSttFitTracks(const char *name, const char *title = "FairTask", PndSttTrackFitter *fitter = nullptr);

  /** Destructor **/
  virtual ~PndSttFitTracks();

  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();

  /** Task execution **/
  virtual void Exec(Option_t *opt);

  /** Finish at the end of each event **/
  virtual void Finish();

  /** Accessors **/
  PndSttTrackFitter *GetFitter() { return fFitter; };
  Int_t GetNofTracks() { return fNofTracks; };

  /** Set concrete track finder **/
  void UseFitter(PndSttTrackFitter *fitter) { fFitter = fitter; };

  void AddHitCollectionName(char *hitCollectionName);

 private:
  void AddAllCollections();
  void AddHitCollection(char const *collectionName);

  void SetParContainers();

  PndSttTrackFitter *fFitter;    // Pointer to TrackFinder concrete class
  TClonesArray *fTrackCandArray; // Input array of STT track candidates
  TClonesArray *fTrackArray;     // Output array of STT tracks
  TClonesArray *fHitArray;
  Int_t fNofTracks; // Number of tracks successfully fitted
  std::vector<std::string> fHitCollectionNames;
  Bool_t fCollectionsComplete;

  /** from parameters array of PndSttTube **/ //  CHECK added
  TClonesArray *fTubeArray;

  PndGeoSttPar *fSttParameters; //  CHECK added

  ClassDef(PndSttFitTracks, 1);
};

#endif
