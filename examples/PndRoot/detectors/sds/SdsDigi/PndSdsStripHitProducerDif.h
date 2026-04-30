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

#ifndef PNDSDSSTRIPHITPRODUCERDIF_H
#define PNDSDSSTRIPHITPRODUCERDIF_H

#include "PndSdsTask.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsMCPoint.h"
#include "PndSdsStrip.h"
#include "PndSdsDigiPixel.h"
#include "PndDetectorList.h"

#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"

#include "PndGeoHandling.h"
#include "PndSdsCalcStripDif.h"

#include <string>
#include <vector>

class TClonesArray;

//! Hit Producer Task for strip detectors with electron diffusion
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
class PndSdsStripHitProducerDif : public PndSdsTask {
 public:
  /** Default constructor \n
   * creates object with parameters taken implicitly from DigiPar-File
   **/
  PndSdsStripHitProducerDif();

  /**
   * creates object with explicit assignment of Digitization parameters
   *
   * @param topPitch strip pitch on top side (cm)
   * @param botPitch strip pitch on bottom side (cm)
   * @param ori Orientation angle of strips on top side (x-y-plane)
   * @param skew Orientation angle of bottom strips relative to top strips
   * @param topAnchor Anchor Point on top side. The coordinates of this point are to be in centimeters
   * from the center (0,0) of the sensor.
   * @param botAnchor Anchor Point on bottom side. Same as above
   * @param nrTopFE number of frontends attached to the top sensor side
   * @param nrBotFE number of frontends attached to the bottom sensor side
   * @param nrFECh number of channels of one single frontend
   * @param threshold charge threshold (electrons)
   * @param noise equiv. noise charge (electrons)
   */
  PndSdsStripHitProducerDif(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                            Double_t threshold, Double_t noise, TString sensorType = "Rect", TString feType = "APV25");

  /** Destructor **/
  virtual ~PndSdsStripHitProducerDif();

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
  virtual void SetMCPointType() = 0;

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void AddDigi(Int_t &iStrip, Int_t iPoint, Int_t detID, Int_t sensorID, Int_t fe, Int_t chan, Double_t charge);

  void SetParamSet(Double_t topPitch, Double_t botPitch, Double_t ori, Double_t skew, TVector2 topAnchor, TVector2 botAnchor, Int_t nrTopFE, Int_t nrBotFE, Int_t nrFECh,
                   Double_t threshold, Double_t noise, TString sensorType, TString feType);
  /** Accessor functions **/
  //   Double_t GetTopPitch()  const {return fTopPitch;}
  //   Double_t GetBotPitch()  const {return fBotPitch;}
  //   Double_t GetOrient()    const {return fOrient;}
  //   Double_t GetSkew()      const {return fSkew;}
  //   TVector2 GetTopAnchor() const {return fTopAnchor;}
  //   TVector2 GetBotAnchor() const {return fBotAnchor;}
  //   Int_t    GetNrTopFE()   const {return fNrTopFE;}
  //   Int_t    GetNrBotFE()   const {return fNrBotFE;}
  //   Int_t    GetNrFECh()    const {return fNrFECh;}
  //   Double_t GetThreshold() const {return fThreshold;}
  //   Double_t GetNoise()     const {return fNoise;}

 protected:
  TString fBranchName;

  /** Input array of PndSdsMCPoints **/

  TClonesArray *fPointArray;
  //! Output array of PndSdsHits
  //  TClonesArray* fHitArray;
  //! Array of hit strips
  TClonesArray *fStripArray;
  //  TClonesArray* fFeStripArray;

  //! Digitization Parameters
  PndSdsStripDigiPar *fDigiParRect;
  PndSdsStripDigiPar *fDigiParTrap;
  PndSdsStripDigiPar *fCurrentDigiPar;

  fDetectorType fMCPointType;

  //! Calculator objects
  PndSdsCalcStripDif *fStripCalcTopRect;
  PndSdsCalcStripDif *fStripCalcBotRect;
  PndSdsCalcStripDif *fStripCalcTopTrap;
  PndSdsCalcStripDif *fStripCalcBotTrap;
  PndSdsCalcStripDif *fCurrentStripCalcTop;
  PndSdsCalcStripDif *fCurrentStripCalcBot;

  void Register();
  void Reset();
  void ProduceHits();

  //   TGeoHMatrix GetTransformation (std::string detName) const;
  //   void GetLocalHitPoints(PndSdsMCPoint* myPoint, FairGeoVector& myHitIn, FairGeoVector& myHitOut);
  //   TVector3 GetSensorDimensions(std::string detName) const;
  Bool_t SelectSensorParams(Int_t sensorID);

  PndGeoHandling *fGeoH;  // converter for detector names
                          /*  Double_t fTopPitch;       // strip pitch on top side
                            Double_t fBotPitch;       // strip pitch on bottom side
                            Double_t fOrient;         // orientation of top strips
                            Double_t fSkew;           // skew angle between upper and lower strips
                            TVector2 fTopAnchor;      // Anchor Point on top side
                            TVector2 fBotAnchor;      // Anchor Point on bottom side
                            Int_t    fNrTopFE;        // Number of Frontends attached to the top side
                            Int_t    fNrBotFE;        // Number of Frontends attached to the bottom side
                            Int_t    fNrFECh;         // Number of channels per frontend-chip
                            Double_t fThreshold;      // strip threshold in electrons
                            Double_t fNoise;          // strip noise in electrons*/
  Bool_t fOverrideParams; // internal Flag that controls use of Parameter Invocations
                          //   TString  fSensType;       // Type of the Sensor
                          //   TString  fFeType;         // Frontend type name (not used yet)

  //  Int_t    stripHits;

  ClassDef(PndSdsStripHitProducerDif, 6);
};

#endif
