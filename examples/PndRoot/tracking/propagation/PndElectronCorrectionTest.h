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

//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndElectronCorrectionTest header file                 -----
// -----                  Created 05/12/2010 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemDigitise
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 08.12.2009
 *@version 1.0
 **
 ** PANDA task class for finding tracks in CT nad GEM
 ** Task level TRACK
 ** Produces objects of type PndTrack and PndTrackCand
 **/

#ifndef PndElectronCorrectionTest_H
#define PndElectronCorrectionTest_H 1

#include "PndPersistencyTask.h"

#include "FairGeanePro.h"

#include "TClonesArray.h"

#include <map>

class TH1;
class TH2;
class FairMCPoint;

class PndElectronCorrectionTest : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndElectronCorrectionTest();

  /** Constructor with name **/
  PndElectronCorrectionTest(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndElectronCorrectionTest();

  void SetTrackBranchNames(TString branchName) { fTrackArrays[branchName] = nullptr; }
  // Test
  void SetEmcBranchName(TString val) { fEmcDetectorName = val; }

  /** Execution **/
  virtual void Exec(Option_t *opt);

 protected:
  // TVector3 GetPhotonCorrection(int option, FairMCPoint *currentPoint = nullptr);
  Bool_t IsPhotonFromMother(FairMCPoint *point, Int_t motherPdg = 11); // checks if the point comes from a photon and if this comes from the primary electron.
 private:
  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  /** Finish at the end of each event **/
  virtual void Finish();

  TClonesArray *fMcTracks = nullptr;
  std::map<TString, TClonesArray *> fTrackArrays;
  TClonesArray *fEmcDetector = nullptr; //! Photon detector (e.g. IdealTube or EMC).
  TString fEmcDetectorName = "EmcBump";

  TClonesArray *fPhotonStart;
  std::map<TString, TH1 *> fHistos;
  std::map<TString, TH2 *> f2DHistos;

  ClassDef(PndElectronCorrectionTest, 1);
};

#endif
