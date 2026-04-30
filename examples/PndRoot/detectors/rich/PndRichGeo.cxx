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

#include "PndRichGeo.h"
#include "FairGeoNode.h"
#include "TRandom.h"

#include <fstream>

ClassImp(PndRichGeo)

  // -----   Default constructor   -------------------------------------------
  PndRichGeo::PndRichGeo()
  : FairGeoSet()
{
  // Constructor
  // fName has to be the name used in the geometry for all volumes.
  // If there is a mismatch the geometry cannot be build.
  fName = "rich";
  maxSectors = 0;
  maxModules = 10;

  fRichOffset = TVector3(0, 0, 700 - 35 - 10 /*+50*/); //-70
  fAlBoxSize = TVector3(600, 600, 100);
  fAlBoxWallThickness = 0.05;
  fAerogelSize = TVector3(0 * 590 + 1 * 290, 0 * 590 + 1 * 120, 4);
  fAerogelOffset = TVector3(0, 0, 1);
  fnOpt = std::vector<Double_t>(1, 1.05);
  fAerogelLayers = std::vector<Double_t>(1, 1);
  fAngleExtansionInner = 1.0;
  fAngleExtansionOuter = 1.0;
  fMirrorCurvature = 20;
  fAngleOfMirrorPosition = 50;
  fMirrorThickness = 0.1;
  fMirrorLength = 290;
  fPhDetLength = 290;
  fPhDetThickness = 3;
  fBeamPipeHoleX = 10;
  fBeamPipeHoleY = 10;

  fMirrorType = 0;
  fFlatMirrorZ = std::vector<Double_t>(2);
  fFlatMirrorY = std::vector<Double_t>(2);
  fFlatMirrorZGlob = std::vector<Double_t>(2);
  fFlatMirrorYGlob = std::vector<Double_t>(2);
  fPhDetZ = std::vector<Double_t>(2);
  fPhDetY = std::vector<Double_t>(2);

  fSenseLevel = 4;
  fSensorsPerDevice = 1 << (fSenseLevel - 1); // number of sensores per device in one direction
  // Quantum efficiency
  fPhDetDev = 0; // pde_dpc3200_22.dat
  fPhDetDev = 1; // pde_h12700.dat
  std::string workdir(getenv("VMCWORKDIR"));
  std::string effFileName = workdir;
  Double_t keff;
  if (fPhDetDev == 0) {
    effFileName += "/detectors/rich/pde_dpc3200_22.dat";
    fPhDetSizeX = 3.26;
    fPhDetSizeY = 3.26;
    fPhDetGapX = 0.01;
    fPhDetGapY = 0.01;
    keff = 1.7; // measured difference MC-EXP
  }
  if (fPhDetDev == 1) {
    effFileName += "/detectors/rich/pde_h12700.dat";
    fPhDetSizeX = 5.2;
    fPhDetSizeY = 5.2;
    fPhDetGapX = 0.01;
    fPhDetGapY = 0.01;
    keff = 1;
  }
  std::ifstream from(effFileName.c_str());
  Double_t wli, pdei;
  from >> wli >> pdei;
  while (!from.eof()) {
    fWlPhoton.push_back(wli);            // nm
    fPDE.push_back(pdei / 100.0 / keff); // %/100
    from >> wli >> pdei;
  };
  fPhDetEff = new TGraph(fWlPhoton.size(), fWlPhoton.data(), fPDE.data());
}

