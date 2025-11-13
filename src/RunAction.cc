#include "RunAction.hh"
#include "SteppingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "MyGlobals.hh"
#include "G4Run.hh"
#include "G4Threading.hh"
#include "G4SystemOfUnits.hh"
#include "spectrum_cdf.hh"
#include <fstream>
#include <sstream>

std::vector<std::string> RunAction::bufferList;
std::vector<G4int> RunAction::globalEnergyHistogram;
std::mutex RunAction::bufferMutex;
std::mutex RunAction::histogramMutex;

RunAction::RunAction()
    : steppingAction(nullptr), primaryGeneratorAction(nullptr),
      fNumBins(1000), fMinEnergy(energyBins.front()), fMaxEnergy(energyBins.back()) {}

RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*) {
    if (G4Threading::IsMasterThread())
        globalEnergyHistogram.resize(fNumBins, 0);
}

void RunAction::EndOfRunAction(const G4Run*) {
    if (!G4Threading::IsMasterThread()) {
        if (steppingAction) {
            std::lock_guard<std::mutex> lock(bufferMutex);
            bufferList.push_back(steppingAction->GetBufferedOutput());
        }

        if (primaryGeneratorAction) {
            std::lock_guard<std::mutex> lock(histogramMutex);
            auto& threadHist = primaryGeneratorAction->GetEnergyHistogram();
            if(globalEnergyHistogram.empty()) globalEnergyHistogram.resize(fNumBins, 0);
            for (size_t i = 0; i < threadHist.size(); ++i)
                globalEnergyHistogram[i] += threadHist[i];
        }
    }

    if (G4Threading::IsMasterThread()) {
        std::ofstream fout("../data/proton_energy_histogram_" + std::to_string(num_run) + ".csv");
        fout << "EnergyBinCenter(MeV),Count\n";
        for (int i = 0; i < fNumBins; ++i) {
            G4double bin_center = fMinEnergy + (i + 0.5)*(fMaxEnergy - fMinEnergy)/fNumBins;
            fout << bin_center/MeV << "," << globalEnergyHistogram[i] << "\n";
        }
        fout.close();

        std::ofstream isotopeOut("../data/isotope_production_" + std::to_string(num_run) + ".csv");
        isotopeOut << "x(mm),y(mm),z(mm),name,Z,A,Ex_keV,energy(MeV),x_hat,y_hat,z_hat,process,volume,threadID\n";
        for (const auto& buf : bufferList) {
            std::istringstream ss(buf);
            std::string line;
            while (std::getline(ss, line)) {
                if (!line.empty()) {
                    isotopeOut << line << "," << G4Threading::G4GetThreadId() << "\n";
                }
            }
        }
        isotopeOut.close();
    }
}

void RunAction::SetSteppingAction(SteppingAction* stepper) {
    steppingAction = stepper;
}

void RunAction::SetPrimaryGeneratorAction(PrimaryGeneratorAction* pga) {
    primaryGeneratorAction = pga;
}