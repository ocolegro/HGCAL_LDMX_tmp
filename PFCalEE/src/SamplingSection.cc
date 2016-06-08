#include "G4VPhysicalVolume.hh"

#include "SamplingSection.hh"

//
void SamplingSection::add(G4double parentKE, G4double depositRawE, G4double depositNonIonE,
		G4double dl, G4double globalTime, G4int pdgId, G4VPhysicalVolume* vol,
		const G4ThreeVector & position, G4int trackID, G4int parentID,
		G4int layerId,G4bool isHadronTrack,G4bool isForward) {
	std::string lstr = vol->GetName();

	for (unsigned ie(0); ie < n_elements * n_sectors; ++ie) {
		if (sublayer_vol[ie] && lstr == sublayer_vol[ie]->GetName()) {
			unsigned idx = getSensitiveLayerIndex(lstr);
			unsigned eleidx = ie % n_elements;
			sublayer_RawDep[eleidx] += depositRawE;
			sublayer_NonIonDep[eleidx] += depositNonIonE;

			sublayer_dl[eleidx] += dl;

			//add hit
			G4SiHit lHit;
			lHit.energyDep = depositRawE;
			lHit.time = globalTime;
			lHit.pdgId = pdgId;
			lHit.layer = layerId;
			lHit.hit_x = position.x();
			lHit.hit_y = position.y();
			lHit.hit_z = position.z();
			lHit.trackId = trackID;
			lHit.parentId = parentID;
			lHit.parentKE = parentKE;


			if (isSensitiveElement(eleidx)) { //if Si || sci
				sens_time[idx] += depositRawE * globalTime;

				//discriminate further by particle type
				if (abs(pdgId) == 22){
					sens_gamDep[idx] += depositRawE;
					if (isForward){
						sens_gamKinFlux[idx] += parentKE;
						sens_gamCounter[idx] += 1;
					}
				}
				else if (abs(pdgId) == 11){
					sens_eleDep[idx] += depositRawE;
					if (isForward){
						//G4cout << "adding a parent id "  << G4endl;
						parent_daughter_Ids[idx].push_back(std::make_pair(parentID,trackID));

						//G4cout << "adding a track ke "  << G4endl;
						trackKe[idx].push_back(parentKE);

						sens_eleKinFlux[idx] += parentKE;
						sens_eleCounter[idx] += 1;
						if (sens_eleKinFlux[idx] > 4000){
							for (int i = 0; i < parent_daughter_Ids.size(); i++){
								G4cout
										<< "The layer flux was " << sens_eleKinFlux[idx]
										<< "The parent trackID is " << parent_daughter_Ids[idx].at(i).first
										<< "and the daughter trackID is " << parent_daughter_Ids[idx].at(i).second
										<< G4endl;
								G4cout
										<< "and the particle KE is " << trackKe[idx].at(i)
										<< G4endl;
							}
						}

					}
				}
				else if (abs(pdgId) == 13) {
					sens_muDep[idx] += depositRawE;
					if (isForward){
						sens_muKinFlux[idx] += parentKE;
						sens_muCounter[idx] += 1;
					}
				} else if (abs(pdgId) == 2112) {
					if (pdgId == 2112 && isHadronTrack)
						sens_neutronDep[idx] += depositRawE;
					if (isForward){
						sens_neutronKinFlux[idx] += parentKE;
						sens_neutronCounter[idx] += 1;
					}
				} else {
					if ((abs(pdgId) != 111) && (abs(pdgId) != 310)
							&& (pdgId != -2212) && (isHadronTrack) )

						sens_hadDep[idx] += depositRawE;
					if (isForward){
						sens_hadKinFlux[idx] += parentKE;
						sens_hadCounter[idx] += 1;
					}
				}
				sens_HitVec[idx].push_back(lHit);
			} //if Si
			else {
				//check for W in layer
				if ((lstr.find("W") == std::string::npos) == 0)
					abs_HitSumVec.push_back(lHit);
			} //if in right material
		} //loop on available materials
	}
}

//
void SamplingSection::report(bool header) {
	if (header)
		G4cout
				<< "E/[MeV]\t  Si\tAbsorber\tTotal\tSi g frac\tSi e frac\tSi mu frac\tSi had frac\tSi <t> \t nG4SiHits"
				<< G4endl;
		G4cout << std::setprecision(3) << "\t  " << getMeasuredEnergy(false) << "\t" << getAbsorbedEnergy() << "\t\t" << getTotalEnergy() << "\t"
		<< getPhotonFraction() << "\t" << getElectronFraction() << "\t" << getMuonFraction() << "\t" << getHadronicFraction() << "\t"
		<< getAverageTime() << "\t"
		<< getTotalSensHits() << "\t"
		<< G4endl;
	}

