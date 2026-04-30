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
 * PndMQMerger.cxx
 *
 * @since 2012-12-06
 * @author D. Klein, A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "PndMQMerger.h"
#include "FairMQPoller.h"

using namespace std;

PndMQMerger::PndMQMerger() : fHasBoostSerialization(false), fOutputData(0)
{
  using namespace baseMQ::tools::resolve;
  // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
  if (has_BoostSerialization<FairTimeStamp *, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1)
    fHasBoostSerialization = true;
}

PndMQMerger::~PndMQMerger() {}

void CustomClean(void *data, void *hint)
{
  LOG(info) << "FREEMESSAGE called for data: " << static_cast<BurstData *>(hint)->fHeader.fBranchName;
  delete static_cast<BurstData *>(hint);
}

void PndMQMerger::Run()
{
  int direction = 0;
  int numInputs = fChannels.at("data-in").size();
  //    fRunningStatus.resize(numInputs);

  LOG(info) << "Number of Input Channels: " << numInputs;

  //    boost::this_thread::sleep(boost::posix_time::milliseconds(10000));

  std::unique_ptr<FairMQPoller> poller(fTransportFactory->CreatePoller(fChannels, {"data-in"}));

  int nMessages = 0;
  while (CheckCurrentState(RUNNING)) {
    poller->Poll(-1);
    for (int channelNr = 0; channelNr < numInputs; channelNr++) {
      LOG(info) << "---- Reading channel " << channelNr << " ----";
      if (poller->CheckInput("data-in", channelNr)) {
        std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());

        nMessages++;

        LOG(info) << "Get Data for Channel: " << channelNr;

        if (Receive(msg, "data-in", channelNr) >= 0) {
          LOG(info) << "---Data Received--- " << msg->GetSize();

          std::string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
          //					LOG(info) << msgStr;
          std::istringstream ibuffer(msgStr);

          try {
            boost::archive::binary_iarchive InputArchive(ibuffer);
            InputArchive >> fInputData;
          } catch (boost::archive::archive_exception &e) {
            LOG(error) << e.what();
            //						continue;
          }
          LOG(info) << "Data: " << fInputData.fHeader.fBurstID << " " << fInputData.fHeader.fBranchName;
          //					LOG(info) << "DataSize: " << fInputData.fData.size() << " " << fInputData.fData[0].size();
          //					for (auto gapItr : fInputData.fData){
          //						for (auto dataItr : gapItr)
          //							LOG(info) << "TimeStamp: " << dataItr->GetTimeStamp();
          //					}

          fInputMap[fInputData.fHeader.fBurstID][fInputData.fHeader.fBranchName] = fInputData;
          LOG(info) << "InputMap for BurstID: " << fInputData.fHeader.fBurstID << " " << fInputMap[fInputData.fHeader.fBurstID].size();

          if (fInputMap[fInputData.fHeader.fBurstID].size() == numInputs) { // all input channels have delivered data for this burstID
            // std::map<std::string, BurstData> dataToProcess = fInputMap[fInputData.fHeader.fBurstID];
            //					fInputMap.erase(fInputMap.find(fInputData.fHeader.fBurstID));
            ProcessData(fInputMap[fInputData.fHeader.fBurstID]);

            //					if (fOutputData != 0){
            //						std::ostringstream obuffer;
            //						boost::archive::binary_oarchive OutputArchive(obuffer);
            //						OutputArchive << *fOutputData;
            //						int outputSize = obuffer.str().length();
            //						unique_ptr<FairMQMessage> msgOut(NewMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomClean, fOutputData));
            //						Send(msgOut, "data-out");
            //					}
          }
        }
      }
    }
  }
}