void PndRichGeo::init(size_t ver0)
{
  size_t nRefInd = (ver0 % 100000) / 1000;
  size_t nlayers = (ver0 % 1000) / 100;
  size_t ver = ver0 % 100;
  nlayers = nlayers ? nlayers : 3;

  double ka1 = angleExtansionInner();
  double ka2 = angleExtansionOuter();
  double beta = mirrorCurvature();
  double alpha = angleOfMirrorPosition();
  // Mirror
  double za = richOffset().Z() /*+ alBoxWallThickness()*/ + aerogelOffset().Z();
  double ya = aerogelSize().Y() / 2;
  double wa = aerogelSize().Z();
  double yp1 = ya + wa;
  double zp1 = wa;
  alpha = alpha < 45 - beta ? alpha : 45 - beta;
  alpha *= M_PI / 180;
  beta *= M_PI / 180;
  double thetaCh = acos(1 / nOpt()[0]);
  double theta = atan(ya / za);
  double alpha1 = 2 * (alpha + beta) - ka1 * thetaCh;
  double alpha2 = 2 * (alpha - beta) + ka2 * thetaCh + theta;
  double alpham = (alpha1 + alpha2) / 2;
  double zm1 = zp1 + yp1 / tan(alpha1);
  double ym1 = 0;
  double zm2 = (zm1 + ya * tan(alpha)) / (1 - tan(theta + ka2 * thetaCh) * tan(alpha));
  double ym2 = (zm2 - zm1) / tan(alpha);
  double zp2 = ((ym2 - yp1 + zm2 * tan(alpha2)) * tan(alpham) + zp1) / (1 + tan(alpha2) * tan(alpham));
  double yp2 = yp1 + (zp2 - zp1) / tan(alpham);
  if (yp2 < ym2) {
    yp2 = ym2;
    zp2 = zm2;
    alpham = atan((zp2 - zp1) / (yp2 - yp1));
  }
  // double phDetWidth = sqrt((zp1-zp2)*(zp1-zp2)+(yp1-yp2)*(yp1-yp2)); //[R.K. 01/2017] unused variable?
  double zmc = (zm1 + zm2) / 2;
  double ymc = (ym1 + ym2) / 2;
  double wm = sqrt((zm1 - zm2) * (zm1 - zm2) + (ym1 - ym2) * (ym1 - ym2));
  double rm = wm / 2 / sin(beta);
  double hm = rm * cos(beta);
  // double zm0 = zmc-hm*cos(alpha)-alBoxSize().Z()/2+aerogelOffset().Z(); //[R.K. 01/2017] unused variable?
  // double ym0 = ymc+hm*sin(alpha); //[R.K. 01/2017] unused variable?
  double theta1 = 360 - (alpha + beta) * 180 / M_PI;
  double theta2 = 360 - (alpha - beta) * 180 / M_PI;
  // double theta3 = 360-theta2; //[R.K. 01/2017] unused variable?

  // photosensor pixel sizes
  // http://www.digitalphotoncounting.com/wp-content/uploads/PDPC_leaflet_A4_2015_10.pdf
  fdX = 0.4075 / 2; // 0.38016;
  fdY = 0.4075 / 2; // 0.32;
  fdZ = 0;
  fiXmax = 2 * (int)(fMirrorLength / 2 / fdX);
  fiYmax = 0; // see further

  fAerogelEntrancePositionZ = za;

  fMirrorRadius = rm;
  fMirrorAxis = TVector3(0, ymc + hm * sin(alpha), za + zmc - hm * cos(alpha));
  theta1 -= theta1 < 180 ? 0 : 360;
  theta2 -= theta2 < 180 ? 0 : 360;
  fMirrorThetaMin = theta1 * M_PI / 180;
  fMirrorThetaMax = theta2 * M_PI / 180;

  UInt_t nm;
  Double_t yShift, zShift, dn;

  switch (nlayers) {
  case 1:
    fnOpt.resize(1);
    fnOpt[0] = 1.05;
    fAerogelLayers.resize(1);
    fAerogelLayers[0] = 1;
    break;
  case 2:
    fnOpt.resize(2);
    dn = nRefInd ? (nRefInd - 1) * 0.00005 : 0.000817682;
    fnOpt[0] = 1.05 - dn;
    fnOpt[1] = 1.05 + dn;
    fAerogelLayers.resize(2);
    fAerogelLayers[0] = 0.5;
    fAerogelLayers[1] = 0.5;
    break;
  case 3:
    fnOpt.resize(3);
    dn = nRefInd ? (nRefInd - 1) * 0.00005 : 0.00108652;
    fnOpt[0] = 1.05 - dn;
    fnOpt[1] = 1.05;
    fnOpt[2] = 1.05 + dn;
    fAerogelLayers.resize(3);
    fAerogelLayers[0] = 0.333333;
    fAerogelLayers[1] = 0.333334;
    fAerogelLayers[2] = 0.333333;
    break;
  case 4:
    fnOpt.resize(4);
    dn = nRefInd ? (nRefInd - 1) * 0.00005 : 0.00122976;
    fnOpt[0] = 1.05 - dn;
    fnOpt[1] = 1.05;
    fnOpt[2] = 1.05;
    fnOpt[3] = 1.05 + dn;
    fAerogelLayers.resize(4);
    fAerogelLayers[0] = 0.25;
    fAerogelLayers[1] = 0.25;
    fAerogelLayers[2] = 0.25;
    fAerogelLayers[3] = 0.25;
    break;
  case 5:
    fnOpt.resize(5);
    fnOpt[0] = 1.05;
    fnOpt[1] = 1.05;
    fnOpt[2] = 1.05;
    fnOpt[3] = 1.05;
    fnOpt[4] = 1.05;
    fAerogelLayers.resize(5);
    fAerogelLayers[0] = 0.2;
    fAerogelLayers[1] = 0.2;
    fAerogelLayers[2] = 0.2;
    fAerogelLayers[3] = 0.2;
    fAerogelLayers[4] = 0.2;
    break;
  default: break;
  }
  yShift = 0;
  zShift = richOffset().Z() + aerogelOffset().Z();
  switch (ver) {
  case 1: // round mirror
    fPhDetY[0] = 60;
    fPhDetY[1] = 82.5719;
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = 58.957;
    fMirrorRadius = 130.35;
    fMirrorAxis = TVector3(0, 100.366, -70.1726);                       // z relative to aerogel entrence
    fMirrorAxisGlob = TVector3(0, 100.366 + yShift, -70.1726 + zShift); // z relative to aerogel entrence
    fMirrorThetaMin = -0.878805;
    fMirrorThetaMax = -0.13694;
    break;
  case 11: // flat mirror (one part)
  case 21: // flat mirror (one part) + side mirrors
    fFlatMirrorY.resize(2);
    fFlatMirrorZ.resize(2);
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 113.511542159065;
    fFlatMirrorZ[0] = 47.6318222110121;
    fFlatMirrorZ[1] = 129.893234066359;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[1];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[1];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 12: // flat mirror (two part)
  case 22: // flat mirror (two part) + side mirrors
    fFlatMirrorY.resize(3);
    fFlatMirrorZ.resize(3);
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 30.7192103643191;
    fFlatMirrorY[2] = 91.7457580720718;
    fFlatMirrorZ[0] = 21.0968997324958;
    fFlatMirrorZ[1] = 53.4669621286154;
    fFlatMirrorZ[2] = 79.0929943996688;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[2];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[2];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 13: // flat mirror (three part)
  case 23: // flat mirror (three part) + side mirrors
    fFlatMirrorY.resize(4);
    fFlatMirrorZ.resize(4);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 15.7768998527303;
    fFlatMirrorY[2] = 53.8249581943035;
    fFlatMirrorY[3] = 86.6039907197112;
    fFlatMirrorZ[0] = 19.8059481359439;
    fFlatMirrorZ[1] = 36.7817321826854;
    fFlatMirrorZ[2] = 63.6406349592571;
    fFlatMirrorZ[3] = 67.0923693467737;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[3];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[3];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 14: // flat mirror (four part)
  case 24: // flat mirror (four part) + side mirrors
    fFlatMirrorY.resize(5);
    fFlatMirrorZ.resize(5);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 8.03026468817330;
    fFlatMirrorY[2] = 30.7475489960267;
    fFlatMirrorY[3] = 59.1955006448811;
    fFlatMirrorY[4] = 84.0239842069457;
    fFlatMirrorZ[0] = 18.5825566438019;
    fFlatMirrorZ[1] = 27.3977855428941;
    fFlatMirrorZ[2] = 47.7739516802010;
    fFlatMirrorZ[3] = 60.7121551030865;
    fFlatMirrorZ[4] = 61.0707645809510;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[4];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[4];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 15: // flat mirror (five part)
  case 25: // flat mirror (five part) + side mirrors
    fFlatMirrorY.resize(6);
    fFlatMirrorZ.resize(6);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 6.43722379520563;
    fFlatMirrorY[2] = 24.4256033426640;
    fFlatMirrorY[3] = 49.2742119933454;
    fFlatMirrorY[4] = 65.6045650294544;
    fFlatMirrorY[5] = 82.9932569195603;
    fFlatMirrorZ[0] = 19.7751084893358;
    fFlatMirrorZ[1] = 26.7049745668535;
    fFlatMirrorZ[2] = 43.2466340126002;
    fFlatMirrorZ[3] = 57.0589176253026;
    fFlatMirrorZ[4] = 60.4234882450729;
    fFlatMirrorZ[5] = 58.6650992017072;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[5];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[5];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 16: // flat mirror (six part)
  case 26: // flat mirror (six part) + side mirrors
    fFlatMirrorY.resize(7);
    fFlatMirrorZ.resize(7);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 4.32030152236946;
    fFlatMirrorY[2] = 16.3735214678972;
    fFlatMirrorY[3] = 34.0234063082931;
    fFlatMirrorY[4] = 53.4151371528727;
    fFlatMirrorY[5] = 67.4943304893323;
    fFlatMirrorY[6] = 82.0821597606174;
    fFlatMirrorZ[0] = 20.0304540223155;
    fFlatMirrorZ[1] = 24.6621130797753;
    fFlatMirrorZ[2] = 36.3146724796001;
    fFlatMirrorZ[3] = 48.7721721707139;
    fFlatMirrorZ[4] = 56.7889550463438;
    fFlatMirrorZ[5] = 58.5790965356983;
    fFlatMirrorZ[6] = 56.5386444942197;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[6];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[6];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 17: // flat mirror (seven part)
  case 27: // flat mirror (seven part) + side mirrors
    fFlatMirrorY.resize(8);
    fFlatMirrorZ.resize(8);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 2.07504791151454;
    fFlatMirrorY[2] = 8.03938318086690;
    fFlatMirrorY[3] = 22.6202807022538;
    fFlatMirrorY[4] = 38.2473277661409;
    fFlatMirrorY[5] = 55.1167285140505;
    fFlatMirrorY[6] = 68.1475547317666;
    fFlatMirrorY[7] = 81.4987977109529;
    fFlatMirrorZ[0] = 19.0632831569999;
    fFlatMirrorZ[1] = 21.3232597676276;
    fFlatMirrorZ[2] = 27.5080822022865;
    fFlatMirrorZ[3] = 40.5611035370563;
    fFlatMirrorZ[4] = 49.9253177088730;
    fFlatMirrorZ[5] = 56.0112344905128;
    fFlatMirrorZ[6] = 57.2586668011183;
    fFlatMirrorZ[7] = 55.1771069627915;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[7];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[7];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 18: // flat mirror (eight part)
  case 28: // flat mirror (eight part) + side mirrors
    fFlatMirrorY.resize(9);
    fFlatMirrorZ.resize(9);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 1.53560904998618;
    fFlatMirrorY[2] = 5.89212190378038;
    fFlatMirrorY[3] = 17.5565807772290;
    fFlatMirrorY[4] = 30.0104598999171;
    fFlatMirrorY[5] = 43.1204560600741;
    fFlatMirrorY[6] = 57.0242748178388;
    fFlatMirrorY[7] = 69.0047342707571;
    fFlatMirrorY[8] = 81.1329884527176;
    fFlatMirrorZ[0] = 19.5409594890924;
    fFlatMirrorZ[1] = 21.2004105360782;
    fFlatMirrorZ[2] = 25.7419689325749;
    fFlatMirrorZ[3] = 36.6887140347995;
    fFlatMirrorZ[4] = 45.2863228266215;
    fFlatMirrorZ[5] = 51.5860160769717;
    fFlatMirrorZ[6] = 55.7435949020503;
    fFlatMirrorZ[7] = 56.4461575595980;
    fFlatMirrorZ[8] = 54.3233266479462;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[8];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[8];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  case 19: // flat mirror (nine part)
  case 29: // flat mirror (nine part) + side mirrors
    fFlatMirrorY.resize(10);
    fFlatMirrorZ.resize(10);
    // with refraction on the surface
    fFlatMirrorY[0] = 0;
    fFlatMirrorY[1] = 1.49645897228357;
    fFlatMirrorY[2] = 5.64778607612293;
    fFlatMirrorY[3] = 16.5322752536511;
    fFlatMirrorY[4] = 27.8183214052547;
    fFlatMirrorY[5] = 40.2374704149686;
    fFlatMirrorY[6] = 52.9871945876487;
    fFlatMirrorY[7] = 62.1756927165643;
    fFlatMirrorY[8] = 71.5135677331208;
    fFlatMirrorY[9] = 80.8735573790680;
    fFlatMirrorZ[0] = 20.3538808252916;
    fFlatMirrorZ[1] = 21.9497914345741;
    fFlatMirrorZ[2] = 26.2257519619150;
    fFlatMirrorZ[3] = 36.3783289992003;
    fFlatMirrorZ[4] = 44.3190814791673;
    fFlatMirrorZ[5] = 50.6718607175374;
    fFlatMirrorZ[6] = 54.9437977944303;
    fFlatMirrorZ[7] = 56.2339910167212;
    fFlatMirrorZ[8] = 55.8374506290110;
    fFlatMirrorZ[9] = 53.7178276378618;
    fPhDetY[0] = 60;
    fPhDetY[1] = fFlatMirrorY[9];
    fPhDetZ[0] = 5.00000000000000;
    fPhDetZ[1] = fFlatMirrorZ[9];
    //
    nm = fFlatMirrorZ.size();
    fFlatMirrorYGlob.resize(nm);
    fFlatMirrorZGlob.resize(nm);
    yShift = 0;
    zShift = richOffset().Z() + aerogelOffset().Z();
    for (UInt_t i = 0; i < nm; i++) {
      fFlatMirrorYGlob[i] = fFlatMirrorY[i] + yShift;
      fFlatMirrorZGlob[i] = fFlatMirrorZ[i] + zShift;
    }
    break;
  default: break;
  }
  fPhDetAngle = std::atan((fPhDetY[1] - fPhDetY[0]) / (fPhDetZ[1] - fPhDetZ[0]));
  fPhDetP0U = TVector3(0, fPhDetY[0] + yShift, fPhDetZ[0] + zShift);
  fPhDetNxU = TVector3(1, 0, 0);
  fPhDetNyU = TVector3(0, fPhDetY[1] - fPhDetY[0], fPhDetZ[1] - fPhDetZ[0]).Unit();
  fPhDetNzU = fPhDetNxU.Cross(fPhDetNyU).Unit();
  fPhDetP0D = TVector3(0, -fPhDetY[0] - yShift, fPhDetZ[0] + zShift);
  fPhDetNxD = TVector3(1, 0, 0);
  fPhDetNyD = TVector3(0, fPhDetY[1] - fPhDetY[0], -(fPhDetZ[1] - fPhDetZ[0])).Unit();
  fPhDetNzD = fPhDetNxD.Cross(fPhDetNyD).Unit();
  fiYmax = (int)(2 * std::sqrt((fPhDetY[1] - fPhDetY[0]) * (fPhDetY[1] - fPhDetY[0]) + (fPhDetZ[1] - fPhDetZ[0]) * (fPhDetZ[1] - fPhDetZ[0])) / fdY);
  // number of photo-devices
  Double_t phDetWidth = std::sqrt((fPhDetY[1] - fPhDetY[0]) * (fPhDetY[1] - fPhDetY[0]) + (fPhDetZ[1] - fPhDetZ[0]) * (fPhDetZ[1] - fPhDetZ[0]));
  fPhDetNumX = 2 * (int)(fMirrorLength / 2 / (fPhDetSizeX + fPhDetGapX) + 1);
  fPhDetNumY = 2 * (int)(phDetWidth / (fPhDetSizeY + fPhDetGapY) + 1);
  fPhDetPixelNumX = fSensorsPerDevice * fPhDetNumX;
  fPhDetPixelNumY = fSensorsPerDevice * fPhDetNumY;
}

