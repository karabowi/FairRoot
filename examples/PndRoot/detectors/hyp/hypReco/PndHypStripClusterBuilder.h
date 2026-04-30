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

// -------------------------------------------------------------------------
// -----             PndMvdStripClusterBuilder header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

/** PndMvdStripClusterBuilder.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** Calculator class to find clusters in one channel dimension
 ** for the HYP strip sensors
 **/

#ifndef PNDHYPSTRIPCLUSTERBUILDER_H
#define PNDHYPSTRIPCLUSTERBUILDER_H

#include <vector>
#include <map>
#include <string>

#include "PndHypDigiStrip.h"
#include "PndHypCluster.h"
//#include "PndHypStripCluster.h"

typedef std::map<Int_t, Int_t> Indexpair;
typedef std::map<std::string, std::map<SensorSide, Indexpair>> Fullmap;

class PndHypStripClusterBuilder {
 public:
  PndHypStripClusterBuilder();
  virtual ~PndHypStripClusterBuilder();

  ///--------------------------------------------------------------------------///

  void Reinit();
  void AddDigi(std::string detname, SensorSide side, Int_t strip, Int_t iPoint);

  std::vector<PndHypCluster> SearchClusters();
  std::vector<PndHypCluster> GetClusters() { return fClusters; }
  PndHypCluster GetCluster(Int_t i);
  PndHypCluster GetTopCluster(Int_t i);
  PndHypCluster GetBotCluster(Int_t i);
  // getter
  std::vector<Int_t> GetTopClusterIDs() const { return fTopclusters; }
  std::vector<Int_t> GetBotClusterIDs() const { return fBotclusters; }

  ///--------------------------------------------------------------------------///

 private:
  ///--------------------------------------------------------------------------///
  void AddCluster(const std::vector<Int_t> &onecluster, SensorSide side);
  Fullmap fSortedDigis;
  std::vector<Int_t> fTopclusters; // contains index to fClusterArray
  std::vector<Int_t> fBotclusters; // contains index to fClusterArray
  std::vector<PndHypCluster> fClusters;

  ///--------------------------------------------------------------------------///

  ClassDef(PndHypStripClusterBuilder, 3);
};

#endif // PndHypStripClusterBuilder
