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
 * PndMQTopix4Sorter.cxx
 *
 * @since 2014-10-10
 * @author A. Rybalchenko
 */

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <PndMapSorterTpl.h>
#include "PndMQTopix4Sorter.h"
#include "PndMQStatus.h"

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "PndSdsDigiTopix4.h"

#include <TH2.h>
#include <TCanvas.h>

using namespace std;

PndMQTopix4Sorter::PndMQTopix4Sorter() : fHasBoostSerialization(false)
{
  // gSystem->ResetSignal(kSigInterrupt);
  // gSystem->ResetSignal(kSigTermination);

  using namespace baseMQ::tools::resolve;
  // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
  if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1)
    fHasBoostSerialization = true;
}

// void PndMQTopix4Sorter::CustomCleanup(void *data, void *object)
//{
//    delete (string*)object;
//}

void PndMQTopix4Sorter::Run()
{
  LOG(info) << "Boost Serialization " << fHasBoostSerialization;
  if (fHasBoostSerialization) {
    FairMQChannel &dataInChannel = fChannels.at("data-in").at(0);
    FairMQChannel &dataOutChannel = fChannels.at("data-out").at(0);

    int receivedMsgs = 0;
    PndMapSorterTpl<PndSdsDigiTopix4> sorter;

    while (CheckCurrentState(RUNNING)) {
      FairMQMessage *header = fTransportFactory->CreateMessage();
      FairMQMessage *msg = fTransportFactory->CreateMessage();

      if (dataInChannel.Receive(header) > 0) {
        int status = *(static_cast<int *>(header->GetData()));

        if (dataInChannel.ExpectsAnotherPart()) {
          if (dataInChannel.Receive(msg)) {
            string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
            istringstream ibuffer(msgStr);

            boost::archive::binary_iarchive InputArchive(ibuffer);

            try {
              InputArchive >> fTopixData;
            } catch (boost::archive::archive_exception &e) {
              LOG(error) << e.what();
            }

            // LOG(info) << "TopixData: " << fTopixData.size();
            //				for (auto iter : fTopixData){
            // LOG(info) << iter.GetTimeStamp();
            //				}

            bool endSorting = false;
            double timeOfLast = 0;
            if (fTopixData.size() > 0) {
              for (auto iter : fTopixData) {
                if (iter.GetTimeStamp() > 0) {
                  sorter.AddElement(iter, iter.GetTimeStamp());
                  timeOfLast = iter.GetTimeStamp();
                } else {
                  endSorting = true;
                  // LOG(info) << "---END SORTING---";
                }
              }
              if (endSorting == false) {
                sorter.WriteOutData(timeOfLast);
                fOutputData = sorter.GetOutputData();
                sorter.DeleteOutputData();
              } else if (endSorting == true || status == PndMQStatus::STOP) {
                LOG(info) << "EndSorting or STOP-Status " << status;
                sorter.WriteOutAll();
                fOutputData = sorter.GetOutputData();
                fOutputData.push_back(PndSdsDigiTopix4());
                sorter.DeleteOutputData();
                endSorting = false;
              }

              unique_ptr<FairMQMessage> headerCpy(fTransportFactory->CreateMessage(sizeof(int)));
              headerCpy->Copy(header);
              dataOutChannel.SendPart(headerCpy);

              std::ostringstream obuffer;
              boost::archive::binary_oarchive OutputArchive(obuffer);
              OutputArchive << fOutputData;
              int outputSize = obuffer.str().length();
              unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
              memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
              dataOutChannel.Send(msg2);

              // LOG(info) << "Data: " << fTopixData.size() << " " << timeOfLast;
              // LOG(info) << "Output: " << fOutputData.size() << " timeOfLast: " << timeOfLast;
              //					for(auto itr : fOutputData)
              // LOG(info) << itr.GetTimeStamp();

              fTopixData.clear();
              fOutputData.clear();
            }
            delete (msg);
            delete (header);
          }
        }
        // LOG(info) << "Received Message: " << receivedMsgs++ << " Size: " << msg->GetSize();
        if (status == PndMQStatus::STOP) {
          LOG(info) << "STOP-Signal Received!";
          sorter.WriteOutAll();
          fOutputData = sorter.GetOutputData();
          fOutputData.push_back(PndSdsDigiTopix4());
          sorter.DeleteOutputData();

          unique_ptr<FairMQMessage> headerCpy(fTransportFactory->CreateMessage(sizeof(int)));
          headerCpy->Copy(header);
          dataOutChannel.SendPart(headerCpy);

          std::ostringstream obuffer;
          boost::archive::binary_oarchive OutputArchive(obuffer);
          OutputArchive << fOutputData;
          int outputSize = obuffer.str().length();
          unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
          memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
          dataOutChannel.Send(msg2);
        }
      }
    }
  }
}

PndMQTopix4Sorter::~PndMQTopix4Sorter() {}
