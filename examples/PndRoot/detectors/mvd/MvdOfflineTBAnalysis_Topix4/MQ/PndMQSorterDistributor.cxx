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
 * PndMQSorterDistributor.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "PndMQSorterDistributor.h"
#include "PndMQStatus.h"

using namespace std;

PndMQSorterDistributor::PndMQSorterDistributor() : fThreshold(1E9), fOffset(1E6), fHasBoostSerialization(false)
{
  using namespace baseMQ::tools::resolve;
  // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
  if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1)
    fHasBoostSerialization = true;
}

PndMQSorterDistributor::~PndMQSorterDistributor() {}

void PndMQSorterDistributor::Run()
{
  int direction = 0;
  int numOutputs = fChannels.at("data-out").size();

  // store the channel references to avoid traversing the map on every loop iteration
  const FairMQChannel &dataInChannel = fChannels.at("data-in").at(0);
  FairMQChannel *dataOutChannels[fChannels.at("data-out").size()];
  LOG(info) << "Number of Output Channels: " << numOutputs;
  for (int i = 0; i < numOutputs; ++i) {
    dataOutChannels[i] = &(fChannels.at("data-out").at(i));
  }

  double currentThreshold = fThreshold;
  double currentOffset = currentThreshold + fOffset;

  while (CheckCurrentState(RUNNING)) {
    std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
    std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());

    if (dataInChannel.Receive(header) > 0) {
      int status = *(static_cast<int *>(header->GetData()));

      if (status != PndMQStatus::RUNNING) {
        LOG(info) << "WrongStatus: " << status;
      }

      if (dataInChannel.ExpectsAnotherPart()) {
        if (dataInChannel.Receive(msg) > 0) {
          std::string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
          std::istringstream ibuffer(msgStr);
          boost::archive::binary_iarchive InputArchive(ibuffer);
          try {
            InputArchive >> fTopixData;
          } catch (boost::archive::archive_exception &e) {
            LOG(error) << e.what();
          }

          //			LOG(info) << "TopixData: " << fTopixData.size();

          bool switchChannels = false;
          for (auto itr : fTopixData) {
            if (itr.GetTimeStamp() < currentThreshold)
              fCurrentOutput.push_back(itr);
            else {
              fNextOutput.push_back(itr);
              if (itr.GetTimeStamp() > currentOffset) {
                //						LOG(info) << "Switch Channels: " << itr.GetTimeStamp() << " > " << currentOffset;
                switchChannels = true;
              }
            }
          }
          fTopixData.clear();
          if (switchChannels == true) {
            fCurrentOutput.push_back(PndSdsDigiTopix4()); // empty data to signal switch of channels
          }

          if (fCurrentOutput.size() > 0) {

            std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage(sizeof(int)));
            int flag = PndMQStatus::RUNNING;
            memcpy(headerCopy->GetData(), &flag, sizeof(int));
            dataOutChannels[direction]->SendPart(headerCopy);

            std::ostringstream obuffer;
            boost::archive::binary_oarchive OutputArchive(obuffer);
            // fPndSdsDigiTopix4Vector = frames.front();
            OutputArchive << fCurrentOutput;
            int outputSize = obuffer.str().length();
            unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
            memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
            // unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
            dataOutChannels[direction]->Send(msg2);
            // LOG(info) << "CurrentOutput send to " << direction << " size: " << fCurrentOutput.size();
            //				for (auto itr : fCurrentOutput){
            //					LOG(info) << itr.GetTimeStamp();
            //				}
            fCurrentOutput.clear();
          }
          if (fNextOutput.size() > 0) {

            int nextOutput = direction + 1;
            if (nextOutput >= numOutputs)
              nextOutput = 0;

            std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage(sizeof(int)));
            int flag = PndMQStatus::RUNNING;
            memcpy(headerCopy->GetData(), &flag, sizeof(int));
            dataOutChannels[nextOutput]->SendPart(headerCopy);

            std::ostringstream obuffer;
            boost::archive::binary_oarchive OutputArchive(obuffer);
            // fPndSdsDigiTopix4Vector = frames.front();
            OutputArchive << fNextOutput;
            int outputSize = obuffer.str().length();
            unique_ptr<FairMQMessage> msg3(fTransportFactory->CreateMessage(outputSize));
            memcpy(msg3->GetData(), obuffer.str().c_str(), outputSize);
            // unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
            dataOutChannels[nextOutput]->Send(msg3);
            // LOG(info) << "NextOutput send to " << nextOutput << " size " << fNextOutput.size();
            fNextOutput.clear();
          }

          if (switchChannels == true) {
            // LOG(info) << "Switch channels old threshold " << currentThreshold << " old offset "<< currentOffset;
            direction++;
            if (direction >= numOutputs) {
              direction = 0;
            }
            currentThreshold += fThreshold;
            currentOffset += fThreshold;
            switchChannels = false;
          }
        }
      }
      if (status == PndMQStatus::STOP) {
        LOG(info) << "STOP-Status Received: " << status;
        for (int i = 0; i < numOutputs; ++i) {
          std::unique_ptr<FairMQMessage> headerCopy(fTransportFactory->CreateMessage());
          headerCopy->Copy(header);
          dataOutChannels[i]->Send(headerCopy);
        }
      }
    }
  }
}
