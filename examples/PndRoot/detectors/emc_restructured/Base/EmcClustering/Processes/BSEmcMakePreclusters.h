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

//-----------------------------------------------------------------------
// File and Version Information:
// $Id: $
//
//---------------------------------------------------------------------
//#pragma once
#ifndef BSEMCMAKEPRECLUSTERS_HH
#define BSEMCMAKEPRECLUSTERS_HH

#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcCrystalPositionPar.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"
#include "BSEmcPrecluster.h"

class BSEmcCrystalPositionPar;
class BSEmcDigi;
class BSEmcGeoNeighbouringRelationPar;
class BSEmcPrecluster;
class PndParameterRegister;
class TBuffer;
class TClass;
class TMemberInspector;
struct BSEmcClusteringData;
template <class T>
class PndContainerI;
template <class T>
class PndMutableContainerI;

/**
 * @class BSEmcMakePreclusters
 * @brief Online like preclustering, takes digis, sorts them into DataConcentrator and only clusters within one DataConcentrator digi set
 * @details To be run before BSEmcMergePreclusters
 * @author Viktor Rodin
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcMakePreclusters : public PndProcess {
 public:
  BSEmcMakePreclusters();
  virtual ~BSEmcMakePreclusters();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  virtual void Process() /*override*/;
  std::vector<BSEmcPrecluster *> Precluster(const std::vector<const BSEmcDigi *> &t_digiCont);

  /** Set precluster position and radius method: 0 = default method (for position, logarithmic weighing; for radius, distance of digi furthest from position), 1 = simplified
       method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize)) for preclusters only, 2 = simplified method for preclusters and clusters. Using
       the simplified method is less accurate, but saves a very large amount of resources **/
  void SetPositionMethod(Int_t t_method) { fPosMethod = t_method; }
  void SetDigiPositionType(Int_t t_type) { fDigiPosType = t_type; } // select whether to use mapped or real digi position

  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }
  void SetClusterPropertiesParName(const std::string &t_parName) { fClusterPropertiesParName = t_parName; }
  void SetPositionParName(const std::string &t_parName) { fPositionParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetPreclusterBranchName(const TString &t_branchname) { fPreclusterBranchname = t_branchname; }

 protected:
  virtual void FinishPreclusters(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcPrecluster *> &t_preclusterCont);
  void FinishPrecluster(const std::vector<const BSEmcDigi *> &t_digiCont, BSEmcPrecluster *t_cluster);

 private:
  TString fDigiBranchname{""};
  TString fPreclusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcPrecluster> *fPreclusterArray{nullptr};
  std::string fClusteringParName{""};
  std::string fClusterPropertiesParName{""};
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};
  Double_t fDigiEnergyThreshold{-1};
  Int_t fPosMethod{0};
  Int_t fDigiPosType{0};
  BSEmcCrystalPositionPar *fPositionPar{nullptr};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};

  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  Int_t evtCounter;
  Int_t digiCounter;
  Int_t nTotDigisPassed;
  Int_t fNrOfDigis;
  Int_t fNrOfEvents;
  Int_t nPrecProg;
  Double_t DCtotRtime;
  Double_t DCtotCtime;

  ClassDef(BSEmcMakePreclusters, 1)
};
#endif /*BSEMCMAKEPRECLUSTERS_HH*/
