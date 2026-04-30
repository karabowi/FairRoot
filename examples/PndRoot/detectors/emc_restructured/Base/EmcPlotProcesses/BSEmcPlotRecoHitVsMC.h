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

#ifndef BSEMCPLOTRECOHITVSMC_HH
#define BSEMCPLOTRECOHITVSMC_HH
#include <memory>

#include "Rtypes.h"
#include "TString.h"
#include "TVector3.h"

#include "BSEmcPlotProcess.h"

class BSEmcRecoHit;
class PndMCTrack;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotRecoHitVsMC
 * @brief Process creating plots to test Theta and Phi reconstruction of RecoHits vs MCTruth value
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotRecoHitVsMC : public BSEmcPlotProcess {
 public:
  BSEmcPlotRecoHitVsMC() : BSEmcPlotProcess("BSEmcPlotRecoHitVsMC"){};
  virtual ~BSEmcPlotRecoHitVsMC(){};
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;
  virtual void Process() /*override*/;

 private:
  std::unique_ptr<PndMCTrack> GetMostPrimaryMCTrack(const BSEmcRecoHit *t_recohit) const;

  ClassDef(BSEmcPlotRecoHitVsMC, 1);
};
#endif /*BSEMCPLOTRECOHITVSMC_HH*/
