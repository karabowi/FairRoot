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

#ifndef PNDRICHRECOTASK_H
#define PNDRICHRECOTASK_H 1

#include "FairTask.h"
#include "TClonesArray.h"
#include "PndRichReco.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TF1.h"
#include <vector>
#include "PndRichResolution.h"

class TClonesArray;

class PndRichRecoTask : public FairTask {

 public:
  /** Default constructor **/
  PndRichRecoTask();

  /** Destructor **/
  ~PndRichRecoTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  void SetGeoVersion(UInt_t version) { fGeoVersion = version; }
  void SetParticleID(UInt_t pid) { fParticleID = pid; }
  void SetTrackPosition(TVector3 pos) { fTrackPosition = pos; }
  void SetTrackDirection(TVector3 dir) { fTrackDirection = dir; }
  void SetTrackPositionSecond(TVector3 pos) { fTrackPositionSecond = pos; }
  void SetTrackDirectionSecond(TVector3 dir) { fTrackDirectionSecond = dir; }
  void SetNumberOfEvents(UInt_t events) { fNumberOfEvents = events; }
  TF1 *GetPeakParameters(std::vector<Double_t> v, UInt_t nbins, Double_t xmin, Double_t xmax, Double_t sig);

  void FinishEvent();
  void FinishTask();

 private:
  /** object persistence **/
  Bool_t fPersistence; //!

  UInt_t fNumberOfEvents;
  UInt_t fGeoVersion;
  UInt_t fParticleID;
  UInt_t fEvent;
  PndRichReco *fRichReco;
  TVector3 fTrackPosition;
  TVector3 fTrackDirection;
  TVector3 fTrackPositionSecond;
  TVector3 fTrackDirectionSecond;
  std::vector<UInt_t> vnhits;
  std::vector<Double_t> vth;
  std::vector<Double_t> vph;
  std::vector<Double_t> vdth;
  std::vector<Double_t> vmean;
  std::vector<Double_t> vsigma;

  PndRichResolution *fRichResolution;

  TClonesArray *fRichBarPoint; //! PndRichBarPoint TCA

  ClassDef(PndRichRecoTask, 1);
};

#endif
