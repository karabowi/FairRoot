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

/*
 * File:   PndMQMvdPixelDigiProcessorBursts.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQMvdPixelDigiProcessorBursts::Run() with Boost transport data format
#include "PndMQMvdPixelDigiProcessorBursts.h"

void PndMQMvdPixelDigiProcessorBursts::SetParameters()
{
  fGeoPar->GetGeometry();
  fDigiPar = (PndSdsPixelDigiPar *)fParCList->FindObject("MVDPixelDigiPar");
  fTotPar = (PndSdsTotDigiPar *)fParCList->FindObject("MVDPixelTotDigiPar");
  fSensorPar = (PndSensorNamePar *)fParCList->FindObject("PndSensorNamePar");
  fSensorPar->FillMap();
  if (fGeoHandler == 0) {
    fGeoHandler = new PndGeoHandling(fSensorPar);
  } else {
    fGeoHandler->SetSensorNamePar(fSensorPar);
  }

  LOG(info) << "Conversion Method: " << *fDigiPar;
  LOG(info) << "Tot Info: " << *fTotPar;
  if (fPixelMapping == 0) {
    fPixelMapping = new PndMQMvdChargeWeightedPixelMapping(fGeoHandler, fDigiPar, fTotPar);
  } else {
    fPixelMapping->SetDigiPar(fDigiPar);
    fPixelMapping->SetTotPar(fTotPar);
    fPixelMapping->SetGeoHandling(fGeoHandler);
  }
}

void PndMQMvdPixelDigiProcessorBursts::ProcessData()
{

  fBurstDataOut.Reset();
  for (auto dataItr : fBurstDataIn.fData)
    fMapSorter.AddElements(dataItr);
  fMapSorter.WriteOutAll();
  std::vector<FairTimeStamp *> sortedData = fMapSorter.GetOutputData();

  //	 for (auto dataItr: sortedData)
  //		 LOG(info) << "DataAfterSorter " << dataItr->GetTimeStamp() ;

  fEventBuilder.FillData(sortedData);
  std::vector<std::vector<FairTimeStamp *>> gapData = fEventBuilder.GetSeparatedData();
  std::vector<std::vector<FairTimeStamp *>> lastData = fEventBuilder.GetLastData();
  gapData.insert(gapData.end(), lastData.begin(), lastData.end());
  //	 for (auto eventItr : gapData)
  //		 for (auto dataItr : eventItr)
  //			 LOG(info) << "DataAfterEventBuilder: " << dataItr->GetTimeStamp();
  if (fBurstDataIn.fHeader.fBranchName == "MVDPixelDigis") {
    std::vector<std::vector<FairTimeStamp *>> hitsInGaps;
    for (auto gapIter : gapData) {
      std::vector<PndSdsDigiPixel *> digiPixel;
      std::vector<FairTimeStamp *> hits;
      if (gapIter.size() > 0) {
        // LOG(info) << "GapSize > 0 " << gapIter.size();
        for (auto dataIter : gapIter) { // iterate hits in gap
          digiPixel.push_back((PndSdsDigiPixel *)dataIter);
          //								 LOG(info) << "DataInGap: " << *(PndSdsDigiPixel*) dataIter;
        }
        std::vector<std::vector<Int_t>> clusterInts = fClusterFinder.GetClusters(digiPixel);

        for (auto clusterIter : clusterInts) // iterate all found cluster
        {
          // LOG(info) << "ClusterSize: " << clusterIter.size();
          std::vector<PndSdsDigiPixel *> digiInCluster;
          for (auto digiInClusterItr : clusterIter) { // iterate digisInt in one cluster
            int pos = digiInClusterItr;
            digiInCluster.push_back(digiPixel[pos]);
          }
          //					 LOG(info) << "DigisInCluster: ";
          if (digiInCluster.size() > 0) {
            //						 for (auto digiIter : digiInCluster)
            //							 LOG(info) << digiIter->GetTimeStamp();
            PndSdsHit *hit = new PndSdsHit(fPixelMapping->GetCluster(digiInCluster));
            hits.push_back(hit);
            //						 LOG(info) << "Hit: " << hit->GetTimeStamp();
            digiInCluster.clear();
            // LOG(info) << "Hit: " << *(PndSdsHit*)(*hits.rbegin());
          }
        }
        hitsInGaps.push_back(hits);
      }
    }
    fBurstDataOut.fHeader.fBranchName = "MVDHitsPixel";
    fBurstDataOut.fData = hitsInGaps;
  }
}
