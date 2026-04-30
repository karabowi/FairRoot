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

#ifndef PNDMVDSIMPLEPIXELCLUSTERFINDER_H
#define PNDMVDSIMPLEPIXELCLUSTERFINDER_H

#include "PndSdsSimplePixelClusterFinder.h"
#include "PndSdsTotDigiPar.h"
#include "PndSdsPixelDigiPar.h"
/// PndMvdSimplePixelClusterFinder.h
/// Simple cluster finder for pixel detectors.
///
/// It takes the first hit in the DigiArray and moves it into a new cluster.
/// Then it looks through the remaining hits in the DigiArray if there are hits which are in the area around the first hit and moves them into the cluster.
/// The radius is given by the first parameter.
/// Then it takes the new hits in the cluster and looks if there are other hits which are in the range of these hits and moves them into the cluster, too.
/// This process is repeated until no hits around the cluster are found
/// The the first hit of the remaining DigiHits is taken to create a new hit and the cluster search process is started again.

class PndMvdSimplePixelClusterFinder : public PndSdsSimplePixelClusterFinder {
 public:
  PndMvdSimplePixelClusterFinder(TString parName = "MVDPixelDigiPar", TString totParName = "MVDPixelTotDigiPar", Int_t verbose = 0);
  PndMvdSimplePixelClusterFinder(PndSdsPixelDigiPar *digiPar, PndSdsTotDigiPar *totPar);
  PndMvdSimplePixelClusterFinder(const PndMvdSimplePixelClusterFinder &) = delete;
  PndMvdSimplePixelClusterFinder &operator=(const PndMvdSimplePixelClusterFinder &) = delete;
  void SetParName(TString val) { fParName = val; }
  void SetTotParName(TString val) { fTotParName = val; }

 protected:
  void SetParameters();

 private:
  TString fParName;
  TString fTotParName;
  PndSdsPixelDigiPar *fDigiPar;
  PndSdsTotDigiPar *fTotDigiPar;
};
#endif
