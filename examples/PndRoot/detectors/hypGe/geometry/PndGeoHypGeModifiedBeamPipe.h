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

/*
 * PndGeoHypGeModifiedBeamPipe.h
 *
 * Copyright 2012 Marcell Steinen <steinen@kph.uni-mainz.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#ifndef PNDGEOHYPGEMODIFIEDBEAMPIPE_H
#define PNDGEOHYPGEMODIFIEDBEAMPIPE_H

#include "TGeoManager.h"
#include "TGeoMedium.h"
#include "TGeoTube.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include <stdio.h>

class PndGeoHypGeModifiedBeamPipe {
 private:
  TGeoMedium *Al;
  TGeoVolume *ModifiedBeamPipe;
  TGeoCombiTrans *ModifiedBeamPipeTransformation;

 public:
  // --------- constructor to use
  PndGeoHypGeModifiedBeamPipe(TGeoMedium *ExtAl, TGeoManager *geom);
  virtual ~PndGeoHypGeModifiedBeamPipe();

  void PlaceBeamPipe(TGeoVolume *top);
  void PlaceBeamPipe(TGeoVolume *top, TGeoMatrix *ModifiedBeamPipePlaceAndDirectionTransformation);
};

#endif /* PNDGEOHYPGEMODIFIEDBEAMPIPE_H */
