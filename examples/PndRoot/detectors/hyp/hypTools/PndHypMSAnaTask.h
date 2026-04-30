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
// -----                 PndHypMSAnaTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------

/** PndHypMSAnaTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/

#ifndef PndHypMSAnaTask_H
#define PndHypMSAnaTask_H

// framework includes
#include "FairTask.h"
#include "TH1.h"
#include "GFTrackCand.h"
#include "FairGeanePro.h"

#include "PndHypGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndHypCluster;

class PndHypMSAnaTask : public FairTask {
 public:
  /** Default constructor **/
  PndHypMSAnaTask();

  /** Destructor **/
  ~PndHypMSAnaTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  virtual void Finish();
  void WriteHistograms();

 private:
  TClonesArray *fMCHits;
  TClonesArray *fMCTracks;

  TClonesArray *fTrackParGeane;
  TClonesArray *fTrackParIni;
  TClonesArray *fTrackParFinal;

  FairGeanePro *fPro;
  PndHypGeoHandling *fGeoH;

  TH1F *histo;
  int fEventNr;
  std::map<int, std::vector<int>> mcHitMap;
  std::map<int, std::vector<int>> fTrackPixHitIdMap;   // Track -> PixHitId
  std::map<int, std::vector<int>> fTrackStripHitIdMap; // Track -> StripHitId

  void Register();

  void Reset();

  void ProduceHits();

  std::map<int, std::vector<int>> AssignHitsToTracks();

  ClassDef(PndHypMSAnaTask, 1);
};

#endif