Double_t PndRichGeo::phDetQEff(Double_t wl)
{
  return ((wl >= fWlPhoton.front()) && (wl <= fWlPhoton.back())) ? fPhDetEff->Eval(wl) : 0;
}

TVector3 PndRichGeo::PhDetPositionLocal(TVector3 pos)
{
  // photodetector: cell size
  if (pos.Y() >= 0) {
    TVector3 dP = pos - fPhDetP0U;
    return TVector3(dP * fPhDetNxU, dP * fPhDetNyU, dP * fPhDetNzU);
  } else {
    TVector3 dP = pos - fPhDetP0D;
    return TVector3(dP * fPhDetNxD, dP * fPhDetNyD, dP * fPhDetNzD);
  }
}

TVector3 PndRichGeo::PhDetPositionGlobal(TVector3 pos)
{
  if (pos.Y() >= 0)
    return fPhDetP0U + pos.X() * fPhDetNxU + pos.Y() * fPhDetNyU + pos.Z() * fPhDetNzU;
  else
    return fPhDetP0D + pos.X() * fPhDetNxD + pos.Y() * fPhDetNyD + pos.Z() * fPhDetNzD;
}

TVector3 PndRichGeo::PositionDiscretization(TVector3 pos, bool cell)
{
  // full variant
  TVector3 posl = PhDetPositionLocal(pos);
  Double_t xl = posl.X();
  Double_t yl = posl.Y();
  // to local coordinate system of the device
  Double_t wx = fPhDetSizeX + fPhDetGapX;
  UInt_t Ix = xl / wx + fPhDetNumX / 2;
  Double_t xlc = wx * (Ix + 0.5 - fPhDetNumX / 2);
  Double_t wy = fPhDetSizeY + fPhDetGapY;
  UInt_t Iy = yl / wy + fPhDetNumY / 2;
  Double_t ylc = wy * (Iy + 0.5 - fPhDetNumY / 2);
  xl -= xlc;
  yl -= ylc;
  //
  Double_t xc[4];
  Double_t yc[4];
  Double_t dxc[4];
  Double_t dyc[4];
  Double_t xcl3[2];
  Double_t ycl3[2];
  Double_t xcl2;
  Double_t ycl2;
  Double_t dx;
  Double_t dy;
  bool gep;
  // dpc3200-22
  // http://www.digitalphotoncounting.com/wp-content/uploads/PDPC_leaflet_A4_2015_10.pdf
  if (fPhDetDev == 0) {
    // pixel center x,y, pixel half widths wx,wy
    xc[0] = 0.23;
    yc[0] = 0.195;
    dxc[0] = 0.16;
    dyc[0] = 0.19;
    xc[1] = 0.56;
    yc[1] = 0.585;
    dxc[1] = 0.16;
    dyc[1] = 0.19;
    xc[2] = 1.02;
    yc[2] = 0.975;
    dxc[2] = 0.16;
    dyc[2] = 0.19;
    xc[3] = 1.35;
    yc[3] = 1.365;
    dxc[3] = 0.16;
    dyc[3] = 0.19;
    // die center x,y
    xcl3[0] = 0.395;
    ycl3[0] = 0.39; // {(xc[0]+xc[1])/2,(xc[2]+xc[3])/2}
    xcl3[1] = 1.185;
    ycl3[1] = 1.17; // {(yc[0]+yc[1])/2,(yc[2]+yc[3])/2}
    // quarter center x,y
    xcl2 = 0.79; // (xc[1]+xc[2])/2
    ycl2 = 0.78; // (yc[1]+yc[2])/2
    //
    dx = 0.395;
    dy = 0.39;
    // geometrical efficiency of pixel (cell filling)
    gep = gRandom->Uniform() <= (cell ? 0.74 : 1);
  }
  // h12700
  // https://www.hamamatsu.com/resources/pdf/etd/H12700_TPMH1348E.pdf
  if (fPhDetDev == 1) {
    // pixel center x,y, pixel half widths wx,wy
    xc[0] = 0.3;
    yc[0] = 0.3;
    dxc[0] = 0.3;
    dyc[0] = 0.3;
    xc[1] = 0.9;
    yc[1] = 0.9;
    dxc[1] = 0.3;
    dyc[1] = 0.3;
    xc[2] = 1.5;
    yc[2] = 1.5;
    dxc[2] = 0.3;
    dyc[2] = 0.3;
    xc[3] = 2.1125;
    yc[3] = 2.1125;
    dxc[3] = 0.3125;
    dyc[3] = 0.3125;
    // die center x,y
    xcl3[0] = 0.6;
    ycl3[0] = 0.6; // {(xc[0]+xc[1])/2,(xc[2]+xc[3])/2}
    xcl3[1] = 1.8125;
    ycl3[1] = 1.8125; // {(yc[0]+yc[1])/2,(yc[2]+yc[3])/2}
    // quarter center x,y
    xcl2 = 1.2125; // ~(xc[1]+xc[2])/2
    ycl2 = 1.2125; // ~(yc[1]+yc[2])/2
    //
    dx = 0.6;
    dy = 0.6;
    // geometrical efficiency of pixel (cell filling)
    gep = true;
  }
  Int_t sx = xl > 0 ? 1 : -1;
  Int_t sy = yl > 0 ? 1 : -1;
  UInt_t ix = (sx * xl) / dx;
  UInt_t iy = (sy * yl) / dy;
  ix = ix > 3 ? 3 : ix;
  iy = iy > 3 ? 3 : iy;
  // pixel level = 4
  Double_t hx = sx * xc[ix];
  Double_t hy = sy * yc[iy];
  if ((std::fabs(xl - hx) < dxc[ix]) && (std::fabs(yl - hy) < dyc[iy]) && gep) {
    // tile level = 1
    if (fSenseLevel == 1) {
      hx = 0;
      hy = 0;
      fSensorIndexX = Ix;
      fSensorIndexY = Iy;
    }
    // qurter level = 2
    if (fSenseLevel == 2) {
      hx = sx * xcl2;
      hy = sy * ycl2;
      fSensorIndexX = Ix * fSensorsPerDevice + (sx < 0 ? 0 : 1);
      fSensorIndexY = Iy * fSensorsPerDevice + (sy < 0 ? 0 : 1);
    }
    // die level = 3
    if (fSenseLevel == 3) {
      UInt_t ixx = ix / 2;
      UInt_t iyy = iy / 2;
      hx = sx * xcl3[ixx];
      hy = sy * ycl3[iyy];
      fSensorIndexX = Ix * fSensorsPerDevice + (sx < 0 ? 1 : 2) + sx * ixx;
      fSensorIndexY = Iy * fSensorsPerDevice + (sy < 0 ? 1 : 2) + sy * iyy;
    }
    // pixel level = 4
    if (fSenseLevel == 4) {
      fSensorIndexX = Ix * fSensorsPerDevice + (sx < 0 ? 3 : 4) + sx * ix;
      fSensorIndexY = Iy * fSensorsPerDevice + (sy < 0 ? 3 : 4) + sy * iy;
    }
    fSensorIndex = fPhDetNumX * fSensorIndexY + fSensorIndexX;
    fSensorPosition = PhDetPositionGlobal(TVector3(hx + xlc, hy + ylc, posl.Z()));
    return fSensorPosition; // photon hits a phdet
  } else {
    fSensorIndexX = -1;
    fSensorIndexY = -1;
    fSensorIndex = -1;
    fSensorPosition = TVector3(0, 0, 0);
    return fSensorPosition; // photon conversion out of sensitive region
  }
}

