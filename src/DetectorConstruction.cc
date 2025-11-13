#include "DetectorConstruction.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Isotope.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
    G4NistManager* nist = G4NistManager::Instance();

    // --- Perfect vacuum (Galactic) -----------------------------------------
    G4Material* vacuum = nist->FindOrBuildMaterial("G4_Galactic");

    // --- World volume -------------------------------------------------------
    G4double world_size = 1.0 * m;
    auto* worldBox = new G4Box("World", world_size, world_size, world_size);
    auto* worldLog = new G4LogicalVolume(worldBox, vacuum, "World");
    auto* worldPhys = new G4PVPlacement(
        0, G4ThreeVector(), worldLog, "World", 0, false, 0, true);

    // --- Custom ²²⁶Ra material ------------------------------------------------
    auto* isotopeRa226 = new G4Isotope("Ra226", 88, 226, 226.02541 * g/mole);
    auto* Ra = new G4Element("Radium", "Ra", 1);
    Ra->AddIsotope(isotopeRa226, 100. * perCent);

    // Metallic radium density ~5.5 g/cm3
    auto* target_composition = new G4Material("Radium226", 5.5 * g/cm3, 1);
    target_composition->AddElement(Ra, 1);

    G4cout << "Target material: " << target_composition << G4endl;

    // --- 226Ra target geometry: 1 in dia SOLID disk, 1 mm thick ----------------
    const G4double target_radius      = (20 * mm);  
    const G4double target_thickness   = 10.0 * mm;            // 1 mm thick
    const G4double target_z_position  = (20 * mm) + (target_thickness / 2.0);

    // G4Tubs(innerR, outerR, halfLength, startPhi, deltaPhi)
    auto* targetTube = new G4Tubs("RaTarget",
                                  0.0 * mm,                // inner radius (solid disk)
                                  target_radius,           // outer radius
                                  target_thickness / 2.0,  // half length
                                  0.0 * deg, 360.0 * deg);

    auto* targetLog = new G4LogicalVolume(targetTube, target_composition, "RaTarget");

    new G4PVPlacement(0,
                      G4ThreeVector(0, 0, target_z_position),
                      targetLog,
                      "RaTarget",
                      worldLog,
                      false,
                      0,
                      true);

    // Optional: print geometry check values
    G4cout << "Ra disk outer radius = " << target_radius/mm
           << " mm, thickness = " << target_thickness/mm << " mm" << G4endl;

    return worldPhys;
}