//#include "RecoilCorrector.hh" // From: https://github.com/cms-met/MetTools/tree/master/RecoilCorrections

#include "JetAnalyzer.h"


JetAnalyzer::JetAnalyzer(edm::ParameterSet& PSet, edm::ConsumesCollector&& CColl):
    JetToken(CColl.consumes<std::vector<pat::Jet> >(PSet.getParameter<edm::InputTag>("jets"))),
    MetToken(CColl.consumes<std::vector<pat::MET> >(PSet.getParameter<edm::InputTag>("met"))),
    JetId(PSet.getParameter<int>("jetid")),
    Jet1Pt(PSet.getParameter<double>("jet1pt")),
    Jet2Pt(PSet.getParameter<double>("jet2pt")),
    JetEta(PSet.getParameter<double>("jeteta")),
    BTag(PSet.getParameter<std::string>("btag")),
    Jet1BTag(PSet.getParameter<int>("jet1btag")),
    Jet2BTag(PSet.getParameter<int>("jet2btag")),
    UseRecoil(PSet.getParameter<bool>("metRecoil")),
    RecoilMCFile(PSet.getParameter<std::string>("metRecoilMC")),
    RecoilDataFile(PSet.getParameter<std::string>("metRecoilData"))
{
  
    isJESFile=false;
    
    JESFile=new TFile("data/JESUncertainty.root", "READ");
    JESFile->cd();
    if(!JESFile->IsZombie()) {
      hist=(TH2F*)JESFile->Get("test/AK5PFchs");
      isJESFile=true;
    }
    
    // Recoil Corrector
    recoilCorr = new RecoilCorrector(RecoilMCFile);
    recoilCorr->addDataFile(RecoilDataFile);
    recoilCorr->addMCFile(RecoilMCFile);
    
    
    
    std::cout << " --- JetAnalyzer initialization ---" << std::endl;
    std::cout << "  jet Id            :\t" << JetId << std::endl;
    std::cout << "  jet pT [1, 2]     :\t" << Jet1Pt << "\t" << Jet2Pt << std::endl;
    std::cout << "  jet eta           :\t" << JetEta << std::endl;
    std::cout << "  b-tagging algo    :\t" << BTag << std::endl;
    std::cout << "  b-tag cut [1, 2]  :\t" << Jet1BTag << "\t" << Jet2BTag << std::endl;
    std::cout << "  apply recoil corr :\t" << (UseRecoil ? "YES" : "NO") << std::endl;
    std::cout << "  recoil file MC    :\t" << RecoilMCFile << std::endl;
    std::cout << "  recoil file Data  :\t" << RecoilDataFile << std::endl;
    std::cout << std::endl;
}

JetAnalyzer::~JetAnalyzer() {
    JESFile->Close();
    delete recoilCorr;
}





std::vector<pat::Jet> JetAnalyzer::FillJetVector(const edm::Event& iEvent) {
    bool isMC(!iEvent.isRealData());
    int BTagTh(Jet1BTag);
    float PtTh(Jet1Pt), EtaTh(JetEta);
    std::vector<pat::Jet> Vect;
    // Declare and open collection
    edm::Handle<std::vector<pat::Jet> > PFJetsCollection;
    iEvent.getByToken(JetToken, PFJetsCollection);
    // Loop on Jet collection
    for(std::vector<pat::Jet>::const_iterator it=PFJetsCollection->begin(); it!=PFJetsCollection->end(); ++it) {
        if(Vect.size()>0) {
            PtTh=Jet2Pt;
            BTagTh=Jet2BTag;
        }
        pat::Jet jet=*it;
        int idx=it-PFJetsCollection->begin();
        jet.addUserInt("Index", idx);
        // Jet Energy Smearing
        if(isMC) {
            const reco::GenJet* genJet=jet.genJet();
            if(genJet) {
                float smearFactor=GetResolutionRatio(jet.eta());
                reco::Candidate::LorentzVector smearedP4;
                smearedP4=jet.p4()-genJet->p4();
                smearedP4*=smearFactor; // +- 3*smearFactorErr;
                smearedP4+=genJet->p4();
                jet.setP4(smearedP4);
            }
        }
        // Pt and eta cut
        if(jet.pt()<PtTh || fabs(jet.eta())>EtaTh) continue;
        // Quality cut
        if(JetId==1 && !isLooseJet(jet)) continue;
        if(JetId==2 && !isTightJet(jet)) continue;
        if(JetId==3 && !isTightLepVetoJet(jet)) continue;
        // b-tagging
        if(BTagTh==1 && jet.bDiscriminator(BTag)<BTagTh) continue;
        // Fill jet scale uncertainty
        jet.addUserInt("isLoose", isLooseJet(jet) ? 1 : 0);
        jet.addUserInt("isTight", isTightJet(jet) ? 1 : 0);
        jet.addUserInt("isTightLepVeto", isTightLepVetoJet(jet) ? 1 : 0);
        jet.addUserFloat("JESUncertainty", jet.pt()*GetScaleUncertainty(jet));
        Vect.push_back(jet); // Fill vector
    }
    return Vect;
}


