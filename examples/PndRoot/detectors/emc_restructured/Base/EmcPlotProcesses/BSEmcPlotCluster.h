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

#ifndef BSEMCPLOTCLUSTER_HH
#define BSEMCPLOTCLUSTER_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcPlotProcess.h"

class BSEmcCluster;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotCluster
 * @brief Plots some cluster values such as energy, number of digis, etc
 * @details
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotCluster : public BSEmcPlotProcess {
 public:
  BSEmcPlotCluster() : BSEmcPlotProcess("BSEmcPlotCluster") {}
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

 private:
  Int_t GetMaxDigiIndex(const BSEmcCluster *t_cluster) const;
  Double_t GetMaxTimeDiff(const BSEmcCluster *t_cluster) const;

  Int_t fEvent{0};
  ClassDef(BSEmcPlotCluster, 1);
};

#endif /*BSEMCPLOTCLUSTER_HH*/
