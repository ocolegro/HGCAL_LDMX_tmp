#ifndef _samplingsection_hh_
#define _samplingsection_hh_

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Colour.hh"

#include <iomanip>
#include <vector>

#include "G4SiHit.hh"

class SamplingSection {
public:
	//CTOR
	SamplingSection(std::vector<std::pair <G4double,std::string>> iEle) {

			std::vector<G4double> aThicknessVec;std::vector<std::string> aMaterialVec;
				for (unsigned i = 0; i < iEle.size(); i++)
				{
					aThicknessVec.push_back(iEle.at(i).first);
					aMaterialVec.push_back(iEle.at(i).second);

				}
				Total_thick = 0;
				n_sens_elements=0;
				n_elements=0;
				n_sectors=0;
				sublayer_thick.clear();
				ele_name.clear();
				sublayer_X0.clear();
				sublayer_L0.clear();
				sublayer_vol.clear();
				hasScintillator = false;
				for (unsigned ie(0); ie<aThicknessVec.size(); ++ie) {
					//consider only material with some non-0 width...
					if (aThicknessVec[ie]>0) {
						sublayer_thick.push_back(aThicknessVec[ie]);
						ele_name.push_back(aMaterialVec[ie]);
						if (aMaterialVec[ie]== "Scintillator") hasScintillator = true;
						sublayer_X0.push_back(0);
						sublayer_dEdx.push_back(0);
						sublayer_L0.push_back(0);
						sublayer_vol.push_back(0);
						Total_thick+=aThicknessVec[ie];
						++n_elements;
						//the following method check the total size...
						//so incrementing first.
						if (isSensitiveElement(n_elements-1)) {
							G4SiHitVec lVec;
							sens_HitVec.push_back(lVec);
							++n_sens_elements;
						}
					}
				}
				sens_HitVec_size_max = 0;
				abs_HitVec_size_max = 0;
				resetCounters();

				std::cout << " -- End of sampling section initialisation. Input " << aThicknessVec.size() << " elements, constructing " << n_elements << " elements with " << n_sens_elements << " sensitive elements." << std::endl;

			};

			//DTOR
			~SamplingSection() {};

			inline void setNumberOfSectors(const unsigned nSec) {
				n_sectors = nSec;
				sublayer_vol.clear();
				for (unsigned ie(0); ie<n_elements*n_sectors; ++ie) {
					sublayer_vol.push_back(0);
				}
			};

			//
			void add(G4double parentKE,G4double depositRawE, G4double depositNonIonE,
					G4double dl,
					G4double globalTime,G4int pdgId,
					G4VPhysicalVolume* vol,
					const G4ThreeVector & position,
					G4int trackID, G4int parentID,
					G4int layerId,G4bool isHadronTrack,G4bool isForward,G4bool isPrimaryTrack);

			inline bool isSensitiveElement(const unsigned & aEle) {
				if (aEle < n_elements &&
						(ele_name[aEle] == "Si" || ele_name[aEle] == "Scintillator")
				) return true;
				return false;
			};

			inline unsigned getSensitiveLayerIndex(std::string astr) {
				if (astr.find("_")== astr.npos) return 0;
				size_t pos = astr.find("phys");
				if (pos != astr.npos && pos>1) {
					unsigned idx = 0;
					std::istringstream(astr.substr(pos-1,1))>>idx;
					return idx;
				}
				return 0;
			};

			inline G4Colour g4Colour(const unsigned & aEle) {
				if (isSensitiveElement(aEle)) return G4Colour::Red();
				if (ele_name[aEle] == "Cu") return G4Colour::Black();
				if (isAbsorberElement(aEle)) return G4Colour::Gray();
				if (ele_name[aEle] == "PCB") return G4Colour::Blue();
				if (ele_name[aEle] == "Air") return G4Colour::Cyan();
				return G4Colour::Yellow();
			};

			inline bool isAbsorberElement(const unsigned & aEle) {
				if (aEle < n_elements &&
						(
								ele_name[aEle] == "Pb" || ele_name[aEle] == "Cu" ||
								ele_name[aEle] == "W" || ele_name[aEle] == "Brass" ||
								ele_name[aEle] == "Fe" || ele_name[aEle] == "Steel" ||
								ele_name[aEle] == "SSteel" || ele_name[aEle] == "Al" ||
								ele_name[aEle] == "WCu" || ele_name[aEle] == "NeutMod"
						)
				) return true;
				return false;
			};

