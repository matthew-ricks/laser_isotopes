#include "PrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "spectrum_cdf.hh" // exports: std::vector<double> energyBins (EDGES), cdf (on EDGES)
#include <cmath>
#include <algorithm>
#include <cassert>

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(),
      fParticleGun(new G4ParticleGun(1)),
      fNumBins(1000), fMinEnergy(energyBins.front()), fMaxEnergy(energyBins.back()),
      fEnergy(0), fThetaMax(0) {

    auto proton = G4ParticleTable::GetParticleTable()->FindParticle("proton");
    fParticleGun->SetParticleDefinition(proton);
    fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.));

    fEnergyHistogram.resize(fNumBins, 0);

    // Basic checks for the header tables
    assert(energyBins.size() == cdf.size());
    assert(energyBins.size() >= 2);
    for (size_t i = 1; i < cdf.size(); ++i) {
        assert(energyBins[i] > energyBins[i-1]); // strictly increasing edges
        assert(cdf[i] >= cdf[i-1]);              // monotone (allow flat steps)
    }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
    // --- Energy sampling (inverse CDF on EDGES) ---
    fEnergy = GenerateCustomEnergy();
    fParticleGun->SetParticleEnergy(fEnergy);

    // --- Bookkeeping histogram (optional) ---
    G4int bin = std::floor((fEnergy - fMinEnergy) / (fMaxEnergy - fMinEnergy) * fNumBins);
    if (bin >= 0 && bin < fNumBins) fEnergyHistogram[bin]++;


    // --- Angular sampling ---
    // full-angle divergence (degrees), convert to half-angle in radians
    const G4double full_angle_deg = 20.03 * std::exp(-0.26 * (fEnergy / MeV));
    const G4double theta_max_rad = (full_angle_deg * CLHEP::pi / 180.0) / 2.0;

    // Sample uniformly in solid angle inside the cone:
    // u ~ U[0,1], cosθ = 1 - u * (1 - cosθmax), θ = arccos(cosθ)
    const G4double cos_theta_max = std::cos(theta_max_rad);
    const G4double u = CLHEP::RandFlat::shoot(); // [0,1)
    const G4double cos_theta = 1.0 - u * (1.0 - cos_theta_max);
    const G4double theta = std::acos(cos_theta);

    const G4double phi = CLHEP::RandFlat::shoot(0., 2.*CLHEP::pi);

    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(
        std::sin(theta)*std::cos(phi),
        std::sin(theta)*std::sin(phi),
        std::cos(theta)
    ));

    fParticleGun->GeneratePrimaryVertex(event);
}

G4double PrimaryGeneratorAction::GenerateCustomEnergy() {
    // Inverse-CDF on EDGE arrays energyBins (E0..EN-1), cdf (C0..CN-1), with C0=0, CN-1=1
    const G4double r = CLHEP::RandFlat::shoot(); // [0,1)
    auto it = std::lower_bound(cdf.begin(), cdf.end(), r);
    size_t idx = std::distance(cdf.begin(), it);

    if (idx == 0) {
        // r == 0 -> return the min edge
        return energyBins.front() * MeV;
    }
    if (idx >= cdf.size()) {
        // r >= 1.0 (extremely rare if shoot() in [0,1)), clamp to max edge
        return energyBins.back() * MeV;
    }

    const G4double E1 = energyBins[idx - 1];
    const G4double E2 = energyBins[idx];
    const G4double C1 = cdf[idx - 1];
    const G4double C2 = cdf[idx];

    // Handle any tiny flat steps safely
    const G4double dC = (C2 - C1);
    if (dC <= 0.0) {
        // Degenerate CDF step: fall back to the right edge (or midpoint)
        return E2 * MeV; // or 0.5*(E1+E2)*MeV;
    }

    const G4double t = (r - C1) / dC; // in [0,1)
    const G4double E = E1 + t * (E2 - E1);
    return E * MeV;
}

const std::vector<G4int>& PrimaryGeneratorAction::GetEnergyHistogram() const {
    return fEnergyHistogram;
}