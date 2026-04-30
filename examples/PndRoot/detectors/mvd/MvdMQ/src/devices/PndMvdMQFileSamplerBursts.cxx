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
 * PndMvdMQFileSamplerBursts.cpp
 *
 * @since 2016-03-08
 * @author R. Karabowicz
 */

#include <PndMvdMQFileSamplerBursts.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/unique_ptr.hpp>

#include "FairMQLogger.h"

#include "FairMQMessage.h"
#include "TMessage.h"
#include "FairEventHeader.h"

#include "PndSdsDigiPixel.h"
#include "PndSdsDigiStrip.h"
#include "PndSttHit.h"
#include "PndSdsHit.h"

using namespace std;

BOOST_CLASS_EXPORT_GUID(PndSdsDigiPixel, "PndSdsDigiPixel");
BOOST_CLASS_EXPORT_GUID(PndSdsDigiStrip, "PndSdsDigiStrip");
BOOST_CLASS_EXPORT_GUID(PndSttHit, "PndSttHit");
// BOOST_CLASS_EXPORT_GUID(PndSdsHit, "PndSdsHit");

PndMvdMQFileSamplerBursts::PndMvdMQFileSamplerBursts()
  : FairMQDevice(), fRunAna(nullptr), fSource(nullptr), fOutputData(), fNObjects(0), fMaxIndex(-1), fBranchNames(), fFileNames(), fEventHeader(0)
{
}

PndMvdMQFileSamplerBursts::~PndMvdMQFileSamplerBursts()
{
  for (auto itr : fBurstBuilder)
    delete itr.second;
  delete fSource;
  delete fRunAna;
}

void PndMvdMQFileSamplerBursts::InitTask()
{
  fRunAna = new FairRunAna();
  if (fFileNames.size() > 0) {
    fSource = new FairFileSource(fFileNames.at(0).c_str());
    for (unsigned int ifile = 1; ifile < fFileNames.size(); ifile++)
      fSource->AddFile(fFileNames.at(ifile));
  }
  fSource->Init();
  LOG(info) << "Going to request " << fBranchNames.size() << "  branches:";

  fSource->ActivateObject((TObject **)&fEventHeader, "EventHeader.");

  for (unsigned int ibrn = 0; ibrn < fBranchNames.size(); ibrn++) {

    LOG(info) << " requesting branch \"" << fBranchNames[ibrn].second << "\"";
    std::string branchName = fBranchNames[ibrn].second;
    int branchStat = fSource->ActivateObject((TObject **)&fInputBranches[branchName], branchName.c_str()); // should check the status...
    LOG(info) << "BranchStat: " << branchStat;
    InitBurstBuilder(branchName);
    LOG(info) << "Activated object \"" << fInputBranches[branchName] << "\" with name \"" << fBranchNames[ibrn].second << " for channel " << fBranchNames[ibrn].first << "/ ("
              << branchStat << ")";
    fNObjects++;
  }

  if (fMaxIndex < 0)
    fMaxIndex = fSource->CheckMaxEventNo();
  LOG(info) << "Input source has " << fMaxIndex << " events.";
}

void PndMvdMQFileSamplerBursts::InitBurstBuilder(std::string branchName)
{
  PndBurstVectorBuilderBase *tmpBuilder = 0;
  if (branchName == "MVDPixelDigis")
    tmpBuilder = new PndBurstVectorBuilderT<PndSdsDigiPixel>;
  else if (branchName == "MVDStripDigis")
    tmpBuilder = new PndBurstVectorBuilderT<PndSdsDigiStrip>;
  else if (branchName == "STTHit")
    tmpBuilder = new PndBurstVectorBuilderT<PndSttHit>;
  else if (branchName == "MVDHitsStrip")
    tmpBuilder = new PndBurstVectorBuilderT<PndSdsHit>;

  if (tmpBuilder != 0)
    fBurstBuilder[branchName] = tmpBuilder;
}

