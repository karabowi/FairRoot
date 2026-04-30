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

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include "PndMQTopix4DigiToHit.h"

#include "PndMQStatus.h"

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"
#include <iomanip>

using namespace std;

PndMQTopix4DigiToHit::PndMQTopix4DigiToHit()
  : fHasBoostSerialization(false), fClusterFinder(20, 32, 1.8), fHitProducer(0.01, 0.01, 20, 32), fEventBuilder(50), fClusterSize(11), fStatusOutput(true)
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

PndMQTopix4DigiToHit::~PndMQTopix4DigiToHit() {}

void PndMQTopix4DigiToHit::Run()
{
  if (fHasBoostSerialization) {
    FairMQChannel &dataInChannel = fChannels.at("data-in").at(0);
    FairMQChannel &dataOutChannel = fChannels.at("data-out").at(0);
    bool statusChannelPresent = false;

    if (fStatusOutput) {
      try {
        (fChannels.at("status-out"));
      } catch (...) {
        fStatusOutput = false;
        LOG(info) << "No Status-Out channel!";
      }
    }

    while (CheckCurrentState(RUNNING)) {
      std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());
      std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
      int status = PndMQStatus::UNDEFINED;
      if (dataInChannel.Receive(header) > 0) {
        status = *(static_cast<int *>(header->GetData()));

        if (dataInChannel.ExpectsAnotherPart()) {
          if (dataInChannel.Receive(msg) > 0) {
            string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
            istringstream ibuffer(msgStr);

            boost::archive::binary_iarchive InputArchive(ibuffer);

            try {
              InputArchive >> fTopixDigis;
            } catch (boost::archive::archive_exception &e) {
              LOG(error) << e.what();
            }
          }
        }
      }
      //			LOG(info) << "InputData: ";
      //			for (auto itr : fTopixDigis){
      //				LOG(info) << std::setw(12) << std::hex << itr.GetTimeStamp();
      //			}

      fEventBuilder.FillData(fTopixDigis);
      std::vector<std::vector<PndSdsDigiTopix4>> separatedData = fEventBuilder.GetSeparatedData();

      for (auto &eventIter : separatedData) {
        std::vector<PndSdsHit> hits;
        if (eventIter.size() > 1) {
          std::vector<std::vector<int>> cluster = fClusterFinder.GetClusters(eventIter);
          //					LOG(info) << "Multiple Event Found with " << eventIter.size() << " digs and " << cluster.size() << " clusters";
          for (auto clusterIter : cluster) {
            std::vector<PndSdsDigiTopix4> clusterDigis;
            for (auto digiIter : clusterIter) {
              clusterDigis.push_back((eventIter)[digiIter]);
              //							LOG(info) << "Digis " << digiIter << " TimeStamp: " << (eventIter)[digiIter].GetTimeStamp();
              //							LOG(info) << "ClusterDigis: " << clusterDigis.rbegin()->GetTimeStamp();
            }
            // PndSdsHit myHit = fDummy.GetHit(clusterDigis);
            // fHitProducer.GetHit(clusterDigis);
            if (clusterDigis.size() < fClusterSize.size() - 1) // counts the sizes of clusters. All above fClusterSize.size() -1 are added to last bin
              fClusterSize[clusterDigis.size()]++;
            else
              fClusterSize[fClusterSize.size() - 1]++;

            hits.push_back(fHitProducer.GetHit(clusterDigis));
          }
        } else {
          // PndSdsHit myHit = fDummy.GetHit(eventIter);
          // fHitProducer.GetHit(eventIter);
          hits.push_back(fHitProducer.GetHit(eventIter));
        }
        fTopixHitsEvent.push_back(hits);
      }

      if (status == PndMQStatus::STOP) {
        LOG(info) << "Received STOP-Signal!" << std::endl;
      }

      unique_ptr<FairMQMessage> headerOut(fTransportFactory->CreateMessage(sizeof(int)));
      memcpy(headerOut->GetData(), &status, sizeof(int));
      dataOutChannel.SendPart(headerOut);

      std::ostringstream obuffer;
      boost::archive::binary_oarchive OutputArchive(obuffer);
      OutputArchive << fTopixHitsEvent;
      int outputSize = obuffer.str().length();
      unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
      memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
      dataOutChannel.Send(msg2);

      if (fStatusOutput == true) {
        unique_ptr<FairMQMessage> headerOut2(fTransportFactory->CreateMessage(sizeof(int)));
        memcpy(headerOut->GetData(), &status, sizeof(int));
        fChannels.at("status-out").at(0).SendPart(headerOut2);

        std::ostringstream obuffer2;
        boost::archive::binary_oarchive OutputArchive2(obuffer2);
        OutputArchive2 << fClusterSize;
        int outputSize2 = obuffer2.str().length();
        unique_ptr<FairMQMessage> msg3(fTransportFactory->CreateMessage(outputSize2));
        memcpy(msg3->GetData(), obuffer2.str().c_str(), outputSize2);
        fChannels.at("status-out").at(0).Send(msg3);
      }

      //			LOG(info) << "OutputEventBuilding: ";
      //			for (auto outerIter : fTopixHitsEvent){
      //				LOG(info) << "Event: ";
      //				for (auto innerIter : outerIter) {
      //					LOG(info)<< innerIter;
      //				}
      //			}
      fTopixDigis.clear();
      fTopixHitsEvent.clear();
    }
  }
}