G4int SamplingSection::getTotalSensHits() {
	G4int tot = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		tot += sens_HitVec[ie].size();
	}
	return tot;
}

G4double SamplingSection::getTotalSensE() {
	double etot = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isSensitiveElement(ie))
			etot += sublayer_RawDep[ie];
	}
	return etot;
}

G4double SamplingSection::getTotalSensNonIonE() {
	double etot = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isSensitiveElement(ie))
			etot += sublayer_NonIonDep[ie];
	}
	return etot;
}

G4double SamplingSection::getAverageTime() {
	double etot = getTotalSensE();
	double time = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		time += sens_time[ie];
	}
	return etot > 0 ? time / etot : 0;
}

//
G4double SamplingSection::getPhotonFraction() {
	double etot = getTotalSensE();
	double val = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		val += sens_gamDep[ie];
	}
	return etot > 0 ? val / etot : 0;
}

//
G4double SamplingSection::getElectronFraction() {
	double etot = getTotalSensE();
	double val = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		val += sens_eleDep[ie];
	}
	return etot > 0 ? val / etot : 0;
}

//
G4double SamplingSection::getMuonFraction() {
	double etot = getTotalSensE();
	double val = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		val += sens_muDep[ie];
	}
	return etot > 0 ? val / etot : 0;
}

//
G4double SamplingSection::getNeutronFraction() {
	double etot = getTotalSensE();
	double val = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		val += sens_neutronDep[ie];
	}
	return etot > 0 ? val / etot : 0;
}

//
G4double SamplingSection::getHadronicFraction() {
	double etot = getTotalSensE();
	double val = 0;
	for (unsigned ie(0); ie < n_sens_elements; ++ie) {
		val += sens_hadDep[ie];
	}
	return etot > 0 ? val / etot : 0;
}

//
G4double SamplingSection::getMeasuredEnergy(bool weighted) {
	G4double weight = (weighted ? getAbsorberX0() : 1.0);
	return weight * getTotalSensE();

}

//
G4double SamplingSection::getAbsorberX0() {
	// G4cout << Pb_thick << " " << Pb_X0 << G4endl;
	double val = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isAbsorberElement(ie))
			if (sublayer_X0[ie] > 0)
				val += sublayer_thick[ie] / sublayer_X0[ie];
	}
	return val;
}
//
G4double SamplingSection::getAbsorberdEdx() {
	// G4cout << Pb_thick << " " << Pb_X0 << G4endl;
	double val = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isAbsorberElement(ie))
			val += sublayer_thick[ie] * sublayer_dEdx[ie];
	}
	return val;
}

//
G4double SamplingSection::getAbsorberLambda() {
	double val = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isAbsorberElement(ie) && sublayer_L0[ie] > 0)
			val += sublayer_thick[ie] / sublayer_L0[ie];
	}
	return val;
}

//
G4double SamplingSection::getAbsorbedEnergy() {
	double val = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (isAbsorberElement(ie))
			val += sublayer_RawDep[ie];
	}
	return val;
}

//
G4double SamplingSection::getTotalEnergy(bool raw) {
	double val = 0;
	for (unsigned ie(0); ie < n_elements; ++ie) {
		if (raw)
			val += sublayer_RawDep[ie];
		else
			val += sublayer_NonIonDep[ie];
	}
	return val;
}

const G4SiHitVec & SamplingSection::getSiHitVec(const unsigned & idx) const {
	return sens_HitVec[idx];
}

const G4SiHitVec & SamplingSection::getAbsHits() const {
	return abs_HitSumVec;
}
void SamplingSection::trackParticleHistory(const unsigned & idx,
		const G4SiHitVec & incoming) {
	for (unsigned iP(0); iP < sens_HitVec[idx].size(); ++iP) { //loop on g4hits
		G4int parId = sens_HitVec[idx][iP].parentId;
		for (unsigned iI(0); iI < incoming.size(); ++iI) { //loop on previous layer
			G4int trId = incoming[iI].trackId;
			if (trId == parId)
				sens_HitVec[idx][iP].parentId = incoming[iI].parentId;
		} //loop on previous layer
	} //loop on g4hits
	  //  return sens_HitVec;
}
