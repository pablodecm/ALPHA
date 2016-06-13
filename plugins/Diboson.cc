// -*- C++ -*-
//
// Package:    Analysis/Diboson
// Class:      Diboson
// 
/**\class Diboson Diboson.cc Analysis/Diboson/plugins/Diboson.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Alberto Zucchetta
//         Created:  Thu, 28 Apr 2016 08:28:54 GMT
//
//

#include "Diboson.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
Diboson::Diboson(const edm::ParameterSet& iConfig):
    GenPSet(iConfig.getParameter<edm::ParameterSet>("genSet")),
    PileupPSet(iConfig.getParameter<edm::ParameterSet>("pileupSet")),
    TriggerPSet(iConfig.getParameter<edm::ParameterSet>("triggerSet")),
    ElectronPSet(iConfig.getParameter<edm::ParameterSet>("electronSet")),
    MuonPSet(iConfig.getParameter<edm::ParameterSet>("muonSet")),
    TauPSet(iConfig.getParameter<edm::ParameterSet>("tauSet")),
    PhotonPSet(iConfig.getParameter<edm::ParameterSet>("photonSet")),
    JetPSet(iConfig.getParameter<edm::ParameterSet>("jetSet")),
    FatJetPSet(iConfig.getParameter<edm::ParameterSet>("fatJetSet")),
    WriteNElectrons(iConfig.getParameter<int>("writeNElectrons")),
    WriteNMuons(iConfig.getParameter<int>("writeNMuons")),
    WriteNLeptons(iConfig.getParameter<int>("writeNLeptons")),
    WriteNTaus(iConfig.getParameter<int>("writeNTaus")),
    WriteNPhotons(iConfig.getParameter<int>("writeNPhotons")),
    WriteNJets(iConfig.getParameter<int>("writeNJets")),
    WriteNFatJets(iConfig.getParameter<int>("writeNFatJets")),
    HistFile(iConfig.getParameter<std::string>("histFile")),
    Verbose(iConfig.getParameter<bool>("verbose"))
{
    //now do what ever initialization is needed
    usesResource("TFileService");
    
    // Initialize Objects
    theGenAnalyzer      = new GenAnalyzer(GenPSet, consumesCollector());
    thePileupAnalyzer   = new PileupAnalyzer(PileupPSet, consumesCollector());
    theTriggerAnalyzer  = new TriggerAnalyzer(TriggerPSet, consumesCollector());
    theElectronAnalyzer = new ElectronAnalyzer(ElectronPSet, consumesCollector());
    theMuonAnalyzer     = new MuonAnalyzer(MuonPSet, consumesCollector());
    theTauAnalyzer      = new TauAnalyzer(TauPSet, consumesCollector());
    thePhotonAnalyzer   = new PhotonAnalyzer(PhotonPSet, consumesCollector());
    theJetAnalyzer      = new JetAnalyzer(JetPSet, consumesCollector());
    theFatJetAnalyzer   = new JetAnalyzer(FatJetPSet, consumesCollector());
    //theBTagAnalyzer     = new BTagAnalyzer(BTagAlgo);
    
    std::vector<std::string> TriggerList(TriggerPSet.getParameter<std::vector<std::string> >("paths"));
    for(unsigned int i = 0; i < TriggerList.size(); i++) TriggerMap[ TriggerList[i] ] = false;
    
    
    // ---------- Plots Initialization ----------
    TFileDirectory allDir=fs->mkdir("All/");
    TFileDirectory genDir=fs->mkdir("Gen/");
    TFileDirectory eleDir=fs->mkdir("Electrons/");
    TFileDirectory muoDir=fs->mkdir("Muons/");
    TFileDirectory tauDir=fs->mkdir("Taus/");
    TFileDirectory phoDir=fs->mkdir("Photons/");
    TFileDirectory jetDir=fs->mkdir("Jets/");
    TFileDirectory kinDir=fs->mkdir("Kin/");
    
    // Make TH1F
    std::vector<std::string> nLabels={"All", "Trigger", "Iso Lep #geq 2", "Z cand ", "Jets #geq 2", "Z mass ", "bJets #geq 1", "bJets #geq 2", "h mass ", "#slash{E}_{T}"};
    
    int nbins;
    float min, max;
    std::string name, title, opt;
    
    ifstream histFile(HistFile);
    if(!histFile.is_open()) {
        throw cms::Exception("Diboson Analyzer", HistFile + " file not found");
    }
    while(histFile >> name >> title >> nbins >> min >> max >> opt) {
        if(name.find('#')==std::string::npos) {
            while(title.find("~")!=std::string::npos) title=title.replace(title.find("~"), 1, " "); // Remove ~
            if(name.substr(0, 2)=="a_") Hist[name] = allDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max); //.substr(2)
            if(name.substr(0, 2)=="g_") Hist[name] = genDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="e_") Hist[name] = eleDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="m_") Hist[name] = muoDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="t_") Hist[name] = tauDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="p_") Hist[name] = phoDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="j_") Hist[name] = jetDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="k_") Hist[name] = kinDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            Hist[name]->Sumw2();
            Hist[name]->SetOption(opt.c_str());
            // Particular histograms
            if(name=="a_nEvents" || name=="e_nEvents" || name=="m_nEvents") for(unsigned int i=0; i<nLabels.size(); i++) Hist[name]->GetXaxis()->SetBinLabel(i+1, nLabels[i].c_str());
        }
    }
    histFile.close();
    
    std::cout << "---------- STARTING ----------" << std::endl;
}


Diboson::~Diboson() {
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    std::cout << "---------- ENDING  ----------" << std::endl;
    
    
    delete theGenAnalyzer;
    delete thePileupAnalyzer;
    delete theTriggerAnalyzer;
    delete theElectronAnalyzer;
    delete theMuonAnalyzer;
    delete theTauAnalyzer;
    delete thePhotonAnalyzer;
    delete theJetAnalyzer;
    delete theFatJetAnalyzer;
    //delete theBTagAnalyzer;
    
}


//
// member functions
//

// ------------ method called for each event  ------------
void Diboson::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    isMC = !iEvent.isRealData();
    EventNumber = iEvent.id().event();
    LumiNumber = iEvent.luminosityBlock();
    RunNumber = iEvent.id().run();
    
    EventWeight = PUWeight = TriggerWeight = LeptonWeight = 1.;
    isZtoEE = isZtoMM = isWtoEN = isWtoMN = isZtoNN = false;
    nPV = nElectrons = nMuons = nTaus = nPhotons = nJets = nFatJets = nBTagJets = 1;
    Chi2 = -1.;
    
    AddFourMomenta addP4;
    
    // Initialize types
    for(int i = 0; i < WriteNElectrons; i++) ObjectsFormat::ResetLeptonType(Electrons[i]);
    for(int i = 0; i < WriteNMuons; i++) ObjectsFormat::ResetLeptonType(Muons[i]);
    for(int i = 0; i < WriteNLeptons; i++) ObjectsFormat::ResetLeptonType(Leptons[i]);
    for(int i = 0; i < WriteNTaus; i++) ObjectsFormat::ResetTauType(Taus[i]);
    for(int i = 0; i < WriteNPhotons; i++) ObjectsFormat::ResetPhotonType(Photons[i]);
    for(int i = 0; i < WriteNJets; i++) ObjectsFormat::ResetJetType(Jets[i]);
    for(int i = 0; i < WriteNFatJets; i++) ObjectsFormat::ResetFatJetType(FatJets[i]);
    ObjectsFormat::ResetMEtType(MEt);
    ObjectsFormat::ResetCandidateType(V);
    ObjectsFormat::ResetCandidateType(HMerged);
    ObjectsFormat::ResetCandidateType(HResolved);
    ObjectsFormat::ResetLorentzType(kH);
    ObjectsFormat::ResetLorentzType(kX);
    
    Hist["a_nEvents"]->Fill(1., EventWeight);
    Hist["e_nEvents"]->Fill(1., EventWeight);
    Hist["m_nEvents"]->Fill(1., EventWeight);
    
    // -----------------------------------
    //           READ OBJECTS
    // -----------------------------------
    
    // Pu weight
    PUWeight = thePileupAnalyzer->GetPUWeight(iEvent);
    nPV = thePileupAnalyzer->GetPV(iEvent);
    EventWeight *= PUWeight;
    
    // Trigger
    theTriggerAnalyzer->FillTriggerMap(iEvent, TriggerMap);
    EventWeight *= TriggerWeight;
    
    // Electrons
    std::vector<pat::Electron> ElecVect = theElectronAnalyzer->FillElectronVector(iEvent);
    nElectrons = ElecVect.size();
    // Muons
    std::vector<pat::Muon> MuonVect = theMuonAnalyzer->FillMuonVector(iEvent);
    nMuons = MuonVect.size();
    // Taus
    std::vector<pat::Tau> TauVect = theTauAnalyzer->FillTauVector(iEvent);
    theTauAnalyzer->CleanTausFromMuons(TauVect, MuonVect, 0.4);
    theTauAnalyzer->CleanTausFromElectrons(TauVect, ElecVect, 0.4);
    nTaus = TauVect.size();
    // Photons
    std::vector<pat::Photon> PhotonVect = thePhotonAnalyzer->FillPhotonVector(iEvent);
    if(TriggerMap.find("HLT_DoublePhoton60_v") != TriggerMap.end() && TriggerMap["HLT_DoublePhoton60_v"]) thePhotonAnalyzer->PlotPhotons(PhotonVect, Hist, EventWeight);
    nPhotons = PhotonVect.size();
    // Jets
    std::vector<pat::Jet> JetsVect = theJetAnalyzer->FillJetVector(iEvent);
    theJetAnalyzer->CleanJetsFromMuons(JetsVect, MuonVect, 0.4);
    theJetAnalyzer->CleanJetsFromElectrons(JetsVect, ElecVect, 0.4);
    nJets = JetsVect.size();
    nBTagJets = theJetAnalyzer->GetNBJets(JetsVect);
    // Fat Jets
    std::vector<pat::Jet> FatJetsVect = theFatJetAnalyzer->FillJetVector(iEvent);
    theFatJetAnalyzer->CleanJetsFromMuons(FatJetsVect, MuonVect, 1.);
    theFatJetAnalyzer->CleanJetsFromElectrons(FatJetsVect, ElecVect, 1.);
    nFatJets = FatJetsVect.size();
    // Missing Energy
    pat::MET MET = theJetAnalyzer->FillMetVector(iEvent);
    pat::MET Neutrino(MET);
    //theJetAnalyzer->ApplyRecoilCorrections(MET, &MET.genMET()->p4(), &theV.p4(), 0);
    
    
    
    // -----------------------------------
    //           GEN LEVEL
    // -----------------------------------
    
    // Gen weights
    std::map<std::string, float> GenWeight = theGenAnalyzer->FillWeightsMap(iEvent);
    EventWeight *= GenWeight["event"];
    // Gen Particles
    std::vector<reco::GenParticle> GenPVect = theGenAnalyzer->FillGenVector(iEvent);
    // Gen candidates
    //reco::Candidate* theGenZ = theGenAnalyzer->FindGenParticle(GenPVect, 23);
    
    std::vector<int> LepIds = {11,13};
    std::vector<int> HadIds = {1,2,3,4,5};
    reco::GenParticle* theGenLep = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, LepIds);
    reco::GenParticle* theGenHad = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, HadIds);

    //Gen level plots and candidates
    double GenZLepMass = -1.;
    double GenZHadMass = -1.;
    double GenZHadPt = -1.;
    double GenXMass = -1.;
    double GenHadDR = -9.;
    bool isGenZZ = false;
    if(theGenLep!=NULL && theGenHad!=NULL){
        const reco::Candidate* theGenZLep = theGenAnalyzer->FindMother(theGenLep);
        const reco::Candidate* theGenZHad = theGenAnalyzer->FindMother(theGenHad);
	for(unsigned int a = 0; a<=theGenZHad->numberOfDaughters(); a++){
            if(theGenZHad!=NULL && theGenZHad->daughter(a)!=NULL && (theGenZHad->pdgId()==23 || theGenZHad->pdgId()==25) && (theGenZHad->daughter(a)->pdgId() == - theGenHad->pdgId())){
	        GenHadDR = reco::deltaR(theGenHad->eta(),theGenHad->phi(),theGenZHad->daughter(a)->eta(),theGenZHad->daughter(a)->phi());
                break;
            }
        }
        if(theGenZLep!=NULL && theGenZLep->pdgId()==23 && theGenZHad!=NULL && (theGenZHad->pdgId()==23 || theGenZHad->pdgId()==25)) {
            Hist["g_ZLepMass"]->Fill(theGenZLep->mass(), EventWeight);
            Hist["g_ZLepPt"]->Fill(theGenZLep->pt(), EventWeight);
            Hist["g_LepPt"]->Fill(theGenLep->pt(), EventWeight);
            GenZLepMass = theGenZLep->mass();
            Hist["g_ZHadMass"]->Fill(theGenZHad->mass(), EventWeight);
            Hist["g_ZHadPt"]->Fill(theGenZHad->pt(), EventWeight);
            Hist["g_HadPt"]->Fill(theGenHad->pt(), EventWeight);
            Hist["g_HadEta"]->Fill(theGenHad->eta(), EventWeight);
            Hist["g_HadDR"]->Fill(GenHadDR, EventWeight);
            Hist["g_ZZDR"]->Fill(reco::deltaR(theGenZHad->eta(),theGenZHad->phi(),theGenZLep->eta(),theGenZLep->phi()), EventWeight);
            Hist["g_ZZDPhi"]->Fill(reco::deltaPhi(theGenZHad->phi(),theGenZLep->phi()), EventWeight);
            Hist["g_LepHadDR"]->Fill(reco::deltaR(theGenHad->eta(),theGenHad->phi(),theGenLep->eta(),theGenLep->phi()), EventWeight);
            GenZHadMass = theGenZHad->mass();
            GenZHadPt = theGenZHad->pt();
            isGenZZ = true;
        }
    }

    
    reco::Candidate* theGenX = theGenAnalyzer->FindGenParticleByIdAndStatus(GenPVect, 39, 62);
    if(theGenX!=NULL && theGenLep!=NULL && theGenHad!=NULL && isGenZZ){
        Hist["g_XMass"]->Fill(theGenX->mass(), EventWeight);
        Hist["g_XPt"]->Fill(theGenX->pt(), EventWeight);
        GenXMass = theGenX->mass();
    }
    

    // ---------- Trigger selections ----------
    // Dummy trigger
    //TriggerWeight*=theElectronAnalyzer->GetDoubleElectronTriggerSF(ElecVect.at(0), ElecVect.at(1));
    //TriggerWeight*=theMuonAnalyzer->GetDoubleMuonTriggerSF(MuonVect.at(0), MuonVect.at(1));
    
    
    
    Hist["a_nEvents"]->Fill(2., EventWeight);
    Hist["e_nEvents"]->Fill(2., EventWeight);
    Hist["m_nEvents"]->Fill(2., EventWeight);
    
    
    
    // -----------------------------------
    //           VECTOR BOSON
    // -----------------------------------
    
    // Categorization depending on the number of leptons
    
    // ---------- Z TO LEPTONS ----------
    if(MuonVect.size()>=2 || ElecVect.size()>=2) {
        if(MuonVect.size()>=2 && ElecVect.size()>=2) {
            if(MuonVect.at(0).pt() > ElecVect.at(0).pt()) isZtoMM=true;
            else isZtoEE=true;
        }
        else if(ElecVect.size()>=2) isZtoEE=true;
        else if(MuonVect.size()>=2) isZtoMM=true;
        else {if(Verbose) std::cout << " - No Iso SF OS Leptons" << std::endl;}
    }
    // ---------- W TO LEPTON and NEUTRINO ----------
    else if(MuonVect.size()==1 || ElecVect.size()==1) {
        if(MuonVect.size()==1 && ElecVect.size()==1) {
            if(MuonVect.at(0).pt() > ElecVect.at(0).pt()) isWtoMN=true;
            else isWtoEN=true;
        }
        else if(ElecVect.size()==1) isWtoEN=true;
        else if(MuonVect.size()==1) isWtoMN=true;
        else {if(Verbose) std::cout << " - No Iso Leptons" << std::endl;}
    }
    // ----------- Z TO NEUTRINOS ---------------
    else if(MET.pt() > 200.) {
        isZtoNN=true;
        if(Verbose) std::cout << " - No charged leptons" << std::endl;
    }
    else {if(Verbose) std::cout << " - No V candidate" << std::endl; return;}
    
    Hist["a_nEvents"]->Fill(3., EventWeight);
    if(isZtoEE) Hist["e_nEvents"]->Fill(3., EventWeight);
    if(isZtoMM) Hist["m_nEvents"]->Fill(3., EventWeight);


    // ---------- Reconstruct V candidate ----------
    pat::CompositeCandidate theV;
    if(isZtoMM) {
        if(MuonVect.at(0).charge()*MuonVect.at(1).charge()<0 && (MuonVect[0].p4() + MuonVect[1].p4()).mass() > 50.) {
            theV.addDaughter(MuonVect.at(0).charge() < 0 ? MuonVect.at(0) : MuonVect.at(1));
            theV.addDaughter(MuonVect.at(0).charge() < 0 ? MuonVect.at(1) : MuonVect.at(0));
            addP4.set(theV);
            // SF
            LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1id"));
            LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(1), MuonPSet.getParameter<int>("muon2id"));
            LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1iso"));
            LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(1), MuonPSet.getParameter<int>("muon2iso"));
        }
        else { if(Verbose) std::cout << " - No OS muons" << std::endl; return; }
    }
    else if(isZtoEE) {
        if(ElecVect.at(0).charge()*ElecVect.at(1).charge()<0 && (ElecVect[0].p4() + ElecVect[1].p4()).mass() > 50.) {
            theV.addDaughter(ElecVect.at(0).charge() ? ElecVect.at(0) : ElecVect.at(1));
            theV.addDaughter(ElecVect.at(0).charge() ? ElecVect.at(1) : ElecVect.at(0));
            addP4.set(theV);
            // SF
            LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(0), ElectronPSet.getParameter<int>("electron1id"));
            LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(1), ElectronPSet.getParameter<int>("electron2id"));
            LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(0));
            LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(1));
        }
        else { if(Verbose) std::cout << " - No OS electrons" << std::endl; return; }
    }
    
    else if(isWtoMN) {
        // W kinematic reconstruction
        float pz = Utilities::RecoverNeutrinoPz(&MuonVect.at(0).p4(), &MET.p4());
        Neutrino.setP4(reco::Particle::LorentzVector(MET.px(), MET.py(), pz, sqrt(MET.pt()*MET.pt() + pz*pz) ));
        theV.addDaughter(MuonVect.at(0));
        theV.addDaughter(Neutrino);
        theV.setCharge(MuonVect.at(0).charge());
        addP4.set(theV);
        // SF
        LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1id"));
        LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1iso"));
    }
    else if(isWtoEN) {
        // W kinematic reconstruction
        float pz = Utilities::RecoverNeutrinoPz(&ElecVect.at(0).p4(), &MET.p4());
        Neutrino.setP4(reco::Particle::LorentzVector(MET.px(), MET.py(), pz, sqrt(MET.pt()*MET.pt() + pz*pz) ));
        theV.addDaughter(ElecVect.at(0));
        theV.addDaughter(Neutrino);
        theV.setCharge(ElecVect.at(0).charge());
        addP4.set(theV);
        // SF
        LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(0), ElectronPSet.getParameter<int>("electron1id"));
        LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(0));
    }
    else if(isZtoNN) {
        theV.addDaughter(MET);
        addP4.set(theV);
    }
    
    else { if(Verbose) std::cout << " - No reconstructible V candidate" << std::endl; return; }
    // Update event weight with lepton selections
    EventWeight *= LeptonWeight;
    
    Hist["a_nEvents"]->Fill(4., EventWeight);
    if(isZtoEE) Hist["e_nEvents"]->Fill(4., EventWeight);
    if(isZtoMM) Hist["m_nEvents"]->Fill(4., EventWeight);
    
    if(isZtoEE) Hist["e_Zmass"]->Fill(theV.mass(), EventWeight);
    if(isZtoMM) Hist["m_Zmass"]->Fill(theV.mass(), EventWeight);
    
    // -----------------------------------
    //           HADRONIC BOSON
    // -----------------------------------
    
    
    // ---------- Z TO HADRONS ----------
    pat::CompositeCandidate theHMerged;
    pat::CompositeCandidate theHResolved;
    pat::CompositeCandidate theHResolvedHpt;
    pat::CompositeCandidate theHResolvedDZ;
    pat::CompositeCandidate theHResolvedDR;
    pat::CompositeCandidate theH;
    pat::CompositeCandidate theX;
    isMerged = isResolved = false;
    
    /////////////////// Highest pT method ////////////////////  
    // Resolved topology
    if(nJets < 2) {if(Verbose) std::cout << " - N jets < 2" << std::endl;} // return;}
    else {
        theH.addDaughter(JetsVect.at(0));
        theH.addDaughter(JetsVect.at(1));
        addP4.set(theH);
        //std::cout << "resolved mass: " << theH.mass() << std::endl;
        //if(theH.mass()<40 || theH.pt()<100) theH.clearDaughters();
        //Hist["a_HAK4Mass_HPt"]->Fill(theH.mass(), EventWeight);
        Hist["a_nEvents"]->Fill(5., EventWeight);
        if(isZtoEE) Hist["e_nEvents"]->Fill(5., EventWeight);
        if(isZtoMM) Hist["m_nEvents"]->Fill(5., EventWeight);
    }

    // Boosted topology
    if(nFatJets < 1) {if(Verbose) std::cout << " - N fat jets < 1" << std::endl;}
    else {
        //Hist["a_HAK8Mass_HPt"]->Fill(FatJetsVect.at(0).hasUserFloat("ak8PFJetsCHSSoftDropMass")?FatJetsVect.at(0).userFloat("ak8PFJetsCHSSoftDropMass"):FatJetsVect.at(0).mass(), EventWeight);
        //std::cout << "merged mass: " << FatJetsVect.at(0).mass() << std::endl;
        if(theH.pt()<FatJetsVect.at(0).pt()) {
            theH.clearDaughters();
            theH.addDaughter(FatJetsVect.at(0));
            addP4.set(theH);
            //theH.addUserFloat("softdropMass", FatJetsVect.at(0).userFloat("ak8PFJetsCHSSoftDropMass"));
            //if(theH.mass()>180) theH.clearDaughters();
        }
    }
    Hist["a_HMass_HPt"]->Fill(theH.hasUserFloat("softdropMass") ? theH.userFloat("softdropMass") : theH.mass(), EventWeight);
    //std::cout << "chosen mass: " << theH.mass() << std::endl;
    
    // Reset theH
    //theH.clearDaughters();
    
    
    /////////////////// Prefer merged AK8 jet method ////////////////////
    
    // Boosted topology
    if(nFatJets >= 1) {
        isMerged = true;
        theHMerged.addDaughter(FatJetsVect.at(0));
        addP4.set(theHMerged);
        //theHMerged.addUserFloat("softdropMass", FatJetsVect.at(0).userFloat("ak8PFJetsCHSSoftDropMass"));
    }

    //printout chosen jet number
    unsigned int ch1(100), ch2(100);    
    // Resolved

    if(nJets >= 2) {
        isResolved = true;
        // chose the two highest-pt jets
        theHResolved.addDaughter(JetsVect.at(0));
        theHResolved.addDaughter(JetsVect.at(1));
        addP4.set(theHResolved);
        // chose the two jets whose candidate has highest pt
        float ptmin(-1.);
        for(unsigned int i1 = 0; i1 < JetsVect.size(); i1++) {
            for(unsigned int i2 = i1+1; i2 < JetsVect.size(); i2++) {
                if( (JetsVect[i1].p4()+JetsVect[i2].p4()).pt() > ptmin ) {
                    ptmin = (JetsVect[i1].p4()+JetsVect[i2].p4()).pt();
                    ch1 = i1;
                    ch2 = i2;
                    if(theHResolvedHpt.numberOfDaughters()>0) theHResolvedHpt.clearDaughters();
                    theHResolvedHpt.addDaughter(JetsVect.at(i1));
                    theHResolvedHpt.addDaughter(JetsVect.at(i2));
                    addP4.set(theHResolvedHpt);
                }
            }
        }
        
        //MC truth histograms
        if(isMC && isGenZZ) {
            Hist["a_den_H_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
            Hist["a_den_H_truth_HadDR"]->Fill(GenHadDR, EventWeight);
	      }
 
        if(isMC && JetsVect.at(0).genParton()!=NULL && JetsVect.at(1).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(0).genParton())==23 && Utilities::FindMotherId(JetsVect.at(1).genParton())==23){
                Hist["a_num_H_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
                Hist["a_num_H_truth_HadDR"]->Fill(GenHadDR, EventWeight);
            }
        }

        if(isMC && (ch1<=JetsVect.size() && ch2<=JetsVect.size())  && JetsVect.at(ch1).genParton()!=NULL && JetsVect.at(ch2).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(ch1).genParton())==23 && Utilities::FindMotherId(JetsVect.at(ch2).genParton())==23){
                Hist["a_num_HHpt_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
                Hist["a_num_HHpt_truth_HadDR"]->Fill(GenHadDR, EventWeight);
            }
        }
        ch1 = 100;
        ch2 = 100;
        
        if(Verbose) std::cout << " - Jet index " << ch1 << "  " << ch2 << std::endl;
        
        //
        // chose the two jets whose mass is closest to Z
        float DZmin(1000.);
        for(unsigned int i1 = 0; i1 < JetsVect.size(); i1++) {
            for(unsigned int i2 = i1+1; i2 < JetsVect.size(); i2++) {
                if( fabs((JetsVect[i1].p4()+JetsVect[i2].p4()).M() - 91) < DZmin ) {
                    DZmin = fabs((JetsVect[i1].p4()+JetsVect[i2].p4()).M()-91);
                    ch1 = i1;
                    ch2 = i2;
                    if(theHResolvedDZ.numberOfDaughters()>0) theHResolvedDZ.clearDaughters();
                    theHResolvedDZ.addDaughter(JetsVect.at(i1));
                    theHResolvedDZ.addDaughter(JetsVect.at(i2));
                    addP4.set(theHResolvedDZ);
                }
            }
        }
        
        //std::cout << "DZ: chosen jets " << ch1 << ", " << ch2 << ", mass " << theHResolvedDZ.mass()  << std::endl;
        if(isMC && (ch1<=JetsVect.size() && ch2<=JetsVect.size()) && JetsVect.at(ch1).genParton()!=NULL && JetsVect.at(ch2).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(ch1).genParton())==23 && Utilities::FindMotherId(JetsVect.at(ch2).genParton())==23){
                Hist["a_num_HDZ_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
                Hist["a_num_HDZ_truth_HadDR"]->Fill(GenHadDR, EventWeight);
            }
        }
        ch1 = 100;
        ch2 = 100;
        //	
        
        // chose the two closest jets in DR
        float DRmin(10.);
        for(unsigned int i1 = 0; i1 < JetsVect.size(); i1++) {
            for(unsigned int i2 = i1+1; i2 < JetsVect.size(); i2++) {
                if( deltaR(JetsVect[i1],JetsVect[i2]) < DRmin ) {
                    DRmin = deltaR(JetsVect[i1],JetsVect[i2]);
                    ch1 = i1;
                    ch2 = i2;
                    if(theHResolvedDR.numberOfDaughters()>0) theHResolvedDR.clearDaughters();
                    theHResolvedDR.addDaughter(JetsVect.at(i1));
                    theHResolvedDR.addDaughter(JetsVect.at(i2));
                    addP4.set(theHResolvedDR);
                }
            }
        }
        
        if(isMC && (ch1<=JetsVect.size() && ch2<=JetsVect.size()) && JetsVect.at(ch1).genParton()!=NULL && JetsVect.at(ch2).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(ch1).genParton())==23 && Utilities::FindMotherId(JetsVect.at(ch2).genParton())==23){
                Hist["a_num_HDR_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
                Hist["a_num_HDR_truth_HadDR"]->Fill(GenHadDR, EventWeight);
	          }
        }
        
       //	
    }
    

    // Global candidate
    pat::CompositeCandidate theXMerged;
    pat::CompositeCandidate theXResolved;
    pat::CompositeCandidate theXResolvedHpt;
    pat::CompositeCandidate theXResolvedDZ;
    pat::CompositeCandidate theXResolvedDR;
    
    theXMerged.addDaughter(theV);
    if(theHMerged.numberOfDaughters()>0) theXMerged.addDaughter(theHMerged);
    addP4.set(theXMerged);
    
    theXResolved.addDaughter(theV);
    if(theHResolved.numberOfDaughters()>0) theXResolved.addDaughter(theHResolved);
    addP4.set(theXResolved);
    
    theXResolvedHpt.addDaughter(theV);
    if(theHResolvedHpt.numberOfDaughters()>0) theXResolvedHpt.addDaughter(theHResolvedHpt);
    addP4.set(theXResolvedHpt);
    
    theXResolvedDZ.addDaughter(theV);
    if(theHResolvedDZ.numberOfDaughters()>0) theXResolvedDZ.addDaughter(theHResolvedDZ);
    addP4.set(theXResolvedDZ);

    theXResolvedDR.addDaughter(theV);
    if(theHResolvedDR.numberOfDaughters()>0) theXResolvedDR.addDaughter(theHResolvedDR);
    addP4.set(theXResolvedDR);

    if(isResolved || isMerged) {
        Hist["a_nEvents"]->Fill(6., EventWeight);
        if(isZtoMM) Hist["m_nEvents"]->Fill(6., EventWeight);
        if(isZtoEE) Hist["e_nEvents"]->Fill(6., EventWeight);
    }
//    else {
//        if(Verbose) std::cout << "No dilepton, no X!" << std::endl;
//        return;
//    }

    /*
    else if(theH.numberOfDaughters()>0){//if is Z to nu nu: apply recoil mass formula
        theX = recoilMassFormula(theH,MET);
    }

    else{//no H, no X
        if(Verbose) std::cout << "No H, no X" << std::endl;
        return;
    }
    */
    reco::Candidate::LorentzVector thekH;
    reco::Candidate::LorentzVector thekX;
    if(nJets < 2) {if(Verbose) std::cout << " - N jets < 2" << std::endl;} // return;}
    else {
        theH.addDaughter(JetsVect.at(0));
        theH.addDaughter(JetsVect.at(1));
        addP4.set(theH);
        
        theX.addDaughter(theV);
        theX.addDaughter(theH);
        addP4.set(theX);
        
        Hist["a_nEvents"]->Fill(5., EventWeight);
        if(isZtoEE) Hist["e_nEvents"]->Fill(5., EventWeight);
        if(isZtoMM) Hist["m_nEvents"]->Fill(5., EventWeight);
        
        // ----------- KINEMATIC FIT -----------
        reco::Candidate::LorentzVector fJet1 = JetsVect.at(0).p4();
        reco::Candidate::LorentzVector fJet2 = JetsVect.at(1).p4();
        if(Verbose) std::cout << "Performing kinematic fit..." << std::endl;
        Chi2 = Utilities::PerformKinematicFit(&JetsVect.at(0), &JetsVect.at(1), &fJet1, &fJet2, 125.0);
        
        // Kinematic Fit Candidates
        thekH = fJet1 + fJet2;
        thekX = theV.p4() + thekH;
        
        // ########## PART 5: VARIABLES ##########
        if(isZtoMM || isZtoEE) {
            // ---------- Angular ----------
            CosThetaStar = Utilities::ReturnCosThetaStar(theX.p4(), theV.p4());
            CosTheta1    = Utilities::ReturnCosTheta1(theV.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4(), theH.daughter(0)->p4(), theH.daughter(1)->p4());
            CosTheta2    = fabs( Utilities::ReturnCosTheta2(theH.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4(), theH.daughter(0)->p4(), theH.daughter(1)->p4()) );
            Phi          = Utilities::ReturnPhi(theX.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4(), theH.daughter(0)->p4(), theH.daughter(1)->p4());
            Phi1         = Utilities::ReturnPhi1(theX.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4());
        }
    }
    
    
    

    // ---------- Print Summary ----------
    if(Verbose) {
        std::cout << " --- Event n. " << iEvent.id().event() << ", lumi " << iEvent.luminosityBlock() << ", run " << iEvent.id().run() << ", weight " << EventWeight << std::endl;
        std::cout << "number of electrons: " << ElecVect.size() << std::endl;
        for(unsigned int i = 0; i < ElecVect.size(); i++) std::cout << "  electron [" << i << "]\tpt: " << ElecVect[i].pt() << "\teta: " << ElecVect[i].eta() << "\tphi: " << ElecVect[i].phi() << "\tmass: " << ElecVect[i].mass() << "\tcharge: " << ElecVect[i].charge() << std::endl;
        std::cout << "number of muons:     " << MuonVect.size() << std::endl;
        for(unsigned int i = 0; i < MuonVect.size(); i++) std::cout << "  muon     [" << i << "]\tpt: " << MuonVect[i].pt() << "\teta: " << MuonVect[i].eta() << "\tphi: " << MuonVect[i].phi() << "\tmass: " << MuonVect[i].mass() << "\tcharge: " << MuonVect[i].charge() << std::endl;
        std::cout << "number of taus:  " << TauVect.size() << std::endl;
        for(unsigned int i = 0; i < TauVect.size(); i++) std::cout << "  tau  [" << i << "]\tpt: " << TauVect[i].pt() << "\teta: " << TauVect[i].eta() << "\tphi: " << TauVect[i].phi() << std::endl;
        std::cout << "number of photons:  " << PhotonVect.size() << std::endl;
        for(unsigned int i = 0; i < PhotonVect.size(); i++) std::cout << "  photon  [" << i << "]\tpt: " << PhotonVect[i].pt() << "\teta: " << PhotonVect[i].eta() << "\tphi: " << PhotonVect[i].phi() << std::endl;
        std::cout << "number of AK4 jets:  " << JetsVect.size() << std::endl;
        for(unsigned int i = 0; i < JetsVect.size(); i++) std::cout << "  AK4 jet  [" << i << "]\tpt: " << JetsVect[i].pt() << "\teta: " << JetsVect[i].eta() << "\tphi: " << JetsVect[i].phi() << "\tmass: " << JetsVect[i].mass() << std::endl;
        std::cout << "number of AK8 jets:  " << FatJetsVect.size() << std::endl;
        for(unsigned int i = 0; i < FatJetsVect.size(); i++) std::cout << "  AK8 jet  [" << i << "]\tpt: " << FatJetsVect[i].pt() << "\teta: " << FatJetsVect[i].eta() << "\tphi: " << FatJetsVect[i].phi() << "\tmass: " << FatJetsVect[i].mass() << std::endl;
        std::cout << "Missing energy:      " << MET.pt() << std::endl;
        std::cout << "V leptonic mass:     " << theV.mass() << ", generated: " << GenZLepMass << std::endl;
        std::cout << "Z merged hadronic mass:     " << theHMerged.mass() << ", generated: " << GenZHadMass << std::endl;
        std::cout << "X merged candidate mass:    " << theXMerged.mass() << ", generated: " << GenXMass << std::endl;
        std::cout << "Z resolved hadronic mass:     " << theHResolved.mass() << ", generated: " << GenZHadMass << std::endl;
        std::cout << "X resolved candidate mass:    " << theXResolved.mass() << ", generated: " << GenXMass << std::endl;
    }

    
    // ---------- Fill objects ----------
    if(Verbose) std::cout << " - Filling objects" << std::endl;
    if(isZtoEE || isWtoEN) for(unsigned int i = 0; i < Leptons.size() && i < ElecVect.size(); i++) ObjectsFormat::FillElectronType(Leptons[i], &ElecVect[i], isMC);
    else if(isZtoMM || isWtoMN) for(unsigned int i = 0; i < Leptons.size() && i < MuonVect.size(); i++) ObjectsFormat::FillMuonType(Leptons[i], &MuonVect[i], isMC);
    for(unsigned int i = 0; i < Taus.size() && i < TauVect.size(); i++) ObjectsFormat::FillTauType(Taus[i], &TauVect[i], isMC);
    for(unsigned int i = 0; i < Photons.size() && i < PhotonVect.size(); i++) ObjectsFormat::FillPhotonType(Photons[i], &PhotonVect[i], isMC);
    for(unsigned int i = 0; i < Jets.size() && i < JetsVect.size(); i++) ObjectsFormat::FillJetType(Jets[i], &JetsVect[i], isMC);
    for(unsigned int i = 0; i < FatJets.size() && i < FatJetsVect.size(); i++) ObjectsFormat::FillFatJetType(FatJets[i], &FatJetsVect[i], isMC);
    ObjectsFormat::FillMEtType(MEt, &MET, isMC);
    ObjectsFormat::FillCandidateType(V, &theV, isMC);
    ObjectsFormat::FillCandidateType(H, &theH, isMC);
    ObjectsFormat::FillCandidateType(X, &theX, isMC);
    ObjectsFormat::FillCandidateType(HMerged, &theHMerged, isMC);
    ObjectsFormat::FillCandidateType(XMerged, &theXMerged, isMC);
    ObjectsFormat::FillCandidateType(HResolved, &theHResolved, isMC);
    ObjectsFormat::FillCandidateType(XResolved, &theXResolved, isMC);
    ObjectsFormat::FillCandidateType(HResolvedHpt, &theHResolvedHpt, isMC);
    ObjectsFormat::FillCandidateType(XResolvedHpt, &theXResolvedHpt, isMC);
    ObjectsFormat::FillCandidateType(HResolvedDZ, &theHResolvedDZ, isMC);
    ObjectsFormat::FillCandidateType(XResolvedDZ, &theXResolvedDZ, isMC);
    ObjectsFormat::FillCandidateType(HResolvedDR, &theHResolvedDR, isMC);
    ObjectsFormat::FillCandidateType(XResolvedDR, &theXResolvedDR, isMC);
    ObjectsFormat::FillLorentzType(kH, &thekH);
    ObjectsFormat::FillLorentzType(kX, &thekX);
    
    if(nJets < 2 && nFatJets < 1) return;
    
    // Fill tree
    tree->Fill();
    if(Verbose) std::cout << " - Tree filled, end of event" << std::endl;
}

