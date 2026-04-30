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
 * PndMvdAnaRadDam.cxx
 *
 *  Created on: Feb 2, 2009
 *      Author: stockman
 */

#include "PndMvdAnaRadDam.h"
#include "TList.h"
#include "TCollection.h"
#include "TObject.h"

#include <iostream>

PndMvdAnaRadDam::PndMvdAnaRadDam() : fCountGoodFiles(0), fFileList(), fHistoMap()
{
  // TODO Auto-generated constructor stub
}

PndMvdAnaRadDam::~PndMvdAnaRadDam()
{
  // TODO Auto-generated destructor stub
}

void PndMvdAnaRadDam::AnalyzeFiles()
{
  for (unsigned int i = 0; i < fFileList.size(); i++) {
    TFile *f = fFileList[i];
    if (f != nullptr) {
      TList *l = f->GetListOfKeys();
      TIter iter(l);
      TObject *ob;
      fCountGoodFiles++;
      while ((ob = iter())) {
        if (TString(ob->GetName()).Contains("ocave_1oMvd-2.1")) {
          TString newName = ob->GetName();
          newName.Append("_sum");
          TProfile2D *histo = (TProfile2D *)(f->Get(ob->GetName()));
          AddHisto(histo);
        }
      }
    }
    //		f->Close();
  }
}

void PndMvdAnaRadDam::AddHisto(TProfile2D *histo)
{
  TString mapName = histo->GetName();
  mapName.Append("_sum");
  if (fHistoMap[mapName] == 0) {
    TProfile2D *newHisto = new TProfile2D(*histo);
    newHisto->SetName(mapName);
    fHistoMap[mapName] = newHisto;
  } else {
    // std::cout << fHistoMap[histo->GetName()]->GetName() << " / " << histo->GetName() << std::endl;
    fHistoMap[mapName]->Add(histo);
  }
  // std::cout << histo->GetName() << std::endl;
}

TH2 *PndMvdAnaRadDam::GetHisto(int i)
{
  std::map<TString, TProfile2D *>::iterator iter = fHistoMap.begin();
  for (int j = 0; j < i; j++)
    iter++;
  return iter->second;
}

TH2 *PndMvdAnaRadDam::GetHistoByName(TString name)
{
  return fHistoMap[name];
}

void PndMvdAnaRadDam::SaveHistos(TString fileName)
{
  TFile f(fileName, "RECREATE");
  // f.Dump();
  //	f.Write();

  for (std::map<TString, TProfile2D *>::const_iterator iter = fHistoMap.begin(); iter != fHistoMap.end(); iter++) {
    if (iter->second != 0) {
      iter->second->Write();
    }
  }
  f.Close();
}
