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
// -----                    PndGemMatchHits header file                -----
// -----                  Created 24/06/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemMatchHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 24.06.2009
 *@version 1.0
 **
 ** PANDA task class for matching PndGemHits and PndGemMCPoints
 ** Task level reco
 ** Fills fRefIndex of the GEMHits
 **/

#ifndef PNDGEMMATCHHITS_H
#define PNDGEMMATCHHITS_H 1

#include "FairTask.h"

#include "TStopwatch.h"

class TClonesArray;
class PndGemDigiPar;

class PndGemMatchHits : public FairTask {

 public:
  /** Default constructor **/
  PndGemMatchHits();

  /** Standard constructor **/
  PndGemMatchHits(Int_t iVerbose);

  /** Constructor with name **/
  PndGemMatchHits(const char *name, Int_t iVerbose);

  /** Destructor **/
  virtual ~PndGemMatchHits();

  /** Execution **/
  virtual void Exec(Option_t *opt);

  /** Finish **/
  virtual void Finish();

 private:
  PndGemDigiPar *fDigiPar;
  TClonesArray *fPoints; /** Input array of PndGemPoint **/
  TClonesArray *fHits;   /** Input array of PndGemHit **/

  Int_t fNHits;
  Int_t fNMatchedHits;
  Int_t fNFakeHits;
  Int_t fNMultiHits;
  ;

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** Intialisation **/
  virtual InitStatus Init();

  /** Reinitialisation **/
  virtual InitStatus ReInit();

  /** Reset eventwise counters **/
  void Reset();

  ClassDef(PndGemMatchHits, 1);
};

#endif
