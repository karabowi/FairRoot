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
// -----                    PndPropagationTest header file                 -----
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

#ifndef PndPropagationTest_H
#define PndPropagationTest_H 1

#include "PndPersistencyTask.h"

#include "FairGeanePro.h"

#include "TClonesArray.h"

#include <map>

class TH1;
class TH2;
class FairMCPoint;

class PndPropagationTest : public PndPersistencyTask {

 public:
  /** Default constructor **/
  PndPropagationTest();

  /** Standard constructor **/
  PndPropagationTest(Int_t propParticle, Int_t iVerbose = 0);

  /** Constructor with name **/
  PndPropagationTest(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndPropagationTest();

  void SetDectectorBranchNames(TString branchName) { fDetArrays[branchName] = nullptr; }
  // Test
  void SetPhotonBranchName(TString val) { fPhotonDetectorName = val; }
  void SetPropPdg(int val) { fPropPdg = val; }

  void SetInnerExtrapolationRadius(double val) { fInnerExtrapoRadius = val; }

  /** Execution **/
  virtual void Exec(Option_t *opt);

 protected:
  // TVector3 GetPhotonCorrection(int option, FairMCPoint *currentPoint = nullptr);
  TVector3 GetMCMomentumCorrection();                                  // uses MC truth momentum of photon hit
  TVector3 GetEnergyHitCorrection(TVector3 &innerPos);                 // uses MC truth energy plus direction between photon hit and electron hit
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
  std::map<TString, TClonesArray *> fDetArrays;
  TClonesArray *fPhotonDetector = nullptr;        //! Photon detector (e.g. IdealTube or EMC).
  TString fPhotonDetectorName = "IdealTubePoint"; //! Branch name of detector holding photon information
  TClonesArray *fPhotonStart = nullptr;           //! Output array for the start position of a photon
  FairGeanePro *fGeanePropagator = nullptr;       //! Geane propagator
  int fPropPdg = -211;                            //! pion as default

  int fGeaneNotFitted = 0;
  int fGenfitNotFitted = 0;

  double fInnerExtrapoRadius = -1;

  std::map<TString, TH1 *> fHistos;
  std::map<TString, TH2 *> f2DHistos;

  ClassDef(PndPropagationTest, 1);
};

#endif
