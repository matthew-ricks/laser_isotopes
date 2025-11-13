#pragma once

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include <sstream>
#include <set>

class RunAction;

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(RunAction* runAction);
    ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* step);

    std::string GetBufferedOutput() const;
    std::string GetPrimaryBufferOutput() const;

private:
    RunAction* fRunAction;
    std::ostringstream buffer;              // neutron info
    std::ostringstream uniquePrimaryBuffer; // unique primaries per event
    std::set<G4int> recordedEvents;         // to track unique eventIDs
    std::set<G4int> recordedNeutronTrackIDs;
};