void JetAnalyzer::CleanJetsFromMuons(std::vector<pat::Jet>& Jets, std::vector<pat::Muon>& Muons) {
    for(unsigned int j = 0; j < Jets.size(); j++) {
        for(unsigned int m = 0; m < Muons.size(); m++) {
            if(deltaR(Jets[j], Muons[m]) < 0.4) Jets.erase(Jets.begin() + j);
        }
    }
}



pat::MET JetAnalyzer::FillMetVector(const edm::Event& iEvent) {
    
    edm::Handle<std::vector<pat::MET> > MetCollection;
    iEvent.getByToken(MetToken, MetCollection);
    pat::MET MEt = MetCollection->front();
    MEt.addUserFloat("ptRaw", MEt.uncorPt());
    MEt.addUserFloat("phiRaw", MEt.uncorPhi());
    MEt.addUserFloat("ptType1", MEt.pt());
    MEt.addUserFloat("phiType1", MEt.phi());
    return MEt;
}

void JetAnalyzer::ApplyRecoilCorrections(pat::MET& MET, const reco::Candidate::LorentzVector* GenV, const reco::Candidate::LorentzVector* RecoV, int nJets) {
    double MetPt(MET.pt()), MetPhi(MET.phi()), MetPtScaleUp(MET.pt()), MetPhiScaleUp(MET.phi()), MetPtScaleDown(MET.pt()), MetPhiScaleDown(MET.phi()), MetPtResUp(MET.pt()), MetPhiResUp(MET.phi()), MetPtResDown(MET.pt()), MetPhiResDown(MET.phi());
    double GenPt(0.), GenPhi(0.), LepPt(0.), LepPhi(0.), LepPx(0.), LepPy(0.);
    double RecoilX(0.), RecoilY(0.), Upara(0.), Uperp(0.);
    
    if(GenV) {
        GenPt = GenV->pt();
        GenPhi = GenV->phi();
    }
    else {
        throw cms::Exception("JetAnalyzer", "GenV boson is null. No Recoil Correction can be derived");
        return;
    }
    
    if(RecoV) {
        LepPt = RecoV->pt();
        LepPhi = RecoV->phi();
        LepPx = RecoV->px();
        LepPy = RecoV->py();
        RecoilX = - MET.px() - LepPx;
        RecoilY = - MET.py() - LepPy;
        Upara = (RecoilX*LepPx + RecoilY*LepPy) / LepPt;
        Uperp = (RecoilX*LepPy - RecoilY*LepPx) / LepPt;
    }
    
    // Apply Recoil Corrections
    if(UseRecoil) {
        recoilCorr->CorrectType2(MetPt,          MetPhi,          GenPt, GenPhi, LepPt, LepPhi, Upara, Uperp,  0,  0, nJets);
        recoilCorr->CorrectType2(MetPtScaleUp,   MetPhiScaleUp,   GenPt, GenPhi, LepPt, LepPhi, Upara, Uperp,  3,  0, nJets);
        recoilCorr->CorrectType2(MetPtScaleDown, MetPhiScaleDown, GenPt, GenPhi, LepPt, LepPhi, Upara, Uperp, -3,  0, nJets);
        recoilCorr->CorrectType2(MetPtResUp,     MetPhiResUp,     GenPt, GenPhi, LepPt, LepPhi, Upara, Uperp,  0,  3, nJets);
        recoilCorr->CorrectType2(MetPtResDown,   MetPhiResDown,   GenPt, GenPhi, LepPt, LepPhi, Upara, Uperp,  0, -3, nJets);
    }
    
    // Set userFloats for systematics
    MET.addUserFloat("ptScaleUp", MetPtScaleUp);
    MET.addUserFloat("ptScaleDown", MetPtScaleDown);
    MET.addUserFloat("ptResUp", MetPtResUp);
    MET.addUserFloat("ptResDown", MetPtResDown);
    
    // Set new P4
    MET.setP4(reco::Candidate::PolarLorentzVector(MetPt, MET.eta(), MetPhi, MET.mass()));
}

float JetAnalyzer::GetScaleUncertainty(pat::Jet& jet) {
    if(!isJESFile) return 1.;
    float pt(jet.pt()), eta(fabs(jet.eta()));
    return hist->GetBinContent(hist->FindBin(pt, eta));
}

