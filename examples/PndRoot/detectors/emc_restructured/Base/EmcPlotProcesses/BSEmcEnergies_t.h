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

#ifndef BSEMCENERGIES_T_HH
#define BSEMCENERGIES_T_HH

#include "TClonesArray.h"

#include "BSEmcDigi.h"
#include "BSEmcGeoNeighbouringRelationPar.h"

struct BSEmcEnergies_t {
  Double_t CenterEnergy{0};
  Double_t NeighbourEnergy{0};
  Double_t MaxNeighbourEnergy{0};
  Int_t NeighbourNumber{0};

  Double_t MaxRatio() { return MaxNeighbourEnergy / CenterEnergy; }
  Double_t SumRatio() { return NeighbourEnergy / CenterEnergy; }
  Double_t MaxPlusRatio() { return (MaxNeighbourEnergy + CenterEnergy) / CenterEnergy; }
  Double_t SumPlusRatio() { return (NeighbourEnergy + CenterEnergy) / CenterEnergy; }

  static BSEmcEnergies_t GetEnergies(Int_t t_centralcrystal, const std::vector<const BSEmcDigi *> &t_digiArray, BSEmcGeoNeighbouringRelationPar *t_neighbouring)
  {
    BSEmcEnergies_t energies;
    Double_t energy = 0.0;
    Int_t detectorid = 0;
    for (const BSEmcDigi *digi : t_digiArray) {
      energy = digi->GetEnergy();
      detectorid = digi->GetDetectorId();
      if (detectorid == t_centralcrystal) {
        energies.CenterEnergy += energy;
      } else if (t_neighbouring->AreNeighbours(detectorid, t_centralcrystal)) {
        energies.NeighbourNumber += 1;
        energies.NeighbourEnergy += energy;
        if (energy > energies.MaxNeighbourEnergy) {
          energies.MaxNeighbourEnergy = energy;
        }
      }
    }
    return energies;
  }
};

#endif /*BSEMCENERGIES_T_HH*/
