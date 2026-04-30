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
 * PndMvdMQFileSink.cxx
 *
 * @since 2016-03-08
 * @author R. Karabowicz
 */

#include <PndMvdMQFileSink.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "FairMQLogger.h"

#include "TMessage.h"

using namespace std;

// special class to expose protected TMessage constructor
class Ex9TMessage : public TMessage {
 public:
  Ex9TMessage(void *buf, Int_t len) : TMessage(buf, len) { ResetBit(kIsOwner); }
};

PndMvdMQFileSink::PndMvdMQFileSink()
  : FairMQDevice(), fFileName(), fTreeName()

    ,
    fBranchNames(), fClassNames(), fFileOption(), fFlowMode(false), fWrite(false)

    ,
    fOutFile(nullptr), fTree(nullptr), fNObjects(0), fOutputObjects(new TObject *[1000]), fFolder(nullptr)
{
}

void PndMvdMQFileSink::Init()
{
  LOG(info) << "SHOULD CREATE THE FILE AND TREE";
  //  fFileName = "/Users/karabowi/fairroot/pixel9_dev/FairRoot/examples/MQ/9-PixelDetector/macros/tmpOut.root";
  fFileOption = "RECREATE";
  fTreeName = "pndsim";

  // fBranchNames.push_back("EventHeader.");
  // fClassNames .push_back("FairEventHeader");
  // fBranchNames.push_back("PixelHits");
  // fClassNames .push_back("TClonesArray(PixelHit)");

  fOutFile = TFile::Open(fFileName.c_str(), fFileOption.c_str());

  fTree = new TTree(fTreeName.c_str(), "/cbmout");

  fFolder = new TFolder("cbmout", "Main Output Folder");
  TFolder *foldEventHeader = fFolder->AddFolder("EvtHeader", "EvtHeader");
  TFolder *foldPixel = fFolder->AddFolder("Pixel", "Pixel");

  TList *BranchNameList = new TList();

  LOG(info) << "PndMvdMQFileSink: " << fBranchNames.size();

  for (fNObjects = 0; fNObjects < fBranchNames.size(); fNObjects++) {
    if (fClassNames[fNObjects].find("TClonesArray(") == 0) {
      fClassNames[fNObjects] = fClassNames[fNObjects].substr(13, fClassNames[fNObjects].length() - 12 - 2);
      fOutputObjects[fNObjects] = new TClonesArray(fClassNames[fNObjects].c_str());
      fTree->Branch(fBranchNames[fNObjects].c_str(), "TClonesArray", &fOutputObjects[fNObjects]);
      foldPixel->Add(fOutputObjects[fNObjects]);
      LOG(info) << fNObjects << " : " << fBranchNames[fNObjects];
      BranchNameList->AddLast(new TObjString(fBranchNames[fNObjects].c_str()));
    } else if (fClassNames[fNObjects].find("FairEventHeader") == 0) {
      fOutputObjects[fNObjects] = new FairEventHeader();
      fTree->Branch(fBranchNames[fNObjects].c_str(), "FairEventHeader", &fOutputObjects[fNObjects]);
      foldEventHeader->Add(fOutputObjects[fNObjects]);
      BranchNameList->AddLast(new TObjString(fBranchNames[fNObjects].c_str()));
    } else {
      LOG(error) << "!!! Unknown output object \"" << fClassNames[fNObjects] << "\" !!!";
    }
  }

  fFolder->Write();
  BranchNameList->Write("BranchList", TObject::kSingleKey);
  BranchNameList->Delete();
  delete BranchNameList;
}

void PndMvdMQFileSink::Run()
{
  while (CheckCurrentState(RUNNING)) {
    FairMQParts parts;

    if (Receive(parts, "data-in") >= 0) {

      TObject *tempObjects[10];
      for (int ipart = 0; ipart < parts.Size(); ipart++) {
        Ex9TMessage tm(parts.At(ipart)->GetData(), parts.At(ipart)->GetSize());
        tempObjects[ipart] = (TObject *)tm.ReadObject(tm.GetClass());
        for (unsigned int ibr = 0; ibr < fBranchNames.size(); ibr++) {
          if (strcmp(tempObjects[ipart]->GetName(), fBranchNames[ibr].c_str()) == 0) {
            fOutputObjects[ibr] = tempObjects[ipart];
            fTree->SetBranchAddress(fBranchNames[ibr].c_str(), &fOutputObjects[ibr]);
          }
        }
      }
      fTree->Fill();
    } else {
      LOG(info) << "oops!";
    }
  }
}

void PndMvdMQFileSink::SetProperty(const int key, const std::string &value)
{
  switch (key) {
  case OutputFileName: SetOutputFileName(value); break;

  default: FairMQDevice::SetProperty(key, value); break;
  }
}

void PndMvdMQFileSink::SetProperty(const int key, const int value)
{
  FairMQDevice::SetProperty(key, value);
}

std::string PndMvdMQFileSink::GetProperty(const int key, const std::string &default_)
{
  switch (key) {
  case OutputFileName: return GetOutputFileName();

  default: return FairMQDevice::GetProperty(key, default_);
  }
}

int PndMvdMQFileSink::GetProperty(const int key, const int value)
{
  return FairMQDevice::GetProperty(key, value);
}

PndMvdMQFileSink::~PndMvdMQFileSink()
{
  if (fTree) {
    fTree->Write();
    delete fTree;
  }

  if (fOutFile) {
    if (fOutFile->IsOpen())
      fOutFile->Close();
    delete fOutFile;
  }
}
