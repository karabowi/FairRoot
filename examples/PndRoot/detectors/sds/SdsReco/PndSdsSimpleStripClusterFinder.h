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

#ifndef PNDSDSSIMPLESTRIPCLUSTERFINDER_H
#define PNDSDSSIMPLESTRIPCLUSTERFINDER_H

#include <vector>
#include <map>
#include <string>

#include "PndSdsDigiStrip.h"
#include "PndSdsCluster.h"
#include "PndSdsStripClusterer.h"
// #include "PndSdsStripCluster.h"

class PndSdsSimpleStripClusterFinder : public PndSdsStripClusterer {
 public:
  PndSdsSimpleStripClusterFinder(Int_t DigiType);
  PndSdsSimpleStripClusterFinder(Int_t DigiType, Int_t rad);
  ~PndSdsSimpleStripClusterFinder();

  std::vector<PndSdsClusterStrip *> SearchClusters();
  void SetSearchRadius(Int_t rad = 0) { fRadius = rad; };

 private:
  Int_t fRadius;

  ClassDef(PndSdsSimpleStripClusterFinder, 1);
};

#endif // PndSdsSimpleStripClusterFinder
