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
#ifndef BSEMCMERGEPRECLUSTERS_HH
#define BSEMCMERGEPRECLUSTERS_HH

#include <memory>
#include <string>
#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "PndContainerI.h"
#include "PndProcess.h"

#include "BSEmcCluster.h"

#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"
#include "BSEmcPositionAlgo.h"
#include "BSEmcPrecluster.h"

class BSEmcCluster;
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
 * @class BSEmcMergePreclusters
 * @brief Process to merge preclusters into Clusters
 * @details To be run after BSEmcMergePreclusters and before BSEmcLocalMaxFindingProcess
 * @author Viktor Rodin
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcMergePreclusters : public PndProcess {
 public:
  BSEmcMergePreclusters();

  virtual ~BSEmcMergePreclusters();

  virtual void SetDetectorName(const std::string &t_detectorName) /*override*/;
  virtual void RequestDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void GetDataContainer(PndContainerRegister *t_register) /*override*/;
  virtual void SetupParameters(const PndParameterRegister *t_paramRegister) /*override*/;
  virtual void Process() /*override*/;
  std::vector<BSEmcCluster *> MergePreclusters(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcPrecluster *> &t_preclusterCont);

  void SetPositionMethod(Int_t method)
  {
    fPosMethod = method;
  } /** Set cluster position method:
0 = default method (logarithmic weighing),
1 = simplified method (xpos=(xmax+xmin)/2, ypos=(ymax+ymin)/2) using the REAL x,y position,
2 = simplified method (xpos=(xmax+xmin)/2, ypos=(ymax+ymin)/2) using the MAPPED XPad,YPad position,
3 = simplified method using the REAL x,y position of the DIGI WITH HIGHEST ENERGY,
4 = simplified method using the MAPPED XPad,YPad position of the DIGI WITH HIGHEST ENERGY.
**/
  void SetNeighbourMethod(Int_t nbmethod)
  {
    fNbMethod = nbmethod;
  } /** Set cluster neighbour nbmethod:
0 = default method (for position, logarithmic weighing; for radius, distance of digi furthest from position) -> use this CIRCLE for neighbour relations),
1 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize) -> use this CIRCLE for neighbour relations),
2 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, rx=(xmax-xmin)/2, ry=(ymax-ymin)/2 -> use this RECTANGULAR BOX for neighbour relations).
3 = simplified method (for position, xpos=(xmax+xmin)/2 ypos=(ymax+ymin)/2; for radius, r=max(ysize, xsize) -> use this SQUARE BOX for neighbour relations).
**/

  void SetClusteringParName(const std::string &t_parName) { fClusteringParName = t_parName; }
  void SetClusterPropertiesParName(const std::string &t_parName) { fClusterPropertiesParName = t_parName; }
  void SetPositionParName(const std::string &t_parName) { fPositionParName = t_parName; }
  void SetNeighbouringRelationParName(const std::string &t_parName) { fNeighbouringRelationParName = t_parName; }

  void SetDigiBranchName(const TString &t_branchname) { fDigiBranchname = t_branchname; }
  void SetPreclusterBranchName(const TString &t_branchname) { fPreclusterBranchname = t_branchname; }
  void SetClusterBranchName(const TString &t_branchname) { fClusterBranchname = t_branchname; }

 protected:
  std::vector<Int_t> FindNeighbouringPrecluster(const std::vector<BSEmcPrecluster *> &t_preclusterCont);
  std::vector<Int_t> ClusterNeigbours(const std::vector<Int_t> &t_neighbours, const std::vector<BSEmcPrecluster *> &t_preclusterCont);

  std::vector<BSEmcCluster *> MergePreclustersIntoCluster(const std::vector<Int_t> &t_clusterNrForPrecluster, const std::vector<BSEmcPrecluster *> &t_preclusterCont);

  virtual void FinishClusters(const std::vector<const BSEmcDigi *> &t_digiCont, const std::vector<BSEmcCluster *> &t_clusterCont);
  void FinishCluster(const std::vector<const BSEmcDigi *> &t_digiCont, BSEmcCluster *tmpcluster);

 private:
  TString fDigiBranchname{""};
  TString fPreclusterBranchname{""};
  TString fClusterBranchname{""};

  PndContainerI<BSEmcDigi> *fDigiArray{nullptr};
  PndMutableContainerI<BSEmcPrecluster> *fPreclusterArray{nullptr};
  PndMutableContainerI<BSEmcCluster> *fClusterArray{nullptr};
  std::string fClusteringParName{""};
  std::string fClusterPropertiesParName{""};
  std::string fPositionParName{""};
  std::string fNeighbouringRelationParName{""};
  BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};

  BSEmcCrystalPositionPar *fPositionPar{nullptr};
  std::unique_ptr<BSEmcPositionAlgo> fPositionProcess{new BSEmcPositionAlgo};
  Int_t fPosMethod;
  Int_t fNbMethod;
  Int_t evtCounter;
  Int_t precCounter;
  Int_t fNrOfPres;
  Int_t nMrgProg;
  Int_t nTotClusters;
  Int_t fRemovedClusters;
  Int_t nTotDigis;
  Int_t wrongConnection;
  Double_t CNtotRtime;
  Double_t CNtotCtime;

  Int_t fClusterNr{0};
  ClassDef(BSEmcMergePreclusters, 2)
};
#endif /*BSEMCMERGEPRECLUSTERS_HH*/
