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
 * PndEmcWaveformWriteoutBuffer.h
 */

#ifndef PNDEMCWAVEFORMWRITEOUTBUFFER_H_
#define PNDEMCWAVEFORMWRITEOUTBUFFER_H_

#include "FairWriteoutBuffer.h"
#include "PndEmcWaveform.h"
#include "TFile.h"
#include "TTree.h"
class FairTimeStamp;

class PndEmcWaveformWriteoutBuffer : public FairWriteoutBuffer {

 public:
  PndEmcWaveformWriteoutBuffer();
  PndEmcWaveformWriteoutBuffer(TString branchName, TString folderName, Bool_t persistance);
  void AddNewDataToTClonesArray(FairTimeStamp *);

  virtual ~PndEmcWaveformWriteoutBuffer();

  virtual double FindTimeForData(FairTimeStamp *data);
  virtual void FillDataMap(FairTimeStamp *data, double activeTime);
  virtual void EraseDataFromDataMap(FairTimeStamp *data);

  virtual void SaveToTree(bool flag);
  virtual void Write();
  // virtual void FillDataToDeadTimeMap(FairTimeStamp* data, double activeTime);
  virtual std::vector<std::pair<double, FairTimeStamp *>> Modify(std::pair<double, FairTimeStamp *> oldData, std::pair<double, FairTimeStamp *> newData);

 protected:
  void WriteToFile(PndEmcWaveform *theWaveform);
  std::map<PndEmcWaveform, double> fData_map;
  // std::map<long, double> fData_map;

  ClassDef(PndEmcWaveformWriteoutBuffer, 1);

  Bool_t fSaveToFile;
  TFile *fWave;
  TTree *tWave;
  Int_t fMod;
  Double_t fEnergy;
  Double_t fPeak;
  Int_t fXcor;
  Int_t fYcor;
  Int_t fIdx;
  Int_t fDetID;
  Double_t fDiffTime;
  Double_t fOverlap;
  Int_t fPileupCount;
};

#endif /* PNDEMCWAVEFORMWRITEOUTBUFFER_H_ */
