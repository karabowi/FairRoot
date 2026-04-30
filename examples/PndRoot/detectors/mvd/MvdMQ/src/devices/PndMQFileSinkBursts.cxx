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
 * File:   PndMQFileSinkBursts.tpl
 * Author: winckler, A. Rybalchenko
 *
 * Created on March 11, 2014, 12:12 PM
 */

// Implementation of PndMQFileSinkBursts::Run() with Boost transport data format
#include "PndMQFileSinkBursts.h"
#include "PndMQGapEventBuilder.h"
#include "PndMvdPixelClusterFinder.h"
#include "PndMQMvdChargeWeightedPixelMapping.h"
#include "PndMapSorter.h"
#include "PndGeoHandling.h"

class TMessage2 : public TMessage {
 public:
  TMessage2(void *buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
};

void PndMQFileSinkBursts::CustomCleanup(void *data, void *hint)
{
  delete (std::string *)hint;
}

void PndMQFileSinkBursts::Run()
{
  PndMQGapEventBuilder eventBuilder;
  PndMapSorter mapSorter;
  PndMvdPixelClusterFinder clusterFinder;
  PndMQMvdChargeWeightedPixelMapping *pixelMapping = 0;
  PndGeoHandling *geoHandler = 0;
  if (fHasBoostSerialization) {
    while (CheckCurrentState(RUNNING)) {
      FairMQParts parts;
      std::unique_ptr<FairMQMessage> msg(fTransportFactory->CreateMessage());
      if (Receive(msg, "data-in") >= 0) {
        //       		 LOG(info) << "Received data!" << std::endl;
        string msgStr(static_cast<char *>(msg->GetData()), msg->GetSize());
        istringstream ibuffer(msgStr);
        if (!ibuffer.good())
          LOG(info) << "IBUFFER IS BAD!";
        boost::archive::text_iarchive InputArchive(ibuffer);

        InputArchive >> fBurstData;

        fNewRunId = fBurstData.fHeader.fRunID;
        if (fNewRunId != fCurrentRunId) {
          fCurrentRunId = fNewRunId;
          UpdateParameters();
          fGeoPar->GetGeometry();
          fDigiPar = (PndSdsPixelDigiPar *)fParCList->FindObject("MVDPixelDigiPar");
          fTotPar = (PndSdsTotDigiPar *)fParCList->FindObject("MVDPixelTotDigiPar");
          fSensorPar = (PndSensorNamePar *)fParCList->FindObject("PndSensorNamePar");
          fSensorPar->FillMap();
          if (geoHandler == 0) {
            geoHandler = new PndGeoHandling(fSensorPar);
          }
          LOG(info) << "Conversion Method: " << *fDigiPar;
          LOG(info) << "Tot Info: " << *fTotPar;
          if (pixelMapping == 0) {
            pixelMapping = new PndMQMvdChargeWeightedPixelMapping(geoHandler, fDigiPar, fTotPar);
          }
        }

        if (fBurstData.fData.size() < 1)
          continue;
        LOG(info) << "BurstData size: " << fBurstData.fData[0].size();
        for (auto dataItr : fBurstData.fData[0])
          LOG(info) << dataItr->GetTimeStamp();
        mapSorter.AddElements(fBurstData.fData[0]);
        mapSorter.WriteOutAll();
        std::vector<FairTimeStamp *> sortedData = mapSorter.GetOutputData();
        //				 for (auto i : sortedData){
        //					 LOG(info) << "sorted: " << i->GetTimeStamp();
        //				 }
        eventBuilder.FillData(std::move(sortedData));
        std::vector<std::vector<FairTimeStamp *>> gapData = eventBuilder.GetSeparatedData();
        if (fBurstData.fHeader.fBranchName == "MVDPixelDigis") {
          for (auto gapIter : gapData) {
            std::vector<PndSdsDigiPixel *> digiPixel;
            if (gapIter.size() > 1) {
              LOG(info) << "GapSize > 1 " << gapIter.size();
              for (auto dataIter : gapIter) {
                digiPixel.push_back((PndSdsDigiPixel *)dataIter);
                //								 LOG(info) << "DataInGap: " << *(PndSdsDigiPixel*) dataIter;
              }
              std::vector<std::vector<Int_t>> clusterInts = clusterFinder.GetClusters(digiPixel);
              for (auto clusterIter : clusterInts) {
                LOG(info) << "ClusterSize: " << clusterIter.size();
                std::vector<PndSdsDigiPixel *> digiInCluster;
                for (auto digiInClusterItr : clusterIter) {
                  int pos = digiInClusterItr;
                  digiInCluster.push_back(digiPixel[pos]);
                }
                if (digiInCluster.size() > 0) {
                  PndSdsHit hit = pixelMapping->GetCluster(digiInCluster);
                  digiInCluster.clear();
                  LOG(info) << "Hit: " << hit;
                }
              }
            }
          }
        }
        //				 for (auto sdItr : sortedData)
        //					 delete(sdItr);
        //				 sortedData.clear();
        //				 mapSorter.DeleteOutputData();
      }
    }
    if (pixelMapping != 0)
      delete pixelMapping;
    if (geoHandler != 0)
      delete geoHandler;

  } else {
    LOG(error) << " Boost Serialization not ok";
  }
}

void PndMQFileSinkBursts::UpdateParameters()
{
  for (int iparC = 0; iparC < fParCList->GetEntries(); iparC++) {
    FairParGenericSet *tempObj = (FairParGenericSet *)(fParCList->At(iparC));
    fParCList->Remove(tempObj);
    fParCList->AddAt(UpdateParameter(tempObj), iparC);
  }
}

FairParGenericSet *PndMQFileSinkBursts::UpdateParameter(FairParGenericSet *thisPar)
{
  std::string paramName = thisPar->GetName();
  //  boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
  std::string *reqStr = new std::string(paramName + "," + std::to_string(fCurrentRunId));
  LOG(warning) << "Requesting parameter \"" << paramName << "\" for Run ID " << fCurrentRunId << " (" << thisPar << ")";
  std::unique_ptr<FairMQMessage> req(NewMessage(const_cast<char *>(reqStr->c_str()), reqStr->length(), CustomCleanup, reqStr));
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
