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

//-------------------------------------------------------//
//     Class PndMvdCreateDefaultApvMap                   //
//                                                       //
// Creates a default Frontend number mapping file        //
//                                                       //
// Created 16.02.2010                                    //
// Author: Ralf Kliemt                                   //
// ralf.kliemt{at}hiskp.uni-bonn.de                      //
//-------------------------------------------------------//

#include "PndMvdCreateDefaultApvMap.h"

#include <iostream>
#include <fstream>

#include "FairRun.h"
#include "FairRuntimeDb.h"
#include "PndMvdContFact.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TGeoMatrix.h"

ClassImp(PndMvdCreateDefaultApvMap);

// Constructor
PndMvdCreateDefaultApvMap::PndMvdCreateDefaultApvMap() : fSortedMapping(), fCurrentDigiPar(nullptr), fDigiParameterList(new TList()), fFeCount(0), fFakeCount(0) {}

// Destructor
PndMvdCreateDefaultApvMap::~PndMvdCreateDefaultApvMap()
{
  if (fDigiParameterList)
    delete fDigiParameterList;
}

// Setup
void PndMvdCreateDefaultApvMap::SetParContainers()
{
  FairRun *ana = FairRun::Instance();
  FairRuntimeDb *rtdb = ana->GetRuntimeDb();
  PndMvdContFact *themvdcontfact = (PndMvdContFact *)rtdb->getContFactory("PndMvdContFact");
  TList *theContNames = themvdcontfact->GetDigiParNames();
  Info("SetParContainers()", "The container names list contains %i entries", theContNames->GetEntries());
  TIter cfIter(theContNames);
  while (TObjString *contname = (TObjString *)cfIter()) {
    TString parsetname = contname->String();
    if (parsetname.BeginsWith("MVDStripDigiPar")) {
      PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)(rtdb->getContainer(parsetname.Data()));
      fDigiParameterList->Add(digipar);
      Info("SetParContainers()", "Add container %s", parsetname.Data());
    }
  }
}

InitStatus PndMvdCreateDefaultApvMap::Init()
{
  // Check for all necessary framework i/o
  if (!gGeoManager) {
    Error("Init", "Cannot find a valid GeoManager");
    return kERROR;
  }
  return kSUCCESS;
}

// The main function
Bool_t PndMvdCreateDefaultApvMap::CreateFile(TString outFileName)
{
  Info("CreateFile", "Output file name is %s", outFileName.Data());
  std::ofstream outfile(outFileName.Data(), std::ofstream::out);

  TString oldpath = gGeoManager->GetPath();
  gGeoManager->CdTop();
  if (!oldpath.BeginsWith(gGeoManager->GetPath()))
    oldpath = gGeoManager->GetPath();
  WriteExpandNode();
  gGeoManager->cd(oldpath.Data());

  // loop over sorted mapping and write to file
  for (std::map<Double_t, std::vector<Map_Entry>>::iterator iter = fSortedMapping.begin(); iter != fSortedMapping.end(); iter++) {
    std::vector<Map_Entry> entryset = iter->second;
    for (std::vector<Map_Entry>::iterator iter2 = entryset.begin(); iter2 != entryset.end(); iter2++) {
      Map_Entry anEntry = *iter2;
      TString detpath = anEntry.first;
      std::pair<Int_t, Int_t> aPair = anEntry.second;
      Int_t feSoft = aPair.first;
      Int_t feHard = aPair.second;
      outfile << feHard << " " << feSoft << " " << detpath << std::endl;
      std::cout << feHard << " " << feSoft << " " << detpath << std::endl;
    }
  }

  outfile.close();
  return kTRUE;
}

void PndMvdCreateDefaultApvMap::WriteExpandNode()
{
  // this volume should be an assambly
  TGeoNode *node = gGeoManager->GetCurrentNode();
  // TGeoVolume* vol = node->GetVolume();
  if (node->GetNdaughters() > 0) { // iterate to daughters
    for (Int_t Nod = 0; Nod < node->GetNdaughters(); Nod++) {
      gGeoManager->CdDown(Nod);
      WriteExpandNode();
      gGeoManager->CdUp();
    }
  } else { // no daughter nodes
    TString detpath = gGeoManager->GetPath();
    // some checks
    if (!detpath.Contains("Strip"))
      return;
    if (!SelectSensorParams(detpath))
      return;
    // Chosen sorting parameter: z Position of the node.
    // TODO: Is it done correctly like this?
    TGeoTranslation *tran = (TGeoTranslation *)node->GetMatrix();
    Double_t sortparam = (tran->GetTranslation())[2];
    Int_t feSens = fCurrentDigiPar->GetNrTopFE() + fCurrentDigiPar->GetNrBotFE();
    for (Int_t fe = 0; fe < feSens; fe++) { // write to mapping
      Int_t feNr;
      // count only real-world existing frontends, others are -1
      if (fCurrentDigiPar->GetNrBotFE() == 1 && fe >= fCurrentDigiPar->GetNrTopFE()) {
        fFakeCount--;      // decrease first, starting at -1!
        feNr = fFakeCount; // handle the single sided sensors with a fake bittom side
      } else {
        feNr = fFeCount;
        fFeCount++; // increase later, starting at 0
      }
      std::pair<Int_t, Int_t> apair(fe, feNr);
      Map_Entry anEntry(detpath, apair);
      (fSortedMapping[sortparam]).push_back(anEntry);
    }
  }
  return;
}

Bool_t PndMvdCreateDefaultApvMap::SelectSensorParams(TString detpath)
{
  TIter parsetiter(fDigiParameterList);
  while (PndSdsStripDigiPar *digipar = (PndSdsStripDigiPar *)parsetiter()) {
    const char *sensortype = digipar->GetSensType();
    if (detpath.Contains(sensortype)) {
      fCurrentDigiPar = digipar;
      Info("SelectSensorParams", "Found a parameter set for %s", detpath.Data());
      return kTRUE;
    }
  }
  // no suiting object found
  Error("SelectSensorParams", "Detector name does not contain a valid parameter name. DetName : %s", detpath.Data());
  return kFALSE;
}

// TString PndMvdCreateDefaultApvMap::FindNodePath(TGeoNode* node)
//{
//  // Find a nodes full path by going there in the gGeoManager
//  // With many volumes this becomes surely slow.
//  const char* oldpath = gGeoManager->GetPath();
//  gGeoManager->CdTop(); // dive down from top node
//  DiveDownToNode(node);
//  TString pathname = gGeoManager->GetPath();
//  gGeoManager->cd(oldpath);
//  return pathname;
//}
//
// void PndMvdCreateDefaultApvMap::DiveDownToNode(TGeoNode* node)
//{
//  // cd gGeoManager from the current node to a given node
//  TGeoNode *currentNode = gGeoManager->GetCurrentNode();
//  if (currentNode == node) return;
//  for (Int_t iNod=0; iNod<currentNode->GetNdaughters();iNod++)
//  {
//    gGeoManager->CdDown(iNod);
//    DiveDownToNode(node);
//    if (gGeoManager->GetCurrentNode() == node) return;
//    gGeoManager->CdUp();
//  }
//}