TVector3 PndRichGeo::PixelPosition(UInt_t ix, UInt_t iy)
{
  UInt_t Ix = ix / fSensorsPerDevice;       // device intex
  UInt_t Iy = iy / fSensorsPerDevice;       // device index
  UInt_t ixl = ix - Ix * fSensorsPerDevice; // local index
  UInt_t iyl = iy - Iy * fSensorsPerDevice; // local index
  UInt_t nsh = fSensorsPerDevice / 2;
  Int_t sx = ixl < nsh ? -1 : 1;
  Int_t sy = iyl < nsh ? -1 : 1;
  UInt_t ixm = ixl < nsh ? nsh - ixl - 1 : ixl - nsh;
  UInt_t iym = iyl < nsh ? nsh - iyl - 1 : iyl - nsh;
  //
  Double_t hx = (Ix + 0.5 - fPhDetNumX / 2) * (fPhDetSizeX + fPhDetGapX); // div. center position (x)
  Double_t hy = (Iy + 0.5 - fPhDetNumY / 2) * (fPhDetSizeY + fPhDetGapY); // div. center position (y)
  //
  Double_t xc[4];
  Double_t yc[4];
  // Double_t dxc[4]; //[R.K.04/2017] unused
  // Double_t dyc[4]; //[R.K.04/2017] unused
  Double_t xcl3[2];
  Double_t ycl3[2];
  Double_t xcl2;
  Double_t ycl2;
  // dpc3200-22
  // http://www.digitalphotoncounting.com/wp-content/uploads/PDPC_leaflet_A4_2015_10.pdf
  if (fPhDetDev == 0) {
    // pixel center x,y, pixel half widths wx,wy
    xc[0] = 0.23;
    yc[0] = 0.195; // dxc[0] = 0.16; dyc[0] = 0.19; //[R.K.04/2017] unused
    xc[1] = 0.56;
    yc[1] = 0.585; // dxc[1] = 0.16; dyc[1] = 0.19; //[R.K.04/2017] unused
    xc[2] = 1.02;
    yc[2] = 0.975; // dxc[2] = 0.16; dyc[2] = 0.19; //[R.K.04/2017] unused
    xc[3] = 1.35;
    yc[3] = 1.365; // dxc[3] = 0.16; dyc[3] = 0.19; //[R.K.04/2017] unused
    // die center x,y
    xcl3[0] = 0.395;
    ycl3[0] = 0.39; // {(xc[0]+xc[1])/2,(xc[2]+xc[3])/2}
    xcl3[1] = 1.185;
    ycl3[1] = 1.17; // {(yc[0]+yc[1])/2,(yc[2]+yc[3])/2}
    // quarter center x,y
    xcl2 = 0.79; // (xc[1]+xc[2])/2
    ycl2 = 0.78; // (yc[1]+yc[2])/2
  }
  // h12700
  // https://www.hamamatsu.com/resources/pdf/etd/H12700_TPMH1348E.pdf
  if (fPhDetDev == 1) {
    // pixel center x,y, pixel half widths wx,wy
    xc[0] = 0.3;
    yc[0] = 0.3; // dxc[0] = 0.3; dyc[0] = 0.3; //[R.K.04/2017] unused
    xc[1] = 0.9;
    yc[1] = 0.9; // dxc[1] = 0.3; dyc[1] = 0.3; //[R.K.04/2017] unused
    xc[2] = 1.5;
    yc[2] = 1.5; // dxc[2] = 0.3; dyc[2] = 0.3; //[R.K.04/2017] unused
    xc[3] = 2.1125;
    yc[3] = 2.1125; // dxc[3] = 0.3125; dyc[3] = 0.3125; //[R.K.04/2017] unused
    // die center x,y
    xcl3[0] = 0.6;
    ycl3[0] = 0.6; // {(xc[0]+xc[1])/2,(xc[2]+xc[3])/2}
    xcl3[1] = 1.8125;
    ycl3[1] = 1.8125; // {(yc[0]+yc[1])/2,(yc[2]+yc[3])/2}
    // quarter center x,y
    xcl2 = 1.2125; // ~(xc[1]+xc[2])/2
    ycl2 = 1.2125; // ~(yc[1]+yc[2])/2
  }
  // tile level = 1
  if (fSenseLevel == 1) {
    hx += 0;
    hy += 0;
  }
  // qurter level = 2
  if (fSenseLevel == 2) {
    hx += sx * xcl2;
    hy += sy * ycl2;
  }
  // die level = 3
  if (fSenseLevel == 3) {
    hx += sx * xcl3[ixm];
    hy += sy * ycl3[iym];
  }
  // pixel level = 4
  if (fSenseLevel == 4) {
    hx += sx * xc[ixm];
    hy += sy * yc[iym];
  }
  fSensorIndexX = ix;
  fSensorIndexY = iy;
  fSensorIndex = fPhDetNumX * fSensorIndexY + fSensorIndexX;
  return PhDetPositionGlobal(TVector3(hx, hy, 0));
}

