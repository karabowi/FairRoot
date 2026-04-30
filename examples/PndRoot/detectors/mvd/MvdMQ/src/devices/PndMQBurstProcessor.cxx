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
 * File:   PndMQBurstProcessor.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQBurstProcessor::Run() with Boost transport data format
#include <PndMQBurstProcessor.h>
#include "PndMQGapEventBuilder.h"
#include "PndMvdPixelClusterFinder.h"
#include "PndMQMvdChargeWeightedPixelMapping.h"
#include "PndMapSorter.h"
#include "PndGeoHandling.h"

class TMessage2 : public TMessage {
 public:
  TMessage2(void *buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
};

void PndMQBurstProcessor::CustomCleanupParameters(void *data, void *hint)
{
  delete (std::string *)hint;
}

void PndMQBurstProcessor::free_string(void *data, void *hint)
{
  delete static_cast<std::string *>(hint);
}

void PndMQBurstProcessor::Run()
{
  if (fHasBoostSerialization) {
    while (CheckCurrentState(RUNNING)) {
      FairMQParts parts;
      std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
      if (Receive(msg, "data-in") >= 0) {
        LOG(info) << "Received data! " << msg->GetSize() << std::endl;
        string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
        istringstream ibuffer(msgStr);
        if (!ibuffer.good())
          LOG(info) << "IBUFFER IS BAD!";
        try {
          boost::archive::binary_iarchive InputArchive(ibuffer);

          InputArchive >> fBurstDataIn;
        } catch (boost::archive::archive_exception &e) {
          LOG(error) << e.what();
          //					continue;
        }
        fNewRunId = fBurstDataIn.fHeader.fRunID;
        if (fNewRunId != fCurrentRunId) {
          fCurrentRunId = fNewRunId;
          UpdateParameters();
          SetParameters();
        }

        if (fBurstDataIn.fData.size() < 1)
          continue;
        //				LOG(info) << "BurstData size: " << fBurstDataIn.fData[0].size() << " BurstID: " << fBurstDataIn.fHeader.fBurstID;

        ProcessData();
      }
      if (fBurstDataOut.fData.size() > 0) {
        fBurstDataOut.fHeader.fBurstID = fBurstDataIn.fHeader.fBurstID;
        std::ostringstream obuffer;
        boost::archive::binary_oarchive OutputArchive(obuffer);
        OutputArchive << fBurstDataOut;
        std::string *strMsg = new std::string(obuffer.str());
        unique_ptr<FairMQMessage> msgOut(NewMessage(const_cast<char *>(strMsg->c_str()), strMsg->length(), free_string, strMsg));
        LOG(info) << "Data sent: " << fBurstDataOut.fHeader.fBranchName << " BurstID: " << fBurstDataOut.fHeader.fBurstID << " size: " << msgOut->GetSize();
        int event = 0;
        //				for (auto eventItr : fBurstDataOut->fData){
        //					for (auto dataItr : eventItr){
        //						LOG(info) << event << " : " << dataItr->GetTimeStamp();
        //					}
        //					event++;
        //				}
        Send(msgOut, "data-out");
      }
    }
  } else {
    LOG(error) << " Boost Serialization not ok";
  }
}

void PndMQBurstProcessor::UpdateParameters()
{
  for (int iparC = 0; iparC < fParCList->GetEntries(); iparC++) {
    FairParGenericSet *tempObj = (FairParGenericSet *)(fParCList->At(iparC));
    fParCList->Remove(tempObj);
    fParCList->AddAt(UpdateParameter(tempObj), iparC);
  }
}

FairParGenericSet *PndMQBurstProcessor::UpdateParameter(FairParGenericSet *thisPar)
{
  std::string paramName = thisPar->GetName();
  //  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  std::string *reqStr = new std::string(paramName + "," + std::to_string(fCurrentRunId));
  LOG(warning) << "Requesting parameter \"" << paramName << "\" for Run ID " << fCurrentRunId << " (" << thisPar << ")";
  std::unique_ptr<FairMQMessage> req(NewMessage(const_cast<char *>(reqStr->c_str()), reqStr->length(), CustomCleanupParameters, reqStr));
  std::unique_ptr<FairMQMessage> rep(NewMessage());

  if (Send(req, "param") > 0) {
    if (Receive(rep, "param") > 0) {
      TMessage2 tm(rep->GetData(), rep->GetSize());
      thisPar = (FairParGenericSet *)tm.ReadObject(tm.GetClass());
      LOG(warning) << "Received parameter" << paramName << " from the server (" << thisPar << ")" << tm.GetClass()->GetName() << " DataSize: " << rep->GetSize();
      thisPar->print();
      return thisPar;
    }
  }
  return nullptr;
}
