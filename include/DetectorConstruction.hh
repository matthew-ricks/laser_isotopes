#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"

class G4VPhysicalVolume;

class DetectorConstruction : public G4VUserDetectorConstruction {
public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    
    // Set and get the Z position of the liquid target
    void SetTargetZPosition(G4double zPos);

private:
    G4double fTargetZPosition;  // Updated variable name to match the new target
};

#endif