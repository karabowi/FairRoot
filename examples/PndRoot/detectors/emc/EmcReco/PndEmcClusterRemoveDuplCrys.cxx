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

#include "PndEmcClusterRemoveDuplCrys.h"
#include "PndEmcDigi.h"
#include "PndEmcCluster.h"

#include "FairRootManager.h"
#include "FairRunAna.h"

#include "TClonesArray.h"

using std::cerr;
using std::cout;
using std::endl;

PndEmcClusterRemoveDuplCrys::PndEmcClusterRemoveDuplCrys(Int_t verbose) : FairTask("PndEmcClusterRemoveDuplCrys", verbose) {}

InitStatus PndEmcClusterRemoveDuplCrys::Init()
{

  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman) {
    cout << "-E- PndEmcClusterRemoveDuplCrys::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get inputarray

  //  fClusterArray = FairRunAna::Instance()->IsTimeStamp() ? (TClonesArray *)ioman->GetObject("EmcClusterTemp") : (TClonesArray *)ioman->GetObject("EmcCluster");
  fClusterArray = (TClonesArray *)ioman->GetObject("EmcCluster");
  if (!fClusterArray) {
    cout << "-E- PndEmcClusterRemoveDuplCrys::Init: "
         << "No EmcCluster array!" << endl;
    return kFATAL;
  }

  // Get input/output array
  fDigiArray = FairRunAna::Instance()->IsTimeStamp() ? (TClonesArray *)ioman->GetObject("EmcDigiClusterBase") : (TClonesArray *)ioman->GetObject("EmcDigi");
  if (!fDigiArray) {
    cout << "-W- PndEmcClusterRemoveDuplCrys::Init: "
         << "No PndEmcDigi array!" << endl;
    return kERROR;
  }

  cout << "-I- PndEmcClusterRemoveDuplCrys: "
       << "Got both arrays" << endl;

  return kSUCCESS;
}

void PndEmcClusterRemoveDuplCrys::Exec(Option_t *)
{

  for (Int_t iClu = 0; iClu < fClusterArray->GetEntriesFast(); ++iClu) {
    PndEmcCluster *theCluster = static_cast<PndEmcCluster *>(fClusterArray->UncheckedAt(iClu));

    for (std::vector<Int_t>::reverse_iterator rit = theCluster->DigiList().rbegin(); rit != theCluster->DigiList().rend();) {

      PndEmcDigi *theDigi = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(*rit));
      Int_t detId = theDigi->GetDetectorId();

      // check wether other digi with same detector Id was already included in cluster
      //..we do a reverse iteration here, because only the first (i.e. with the smallest idx) digi out of all digis with same detId in fDigiArry will be seen in the MemberDigiMap
      //(cf PndEmcMakeCluster::addDigi...)

      Int_t tmp_pos = theCluster->MemberDigiMap().find(detId)->second;
      if (tmp_pos != *rit) { // in Member Map ..other digi is related to this detId...add digis up
        PndEmcDigi *oldDigi = static_cast<PndEmcDigi *>(fDigiArray->UncheckedAt(tmp_pos));
        oldDigi->AbsorbEnergy(*theDigi);

        // std::cerr << "Absorbing energy (ptr|detId): (" << oldDigi << " | "<< oldDigi->GetDetectorId() << ")" << std::flush;
        theCluster->removeDigi(fDigiArray, (++rit).base()); // remove theDigi from cluster
      } else {
        ++rit;
      }
    }
  }
}
