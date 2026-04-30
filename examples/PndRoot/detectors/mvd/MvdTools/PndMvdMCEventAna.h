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

//
// C++ Interface: PndMvdMCEventAna
//
// Description:
//
//
// Author: t.stockmanns <stockman@ikp455>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef PNDMVDMCEVENTANA_H
#define PNDMVDMCEVENTANA_H

#include "PndMvdEventAna.h"
#include "PndSdsMCPoint.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TClonesArray.h"

/**
@author t.stockmanns
*/
class PndMvdMCEventAna : public PndMvdEventAna {
 public:
  PndMvdMCEventAna(){};
  PndMvdMCEventAna(TString fileName);
  virtual ~PndMvdMCEventAna(){};

  virtual void InitBranch();
  virtual void InitHistos();
  virtual void AnaHitNr(Int_t hitNr);

 private:
  TClonesArray *fMCTrackArray;
  PndGeoHandling *fGeoH;

  ClassDef(PndMvdMCEventAna, 1);
};

#endif
