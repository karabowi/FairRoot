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

// --------------------------------------------------------------------------
// -----                     PndSciTpoint header file                  	-----
// -----                    created by A. Sanchez                  	-----
// -----                   modified by D. Steinschaden                 	-----
// -----                   last update    04.2015 		       	-----
// --------------------------------------------------------------------------

#ifndef PNDSCITPOINT_H
#define PNDSCITPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "PndMCPoint.h"
#include "TString.h"

class PndSciTPoint : public PndMCPoint {

 public:
  //** Default constructor **/
  PndSciTPoint();

  PndSciTPoint(Int_t eventID, Int_t trackID, Int_t detectorID, TString detName, TVector3 posin, TVector3 momin, TVector3 posout, TVector3 momout, Double_t tof, Double_t length,
               Double_t eLoss);

  //** Copy constructor **/
  PndSciTPoint(const PndSciTPoint &point) : PndMCPoint(point), fDetName(point.fDetName) { *this = point; };

  //** Destructor **/
  virtual ~PndSciTPoint();

  //** Accessors **/

  TString GetDetName() const { return fDetName; };

  /** Output to screen **/
  virtual void Print(const Option_t *opt) const;

 protected:
  TString fDetName;

  ClassDef(PndSciTPoint, 5)
};

#endif
