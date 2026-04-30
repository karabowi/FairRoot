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
// -----                     PndGemEventBuilderOnTracks header file                -----
// -----                  Created 22/08/2013 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemEventBuilderOnTracks
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 22/08/2013
 *@version 1.0
 **
 ** PANDA task class for event building basing on reconstructed GEM tracks.
 ** Task level RECO
 **/

#ifndef PNDGEMEVENTBUILDERONTRACKS_H
#define PNDGEMEVENTBUILDERONTRACKS_H 1

#include "PndTrack.h"

#include "FairEventBuilder.h"

#include "TStopwatch.h"

#include <map>
#include <set>
#include <vector>

class TClonesArray;

class FairRecoEventHeader;

struct RecoEvent {
  std::vector<Int_t> trackIndex;
  Int_t nofTracks;
  Double_t meanTime;
};

class PndGemEventBuilderOnTracks : public FairEventBuilder {

 public:
  /** Default constructor **/
  PndGemEventBuilderOnTracks();

  /** Constructor with task name **/
  PndGemEventBuilderOnTracks(const char *name, Int_t iVerbose = 0);

  /** Destructor **/
  virtual ~PndGemEventBuilderOnTracks();

  virtual void StoreEventData(FairRecoEventHeader *recoEvent);

  std::vector<std::pair<double, FairRecoEventHeader *>> FindEvents();

  std::vector<std::pair<double, PndTrack *>> Modify(std::pair<double, PndTrack *> oldData, std::pair<double, PndTrack *> newData);
  void AddNewDataToTClonesArray(FairTimeStamp *data);
  double FindTimeForData(FairTimeStamp *data);
  void FillDataMap(FairTimeStamp *data, double activeTime);
  void EraseDataFromDataMap(FairTimeStamp *data);

 private:
  TClonesArray *fGemTracks;
  TClonesArray *fGemOutTracks;

  Int_t fTNofEvents;
  Int_t fTNofTracks;
  Int_t fTNofRecoEvents;

  Double_t fGemTrackDelay;
  TStopwatch fTimer;
  Double_t fExecTime;
  std::vector<RecoEvent> fRecoEvents;

  std::map<FairRecoEventHeader, double> fEvent_map;
  std::map<PndTrack, double> fData_map;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual Bool_t Init();

  /** Reinitialisation **/
  virtual Bool_t ReInit();

  virtual void Print();

  /** Finish at the end of each event **/
  virtual void Finish();

  Int_t CompareTrackToPreviousEvents(Int_t trackId, PndTrack *tempTrack);

  ClassDef(PndGemEventBuilderOnTracks, 1);
};

#endif