			//reset
			inline void resetCounters()
			{
				sublayer_RawDep.clear();
				sublayer_PrimaryDep.clear();
				sublayer_dl.clear();
				sens_time.clear();
				sens_gamDep.clear();
				sens_eleDep.clear();
				sens_muDep.clear();
				sens_neutronDep.clear();
				sens_hadDep.clear();
				sens_muKinFlux.clear();
				sens_neutronKinFlux.clear();
				sens_hadKinFlux.clear();
				sens_gamKinFlux.clear();
				sens_eleKinFlux.clear();

				Etracks.clear();
				Gtracks.clear();
				Htracks.clear();
				Mtracks.clear();
				Ntracks.clear();
				Alltracks.clear();

				sens_muCounter.clear();
				sens_neutronCounter.clear();
				sens_hadCounter.clear();
				sens_eleCounter.clear();
				sens_gamCounter.clear();

				sublayer_RawDep.resize(n_elements,0);
				sublayer_PrimaryDep.resize(n_elements,0);
				sublayer_dl.resize(n_elements,0);
				Alltracks.resize(n_elements);
				sens_time.resize(n_sens_elements,0);
				sens_gamDep.resize(n_sens_elements,0);
				sens_eleDep.resize(n_sens_elements,0);
				sens_muDep.resize(n_sens_elements,0);
				sens_neutronDep.resize(n_sens_elements,0);
				sens_hadDep.resize(n_sens_elements,0);
				sens_muKinFlux.resize(n_sens_elements,0);
				sens_neutronKinFlux.resize(n_sens_elements,0);
				sens_hadKinFlux.resize(n_sens_elements,0);
				sens_eleKinFlux.resize(n_sens_elements,0);
				sens_gamKinFlux.resize(n_sens_elements,0);

				Etracks.resize(n_sens_elements);
				Gtracks.resize(n_sens_elements);
				Htracks.resize(n_sens_elements);
				Mtracks.resize(n_sens_elements);
				Ntracks.resize(n_sens_elements);

				sens_muCounter.resize(n_sens_elements,0);
				sens_neutronCounter.resize(n_sens_elements,0);
				sens_hadCounter.resize(n_sens_elements,0);
				sens_eleCounter.resize(n_sens_elements,0);
				sens_gamCounter.resize(n_sens_elements,0);
				//reserve some space based on first event....
				for (unsigned idx(0); idx<n_sens_elements; ++idx) {
					if (sens_HitVec[idx].size() > sens_HitVec_size_max) {
						sens_HitVec_size_max = 2*sens_HitVec[idx].size();
						//G4cout << "-- SamplingSection::resetCounters(), space reserved for HitVec vector increased to " << sens_HitVec_size_max << G4endl;
					}
					if (abs_HitSumVec.size() > abs_HitVec_size_max) {
						abs_HitVec_size_max = 2*abs_HitSumVec.size();
						//G4cout << "-- SamplingSection::resetCounters(), space reserved for absHitVec vector increased to " << abs_HitVec_size_max << G4endl;

					}
					sens_HitVec[idx].clear();
					sens_HitVec[idx].reserve(sens_HitVec_size_max);
					abs_HitSumVec.clear();
					abs_HitSumVec.reserve(abs_HitVec_size_max);
				}
			}

			inline G4double getKinMuon() {
				double val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_muKinFlux[ie];
				}
				return val;
			}
			;
			inline G4double getKinHadron() {
				double val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_hadKinFlux[ie];
				}
				return val;
			}
			;
			inline G4double getKinNeutron() {
				double val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_neutronKinFlux[ie];
				}
				return val;
			}
			;
			inline G4double getKinEle() {
				double val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_eleKinFlux[ie];
				}
				return val;
			}
			;
			inline G4double getKinGam() {
				double val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_gamKinFlux[ie];
				}
				return val;
			}
			;
			inline unsigned getMuonCount() {
				unsigned int  val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_muCounter[ie];
				}
				return val;
			}
			;
			inline unsigned getHadronCount() {
				unsigned int val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_hadCounter[ie];
				}
				return val;
			}
			;
			inline unsigned getNeutronCount() {
				unsigned int val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_neutronCounter[ie];
				}
				return val;
			}
			;
			inline unsigned getEleCount() {
				unsigned int val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_eleCounter[ie];
				}
				return val;
			}
			;
			inline unsigned getGamCount() {
				unsigned int val = 0;
				for (unsigned ie(0); ie < n_sens_elements; ++ie) {
					val += sens_gamCounter[ie];
				}
				return val;
			}
			;
			//
			G4double getMeasuredEnergy(bool weighted=true);

			G4double getAbsorbedEnergy();
			G4double getTotalEnergy(bool raw = true);
			G4double getAbsorberX0();
			G4double getAbsorberdEdx();
			G4double getAbsorberLambda();
			G4double getHadronicFraction();
			G4double getNeutronFraction();
			G4double getMuonFraction();
			G4double getPhotonFraction();
			G4double getElectronFraction();
			G4double getAverageTime();
			G4int getTotalSensHits();
			G4double getTotalSensE();
			G4double getTotalSensNonIonE();
			const G4SiHitVec & getSiHitVec(const unsigned & idx) const;
			const G4SiHitVec & getAbsHits() const;

			void trackParticleHistory(const unsigned & idx,const G4SiHitVec & incoming);

			//
			void report(bool header=false);

			//members
			unsigned n_elements;
			unsigned n_sectors;
			unsigned n_sens_elements;
			std::vector<G4double> sublayer_thick;
			std::vector<std::string> ele_name;
			std::vector<G4double> sublayer_X0;
			std::vector<G4double> sublayer_dEdx;
			std::vector<G4double> sublayer_L0;
			std::vector<G4double> sublayer_RawDep;
			std::vector<G4double> sublayer_PrimaryDep;
			std::vector<G4double> sublayer_dl;

			std::vector<G4VPhysicalVolume*> sublayer_vol;
			std::vector<G4double> sens_gamDep, sens_eleDep, sens_muDep, sens_gamKinFlux,sens_eleKinFlux,
			sens_muKinFlux,sens_neutronDep, sens_neutronKinFlux,
			sens_hadDep, sens_hadKinFlux, sens_time;
			std::vector<std::vector<unsigned int>> Alltracks,Etracks,Gtracks,Mtracks,Ntracks,Htracks;
			std::vector<unsigned int> sens_neutronCounter,sens_hadCounter,sens_muCounter,sens_gamCounter,sens_eleCounter;
			G4double Total_thick;

			std::vector<G4SiHitVec> sens_HitVec;
			G4SiHitVec abs_HitSumVec;

			unsigned sens_HitVec_size_max;
			unsigned abs_HitVec_size_max;

			bool hasScintillator;

		};

#endif
