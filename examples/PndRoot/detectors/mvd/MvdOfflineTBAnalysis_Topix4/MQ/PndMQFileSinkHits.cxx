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
 * File:   PndMQFileSinkHits.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSinkHits::Run() with Boost transport data format
#include "PndMQFileSinkHits.h"
#include "PndMQStatus.h"

void PndMQFileSinkHits::Run()
{
  if (fHasBoostSerialization) {
    int receivedMsgs = 0;

    // store the channel references to avoid traversing the map on every loop iteration
    FairMQChannel &dataInChannel = fChannels.at("data-in").at(0);

    while (CheckCurrentState(RUNNING)) {
      std::unique_ptr<FairMQMessage> header(fTransportFactory->CreateMessage());
      std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
      if (dataInChannel.Receive(header) > 0) {
        int status = *(static_cast<int *>(header->GetData()));

        if (dataInChannel.ExpectsAnotherPart()) {
          receivedMsgs++;
          if (dataInChannel.Receive(msg)) {
            string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
            istringstream ibuffer(msgStr);
            boost::archive::binary_iarchive InputArchive(ibuffer);
            // LOG(info) << "Received Message: " << receivedMsgs;
            try {
              InputArchive >> fHitVector;
            } catch (boost::archive::archive_exception &e) {
              LOG(error) << e.what();
            }

            PndSdsHit *myHit = 0;
            bool dataAboveTimeThreshold = false;
            double timeStampThreshold = 0;
            for (auto &eventIter : fHitVector) {
              fOutput->Delete();
              int numData = eventIter.size();
              for (Int_t i = 0; i < numData; ++i) {
                if (eventIter[i].GetTimeStamp() > timeStampThreshold) {
                  myHit = new ((*fOutput)[fOutput->GetEntriesFast()]) PndSdsHit(eventIter[i]);
                  dataAboveTimeThreshold = true;
                }
                //                   LOG(info) << "Data: " << i << " " << fHitVector.at(i).GetTimeStamp();
              }
              if (dataAboveTimeThreshold) {
                if (fOutput->IsEmpty()) {
                  LOG(error) << "PndMQFileSinkHits::Run(): No Output array!";
                } else {

                  fTree->Fill();
                }
              }
            }
            if (receivedMsgs % 1000 == 0 && myHit != 0) {
              LOG(info) << receivedMsgs << " : " << myHit->GetTimeStamp();
            }
          }
        }
        if (status == PndMQStatus::STOP) {
          LOG(info) << "STOP-Signal Received!";
          fTree->AutoSave();
          LOG(info) << "AutoSave called!";
          fTree->Write();
          fOutFile->Close();
        }
      }

      if (fHitVector.size() > 0) {
        fHitVector.clear();
      }
    }

    LOG(info) << "I've received " << receivedMsgs << " messages!";
  } else {
    LOG(error) << " Boost Serialization not ok";
  }
}
