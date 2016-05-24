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

#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "HepMCG4AsciiReader.hh"
#include "HepMCG4PythiaInterface.hh"

#include <fstream>
#include <string>
#include <sstream>
#define PI 3.1415926535

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<std::string> split(const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> tokens;
	while (std::getline(ss, item, delim)) {
		tokens.push_back(item);
	}
	return tokens;
}

PrimaryGeneratorAction::PrimaryGeneratorAction(G4int mod, bool signal) {
	model_ = mod;
	signal_ = signal;
	G4int n_particle = 1;

	// default generator is particle gun.
	currentGenerator = particleGun = new G4ParticleGun(n_particle);
	currentGeneratorName = "particleGun";
	hepmcAscii = new HepMCG4AsciiReader();
#ifdef G4LIB_USE_PYTHIA
	pythiaGen= new HepMCG4PythiaInterface();
#else
	pythiaGen = 0;
#endif
	gentypeMap["particleGun"] = particleGun;
	gentypeMap["hepmcAscii"] = hepmcAscii;
	gentypeMap["pythia"] = pythiaGen;

	Detector =
			(DetectorConstruction*) G4RunManager::GetRunManager()->GetUserDetectorConstruction();

	//create a messenger for this class
	gunMessenger = new PrimaryGeneratorMessenger(this);

	// default particle kinematic

	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;
	G4ParticleDefinition* particle = particleTable->FindParticle(particleName =
			"e-");
	particleGun->SetParticleDefinition(particle);
	particleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
	particleGun->SetParticleEnergy(4. * GeV);
	G4double position = -0.5 * (Detector->GetWorldSizeZ());
	particleGun->SetParticlePosition(G4ThreeVector(0. * cm, 0. * cm, position));

	G4cout << " -- Gun position set to: 0,0," << position << G4endl;

	rndmFlag = "off";

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
	delete particleGun;
	delete hepmcAscii;
	delete pythiaGen;
	delete gunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
	G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
	G4String particleName;

	G4ParticleDefinition* particle = particleTable->FindParticle(particleName =
			"e-");
	particleGun->SetParticleDefinition(particle);
	if (signal_ == false){
		G4double et = 4.0;
		particleGun->SetParticleEnergy(et * GeV);
		particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.));
	}
	else{
		G4cout << "Insert signal code here" << G4endl;
		G4cout << "The line is " << anEvent->GetEventID() << G4endl;
		G4double E,dir_x,dir_y,dir_z;

		std::ifstream in("../data/mom.txt");
		std::string line;
		for (int i =0;i < anEvent->GetEventID()+1; i++){
			std::getline(in, line);
			if (i ==anEvent->GetEventID()){
		        std::vector<std::string> split_ = (split(line, ' '));
		        //Fetch the components from this line
		        std::istringstream os(split_.at(0));
		        os >> E;
		        os.clear();

		        os << split_.at(1);
		        os >> dir_x;
		        os.clear();

		        os << split_.at(2);
		        os >> dir_y;
		        os.clear();

		        os << split_.at(3);
		        os >> dir_z;
		        os.clear();
			}
		}
		G4cout << "The tuple (E,dir_x,dir_y,dir_z) = (" << E << "," << dir_x << ", " << dir_y << " , " << dir_z << ")" <<  G4endl;
		particleGun->SetParticleEnergy(E * GeV);
		particleGun->SetParticleMomentumDirection(G4ThreeVector(dir_x, dir_y, dir_z));
	}


	G4double y0 = G4RandFlat::shoot(-65.,65);
	G4double x0 = G4RandFlat::shoot(-65.,65);
	G4double z0 = -0.5 * (Detector->GetWorldSizeZ());

	if (model_ == 0)
		particleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));

	G4cout << " -- Gun position set to: " << x0 << "," << y0 << "," << z0
			<< G4endl;

	if (currentGenerator) {
		currentGenerator->GeneratePrimaryVertex(anEvent);

	} else
		G4Exception("PrimaryGeneratorAction::GeneratePrimaries",
				"PrimaryGeneratorAction001", FatalException,
				"generator is not instanciated.");

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

