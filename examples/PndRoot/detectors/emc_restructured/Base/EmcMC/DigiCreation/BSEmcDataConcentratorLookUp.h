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

#ifndef BSEMCDATACONCENTRATORLOOKUP_HH
#define BSEMCDATACONCENTRATORLOOKUP_HH

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TObject.h"
#include "TString.h"

class TBuffer;
class TClass;
class TMemberInspector;

enum class BSEmcDetectorType : Int_t { FWENDCAP, BWENDCAP, BARREL, SHASHLYK };

/**
 * @class BSEmcDataConcentratorLookUp
 * @brief rudimentary if-statement heavy lookup for data concentrator number for each crystal (detectorId) developed by Viktor Rodin
 * @details Used in BSEmcDigiDCSetterProcess: Has to be set, else Preclustering will not work
 * @author Viktor Rodin
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @date 2021-02
 * @ingroup EmcBase
 */
class BSEmcDataConcentratorLookUp : public TObject {
 public:
  Int_t GetNumberOfDCsFor(TString t_detector) const;           // for mapping into virtual Data Concentrators (DC)
  Int_t GetNumberOfDCsFor(BSEmcDetectorType t_detector) const; // for mapping into virtual Data Concentrators (DC)

  Int_t GetDCNumber(Int_t t_detectorId) const; // for mapping into virtual Data Concentrators (DC)

  ClassDef(BSEmcDataConcentratorLookUp, 8);
};

#endif /*BSEMCDATACONCENTRATORLOOKUP_HH*/
