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

/** PndFieldPar.h
 **
 ** Parameter set for the PND magnetic field. For the runtime database.
 **/

#ifndef PNDRICHCALDBPAR_H
#define PNDRICHCALDBPAR_H 1

#include "FairField.h"
#include "FairParGenericSet.h"

class FairParamList;

class PndRichCalDbPar : public FairParGenericSet {
 public:
  /** Standard constructor  **/
  PndRichCalDbPar(const char *name, const char *title, const char *context);

  /** default constructor  **/
  PndRichCalDbPar();

  /** Destructor **/
  ~PndRichCalDbPar();

  /** Put parameters **/
  virtual void putParams(FairParamList *list);

  /** Get parameters **/
  virtual Bool_t getParams(FairParamList *list);

  /** Set parameters from FairField  **/
  void SetParameters(FairField *field);

  /** Accessors **/
  Int_t GetType() const { return fType; }
  Double_t GetPmin() const { return fPmin; }
  Double_t GetXmin() const { return fXmin; }
  Double_t GetYmin() const { return fYmin; }
  Double_t GetTmin() const { return fTmin; }
  Double_t GetFmin() const { return fFmin; }
  Double_t GetPmax() const { return fPmax; }
  Double_t GetXmax() const { return fXmax; }
  Double_t GetYmax() const { return fYmax; }
  Double_t GetTmax() const { return fTmax; }
  Double_t GetFmax() const { return fFmax; }
  Double_t GetBetaMean() const { return fBetaMean; }
  Double_t GetBetaSig() const { return fBetaSig; }
  Double_t GetBetaEff() const { return fBetaEff; }
  void MapName(TString &name) { name = fMapName; }

 private:
  /** Field type
   ** 0 = constant field
   ** 1 = field map
   ** 2 = field map sym2 (symmetries in x and y)
   ** 3 = field map sym3 (symmetries in x, y and z)
   **/
  Int_t fType;

  /** Field limits in case of constant field **/
  Double_t fPmin, fPmax;
  Double_t fXmin, fXmax;
  Double_t fYmin, fYmax;
  Double_t fTmin, fTmax;
  Double_t fFmin, fFmax;

  /** Field values in case of constant field [kG] **/
  Double_t fBetaMean, fBetaSig, fBetaEff;

  /** Field map name in case of field map **/
  TString fMapName;

  ClassDef(PndRichCalDbPar, 1);
};

#endif
