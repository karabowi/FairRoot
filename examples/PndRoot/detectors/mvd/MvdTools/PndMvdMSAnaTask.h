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
// -----                 PndMvdMSAnaTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------

/** PndMvdMSAnaTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/

#ifndef PndMvdMSAnaTask_H
#define PndMvdMSAnaTask_H

// framework includes
#include "FairTask.h"
#include "TH1.h"
#include "FairGeanePro.h"

#include "PndGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndSdsCluster;

class PndMvdMSAnaTask : public FairTask {
 public:
  /** Default constructor **/
  PndMvdMSAnaTask();

  /** Destructor **/
  ~PndMvdMSAnaTask();
  PndMvdMSAnaTask(const PndMvdMSAnaTask &) = delete;
  PndMvdMSAnaTask &operator=(const PndMvdMSAnaTask &) = delete;

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();

 private:
  TClonesArray *fMCHits;
  TClonesArray *fMCTracks;
  TClonesArray *fTrackParGeane;
  TClonesArray *fTrackParIni;
  TClonesArray *fTrackParFinal;
  TClonesArray *fDetName;

  FairGeanePro *fPro;
  PndGeoHandling *fGeoH;
  int fEventNr;
  bool fUseMVDPoint;

  std::map<int, std::vector<int>> fTrackPixHitIdMap;   // Track -> PixHitId
  std::map<int, std::vector<int>> fTrackStripHitIdMap; // Track -> StripHitId

  void Register();

  void Reset();

  void ProduceHits();

  std::map<int, std::vector<int>> AssignHitsToTracks();

  ClassDef(PndMvdMSAnaTask, 1);
};

#endif
