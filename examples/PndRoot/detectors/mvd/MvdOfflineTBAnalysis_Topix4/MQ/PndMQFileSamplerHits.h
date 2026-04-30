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
 * PndMQFileSamplerHits.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef PndMQFileSamplerHits_H_
#define PndMQFileSamplerHits_H_

#include <iostream>

#include "Rtypes.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"
#include "TSystem.h"
#include "TList.h"
#include "TObjString.h"

#include "FairMQDevice.h"
#include "FairMQLogger.h"

#include "PndSdsHit.h"

#include "baseMQtools.h"

#include "TMessage.h"

#ifndef __CINT__
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/serialization/access.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#endif //__CINT__

class TVector3;
class TFile;
class TTree;
class TClonesArray;

using namespace std;

class PndMQFileSamplerHits : public FairMQDevice {
 public:
 public:
  enum { InputFile = FairMQDevice::Last, Last };

  PndMQFileSamplerHits(std::string inputFileName = "")
    : fInFile(nullptr), fTree(nullptr), fInput(nullptr), fHitVector(), fHasBoostSerialization(false), fInputFileName(inputFileName), fBranchNameList(0)
  {
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigTermination);

    // Check if boost serialization is available if it is chosen
    using namespace baseMQ::tools::resolve;
    // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
    if (is_same<boost::archive::binary_oarchive, boost::archive::binary_oarchive>::value) {
      if (has_BoostSerialization<PndSdsHit, void(boost::archive::binary_oarchive &, const unsigned int)>::value == 1) {
        fHasBoostSerialization = true;
      }
    }
  }

  virtual ~PndMQFileSamplerHits()
  {
    // fBranchNameList->Write("BranchList", TObject::kSingleKey);
    // fTree->Write();
    fInFile->Close();
    if (fHitVector.size() > 0) {
      fHitVector.clear();
    }
  }

  virtual void SetFileName(std::string fileName) { fInputFileName = fileName; }

  virtual void InitInputFile()
  {
    fInput = new TClonesArray("PndSdsHit");

    fInFile = new TFile(fInputFileName.c_str(), "read");
    fTree = (TTree *)fInFile->Get("pndsim");
    fTree->SetBranchAddress("Output", &fInput);
  }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fHitVector;
  }

  void SetProperty(const int key, const std::string &value)
  {
    switch (key) {
    case InputFile: fInputFileName = value; break;
    default: FairMQDevice::SetProperty(key, value); break;
    }
  }

  std::string GetProperty(const int key, const std::string &default_ /*= ""*/)
  {
    switch (key) {
    case InputFile: return fInputFileName;
    default: return FairMQDevice::GetProperty(key, default_);
    }
  }

 protected:
  virtual void Run();

 private:
  TFile *fInFile;
  TTree *fTree;
  TClonesArray *fInput;
  TList *fBranchNameList;
  std::string fInputFileName;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  //    vector<PndSdsHit> fHitVector;
  vector<vector<PndSdsHit>> fHitVector;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  /// Copy Constructor
  //    PndMQFileSamplerHits(const PndMQFileSamplerHits&);
  //    PndMQFileSamplerHits operator=(const PndMQFileSamplerHits&);
};

#endif /* PndMQFileSamplerHits_H_ */
