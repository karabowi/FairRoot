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
// -----                   PndGemFindHitsAna header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGemFindHitsAna
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief analyze found GEM hits
 *
 *  Compare the found hits in all stations.
 *  Draw the ( position back - position front ). On each station two hits
 *  are registered, the difference in Z position of drift volumes are about 3 cm.
 *  Thus the difference in hits X,Y position should be rather well defined.
 *  From this it might be possible to align planes in each station.
 **/

#ifndef PNDGEMFINDHITSANA_H
#define PNDGEMFINDHITSANA_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairTask.h"

#include "PndGemHit.h"
#include "PndGemDigiPar.h"

#include <vector>

class PndGemFindHitsAna : public FairTask {

 public:
  /** Default constructor **/
  PndGemFindHitsAna();

  PndGemFindHitsAna(Int_t iVerbose);
  PndGemFindHitsAna(TString taskName, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemFindHitsAna();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t &verbose) { fVerbose = verbose; };

 private:
  PndGemDigiPar *fDigiPar;

  TClonesArray *fGemHitArray;

  /** Event counter **/
  Int_t fNofEvents; ///< event counter

  Double_t fGridSize;

  std::vector<Int_t> fStatBegHist;
  std::vector<Int_t> fGridHalfLen;

  TList *fHistoList;
  // histograms to store position difference of front vs back hits
  // all histograms will be stored in TClonesArray
  TClonesArray *fhFrontBackDiff;

  void CreateHistos();
  void AnaHistos();

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  ClassDef(PndGemFindHitsAna, 1);
};

#endif
