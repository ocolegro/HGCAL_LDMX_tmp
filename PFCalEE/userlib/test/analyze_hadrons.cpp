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
	//freopen("test_log.txt", "w", stdout);
	TFile *infile = TFile::Open(argv[1]);
	TTree *tree = (TTree*) infile->Get("HGCSSTree");

	std::vector<HGCSSSamplingSection> * samplingVec = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &samplingVec);

	std::vector<HGCSSSimHit> * hitVec = 0;
	tree->SetBranchAddress("HGCSSSimHitVec", &hitVec);

	std::vector<HGCSSGenParticle> * hadronVec = 0;
	tree->SetBranchAddress("HGCSSTrackVec", &hadronVec);

	std::vector<HGCSSGenParticle> * targetVec = 0;
	tree->SetBranchAddress("HGCSSGenParticleVec", &targetVec);

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("hadrons", "Hadron Study");

	Int_t nHadrons,pdgid[500];

	t1.Branch("nHadrons", &nHadrons, "nHadrons/I");
	t1.Branch("pdgid", &pdgid, "pdgid[500]/I");

	unsigned nEvts = tree->GetEntries();

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		if (ievt > 10000)
			break;
		for (Int_t j = 0; j < targetVec->size(); j++) {
			HGCSSGenParticle& target = (*targetVec)[j];

			}

		for (Int_t j = 0; j < hadronVec->size(); j++) {
			HGCSSGenParticle& hadron = (*hadronVec)[j];

			}



		t1.Fill();
	}
	t1.Write();

	return 1;
}
