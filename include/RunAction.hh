#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include "G4Run.hh"
#include <vector>
#include <string>
#include <mutex>

class SteppingAction;
class PrimaryGeneratorAction;

class RunAction : public G4UserRunAction {
public:
    RunAction();
    ~RunAction() override;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

    void SetSteppingAction(SteppingAction* stepper);
    void SetPrimaryGeneratorAction(PrimaryGeneratorAction* pga);

    static std::vector<std::string> bufferList;
    static std::vector<G4int> globalEnergyHistogram;
    static std::mutex bufferMutex;
    static std::mutex histogramMutex;

private:
    SteppingAction* steppingAction;
    PrimaryGeneratorAction* primaryGeneratorAction;

    G4int fNumBins;
    G4double fMinEnergy;
    G4double fMaxEnergy;
};

#endif