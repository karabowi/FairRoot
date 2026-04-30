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

/********************************************************************************
 *    Copyright (C) 2014 GSI Helmholtzzentrum fuer Schwerionenforschung GmbH    *
 *                                                                              *
 *              This software is distributed under the terms of the             *
 *         GNU Lesser General Public Licence version 3 (LGPL) version 3,        *
 *                  copied verbatim in the file "LICENSE"                       *
 ********************************************************************************/
/**
 * PndMQTopix4Sink.cxx
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include "PndMQHitEventDevice.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"
#include "PndMQStatus.h"

using namespace std;

PndMQHitEventDevice::PndMQHitEventDevice() : fHasBoostSerialization(false), fGlobalRunningStatus(true), fBuilder(0)
{
  using namespace baseMQ::tools::resolve;
  bool checkOutputClass = false;

  if (is_same<boost::archive::binary_oarchive, boost::archive::binary_oarchive>::value) {
    if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_oarchive &, const unsigned int)>::value == 1) {
      checkOutputClass = true;
      fHasBoostSerialization = true;
    }
  }
  LOG(info) << "HasBoostSerialization: " << fHasBoostSerialization;
}

PndMQHitEventDevice::~PndMQHitEventDevice()
{
  delete (fBuilder);
}

void PndMQHitEventDevice::Run()
{
  int numInputs = fChannels.at("data-in").size();

  // store the channel references to avoid traversing the map on every loop iteration
  const FairMQChannel &dataOutChannel = fChannels.at("data-out").at(0);
  const FairMQChannel &statusChannel = fChannels.at("status-out").at(0);
  FairMQChannel *dataInChannels[fChannels.at("data-in").size()];
  LOG(info) << "Number of Input Channels: " << numInputs;
  fBuilder = new PndMQHitsEventBuilder(numInputs);
  for (int i = 0; i < numInputs; ++i) {
    dataInChannels[i] = &(fChannels.at("data-in").at(i));
  }
  fDataFromChannels.resize(numInputs);
  fRunningStatus.resize(numInputs);
  for (int channel = 0; channel < fRunningStatus.size(); channel++)
    fRunningStatus[channel] = true;

  //	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

  std::vector<int> fillLevel(numInputs, 0);

  int eventCounter = 0;
  bool stopMessageOnce = true;

  while (CheckCurrentState(RUNNING)) {
    if (fGlobalRunningStatus == true) {
      for (int channelNr = 0; channelNr < numInputs; channelNr++) {
        //        	LOG(info) << "---- Reading channel " << channelNr << " ----";
        if (fillLevel[channelNr] == 0 && fRunningStatus[channelNr] == true) {

          std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());
          std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());

          if (dataInChannels[channelNr]->Receive(header) > 0) {
            int status = *(static_cast<int *>(header->GetData()));
            if (status == PndMQStatus::RUNNING)
              fRunningStatus[channelNr] = true;
            else if (status == PndMQStatus::STOP) {
              fRunningStatus[channelNr] = false;
              LOG(info) << "STOP-Status received for channel: " << channelNr;
            }
            if (dataInChannels[channelNr]->ExpectsAnotherPart()) {

              if (dataInChannels[channelNr]->Receive(msg) > 0) {
                std::string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
                std::istringstream ibuffer(msgStr);

                boost::archive::binary_iarchive InputArchive(ibuffer);

                try {
                  InputArchive >> fHitData;
                } catch (boost::archive::archive_exception &e) {
                  LOG(error) << e.what();
                }
                // fDataFromChannels[channelNr].push_back(fHitData);
                fDataFromChannels[channelNr].insert(fDataFromChannels[channelNr].end(), fHitData.begin(), fHitData.end());
                // LOG(info) << "Data in channel " << fDataFromChannels[channelNr].size();
                fHitData.clear();
              }
            }
          }
          if (fillLevel[channelNr] == 0 && fRunningStatus[channelNr] == false) {
            fGlobalRunningStatus = false;
            LOG(info) << "GlobarRunningStatus set to false for channel " << channelNr;
          }
        }
      }
      fBuilder->AddData(fDataFromChannels);
      fEventData = fBuilder->GetEvents();

      if (eventCounter++ % 100 == 0) {
        LOG(info) << eventCounter << " nEvents: " << fEventData.size() << " hits in Event " << fEventData.front().size()
                  << " timeStamp: " << TString::Format("%12.0f", fEventData.front().front().GetTimeStamp()).Data() << " sensorID " << fEventData.front().front().GetSensorID();
        fSensorsInEvent = fBuilder->GetSensorsInEvent();
        LOG(info) << "ChannelsInEvent: ";
        for (auto data : fSensorsInEvent)
          LOG(info) << data;

        std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage(sizeof(int)));
        int flag = PndMQStatus::RUNNING;
        memcpy(headerCopy->GetData(), &flag, sizeof(int));
        statusChannel.SendPart(headerCopy);

        std::ostringstream obuffer;
        boost::archive::binary_oarchive OutputArchive(obuffer);
        // fPndSdsDigiTopix4Vector = frames.front();
        OutputArchive << fSensorsInEvent;
        int outputSize = obuffer.str().length();
        unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage(outputSize));
        memcpy(msg->GetData(), obuffer.str().c_str(), outputSize);
        // unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
        statusChannel.Send(msg);
      }

      std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage(sizeof(int)));
      int flag = PndMQStatus::RUNNING;
      memcpy(headerCopy->GetData(), &flag, sizeof(int));
      dataOutChannel.SendPart(headerCopy);

      std::ostringstream obuffer;
      boost::archive::binary_oarchive OutputArchive(obuffer);
      // fPndSdsDigiTopix4Vector = frames.front();
      OutputArchive << fEventData;
      int outputSize = obuffer.str().length();
      unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
      memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
      // unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
      dataOutChannel.Send(msg2);

      fillLevel = fBuilder->GetInputDataLevel();
      //		LOG(info) << "EventData.size() " << eventData.size();
      //		for (auto eventIter : eventData){
      //			LOG(info) << "Event";
      //			for (auto dataIter : eventIter){
      //				LOG(info) << dataIter.GetSensorID() << " " << dataIter.GetTimeStamp();
      //			}
      //		}

      for (int channelNr = 0; channelNr < fDataFromChannels.size(); channelNr++) {
        fDataFromChannels[channelNr].clear();
      }
    }
    bool allStop = true;
    for (auto state : fRunningStatus) {
      if (state == true)
        allStop = false;
    }
    if (fGlobalRunningStatus == false && stopMessageOnce == true) {
      LOG(info) << "STOP-Signal received for one input";
      std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage(sizeof(int)));
      int flag = PndMQStatus::STOP;
      memcpy(headerCopy->GetData(), &flag, sizeof(int));
      dataOutChannel.Send(headerCopy);
      stopMessageOnce = false;
    }
  }
}

void PndMQHitEventDevice::SetProperty(const int key, const string &value)
{
  switch (key) {
  default: FairMQDevice::SetProperty(key, value); break;
  }
}

string PndMQHitEventDevice::GetProperty(const int key, const string &default_ /*= ""*/)
{
  switch (key) {
  default: return FairMQDevice::GetProperty(key, default_);
  }
}

void PndMQHitEventDevice::SetProperty(const int key, const int value)
{
  switch (key) {
  default: FairMQDevice::SetProperty(key, value); break;
  }
}

int PndMQHitEventDevice::GetProperty(const int key, const int default_ /*= 0*/)
{
  switch (key) {
  default: return FairMQDevice::GetProperty(key, default_);
  }
}
