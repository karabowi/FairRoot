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

/** PndMultiFieldPar.h
 ** @author M.Al-Turany
 ** @since 30.01.2007
 ** @version 1.0
 **
 ** Parameter set for the region between Solenoid and dipole. For the runtime database.
 **/

#ifndef PNDMULTIFIELDPAR_H
#define PNDMULTIFIELDPAR_H 1
#include "PndMapPar.h"
#include "PndMultiField.h"

class FairParamList;

class PndMultiFieldPar : public PndMapPar {

 public:
  /** Standard constructor  **/
  PndMultiFieldPar(const char *name, const char *title, const char *context);

  /** default constructor  **/
  PndMultiFieldPar();

  /** Destructor **/
  ~PndMultiFieldPar();

  void putParams(FairParamList *list);

  /** Get parameters **/
  Bool_t getParams(FairParamList *list);

  /** Set parameters from FairField  **/
  void SetParameters(FairField *field);

  TObjArray *GetParArray() { return fParArray; }

 private:
  PndMultiFieldPar(const PndMultiFieldPar &L) : PndMapPar(L), fParArray(L.fParArray){};
  PndMultiFieldPar &operator=(const PndMultiFieldPar &) { return *this; };

 protected:
  TObjArray *fParArray;

  ClassDef(PndMultiFieldPar, 1);
};

#endif