// https://twiki.cern.ch/twiki/bin/view/CMS/JetResolution
float JetAnalyzer::GetResolutionRatio(float eta) {
    eta=fabs(eta);
    if(eta>=0.0 && eta<0.5) return 1.052; // +-0.012+0.062-0.061
    if(eta>=0.5 && eta<1.1) return 1.057; // +-0.012+0.056-0.055
    if(eta>=1.1 && eta<1.7) return 1.096; // +-0.017+0.063-0.062
    if(eta>=1.7 && eta<2.3) return 1.134; // +-0.035+0.087-0.085
    if(eta>=2.3 && eta<5.0) return 1.288; // +-0.127+0.155-0.153
    return -1.;
}
float JetAnalyzer::GetResolutionErrorUp(float eta) {
    eta=fabs(eta);
    if(eta>=0.0 && eta<0.5) return 1.115;
    if(eta>=0.5 && eta<1.1) return 1.114;
    if(eta>=1.1 && eta<1.7) return 1.161;
    if(eta>=1.7 && eta<2.3) return 1.228;
    if(eta>=2.3 && eta<5.0) return 1.488;
    return -1.;
}
float JetAnalyzer::GetResolutionErrorDown(float eta) {
    eta=fabs(eta);
    if(eta>=0.0 && eta<0.5) return 0.990;
    if(eta>=0.5 && eta<1.1) return 1.001;
    if(eta>=1.1 && eta<1.7) return 1.032;
    if(eta>=1.7 && eta<2.3) return 1.042;
    if(eta>=2.3 && eta<5.0) return 1.089;
    return -1.;
}

// PFJet Quality ID 2015-2016: see https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetID#Recommendations_for_13_TeV_data
bool JetAnalyzer::isLooseJet(pat::Jet& jet) {
    if(fabs(jet.eta())<=3.){
        if(jet.neutralHadronEnergyFraction()>=0.99) return false;
        if(jet.neutralEmEnergyFraction()>=0.99) return false;
        if(jet.numberOfDaughters()<=1) return false;
        if(fabs(jet.eta())<=2.4) {
          if(jet.chargedHadronEnergyFraction()<=0.) return false;
          if(jet.chargedEmEnergyFraction()>=0.99) return false;
          if(jet.chargedMultiplicity()<=0) return false;
        }
    }
    else{
        if(jet.neutralEmEnergyFraction()>=0.90) return false;
	if(jet.neutralMultiplicity()<=10) return false;
    }
    return true;
}

bool JetAnalyzer::isTightJet(pat::Jet& jet) {
    if(fabs(jet.eta())<=3.){
        if(jet.neutralHadronEnergyFraction()>=0.90) return false;
        if(jet.neutralEmEnergyFraction()>=0.90) return false;
        if(jet.numberOfDaughters()<=1) return false;
        if(fabs(jet.eta())<=2.4) {
          if(jet.chargedHadronEnergyFraction()<=0.) return false;
          if(jet.chargedEmEnergyFraction()>=0.99) return false;
          if(jet.chargedMultiplicity()<=0) return false;
        }
    }
    else{
        if(jet.neutralEmEnergyFraction()>=0.90) return false;
	if(jet.neutralMultiplicity()<=10) return false;
    }
    return true;
}

bool JetAnalyzer::isTightLepVetoJet(pat::Jet& jet) {
    if(fabs(jet.eta())<=3.){
        if(jet.neutralHadronEnergyFraction()>=0.90) return false;
        if(jet.neutralEmEnergyFraction()>=0.90) return false;
        if(jet.numberOfDaughters()<=1) return false;
        if(jet.muonEnergyFraction()>=0.8) return false;
        if(fabs(jet.eta())<=2.4) {
          if(jet.chargedHadronEnergyFraction()<=0.) return false;
          if(jet.chargedEmEnergyFraction()>=0.90) return false;
          if(jet.chargedMultiplicity()<=0) return false;
        }
    }
    else{
        if(jet.neutralEmEnergyFraction()>=0.90) return false;
	if(jet.neutralMultiplicity()<=10) return false;
    }
    return true;
}


/*
bool JetAnalyzer::isMediumJet(pat::Jet& jet) {
    if(jet.neutralHadronEnergyFraction()>0.95) return false;
    if(jet.neutralEmEnergyFraction()>0.95) return false;
    if(jet.numberOfDaughters()<=1) return false;
    if(fabs(jet.eta())<2.4) {
      if(jet.chargedHadronEnergyFraction()<=0.) return false;
      if(jet.chargedEmEnergyFraction()>0.99) return false;
      if(jet.chargedMultiplicity()<=0) return false;
    }
    return true;
}
bool JetAnalyzer::isTightJet(pat::Jet& jet) {
    if(jet.neutralHadronEnergyFraction()>0.90) return false;
    if(jet.neutralEmEnergyFraction()>0.90) return false;
    if(jet.numberOfDaughters()<=1) return false;
    if(fabs(jet.eta())<2.4) {
      if(jet.chargedHadronEnergyFraction()<=0.) return false;
      if(jet.chargedEmEnergyFraction()>0.99) return false;
      if(jet.chargedMultiplicity()<=0) return false;
    }
    return true;
}

*/