TVector3 PndRichGeo::LocalPositionDiscretization(TVector3 pos, Double_t dX, Double_t dY, Double_t dZ)
{
  // simple variant
  Double_t dX_ = dX > 0 ? dX : fdX;
  Double_t dY_ = dY > 0 ? dY : fdY;
  Double_t dZ_ = dZ > 0 ? dZ : fdZ;
  Double_t x = pos.X();
  Double_t y = pos.Y();
  Double_t z = pos.Z();
  if (x && dX)
    x = ((int)(x / dX_) + x / std::fabs(x) / 2) * dX_;
  if (y && dY)
    y = ((int)(y / dY_) + y / std::fabs(y) / 2) * dY_;
  if (z && dZ)
    z = ((int)(z / dZ_) + z / std::fabs(z) / 2) * dZ_;
  return TVector3(x, y, z);
}

UInt_t PndRichGeo::IndexX(TVector3 pos)
{
  if (pos != fSensorPosition)
    PositionDiscretization(pos, false);
  return fSensorIndexX;
  // return (int)((pos.X()+fdX*fiXmax/2)/fdX);
}

UInt_t PndRichGeo::IndexY(TVector3 pos)
{
  if (pos != fSensorPosition)
    PositionDiscretization(pos, false);
  return fSensorIndexY;
  // return (int)((pos.Y()+fdY*fiXmax/2)/fdY);
}

TVector3 PndRichGeo::PixelPositionLocal(UInt_t ix, UInt_t iy)
{
  Double_t x = ix * fdX - fiXmax * fdX / 2 + 0.5 * fdX;
  Double_t y = iy * fdY - fiYmax * fdY / 2 + 0.5 * fdY;
  return TVector3(x, y, 0);
}

TVector3 PndRichGeo::PixelPositionGlobal(UInt_t ix, UInt_t iy)
{
  return PhDetPositionGlobal(PixelPositionLocal(ix, iy));
}

// -------------------------------------------------------------------------

const char *PndRichGeo::getModuleName(Int_t m)
{
  /** Returns the module name of PndRich number m
      Setting PndRich here means that all modules names in the
      ASCII file should start with PndRich otherwise they will
      not be constructed
  */
  sprintf(modName, "rich0%i", m + 1);
  return modName;
}

const char *PndRichGeo::getEleName(Int_t m)
{
  /** Returns the element name of Det number m */
  sprintf(eleName, "rich0%i", m + 1);
  return eleName;
}
