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
 * PndGeoHypGeModifiedCTFrame.cxx
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

#include "PndGeoHypGeModifiedCTFrame.h"

PndGeoHypGeModifiedCTFrame::PndGeoHypGeModifiedCTFrame(TGeoMedium *ExtAl, TGeoManager *geom)
{
  Al = ExtAl;
  ModifiedCTFrame = geom->MakeBox("ModifiedCTFrame", Al, 1, 40.5 / 2, 50);
  ModifiedCTFrame->SetLineColor(kGray);
}

PndGeoHypGeModifiedCTFrame::~PndGeoHypGeModifiedCTFrame() {}

void PndGeoHypGeModifiedCTFrame::BuildFrame()
{
  ModifiedCTFrameAssembly = new TGeoVolumeAssembly("ModifiedCTFrameAssembly");
  ModifiedCTFrameAssembly->AddNode(ModifiedCTFrame, 1, new TGeoTranslation(0, 45 - 40.5 / 2, 0));
  ModifiedCTFrameAssembly->AddNode(ModifiedCTFrame, 2, new TGeoTranslation(0, -45 + 40.5 / 2, 0));
}
void PndGeoHypGeModifiedCTFrame::PlaceCTFrame(TGeoVolume *top)
{
  BuildFrame();
  top->AddNode(ModifiedCTFrameAssembly, 1);
}

void PndGeoHypGeModifiedCTFrame::PlaceCTFrame(TGeoVolume *top, TGeoMatrix *ModifiedCTFramePlaceAndDirectionTransformation)
{
  BuildFrame();
  top->AddNode(ModifiedCTFrameAssembly, 1, ModifiedCTFramePlaceAndDirectionTransformation);
}
