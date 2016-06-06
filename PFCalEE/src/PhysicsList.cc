//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
// $Id$
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PhysicsList.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "G4ProcessManager.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4EmStandardPhysics.hh"
#include "G4EmStandardPhysics_option2.hh"
//#include "G4VUserPhysicsList.hh"

#include "StepMax.hh"
#include "G4DecayPhysics.hh"

#include "G4BosonConstructor.hh"
#include "G4LeptonConstructor.hh"
#include "G4MesonConstructor.hh"
#include "G4BosonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"
#include "G4ShortLivedConstructor.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() :
G4VModularPhysicsList(),fEmPhysicsList(0) //G4VUserPhysicsList()
{
	defaultCutValue = 0.03 * mm;
	SetVerboseLevel(1);
	fEmPhysicsList = new G4EmStandardPhysics_option2();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList() {
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::SetCuts() {
	if (verboseLevel > 0) {
		G4cout << "PhysicsList::SetCuts:";
		G4cout << "CutLength : " << G4BestUnit(defaultCutValue, "Length")
				<< G4endl;
			}

			// set cut values for gamma at first and for e- second and next for e+,
			// because some processes for e+/e- need cut values for gamma
			//

			SetCutValue(.7*mm, "gamma");
			SetCutValue(.7*mm, "e-");
			SetCutValue(.7*mm, "e+");

			//set smaller cut for Si
			const std::vector<G4LogicalVolume*> & logSi = ((DetectorConstruction*)G4RunManager::GetRunManager()->GetUserDetectorConstruction())->getSiLogVol();

			for(size_t i=0; i<logSi.size(); i++) {
				G4Region* reg = logSi[i]->GetRegion();
				G4ProductionCuts* cuts = new G4ProductionCuts;
				cuts->SetProductionCut(defaultCutValue);
				reg->SetProductionCuts(cuts);
			}

			if (verboseLevel>0) DumpCutValuesTable();
		}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Construct processes:

void PhysicsList::ConstructProcess()
{
  if (verboseLevel > -1) {
    G4cout << "PhysicsList::ConstructProcess start" << G4endl;
  }
  // transportation
  //
  AddTransportation();

  // electromagnetic physics list
  //
  fEmPhysicsList->ConstructProcess();

  // decay process
  //
  //fDecay->ConstructProcess();
  AddDecay();
  // step limitation (as a full process)
  //
  AddStepMax();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Construct particles:

void PhysicsList::ConstructParticle()
{
    G4BosonConstructor  pBosonConstructor;
    pBosonConstructor.ConstructParticle();

    G4LeptonConstructor pLeptonConstructor;
    pLeptonConstructor.ConstructParticle();

    G4MesonConstructor pMesonConstructor;
    pMesonConstructor.ConstructParticle();

    G4BaryonConstructor pBaryonConstructor;
    pBaryonConstructor.ConstructParticle();

    G4IonConstructor pIonConstructor;
    pIonConstructor.ConstructParticle();

    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//Add stepping action:

void PhysicsList::AddStepMax()
{
  // Step limitation seen as a process
  StepMax* stepMaxProcess = new StepMax();

  theParticleIterator->reset();
  while ((*theParticleIterator)()){
      G4ParticleDefinition* particle = theParticleIterator->value();
      G4ProcessManager* pmanager = particle->GetProcessManager();

      if (stepMaxProcess->IsApplicable(*particle))
        {
          pmanager ->AddDiscreteProcess(stepMaxProcess);
        }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// particles

void PhysicsList::AddDecay()
{
  // Add Decay Process

  G4Decay* fDecayProcess = new G4Decay();

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();

    if (fDecayProcess->IsApplicable(*particle) && !particle->IsShortLived()) {

      pmanager ->AddProcess(fDecayProcess);

      // set ordering for PostStepDoIt and AtRestDoIt
      pmanager ->SetProcessOrdering(fDecayProcess, idxPostStep);
      pmanager ->SetProcessOrdering(fDecayProcess, idxAtRest);

    }
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

