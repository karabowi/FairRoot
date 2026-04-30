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

#ifndef PNDRICHGEO_H
#define PNDRICHGEO_H

#include "FairGeoSet.h"
#include "TVector3.h"
#include "TGraph.h"

class PndRichGeo : public FairGeoSet {

 private:
  TVector3 fRichOffset; //!< Rich system offset [cm]
  TVector3 fAlBoxSize;  //!< Aluminium Box size [cm]
  double fAlBoxWallThickness;

  TVector3 fAerogelSize;                //!< Aerogel size [cm]
  TVector3 fAerogelOffset;              //!< Aerogel offset [cm]
  std::vector<Double_t> fnOpt;          //!< refraction index of the aerogel
  std::vector<Double_t> fAerogelLayers; //!< No idea (SS)

  double fAngleExtansionInner;   //!< Angle extansion for cherenkov light
                                 //!< for inner part of the mirror
                                 //!< [in unit of maximal cherenrov angle]
  double fAngleExtansionOuter;   //!< Angle extansion for cherenkov light
                                 //!< for outer part of the mirror
                                 //!< [in unit of maximal cherenrov angle]
  double fMirrorCurvature;       //!< Mirror curvature [degree]
  double fAngleOfMirrorPosition; //!< Angle of the mirror chord [degree]
  double fMirrorThickness;       //!< Mirror thickness [cm]
  double fMirrorLength;          //!< Mirror length [cm]

  double fPhDetLength;    //!< Length of the photodetector plate [cm]
  double fPhDetThickness; //!< Thickness or the photodetector plate [cm]

  double fBeamPipeHoleX; //!< X size of the beam pipe hole [cm]
  double fBeamPipeHoleY; //!< Y size of the beam pipe hole [cm]

  size_t fMirrorType;

  /*! Circle mirror parameters */
  double fAerogelEntrancePositionZ;
  double fMirrorThetaMin;
  double fMirrorThetaMax;
  double fMirrorRadius;
  TVector3 fMirrorAxis;
  TVector3 fMirrorAxisGlob;

  /*! Flat mirrors parameters */
  std::vector<Double_t> fFlatMirrorZ;     //!< No idea (SS)
  std::vector<Double_t> fFlatMirrorY;     //!< No idea (SS)
  std::vector<Double_t> fFlatMirrorZGlob; //!< No idea (SS)
  std::vector<Double_t> fFlatMirrorYGlob; //!< No idea (SS)

  /*! PhotoDet parameters */
  std::vector<Double_t> fPhDetZ; //!< No idea (SS)
  std::vector<Double_t> fPhDetY; //!< No idea (SS)
  std::vector<Double_t> fWlPhoton;
  std::vector<Double_t> fPDE;
  TGraph *fPhDetEff;

  UInt_t fSenseLevel;
  UInt_t fSensorsPerDevice;
  Int_t fSensorIndexX;
  Int_t fSensorIndexY;
  Int_t fSensorIndex;
  TVector3 fSensorPosition;

  /*! PhDet parameters */
  Double_t fPhDetSizeX;
  Double_t fPhDetSizeY;
  Double_t fPhDetGapX;
  Double_t fPhDetGapY;
  UInt_t fPhDetNumX;
  UInt_t fPhDetNumY;
  UInt_t fPhDetPixelNumX;
  UInt_t fPhDetPixelNumY;

  UInt_t fPhDetDev;
  Double_t fPhDetAngle;
  TVector3 fPhDetP0U, fPhDetNxU, fPhDetNyU, fPhDetNzU;
  TVector3 fPhDetP0D, fPhDetNxD, fPhDetNyD, fPhDetNzD;
  Double_t fdX, fdY, fdZ;
  UInt_t fiXmax, fiYmax;

 protected:
  char modName[20]; // name of module
  char eleName[20]; // substring for elements in module
 public:
  PndRichGeo();
  ~PndRichGeo() {}
  const char *getModuleName(Int_t);
  const char *getEleName(Int_t);
  inline Int_t getModNumInMod(const TString &);

