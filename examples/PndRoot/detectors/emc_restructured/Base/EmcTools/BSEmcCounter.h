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

#ifndef BSEMCCOUNTER_HH
#define BSEMCCOUNTER_HH

#include "TStopwatch.h"

#include "FairLogger.h"
#include "FairRootManager.h"

#include "PndProcess.h"

/**
 * @class BSEmcCounter
 * @brief Simple Process that prints every 100 Events the number of events
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcCounter : public PndProcess {
 public:
  virtual void Process() /*override*/
  {
    if (fEventnumber % 100 == 0) {
      fTimer.Stop();
      Double_t realTime = fTimer.RealTime();
      fTotalTime += realTime;
      if (fEventnumber > 0) {
        LOG(info) << "Event: " << fEventnumber << " - Last 100 Events took " << realTime << " s - expeting to finish in "
                  << (fTotalTime) * ((Double_t)fTotalEventNumber / (Double_t)(fEventnumber)-1);
      }
      fTimer.Reset();
      fTimer.Start();
    }
    ++fEventnumber;
  }
  // virtual void SetupParameters(ParSet* /*parameter*/){};
  virtual void SetupParameters(const PndParameterRegister * /*t_paramRegister*/) /*override*/
  {
    fTotalEventNumber = FairRootManager::Instance()->CheckMaxEventNo();
    fTimer.Start();
  };

 private:
  TStopwatch fTimer{};
  Double_t fTotalTime{0};
  Int_t fEventnumber{0};
  Int_t fTotalEventNumber{1000};
};

#endif /*BSEMCCOUNTER_HH*/
