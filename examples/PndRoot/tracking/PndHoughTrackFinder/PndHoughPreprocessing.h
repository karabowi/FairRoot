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

#ifndef PndHoughPreprocessing_H_
#define PndHoughPreprocessing_H_

#include "FairTask.h"
#include "FairRootManager.h"
#include "PndTrackCand.h"
#include "PndHoughData.h"
#include "PndStt2GeoHandler.h"

class PndHoughPreprocessing {

 public:
  PndHoughPreprocessing(PndHoughData *data)
    : fHits(data->GetHits()), fMapTubetoHit(data->GetMapTubetoHit()), fMapFairLinktoFairHit(data->GetMapFairLinktoFairHit()),
      fMapFairLinktoIsochrone(data->GetMapFairLinktoIsochrone()), fMapFairLinktoIsochroneError(data->GetMapFairLinktoIsochroneError()),
      fMapFairLinktoTubeId(data->GetMapFairLinktoTubeId()), fSttGeoH(data->GetSttGeoH())
      //fMapFairLinktoTubeId(data->GetMapFairLinktoTubeId()), fStrawMap(data->GetStrawMap())
  {
    ioman = FairRootManager::Instance();
  };

  virtual ~PndHoughPreprocessing(){};
  /** @brief Deletes all hits, where a tube is hitted twice or more. */
  PndTrackCand CleanData(PndTrackCand &trackCand);
  /** @brief Deletes all skewed hits. */
  PndTrackCand RejectSkewed(PndTrackCand &trackCand);
  /** @brief Deletes all hits, where a tube is hitted twice or more and all skewed hits. */
  PndTrackCand CleanAndRejectSkewed(PndTrackCand &trackCand);

 private:
  FairRootManager *ioman = nullptr;
  //PndSttStrawMap *fStrawMap = nullptr;
  PndStt2GeoHandler *fSttGeoH = nullptr;
  std::map<FairLink, FairHit *> fMapFairLinktoFairHit;
  std::map<FairLink, Double_t> fMapFairLinktoIsochrone;
  std::map<FairLink, Double_t> fMapFairLinktoIsochroneError;
  std::map<FairLink, Int_t> fMapFairLinktoTubeId;
  std::map<int, FairHit *> fMapTubetoHit; // maps hitted tube to FairHit
  std::vector<FairHit *> fHits;           // vector with selected hits of an event

  ClassDef(PndHoughPreprocessing, 1);
};

#endif /*PndHoughPreprocessing_H_*/
