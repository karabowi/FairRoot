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
// -----             PndSdsStripClusterer header file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

/** PndSdsStripClusterer.h
 *@author R.Kliemt <r.kliemt@physik.tu-dresden.de>
 **
 ** Calculator class to find clusters in one channel dimension
 ** for the MVD strip sensors
 **/

#ifndef PNDSDSSTRIPCLUSTERBUILDER_H
#define PNDSDSSTRIPCLUSTERBUILDER_H

#include <vector>
#include <map>
#include <string>

#include "PndSdsDigiStrip.h"
#include "PndSdsClusterStrip.h"
// #include "PndSdsStripCluster.h"

typedef std::map<Int_t, Int_t> Indexpair;
typedef std::map<Int_t, Indexpair> Indextriple;
typedef std::map<SensorSide, Indextriple> SidedTriple;
typedef std::map<Int_t, SidedTriple> Fullmap;

class PndSdsStripClusterer {
 public:
  PndSdsStripClusterer(Int_t DigiType);
  virtual ~PndSdsStripClusterer();

  void Reinit();

  void AddDigi(Int_t sensorID, SensorSide side, Int_t timestamp, Int_t strip, Int_t iDigi);
  void ClearDigis() { fSortedDigis.clear(); }
  virtual std::vector<PndSdsClusterStrip *> SearchClusters() = 0;

  std::vector<PndSdsClusterStrip *> GetClusters() const { return fClusters; }
  PndSdsClusterStrip *GetCluster(Int_t i);
  PndSdsClusterStrip *GetTopCluster(Int_t i);
  PndSdsClusterStrip *GetBotCluster(Int_t i);
  std::vector<Int_t> GetTopClusterIDs() const { return fTopclusters; }
  std::vector<Int_t> GetBotClusterIDs() const { return fBotclusters; }
  std::vector<Int_t> GetLeftDigiIDs() const { return fLeftDigis; }

  void SetDigiType(Int_t digiType) { fDigiType = digiType; }

 protected:
  void AddCluster(const std::vector<Int_t> &onecluster, SensorSide side);
  Fullmap fSortedDigis;
  std::vector<Int_t> fTopclusters; // contains index to fClusterArray
  std::vector<Int_t> fBotclusters; // contains index to fClusterArray
  std::vector<Int_t> fLeftDigis;   // contains index to the not assigned digis
  std::vector<PndSdsClusterStrip *> fClusters;

  Int_t fDigiType;

  ClassDef(PndSdsStripClusterer, 3);
};

#endif // PndSdsStripClusterer
