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

#ifndef BSEMCPLOTBARRELFWENDCAPOVERLAP_HH
#define BSEMCPLOTBARRELFWENDCAPOVERLAP_HH

#include <vector>

#include "Rtypes.h"
#include "TString.h"
#include "TVector3.h"

#include "PndContainerI.h"

#include "BSEmcPlotProcess.h"
#include "BSEmcRecoHit.h"

class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotBarrelFwEndcapOverlap
 * @brief Plots some values to investigate the overlap of the Barrel and the FwEndcap
 * @details Fetches the FwEndcap and Barrel RecoHit Arrays and plots some values
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotBarrelFwEndcapOverlap : public BSEmcPlotProcess {
 public:
  BSEmcPlotBarrelFwEndcapOverlap() : BSEmcPlotProcess("BSEmcPlotBarrelFwEndcapOverlap") {}

  virtual ~BSEmcPlotBarrelFwEndcapOverlap() {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

 private:
  PndContainerI<BSEmcRecoHit> *fFWECRecoHitArray{nullptr};
  PndContainerI<BSEmcRecoHit> *fBarrelRecoHitArray{nullptr};

  ClassDef(BSEmcPlotBarrelFwEndcapOverlap, 0);
};

#endif /*BSEMCPLOTBARRELFWENDCAPOVERLAP_HH*/
