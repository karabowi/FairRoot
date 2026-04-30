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

/* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemEmpty source file         -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** PndGemEmpty
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15.02.2009
 *@version 1.0
 **
 ** PANDA task class to do nothing
 **/

#include "PndGemEmpty.h"

#include "PndGemCluster.h"
#include "PndGemDigi.h"
#include "PndGemDigiPar.h"
#include "PndGemSensor.h"
#include "PndGemStation.h"

#include "PndDetectorList.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairLogger.h"

#include "TClonesArray.h"
#include "TMath.h"
#include "TRandom.h"

#include <iomanip>

using std::cerr;
using std::cout;
using std::endl;
using std::fixed;
using std::flush;
using std::left;
using std::map;
using std::right;
using std::set;
using std::setprecision;
using std::setw;

// -----   Default constructor   ------------------------------------------
PndGemEmpty::PndGemEmpty() : FairTask("GEMEmpty", 0), fFunctor(nullptr), fInBranch(nullptr), fOutBranch(nullptr), fInBranchName("GEMDigi") {}
// -------------------------------------------------------------------------

// -----   Standard constructor   ------------------------------------------
PndGemEmpty::PndGemEmpty(Int_t iVerbose) : FairTask("GEMEmpty", iVerbose), fFunctor(nullptr), fInBranch(nullptr), fOutBranch(nullptr), fInBranchName("GEMDigi") {}
// -------------------------------------------------------------------------

// -----   Constructor with name   -----------------------------------------
PndGemEmpty::PndGemEmpty(const char *name, Int_t iVerbose) : FairTask(name, iVerbose), fFunctor(nullptr), fInBranch(nullptr), fOutBranch(nullptr), fInBranchName("GEMDigi") {}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
PndGemEmpty::~PndGemEmpty()
{
  if (nullptr != fOutBranch) {
    fOutBranch->Delete();
    delete fOutBranch;
  }
  if (0 != fFunctor)
    delete fFunctor;
}
// -------------------------------------------------------------------------

// -----   Public method Exec   --------------------------------------------
void PndGemEmpty::Exec(Option_t *)
{
  //  cout << endl << "======== PndGemEmpty::Exec(Event = " << fTNofEvents << " ) ====================" << endl;

  if (fInBranchName.Contains("Sorted")) {
    fInBranch->Clear();
    fInBranch = FairRootManager::Instance()->GetData(fInBranchName, fFunctor, 20.); // FairRootManager::Instance()->GetEventTime() +
  } else
    fInBranch = (TClonesArray *)FairRootManager::Instance()->GetObject(fInBranchName);

  fOutBranch = FairRootManager::Instance()->GetTClonesArray("GEMCluster");
  if (!fOutBranch)
    Fatal("Exec", "No GEM Cluster Array");
  fOutBranch->Delete();

  std::vector<Int_t> clusterRefs;
  // PndGemCluster* cluster = nullptr;
  for (Int_t iout = 0; iout < fInBranch->GetEntries() / 14; iout++) {
    new ((*fOutBranch)[iout]) PndGemCluster(585180824, iout, 0, iout * 2., 100, 0, clusterRefs);
  }

  LOG(info) << " PndGemEmpty::Exec. There are " << fInBranch->GetEntries() << " objects of " << fInBranchName.Data();
}
// -------------------------------------------------------------------------

// -----   Private method SetParContainers   -------------------------------
void PndGemEmpty::SetParContainers()
{

  // Get run and runtime database
  FairRunAna *run = FairRunAna::Instance();
  if (!run)
    LOG(fatal) << "SetParContainers: No analysis run";

  FairRuntimeDb *db = run->GetRuntimeDb();
  if (!db)
    LOG(fatal) << "SetParContainers: No runtime database";
}
// -------------------------------------------------------------------------

// -----   Private method Init   -------------------------------------------
InitStatus PndGemEmpty::Init()
{

  // Get input array
  FairRootManager *ioman = FairRootManager::Instance();
  if (!ioman)
    Fatal("Init", "No FairRootManager");
  fInBranch = (TClonesArray *)ioman->GetObject(fInBranchName.Data());
  fOutBranch = ioman->Register("GEMCluster", "PndGemCluster", "PndGEM", kTRUE);

  fFunctor = new TimeGap();

  LOG(info) << " " << fName.Data() << "::Init(). Initialization succesfull.";

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Private method ReInit   -----------------------------------------
InitStatus PndGemEmpty::ReInit()
{

  // Create sectorwise digi sets
  //  MakeSets();

  return kSUCCESS;
}
// -------------------------------------------------------------------------

// -----   Public method Finish   ------------------------------------------
void PndGemEmpty::Finish()
{
  if (fOutBranch)
    fOutBranch->Clear();

  cout << "-------------------- " << fName.Data() << " : Summary -----------------------" << endl;
  cout << "-- DONE -- " << endl;
  cout << "---------------------------------------------------------------------" << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndGemEmpty)
