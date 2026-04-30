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
 * PndGeoHypGeModifiedBeamPipe.cpp
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

#include "PndGeoHypGeModifiedBeamPipe.h"

// --------- constructor to use
PndGeoHypGeModifiedBeamPipe::PndGeoHypGeModifiedBeamPipe(TGeoMedium *ExtAl, TGeoManager *geom)
{
  Al = ExtAl;
  ModifiedBeamPipe = geom->MakeTube("ModifiedBeamPipe", Al, 4.4, 4.5, 150); // simple version atm!
  ModifiedBeamPipe->SetLineColor(kGray);
}

PndGeoHypGeModifiedBeamPipe::~PndGeoHypGeModifiedBeamPipe() {}

void PndGeoHypGeModifiedBeamPipe::PlaceBeamPipe(TGeoVolume *top)
{
  top->AddNode(ModifiedBeamPipe, 1);
}

void PndGeoHypGeModifiedBeamPipe::PlaceBeamPipe(TGeoVolume *top, TGeoMatrix *ModifiedBeamPipePlaceAndDirectionTransformation)
{
  top->AddNode(ModifiedBeamPipe, 1, ModifiedBeamPipePlaceAndDirectionTransformation);
}
