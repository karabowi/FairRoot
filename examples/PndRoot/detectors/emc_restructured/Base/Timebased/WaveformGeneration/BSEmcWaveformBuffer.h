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
 * BSEmcWaveformBuffer.h
 */

#ifndef BSEMCWAVEFORMBUFFER_HH
#define BSEMCWAVEFORMBUFFER_HH

#include <map>
#include <utility>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TString.h"

#include "FairWriteoutBuffer.h"

#include "BSEmcWaveformData.h"

class FairTimeStamp;
class BSEmcWaveformData;
class TClonesArray;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief buffer for waveforms, used by BSEmcFwEndcapTimebasedWaveforms
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @ingroup EmcBase
 */
class BSEmcWaveformBuffer : public FairWriteoutBuffer {
 public:
  BSEmcWaveformBuffer();
  BSEmcWaveformBuffer(TString t_branchName, TString t_className, TString t_folderName, Bool_t t_persistance);

  virtual ~BSEmcWaveformBuffer();

  virtual void AddNewDataToTClonesArray(FairTimeStamp * /*data*/) /*override*/;
  virtual Double_t FindTimeForData(FairTimeStamp *t_data) /*override*/;
  virtual void FillDataMap(FairTimeStamp *t_data, Double_t t_activeTime) /*override*/;
  virtual void EraseDataFromDataMap(FairTimeStamp *t_data) /*override*/;

  virtual void DeleteOldData() /*override*/;
  virtual void StoreWaveformData(TString t_branchName, TString t_folderName, Bool_t t_persistance) /*override*/;

  virtual void FillNewData(BSEmcWaveformData *) /*override*/;

 private:
  // don't allow copying (-Weffc++)
  BSEmcWaveformBuffer(const BSEmcWaveformBuffer &);            // no implementation
  BSEmcWaveformBuffer &operator=(const BSEmcWaveformBuffer &); // no implementation

 protected:
  std::vector<std::pair<Double_t, FairTimeStamp *>> Modify(std::pair<Double_t, FairTimeStamp *> t_oldData, std::pair<Double_t, FairTimeStamp *> t_newData);

  std::map<BSEmcWaveformData, Double_t> fData_map{};
  Bool_t fStoreWaveformData{kFALSE};
  TClonesArray *fWfDataArray{nullptr};
  TString fWfDataBranchName{""};

  ClassDef(BSEmcWaveformBuffer, 0);
};

#endif /*BSEMCWAVEFORMBUFFER_HH*/
