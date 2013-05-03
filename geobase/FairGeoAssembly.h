#ifndef FAIRGEOASSEMBLY_H
#define FAIRGEOASSEMBLY_H

#include "FairGeoBasicShape.h"          // for FairGeoBasicShape

#include "Riosfwd.h"                    // for fstream
#include "Rtypes.h"                     // for FairGeoAssembly::Class, etc

#include <iosfwd>                       // for fstream

class FairGeoTransform;
class FairGeoVolume;
class TArrayD;

class FairGeoAssembly : public FairGeoBasicShape
{
  public:
    FairGeoAssembly();
    ~FairGeoAssembly();
    TArrayD* calcVoluParam(FairGeoVolume*);
    void calcVoluPosition(FairGeoVolume*,
                          const FairGeoTransform&,const FairGeoTransform&);
    Int_t readPoints(fstream* pFile,FairGeoVolume* volu);
    Bool_t writePoints(fstream*,FairGeoVolume*);
    void printPoints(FairGeoVolume* volu);

    ClassDef(FairGeoAssembly,0) // class for geometry shape ASSEMBLY
};

#endif  /* !FAIRGEOASSEMBLY_H */


