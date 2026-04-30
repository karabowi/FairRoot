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

#ifndef PNDHYPSTRIPHITPRODUCER_H
#define PNDHYPSTRIPHITPRODUCER_H

#include "FairTask.h"
#include "PndGeoHypPar.h"
#include "PndHypStripDigiPar.h"
//#include "PndHypGeoMappingPar.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"
#include "PndHypStrip.h"
//#include "PndHypDigiPixel.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include <iostream>
#include "PndHypCalcStrip.h"
#include "PndHypGeoHandling.h"

#include <string>
#include <vector>

class TClonesArray;

//! Hit Producer Task for strip detectors
/**
   * The choice of the parameters used for Digitization depends on the invocation of the
   * different constructors of this task. Instantiating by the default constructor forces
   * the digitisation parameters initialized from the DigiPar-Database. On the other hand these
   * parameters may be overridden by invoking the constructor:
   * @code PndMvdStripHitProducer(Double_t, Double_t, Double_t, Double_t, TVector2, TVector2,
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

class PndHypStripHitProducer : public FairTask {
 public:
  /** Default constructor **/
  PndHypStripHitProducer();

  PndHypStripHitProducer(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                         Double_t threshold, Double_t noise, TString sensorType = "Rect", TString feType = "APV25");

  /** Destructor **/
  virtual ~PndHypStripHitProducer();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetParamSet(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                   Double_t threshold, Double_t noise, TString sensorType, TString feType);

 private:
  TString fBranchName;
  /** Input array of PndHypPoints **/
  TClonesArray *fPointArray;

  /** Output array of PndHypHits **/
  TClonesArray *fHitArray;
  TClonesArray *fStripArray;
  //  TClonesArray* fFeStripArray;

  PndGeoHypPar *fGeoPar;
  PndHypStripDigiPar *fCurrentDigiPar;
  PndHypStripDigiPar *fDigiPar;
  PndHypCalcStrip *fStripCalcTop;
  PndHypCalcStrip *fStripCalcBot;
  // PndHypGeoMappingPar* fGeoMappingPar;

  void Register();
  void Reset();
  void ProduceHits();

  // TGeoHMatrix GetTransformation (std::string detName) const;
  // void GetLocalHitPoints(PndHypPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
  TVector3 GetSensorDimensions(std::string detName) const;
  Bool_t SelectSensorParams(TString detname);
  PndHypGeoHandling *fGeoH; // converter for detector names
                            /// Anchor Point on top side
  TVector2 fTopAnchor;
  /// Anchor Point on bottom side
  TVector2 fBotAnchor;
  /*
  Double_t ftopPitch;	//strip pitch on top side;
  Double_t fbotPitch;	//strip pitch on bottom side;
  Double_t forient;	    //orientation of top strips;
  Double_t fskew;       //skew angle between upper and lower strips;
  /// Anchor Point on top side
  TVector2 fTopAnchor;
  /// Anchor Point on bottom side
  TVector2 fBotAnchor;
  Int_t    fnrTopFE;
  Int_t    fnrBotFE;
  Int_t    fnrFECh;
  Double_t fthreshold;  //strip threshold in electrons
  Double_t fnoise;      //strip noise in electrons
  */
  /// internal Flag that controls use of Parameter Invocations
  Bool_t fOverrideParams;

  // Int_t    stripHits;
  ClassDef(PndHypStripHitProducer, 5);
};

#endif
