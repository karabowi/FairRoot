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

#ifndef BSEMCFETCHDIGISFROMPRECLUSTERLINKS_HH
#define BSEMCFETCHDIGISFROMPRECLUSTERLINKS_HH

#include <memory>
#include <string>

#include "Rtypes.h"
#include "TString.h"

#include "PndContainerI.h"
#include "PndParameterRegister.h"
#include "PndProcess.h"

#include "BSEmcDataBranchNames.h"
#include "BSEmcDigi.h"
#include "BSEmcPrecluster.h"

class BSEmcDigi;
class BSEmcPrecluster;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcFetchDigisFromPreclusterLinks
 * @brief As Timebunching of preclusters breaks the eventstructure and therefore our digi indices cluster concept.
 * @details We need to create a new digi container and reset the precluster digi indices to point at them.
 * Be aware, that this only works for preclusters using the same digi container source.
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcFetchDigisFromPreclusterLinks : public PndProcess {
 public:
  BSEmcFetchDigisFromPreclusterLinks(const TString &t_sourcedigibranchname = "Preclustering" + BSEmcDataBranchNames::fgDigiBranchName + "FwEndcap");
  virtual ~BSEmcFetchDigisFromPreclusterLinks();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_parameterRegister) /*override*/;
  virtual void Process() /*override*/;

  void SetDigiSourceBranchName(const TString &t_branchname) { fDigiSourceBranchname = t_branchname; }
  void SetDigiTargetBranchName(const TString &t_branchname) { fDigiTargetBranchName = t_branchname; }
  void SetPreclusterBranchName(const TString &t_branchname) { fPreclusterBranchname = t_branchname; }

 private:
  PndMutableContainerI<BSEmcDigi> *fDigis{nullptr};
  PndMutableContainerI<BSEmcPrecluster> *fPrecluster{nullptr};
  TString fDigiSourceBranchname{""};
  TString fDigiTargetBranchName{""};
  TString fPreclusterBranchname{""};

  ClassDef(BSEmcFetchDigisFromPreclusterLinks, 1);
};

#endif /*BSEMCFETCHDIGISFROMPRECLUSTERLINKS_HH*/
