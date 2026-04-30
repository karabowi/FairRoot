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

//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id:$
//
// Description:
//	Class BSEmcPreclusters. An extension of PndEmcCluster - for testing
//          purposes only.
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
// Adapted for the PANDA experiment at GSI
//
// Author List:
//
//------------------------------------------------------------------------
//#pragma once
#ifndef BSEMCPRECLUSTER_HH
#define BSEMCPRECLUSTER_HH

#include <vector>

#include "Rtypes.h"     // for BSEmcPrecluster::Class, BSEmcPrecluster::S...
#include "RtypesCore.h" // for Double_t, Int_t

#include "BSEmcCluster.h" // for BSEmcCluster
#include "BSEmcDataBranchNames.h"

class TBuffer;
class TClass;
class TMemberInspector;

//#include <vector> // for vector

/**
 * @brief represents an emc precluster, formed in the (virtual) data concentrators
 * @author
 * @ingroup EmcData
 */
class BSEmcPrecluster : public BSEmcCluster {

 public:
  void AddDigiInfo(Int_t t_digiIdx, Int_t t_detectorId, Double_t t_energy, Double_t t_time, Double_t t_x, Double_t t_y, Double_t t_z);

  Int_t GetNumberOfDigis() { return fDigiTimeArray.size(); };
  std::vector<Double_t> &GetMemberDigiTimes() { return fDigiTimeArray; };
  std::vector<Double_t> &GetMemberDigiEnergies() { return fDigiEnergyArray; };
  std::vector<Double_t> &GetMemberDigiXpos() { return fDigiXposArray; };
  std::vector<Double_t> &GetMemberDigiYpos() { return fDigiYposArray; };
  std::vector<Double_t> &GetMemberDigiZpos() { return fDigiZposArray; };

  BSEmcPrecluster();
  BSEmcPrecluster(const BSEmcPrecluster &t_other);
  virtual ~BSEmcPrecluster();

 protected:
  std::vector<Double_t> fDigiTimeArray;
  std::vector<Double_t> fDigiEnergyArray;
  std::vector<Double_t> fDigiXposArray;
  std::vector<Double_t> fDigiYposArray;
  std::vector<Double_t> fDigiZposArray;

  ClassDef(BSEmcPrecluster, 1)
};
#endif /*BSEMCPRECLUSTER_HH*/
