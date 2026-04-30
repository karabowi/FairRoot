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
// -----             PndMvdStripClusterBuilder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndHypStripClusterBuilder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include <cmath>
#include "TMath.h"

PndHypStripClusterBuilder::PndHypStripClusterBuilder()
{
  // dummy
}

PndHypStripClusterBuilder::~PndHypStripClusterBuilder() {}

///--------------------------------------------------------------------------///
///--------------------------------------------------------------------------///
///--------------------------------------------------------------------------///

void PndHypStripClusterBuilder::Reinit()
{
  fSortedDigis.clear();
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
}

void PndHypStripClusterBuilder::AddDigi(std::string detName, SensorSide side, Int_t strip, Int_t iPoint)
{
  fSortedDigis[detName][side][strip] = iPoint;
  // std::cout<<" det name "<<detName<<" ipoint "<<iPoint<<std::endl;
}

std::vector<PndHypCluster> PndHypStripClusterBuilder::SearchClusters()
{
  ///  -----  search for clusters  -----
  ///  Take neighbouring fired strips.
  ///  use that std::map is a SORTED object
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  std::vector<Int_t> onecluster;

  for (Fullmap::iterator itSensors = fSortedDigis.begin(); itSensors != fSortedDigis.end(); ++itSensors) {
    // std::cout<< "name "<<itSensors->first<<std::endl;
    for (std::map<SensorSide, Indexpair>::iterator itSide = (itSensors->second).begin(); itSide != (itSensors->second).end(); ++itSide) {

      Indexpair flagmap;
      // std::cout<< "side "<<itSensors->second<<std::endl;
      for (Indexpair::iterator itStrip = (itSide->second).begin(); itStrip != (itSide->second).end(); ++itStrip) {
        flagmap[itStrip->second] = 1;
      }

      for (Indexpair::iterator itStrip = (itSide->second).begin(); itStrip != (itSide->second).end(); ++itStrip) {
        if (!(1 == flagmap[itStrip->second]))
          continue;
        for (Indexpair::iterator itStrip3 = itStrip; itStrip3 != (itSide->second).end(); ++itStrip3) {
          if (!(1 == flagmap[itStrip3->second]))
            continue;
          if (std::abs(itStrip->first - itStrip3->first) > 1)
            continue;

          onecluster.push_back(itStrip3->second);
          // make a helper to the next element
          flagmap[itStrip3->second] = -1; // do not reuse this digi
          itStrip = itStrip3;
        }
        AddCluster(onecluster, itSide->first);
        onecluster.clear();
      } // end loop itStrip

      if (onecluster.size() > 0) {
        std::cout << "mvd strip clusterfinder: cluster hangover? " << onecluster.size() << std::endl;
        AddCluster(onecluster, itSide->first);
        onecluster.clear();
      }
    } // end loop it Side
  }   // end loop sensor
  return fClusters;
}

PndHypCluster PndHypStripClusterBuilder::GetCluster(Int_t i)
{
  if (0 <= i && (int)fClusters.size() < i)
    return fClusters[i];
  return PndHypCluster();
}
PndHypCluster PndHypStripClusterBuilder::GetTopCluster(Int_t i)
{
  if (0 <= i && (int)fTopclusters.size() < i)
    return fClusters[(fTopclusters[i])];
  return PndHypCluster();
}
PndHypCluster PndHypStripClusterBuilder::GetBotCluster(Int_t i)
{
  if (0 <= i && (int)fBotclusters.size() < i)
    return fClusters[(fBotclusters[i])];
  return PndHypCluster();
}

void PndHypStripClusterBuilder::AddCluster(const std::vector<Int_t> &onecluster, SensorSide side)
{
  Int_t clindex = fClusters.size();
  if (side == SensorSide::kBOTTOM) {
    fBotclusters.push_back(clindex);
    // std::cout<<"Adding bot Cluster number "<<clindex<<" side "<<side<<" to the bottom list."<<std::endl;
  } else if (side == SensorSide::kTOP) {
    fTopclusters.push_back(clindex);
    // std::cout<<"Adding top Cluster number "<<clindex<<" side "<<side<<" to the top list."<<std::endl;
  }
  PndHypCluster cl(onecluster);
  cl.SetSensorSide(side);
  // std::cout<<"side clus cand "<<cl.GetSensorSide()<<std::endl;

  fClusters.push_back(cl);
}

ClassImp(PndHypStripClusterBuilder);
///--------------------------------------------------------------------------///
///--------------------------------------------------------------------------///
///--------------------------------------------------------------------------///

// double PndHypStripClusterBuilder::meanColumn(std::vector<PndHypDigiPixel*>& list){
//   double mean = 0.;
//   if (list.size() == 0){
//     std::cout<<" there are no channels in list to form cluster!"<<std::endl;
//   }
//   else{
//     // loop over digis to get weighted mean
//     double sumWeights = 0.;
//     double sumChannel = 0.;
//     for (int loopList = 0; loopList<list.size(); loopList++) {
//       sumWeights += list[loopList]->getTot();
//       sumChannel += list[loopList]->getTot() * list[loopList]->getColumn();
//     }
//     // calc the weighted mean
//     mean = sumChannel/sumWeights;
//   }
//   return mean;
// }
//
//
// double PndHypStripClusterBuilder::errMeanColumn(std::vector<PndHypDigiPixel*>& list, double pitch){
//
//   // only geometric resolution so far....
//   double errMean = list.size()*pitch/sqrt(12);
//   return errMean;
// }

// --- somehow the iterator does not work here with the templates
//  for(std::vector<T1*>::iterator iterdigi = digiList.begin();
//      iterdigi!=digiList.end();iterdigi++)
//
