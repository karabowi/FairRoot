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

#ifndef BSEMCFWENDCAPBLINDDECONVPAR_HH
#define BSEMCFWENDCAPBLINDDECONVPAR_HH

#include <string>

#include "Rtypes.h"
#include "RtypesCore.h"
#include "TArrayD.h"

#include "FairParGenericSet.h"
#include "FairParamList.h"

class FairParamList;
class TBuffer;
class TClass;
class TMemberInspector;

/**
 * @brief parameter set for the FwEndcap Blind deconvolution approach
 * @author Ph. Mahlberg <mahlberg@hiskp.uni-bonn.de>
 * @author Ben Salisbury <salisbury@hiskp.uni-bonn.de>, HISKP Bonn
 * @ingroup FwEndcap
 */
class BSEmcFwEndcapBlindDeconvPar : public FairParGenericSet {

 public:
  static const std::string fgParameterName;
  BSEmcFwEndcapBlindDeconvPar(const char *t_name = "BSEmcFwEndcapBlindDeconvPar", const char *t_title = "Emc forward endcap digi parameters",
                              const char *t_context = "TestDefaultContext");
  virtual ~BSEmcFwEndcapBlindDeconvPar(void){};

  const Text_t *GetDeconvolutionFilename() const { return fDeconvolutionFilename; };
  Double_t GetPeakFinderThreshold() const { return fPeakFinderThreshold; };
  Double_t GetPeakFinderLowerLimit() const { return fPeakFinderLowerLimit; };
  Double_t GetPeakFinderUpperLimit() const { return fPeakFinderUpperLimit; };

  Double_t GetPileUpDetectionYScale() const { return fPileUpDetectionYScale; };
  Double_t GetPileUpDetectionXOffset() const { return fPileUpDetectionXOffset; };
  Double_t GetPileUpDetectionYOffset() const { return fPileUpDetectionYOffset; };
  Int_t GetBaselineSampleNumber() const { return fBaselineSampleNumber; };

  virtual void putParams(FairParamList *t_list) /*override*/;
  virtual Bool_t getParams(FairParamList *t_list) /*override*/;

 private:
  // Empty method, WHY is this Called in constructor(FIXME)
  virtual void clear(void) /*override*/ {};

  Text_t fDeconvolutionFilename[100]{0};

  Double_t fPeakFinderThreshold{0};
  Double_t fPeakFinderLowerLimit{0};
  Double_t fPeakFinderUpperLimit{0};

  Double_t fPileUpDetectionYScale{0};
  Double_t fPileUpDetectionXOffset{0};
  Double_t fPileUpDetectionYOffset{0};

  Int_t fBaselineSampleNumber{0};

  ClassDef(BSEmcFwEndcapBlindDeconvPar, 3);
};

#endif /*BSEMCFWENDCAPBLINDDECONVPAR_HH*/
