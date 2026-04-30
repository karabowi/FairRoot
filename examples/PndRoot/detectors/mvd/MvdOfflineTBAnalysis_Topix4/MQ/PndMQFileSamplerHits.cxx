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
 * File:   PndMQFileSamplerHits.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSamplerHits::Run() with Boost transport data format
#include "PndMQFileSamplerHits.h"

#include "PndMQStatus.h"

void PndMQFileSamplerHits::Run()
{
  if (fHasBoostSerialization) {
    InitInputFile();
    int sendMsgs = 0;

    // store the channel references to avoid traversing the map on every loop iteration
    FairMQChannel &dataOutChannel = fChannels.at("data-out").at(0);

    int nEvents = fTree->GetEntries();
    bool firstRun = true;
    do {
      if (firstRun == true) {
        LOG(info) << "Number of events: " << nEvents;
        for (int eventNr = 0; eventNr < nEvents; eventNr++) {
          std::vector<PndSdsHit> tempVector;
          fTree->GetEntry(eventNr);
          for (int i = 0; i < fInput->GetEntriesFast(); i++) {
            PndSdsHit *hit = static_cast<PndSdsHit *>(fInput->At(i));
            if (!hit)
              continue;
            tempVector.push_back(*hit);
          }

          fHitVector.push_back(tempVector);

          if (eventNr % 1000 == 0) {

            unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(sizeof(int)));
            int status = PndMQStatus::RUNNING;
            memcpy(header->GetData(), &status, sizeof(int));
            dataOutChannel.SendPart(header);

            std::ostringstream obuffer;
            boost::archive::binary_oarchive OutputArchive(obuffer);
            OutputArchive << fHitVector;
            int outputSize = obuffer.str().length();
            unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
            memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
            dataOutChannel.Send(msg2);

            if (fHitVector.size() > 0)
              fHitVector.clear();

            if (!CheckCurrentState(RUNNING)) {
              break;
            }
          }

          if (eventNr + 1 == nEvents) {
            unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(sizeof(int)));
            int status = PndMQStatus::RUNNING;
            memcpy(header->GetData(), &status, sizeof(int));
            dataOutChannel.SendPart(header);

            std::ostringstream obuffer;
            boost::archive::binary_oarchive OutputArchive(obuffer);
            OutputArchive << fHitVector;
            int outputSize = obuffer.str().length();
            unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(outputSize));
            memcpy(msg2->GetData(), obuffer.str().c_str(), outputSize);
            dataOutChannel.Send(msg2);

            if (fHitVector.size() > 0)
              fHitVector.clear();

            if (!CheckCurrentState(RUNNING)) {
              break;
            }
          }
        }
        firstRun = false;
        LOG(info) << "Finished reading data!";

        unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage(sizeof(int)));
        int status = PndMQStatus::STOP;
        memcpy(header->GetData(), &status, sizeof(int));
        dataOutChannel.Send(header);
      }
    } while (CheckCurrentState(RUNNING));

    LOG(info) << "I've send " << sendMsgs << " messages!";
  } else {
    LOG(error) << " Boost Serialization not ok";
  }
}
