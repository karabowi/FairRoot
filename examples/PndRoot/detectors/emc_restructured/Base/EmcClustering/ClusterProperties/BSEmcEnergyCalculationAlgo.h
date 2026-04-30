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

#ifndef BSEMCENERGYCALCULATIONALGO_HH
#define BSEMCENERGYCALCULATIONALGO_HH

#include <vector>

#include "Rtypes.h"
#include "RtypesCore.h"

#include "BSEmcGeoNeighbouringRelationPar.h"

class BSEmcDigi;
class BSEmcCluster;
class BSEmcGeoNeighbouringRelationPar;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @class BSEmcEnergyCalculationAlgo
 * @brief Helper class to calculate the Energies of a cluster
 * @details Offers functions to calculate the total Energy of a cluster, the maximum energy (E1),
 *  the energy of the 9 central crystals (maximum + 8 neighbours) (E9),
 *  or the energy of the 25 most central crystals (central crystal + neighbours and neighbours neighbours).
 *
 * BSEmcEnergyCalculationAlgo are used in BSEmcClusteringProcess, BSEmcExpClusterSplitting and BSEmcCreateRecoHitProcess
 * @author Stephen J. Gowdy	Originator
 * @author Phil Strother
 * @author Helmut Marsiske
 * @author Dima Melnichuk, adaption for pandaroot
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcEnergyCalculationAlgo {
 public:
  BSEmcEnergyCalculationAlgo();
  virtual ~BSEmcEnergyCalculationAlgo();

  void SetNeighbouringRelation(const BSEmcGeoNeighbouringRelationPar *t_neighbouringRelation) { fNeighbouringRelationPar = t_neighbouringRelation; }

  Double_t CalculateEnergy(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const;
  Double_t CalculateE1(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const;
  Double_t CalculateE9(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const;
  Double_t CalculateE25(const BSEmcCluster *t_cluster, const std::vector<const BSEmcDigi *> &t_digis) const;

 private:
  const BSEmcGeoNeighbouringRelationPar *fNeighbouringRelationPar{nullptr};
  ClassDef(BSEmcEnergyCalculationAlgo, 1);
};

#endif /*BSEMCENERGYCALCULATIONALGO_HH*/
