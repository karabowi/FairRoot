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
// -----             PndSdsSimpleStripClusterFinder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndSdsSimpleStripClusterFinder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
#include "math.h"

// enum SensorSide { SensorSide::kTOP, SensorSide::kBOTTOM };

PndSdsSimpleStripClusterFinder::PndSdsSimpleStripClusterFinder(Int_t DigiType) : PndSdsStripClusterer(DigiType), fRadius(1) {}
PndSdsSimpleStripClusterFinder::PndSdsSimpleStripClusterFinder(Int_t DigiType, Int_t rad) : PndSdsStripClusterer(DigiType), fRadius(rad) {}

PndSdsSimpleStripClusterFinder::~PndSdsSimpleStripClusterFinder() {}

std::vector<PndSdsClusterStrip *> PndSdsSimpleStripClusterFinder::SearchClusters()
{
  ///  -----  search for clusters  -----
  ///  Take neighbouring fired strips.
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  fLeftDigis.clear();
  std::vector<Int_t> onecluster;
  Indexpair::iterator tempStrip;
  // fSortedDigis[sensor][side][stripnr]=digiindex
  for (Fullmap::iterator itSensors = fSortedDigis.begin(); itSensors != fSortedDigis.end(); ++itSensors) // iterate over sensor
  {
    for (std::map<SensorSide, Indextriple>::iterator itSide = (itSensors->second).begin(); itSide != (itSensors->second).end(); ++itSide) // iterate over sensor side
    {
      //       std::cout<<"clsterfinder: ";
      //       if(itSide->first == SensorSide::kTOP) std::cout<<"top"<<std::endl;
      //       else std::cout<<"bottom"<<std::endl;
      for (Indextriple::iterator itTime = (itSide->second).begin(); itTime != (itSide->second).end(); ++itTime) { // iterate over timestamp

        // create a flagmap: for each digi (identified by iDigi, the index of the clonesarray) a flag if it is used
        Indexpair flagmap;
        for (Indexpair::iterator itStrip = (itTime->second).begin(); itStrip != (itTime->second).end(); ++itStrip) {
          flagmap[itStrip->second] = 1;
        } // iterate over channel

        for (Indexpair::iterator itStrip = (itTime->second).begin(); itStrip != (itTime->second).end(); itStrip++) {
          tempStrip = itStrip;
          if (!(1 == flagmap[itStrip->second]))
            continue;
          for (Indexpair::iterator itStrip3 = itStrip; itStrip3 != (itTime->second).end(); ++itStrip3) {
            if (!(1 == flagmap[itStrip3->second]))
              continue;
            if (std::abs(tempStrip->first - itStrip3->first) > fRadius)
              continue;
            onecluster.push_back(itStrip3->second);
            flagmap[itStrip3->second] = -1; // do not reuse this digi
            tempStrip = itStrip3;
            //             std::cout<<"add strip "<<itStrip3->first << " from digi "<<itStrip3->second <<std::endl;
          }
          AddCluster(onecluster, itSide->first);
          onecluster.clear();
          // std::cout<<" --- "<<std::endl;
        } // end loop itStrip

        if (onecluster.size() > 0) {
          Warning("SearchClusters", "a hangover cluster of %zu digis", onecluster.size());
          AddCluster(onecluster, itSide->first);
          onecluster.clear();
        }
        for (Indexpair::iterator itflag = flagmap.begin(); itflag != flagmap.end(); ++itflag) {
          if (1 == itflag->second)
            fLeftDigis.push_back(itflag->first);
        }
      } // end loop itTime
    }   // end loop it Side
  }     // end loop sensor
  return fClusters;
}

ClassImp(PndSdsSimpleStripClusterFinder);

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

// double PndSdsSimpleStripClusterFinder::meanColumn(std::vector<PndSdsDigiPixel*>& list){
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
// double PndSdsSimpleStripClusterFinder::errMeanColumn(std::vector<PndSdsDigiPixel*>& list, double pitch){
//
//   // only geometric resolution so far....
//   double errMean = list.size()*pitch/sqrt(12);
//   return errMean;
// }
