#include<string>
#include<set>
#include<iostream>
#include<fstream>
#include<sstream>
#include <algorithm>
#include <boost/algorithm/string.hpp>

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TNtuple.h"
#include "TH2D.h"
#include "TH2Poly.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "Math/Vector4D.h"

#include "HGCSSEvent.hh"
#include "HGCSSInfo.hh"
#include "HGCSSSimHit.hh"
#include "HGCSSRecoHit.hh"
#include "HGCSSGenParticle.hh"
#include "HGCSSRecoJet.hh"
#include "HGCSSCalibration.hh"
#include "HGCSSDigitisation.hh"
#include "HGCSSDetector.hh"
#include "HGCSSGeometryConversion.hh"
#include "HGCSSSamplingSection.hh"
#ifdef __MAKECINT__
#pragma link C++ class vector<float>+;
#endif

int main(int argc, char** argv) {
	std::cout << "Opening the file " << argv[1] << std::endl;
	TFile *infile = TFile::Open(argv[1]);
	TTree *tree = (TTree*) infile->Get("HGCSSTree");
	freopen("log.txt", "w", stdout);

	std::vector<HGCSSSamplingSection> * samplingVec = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &samplingVec);

	std::vector<HGCSSSimHit> * hitVec = 0;
	tree->SetBranchAddress("HGCSSSimHitVec", &hitVec);

	std::vector<HGCSSGenParticle> * hadronVec = 0;
	tree->SetBranchAddress("HGCSSHadronVec", &hadronVec);

	std::vector<HGCSSGenParticle> * targetVec = 0;
	tree->SetBranchAddress("HGCSSTargetVec", &targetVec);
	Int_t firstLayer = 0;

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("sampling", "Sampling Study");
	Int_t nHadrons,nTargetParticles;

	t1.Branch("nHadrons", &nHadrons, "nHadrons/I");
	t1.Branch("nTargetParticles", &nTargetParticles, "nTargetParticles/I");

	Float_t hadron_time[500],hadron_xpos[500],hadron_ypos[500],hadron_zpos[500],
	hadron_mass[500],hadron_px[500],hadron_py[500],hadron_pz[500],
	hadron_pdgid[500],hadron_layer[500],hadron_charge[500],hadron_trackid[500];

	t1.Branch("hadron_time", &hadron_time, "hadron_time[nHadrons]/F");
	t1.Branch("hadron_xpos", &hadron_xpos, "hadron_xpos[nHadrons]/F");
	t1.Branch("hadron_ypos", &hadron_ypos, "hadron_ypos[nHadrons]/F");
	t1.Branch("hadron_zpos", &hadron_zpos, "hadron_zpos[nHadrons]/F");
	t1.Branch("hadron_mass", &hadron_mass, "hadron_mass[nHadrons]/F");
	t1.Branch("hadron_px", &hadron_px, "hadron_px[nHadrons]/F");
	t1.Branch("hadron_py", &hadron_py, "hadron_py[nHadrons]/F");
	t1.Branch("hadron_pz", &hadron_pz, "hadron_pz[nHadrons]/F");
	t1.Branch("hadron_pdgid", &hadron_pdgid, "hadron_pdgid[nHadrons]/F");
	t1.Branch("hadron_charge", &hadron_charge, "hadron_charge[nHadrons]/F");
	t1.Branch("hadron_trackid", &hadron_trackid, "hadron_trackid[nHadrons]/F");
	t1.Branch("hadron_layer", &hadron_layer, "hadron_layer[nHadrons]/F");


	Float_t target_time[500],target_xpos[500],target_ypos[500],target_zpos[500],
	target_mass[500],target_px[500],target_py[500],target_pz[500],
	target_pdgid[500],target_charge[500],target_trackid[500];

	t1.Branch("target_time", &target_time, "target_time[nTargetParticles]/F");
	t1.Branch("target_xpos", &target_xpos, "target_xpos[nTargetParticles]/F");
	t1.Branch("target_ypos", &target_ypos, "target_ypos[nTargetParticles]/F");
	t1.Branch("target_zpos", &target_zpos, "target_zpos[nTargetParticles]/F");
	t1.Branch("target_mass", &target_mass, "target_mass[nTargetParticles]/F");
	t1.Branch("target_px", &target_px, "target_px[nTargetParticles]/F");
	t1.Branch("target_py", &target_py, "target_py[nTargetParticles]/F");
	t1.Branch("target_pz", &target_pz, "target_pz[nTargetParticles]/F");
	t1.Branch("target_pdgid", &target_pdgid, "target_pdgid[nTargetParticles]/F");
	t1.Branch("target_charge", &target_charge, "target_charge[nTargetParticles]/F");
	t1.Branch("target_trackid", &target_trackid, "target_trackid[nTargetParticles]/F");


	unsigned nEvts = tree->GetEntries();

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);



		for (Int_t j = 0; j < targetVec->size(); j++) {
			nTargetParticles = nTargetParticles + 1;
			HGCSSGenParticle& target = (*targetVec)[j];
			target_time[j]      = target.time();
			target_xpos[j] 	    = target.x();
			target_ypos[j] 		= target.y();
			target_zpos[j]   	= target.z();
			target_mass[j]   	= target.mass();
			target_px[j]   		= target.px();
			target_py[j]   		= target.py();
			target_pz[j]   		= target.pz();
			target_pdgid[j]   	= target.pdgid();
			target_charge[j]   	= target.charge();
			target_trackid[j]   = target.trackID();
		}

		for (Int_t j = 0; j < hadronVec->size(); j++) {
			HGCSSGenParticle& hadron = (*hadronVec)[j];
			hadron_time[j]      = hadron.time();
			hadron_xpos[j] 	    = hadron.x();
			hadron_ypos[j] 		= hadron.y();
			hadron_zpos[j]   	= hadron.z();
			hadron_mass[j]   	= hadron.mass();
			hadron_px[j]   		= hadron.px();
			hadron_py[j]   		= hadron.py();
			hadron_pz[j]   		= hadron.pz();
			hadron_pdgid[j]   	= hadron.pdgid();
			hadron_charge[j]   	= hadron.charge();
			hadron_trackid[j]   = hadron.trackID();
			hadron_trackid[j]   = hadron.layer();

		}



		t1.Fill();
	}
	t1.Write();

	return 1;
}
