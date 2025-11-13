#ifndef PRIMARYGENERATORACTION_HH
#define PRIMARYGENERATORACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include <vector>

class G4ParticleGun;
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
    PrimaryGeneratorAction();
    ~PrimaryGeneratorAction() override;

    void GeneratePrimaries(G4Event* event) override;

    const std::vector<G4int>& GetEnergyHistogram() const;

private:
    G4ParticleGun* fParticleGun;

    G4double fEnergy;
    G4double fThetaMax;

    std::vector<G4int> fEnergyHistogram;
    G4int fNumBins;
    G4double fMinEnergy;
    G4double fMaxEnergy;

    G4double GenerateCustomEnergy();
};

#endif