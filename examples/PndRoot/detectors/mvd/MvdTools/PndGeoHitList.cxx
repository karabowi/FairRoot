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

#include "PndGeoHitList.h"
#include <sstream>
#include "FairLogger.h"

ClassImp(PndGeoHitList);

PndGeoHitList::PndGeoHitList() : fGroupName(""), fVolName(""), fNodeList(), fGroupVolume(nullptr), fTopVolume(nullptr), fBaseVolume(nullptr) {}

PndGeoHitList::PndGeoHitList(TString groupName, TString topNodeName, TGeoVolume *baseVolume)
  : fGroupName(""), fVolName(""), fNodeList(), fGroupVolume(nullptr), fTopVolume(nullptr), fBaseVolume(nullptr)
{
  CreateNewGroup(groupName, topNodeName, baseVolume);
}

PndGeoHitList::~PndGeoHitList() {}

void PndGeoHitList::CreateNewGroup(TString groupName, TString topNodeName, TGeoVolume *baseVolume)
{
  fGroupName = groupName;
  fBaseVolume = baseVolume;
  fVolName = baseVolume->GetName();

  fGroupVolume = new TGeoVolumeAssembly(fGroupName.Data());
  fTopVolume = gGeoManager->GetVolume(topNodeName.Data());
  fTopVolume->AddNode(fGroupVolume, 1);

  std::cout << "GroupName: " << fGroupName << " " << fGroupVolume << std::endl;
  std::cout << "BaseVolume: " << fVolName << " " << fBaseVolume << std::endl;
  std::cout << "TopVolum: " << topNodeName << " " << fTopVolume << std::endl;
}

void PndGeoHitList::AddHit(FairHit *hit, Bool_t vis)
{
  AddHit(hit->GetX(), hit->GetY(), hit->GetZ(), vis);
}

void PndGeoHitList::AddHit(Double_t x, Double_t y, Double_t z, Bool_t vis)
{
  CreateGeoHits(1);
  TGeoTranslation *tr = (TGeoTranslation *)fNodeList[fNodeList.size() - 1]->GetMatrix();
  tr->SetTranslation(x, y, z);
  fNodeList[fNodeList.size() - 1]->SetVisibility(vis);
}

void PndGeoHitList::AddHit(TGeoMatrix *mat, Bool_t vis)
{
  CreateGeoHit(mat);
  fNodeList[fNodeList.size() - 1]->SetVisibility(vis);
}

void PndGeoHitList::AddHits(TClonesArray *hitList, Bool_t vis)
{
  Int_t start = fNodeList.size();
  CreateGeoHits(hitList->GetEntries());

  for (Int_t i = 0; i < hitList->GetEntries(); i++) {
    FairHit *myHit = (FairHit *)hitList->At(i);
    TGeoTranslation *tr = (TGeoTranslation *)fNodeList[start + i]->GetMatrix();
    tr->SetTranslation(myHit->GetX(), myHit->GetY(), myHit->GetZ());
    fNodeList[start + i]->SetVisibility(vis);
  }
}

void PndGeoHitList::SetHits(TClonesArray *hitList, Bool_t vis)
{
  if ((unsigned int)hitList->GetEntries() > fNodeList.size()) {
    CreateGeoHits(hitList->GetEntries() - fNodeList.size());
    std::cout << "hitList: " << hitList->GetEntries() << "fNodeList: " << fNodeList.size() << std::endl;
  } else
    RemoveGeoHits(hitList->GetEntries());

  for (Int_t i = 0; i < hitList->GetEntries(); i++) {
    FairHit *myHit = (FairHit *)hitList->At(i);
    TGeoTranslation *tr = (TGeoTranslation *)fNodeList[i]->GetMatrix();
    tr->SetTranslation(myHit->GetX(), myHit->GetY(), myHit->GetZ());
    fNodeList[i]->SetVisibility(vis);
  }
}

