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

	std::vector<HGCSSSamplingSection> * samplingVec = 0;
	tree->SetBranchAddress("HGCSSSamplingSectionVec", &samplingVec);
	Int_t firstLayer = 1;
	unsigned nEvts = tree->GetEntries();

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("hadrons", "Hadron Study");

	Float_t fullDep, fullSen, hadronSen, neutronSen, muonSen;
	Int_t layer[500],caloLen;
	t1.Branch("fullDep", &fullDep, "fullDep/F");
	t1.Branch("fullSen", &fullSen, "fullSen/F");
	t1.Branch("hadronSen", &hadronSen, "hadronSen/F");
	t1.Branch("neutronSen", &neutronSen, "neutronSen/F");
	t1.Branch("muonSen", &muonSen, "muonSen/F");
	t1.Branch("caloLen", &caloLen, "caloLen/I");
	t1.Branch("layer", &layer, "layer[caloLen]/I");

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		fullSen = 0;
		fullDep = 0;
		hadronSen = 0;
		neutronSen = 0;
		muonSen = 0;
		caloLen = 0;

		if (ievt > 2500) break;

		for (Int_t j = firstLayer; j < samplingVec->size(); j++) {
			HGCSSSamplingSection& sec = (*samplingVec)[j];
			fullSen    += sec.measuredE();
			fullDep    += sec.totalE();
			hadronSen  += sec.hadKin();
			neutronSen += sec.neutronKin();
			muonSen    += sec.muKin();
			layer[j-firstLayer]   = j-firstLayer;
			caloLen    = caloLen + 1;
		}

		t1.Fill();
	}
	t1.Write();

	return 1;
}
