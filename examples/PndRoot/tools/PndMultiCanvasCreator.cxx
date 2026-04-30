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



#include <PndMultiCanvasCreator.h>

#include <iostream>
#include <TMath.h>

#include <TStyle.h>
#include <TImageDump.h>

ClassImp(PndMultiCanvasCreator);

PndMultiCanvasCreator::PndMultiCanvasCreator()
{
  // TODO Auto-generated constructor stub
}

PndMultiCanvasCreator::~PndMultiCanvasCreator()
{
  // TODO Auto-generated destructor stub
}

void PndMultiCanvasCreator::SelectPad(int padNumber)
{
  int actualCanvas = TMath::Floor(static_cast<double>(padNumber) / fPicPerCan);
  int actualPad = (padNumber % fPicPerCan) + 1;
  //      std::cout << histoCount << " : " << actualCanvas << "/" << actualPad << std::endl;
  if (!(fCanvasses.size() > actualCanvas)) {
    //        std::cout << "New canvas" << std::endl;
    fCanvasses.push_back(createCanvas());
  }
  fCanvasses[actualCanvas]->cd(actualPad);
}

TCanvas *PndMultiCanvasCreator::createCanvas()
{
  double division = TMath::Sqrt(static_cast<double>(fPicPerCan));
  int first = ceil(division);
  int second = round(division);
  //  std::cout << "createCanvas: " << picpercan << " : " << first << "/" << second << std::endl;
  TCanvas *newCan = new TCanvas();
  if (fDimX != 0 && fDimY != 0) {
    newCan->SetCanvasSize(fDimX, fDimY);
  }
  newCan->Divide(first, second);
  return newCan;
}

void PndMultiCanvasCreator::SaveCanvasses()
{
  gStyle->SetImageScaling(3.0);

  TString path = fOutputDir + "/";
  TString filePrefix = fCanPrefix;

  for (int i = 0; i < fCanvasses.size(); i++) {
    TString fileName = filePrefix;
    fileName += i;
    fileName += ".";
    fileName += fOutputFormat;
    path = fOutputDir + "/";
    path += fileName;
    TImageDump *dump = new TImageDump(path.Data());
    fCanvasses[i]->Paint();
    dump->Close();
    fOutputFileNames.push_back(path);
  }
}
