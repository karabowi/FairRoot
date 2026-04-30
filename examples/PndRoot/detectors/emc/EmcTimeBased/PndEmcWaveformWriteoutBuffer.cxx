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
 * PndEmcWaveformWriteoutBuffer.cxx
 */
#include <assert.h>
#include "PndEmcWaveformWriteoutBuffer.h"
#include "FairRunAna.h"
#include "FairEventHeader.h"

ClassImp(PndEmcWaveformWriteoutBuffer);

#include "PndEmcWaveform.h"
#include <map>
// std::map<Int_t, Int_t> CntMap;

PndEmcWaveformWriteoutBuffer::PndEmcWaveformWriteoutBuffer() : FairWriteoutBuffer() {}

PndEmcWaveformWriteoutBuffer::PndEmcWaveformWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance)
  : FairWriteoutBuffer(branchName, "PndEmcWaveform", folderName, persistance), fSaveToFile(false)
{
}
void PndEmcWaveformWriteoutBuffer::SaveToTree(Bool_t flag)
{
  fSaveToFile = flag;
  if (fSaveToFile) {
    fWave = new TFile("PndEmcWaveformWriteoutBuffer.root", "RECREATE");
    tWave = new TTree("wave", "wave");
    tWave->Branch("e", &fEnergy, "e/D");
    tWave->Branch("peak", &fPeak, "peak/D");
    tWave->Branch("mod", &fMod, "mod/I");
    tWave->Branch("xcor", &fXcor, "xcor/I");
    tWave->Branch("ycor", &fYcor, "ycor/I");
    // tWave->Branch("idx"    ,&fIdx     ,"idx/I");
    tWave->Branch("detid", &fDetID, "detid/I");
    tWave->Branch("dt", &fDiffTime, "dt/D");
    tWave->Branch("overlapt", &fOverlap, "overlapt/D");
    tWave->Branch("wpc", &fPileupCount, "wpc/I");
  }
}

PndEmcWaveformWriteoutBuffer::~PndEmcWaveformWriteoutBuffer() {}
void PndEmcWaveformWriteoutBuffer::Write()
{
  if (fSaveToFile) {
    TFile *oldFile = gFile;
    gFile = fWave;
    gFile->cd();
    tWave->Write();
    gFile = oldFile;
  }
}

void PndEmcWaveformWriteoutBuffer::AddNewDataToTClonesArray(FairTimeStamp *data)
{
  FairRootManager *ioman = FairRootManager::Instance();
  TClonesArray *myArray = ioman->GetTClonesArray(fBranchName);
  if (fVerbose > 1)
    std::cout << "Data Inserted: " << *(PndEmcWaveform *)(data) << std::endl;
  new ((*myArray)[myArray->GetEntries()]) PndEmcWaveform(*(PndEmcWaveform *)(data));
}

