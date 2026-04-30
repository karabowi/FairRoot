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
// -----             PndSdsStripAdvClusterFinder source file             -----
// -----          Converted 27.11.2007 from R.Jaekel by R.Kliemt       -----
// -------------------------------------------------------------------------

#include "PndSdsStripAdvClusterFinder.h"

// includes for sort algorithms
#include <algorithm>
#include <functional>
#include <vector>
#include "TMath.h"
// enum SensorSide { SensorSide::kTOP, SensorSide::kBOTTOM };

PndSdsStripAdvClusterFinder::PndSdsStripAdvClusterFinder(Int_t DigiType) : PndSdsStripClusterer(DigiType), fNmCh(2), fNmTS(0) {}

PndSdsStripAdvClusterFinder::PndSdsStripAdvClusterFinder(Int_t DigiType, Int_t NrofmissedChannels, Int_t NrofmissedTimestamp)
  : PndSdsStripClusterer(DigiType), fNmCh(NrofmissedChannels + 1), fNmTS(NrofmissedTimestamp)
{
}

PndSdsStripAdvClusterFinder::~PndSdsStripAdvClusterFinder() {}

std::vector<PndSdsClusterStrip *> PndSdsStripAdvClusterFinder::SearchClusters()
{
  ///  -----  search for clusters  -----
  ///  Take neighbouring fired strips.
  fClusters.clear();
  fTopclusters.clear();
  fBotclusters.clear();
  std::vector<Int_t> onecluster;
  Indexpair::iterator tempStrip;
  // fSortedDigis[sensor][side][stripnr]=digiindex
  std::cout << "beim Cluster" << std::endl;
  for (Fullmap::iterator itSensors = fSortedDigis.begin(); itSensors != fSortedDigis.end(); ++itSensors) {                                  // iterate over sensor
    for (std::map<SensorSide, Indextriple>::iterator itSide = (itSensors->second).begin(); itSide != (itSensors->second).end(); ++itSide) { // iterate over sensor side
      // CH             TS      ID,Used
      std::map<Int_t, std::map<Int_t, std::vector<Int_t>>> hitmap;
      for (Indextriple::iterator itTime = (itSide->second).begin(); itTime != (itSide->second).end(); ++itTime) {    // iterate over timestamp
        for (Indexpair::iterator itStrip = (itTime->second).begin(); itStrip != (itTime->second).end(); ++itStrip) { // iterate over channel
          hitmap[itStrip->first][itTime->first][0] = itStrip->second;                                                // ID store
          hitmap[itStrip->first][itTime->first][1] = 1;                                                              // used store
        }
      } // loop time

      // loop over channels in hitmap
      for (std::map<Int_t, std::map<Int_t, std::vector<Int_t>>>::const_iterator ch = hitmap.begin(); ch != hitmap.end(); ++ch) {
        // loop over all frames in hitmap in this channel
        for (std::map<Int_t, std::vector<Int_t>>::const_iterator ts = ch->second.begin(); ts != ch->second.end(); ++ts) {
          //				std::vector< Int_t > onecluster;
          ClusterIterator(ch->first, ts->first, hitmap, onecluster); // new Cluster from ch and ts
          if (onecluster.size() > 0) {
            //          			std::cout<<" -W- PndSdsStripAdvClusterFinder::SearchClusters(): cluster hangover? "<<onecluster.size()<<std::endl;
            AddCluster(onecluster, itSide->first);
            onecluster.clear();
          }
        } // iter time
      }   // iter channel
    }     // end loop it Side
  }       // end loop sensor
  return fClusters;
}

void PndSdsStripAdvClusterFinder::ClusterIterator(Int_t newCh, Int_t newFrameID, std::map<Int_t, std::map<Int_t, std::vector<Int_t>>> &hitmap, std::vector<Int_t> &onecluster)
{
  if (hitmap.find(newCh) == hitmap.end())
    return; // end of hitmap reached (channel)
  if (hitmap[newCh].find(newFrameID) == hitmap[newCh].end())
    return; // end of hitmap reached (frame)
  if (hitmap[newCh][newFrameID][1] == -1)
    return;                                           // already identified
  hitmap[newCh][newFrameID][1] = -1;                  // now identified
  onecluster.push_back(hitmap[newCh][newFrameID][0]); // to clusterlist

  for (Int_t channel = (newCh - fNmCh); channel != (newCh + fNmCh); ++channel) {
    for (Int_t frame = (newFrameID - fNmTS); frame != (newFrameID + fNmTS); ++frame) {
      ClusterIterator(channel, frame, hitmap, onecluster);
    }
  }
  return;
}

ClassImp(PndSdsStripAdvClusterFinder);
