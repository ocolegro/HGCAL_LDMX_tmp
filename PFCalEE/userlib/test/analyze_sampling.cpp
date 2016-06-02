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


	std::vector<HGCSSGenParticle> * trackVec = 0;
	tree->SetBranchAddress("HGCSSTrackVec", &trackVec);

	std::vector<HGCSSGenParticle> * genVec = 0;
	tree->SetBranchAddress("HGCSSGenParticleVec", &genVec);

	Int_t firstLayer = 1;
	unsigned nEvts = tree->GetEntries();

	TFile hfile("analyzed_tuple.root", "RECREATE");
	TTree t1("hadrons", "Hadron Study");

	Float_t summedDep, summedSen, summedHFlux, summedNFlux, summedMFlux,maxTrackKe,genKin;
	Float_t layerHFlux[500], layerNFlux[500], layerMFlux[500],
			layerHWgtCnt[500], layerEWgtCnt[500], layerDep[500], layerSen[500],layerHCount[500],layerNCount[500],layerMCount[500],
			summedNCount,summedHCount,summedMcount,layerHWgtAvg,layerEWgtAvg;

	Int_t layer[500], caloLen, layMax,genCounter;
	t1.Branch("caloLen", &caloLen, "caloLen/I");

	t1.Branch("summedDep", &summedDep, "summedDep/F");
	t1.Branch("summedSen", &summedSen, "summedSen/F");

	t1.Branch("summedHFlux", &summedHFlux, "summedHFlux/F");
	t1.Branch("summedNFlux", &summedNFlux, "summedNFlux/F");
	t1.Branch("summedMFlux", &summedMFlux, "summedMFlux/F");


	t1.Branch("summedHCount", &summedHCount, "summedHCount/F");
	t1.Branch("summedNCount", &summedNCount, "summedNCount/F");
	t1.Branch("summedMcount", &summedMcount, "summedMcount/F");

	t1.Branch("layerHWgtAvg", &layerHWgtAvg, "layerHWgtAvg/F");
	t1.Branch("layerEWgtAvg", &layerEWgtAvg, "layerEWgtAvg/F");

	t1.Branch("layerHFlux", &layerHFlux, "layerHFlux[caloLen]/F");
	t1.Branch("layerNFlux", &layerNFlux, "layerNFlux[caloLen]/F");
	t1.Branch("layerMFlux", &layerMFlux, "layerMFlux[caloLen]/F");


	t1.Branch("layerHCount", &layerHCount, "layerHCount[caloLen]/F");
	t1.Branch("layerNCount", &layerNCount, "layerNCount[caloLen]/F");
	t1.Branch("layerMCount", &layerMCount, "layerMCount[caloLen]/F");

	t1.Branch("layerHWgtCnt", &layerHWgtCnt, "layerHWgtCnt[caloLen]/F");
	t1.Branch("layerEWgtCnt", &layerEWgtCnt, "layerEWgtCnt[caloLen]/F");


	t1.Branch("maxTrackKe", &maxTrackKe, "maxTrackKe/F");
	t1.Branch("genKin", &genKin, "genKin/F");

	t1.Branch("genCounter", &genCounter, "genCounter/I");
	t1.Branch("layMax", &layMax, "layMax/I");

	t1.Branch("layerSen", &layerSen, "layerSen[caloLen]/F");
	t1.Branch("layerDep", &layerDep, "layerDep[caloLen]/F");


	t1.Branch("layer", &layer, "layer[caloLen]/I");

	for (unsigned ievt(0); ievt < nEvts; ++ievt) { //loop on entries
		tree->GetEntry(ievt);

		summedSen = 0, summedDep = 0, summedHFlux = 0, summedNFlux = 0, summedMFlux =
				0, caloLen = 0,summedHCount=0,summedNCount=0,summedMcount=0,maxTrackKe=0,genCounter = 0,genKin=0,layerHWgtAvg=0,layerEWgtAvg=0;

		if (ievt > 10000)
			break;

		for (Int_t j = firstLayer; j < samplingVec->size(); j++) {
			HGCSSSamplingSection& sec = (*samplingVec)[j];
			summedSen += sec.measuredE();
			summedDep += sec.totalE();

			layerSen[j - firstLayer] = sec.hadKin()/3.0;
			layerDep[j - firstLayer] = sec.neutronKin()/3.0;

			summedHFlux += sec.hadKin()/(3.0 * 26);
			summedNFlux += sec.neutronKin()/(3.0 * 26);
			summedMFlux += sec.muKin()/(3.0 * 26);

			summedHCount += sec.hadCount()/(3.0 * 26);
			summedNCount += sec.neutronCount()/(3.0 * 26);
			summedMcount += sec.muCount()/(3.0 * 26);

			layerHFlux[j - firstLayer] = sec.hadKin()/3.0;
			layerNFlux[j - firstLayer] = sec.neutronKin()/3.0;
			layerMFlux[j - firstLayer] = sec.muKin()/3.0;

			layerHCount[j - firstLayer] = sec.hadCount()/3.0;
			layerNCount[j - firstLayer] = sec.neutronCount()/3.0;
			layerMCount[j - firstLayer] = sec.muCount()/3.0;


			layerHWgtCnt[j - firstLayer] = sec.hadWgtCnt();
			layerEWgtCnt[j - firstLayer] = sec.eleWgtCnt();

			layerHWgtAvg += sec.hadWgtCnt()/26.0;
			layerEWgtAvg += sec.eleWgtCnt()/26.0;

			layer[j - firstLayer] = j - firstLayer;
			caloLen = caloLen + 1;
		}
			for (Int_t j = 0; j < trackVec->size(); j++) {
				HGCSSGenParticle& parton = (*trackVec)[j];
				genCounter += 1;
				Float_t engK = parton.E() -parton.mass();
				genKin += engK;

				if (engK> maxTrackKe){
					maxTrackKe = engK;
					layMax  = parton.layer();
				}

		}

		t1.Fill();
	}
	t1.Write();
	return 1;
}
