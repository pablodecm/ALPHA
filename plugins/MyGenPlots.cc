#include "TH1F.h"
#include "TMath.h"

#include <iostream>

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenRunInfoProduct.h"

// essentials !!!
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ServiceRegistry/interface/Service.h" 
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1.h"

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/JetReco/interface/Jet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/Candidate/interface/CompositeRefCandidateT.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "DataFormats/HepMCCandidate/interface/GenStatusFlags.h"
#include <vector>

class MyGenPlots : public edm::EDAnalyzer {
 public:
  explicit MyGenPlots(const edm::ParameterSet&);
  void analyze(const edm::Event&, const edm::EventSetup&);

 private:
  TH1F* ZLepMass;
  TH1F* ZHadMass;
   
};

MyGenPlots::MyGenPlots(const edm::ParameterSet& cfg)
{
  edm::Service<TFileService> fs;
  ZLepMass = fs->make<TH1F>("ZLepMass", "ZLepMass", 4000, 0, 4000);
  ZHadMass = fs->make<TH1F>("ZHadMass", "ZHadMass", 4000, 0, 4000);

}

using namespace edm;
//using namespace std;
//using namespace reco;

void MyGenPlots::analyze(const edm::Event& event, const edm::EventSetup& setup) {

  
  //edm::Handle< vector<reco::GenJet> > genParticles;
  //event.getByLabel("slimmedGenJets", genParticles);
  edm::Handle<reco::GenParticleCollection> genParticles;
  event.getByLabel("prunedGenParticles", genParticles);
  /*for(size_t i = 0; i < genParticles->size(); ++ i) {
    const GenParticle & p = (*genParticles)[i];
    int id = p.pdgId();
    int st = p.status();  
    double pt = p.pt();
    double mass = p.mass(), eta = p.eta(), phi = p.phi(), rap = p.rapidity();
    size_t n = p.numberOfDaughters();

    if (fabs(id) == 23){ 
      std::cout << "Status della Z: " << st << std::endl;
      std::cout << "Figlie della Z: " << n << std::endl;
    }  
    }*/
  
}

DEFINE_FWK_MODULE(MyGenPlots);
