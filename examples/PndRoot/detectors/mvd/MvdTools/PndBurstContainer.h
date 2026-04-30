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

#ifndef PNDBURSTCONTAINER_HH
#define PNDBURSTCONTAINER_HH

#include "FairTimeStamp.h"
#include "FairMQLogger.h"

#include <vector>

class PndBurstContainer {
 public:
  PndBurstContainer() : fThreshold(2E3), fOffset(2E3), fCurrentThreshold(fThreshold), fCurrentOffset(fOffset + fThreshold) { fContainer.resize(3); }

  std::vector<std::vector<FairTimeStamp *>> ProcessData(std::vector<FairTimeStamp *> data);
  std::vector<std::vector<FairTimeStamp *>> GetLastData() { return fContainer; }

  virtual void SetThreshold(double val) { fThreshold = val; }
  virtual void SetOffset(double val) { fOffset = val; }

  virtual double GetThreshold() { return fThreshold; }
  virtual double GetOffset() { return fOffset; }

 protected:
  bool IsAboveOffset(FairTimeStamp *val) { return val->GetTimeStamp() > fCurrentOffset; }
  int CalcContainerPos(FairTimeStamp *val)
  {
    int result;
    double resultD = ((val->GetTimeStamp() - fCurrentThreshold) / fThreshold);
    result = resultD;
    if (resultD > 0)
      result++;
    return result;
  }

 private:
  std::vector<std::vector<FairTimeStamp *>> fContainer;
  double fThreshold;
  double fCurrentThreshold;
  double fOffset;
  double fCurrentOffset;
};

#endif
