#include "SteppingAction.hh"
#include "RunAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4Ions.hh"
#include "G4Threading.hh"
#include <set>

SteppingAction::SteppingAction(RunAction* runAction)
    : G4UserSteppingAction(), fRunAction(runAction) {}

SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step) {
    const G4Track* track = step->GetTrack();

    // Only log each new TRACK once (its first transport step)
    if (track->GetCurrentStepNumber() != 1) return;

    // Identify where it is created
    const auto* pre = step->GetPreStepPoint();
    if (!pre) return;
    const auto touch = pre->GetTouchableHandle();
    if (!touch) return;
    const G4VPhysicalVolume* vol = touch->GetVolume();
    if (!vol) return;
    const G4String volName = vol->GetName();

    // Limit to the Ra target volume (change name here if different)
    if (volName != "RaTarget") return;

    // Particle information
    const G4ParticleDefinition* pdef = track->GetDefinition();
    const G4String pname = pdef->GetParticleName();

    // We want *isotopes/nuclei* that are produced (residuals + daughters).
    // Geant4 represents heavy nuclei via G4Ions; light ions have dedicated types but still derive from nucleus category.
    // Use dynamic_cast to get Z, A, and excitation energy when available.
    const G4Ions* ion = dynamic_cast<const G4Ions*>(pdef);

    // Skip common non-nuclear particles explicitly
    if (pname == "gamma" || pname == "proton" || pname == "neutron" ||
        pname == "e-" || pname == "e+" || pname == "mu-" || pname == "mu+") {
        return;
    }

    // If this is NOT a nucleus (ion == nullptr), skip
    if (!ion) {
        return;
    }

    // Optional: if you want to ignore very light ions (e.g., d, t, He3, alpha),
    // uncomment the guard below (A >= 5 keeps boron and heavier). Leave commented to log *all* nuclei.
    // if (ion->GetAtomicMass() < 5) return;

    const auto pos = pre->GetPosition();
    const auto dir = track->GetMomentumDirection();
    const auto ekin = pre->GetKineticEnergy();

    const G4VProcess* creator = track->GetCreatorProcess();
    const G4String procName = creator ? creator->GetProcessName() : "primary";

    // Extract Z, A, excitation (eV)
    const G4int Z = ion->GetAtomicNumber();
    const G4int A = ion->GetAtomicMass();
    const G4double Ex = ion->GetExcitationEnergy(); // in keV by Geant4 convention

    // CSV: x,y,z, name, Z, A, Ex_keV, Ek, dirx, diry, dirz, process, volume, threadID
    buffer << pos.x() << "," << pos.y() << "," << pos.z() << ","
           << pname << "," << Z << "," << A << "," << Ex << ","
           << ekin << "," << dir.x() << "," << dir.y() << "," << dir.z() << ","
           << procName << "," << volName << "," << G4Threading::G4GetThreadId() << "\n";
}

std::string SteppingAction::GetBufferedOutput() const {
    return buffer.str();
}

std::string SteppingAction::GetPrimaryBufferOutput() const {
    return uniquePrimaryBuffer.str();
}