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
 * PndMQFileSink.h
 *
 * @since 2013-06-05
 * @author A. Rybalchenko
 */

#ifndef PndMQFileSink_H_
#define PndMQFileSink_H_

#include <iostream>

#include "Rtypes.h"
#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TVector3.h"
#include "TString.h"
#include "TSystem.h"

#include "FairMQDevice.h"
#include "FairMQLogger.h"

#include "PndSdsDigiTopix4.h"

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

class PndMQFileSink : public FairMQDevice {
 public:
  PndMQFileSink() : fOutFile(nullptr), fTree(nullptr), fOutput(nullptr), fHitVector(), fHasBoostSerialization(false)
  {
    gSystem->ResetSignal(kSigInterrupt);
    gSystem->ResetSignal(kSigTermination);

    // Check if boost serialization is available if it is chosen
    using namespace baseMQ::tools::resolve;
    // coverity[pointless_expression]: suppress coverity warnings on apparant if(const).
    if (is_same<boost::archive::binary_iarchive, boost::archive::binary_iarchive>::value || is_same<boost::archive::binary_iarchive, boost::archive::text_iarchive>::value) {
      if (has_BoostSerialization<PndSdsDigiTopix4, void(boost::archive::binary_iarchive &, const unsigned int)>::value == 1) {
        fHasBoostSerialization = true;
      }
    }
  }

  virtual ~PndMQFileSink()
  {
    fTree->Write();
    fOutFile->Close();
    if (fHitVector.size() > 0) {
      fHitVector.clear();
    }
  }

  virtual void InitOutputFile(TString defaultId = "100")
  {
    fOutput = new TClonesArray("PndSdsDigiTopix4");

    fOutFile = new TFile("Filesink1.root", "recreate");
    fTree = new TTree("MQOut", "Test output");
    fTree->Branch("Output", "TClonesArray", &fOutput, 64000, 99);
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

#ifndef __CINT__ // for BOOST serialization
  friend class boost::serialization::access;
  vector<PndSdsDigiTopix4> fHitVector;
  bool fHasBoostSerialization;
#endif // for BOOST serialization

  /// Copy Constructor
  //    PndMQFileSink(const PndMQFileSink&);
  //    PndMQFileSink operator=(const PndMQFileSink&);
};

#endif /* PndMQFileSink_H_ */
