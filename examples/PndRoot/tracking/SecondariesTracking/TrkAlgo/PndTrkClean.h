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

/** PndTrkClean
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKCLEAN_H
#define PNDTRKCLEAN_H 1

#include "PndGeoHandling.h"
#include "PndTrkCluster.h"
#include "PndTrkClusterList.h"
#include "PndTrkHit.h"
#include "TClonesArray.h"

class PndTrkClean : public TObject {
 public:
  PndTrkClean();
  PndTrkClean(TClonesArray *tubearray);
  ~PndTrkClean();

  int FindMvdLayer(int sensorID);
  PndTrkClusterList Cleanup2(PndTrkCluster *cluster);
  Bool_t CheckPairOfHits(PndTrkHit *hit1, PndTrkHit *hit2);
  int CheckSectorDistribution(PndTrkCluster *cluster);
  PndTrkCluster CleanSectors(PndTrkCluster *cluster, int sector);
  PndTrkClusterList MergeClusters(PndTrkClusterList *clusterlist);
  PndTrkClusterList Split(PndTrkCluster *cluster, std::vector<int> breakpoints);
  Bool_t SplitAtHit(PndTrkCluster *hitlist, PndTrkHit *athit, PndTrkCluster &cluster1, PndTrkCluster &cluster2);

 protected:
  PndGeoHandling *fGeoH;
  TClonesArray *fTubeArray;
  ClassDef(PndTrkClean, 1);
};

#endif