void PndGeoHitList::CreateGeoHit(TGeoMatrix *mat)
{
  TGeoHMatrix *matCopy = new TGeoHMatrix(*mat);
  Int_t startNumber = fNodeList.size();
  std::string volumeName = fVolName.Data();
  std::stringstream nodeName;
  matCopy->RegisterYourself();
  fGroupVolume->AddNode(fBaseVolume, startNumber, matCopy);
  nodeName << volumeName << "_" << startNumber;
  TGeoNode *myNode = fGroupVolume->FindNode(nodeName.str().c_str());
  std::cout << "Node: " << nodeName.str() << " " << myNode << std::endl;
  fNodeList.push_back(myNode);
  nodeName.str("");
}

void PndGeoHitList::CreateGeoHits(Int_t number)
{
  /*  Int_t startNumber = fNodeList.size();
    std::string volumeName = fVolName.Data();
    std::stringstream nodeName;
    std::cout << "StartNumber: " << fNodeList.size() << " anz: " << number << std::endl;
    for (Int_t i = 0; i < number; i++){
      TGeoTranslation *tr = new TGeoTranslation(1,1,i);
      fGroupVolume->AddNode(fBaseVolume,startNumber+i, tr);
      nodeName << volumeName << "_" << startNumber+i;
      TGeoNode* myNode = fGroupVolume->FindNode(nodeName.str().c_str());
      std::cout << "Node: " << nodeName.str() << " " << myNode << std::endl;
      fNodeList.push_back(myNode);
      nodeName.str("");
    }*/

  for (Int_t i = 0; i < number; i++) {
    TGeoTranslation *tr = new TGeoTranslation(1, 1, i);
    CreateGeoHit(tr);
  }
}

void PndGeoHitList::RemoveGeoHits(Int_t from)
{
  std::cout << "Old RecoList size: " << fNodeList.size() << std::endl;
  for (UInt_t i = from; i < fNodeList.size(); i++) {
    fGroupVolume->RemoveNode(fNodeList[i]);
  }
  fNodeList.erase(fNodeList.begin() + from, fNodeList.end());

  std::cout << "New RecoList size: " << fNodeList.size() << std::endl;
}

void PndGeoHitList::SetGeoHitsInvisible(Int_t from)
{
  for (UInt_t i = from; i < fNodeList.size(); i++)
    fNodeList[i]->SetVisibility(kFALSE);
}

void PndGeoHitList::SetVisibility(TString NodeName, bool value)
{
  Int_t id = GetNodeId(NodeName);
  if (id < 0)
    LOG(error) << " PndGeoHitList: Not a valid NodeName!";
  else
    SetVisibility(id, value);
}

void PndGeoHitList::SetVisibility(TGeoHMatrix pos, bool value)
{
  Int_t id = GetNodeId(pos);
  if (id < 0)
    LOG(error) << " PndGeoHitList: Not a valid NodeName!";
  else
    SetVisibility(id, value);
}

void PndGeoHitList::SetVisibility(Int_t id, bool value)
{
  if ((unsigned int)id < fNodeList.size())
    fNodeList[id]->SetVisibility(value);
  else
    LOG(error) << " PndGeoHitList: Not a valid id!";
}

void PndGeoHitList::SetAllInvisible()
{
  SetGeoHitsInvisible(0);
}

Int_t PndGeoHitList::GetNodeId(TString NodeName)
{
  for (UInt_t i = 0; i < fNodeList.size(); i++) {
    std::cout << "NodeNames: " << fNodeList[i]->GetName() << std::endl;
    if (fNodeList[i]->GetName() == NodeName)
      return i;
  }
  return -1;
}

Int_t PndGeoHitList::GetNodeId(TGeoHMatrix pos)
{
  //  std::cout << "pos: " << std::endl;
  //  pos.Print();

  for (UInt_t i = 0; i < fNodeList.size(); i++) {
    //    std::cout << "NodeMatrix: " << std::endl;
    //    fNodeList[i]->GetMatrix()->Print();

    if (pos == *((TGeoHMatrix *)fNodeList[i]->GetMatrix()))
      return i;
  }
  return -1;
}
