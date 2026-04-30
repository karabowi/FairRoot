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

#include "BSEmcAddLinksToCluster.h"

#include "RtypesCore.h"
#include "TString.h"

#include "FairLink.h"

#include "PndContainerI.h"
#include "PndMutableContainerI.h"

#include "BSEmcCluster.h"

#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"
#include "BSEmcSubCluster.h"

#include "PndParameterRegister.h"

BSEmcAddLinksToCluster::BSEmcAddLinksToCluster() : PndProcess("BSEmcAddLinksToCluster") {}

BSEmcAddLinksToCluster::~BSEmcAddLinksToCluster() {}

void BSEmcAddLinksToCluster::SetDetectorName(const std::string &t_detectorName)
{
  fDetectorName = t_detectorName;
}

void BSEmcAddLinksToCluster::RequestDataContainer(PndContainerRegister *t_register)
{
  t_register->Request({fDigiBranchname, "BSEmcDigi", kFALSE, kFALSE});
  t_register->Request({fClusterBranchname, "BSEmcCluster", kTRUE, kTRUE});
  t_register->Request({fSubClusterBranchname, "BSEmcSubCluster", kTRUE, kTRUE});
}

void BSEmcAddLinksToCluster::GetDataContainer(PndContainerRegister *t_register)
{
  fDigis = t_register->GetInput<BSEmcDigi>(fDigiBranchname);
  fCluster = t_register->GetOutput<BSEmcCluster>(fClusterBranchname);
  fSubCluster = t_register->GetOutput<BSEmcSubCluster>(fSubClusterBranchname);
}

void BSEmcAddLinksToCluster::SetupParameters(const PndParameterRegister * /*t_parameterRegister*/)
{
}

void BSEmcAddLinksToCluster::Process()
{
  SetDigiLinksFor(fCluster->GetVector());
  SetDigiLinksFor(fSubCluster->GetVector());
  SetClusterLinksFor(fSubCluster->GetVector());
}

template <class T>
void BSEmcAddLinksToCluster::SetDigiLinksFor(const std::vector<T *> &t_cluster)
{
  for (BSEmcCluster *acluster : t_cluster) {
    for (const BSEmcDigiInfo_t &digiInfo : acluster->GetDigis()) {
      const BSEmcDigi *digi = fDigis->GetConstElementPtr(digiInfo.fDigiIdx);
      acluster->AddDigiLink(digi->GetEntryNr());
    }
  }
}

void BSEmcAddLinksToCluster::SetClusterLinksFor(const std::vector<BSEmcSubCluster *> &t_subcluster)
{
  for (BSEmcSubCluster *asubcluster : t_subcluster) {
    asubcluster->SetInsertHistory(kFALSE);
    asubcluster->AddLink(FairLink(fCluster->GetBranchName(), asubcluster->GetParentClusterIndex()));
  }
}
