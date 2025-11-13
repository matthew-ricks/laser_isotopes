#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"

ActionInitialization::ActionInitialization() {}
ActionInitialization::~ActionInitialization() {}

void ActionInitialization::BuildForMaster() const {
    auto runAction = new RunAction();
    SetUserAction(runAction);
}

void ActionInitialization::Build() const {
    // Create components
    auto generator = new PrimaryGeneratorAction();
    auto runAction = new RunAction();
    auto stepper = new SteppingAction(runAction);

    // Register actions
    SetUserAction(generator);
    SetUserAction(runAction);
    SetUserAction(stepper);

    // Link generator to runAction so histogram data can be merged
    runAction->SetPrimaryGeneratorAction(generator);
    runAction->SetSteppingAction(stepper);
}