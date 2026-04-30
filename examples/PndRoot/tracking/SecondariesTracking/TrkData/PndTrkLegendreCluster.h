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

/** PndTrkLegendreCluster
 **
 ** @author Lia Lavezzi
 **/

#ifndef PNDTRKLEGENDRECLUSTER_H
#define PNDTRKLEGENDRECLUSTER_H 1

#include "PndTrkCluster.h"

class PndTrkLegendreCluster : public PndTrkCluster {

 public:
  PndTrkLegendreCluster();
  virtual ~PndTrkLegendreCluster();
  PndTrkLegendreCluster(const PndTrkLegendreCluster &cluster);
  PndTrkLegendreCluster(const PndTrkCluster &cluster);
  PndTrkLegendreCluster &operator=(const PndTrkLegendreCluster &cluster);

  void SetR(double r) { fR = r; }
  void SetTheta(double theta) { fTheta = theta; }

  Double_t GetTheta() { return fTheta; }
  Double_t GetR() { return fR; }
  int MergeTo(PndTrkLegendreCluster *cluster2);

 protected:
  double fTheta;
  double fR;

  ClassDef(PndTrkLegendreCluster, 1);
};

#endif
