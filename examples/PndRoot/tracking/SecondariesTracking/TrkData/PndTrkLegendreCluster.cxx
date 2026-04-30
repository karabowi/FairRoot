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

//
// PndTrkLegendreCluster.cxx
//
//
//
//
// authors: Lia Lavezzi - INFN Pavia (2012)
//
////////////////////////////////////////////////////////////

#include "PndTrkLegendreCluster.h"

using namespace std;

PndTrkLegendreCluster::PndTrkLegendreCluster() : PndTrkCluster(), fTheta(0), fR(0) {}

PndTrkLegendreCluster::~PndTrkLegendreCluster() {}

PndTrkLegendreCluster::PndTrkLegendreCluster(const PndTrkLegendreCluster &cluster) : PndTrkCluster(cluster)
{
  //, fTheta(cluster.fTheta), fR(cluster.fR) {

  //   ((PndTrkCluster&)cluster).fFromPoint = fFromPoint;
  //   ((PndTrkCluster&)cluster).fIRegion  = fIRegion;
  //   ((PndTrkCluster&)cluster).hitlist = hitlist;
  *this = cluster;
}

PndTrkLegendreCluster::PndTrkLegendreCluster(const PndTrkCluster &cluster) : PndTrkCluster(cluster)
{
  if (dynamic_cast<const PndTrkLegendreCluster *>(&cluster)) {
    fTheta = ((PndTrkLegendreCluster)cluster).fTheta;
    fR = ((PndTrkLegendreCluster)cluster).fR;
  } else {
    cout << ":FALSE" << endl;
    fTheta = 14;
    fR = 52;
  }
}

PndTrkLegendreCluster &PndTrkLegendreCluster::operator=(const PndTrkLegendreCluster &cluster)
{
  PndTrkCluster::operator=(cluster);
  fTheta = cluster.fTheta;
  fR = cluster.fR;

  return *this;
}

int PndTrkLegendreCluster::MergeTo(PndTrkLegendreCluster *cluster2)
{

  cout << "THETA MERGE: " << fTheta << " " << cluster2->GetTheta() << endl;
  fTheta = (fTheta + cluster2->GetTheta()) / 2.;
  cout << "THETA MERGED: " << fTheta << endl;

  cout << "R MERGE: " << fR << " " << cluster2->GetR() << endl;
  fR = (fR + cluster2->GetR()) / 2.;
  cout << "R MERGED: " << fR << endl;

  return PndTrkCluster::MergeTo(cluster2);
}

ClassImp(PndTrkLegendreCluster)
