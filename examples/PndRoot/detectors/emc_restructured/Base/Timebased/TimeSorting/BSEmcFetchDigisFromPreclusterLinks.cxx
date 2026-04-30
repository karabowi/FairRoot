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

#include "BSEmcFetchDigisFromPreclusterLinks.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

#include "RtypesCore.h"
#include "TObject.h"
#include "TString.h"
#include "TVector3.h"

#include "FairLink.h"
#include "FairMultiLinkedData.h"
#include "FairRootManager.h"
#include "fairlogger/Logger.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"

#include "BSEmcDigi.h"
#include "BSEmcPrecluster.h"

#include "PndParameterRegister.h"

BSEmcFetchDigisFromPreclusterLinks::BSEmcFetchDigisFromPreclusterLinks(const TString &t_sourcedigibranchname)
  : PndProcess("BSEmcFetchDigisFromPreclusterLinks"), fDigiSourceBranchname(t_sourcedigibranchname)
{
}

BSEmcFetchDigisFromPreclusterLinks::~BSEmcFetchDigisFromPreclusterLinks() {}

void BSEmcFetchDigisFromPreclusterLinks::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
}

void BSEmcFetchDigisFromPreclusterLinks::SetupParameters(const PndParameterRegister * /*t_paramRegister*/) {}

// -------------------------------------------------------------------------
void BSEmcFetchDigisFromPreclusterLinks::RequestDataContainer(PndContainerRegister *t_register)
{
  // Lets use the defaults set by the task
  fDigiTargetBranchName = (fDigiTargetBranchName == "") ? "Preclusterbunched" + BSEmcDataBranchNames::fgDigiBranchName + this->fDetectorName : fDigiTargetBranchName;

  t_register->Request({fDigiTargetBranchName, "BSEmcDigi", kTRUE, kTRUE});
  t_register->Request({fPreclusterBranchname, "BSEmcPrecluster", kTRUE, kTRUE});
}

// -------------------------------------------------------------------------
void BSEmcFetchDigisFromPreclusterLinks::GetDataContainer(PndContainerRegister *t_register)
{
  fPrecluster = t_register->GetOutput<BSEmcPrecluster>(fPreclusterBranchname);
  fDigis = t_register->GetOutput<BSEmcDigi>(fDigiTargetBranchName);
  t_register->SetAsDefaultBranchFor(fDigiTargetBranchName, BSEmcDigi().ClassName());
}

/**
 * @brief After timebunching the preclusters the precluster digi indices will not be indicating the right digis, as the eventstructure was broken.
 *
 * Here we attempt to create a new digi container and reseting the precluster digi indices accordingly, so we can continue with our reconstruction.
 *
 */
void BSEmcFetchDigisFromPreclusterLinks::Process()
{
  fDigis->Reset();

  std::map<FairLink, Int_t> linkToIdx;
  Int_t index = fDigis->GetSize();
  LOG(debug) << "BSEmcFetchDigisFromPreclusterLinks::Exec " << fPrecluster->GetSize() << " precluster in " << fPrecluster->GetBranchName() << " need to fetch their links.";

  for (BSEmcPrecluster *precluster : fPrecluster->GetVector()) {
    precluster->Clear();
    FairMultiLinkedData digiLinks = precluster->GetLinksWithType(FairRootManager::Instance()->GetBranchId(fDigiSourceBranchname));
    LOG(debug1) << "BSEmcFetchDigisFromPreclusterLinks::Exec precluster has " << digiLinks.GetNLinks() << " links to " << fDigiSourceBranchname << " and in total has "
                << precluster->GetLinks().size() << " links.";
    Double_t energy = precluster->GetEnergy();
    TVector3 position = precluster->GetPosition();
    for (Int_t j = 0; j < digiLinks.GetNLinks(); j++) {
      FairLink link = digiLinks.GetLink(j);
      std::unique_ptr<BSEmcDigi> digi = std::unique_ptr<BSEmcDigi>(dynamic_cast<BSEmcDigi *>(FairRootManager::Instance()->GetCloneOfLinkData(link)));
      if (digi != nullptr) {
        if (linkToIdx.find(link) == linkToIdx.end()) {
          linkToIdx[link] = fDigis->GetSize(); // Order is important for 0 indexing
          fDigis->CreateCopy(*(digi.get()));
        }
        energy += digi->GetEnergy();
        precluster->AddDigi(linkToIdx[link], digi->GetDetectorId());
        precluster->SetEnergy(
          energy); // These two lines are necessary as AddDigi invalidates the cluster energy and position (I do not think that this invalidation concept is overly sensible)
        precluster->SetPosition(position);

      } else {
        LOG(error) << "BSEmcFetchDigisFromPreclusterLinks::Exec - " << this->fDigiSourceBranchname << "- FairLink " << link << "to BSEmcDigi delivers null";
      }
    }
  }
  LOG(debug) << "BSEmcFetchDigisFromPreclusterLinks::Exec " << fDigis->GetSize() - index << " digis were stored in " << fDigis->GetBranchName();
}

ClassImp(BSEmcFetchDigisFromPreclusterLinks)
