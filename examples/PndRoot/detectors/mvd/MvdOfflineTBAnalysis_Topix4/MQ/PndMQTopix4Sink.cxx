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
#include <boost/archive/binary_iarchive.hpp>
#include <PndMQTopix4Sink.h>

#include "baseMQtools.h"

#include "FairMQLogger.h"
#include "mrfdata_8b.h"
#include "PndSdsDigiTopix4.h"

#include <TH2.h>
#include <TCanvas.h>

using namespace std;

PndMQTopix4Sink::PndMQTopix4Sink() : fHasBoostSerialization(false)
{
  // gSystem->ResetSignal(kSigInterrupt);
  // gSystem->ResetSignal(kSigTermination);

  using namespace baseMQ::tools::resolve;
  // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
  if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1)
    fHasBoostSerialization = true;
}

// void PndMQTopix4Sink::CustomCleanup(void *data, void *object)
//{
//    delete (string*)object;
//}

void PndMQTopix4Sink::Run()
{
  LOG(info) << "Boost Serialization " << fHasBoostSerialization;
  if (fHasBoostSerialization) {
    FairMQChannel &dataInChannel = fChannels.at("data-in").at(0);
    int receivedMsgs = 0;

    while (CheckCurrentState(RUNNING)) {
      FairMQMessage *msg = fTransportFactory->CreateMessage();

      if (dataInChannel.Receive(msg) > 0) {
        LOG(info) << "Received Message: ";
        LOG(info) << receivedMsgs++;
        LOG(info) << msg->GetSize();

        string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
        istringstream ibuffer(msgStr);

        boost::archive::binary_iarchive InputArchive(ibuffer);

        try {
          InputArchive >> fTopixData;
        } catch (boost::archive::archive_exception &e) {
          LOG(error) << e.what();
        }

        LOG(info) << "TopixData: " << fTopixData.size();

        if (fTopixData.size() > 0) {
          //            	ostringstream obuffer;
          //            	boost::archive::binary_oarchive OutputArchive(obuffer);
          //            	OutputArchive << frames.front();
          //            	int outputSize = obuffer.str().length();
          //            	unique_ptr<FairMQMessage> msg2(fTransportFactory->CreateMessage(const_cast<char*>(obuffer.str().c_str()), outputSize, CustomCleanup, &obuffer));
          //            	fChannels.at("data-out").at(0).Send(msg);
          LOG(info) << "Data: " << fTopixData.size() << " " << fTopixData.front() << std::endl;
        }
        delete (msg);

        if (fTopixData.size() > 0)
          fTopixData.clear();
      }
    }
  }
}

PndMQTopix4Sink::~PndMQTopix4Sink() {}
