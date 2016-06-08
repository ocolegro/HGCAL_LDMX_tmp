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

#include "LHEPrimaryGeneratorMessenger.hh"

#include "LHEPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LHEPrimaryGeneratorMessenger::LHEPrimaryGeneratorMessenger(LHEPrimaryGeneratorAction* Gun)
 : G4UImessenger(),
   fAction(Gun),
   fDir(0),
   fInputFile(0)

{
  G4cout << "WE SHOULD BE GETTING HERER!!!!!!!!!!!!!!!!!!!!!!!!" << G4endl;
  fDir = new G4UIdirectory("/filemode/");
  fDir->SetGuidance("Primary generator control for neutron events");

  fInputFile = new G4UIcmdWithAString("/filemode/inputFilename", this);
  fInputFile->SetGuidance("Select input data file.");
  fInputFile->SetParameterName("choice", false);
  fInputFile->AvailableForStates(G4State_PreInit);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

LHEPrimaryGeneratorMessenger::~LHEPrimaryGeneratorMessenger()
{
  delete fInputFile;
  delete fDir;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void LHEPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
  if (command == fInputFile) { fAction->SetInputFile(newValue); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
