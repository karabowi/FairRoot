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
 * PndMQFileSinkHits.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef PndMQFileSinkHits_H_
#define PndMQFileSinkHits_H_

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
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#endif //__CINT__

class TVector3;
class TFile;
class TTree;
class TClonesArray;

using namespace std;

class PndMQFileSinkHits : public FairMQDevice {
 public:
  PndMQFileSinkHits(std::string outputFileName = "FilesSinkHits.root")
    : fOutFile(nullptr), fTree(nullptr), fOutput(nullptr), fHitVector(), fHasBoostSerialization(false), fOutputFileName(outputFileName), fBranchNameList(0)
  {
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigTermination);

    // Check if boost serialization is available if it is chosen
    using namespace baseMQ::tools::resolve;
    // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
    if (is_same<boost::archive::binary_iarchive, boost::archive::binary_iarchive>::value || is_same<boost::archive::binary_iarchive, boost::archive::text_iarchive>::value) {
      if (has_BoostSerialization<PndSdsHit, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1) {
        fHasBoostSerialization = true;
      }
    }
  }

  virtual ~PndMQFileSinkHits()
  {
    fBranchNameList->Write("BranchList", TObject::kSingleKey);
    // fTree->Write();
    // fOutFile->Close();
    if (fHitVector.size() > 0) {
      fHitVector.clear();
    }
  }

  virtual void SetFileName(std::string fileName) { fOutputFileName = fileName; }

  virtual void InitOutputFile(TString defaultId = "100")
  {
    fOutput = new TClonesArray("PndSdsHit");

    std::stringstream ss;
    //  ss << "/Volumes/RAMDisk/";
    ss << fOutputFileName.c_str();

    fOutFile = new TFile(ss.str().c_str(), "recreate");
    fTree = new TTree("pndsim", "Test output");
    fTree->Branch("Output", "TClonesArray", &fOutput, 64000, 99);
    fBranchNameList = new TList();
    fBranchNameList->AddLast(new TObjString("Output"));
  }

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fHitVector;
  }

 protected:
  virtual void Run();

 private:
  TFile *fOutFile;
  TTree *fTree;
  TClonesArray *fOutput;
  TList *fBranchNameList;
  std::string fOutputFileName;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  vector<vector<PndSdsHit>> fHitVector;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  /// Copy Constructor
  //    PndMQFileSinkHits(const PndMQFileSinkHits&);
  //    PndMQFileSinkHits operator=(const PndMQFileSinkHits&);
};

#endif /* PndMQFileSinkHits_H_ */
