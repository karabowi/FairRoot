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
 * File:   PndMQFileSink.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSink::Run() with Boost transport data format
#include <PndMQFileSink.h>

void PndMQFileSink::Run()
{
  if (fHasBoostSerialization) {
    int receivedMsgs = 0;

    // store the channel references to avoid traversing the map on every loop iteration
    FairMQChannel &dataInChannel = fChannels.at("data-in").at(0);

    while (CheckCurrentState(RUNNING)) {
      FairMQMessage *msg = fTransportFactory->CreateMessage();

      if (dataInChannel.Receive(msg) > 0) {
        receivedMsgs++;
        string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
        istringstream ibuffer(msgStr);
        boost::archive::binary_iarchive InputArchive(ibuffer);
        LOG(info) << "Received Message: " << receivedMsgs;
        try {
          InputArchive >> fHitVector;
        } catch (boost::archive::archive_exception &e) {
          LOG(error) << e.what();
        }

        int numInput = fHitVector.size();
        fOutput->Delete();

        for (Int_t i = 0; i < numInput; ++i) {
          new ((*fOutput)[i]) PndSdsDigiTopix4(fHitVector.at(i));
          //                   LOG(info) << "Data: " << i << " " << fHitVector.at(i).GetTimeStamp();
        }

        if (fOutput->IsEmpty()) {
          LOG(error) << "PndMQFileSink::Run(): No Output array!";
        }

        fTree->Fill();
      }

      delete msg;

      if (fHitVector.size() > 0) {
        fHitVector.clear();
      }
    }

    LOG(info) << "I've received " << receivedMsgs << " messages!";
  } else {
    LOG(error) << " Boost Serialization not ok";
  }
}
