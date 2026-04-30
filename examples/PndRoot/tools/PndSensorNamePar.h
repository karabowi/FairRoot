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
// C++ Interface: PndSensorNamePar
//
#ifndef PNDSENSORNAMEPAR_H
#define PNDSENSORNAMEPAR_H

#include "FairParGenericSet.h"
#include "FairParamList.h"
#include "FairRun.h"

#include "TObjArray.h"
#include "TObjString.h"

#include <iostream>
#include <map>

//! Unique match between SensorID and path in TGeoManager
class PndSensorNamePar : public FairParGenericSet {
 public:
  PndSensorNamePar(const char *name = "PndSensorNamePar", const char *title = "Match between GeoManager path and SensorId", const char *context = "TestDefaultContext");
  ~PndSensorNamePar(void);

  void clear(void){};
  void putParams(FairParamList *list);
  Bool_t getParams(FairParamList *list);

  virtual void print() { Print(); }
  void Print();

  Int_t AddSensorName(TObjString *name);

  Int_t SensorInList(TObjString *name);

  void FillMap();

  TString GetSensorName(Int_t index);

  TObjArray *GetSensorNames() const { return fSensorNames; }

  std::vector<TString> GetSensorNamesWithString(TString identifier);

  PndSensorNamePar(const PndSensorNamePar &L);
  PndSensorNamePar &operator=(const PndSensorNamePar &L);

  friend std::ostream &operator<<(std::ostream &output, const PndSensorNamePar &par);
  Int_t AddSensorNameAndId(TObjString *name, Int_t id);

 private:
  TObjArray *fSensorNames;
  std::map<TString, Int_t> fMapOfSensorNames;   //!
  std::map<Int_t, TString> fMapOfSensorIndizes; //!

  ClassDef(PndSensorNamePar, 1);
};

#endif /*!MVDSTRIPDIGIPAR_H*/
