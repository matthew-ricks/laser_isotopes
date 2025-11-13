#include "G4MTRunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "PhysicsList.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "MyGlobals.hh"

#include <cstdlib>
#include <iostream>

int main(int argc, char** argv) {

    // Check for command-line argument
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <run_number>" << std::endl;
        return 1;
    }

    num_run = std::atoi(argv[1]);  // Set global num_run
    G4Random::setTheSeed(time(NULL));

    // Create multithreaded run manager
    G4MTRunManager* runManager = new G4MTRunManager();

    // Use all available cores (or override with G4FORCENUMBEROFTHREADS)
    runManager->SetNumberOfThreads(G4Threading::G4GetNumberOfCores());
    // runManager->SetNumberOfThreads(1);

    runManager->SetVerboseLevel(0);
    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new PhysicsList());

    runManager->SetUserInitialization(new ActionInitialization());


    G4UImanager* uiManager = G4UImanager::GetUIpointer();
    uiManager->ApplyCommand(G4String("/process/verbose       0"));
    uiManager->ApplyCommand(G4String("/process/em/verbose    0"));
    uiManager->ApplyCommand(G4String("/process/had/verbose   0"));
    uiManager->ApplyCommand(G4String("/process/eLoss/verbose 0"));
    uiManager->ApplyCommand("/run/printProgress 1000000");
    uiManager->ApplyCommand("/run/initialize");

    // Start the beam-on simulation
    G4String runString = "/run/beamOn " + std::to_string(numParticles);
    uiManager->ApplyCommand(runString); // Number of histories

    delete runManager;

    return 0;
}
