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
 * PndMQFileSinkBursts.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef PndMQFileSinkBursts_H_
#define PndMQFileSinkBursts_H_

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
#include "FairParGenericSet.h"
#include "FairGeoParSet.h"

#include "PndMvdMQFileSamplerBursts.h"
#include "PndSdsPixelDigiPar.h"
#include "PndSdsTotDigiPar.h"
#include "PndSensorNamePar.h"

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
#include <boost/serialization/unique_ptr.hpp>
#endif //__CINT__

class TVector3;
class TFile;
class TTree;
class TClonesArray;

using namespace std;

class PndMQFileSinkBursts : public FairMQDevice {
 public:
  PndMQFileSinkBursts(std::string outputFileName = "FilesSinkBursts.root")
    : fOutFile(nullptr), fTree(nullptr), fOutput(nullptr), fHitVector(), fHasBoostSerialization(false), fOutputFileName(outputFileName), fBranchNameList(0), fCurrentRunId(0)
  {
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigTermination);

    // Check if boost serialization is available if it is chosen
    using namespace baseMQ::tools::resolve;
    // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
    if (is_same<boost::archive::binary_iarchive, boost::archive::binary_iarchive>::value || is_same<boost::archive::binary_iarchive, boost::archive::text_iarchive>::value) {
      if (has_BoostSerialization<FairTimeStamp, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1) {
        fHasBoostSerialization = true;
      }
    }

    fGeoPar = new FairGeoParSet("FairGeoParSet");
    fParCList = new TList();
    fParCList->Add(fGeoPar);

    fDigiPar = new PndSdsPixelDigiPar("MVDPixelDigiPar");
    fParCList->Add(fDigiPar);

    fTotPar = new PndSdsTotDigiPar("MVDPixelTotDigiPar");
    fParCList->Add(fTotPar);

    fSensorPar = new PndSensorNamePar("PndSensorNamePar");
    fParCList->Add(fSensorPar);
  }

  virtual ~PndMQFileSinkBursts()
  {
    // fBranchNameList->Write("BranchList", TObject::kSingleKey);
    // fTree->Write();
    // fOutFile->Close();
    if (fHitVector.size() > 0) {
      fHitVector.clear();
    }
  }

  virtual void SetFileName(std::string fileName) { fOutputFileName = fileName; }

  virtual void InitOutputFile(TString defaultId = "100")
  {
    //        fOutput = new TClonesArray("PndSdsHit");
    //
    //        std::stringstream ss;
    //      //  ss << "/Volumes/RAMDisk/";
    //        ss << fOutputFileName.c_str();
    //
    //
    //        fOutFile = new TFile(ss.str().c_str(), "recreate");
    //        fTree = new TTree("pndsim", "Test output");
    //        fTree->Branch("Output", "TClonesArray", &fOutput, 64000, 99);
    //        fBranchNameList = new TList();
    //        fBranchNameList->AddLast(new TObjString("Output"));
  }

  void UpdateParameters();
  FairParGenericSet *UpdateParameter(FairParGenericSet *thisPar);
  static void CustomCleanup(void *data, void *hint);

  template <class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar &fHitVector;
    ar &fBurstData;
  }

 protected:
  virtual void Run();

 private:
  TFile *fOutFile;
  TTree *fTree;
  TClonesArray *fOutput;
  TList *fBranchNameList;
  std::string fOutputFileName;
  BurstData fBurstData;
  int fCurrentRunId;
  int fNewRunId;
  TList *fParCList;
  FairGeoParSet *fGeoPar;
  PndSdsPixelDigiPar *fDigiPar;
  PndSdsTotDigiPar *fTotPar;
  PndSensorNamePar *fSensorPar;

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  vector<FairTimeStamp *> fHitVector;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  /// Copy Constructor
  //    PndMQFileSinkBursts(const PndMQFileSinkBursts&);
  //    PndMQFileSinkBursts operator=(const PndMQFileSinkBursts&);
};

#endif /* PndMQFileSinkBursts_H_ */
