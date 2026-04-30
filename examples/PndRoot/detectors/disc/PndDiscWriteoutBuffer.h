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

//-------------------------------------------------------------------------
// Author:      Oliver Merle (Oliver.Merle@exp2.physik.uni-giessen.de)
// Changes:     Mustafa Schmidt (Mustafa.A.Schmidt@physik.uni-giessen.de)
// Date:        30.11.2015
// Description: Customization of FairRoot Write out buffer
//              for time-based simulations
//-------------------------------------------------------------------------

#ifndef DISCDIRCWRITEOUTBUFFER_H
#define DISCDIRCWRITEOUTBUFFER_H

// ---------------------------
// vanilla PROOT
// ---------------------------
//#define RRTTI
#define FWB_USE_INHERITANCE
// ---------------------------

#include "FairWriteoutBuffer.h"
#include "PndDiscDigitizedHit.h"
#include <iostream>

class PndDiscWriteoutBuffer : public FairWriteoutBuffer {
 public:
#ifdef RRTTI
  /// Functor to sort the pointers in std::map according to the target objects less function:
  struct CompareDigits {
    bool operator()(const FairTimeStamp *const &p1, const FairTimeStamp *const &p2) const { return (*p1) < (*p2); }
  };
#endif

 public:
  PndDiscWriteoutBuffer();
  PndDiscWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  virtual ~PndDiscWriteoutBuffer();

  // the following should become a functor:
  virtual std::vector<std::pair<double, FairTimeStamp *>> Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData);

#ifdef FWB_USE_INHERITANCE
  // Interface due to lack of templates, will be replaced by
  // RTTI based implementations in the base class:
  virtual void AddNewDataToTClonesArray(FairTimeStamp *);
  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);
#endif

 protected:
#ifndef RRTTI
  std::map<PndDiscDigitizedHit, double> fData_map;
#else
  typedef std::map<FairTimeStamp *, double, CompareDigits> DataMap_t;
  DataMap_t fData_map;
  TBuffer *buffer;
#endif

  ClassDef(PndDiscWriteoutBuffer, 1);
};

#endif // DISCDIRCWRITEOUTBUFFER_H