  void init(size_t ver = 0);
  Double_t aerogelEntrancePositionZ() { return fAerogelEntrancePositionZ; }
  Double_t mirrorThetaMin() { return fMirrorThetaMin; }
  Double_t mirrorThetaMax() { return fMirrorThetaMax; }
  Double_t mirrorRadius() { return fMirrorRadius; }
  TVector3 mirrorAxis() { return fMirrorAxis; }
  TVector3 mirrorAxisGlob() { return fMirrorAxisGlob; }
  Double_t phDetAngle() { return fPhDetAngle; };
  TVector3 PhDetPositionLocal(TVector3 pos);
  TVector3 PhDetPositionGlobal(TVector3 pos);
  TVector3 PositionDiscretization(TVector3 pos, bool cell = true);
  TVector3 LocalPositionDiscretization(TVector3 pos, Double_t dX = -1, Double_t dY = -1, Double_t dZ = -1);
  UInt_t IndexX(TVector3 pos);
  UInt_t IndexY(TVector3 pos);
  TVector3 PixelPosition(UInt_t ix, UInt_t iy);
  TVector3 PixelPositionLocal(UInt_t ix, UInt_t iy);
  TVector3 PixelPositionGlobal(UInt_t ix, UInt_t iy);

  /*! */
  TVector3 richOffset() { return fRichOffset; }

  /*! */
  TVector3 alBoxSize() { return fAlBoxSize; }

  /*! */
  Double_t alBoxWallThickness() { return fAlBoxWallThickness; }

  /*! */
  TVector3 aerogelSize() { return fAerogelSize; }

  /*! */
  TVector3 aerogelOffset() { return fAerogelOffset; }

  /*! */
  std::vector<Double_t> nOpt() { return fnOpt; }

  /*! */
  std::vector<Double_t> aerogelLayers() { return fAerogelLayers; }

  /*! */
  Double_t angleExtansionInner() { return fAngleExtansionInner; }

  /*! */
  Double_t angleExtansionOuter() { return fAngleExtansionOuter; }

  /*! */
  Double_t mirrorCurvature() { return fMirrorCurvature; }

  /*! */
  Double_t angleOfMirrorPosition() { return fAngleOfMirrorPosition; }

  /*! */
  Double_t mirrorThickness() { return fMirrorThickness; }

  /*! */
  Double_t mirrorLength() { return fMirrorLength; }

  /*! */
  Double_t phDetLength() { return fPhDetLength; }

  /*! */
  Double_t phDetThickness() { return fPhDetThickness; }

  /*! */
  Double_t beamPipeHoleX() { return fBeamPipeHoleX; }

  /*! */
  Double_t beamPipeHoleY() { return fBeamPipeHoleY; }

  /*! */
  std::vector<Double_t> flatMirrorY() { return fFlatMirrorY; }

  /*! */
  std::vector<Double_t> flatMirrorZ() { return fFlatMirrorZ; }

  /*! */
  std::vector<Double_t> flatMirrorYGlob() { return fFlatMirrorYGlob; }

  /*! */
  std::vector<Double_t> flatMirrorZGlob() { return fFlatMirrorZGlob; }

  /*! */
  std::vector<Double_t> phDetY() { return fPhDetY; }

  /*! */
  std::vector<Double_t> phDetZ() { return fPhDetZ; }

  /*! */
  UInt_t sensorIndex() { return fSensorIndex; }

  /*! */
  UInt_t phDetNPixelMaxX() { return fPhDetPixelNumX; }

  /*! */
  UInt_t phDetNPixelMaxY() { return fPhDetPixelNumY; }

  /*! */
  Double_t phDetQEff(Double_t wl);

  /*! */
  Double_t phDetSizeX() { return fPhDetSizeX; }

  /*! */
  Double_t phDetSizeY() { return fPhDetSizeY; }

  /*! */
  Double_t phDetGapX() { return fPhDetGapX; }

  /*! */
  Double_t phDetGapY() { return fPhDetGapY; }

  /*! */
  UInt_t phDetNumX() { return fPhDetNumX; }

  /*! */
  UInt_t phDetNumY() { return fPhDetNumY; }

  /*! */
  UInt_t sensorsPerDevice() { return fSensorsPerDevice; }

  ClassDef(PndRichGeo, 1)
};

inline Int_t PndRichGeo::getModNumInMod(const TString &name)
{
  /** returns the module index from module name
   ?? in name[??] has to be the length of the detector name in the
   .geo file. For example if all nodes in this file starts with
   newdetector ?? has to be 11.
  */
  return (Int_t)(name[4] - '0') - 1; //
}

#endif
