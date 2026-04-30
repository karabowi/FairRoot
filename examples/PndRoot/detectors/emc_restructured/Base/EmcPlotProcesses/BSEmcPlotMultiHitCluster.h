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

#ifndef BSEMCPLOTMULTIHITCLUSTER_HH
#define BSEMCPLOTMULTIHITCLUSTER_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "BSEmcPlotProcess.h"

class BSEmc2DMapper;
class BSEmcCluster;
class BSEmcRecoHit;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcPlotMultiHitCluster
 * @brief Plots events where a cluster contains several maxima
 * @details Uses a Emc-Detector specific BSEmc2DMapper to create 2D map plots showing digis and their energy
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcPlotMultiHitCluster : public BSEmcPlotProcess {
 public:
  BSEmcPlotMultiHitCluster(BSEmc2DMapper *t_mapper = nullptr);
  virtual ~BSEmcPlotMultiHitCluster();
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void Process() /*override*/;
  virtual void InitHistos(const TString &t_folderprefix) /*override*/;

  void SetMapper(BSEmc2DMapper *t_mapper) { fMapper = t_mapper; }

 private:
  void Create2DHistOfClusterDigis(const BSEmcCluster *t_cluster);
  std::vector<const BSEmcRecoHit *> GetRecoHitsInCluster(Int_t t_clusterindex);
  void FillMaxNeighbourEnergyOverCentralEnergy(Int_t t_clusterindex);

  BSEmc2DMapper *fMapper{nullptr};
  Int_t fEvent{0};
  ClassDef(BSEmcPlotMultiHitCluster, 1);
};

#endif /*BSEMCPLOTMULTIHITCLUSTER_HH*/
