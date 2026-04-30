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

////////////////////////////////////////////////////////////////////////////
// PndOtSignalOverlap header file
//
// Copied from PndFtsSingalOverlap
//
// authors: Radoslaw Karabowicz, GSI, 2024
////////////////////////////////////////////////////////////////////////////

#ifndef PNDOTSIGNALOVERLAP_H
#define PNDOTSIGNALOVERLAP_H 1

// from ROOT
#include <TObject.h>

class TClonesArray;

class PndOtSignalOverlap : public TObject {

 public:
  /** Default constructor **/
  PndOtSignalOverlap();

  PndOtSignalOverlap(TClonesArray *OriginalHitArray);
  PndOtSignalOverlap(Int_t verbose, TClonesArray *OriginalHitArray);

  /** Destructor **/
  ~PndOtSignalOverlap();

  Bool_t OverlapSimultaneousSignals(TClonesArray *OverlapHitArray);
  Bool_t WriteToOutputHit(Int_t hitid);

 private:
  TClonesArray *fOriginalHitArray;
  TClonesArray *fOverlapHitArray;
  Int_t fVerbose;
  ClassDef(PndOtSignalOverlap, 1);
};

#endif
