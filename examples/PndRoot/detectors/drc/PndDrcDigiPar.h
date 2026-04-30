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
// -----                    PndDrcDigiPar header file                 -----
// -----                  Created 09/11/2010 by M. Patsyuk            -----
// -------------------------------------------------------------------------

/** PndDrcDigiPar
 *@author Maria Patsyuk <m.patsyuk@gsi.de>
 *@since 09.11.2010
 *@version 1.0
 **
 ** PANDA task class for storing digitization parameters for DIRC barrel
 ** Task level RECO
 **/

#ifndef PNDDRCDIGIPAR_H
#define PNDDRCDIGIPAR_H

#include <TVector2.h>
#include <TArrayD.h>
#include <TMath.h>

#include "FairParGenericSet.h"
#include "FairParamList.h"

//! Digitization Parameter Class for DIRC barrel part
class PndDrcDigiPar : public FairParGenericSet {
 public:
  PndDrcDigiPar(const char *name = "PndDrcParTest", const char *title = "PndDrc digi parameter", const char *context = "TestDefaultContext");
  ~PndDrcDigiPar(void);
  void clear(void){};
  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

  void Print();

  /** Accessors **/
  Int_t GetNHitPixels() { return fNOfHitPixels; };
  Int_t GetNAmbiguities() { return fNOfAmbiguities; };
  Int_t GetNPixelParam() { return fNOfParametersPerPixel; };
  // get parameters of a given pixel, array should be the size of fNPixelParam
  Bool_t GetParamsForPixel(Int_t, Double_t *);

 private:
  TArrayD fDrcParameters;
  Int_t fNOfHitPixels;
  Int_t fNOfAmbiguities;
  Int_t fNOfParametersPerPixel;

  ClassDef(PndDrcDigiPar, 2);
};

#endif
