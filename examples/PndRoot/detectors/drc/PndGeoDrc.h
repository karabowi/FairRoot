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

// -------------------------------------------------------------------------
// -----                     PndGeoDrc header file                     -----
// -----               Created 11/10/06  by A. Cecchi                  -----
// -------------------------------------------------------------------------

#ifndef PNDGEODRC_H
#define PNDGEODRC_H

#include "FairGeoSet.h"

class PndGeoDrc : public FairGeoSet {

 private:
  double fNquartz;
  double fUquartz;
  double fNEV;
  double fRoughness;   //!< Quartz bar surface roughness [nm] = 10A
  double fGlueLayer;   //!< Thickness of the glue layer [cm].
  double fGreaseLayer; //!< Thickness of the layer of optical grease [cm]

  double fRadius;       //!< Barrel radius [cm] (middle of radiator)
  double fBarHalfThick; //!< Radiator bars half thickness [cm].
  double fBarWidth;     //!< Radiator width [cm].
  double fBarBoxZDown;  //!< Bar box down stream coordinate [cm].
  double fBarBoxZUp;    //!< Bar box up   stream coordinate [cm].
  double fBarhGap;      //!< Half gap between bars [cm].
  double fBarNum;       //!< Number of bars per barbox.
  double fBoxGap;       //!< Gap between bar and bar box [cm].
  double fBoxThick;     //!< Bar box thickness [cm].

  double fBarBoxNum;   //!< Total number of bar boxes in the barrel.
  double fBarBoxGap;   //!< Gap between neighboring bar boxes [cm].
  double fBarBoxAngle; //!< Angle between centers of the neighboring bar boxes [degrees].
  double fPipehAngle;  //!< Half of the phi angle taken by the target pipe [degrees].

  double fEVlength;    //!< Length of Expansion Volume [cm].
  double fEVdrop;      //!< [cm] drop of the EV - inner radius
  double fEVoffset;    //!< [cm] offset of the EV - outer radius
  double fEVbackAngle; //!< [degrees] angle btw PDplane and global Z axis

  // prism parameters:
  double fPhLength; //!< [cm] half length of the prism
  double fPAngle;   //!< [degrees] opening angle of the prism
  double fPDrop;    //!< [cm] drop of the prism - inner side
  double fPOffset;  //!< [cm] offset of the prism - outer side

  // MCP parameters:
  double fMCPactiveArea; //!< [cm] size of the active area of one MCP
  double fMCPsize;       //!< [cm] total size of one MCP
  double fMCPgap;        //!< [cm] gap between MCPs
  int fNpix;             //!< total number of pixels for one MCP (8 x 8 pixels)
  double fPixelSize;     //!< [cm] size of one pixel
  double fSigmaCharge;   //!< [cm] sigma of the charge sharing
  double fDeadTime;      //!< [ns] dead time of the photon detector

  double fLside;
  double fPi;

 protected:
  char modName[20]; // name of module
  char eleName[20]; // substring for elements in module
 public:
  PndGeoDrc();
  ~PndGeoDrc() {}
  const char *getModuleName(Int_t);
  const char *getEleName(Int_t);

  /*! The mean refraction index of fused silica (for reconstruction).
    \return The Nquartz. */
  Double_t nQuartz() { return fNquartz; }

  Double_t uQuartz() { return fUquartz; }

  /*! The refraction index of the expansion volume.
    \return The NEV. */
  Double_t nEV() { return fNEV; }

  /*! The roughness of the quartz bar surface.
    \return The fRoughness. */
  Double_t Roughness() { return fRoughness; }

  /*! The thickness of the glue layer.
    \return The fGlueLayer. */
  Double_t GlueLayer() { return fGlueLayer; }

  /*! The thickness of the optical grease layer.
    \return The fGreaseLayer. */
  Double_t GreaseLayer() { return fGlueLayer; }

  /*! The DRC barrel radius.
    \return The radius [cm]. */
  Double_t radius() { return fRadius; }

  /*! The DRC barrel bar half thickness.
    \return Half thickness [cm]. */
  Double_t barHalfThick() { return fBarHalfThick; }

  /*! The DRC barrel width.
    \return Width [cm]. */
  Double_t BarWidth() { return fBarWidth; }

  /*! The DRC barrel bar box downstream coordinate.
    \return Coordinate [cm]. */
  Double_t barBoxZDown() { return fBarBoxZDown; }

  /*! The DRC barrel bar box upstream coordinate.
    \return Coordinate [cm]. */
  Double_t barBoxZUp() { return fBarBoxZUp; }

  /*! The  half gap between bars.
    \return The BarGap [cm]. */
  Double_t barhGap() { return fBarhGap; }

  /*! The gap between bars and the bar box.
    \return The Gap [cm]. */
  Double_t boxGap() { return fBoxGap; }

  /*! The thickness of the bar box.
    \return The BoxThickness [cm]. */
  Double_t boxThick() { return fBoxThick; }

  /*! Number of bars per  bar box.
    \return The number */
  Double_t barNum() { return fBarNum; }

  /*! The length of the Expansion Volume [cm]. */
  Double_t EVlen() { return fEVlength; }

  /*! The Gap between neighboring bar boxes [degrees]. */
  Double_t BBoxGap() { return fBarBoxGap; }

  /*! The angle between centers of the neighboring bar boxes [degrees]. */
  Double_t BBoxAngle() { return fBarBoxAngle; }

  /*! Total number of bar boxes in the barrel. */
  Double_t BBoxNum() { return fBarBoxNum; }

  /*! Half of the phi angle taken by the target pipe [degrees]. */
  Double_t PipehAngle() { return fPipehAngle; }

  /*! The drop of the Expansion Volume [cm]. */
  Double_t EVdrop() { return fEVdrop; }

  /*! The offset of the Expansion Volume [cm]. */
  Double_t EVoffset() { return fEVoffset; }

  /*! The angle between the PDplane and global Z axis [degrees]. */
  Double_t EVbackAngle() { return fEVbackAngle; }

  /*! The offset of the Prism [cm]. */
  Double_t PrismOffset() { return fPOffset; }

  /*! The drop of the Prism [cm]. */
  Double_t PrismDrop() { return fPDrop; }

  /*! The half length of the Prism [cm]. */
  Double_t PrismhLength() { return fPhLength; }

  /*! The opening angle of the Prism [degrees]. */
  Double_t PrismAngle() { return fPAngle; }

  /*! The size of an MCP [cm]. */
  Double_t McpSize() { return fMCPsize; }

  /*! The size of the active area of an MCP [cm]. */
  Double_t McpActiveArea() { return fMCPactiveArea; }

  /*! The size of the gap between neighboring MCPs [cm]. */
  Double_t McpGap() { return fMCPgap; }

  /*! The total number of pixels for one MCPs. */
  Int_t Npixels() { return fNpix; }

  /*! The size of one pixel [cm]. */
  Double_t PixelSize() { return fPixelSize; }

  /*! The sigma of the charge sharing [cm]. */
  Double_t SigmaCharge() { return fSigmaCharge; }

  /*! The dead time of the photon detector [ns]. */
  Double_t DeadTime() { return fDeadTime; }

  /*! The width of the side [cm]. */
  Double_t Lside() { return fLside; }

  ClassDef(PndGeoDrc, 2) // Class for Drc
};

#endif /* !PNDGEODRC_H */
