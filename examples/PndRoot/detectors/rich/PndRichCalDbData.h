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
// -----                    PndFieldMapData header file                -----
// -----                V. Friese                 13/02/06             -----
// -------------------------------------------------------------------------

/** PndRichCalDbData.h
 ** @author V.Friese <v.friese@gsi.de>
 ** @since 14.02.2006
 ** @version1.0
 ** Modified by M.Al-Turany for PANDA
 ** This class holds the real data arrays of a magnetic field map,
 ** which are read from / written to file. Nota bene: Field values
 ** are in the same units as they where in the file, in contrast to PndRichCalDb, whcih holds the
 ** field in kG.
 **/

#ifndef PNDRICHCALDBDATA_H
#define PNDRICHCALDBDATA_H

#include "TNamed.h"

class TArrayF;

class PndRichCalDb;

class PndRichCalDbData : public TNamed {

 public:
  /** Default constructor **/
  PndRichCalDbData();

  /** Standard constructor **/
  PndRichCalDbData(const char *name);

  /** Constructor from an existing PndRichCalDb **/
  PndRichCalDbData(const char *name, const PndRichCalDb &cal);

  /** Destructor **/
  virtual ~PndRichCalDbData();

  /** Accessors to field parameters in local coordinate system **/
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
  Int_t GetNp() const { return fNp; }
  Int_t GetNx() const { return fNx; }
  Int_t GetNy() const { return fNy; }
  Int_t GetNt() const { return fNt; }
  Int_t GetNf() const { return fNf; }

  /** Accessors to the field value arrays **/
  TArrayF *GetBetaMean() const { return fBetaMean; }
  TArrayF *GetBetaSig() const { return fBetaSig; }
  TArrayF *GetBetaEff() const { return fBetaEff; }

  Int_t SizeOf();

 private:
  PndRichCalDbData(const PndRichCalDbData &L);
  PndRichCalDbData &operator=(const PndRichCalDbData &) { return *this; };

  /** Type of map. 1 = PndRichCalDb, 2 = Sym2, 3 = Sym3 **/
  Int_t fType;

  /** Field limits in local coordinate system **/
  Double_t fPmin, fPmax;
  Double_t fXmin, fXmax;
  Double_t fYmin, fYmax;
  Double_t fTmin, fTmax;
  Double_t fFmin, fFmax;

  /** Number of grid points  **/
  Int_t fNp, fNx, fNy, fNt, fNf;

  /** Arrays with the field values in T **/
  TArrayF *fBetaMean;
  TArrayF *fBetaSig;
  TArrayF *fBetaEff;

  ClassDef(PndRichCalDbData, 1)
};

#endif