//#ifdef THIS_IS_AN_EVENT_EXAMPLE
//   Handle<ExampleData> pIn;
//   iEvent.getByLabel("example",pIn);
//#endif
//   
//#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
//   ESHandle<SetupData> pSetup;
//   iSetup.get<SetupRecord>().get(pSetup);
//#endif



// ------------ method called once each job just before starting event loop  ------------
void Diboson::beginJob() {
    
    // Object objects are created only one in the begin job. The reference passed to the branch has to be the same
    for(int i = 0; i < WriteNElectrons; i++) Electrons.push_back( LeptonType() );
    for(int i = 0; i < WriteNMuons; i++) Muons.push_back( LeptonType() );
    for(int i = 0; i < WriteNLeptons; i++) Leptons.push_back( LeptonType() );
    for(int i = 0; i < WriteNTaus; i++) Taus.push_back( TauType() );
    for(int i = 0; i < WriteNPhotons; i++) Photons.push_back( PhotonType() );
    for(int i = 0; i < WriteNJets; i++) Jets.push_back( JetType() );
    for(int i = 0; i < WriteNFatJets; i++) FatJets.push_back( FatJetType() );
    
    
    // Create Tree and set Branches
    tree=fs->make<TTree>("tree", "tree");
    tree->Branch("isMC", &isMC, "isMC/O");
    tree->Branch("EventNumber", &EventNumber, "EventNumber/L");
    tree->Branch("LumiNumber", &LumiNumber, "LumiNumber/L");
    tree->Branch("RunNumber", &RunNumber, "RunNumber/L");
    tree->Branch("EventWeight", &EventWeight, "EventWeight/F");
    tree->Branch("PUWeight", &PUWeight, "PUWeight/F");
    tree->Branch("TriggerWeight", &TriggerWeight, "TriggerWeight/F");
    tree->Branch("LeptonWeight", &LeptonWeight, "LeptonWeight/F");
    
    // Set trigger branches
    for(auto it = TriggerMap.begin(); it != TriggerMap.end(); it++) tree->Branch(it->first.c_str(), &(it->second), (it->first+"/O").c_str());
    
    // Analysis variables
    tree->Branch("isZtoEE", &isZtoEE, "isZtoEE/O");
    tree->Branch("isZtoMM", &isZtoMM, "isZtoMM/O");
    tree->Branch("isWtoEN", &isWtoEN, "isWtoEN/O");
    tree->Branch("isWtoMN", &isWtoMN, "isWtoMN/O");
    tree->Branch("isZtoNN", &isZtoNN, "isZtoNN/O");
    tree->Branch("isMerged", &isMerged, "isMerged/O");
    tree->Branch("isResolved", &isResolved, "isResolved/O");
    
    // Objects
    tree->Branch("nPV", &nPV, "nPV/I");
    tree->Branch("nElectrons", &nElectrons, "nElectrons/I");
    tree->Branch("nMuons", &nMuons, "nMuons/I");
    tree->Branch("nTaus", &nTaus, "nTaus/I");
    tree->Branch("nPhotons", &nPhotons, "nPhotons/I");
    tree->Branch("nJets", &nJets, "nJets/I");
    tree->Branch("nFatJets", &nFatJets, "nFatJets/I");
    tree->Branch("nBTagJets", &nBTagJets, "nBTagJets/I");
    
    tree->Branch("Chi2", &Chi2, "Chi2/F");
    // Angular variables
    tree->Branch("CosThetaStar", &CosThetaStar, "CosThetaStar/F");
    tree->Branch("CosTheta1", &CosTheta1, "CosTheta1/F");
    tree->Branch("CosTheta2", &CosTheta2, "CosTheta2/F");
    tree->Branch("Phi", &Phi, "Phi/F");
    tree->Branch("Phi1", &Phi1, "Phi1/F");
  
    // Set Branches for objects
    for(int i = 0; i < WriteNElectrons; i++) tree->Branch(("Electron"+std::to_string(i+1)).c_str(), &(Electrons[i]), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNMuons; i++) tree->Branch(("Muon"+std::to_string(i+1)).c_str(), &(Muons[i]), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNLeptons; i++) tree->Branch(("Lepton"+std::to_string(i+1)).c_str(), &(Leptons[i]), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNTaus; i++) tree->Branch(("Tau"+std::to_string(i+1)).c_str(), &(Taus[i]), ObjectsFormat::ListTauType().c_str());
    for(int i = 0; i < WriteNPhotons; i++) tree->Branch(("Photon"+std::to_string(i+1)).c_str(), &(Photons[i]), ObjectsFormat::ListPhotonType().c_str());
    for(int i = 0; i < WriteNJets; i++) tree->Branch(("Jet"+std::to_string(i+1)).c_str(), &(Jets[i]), ObjectsFormat::ListJetType().c_str());
    for(int i = 0; i < WriteNFatJets; i++) tree->Branch(("FatJet"+std::to_string(i+1)).c_str(), &(FatJets[i]), ObjectsFormat::ListFatJetType().c_str());
    tree->Branch("MEt", &MEt, ObjectsFormat::ListMEtType().c_str());
    tree->Branch("V", &V, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("H", &H, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("X", &X, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HMerged", &HMerged, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XMerged", &XMerged, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolved", &HResolved, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedHpt", &HResolvedHpt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedDZ", &HResolvedDZ, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedDR", &HResolvedDR, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolved", &XResolved, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedHpt", &XResolvedHpt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedDZ", &XResolvedDZ, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedDR", &XResolvedDR, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("kH", &kH, ObjectsFormat::ListLorentzType().c_str());
    tree->Branch("kX", &kX, ObjectsFormat::ListLorentzType().c_str());
}

// ------------ method called once each job just after ending the event loop  ------------
void Diboson::endJob() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Diboson::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(Diboson);