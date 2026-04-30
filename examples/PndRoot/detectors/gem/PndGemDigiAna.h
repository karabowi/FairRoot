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
// -----                   PndGemDigiAna header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGemDigiAna
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief analyze GEM digis
 *
 *  Compare the found digis.
 *  First version compares only the time between digis.
 *      For each strip the time of last digi on that strip is stored: DLTS
 *      By default the DLTS is set to -1. If strip is fired and the DLTS is different from -1,
 *      the histogram with time between digis is filled.
 *      Moreover, at any given time the DLTS will show for how long was the strip inactive.
 **/

#ifndef PNDGEMDIGIANA_H
#define PNDGEMDIGIANA_H

#include "TH1F.h"
#include "TH2F.h"

#include "FairTask.h"

#include "PndGemDigi.h"
#include "PndGemDigiPar.h"

#include <vector>

class PndGemDigiAna : public FairTask {

 public:
  /** Default constructor **/
  PndGemDigiAna();

  PndGemDigiAna(Int_t iVerbose);
  PndGemDigiAna(TString taskName, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemDigiAna();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t &verbose) { fVerbose = verbose; };

 private:
  PndGemDigiPar *fDigiPar;

  TClonesArray *fGemDigiArray;

  /** Event counter **/
  Int_t fNofEvents; ///< event counter

  TList *fHistoList;
  // histograms to store last digi's time stamp on all strips
  // histograms showing times between digis on all strips
  TClonesArray *fHistoArray;

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

  ClassDef(PndGemDigiAna, 1);
};

#endif
