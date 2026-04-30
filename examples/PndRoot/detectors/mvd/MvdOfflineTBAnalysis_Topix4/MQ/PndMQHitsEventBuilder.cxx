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
 * PndMQHitsEventBuilder.cxx
 *
 *  Created on: 21.10.2015
 *      Author: Stockmanns
 */

#include <PndMQHitsEventBuilder.h>

#include "FairMQLogger.h"

PndMQHitsEventBuilder::PndMQHitsEventBuilder(int nChannels) : fNChannels(nChannels), fEventCounter(0), fSensorsInEvent(nChannels, 0)
{
  fInputData.resize(nChannels);
}

PndMQHitsEventBuilder::~PndMQHitsEventBuilder()
{
  // TODO Auto-generated destructor stub
}

void PndMQHitsEventBuilder::AddData(vector<vector<vector<PndSdsHit>>> &data)
{
  for (int channelNr = 0; channelNr < data.size(); channelNr++) {
    // LOG(info) << "PndMQHitsEventBuilder::AddData from Channel " << channelNr << " size " << data[channelNr].size();
    fInputData[channelNr].insert(fInputData[channelNr].end(), data[channelNr].begin(), data[channelNr].end());
  }
}

vector<vector<PndSdsHit>> PndMQHitsEventBuilder::GetEvents()
{
  vector<vector<PndSdsHit>> result;
  while (HasData()) {
    result.push_back(GetNextEvent());
  }
  return result;
}

bool PndMQHitsEventBuilder::HasData()
{
  bool result = true;
  for (int channelIter = 0; channelIter < fInputData.size(); channelIter++) {
    if (fInputData[channelIter].size() == 0) {
      result = false;
    }
  }
  //	if (result == false)
  //		LOG(info) << "HasData " << result;
  return result;
}

vector<PndSdsHit> PndMQHitsEventBuilder::GetNextEvent()
{
  vector<PndSdsHit> result;
  //	if (fEventCounter % 1000 == 0){
  //		LOG(info) << fEventCounter;
  //		for (int channelNr = 0; channelNr < fNChannels; channelNr++){
  //			LOG(info) << channelNr << " : " << fInputData[channelNr].size();
  //		}
  //	}
  //	fEventCounter++;
  //	LOG(info) << "Data in channels";
  //	for (auto channelIter: fInputData){
  //		LOG(info) << "Channel size: " << channelIter.size();
  //		for (auto eventIter : channelIter)
  //			for (auto dataIter : eventIter)
  //				LOG(info) << dataIter.GetSensorID() << " " << dataIter.GetTimeStamp();
  //	}

  vector<vector<PndSdsHit>> firstDataInChannels;
  firstDataInChannels.resize(fNChannels);
  for (int channelNr = 0; channelNr < fInputData.size(); channelNr++) {
    if (fInputData[channelNr].size() > 0) {
      if (fInputData[channelNr][0].size() > 0)
        firstDataInChannels[channelNr] = fInputData[channelNr][0];
      else
        return result;
    }
  }
  //	LOG(info) << "FirstDataInChannel:";
  //	for (int channel = 0; channel < firstDataInChannels.size(); channel++){
  //		//LOG(info) << "Channel: " << channel << " : ";
  //		for (auto data : firstDataInChannels[channel]){
  //			LOG(info) << channel << " : " << TString::Format("%12.0f", data.GetTimeStamp()).Data();
  //		}
  //	}
  vector<bool> channelInEvent = GetChannelsInEvent(firstDataInChannels);

  int nSensorsInEvent = 0;
  //	LOG(info) << "ChannelMatch:";
  for (auto inEvent : channelInEvent) {
    // LOG(info) << inEvent;
    nSensorsInEvent += inEvent;
  }

  if (nSensorsInEvent > 0)
    fSensorsInEvent[nSensorsInEvent - 1]++;

  //	LOG(info) << "channelInEvent.size " << channelInEvent.size();
  for (int channelNr = 0; channelNr < fNChannels; channelNr++) {
    if (channelInEvent[channelNr] == true) {
      // LOG(info) << "Add Data to result from: " << channelNr;
      result.insert(result.end(), firstDataInChannels[channelNr].begin(), firstDataInChannels[channelNr].end());
      // LOG(info) << "DataInserted: " << result.size();
      fInputData[channelNr].erase(fInputData[channelNr].begin());
      //			LOG(info) << "Delete data from InputData " << channelNr << " " << fInputData[channelNr].size();
    }
  }
  // if (nChannelsInEvent > 2){
  //	LOG(info) << "Data in Result: ";
  //	for (auto data : result)
  //		LOG(info) << data.GetSensorID() << " : " << data.GetTimeStamp();
  //}
  return result;
}

vector<bool> PndMQHitsEventBuilder::GetChannelsInEvent(vector<vector<PndSdsHit>> &eventData)
{
  double offset = 50;
  vector<bool> result(fNChannels, false);

  int firstChannel = FindFirstChannel(eventData);
  //	LOG(info) << "FirstChannel: " << firstChannel;
  if (firstChannel > -1) {
    double firstTimeStamp = eventData[firstChannel][0].GetTimeStamp();
    double lastTimeStamp = eventData[firstChannel].rbegin()->GetTimeStamp() + offset;
    result[firstChannel] = true;

    for (int channelNr = 0; channelNr < eventData.size(); channelNr++) {
      if (channelNr != firstChannel) {
        double actualFirstTS = eventData[channelNr].begin()->GetTimeStamp();
        double actualLastTS = eventData[channelNr].rbegin()->GetTimeStamp();
        if (actualFirstTS == firstTimeStamp)
          result[channelNr] = true;
        if (actualFirstTS < firstTimeStamp) {
          if (actualLastTS >= firstTimeStamp) {
            result[channelNr] = true;
          }
        } else if (actualFirstTS <= lastTimeStamp) {
          result[channelNr] = true;
        }
      }
    }
  }
  return result;
}

int PndMQHitsEventBuilder::FindFirstChannel(vector<vector<PndSdsHit>> &eventData)
{
  int result = -1;
  double oldTimeStamp = -1;
  for (int channelNr = 0; channelNr < eventData.size(); channelNr++) {
    double actualTimeStamp = eventData[channelNr].front().GetTimeStamp();
    if (actualTimeStamp < oldTimeStamp || oldTimeStamp < 0) {
      result = channelNr;
      oldTimeStamp = actualTimeStamp;
    }
  }
  return result;
}