double PndEmcWaveformWriteoutBuffer::FindTimeForData(FairTimeStamp *data)
{
  std::map<PndEmcWaveform, double>::iterator it;
  // std::map<long, double>::iterator it;
  PndEmcWaveform &myData = *((PndEmcWaveform *)data);
  it = fData_map.find(myData);
  if (it == fData_map.end())
    return -1;
  else
    return it->second;
}
void PndEmcWaveformWriteoutBuffer::FillDataMap(FairTimeStamp *data, double activeTime)
{
  PndEmcWaveform &myData = *((PndEmcWaveform *)data);
  fData_map.insert(std::pair<PndEmcWaveform, double>(myData, activeTime));
}
void PndEmcWaveformWriteoutBuffer::EraseDataFromDataMap(FairTimeStamp *data)
{
  PndEmcWaveform &myData = *((PndEmcWaveform *)data);
  if (fData_map.find(myData) != fData_map.end())
    fData_map.erase(fData_map.find(myData));
}
std::vector<std::pair<double, FairTimeStamp *>> PndEmcWaveformWriteoutBuffer::Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData)
{
  assert(oldData.second);
  assert(newData.second);

  std::vector<std::pair<double, FairTimeStamp *>> result;
  std::pair<double, FairTimeStamp *> singleResult;
  /*if (newData.first > 0)
    singleResult.first += newData.first;*/

  PndEmcWaveform *oldWave = (PndEmcWaveform *)(oldData.second);
  PndEmcWaveform *newWave = (PndEmcWaveform *)(newData.second);
  //(*newWave) += (*oldWave);
  if (fVerbose > 1) {
    cout << "PndEmcWaveformWriteoutBuffer::Modify called #" << oldWave->GetDetectorId() << ", #" << newWave->GetDetectorId() << endl;
  }

  if (oldWave->GetTimeStamp() < newWave->GetTimeStamp()) {
    (*oldWave) += (*newWave);
    singleResult.second = oldData.second;
    singleResult.first = oldWave->GetActiveTime();

    if (fSaveToFile) {
      // fEnergy = theHit->GetEnergy();
      // fPeak = theWaveform->Max();
      fMod = oldWave->GetModule();
      fDetID = oldWave->GetDetectorId();
      fDiffTime = newWave->GetTimeStamp() - oldWave->GetTimeStamp();
      fOverlap = oldWave->GetActiveTime() - newWave->GetTimeStamp();
      PndEmcTwoCoordIndex *tci = oldWave->GetTCI();
      fXcor = tci->XCoord();
      fYcor = tci->YCoord();
      fPileupCount = oldWave->GetPileupCount();
      // fIdx = tci->Index();
      tWave->Fill();
      if (fPileupCount >= 1)
        WriteToFile(oldWave);
    }
  } else {
    (*newWave) += (*oldWave);
    singleResult.second = newData.second;
    singleResult.first = newWave->GetActiveTime();

    if (fSaveToFile) {
      fMod = newWave->GetModule();
      fDetID = newWave->GetDetectorId();
      fDiffTime = oldWave->GetTimeStamp() - newWave->GetTimeStamp();
      fOverlap = newWave->GetActiveTime() - oldWave->GetTimeStamp();
      PndEmcTwoCoordIndex *tci = newWave->GetTCI();
      fXcor = tci->XCoord();
      fYcor = tci->YCoord();
      fPileupCount = newWave->GetPileupCount();
      // fIdx = tci->Index();
      tWave->Fill();
      if (fPileupCount >= 1)
        WriteToFile(newWave);
    }
  }

  if (fVerbose > 0) {
    // if( CntMap[oldWave->GetDetectorId()] ++ >= 2)
    //	std::cout<<"DetectorID #"<<oldWave->GetDetectorId()<<" has "<<CntMap[oldWave->GetDetectorId()]<<" added"<<std::endl;
    // std::cout << "Modify PndEmcWaveform at DetectorID# "<<oldWave->GetDetectorId()<<", #"<<newWave->GetDetectorId() << std::endl;
    std::cout << "OldData: " << oldData.first << " : " << oldData.second << " NewData: " << newData.first << " : " << newData.second << std::endl;
    std::cout << "Resulting Data: " << singleResult.first << " : " << singleResult.second << std::endl;
  }

  result.push_back(singleResult);
  // FillDataMap(singleResult.second, singleResult.first);

  return result;
}
void PndEmcWaveformWriteoutBuffer::WriteToFile(PndEmcWaveform *theWaveform)
{
  TGraphErrors *gr = theWaveform->ToTGraph();
  TString name("evt");
  Int_t evtNo = FairRunAna::Instance()->GetEventHeader()->GetMCEntryNumber() - 1;
  name += evtNo;
  name += "_e";
  std::vector<Int_t> evtList = theWaveform->GetEvtList();
  for (size_t i = 0; i < evtList.size(); ++i) {
    name += evtList[i];
    name += "_";
  }
  name += theWaveform->GetDetectorId();
  gr->SetName(name);
  gr->SetTitle(name);
  TFile *oldFile = gFile;
  gFile = fWave;
  gFile->cd();
  gr->Write();
  oldFile->cd();
  delete gr;
}
/*void PndEmcWaveformWriteoutBuffer::FillDataToDeadTimeMap(FairTimeStamp* data, double activeTime)
  {
  if (fActivateBuffering) {
  typedef  std::multimap<double, FairTimeStamp*>::iterator DTMapIter;
  typedef  std::map<FairTimeStamp, double>::iterator DataMapIter;

  PndEmcWaveform* emc = (PndEmcWaveform*) data;

  std::cout<<"data timestamp#"<<data->GetTimeStamp()<<", activeTime#"<<activeTime<<std::endl;
  std::cout<<"detector ID#"<<emc->GetDetectorId()<<std::endl;
  std::cout<<"begin FindTimeForData"<<std::endl;
  double timeOfOldData = FindTimeForData(data);
  std::cout<<"end FindTimeForData"<<std::endl;
  if(timeOfOldData > -1) {        //if an older active data object is already present
  if (fVerbose > 1) {
  std::cout << " OldData found! " << std::endl;
  }
  if (fVerbose > 1) {
  std::cout << "New Data: " << activeTime << " : " << data << std::endl;
  }
  double currentdeadtime = timeOfOldData;
  std::cout<<"currentdeadtime #"<<currentdeadtime<<std::endl;
  FairTimeStamp* oldData(0);
  bool not_equal(true);
  for (DTMapIter it = fDeadTime_map.lower_bound(currentdeadtime); it != fDeadTime_map.upper_bound(currentdeadtime); it++) {
  oldData = it->second;
  if (fVerbose > 1) {
  std::cout << "Check Data: " << it->first << " : " << oldData << std::endl;
  PndEmcWaveform* old = (PndEmcWaveform*)oldData;
  PndEmcWaveform* New = (PndEmcWaveform*)data;
  std::cout << "Check Data: 2#" << old->GetDetectorId() << " : " << New->GetDetectorId() << std::endl;
  }
  if (oldData->equal(data)) {
  if (fVerbose > 1) {
  std::cout << " oldData == data " << std::endl;
  }
  if (fVerbose > 1) {
  std::cout << it->first << " : " << it->second << std::endl;
  }
  not_equal = false;
  std::cout<<"before fDeadTime_map.erase"<<std::endl;
  fDeadTime_map.erase(it);
  std::cout<<"before EraseDataFromDataMap"<<std::endl;
  EraseDataFromDataMap(oldData);
  std::cout<<"end EraseDataFromDataMap"<<std::endl;
  break;
  }
  }
  if(oldData == 0 || not_equal) return;
  std::vector<std::pair<double, FairTimeStamp*> > modifiedData = Modify(std::pair<double, FairTimeStamp*>(currentdeadtime, oldData), std::pair<double, FairTimeStamp*>(-1, data));
  for (int i = 0; i < modifiedData.size(); i++) {
  FillDataToDeadTimeMap(modifiedData[i].second, modifiedData[i].first);
  if (fVerbose > 1) {
  std::cout << i << " :Modified Data: " << modifiedData[i].first << " : " << modifiedData[i].second << std::endl;
  }
  }
  } else {
  if (fVerbose > 1) {
  std::cout << "-I- FairWriteoutBuffer::FillNewData Data Inserted: " << activeTime << " : ";
  data->Print();
  std::cout << std::endl;
  }
  fDeadTime_map.insert(std::pair<double, FairTimeStamp*>(activeTime, data));
  std::cout<<"begin FillDataMap"<<std::endl;
  FillDataMap(data, activeTime);
  std::cout<<"end FillDataMap"<<std::endl;
  }
  } else {
  AddNewDataToTClonesArray(data);
  }
  }*/
