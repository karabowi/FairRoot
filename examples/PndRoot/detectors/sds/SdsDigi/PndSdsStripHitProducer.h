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

#ifndef PNDSDSSTRIPHITPRODUCER_H
#define PNDSDSSTRIPHITPRODUCER_H

#include "PndSdsTask.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsMCPoint.h"
#include "PndSdsStrip.h"
#include "PndSdsDigiStrip.h"
#include "PndDetectorList.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "FairMCEventHeader.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"

#include "PndGeoHandling.h"
#include "PndSdsCalcStrip.h"
#include "PndSdsChargeConversion.h"
#include "PndSdsDigiStripWriteoutBuffer.h"

#include <string>
#include <vector>
#include <map>

class TClonesArray;

//! Hit Producer Task for strip detectors
/**
 * The choice of the parameters used for Digitization depends on the invocation of the
 * different constructors of this task. Instantiating by the default constructor forces
 * the digitisation parameters initialized from the DigiPar-Database. On the other hand these
 * parameters may be overridden by invoking the constructor:
 * @code PndSdsStripHitProducer(Double_t, Double_t, Double_t, Double_t, TVector2, TVector2,
 * Int_t, Int_t, Int_t, Double_t, Double_t) @endcode \n
 * Basically, the sensors are thought to be rectangular (even if they are not).
 * Knowing the origin of the wafer, the strips can be described by an angle (orientation),
 * their separation with respect to each other (pitch) and one point that is known to be part of the first strip (anchor point).\n
 * The numbering scheme is as follows (assuming 128 channels per FE): \n
 *
   <table>
   <TR> <TD> strip index </TD> <TD>frontend #</TD> <TD> side </TD> </TR>
   <TR> <TD>     0       </TD> <TD>    0     </TD> <TD>  top </TD> </TR> <TD><- Top Anchor</TD>
   <TR> <TD>     1       </TD> <TD>    0     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>    0     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    127      </TD> <TD>    0     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    128      </TD> <TD>    1     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>    1     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    255      </TD> <TD>    1     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    256      </TD> <TD>    2     </TD> <TD>  top </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>   ...    </TD> <TD>  top </TD> </TR>
   <TR> <TD> topNrFE*128-1 </TD> <TD> topNrFE-1  </TD> <TD>  top </TD> </TR>
   <TR> <TD> topNrFE*128 </TD> <TD> topNrFE  </TD> <TD>  bottom </TD> </TR> <TD><- Bottom Anchor</TD>
   <TR> <TD>    ...      </TD> <TD> topNrFE  </TD> <TD>  bottom </TD> </TR>
   <TR> <TD> topNrFE*128+127 </TD> <TD> topNrFE  </TD> <TD>  bottom </TD> </TR>
   <TR> <TD> topNrFE*128+128 </TD> <TD> topNrFE+1  </TD> <TD>  bottom </TD> </TR>
   <TR> <TD>    ...      </TD> <TD>   ...    </TD> <TD>  bottom </TD> </TR>
   <TR> <TD> (topNrFE+botNrFE)*128-1 </TD> <TD> topNrFE+botNrFE-1  </TD> <TD>  bottom </TD> </TR>

   </table>
 * \n
 * The numbering starts from the strip containing the anchor point following the direction
 * orthogonal to the strips in mathematically positive sense (along x-axis in positive direction,
 * if the strip orientation equals 90 degrees).
 * \n
 * @author HG Zaunick <hg.zaunick@physik.tu-dresden.de>
 *
 **/
class PndSdsStripHitProducer : public PndSdsTask {
 public:
  /** Default constructor \n
   * creates object with parameters taken implicitly from DigiPar-File
   **/
  PndSdsStripHitProducer();

