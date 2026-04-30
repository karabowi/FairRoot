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

#ifndef PndMvdReadInTBDataTask_H_
#define PndMvdReadInTBDataTask_H_

#include "FairTask.h"
#include "PndGeoHandling.h"
#include "PndMvdReadInToPix4TBData.h"

#include <string>
#include <fstream>
#include <vector>

class PndMvdReadInTBDataTask : public FairTask {
 public:
  PndMvdReadInTBDataTask();
  virtual ~PndMvdReadInTBDataTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();

  void SetFrequency(Double_t val) { fClockFrequency = val; }
  void SetNumberOfFrontEnds(Int_t numberfe);

  virtual void AddFile(Int_t asic, std::string fileName)
  {
    std::cout << "File Added to asic " << asic - 1 << " fileName " << fileName << std::endl;
    if (fFileNames.size() < asic) {
      fFileNames.resize(asic);
    }
    fFileNames[asic - 1].push_back(fileName);
  };

 protected:
  void WriteoutToPix4Digi(PndSdsDigiTopix4 &data);
  void WriteoutToPix4Frames(std::vector<std::vector<PndSdsDigiTopix4>> &frames);

 private:
  std::vector<std::vector<std::string>> fFileNames;

  // std::vector<TString> fFileNames;
  std::vector<std::ifstream *> fFileHandles;
  std::vector<PndMvdReadInToPix4TBData *> fReader;
  std::vector<Bool_t> fEndOfFile;

  UInt_t fEvent;

  TClonesArray *fDigiArray;
  TClonesArray *fFrameHeaderArray;
  TClonesArray *fAllFrameHeaderArray;

  Bool_t fInitDone;
  Double_t fClockFrequency; // in MHz

  void Register();
  void Reset();

  ClassDef(PndMvdReadInTBDataTask, 1);
};

#endif /*PndMvdReadInTBDataTask_H_*/
