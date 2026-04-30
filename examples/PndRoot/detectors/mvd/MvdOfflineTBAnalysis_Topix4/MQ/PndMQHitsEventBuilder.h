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
 * PndMQHitsEventBuilder.h
 *
 *  Created on: 21.10.2015
 *      Author: Stockmanns
 */

#ifndef MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQHITSEVENTBUILDER_H_
#define MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQHITSEVENTBUILDER_H_

#include <vector>
#include <array>

#include "PndSdsHit.h"

using namespace std;

class PndMQHitsEventBuilder {
 public:
  PndMQHitsEventBuilder(int nChannels = 4);
  virtual ~PndMQHitsEventBuilder();

  void AddData(vector<vector<vector<PndSdsHit>>> &data);
  vector<vector<PndSdsHit>> GetEvents();
  vector<int> GetSensorsInEvent() { return fSensorsInEvent; }
  std::vector<int> GetInputDataLevel()
  {
    std::vector<int> result;
    for (auto &channelIter : fInputData) {
      result.push_back(channelIter.size());
    }
    return result;
  }

 protected:
  vector<bool> GetChannelsInEvent(vector<vector<PndSdsHit>> &eventData);
  int FindFirstChannel(vector<vector<PndSdsHit>> &eventData); ///< channel which has the oldest data
  vector<PndSdsHit> GetNextEvent();
  vector<int> fSensorsInEvent;

  bool HasData();

 private:
  vector<vector<vector<PndSdsHit>>> fInputData; //< array of InputChannels. An InputChannel contains PndSdsHits grouped in events
  int fNChannels;
  int fEventCounter;
};

#endif /* MVD_MVDOFFLINETBANALYSIS_TOPIX4_MQ_PNDMQHITSEVENTBUILDER_H_ */