  PndSdsStripHitProducer(PndSdsStripHitProducer &other)
    : PndSdsTask(), fPointArray(other.fPointArray), fStripArray(other.fStripArray), fDataBuffer(other.fDataBuffer), fDigiParameterList(other.fDigiParameterList),
      fChargeDigiParameterList(other.fChargeDigiParameterList), fCurrentDigiPar(other.fCurrentDigiPar), fCurrentChargeConverter(other.fCurrentChargeConverter),
      fStripCalcTop(other.fStripCalcTop), fStripCalcBot(other.fStripCalcBot), fChargeConverter(other.fChargeConverter), fCurrentStripCalcTop(other.fCurrentStripCalcTop),
      fCurrentStripCalcBot(other.fCurrentStripCalcBot), fMcEventHeader(other.fMcEventHeader), fGeoH(other.fGeoH), fOverrideParams(other.fOverrideParams),
      fTimeOrderedDigi(other.fTimeOrderedDigi), fEventNr(other.fEventNr){};
  /** Like default Constructor, but passes a name to PndSdsTask**/
  PndSdsStripHitProducer(const char *name);

  /** Destructor **/
  virtual ~PndSdsStripHitProducer();

  virtual void SetParContainers();
  /** Virtual method Init **/
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** pure virtual method SetBranchNames
   **
   ** called by Init()
   ** function to set individual branch names
   **/
  virtual void SetBranchNames() = 0;
  virtual void SetMCPointType(){};

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);
  virtual void FinishEvent();
  virtual void FinishTask();
  void AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge);

  void RunTimeBased() { fTimeOrderedDigi = kTRUE; }

  PndSdsStripHitProducer &operator=(PndSdsStripHitProducer &other)
  {
    if (this != &other) // protect against invalid self-assignment
    {
      fPointArray = other.fPointArray;
      fStripArray = other.fStripArray;
      fDataBuffer = other.fDataBuffer;
      fDigiParameterList = other.fDigiParameterList;
      fChargeDigiParameterList = other.fChargeDigiParameterList;
      fCurrentDigiPar = other.fCurrentDigiPar;
      fCurrentChargeConverter = other.fCurrentChargeConverter;
      fStripCalcTop = other.fStripCalcTop;
      fStripCalcBot = other.fStripCalcBot;
      fChargeConverter = other.fChargeConverter;
      fCurrentStripCalcTop = other.fCurrentStripCalcTop;
      fCurrentStripCalcBot = other.fCurrentStripCalcBot;
      fMcEventHeader = other.fMcEventHeader;
      fGeoH = other.fGeoH;
      fOverrideParams = other.fOverrideParams;
      fTimeOrderedDigi = other.fTimeOrderedDigi;
      fEventNr = other.fEventNr;
    }
    return *this;
  }

 protected:
  /** Input array of PndSdsMCPoints **/
  TClonesArray *fPointArray;

  //! Output array of PndSdsHits
  TClonesArray *fStripArray;

  FairWriteoutBuffer *fDataBuffer;

  //! Digitization Parameters
  TList *fDigiParameterList;
  TList *fChargeDigiParameterList;
  PndSdsStripDigiPar *fCurrentDigiPar;
  PndSdsChargeConversion *fCurrentChargeConverter;

  //! Calculator objects
  std::map<const char *, PndSdsCalcStrip *> fStripCalcTop;
  std::map<const char *, PndSdsCalcStrip *> fStripCalcBot;
  std::map<const char *, PndSdsChargeConversion *> fChargeConverter;
  PndSdsCalcStrip *fCurrentStripCalcTop;
  PndSdsCalcStrip *fCurrentStripCalcBot;

  void Register();
  void Reset();
  void ProduceHits();
  virtual void SetCalculators();
  Bool_t SelectSensorParams(Int_t sensorID);

  Double_t DigitizeTime(Double_t time, Double_t charge);

  FairMCEventHeader *fMcEventHeader; //!

  PndGeoHandling *fGeoH;   //! ///< converter for detector names
  Bool_t fOverrideParams;  ///< internal Flag that controls use of Parameter Invocations
  Bool_t fTimeOrderedDigi; ///< parameter to switch to time ordered simulation

  Int_t fEventNr; ///< EventCounter

  ClassDef(PndSdsStripHitProducer, 5);
};

#endif
