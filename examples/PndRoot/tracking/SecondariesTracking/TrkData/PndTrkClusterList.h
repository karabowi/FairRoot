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

/** PndTrkClusterList
 ** Class for pattern recognition ClusterList
 ** @author Lia Lavezzi
 **
 ** fSensorID: identifies the specific sensor of the MVD,
               the specific tube of the STT
 **/

#ifndef PNDTRKCLUSTERLIST_H
#define PNDTRKCLUSTERLIST_H 1

#include "TVector3.h"
#include "PndTrkCluster.h"

class TClonesArray;
class PndTrkClusterList : public TObject {

 public:
  PndTrkClusterList();
  // copy ctor
  PndTrkClusterList(const PndTrkClusterList &clist);
  ~PndTrkClusterList();
  PndTrkClusterList &operator=(const PndTrkClusterList &clist);

  void AddCluster(PndTrkCluster *cluster);
  void DeleteCluster(Int_t index);
  void Reset();
  void Clear(Option_t *opt = "");

  inline Int_t GetNofClusters() { return fClusterList.GetEntriesFast(); }

  PndTrkCluster *GetCluster(Int_t index) { return (PndTrkCluster *)fClusterList[index]; }

  /*   void ReplaceCluster(Int_t index, PndTrkCluster *cluster); */

 protected:
  TClonesArray fClusterList;
  // TObjArray clusterlist;
  // std::vector< PndTrkCluster > clusterlist;

  ClassDef(PndTrkClusterList, 1);
};

#endif
