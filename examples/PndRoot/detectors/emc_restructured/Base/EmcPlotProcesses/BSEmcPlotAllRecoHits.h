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

#ifndef BSEMCPLOTALLRECOHITS_HH
#define BSEMCPLOTALLRECOHITS_HH

#include "Rtypes.h"
#include "TString.h"

#include "PndContainerI.h"

#include "BSEmcPlotProcess.h"
#include "BSEmcRecoHit.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotAllRecoHits
 * @brief Process to plot all EMC Reco Hits
 * @details Fetches all RecoHit Arrays and plots some values of interest
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotAllRecoHits : public BSEmcPlotProcess {
 public:
  BSEmcPlotAllRecoHits() : BSEmcPlotProcess("BSEmcPlotAllRecoHits") {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

 private:
  void FillPositionPlot(const BSEmcRecoHit *t_recoHit);

  PndContainerI<BSEmcRecoHit> *fFWECRecoHitArray{};
  PndContainerI<BSEmcRecoHit> *fBarrelRecoHitArray{};
  PndContainerI<BSEmcRecoHit> *fBWECRecoHitArray{};
  PndContainerI<BSEmcRecoHit> *fShashlikRecoHitArray{};
  ClassDef(BSEmcPlotAllRecoHits, 1);
};

#endif /*BSEMCPLOTALLRECOHITS_HH*/
