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
 * File:   PndMQSdsHitLoader.tpl
 * @since 2014-02-08
 * @author: A. Rybalchenko, N. Winckler
 *
 */

#include "PndMQSdsHitLoader.h"

// Default implementation of PndMQSdsHitLoader::Exec() with Boost transport data format
void PndMQSdsHitLoader::Exec(Option_t *)
{
  // Default implementation of the base template Exec function using boost
  // the condition check if the input class has a function member with name
  // void serialize(boost::archive::binary_oarchive & ar, const unsigned int version) and if the payload are of boost type

  if (fHasBoostSerialization) {
    ostringstream buffer;
    boost::archive::binary_oarchive OutputArchive(buffer);
    for (Int_t i = 0; i < fInput->GetEntriesFast(); ++i) {
      PndSdsHit *digi = static_cast<PndSdsHit *>(fInput->At(i));
      if (!digi) {
        continue;
      }
      fDigiVector.push_back(*digi);
    }

    OutputArchive << fDigiVector;
    int size = buffer.str().length();
    fOutput = fTransportFactory->CreateMessage(size);
    memcpy(fOutput->GetData(), buffer.str().c_str(), size);

    // delete the vector content
    if (fDigiVector.size() > 0) {
      fDigiVector.clear();
    }
  } else {
    LOG(error) << "Boost Serialization not available";
  }
}
