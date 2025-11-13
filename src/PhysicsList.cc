#include "PhysicsList.hh"

// --- EM & decay ---
#include "G4DecayPhysics.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmExtraPhysics.hh"              // (optional but harmless)

// --- Hadronic core / models ---
#include "G4HadronElasticPhysicsHP.hh"      // HP elastic for neutrons
#include "G4HadronPhysicsINCLXX.hh"         // INCL++ cascade (good for light ions incl. d)
#include "G4StoppingPhysics.hh"
#include "G4NeutronTrackingCut.hh"

// --- Ions ---
#include "G4IonPhysicsXS.hh"                // Ion physics with XS (recommended in G4 11.x)

// Utils
#include "G4SystemOfUnits.hh"

PhysicsList::PhysicsList() : G4VModularPhysicsList() {
    // ========== EM & Decays ==========
    RegisterPhysics(new G4DecayPhysics());
    RegisterPhysics(new G4EmStandardPhysics_option4());
    RegisterPhysics(new G4EmExtraPhysics());  

    // ========== Hadronic ==========
    // HP elastic ensures accurate n elastic scattering at low energies.
    RegisterPhysics(new G4HadronElasticPhysicsHP());

    // INCL++ handles p/n/π and LIGHT IONS (d for example) well at lower energies.
    // It also hands off to appropriate models at higher energies.
    RegisterPhysics(new G4HadronPhysicsINCLXX());

    // Stopping physics (muons/ions at rest, etc.)
    RegisterPhysics(new G4StoppingPhysics());

    // Tracking cut for very slow neutrons
    RegisterPhysics(new G4NeutronTrackingCut());

    // ========== Ions ==========
    // Improved low-energy treatment for ions using evaluated cross sections.
    RegisterPhysics(new G4IonPhysicsXS());

    SetVerboseLevel(0);
}

void PhysicsList::SetCuts() {
    // Default cuts are fine for hadronic production; you can tighten EM cuts if needed.
    SetCutsWithDefault();
}