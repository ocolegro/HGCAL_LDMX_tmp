#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "G4EmSaturation.hh"

class EventAction;

class SteppingAction: public G4UserSteppingAction {
public:
	SteppingAction();
	virtual ~SteppingAction();

	void UserSteppingAction(const G4Step*);
	inline unsigned getLayer(std::string astr) {

		G4cout << "The layer is " << astr <<
				"After stripping we have " << astr.erase( std::remove_if(astr.begin(), astr.end(), &ispunct), astr.end()) << G4endl;
		return 0;

	};
private:
	EventAction *eventAction_;
	//to correct the energy in the scintillator
	G4EmSaturation* saturationEngine;
	G4double timeLimit_;

};

#endif
