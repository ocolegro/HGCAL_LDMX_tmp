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

int main()
{
  freopen( "hadron_log.txt", "w", stdout );

  TFile *infile = TFile::Open("/afs/cern.ch/work/o/ocolegro/test/git_hexaV02-01-01/version_30/model_0/e-/BOFF/et_30/eta_10000.000/run_0/HGcal__version30_model0_BOFF_et30_eta10000.000_run0.root");
  //TFile *infile = TFile::Open("/afs/cern.ch/work/o/ocolegro/electron_high_stats_model1/git_hexaV02-01-01/version_30/model_1/e-/BOFF/et_30/eta_10000.000/run_0/HGcal__version30_model1_BOFF_et30_eta10000.000_run0.root");

  TTree *tree = (TTree*)infile->Get("HGCSSTree");

  std::vector<HGCSSSamplingSection> * simhits = 0;
  tree->SetBranchAddress("HGCSSSamplingSectionVec",&simhits);

  unsigned nEvts = tree->GetEntries();

  TFile hfile("simhits.root","RECREATE");
  TTree t1("hadrons","Hadron Study");

  Float_t Eng_dep;
  t1.Branch("Eng_dep",&Eng_dep,"Eng_dep/F");


  for (unsigned ievt(0); ievt<nEvts; ++ievt){//loop on entries
    tree->GetEntry(ievt);

    Double_t eng = 0;
	std::cout << "The event is " << ievt << std::endl;

	std::cout << eng << std::endl;

    for (Int_t j = 0; j<simhits->size();j++)
    {
    	HGCSSSamplingSection& sec =(*simhits)[j];
    	eng += sec.measuredE();
    	std::cout << sec.getTotalSensE() << std::endl;
    }
	std::cout << eng << std::endl;

    Eng_dep = eng;
    t1.Fill();
  }
  t1.Write();

return 1;
}
