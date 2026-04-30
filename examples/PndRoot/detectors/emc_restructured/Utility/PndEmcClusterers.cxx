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

#include "PndEmcClusterers.h"

#include "PndPersistencyTask.h"

#include "BSEmcAddLinksToCluster.h"
#include "BSEmcBarrelECF.h"
#include "BSEmcBarrelErrorMatrixCalculation.h"
#include "BSEmcBwEndcapECF.h"
#include "BSEmcClusteringProcess.h"
#include "BSEmcClusteringTask.h"
#include "BSEmcCounter.h"
#include "BSEmcCreateRecoHitProcess.h"
#include "BSEmcFlagSplitOffs.h"
#include "BSEmcMergeRecoHits.h"
#include "BSEmcDataBranchNames.h"
#include "BSEmcErrorMatrixCalculation.h"
#include "BSEmcExpClusterSplittingProcess.h"
#include "BSEmcFinishRecoHitProcess.h"
#include "BSEmcFwEndcapECF.h"
#include "BSEmcJoinBranches.h"
#include "BSEmcLocalMaxFindingProcess.h"
#include "BSEmcMCPropagationProcess.h"
#include "BSEmcOutOfClusterCorrectionProcess.h"
#include "BSEmcParameterLoadingProcess.h"
#include "BSEmcPhiRecoHitSplitter.h"
#include "BSEmcShashlikECF.h"
#include "BSEmcRemoveSplitOffMax.h"

BSEmcBaseClusterer::BSEmcBaseClusterer(const std::string &t_detectorName, BSEmcCorrection *t_ecf, BSEmcErrorMatrixCalculation *t_errorCalc, Bool_t t_removeSplitoff)
  : PndPersistencyTask("BSEmcBarrelClusterer")
{
  BSEmcClusteringTask *clusterer = new BSEmcClusteringTask(t_detectorName);
  clusterer->SetDigiBranchName(BSEmcDataBranchNames::fgDigiBranchName + t_detectorName);
  clusterer->SetClusterBranchName(BSEmcDataBranchNames::fgClusterBranchName + t_detectorName);
  clusterer->SetSubClusterBranchName(BSEmcDataBranchNames::fgSubClusterBranchName + t_detectorName);
  clusterer->SetRecoHitBranchName(BSEmcDataBranchNames::fgRecoHitBranchName + t_detectorName);

  clusterer->AddProcess(new BSEmcClusteringProcess());
  clusterer->AddProcess(new BSEmcLocalMaxFindingProcess());
  if (!t_removeSplitoff) {
    clusterer->AddProcess(new BSEmcRemoveSplitOffMax());
  }

  clusterer->AddProcess(new BSEmcExpClusterSplittingProcess());

  clusterer->AddProcess(new BSEmcAddLinksToCluster());
  clusterer->AddProcess(new BSEmcMCPropagationProcess());

  clusterer->AddProcess(new BSEmcCreateRecoHitProcess());
  if (t_removeSplitoff) {
    clusterer->AddProcess(new BSEmcFlagSplitOffs());
    clusterer->AddProcess(new BSEmcMergeRecoHits());
  }
  clusterer->AddProcess(new BSEmcOutOfClusterCorrectionProcess());
  BSEmcFinishRecoHitProcess *finish = new BSEmcFinishRecoHitProcess();
  finish->SetECF(t_ecf);
  finish->SetErrorMatrixCalculator(t_errorCalc);
  clusterer->AddProcess(finish);
  this->Add(clusterer);
}

BSEmcClusterer::BSEmcClusterer() : PndPersistencyTask("BSEmcClusterer")
{
  TString vmcdir = gSystem->Getenv("VMCWORKDIR");
  TString qadir = vmcdir + "/macro/params/";
  PndProcessTask *general = new BSEmcClusteringTask("General");
  general->AddProcess(new BSEmcParameterLoadingProcess(TString{qadir + "EmcCrystalsNeighbouringRelations.txt"}.Data()));
  general->AddProcess(new BSEmcCounter());
  this->Add(general);
  this->Add(new BSEmcBaseClusterer("Barrel", new BSEmcBarrelECF(), new BSEmcBarrelErrorMatrixCalculation(), kTRUE));
  this->Add(new BSEmcBaseClusterer("BwEndcap", new BSEmcBwEndcapECF(), new BSEmcErrorMatrixCalculation(), kTRUE));
  this->Add(new BSEmcBaseClusterer("FwEndcap", new BSEmcFwEndcapECF(), new BSEmcErrorMatrixCalculation(), kTRUE));
  this->Add(new BSEmcBaseClusterer("Shashlik", new BSEmcShashlikECF(), new BSEmcErrorMatrixCalculation(), kFALSE));

  BSEmcPhiRecoHitSplitter *phisplitter = new BSEmcPhiRecoHitSplitter();
  this->Add(phisplitter);
  // Combine all different RecoHit containers into one "EmcRecoHits" Container.
  // The EmcRecoHitBarrel, BwEndcap,..., will be empty afterwards!!!
  std::vector<TString> sourcebranches = {
    "EmcRecoHitBarrel",
    "EmcRecoHitBwEndcap",
    "EmcRecoHitShashlik",
    "EmcRecoHitFwEndcap",
  };
  BSEmcJoinBranches *branchJoiner = new BSEmcJoinBranches(sourcebranches, "EmcRecoHits");
  this->Add(branchJoiner);
}
