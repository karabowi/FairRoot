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
 * PndMQSorterMerger.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "PndMQSorterMerger.h"
#include "PndMQStatus.h"

using namespace std;

PndMQSorterMerger::PndMQSorterMerger() : fHasBoostSerialization(false)
{
  using namespace baseMQ::tools::resolve;
  // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
  if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1)
    fHasBoostSerialization = true;
}

PndMQSorterMerger::~PndMQSorterMerger() {}

void PndMQSorterMerger::Run()
{
  int direction = 0;
  int numInputs = fChannels.at("data-in").size();
  fRunningStatus.resize(numInputs);

  // store the channel references to avoid traversing the map on every loop iteration
  const FairMQChannel &dataOutChannel = fChannels.at("data-out").at(0);
  FairMQChannel *dataInChannels[fChannels.at("data-in").size()];
  LOG(info) << "Number of Input Channels: " << numInputs;
  for (int i = 0; i < numInputs; ++i) {
    dataInChannels[i] = &(fChannels.at("data-in").at(i));
    fRunningStatus[i] = true;
  }
  fData.resize(numInputs);
  int activeChannel = 0;
  bool switchChannel = false;
  bool channelSwitched = false;

  double oldTS = -1;

  int nMessages = 0;
  while (CheckCurrentState(RUNNING)) {
    for (int channelNr = 0; channelNr < numInputs; channelNr++) {
      //       	LOG(info) << "---- Reading channel " << channelNr << " ----";
      std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
      std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());

      nMessages++;

      if (fRunningStatus[channelNr] == true && dataInChannels[channelNr]->Receive(header) > 0) {

        int status = *(static_cast<int *>(header->GetData()));

        if (status == PndMQStatus::STOP) {
          fRunningStatus[channelNr] = false;
          LOG(info) << "STOP-Signal received for channel " << channelNr;
        }

        if (dataInChannels[channelNr]->ExpectsAnotherPart()) {
          if (dataInChannels[channelNr]->Receive(msg)) {
            std::string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
            std::istringstream ibuffer(msgStr);

            boost::archive::binary_iarchive InputArchive(ibuffer);

            try {
              InputArchive >> fInputData;
            } catch (boost::archive::archive_exception &e) {
              LOG(error) << e.what();
            }

            fData[channelNr].insert(fData[channelNr].end(), fInputData.begin(), fInputData.end());

            fInputData.clear();

            //				LOG(info) << "fData size for channel " << channelNr << " is " << fData[channelNr].size();
            //				for (auto data : fData[channelNr])
            //					LOG(info) << data.GetTimeStamp();
            if (activeChannel == channelNr) {
              //							LOG(info) << "--- Writing channel " << activeChannel << " ---";
              if (fData[channelNr].size() > 0) {
                for (std::vector<PndSdsDigiTopix4>::iterator data = fData[channelNr].begin(); data != fData[channelNr].end(); data++) {
                  if (data->GetTimeStamp() < 0) {
                    fOutputData.insert(fOutputData.end(), fData[channelNr].begin(), data);
                    switchChannel = true;
                    channelSwitched = true;
                    fData[channelNr].erase(fData[channelNr].begin(), ++data);
                    //								LOG(info) << "Negative TS in " << channelNr << " new Data size " << fData[channelNr].size();
                    break;
                  }
                }
                if (switchChannel == false) {
                  fOutputData = fData[channelNr];
                  fData[channelNr].clear();
                }

                int flag = PndMQStatus::RUNNING;

                unique_ptr<FairMQMessage> headerOut(fTransportFactory->CreateMessage(sizeof(int)));
                memcpy(headerOut->GetData(), &flag, sizeof(int));
                dataOutChannel.SendPart(headerOut);

                std::ostringstream obuffer;
                boost::archive::binary_oarchive OutputArchive(obuffer);
                OutputArchive << fOutputData;
                int outputSize = obuffer.str().length();
                unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
                memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
                dataOutChannel.Send(msg2);
                //								if (nMessages % 1000 == 0){
                //									LOG(info) << "fOutputData.size: " << fOutputData.size() << " " << TString::Format("%12.0f", fOutputData.front().GetTimeStamp()).Data();
                //								}
                for (auto info : fOutputData) {
                  //									LOG(info) << TString::Format("%12.0f", info.GetTimeStamp()).Data();
                  if (info.GetTimeStamp() > 0 && oldTS > info.GetTimeStamp()) {
                    LOG(info) << "++++ SortingError ++++ " << oldTS << " > " << info.GetTimeStamp();
                  }
                  oldTS = info.GetTimeStamp();
                }
                fOutputData.clear();
                if (switchChannel == true) {
                  activeChannel++;
                  if (activeChannel >= numInputs) {
                    activeChannel = 0;
                  }
                  //							LOG(info) << "Switch active channel to " << activeChannel;
                  switchChannel = false;
                }
              }
            }
          }
        }
        bool allStop = true;
        for (auto state : fRunningStatus) {
          if (state == true)
            allStop = false;
        }

        if (allStop == true) {
          LOG(info) << "STOP-Signal received for all. Emptying buffers.";
          for (int i = 0; i < numInputs; i++) {
            int flag = PndMQStatus::UNDEFINED;
            if (i != numInputs - 1) {
              flag = PndMQStatus::RUNNING;
            } else {
              flag = PndMQStatus::STOP;
            }
            unique_ptr<FairMQMessage> headerOut(fTransportFactory->CreateMessage(sizeof(int)));
            memcpy(headerOut->GetData(), &flag, sizeof(int));
            dataOutChannel.SendPart(headerOut);

            fOutputData = fData[activeChannel];
            fData[channelNr].clear();

            std::ostringstream obuffer;
            boost::archive::binary_oarchive OutputArchive(obuffer);
            OutputArchive << fOutputData;
            int outputSize = obuffer.str().length();
            unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
            memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
            dataOutChannel.Send(msg2);
            //						LOG(info) << "fOutputData.size: " << fOutputData.size();
            for (auto info : fOutputData) {
              //							LOG(info) << info.GetTimeStamp();
              if (info.GetTimeStamp() > 0 && oldTS > info.GetTimeStamp()) {
                LOG(info) << "++++ SortingError ++++ " << oldTS << " > " << info.GetTimeStamp();
              }
              oldTS = info.GetTimeStamp();
            }
            fOutputData.clear();

            activeChannel++;
            if (activeChannel >= numInputs) {
              activeChannel = 0;
            }
          }
        }
      }
    }
  }
}