class TMessage2 : public TMessage {
 public:
  TMessage2(void *buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
};

// helper function to clean up the object holding the data after it is transported.
void free_string(void *data, void *hint)
{
  delete static_cast<std::string *>(hint);
}

void PndMvdMQFileSamplerBursts::Run()
{
  int eventCounter = 0;

  // Check if we are still in the RUNNING state.
  //	boost::this_thread::sleep(boost::posix_time::milliseconds(100000));
  while (CheckCurrentState(RUNNING)) {
    if (eventCounter != fMaxIndex) {

      Int_t readEventReturn = fSource->ReadEvent(eventCounter);

      if (readEventReturn != 0)
        break;

      //			if (fEventHeader != 0)
      //				LOG(info) << "EventHeader: " << fEventHeader->GetRunId() << " " << fEventHeader->GetEventTime() << std::endl;

      for (auto branchItr : fInputBranches) {
        if (branchItr.first.find(".") == std::string::npos) {
          std::vector<std::vector<FairTimeStamp *>> data;
          TClonesArray *tmpArray = (TClonesArray *)branchItr.second;

          if (fBurstBuilder.count(branchItr.first) > 0) {
            fOutputData[branchItr.first] = fBurstBuilder[branchItr.first]->ProcessData(tmpArray);
            //				LOG(info) << branchItr.first << " has " << fOutputData[branchItr.first].size() << " bursts! ";
          }
        }
      }
    } else {
      LOG(info) << "FinishRun";
      for (auto branchItr : fInputBranches) {
        if (branchItr.first.find(".") == std::string::npos) {
          if (fBurstBuilder.count(branchItr.first) > 0) {
            fOutputData[branchItr.first] = fBurstBuilder[branchItr.first]->GetLastData();
            //				LOG(info) << branchItr.first << " has " << fOutputData[branchItr.first].size() << " last bursts! ";
            //				for (auto dataItr : fOutputData[branchItr.first]){
            //					LOG(info) << branchItr.first << " has data: " << dataItr.size();
            //				}
          }
        }
      }
    }

    for (auto portIt = fPorts.begin(); portIt != fPorts.end(); portIt++) {
      for (auto branchIt = fPortBranchNameMap.lower_bound(*portIt); branchIt != fPortBranchNameMap.upper_bound(*portIt); ++branchIt) {
        //				LOG(info) << branchIt->second << " burstSize " << fOutputData[branchIt->second].size();
        for (auto dataIt = fOutputData[branchIt->second].begin(); dataIt != fOutputData[branchIt->second].end(); ++dataIt) {
          //					LOG(info) << branchIt->second << " dataSize " << dataIt->size();
          if (dataIt->size() > 0) {
            BurstData bData; // = new BurstData;
            std::vector<std::vector<FairTimeStamp *>> dataVector;
            dataVector.push_back(*dataIt);
            bData.fData = dataVector;
            bData.fHeader.fBranchName = branchIt->second;
            bData.fHeader.fRunID = fEventHeader->GetRunId();
            bData.fHeader.fBurstID = fBurstBuilder[branchIt->second]->GetBurstId(dataVector[0][0]);
            std::ostringstream obuffer;
            boost::archive::binary_oarchive OutputArchive(obuffer);
            OutputArchive << bData;
            std::string *strMsg = new std::string(obuffer.str());
            unique_ptr<FairMQMessage> msg(NewMessage(const_cast<char *>(strMsg->c_str()), strMsg->length(), free_string, strMsg));
            LOG(info) << "Send message: " << bData.fHeader.fBranchName << " " << bData.fHeader.fBurstID << " size: " << msg->GetSize();
            // LOG(info) << obuffer.str();
            Send(msg, *portIt);
          }
        }
      }
    }
    if (eventCounter != fMaxIndex) {
      eventCounter++;
    } else
      break;
  }
  LOG(info) << "Going out of RUNNING state.";
}
