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
    std::vector<std::string> nLabels={"All", "Trigger", "Iso Lep #geq 2", "Z cand ", "Jets #geq 2", "Z mass ", "h mass ", "Top veto", "bJets #geq 1", "bJets #geq 2"};
    
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
    
    EventWeight = StitchWeight = ZewkWeight = WewkWeight = PUWeight = TriggerWeight = LeptonWeight = 1.;
    FacWeightUp = FacWeightDown = RenWeightUp = RenWeightDown = ScaleWeightUp = ScaleWeightDown = 1.;
    isZtoEE = isZtoMM = isTtoEM = isWtoEN = isWtoMN = isZtoNN = false;
    nPV = nElectrons = nMuons = nTaus = nPhotons = nJets = nFatJets = nBTagJets = 1;
    nVetoElectrons = nLooseElectrons = nLooseMuons = nTrackerHighPtMuons = 0;
    MaxJetBTag = MaxFatJetBTag = Chi2 = -1.;
    MinJetMetDPhi = 10.;
    massRecoilFormula = -1.;
    /*
    Lepton1_isMuon = Lepton1_isElectron = Lepton1_isLoose = Lepton1_isHighPt = Lepton1_isTrackerHighPt = Lepton1_isTight = false;
    Lepton1_pt = Lepton1_trkIso = -1.;
    Lepton2_isMuon = Lepton2_isElectron = Lepton2_isLoose = Lepton2_isHighPt = Lepton2_isTrackerHighPt = Lepton2_isTight = false;
    Lepton2_pt = Lepton2_trkIso = -1.;
    MEt_pt = -1.;
    V_pt = V_dPhi = V_mass = V_tmass = -1.;
    X_pt = X_dPhi = X_mass = X_tmass = -1.;
    FatJet1_isTight = false;
    FatJet1_pt = FatJet1_prunedMass = FatJet1_softdropMass = FatJet1_softdropPuppiMass = FatJet1_prunedMassCorr = FatJet1_softdropMassCorr = FatJet1_softdropPuppiMassCorr = FatJet1_chsTau21 = FatJet1_puppiTau21 = FatJet1_ddtTau21 = FatJet1_CSV1 = FatJet1_CSV2 = -1.;
    */
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
    ObjectsFormat::ResetCandidateType(X);
    /*ObjectsFormat::ResetCandidateType(H);
    ObjectsFormat::ResetCandidateType(A);
    ObjectsFormat::ResetCandidateType(HMerged);
    ObjectsFormat::ResetCandidateType(XMerged);
    ObjectsFormat::ResetCandidateType(HResolved);
    ObjectsFormat::ResetCandidateType(XResolved);
    ObjectsFormat::ResetCandidateType(HResolvedPt);
    ObjectsFormat::ResetCandidateType(XResolvedPt);
    ObjectsFormat::ResetCandidateType(HResolvedHpt);
    ObjectsFormat::ResetCandidateType(XResolvedHpt);
    ObjectsFormat::ResetCandidateType(HResolvedDZ);
    ObjectsFormat::ResetCandidateType(XResolvedDZ);
    ObjectsFormat::ResetCandidateType(HResolvedDR);
    ObjectsFormat::ResetCandidateType(XResolvedDR);
    ObjectsFormat::ResetLorentzType(kH);
    ObjectsFormat::ResetLorentzType(kA);*/
    
    Hist["a_nEvents"]->Fill(1., EventWeight);
    Hist["e_nEvents"]->Fill(1., EventWeight);
    Hist["m_nEvents"]->Fill(1., EventWeight);
    
    // -----------------------------------
    //           READ OBJECTS
    // -----------------------------------
    
    // Pu weight
    PUWeight = thePileupAnalyzer->GetPUWeight(iEvent);
    nPV = thePileupAnalyzer->GetPV(iEvent);
    Hist["a_nPVNoWeight"]->Fill(nPV, EventWeight);
    EventWeight *= PUWeight;
    Hist["a_nPVReWeight"]->Fill(nPV, EventWeight);
    
    // Trigger
    theTriggerAnalyzer->FillTriggerMap(iEvent, TriggerMap);
    EventWeight *= TriggerWeight;
    
    // Electrons
    std::vector<pat::Electron> ElecVect = theElectronAnalyzer->FillElectronVector(iEvent);
    nElectrons = ElecVect.size();
    for(unsigned int i =0; i<ElecVect.size(); i++){
        if(ElecVect.at(i).userInt("isVeto")==1) nVetoElectrons++;
        if(ElecVect.at(i).userInt("isLoose")==1) nLooseElectrons++;
    }
    // Muons
    std::vector<pat::Muon> MuonVect = theMuonAnalyzer->FillMuonVector(iEvent);
    nMuons = MuonVect.size();
    for(unsigned int i =0; i<MuonVect.size(); i++){
        if(MuonVect.at(i).userInt("isLoose")==1) nLooseMuons++;
        if(MuonVect.at(i).userInt("isTrackerHighPt")==1) nTrackerHighPtMuons++;
    }
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
    //theFatJetAnalyzer->CleanJetsFromMuons(FatJetsVect, MuonVect, 1.); // Do NOT clean the fatjet now
    //theFatJetAnalyzer->CleanJetsFromElectrons(FatJetsVect, ElecVect, 1.); // Do NOT clean the fatjet now
    nFatJets = FatJetsVect.size();
    // Missing Energy
    pat::MET MET = theJetAnalyzer->FillMetVector(iEvent);
    pat::MET Neutrino(MET);
    //MET trigger efficiency for muons ad electrons
    float metNoMupx = MET.px();
    float metNoMupy = MET.py();
    bool METtrigNoMu = false;
    bool METtrig = false;
    METtrigNoMu = TriggerMap["HLT_PFMETNoMu90_PFMHTNoMu90_IDTight_v"] || TriggerMap["HLT_PFMETNoMu120_PFMHTNoMu120_IDTight_v"];
    METtrig = TriggerMap["HLT_PFMET170_NotCleaned_v"] || TriggerMap["HLT_PFMET170_NoiseCleaned_v"] || TriggerMap["HLT_PFMET170_JetIdCleaned_v"] || TriggerMap["HLT_PFMET170_HBHECleaned_v"] || TriggerMap["HLT_PFMET170_BeamHaloCleaned_v"];
    if(nFatJets>0){
        if(nMuons>0){
          if(TriggerMap["HLT_Mu45_eta2p1_v"] && MuonVect.at(0).userInt("isTight") && MuonVect.at(0).userFloat("pfIso04")<0.15 && MuonVect.at(0).pt()>55){
	      metNoMupx += MuonVect.at(0).px();
	      metNoMupy += MuonVect.at(0).py();
	      reco::Particle::LorentzVector metNoMup4(metNoMupx, metNoMupy, 0, sqrt(pow(metNoMupx,2) + pow(metNoMupy,2)) );
	      MET.addUserFloat("ptNoMu",metNoMup4.pt());
	      MET.addUserFloat("phiNoMu",metNoMup4.phi());
	      Hist["a_mu_den_trig"]->Fill(MET.pt(), EventWeight);
	      Hist["a_mu_den_trigNoMu"]->Fill(MET.userFloat("ptNoMu"), EventWeight);
	      if(METtrigNoMu) Hist["a_mu_num_trigNoMu"]->Fill(MET.userFloat("ptNoMu"), EventWeight);
	      if(METtrig) Hist["a_mu_num_trig"]->Fill(MET.pt(), EventWeight);
          }
        }
        if(nElectrons>0){
          if(TriggerMap["HLT_Ele27_WPLoose_Gsf_v"] && ElecVect.at(0).userInt("isTight") && ElecVect.at(0).pt()>55){
	      Hist["a_ele_den_trig"]->Fill(MET.pt(), EventWeight);
	      if(METtrig || METtrigNoMu) Hist["a_ele_num_trig"]->Fill(MET.pt(), EventWeight);
          }
        }
    }
    //theJetAnalyzer->ApplyRecoilCorrections(MET, &MET.genMET()->p4(), &theV.p4(), 0);
    
    
    // -----------------------------------
    //           GEN LEVEL
    // -----------------------------------
    
    // Gen weights
    std::map<std::string, float> GenWeight = theGenAnalyzer->FillWeightsMap(iEvent);
    EventWeight *= GenWeight["event"];
    if(GenWeight.find("2") != GenWeight.end()) FacWeightUp     = GenWeight["2"];
    if(GenWeight.find("3") != GenWeight.end()) FacWeightDown   = GenWeight["3"];
    if(GenWeight.find("4") != GenWeight.end()) RenWeightUp     = GenWeight["4"];
    if(GenWeight.find("7") != GenWeight.end()) RenWeightDown   = GenWeight["7"];
    if(GenWeight.find("5") != GenWeight.end()) ScaleWeightUp   = GenWeight["5"];
    if(GenWeight.find("9") != GenWeight.end()) ScaleWeightDown = GenWeight["9"];
    // Lhe Particles
    std::map<std::string, float> LheMap = theGenAnalyzer->FillLheMap(iEvent);
    // Mc Stitching
    StitchWeight = theGenAnalyzer->GetStitchWeight(LheMap);
    //EventWeight *= StitchWeight; // Not yet
    // Gen Particles
    std::vector<reco::GenParticle> GenPVect = theGenAnalyzer->FillGenVector(iEvent);
    // Gen candidates
    reco::Candidate* theGenZ = theGenAnalyzer->FindGenParticle(GenPVect, 23);
    reco::Candidate* theGenW = theGenAnalyzer->FindGenParticle(GenPVect, 24);
    // EWK corrections
    if(theGenZ) ZewkWeight = theGenAnalyzer->GetZewkWeight(theGenZ->pt());
    if(theGenW) WewkWeight = theGenAnalyzer->GetWewkWeight(theGenW->pt());
    
//    if(LheMap.find("lhePtZ")!=LheMap.end()) ZewkWeight = theGenAnalyzer->GetZewkWeight(LheMap["lhePtZ"]);
//    if(LheMap.find("lhePtW")!=LheMap.end()) WewkWeight = theGenAnalyzer->GetWewkWeight(LheMap["lhePtW"]);
    
    EventWeight *= ZewkWeight * WewkWeight;
   
    /*
//        float genPtZ(-1.), genPtW(-1.);
//    if(!genZ && genL1 && genL2) genPtZ = (!genZ) ? genZ->pt() : (*genL1 + *genL2).pt();
//    if(!genW && ((genL1 && genN1) || (genL2 && genN2))) genPtW = (!genW) ? genW->pt() : ( (genL1 && genN1) ? (*genL1 + *genN1).pt() : (*genL2 + *genN2).pt() );
//    if(genZ) genPtZ = genZ->pt();
//    if(genW) genPtW = genW->pt();
    
    reco::GenParticle* genL1 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{-11, -13}, 23);
    reco::GenParticle* genL2 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{+11, +13}, 23);
    reco::GenParticle* genQ1 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{5}, 25);
    reco::GenParticle* genQ2 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{-5}, 25);
    reco::GenParticle* genZ = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{23});
    reco::GenParticle* genH = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{25});
    reco::GenParticle* genX = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{36});
    
    if(genL1!=NULL && genL2!=NULL && genQ1!=NULL && genQ2!=NULL && genZ!=NULL && genH!=NULL && genX!=NULL) {
        //reco::Candidate::LorentzVector genV(genL1->p4() + genL2->p4()), genH(genQ1->p4() + genQ2->p4()), genX(genL1->p4() + genL2->p4() + genQ1->p4() + genQ2->p4());
        
        Hist["g_Xmass"]->Fill(genX->mass(), EventWeight);
        Hist["g_Xpt"]->Fill(genX->pt(), EventWeight);
        Hist["g_Zmass"]->Fill(genZ->mass(), EventWeight);
        Hist["g_Zpt"]->Fill(genZ->pt(), EventWeight);
        Hist["g_ZdR"]->Fill(reco::deltaR(genL1->eta(), genL1->phi(), genL2->eta(), genL2->phi()), EventWeight);
        Hist["g_Hmass"]->Fill(genH->mass(), EventWeight);
        Hist["g_Hpt"]->Fill(genH->pt(), EventWeight);
        Hist["g_HdR"]->Fill(reco::deltaR(genQ1->eta(), genQ1->phi(), genQ2->eta(), genQ2->phi()), EventWeight);
        
        float genCosThetaStar = Utilities::ReturnCosThetaStar(genX->p4(), genZ->p4());
        float genCosTheta1    = Utilities::ReturnCosTheta1(genZ->p4(), genL1->p4(), genL2->p4(), genQ1->p4(), genQ2->p4());
        float genCosTheta2    = fabs( Utilities::ReturnCosTheta2(genH->p4(), genL1->p4(), genL2->p4(), genQ1->p4(), genQ2->p4()) );
        float genPhi          = Utilities::ReturnPhi(genX->p4(), genL1->p4(), genL2->p4(), genQ1->p4(), genQ2->p4());
        float genPhi1         = Utilities::ReturnPhi1(genX->p4(), genL1->p4(), genL2->p4());
        
        Hist["g_CosThetaStar"]->Fill(genCosThetaStar, EventWeight);
        Hist["g_CosTheta1"]->Fill(genCosTheta1, EventWeight);
        Hist["g_CosTheta2"]->Fill(genCosTheta2, EventWeight);
        Hist["g_Phi"]->Fill(genPhi, EventWeight);
        Hist["g_Phi1"]->Fill(genPhi1, EventWeight);
        
    }
    */

    std::vector<int> LepIds = {12,14,16,-12,-14,-16};
    std::vector<int> HadIds = {1,2,3,4,5,-1,-2,-3,-4,-5};
    reco::GenParticle* theGenLep = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, LepIds);
    reco::GenParticle* theGenHad = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, HadIds);
    
    //Gen level plots and candidates
    //double GenZLepMass = -1.;
    //double GenZHadMass = -1.;
    //double GenZHadPt = -1.;
    //double GenXMass = -1.;
    double GenHadDR = -9.;
    double GenLepDR = -9.;
    //double GenZHadFatJetDR = -9.;
    bool isGenZZ = false;
    reco::Particle::LorentzVector p4GenZHad;
    if(theGenLep!=NULL && theGenHad!=NULL){
        const reco::Candidate* theGenZLep = theGenAnalyzer->FindMother(theGenLep);
        const reco::Candidate* theGenZHad = theGenAnalyzer->FindMother(theGenHad);
        for(unsigned int a = 0; a<=theGenZHad->numberOfDaughters(); a++) {
            if(theGenZHad!=NULL && theGenZHad->daughter(a)!=NULL && (theGenZHad->pdgId()==23 || theGenZHad->pdgId()==25) && (theGenZHad->daughter(a)->pdgId() == - theGenHad->pdgId())){
                GenHadDR = reco::deltaR(theGenHad->eta(),theGenHad->phi(),theGenZHad->daughter(a)->eta(),theGenZHad->daughter(a)->phi());
                break;
            }
        }
        for(unsigned int b = 0; b<=theGenZLep->numberOfDaughters(); b++) {
            if(theGenZLep!=NULL && theGenZLep->daughter(b)!=NULL && (theGenZLep->pdgId()==23 || theGenZLep->pdgId()==25) && (theGenZLep->daughter(b)->pdgId() == - theGenLep->pdgId())){
                GenLepDR = reco::deltaR(theGenLep->eta(),theGenLep->phi(),theGenZLep->daughter(b)->eta(),theGenZLep->daughter(b)->phi());
                break;
            }
        }
        if(theGenZLep!=NULL && theGenZLep->pdgId()==23 && theGenZHad!=NULL && (theGenZHad->pdgId()==23 || theGenZHad->pdgId()==25)) {
            Hist["g_ZLepMass"]->Fill(theGenZLep->mass(), EventWeight);
            Hist["g_ZLepPt"]->Fill(theGenZLep->pt(), EventWeight);
            Hist["g_LepPt"]->Fill(theGenLep->pt(), EventWeight);
            Hist["g_ZHadMass"]->Fill(theGenZHad->mass(), EventWeight);
            Hist["g_ZHadPt"]->Fill(theGenZHad->pt(), EventWeight);
            Hist["g_HadPt"]->Fill(theGenHad->pt(), EventWeight);
            Hist["g_HadEta"]->Fill(theGenHad->eta(), EventWeight);
            Hist["g_HadDR"]->Fill(GenHadDR, EventWeight);
            Hist["g_LepDR"]->Fill(GenLepDR, EventWeight);
            Hist["g_LepEta"]->Fill(theGenLep->eta(), EventWeight);
            Hist["g_ZZDR"]->Fill(reco::deltaR(theGenZHad->eta(),theGenZHad->phi(),theGenZLep->eta(),theGenZLep->phi()), EventWeight);
            Hist["g_ZZDPhi"]->Fill(reco::deltaPhi(theGenZHad->phi(),theGenZLep->phi()), EventWeight);
            Hist["g_LepHadDR"]->Fill(reco::deltaR(theGenHad->eta(),theGenHad->phi(),theGenLep->eta(),theGenLep->phi()), EventWeight);
            //GenZLepMass = theGenZLep->mass();
            //GenZHadMass = theGenZHad->mass();
            //GenZHadPt = theGenZHad->pt();
            isGenZZ = true;
            p4GenZHad = theGenZHad->p4();
        }
    }
    
    
    reco::Candidate* theGenX = theGenAnalyzer->FindGenParticleByIdAndStatus(GenPVect, 39, 62);
    if(!theGenX) theGenX = theGenAnalyzer->FindGenParticleByIdAndStatus(GenPVect, 36, 62);
    if(theGenX!=NULL && theGenLep!=NULL && theGenHad!=NULL && isGenZZ){
        Hist["g_XMass"]->Fill(theGenX->mass(), EventWeight);
        Hist["g_XMT"]->Fill(theGenX->mt(), EventWeight);
        Hist["g_XPt"]->Fill(theGenX->pt(), EventWeight);
        Hist["g_XRapidity"]->Fill(theGenX->rapidity(), EventWeight);
        //GenXMass = theGenX->mass();
    }
    

    // ---------- Trigger selections ----------
    // Dummy trigger
    //TriggerWeight*=theElectronAnalyzer->GetDoubleElectronTriggerSF(ElecVect.at(0), ElecVect.at(1));
    //TriggerWeight*=theMuonAnalyzer->GetDoubleMuonTriggerSF(MuonVect.at(0), MuonVect.at(1));
    
    
    
    Hist["a_nEvents"]->Fill(2., EventWeight);
    Hist["e_nEvents"]->Fill(2., EventWeight);
    Hist["m_nEvents"]->Fill(2., EventWeight);
    
    
    // Electron efficiency plots
    reco::GenParticle* genE1 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{-11});
    reco::GenParticle* genE2 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{+11});
    if(genE1!=NULL && genE2!=NULL) {
        int e1(-1), e2(-1);
        float dRll(-1.);
        dRll = reco::deltaR(genE1->eta(), genE1->phi(), genE2->eta(), genE2->phi());
        //pTll = (genE1->p4() + genE2->p4()).pt();
        
        for(unsigned int e = 0; e < ElecVect.size(); e++) {
            if(reco::deltaR(genE1->eta(), genE1->phi(), ElecVect[e].eta(), ElecVect[e].phi()) < 0.1 && fabs(1.-ElecVect[e].pt()/genE1->pt()) < 0.3) e1 = e;
            else if(((int)e)!=e1 && reco::deltaR(genE2->eta(), genE2->phi(), ElecVect[e].eta(), ElecVect[e].phi()) < 0.1 && fabs(1.-ElecVect[e].pt()/genE2->pt()) < 0.3) e2 = e;
        }
        if(e1 >= 0 && e2 >= 0) {
            Hist["e_nEvents"]->Fill(3., EventWeight);
            Hist["e_dR_reco"]->Fill(dRll);
            if(ElecVect[e1].pt() > 55. && ElecVect[e2].pt() > 20.) {
                Hist["e_nEvents"]->Fill(4., EventWeight);
                Hist["e_dR_pt"]->Fill(dRll);
                if(ElecVect[e1].charge() != ElecVect[e2].charge() && (ElecVect[e1].p4() + ElecVect[e2].p4()).mass() > 70 && (ElecVect[e1].p4() + ElecVect[e2].p4()).mass() < 110) {
                    Hist["e_nEvents"]->Fill(5., EventWeight);
                    Hist["e_dR_Z"]->Fill(dRll);
                    Hist["e_dR"]->Fill(dRll);
                    if(ElecVect[e1].userInt("isLoose") == 1 || ElecVect[e2].userInt("isLoose") == 1) {
                        Hist["e_nEvents"]->Fill(6., EventWeight);
                        Hist["e_dR_LooseId"]->Fill(dRll);
                        if(ElecVect[e1].userInt("isLoose") == 1 && ElecVect[e2].userInt("isLoose") == 1) {
                            Hist["e_nEvents"]->Fill(7., EventWeight);
                            Hist["e_dR_LooseLooseId"]->Fill(dRll);
                        }
                    }
                    if(ElecVect[e1].userInt("isVeto") == 1 && ElecVect[e2].userInt("isVeto") == 1) Hist["e_dR_VetoVetoId"]->Fill(dRll);
                    if(ElecVect[e1].userInt("isMedium") == 1 && ElecVect[e2].userInt("isMedium") == 1) Hist["e_dR_MediumMediumId"]->Fill(dRll);
                    if(ElecVect[e1].userInt("isTight") == 1 && ElecVect[e2].userInt("isTight") == 1) Hist["e_dR_TightTightId"]->Fill(dRll);
                }
            }
        }
    }
    
    
    // Muon efficiency plots
    reco::GenParticle* genM1 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{-13});
    reco::GenParticle* genM2 = theGenAnalyzer->FindGenParticleGenByIds(GenPVect, std::vector<int>{+13});
    if(genM1!=NULL && genM2!=NULL) {
        int m1(-1), m2(-1);
        float dRll(-1.);
        dRll = reco::deltaR(genM1->eta(), genM1->phi(), genM2->eta(), genM2->phi());
        //pTll = (genM1->p4() + genM2->p4()).pt();
        
        for(unsigned int m = 0; m < MuonVect.size(); m++) {
            if(reco::deltaR(genM1->eta(), genM1->phi(), MuonVect[m].eta(), MuonVect[m].phi()) < 0.1 && fabs(1.-MuonVect[m].pt()/genM1->pt()) < 0.3) m1 = m;
            else if(((int)m)!=m1 && reco::deltaR(genM2->eta(), genM2->phi(), MuonVect[m].eta(), MuonVect[m].phi()) < 0.1 && fabs(1.-MuonVect[m].pt()/genM2->pt()) < 0.3) m2 = m;
        }
        if(m1 >= 0 && m2 >= 0) {
            Hist["m_nEvents"]->Fill(3., EventWeight);
            Hist["m_dR_reco"]->Fill(dRll);
            if(MuonVect[m1].pt() > 55. && MuonVect[m2].pt() > 20.) {
                Hist["m_nEvents"]->Fill(4., EventWeight);
                Hist["m_dR_pt"]->Fill(dRll);
                if(MuonVect[m1].charge() != MuonVect[m2].charge() && (MuonVect[m1].p4() + MuonVect[m2].p4()).mass() > 70 && (MuonVect[m1].p4() + MuonVect[m2].p4()).mass() < 110) {
                    Hist["m_nEvents"]->Fill(5., EventWeight);
                    Hist["m_dR_Z"]->Fill(dRll);
                    Hist["m_dR"]->Fill(dRll);
                    if(MuonVect[m1].userInt("isHighPt") == 1 || MuonVect[m2].userInt("isHighPt") == 1) {
                        Hist["m_nEvents"]->Fill(6., EventWeight);
                        Hist["m_dR_HighptId"]->Fill(dRll);
                        if((MuonVect[m1].userInt("isHighPt")==1 && MuonVect[m2].userInt("isTrackerHighPt")==1) || (MuonVect[m2].userInt("isHighPt")==1 && MuonVect[m1].userInt("isTrackerHighPt")==1)) {
                            Hist["m_nEvents"]->Fill(7., EventWeight);
                            Hist["m_dR_HighptTrackerId"]->Fill(dRll);
                            if(MuonVect[m1].userFloat("trkIso") < 0.1 && MuonVect[m2].userFloat("trkIso") < 0.1) {
                                Hist["m_nEvents"]->Fill(8., EventWeight);
                                Hist["m_dR_HighptTrackerIdTrackerIso"]->Fill(dRll);
                            }
                        }
                    }
                    if(MuonVect[m1].userInt("isHighPt") == 1 && MuonVect[m2].userInt("isHighPt") == 1) Hist["m_dR_HighptHighptId"]->Fill(dRll);
                    if(MuonVect[m1].userInt("isTight") == 1 && MuonVect[m2].userInt("isTight") == 1) Hist["m_dR_TightTightId"]->Fill(dRll);
                    if(MuonVect[m1].userInt("isMedium") == 1 && MuonVect[m2].userInt("isMedium") == 1) Hist["m_dR_MediumMediumId"]->Fill(dRll);
                    if(MuonVect[m1].userInt("isLoose") == 1 && MuonVect[m2].userInt("isLoose") == 1) Hist["m_dR_LooseLooseId"]->Fill(dRll);
                }
            }
        }
    }
    
    
    // -----------------------------------
    //           VECTOR BOSON
    // -----------------------------------
    
    // Categorization depending on the number of leptons
    
    // ---------- Z TO LEPTONS ----------
    /*
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
        if(MuonVect.size()==1 && ElecVect.size()==1) isTtoEM = true;
        else if(ElecVect.size()==1) isWtoEN=true;
        else if(MuonVect.size()==1) isWtoMN=true;
        else {if(Verbose) std::cout << " - No Iso Leptons" << std::endl;}
    }
      
    // ----------- Z TO NEUTRINOS ---------------
    if(MET.pt() > 100.) {
        isZtoNN=true;
        if(Verbose) std::cout << " - Possible Z inv candidate" << std::endl;
    }

    
    else {if(Verbose) std::cout << " - No V candidate" << std::endl; return;}
    if(isWtoEN || isWtoMN) {if(Verbose) std::cout << " - W->lnu candidate" << std::endl; return;}
    if(not(isZtoEE || isZtoMM || isZtoNN || isWtoEN || isWtoMN)) {if(Verbose) std::cout << " - No V candidate" << std::endl; return;}    
    */
    Hist["a_nEvents"]->Fill(3., EventWeight);
    Hist["m_nEvents"]->Fill(8., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(3., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(3., EventWeight);


    // ---------- Reconstruct V candidate ----------
    
    pat::CompositeCandidate theV;
    /*
    if(isZtoMM) {
        if(Verbose) std::cout << " - Try to reconstruct Z -> mm" << std::endl;
        if(MuonVect.at(0).charge()*MuonVect.at(1).charge()<0 && (MuonVect[0].p4() + MuonVect[1].p4()).mass() > 50.) {
            theV.addDaughter(MuonVect.at(0).charge() < 0 ? MuonVect.at(0) : MuonVect.at(1));
            theV.addDaughter(MuonVect.at(0).charge() < 0 ? MuonVect.at(1) : MuonVect.at(0));
            addP4.set(theV);
            // SF
            if(isMC) {
                LeptonWeight *= theMuonAnalyzer->GetMuonTrkSF(MuonVect.at(0));
                LeptonWeight *= theMuonAnalyzer->GetMuonTrkSF(MuonVect.at(1));
                LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1id"));
                LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(1), MuonPSet.getParameter<int>("muon2id"));
                LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1iso"));
                LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(1), MuonPSet.getParameter<int>("muon2iso"));
            }
        }
        else { if(Verbose) std::cout << " - No OS muons" << std::endl; return; }
    }
    else if(isZtoEE) {
        if(Verbose) std::cout << " - Try to reconstruct Z -> ee" << std::endl;
        if(ElecVect.at(0).charge()*ElecVect.at(1).charge()<0 && (ElecVect[0].p4() + ElecVect[1].p4()).mass() > 50.) {
            theV.addDaughter(ElecVect.at(0).charge() ? ElecVect.at(0) : ElecVect.at(1));
            theV.addDaughter(ElecVect.at(0).charge() ? ElecVect.at(1) : ElecVect.at(0));
            addP4.set(theV);
            // SF
            if(isMC) {
                LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(0), ElectronPSet.getParameter<int>("electron1id"));
                LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(1), ElectronPSet.getParameter<int>("electron2id"));
                LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(0));
                LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(1));
            }
        }
        else { if(Verbose) std::cout << " - No OS electrons" << std::endl; return; }
    }*/
    float maxZpt(-1.);
    if(MuonVect.size()>=2) {
        if(Verbose) std::cout << " - Try to reconstruct Z -> mm" << std::endl;
        // Indentify leptons
        int m1(-1), m2(-1);
        //float maxZpt(-1.);
	//std::cout << "max Zpt before mu cicle " << maxZpt << std::endl;
        for(unsigned int i = 0; i < MuonVect.size(); i++) {
            for(unsigned int j = 1; j < MuonVect.size(); j++) {
                if(i==j || MuonVect[i].charge() == MuonVect[j].charge()) continue;
                float Zpt = (MuonVect[i].p4() + MuonVect[j].p4()).pt();
		//std::cout << " Zpt: " << Zpt << "with muons " << i << " and " << j << std::endl;
                float Zmass = (MuonVect[i].p4() + MuonVect[j].p4()).mass();
		//std::cout << " Zmass: " << Zmass << "with muons " << i << " and " << j<< std::endl;
                if(Zmass > 70. && Zmass < 110. && Zpt > maxZpt) {m1 = i; m2 = j; maxZpt = Zpt; isZtoMM = true;}//std::cout << "a good z in muons" << std::endl; std::cout << "new max z pt " << maxZpt << std::endl;}
            }
        }
        // Build candidate
        if(m1 >= 0 && m2 >= 0 && isZtoMM) {
	    theV.addDaughter(MuonVect.at(m1).charge() < 0 ? MuonVect.at(m1) : MuonVect.at(m2));
            theV.addDaughter(MuonVect.at(m1).charge() < 0 ? MuonVect.at(m2) : MuonVect.at(m1));
            addP4.set(theV);
            // SF
            if(isMC) {                
                /// FIXME -> APPLYING THE SF FOR Mu45eta2p1 HADRCODED <- FIXME ///
                if (MuonVect.at(m1).pt() > MuonVect.at(m2).pt() )
                    LeptonWeight *= theMuonAnalyzer->GetMuonTriggerSFMu45eta2p1(MuonVect.at(m1));
                else
                    LeptonWeight *= theMuonAnalyzer->GetMuonTriggerSFMu45eta2p1(MuonVect.at(m2));
                LeptonWeight *= theMuonAnalyzer->GetMuonTrkSF(MuonVect.at(m1));
                LeptonWeight *= theMuonAnalyzer->GetMuonTrkSF(MuonVect.at(m2));
                LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(m1), MuonPSet.getParameter<int>("muon1id"));
                LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(m2), MuonPSet.getParameter<int>("muon2id"));
                LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(m1), MuonPSet.getParameter<int>("muon1iso"));
                LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(m2), MuonPSet.getParameter<int>("muon2iso"));
            }
        //else { if(Verbose) std::cout << " - No OS muons" << std::endl; return; }
        // Clean-up muon collection
        pat::Muon Mu1 = MuonVect.at(m1), Mu2 = MuonVect.at(m2);
        MuonVect.clear();
        if(Mu1.pt() > Mu2.pt()) {MuonVect.push_back(Mu1); MuonVect.push_back(Mu2);}
        else {MuonVect.push_back(Mu2); MuonVect.push_back(Mu1);}
	isZtoMM = true;
        }
    }
    if(ElecVect.size()>=2) {
        if(Verbose) std::cout << " - Try to reconstruct Z -> ee" << std::endl;
        // Indentify leptons
        int e1(-1), e2(-1);
        //float maxZpt(-1.);
	//std::cout << "max Zpt before ele cicle " << maxZpt << std::endl;
        for(unsigned int i = 0; i < ElecVect.size(); i++) {
            for(unsigned int j = 1; j < ElecVect.size(); j++) {
                if(i==j || ElecVect[i].charge() == ElecVect[j].charge()) continue;
                float Zpt = (ElecVect[i].p4() + ElecVect[j].p4()).pt();
		//std::cout << " Zpt: " << Zpt << "with eles " << i << " and " << j << std::endl;
                float Zmass = (ElecVect[i].p4() + ElecVect[j].p4()).mass();
		//std::cout << " Zmass: " << Zmass << "with eles " << i << " and " << j << std::endl;
                if(Zmass > 70. && Zmass < 110. && Zpt > maxZpt) {e1 = i; e2 = j; maxZpt = Zpt; isZtoEE = true; isZtoMM = false;}// std::cout << "a good z in el" << std::endl; std::cout << "new max z pt " << maxZpt << std::endl;}
            }
        }
        // Build candidate
        if(e1 >= 0 && e2 >= 0 && isZtoEE && not(isZtoMM)) {
            if(theV.numberOfDaughters()>0) theV.clearDaughters();
            theV.addDaughter(ElecVect.at(e1).charge() ? ElecVect.at(e1) : ElecVect.at(e2));
            theV.addDaughter(ElecVect.at(e1).charge() ? ElecVect.at(e2) : ElecVect.at(e1));
            addP4.set(theV);
            // SF
            if(isMC) {
                /// FIXME -> APPLYING THE SF FOR Ele105 HADRCODED <- FIXME ///
                if (ElecVect.at(e1).pt() > ElecVect.at(e2).pt() )
                    LeptonWeight *= theElectronAnalyzer->GetElectronTriggerSFEle105(ElecVect.at(e1));
                else
                    LeptonWeight *= theElectronAnalyzer->GetElectronTriggerSFEle105(ElecVect.at(e2));                
                LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(0), ElectronPSet.getParameter<int>("electron1id"));
                LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(1), ElectronPSet.getParameter<int>("electron2id"));
                LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(0));
                LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(1));
            }
        //else { if(Verbose) std::cout << " - No OS electrons" << std::endl; return; }
        // Clean-up electron collection
        pat::Electron Ele1 = ElecVect.at(e1), Ele2 = ElecVect.at(e2);
        ElecVect.clear();
        if(Ele1.pt() > Ele2.pt()) {ElecVect.push_back(Ele1); ElecVect.push_back(Ele2);}
        else {ElecVect.push_back(Ele2); ElecVect.push_back(Ele1);}
        isZtoEE = true;
        isZtoMM = false;
        }
    }
    /*
    else if(isTtoEM) {
        if(Verbose) std::cout << " - Try to reconstruct TT -> enmn" << std::endl;
        theV.addDaughter(MuonVect.at(0));
        theV.addDaughter(ElecVect.at(0));
        addP4.set(theV);
    }
    else if(isWtoMN) {
        if(Verbose) std::cout << " - Try to reconstruct W -> mn" << std::endl;
        // W kinematic reconstruction
        float pz = Utilities::RecoverNeutrinoPz(&MuonVect.at(0).p4(), &MET.p4());
        Neutrino.setP4(reco::Particle::LorentzVector(MET.px(), MET.py(), pz, sqrt(MET.pt()*MET.pt() + pz*pz) ));
        theV.addDaughter(MuonVect.at(0));
        theV.addDaughter(Neutrino);
        addP4.set(theV);
        // SF
        if(isMC) {
            LeptonWeight *= theMuonAnalyzer->GetMuonTrkSF(MuonVect.at(0));
            LeptonWeight *= theMuonAnalyzer->GetMuonIdSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1id"));
            LeptonWeight *= theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(0), MuonPSet.getParameter<int>("muon1iso"));
        }
    }
    else if(isWtoEN) {
        if(Verbose) std::cout << " - Try to reconstruct W -> em" << std::endl;
        // W kinematic reconstruction
        float pz = Utilities::RecoverNeutrinoPz(&ElecVect.at(0).p4(), &MET.p4());
        Neutrino.setP4(reco::Particle::LorentzVector(MET.px(), MET.py(), pz, sqrt(MET.pt()*MET.pt() + pz*pz) ));
        theV.addDaughter(ElecVect.at(0));
        theV.addDaughter(Neutrino);
        addP4.set(theV);
        // SF
        if(isMC) {
            LeptonWeight *= theElectronAnalyzer->GetElectronIdSF(ElecVect.at(0), ElectronPSet.getParameter<int>("electron1id"));
            LeptonWeight *= theElectronAnalyzer->GetElectronRecoEffSF(ElecVect.at(0));
        }
    }
    */
    if(not(isZtoEE) && not(isZtoMM)) {
        if(MET.pt() > 100.) {
            isZtoNN=true;
            //if(Verbose) std::cout << " - Possible Z inv candidate" << std::endl;    
            if(Verbose) std::cout << " - Try to reconstruct Z -> nn" << std::endl;
            if(theV.numberOfDaughters()>0) theV.clearDaughters();
            theV.addDaughter(MET);
            addP4.set(theV);
        }
    }
    
    if(not(isZtoNN) && not(isZtoEE) && not(isZtoMM)) { if(Verbose) std::cout << " - No reconstructible V candidate" << std::endl; return; }
    // Update event weight with lepton selections
    EventWeight *= LeptonWeight;
    
    Hist["a_nEvents"]->Fill(4., EventWeight);
    Hist["m_nEvents"]->Fill(9., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(4., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(4., EventWeight);
    
    if(isZtoEE) {
        Hist["e_Zmass"]->Fill(theV.mass(), EventWeight);
        if(ElecVect.at(0).isEB() && ElecVect.at(1).isEB()) Hist["e_ZmassBB"]->Fill(theV.mass(), EventWeight);
        if(ElecVect.at(0).isEE() && ElecVect.at(1).isEB()) Hist["e_ZmassEB"]->Fill(theV.mass(), EventWeight);
        if(ElecVect.at(0).isEB() && ElecVect.at(1).isEE()) Hist["e_ZmassBE"]->Fill(theV.mass(), EventWeight);
        if(ElecVect.at(0).isEE() && ElecVect.at(1).isEE()) Hist["e_ZmassEE"]->Fill(theV.mass(), EventWeight);
    }
    if(isZtoMM) {
        Hist["m_Zmass"]->Fill(theV.mass(), EventWeight);
        if(abs(MuonVect.at(0).eta())<1.1 && abs(MuonVect.at(1).eta())<1.1) Hist["m_ZmassBB"]->Fill(theV.mass(), EventWeight);
        if(abs(MuonVect.at(0).eta())>1.1 && abs(MuonVect.at(1).eta())<1.1) Hist["m_ZmassEB"]->Fill(theV.mass(), EventWeight);
        if(abs(MuonVect.at(0).eta())<1.1 && abs(MuonVect.at(1).eta())>1.1) Hist["m_ZmassBE"]->Fill(theV.mass(), EventWeight);
        if(abs(MuonVect.at(0).eta())>1.1 && abs(MuonVect.at(1).eta())>1.1) Hist["m_ZmassEE"]->Fill(theV.mass(), EventWeight);
    }

    // -----------------------------------
    //           HADRONIC BOSON
    // -----------------------------------
    
    /*
    // ---------- Z TO HADRONS ----------
    pat::CompositeCandidate theHMerged;
    pat::CompositeCandidate theHResolved;
    pat::CompositeCandidate theHResolvedHpt;
    pat::CompositeCandidate theHResolvedDZ;
    pat::CompositeCandidate theHResolvedDR;
    pat::CompositeCandidate theHResolvedPt;
    
    isMerged = isResolved = false;
    
    /////////////////// Highest pT method ////////////////////  
    // Resolved topology
    if(nJets < 2) {if(Verbose) std::cout << " - N jets < 2" << std::endl;} // return;}
    else {
        theHResolvedPt.addDaughter(JetsVect.at(0));
        theHResolvedPt.addDaughter(JetsVect.at(1));
        addP4.set(theHResolvedPt);
        //std::cout << "resolved mass: " << theHResolvedPt.mass() << std::endl;
        //if(theHResolvedPt.mass()<40 || theHResolvedPt.pt()<100) theHResolvedPt.clearDaughters();
        //Hist["a_HAK4Mass_HPt"]->Fill(theHResolvedPt.mass(), EventWeight);
    }

    // Boosted topology
    if(nFatJets < 1) {if(Verbose) std::cout << " - N fat jets < 1" << std::endl;}
    else {
        //Hist["a_HAK8Mass_HPt"]->Fill(FatJetsVect.at(0).hasUserFloat("ak8PFJetsCHSSoftDropMass")?FatJetsVect.at(0).userFloat("ak8PFJetsCHSSoftDropMass"):FatJetsVect.at(0).mass(), EventWeight);
        //std::cout << "merged mass: " << FatJetsVect.at(0).mass() << std::endl;
        if(theHResolvedPt.pt()<FatJetsVect.at(0).pt()) {
            theHResolvedPt.clearDaughters();
            theHResolvedPt.addDaughter(FatJetsVect.at(0));
            addP4.set(theHResolvedPt);
            //theH.addUserFloat("softdropMass", FatJetsVect.at(0).userFloat("ak8PFJetsCHSSoftDropMass"));
            //if(theH.mass()>180) theH.clearDaughters();
        }
    }
    Hist["a_HMass_HPt"]->Fill(theHResolvedPt.hasUserFloat("softdropMass") ? theHResolvedPt.userFloat("softdropMass") : theHResolvedPt.mass(), EventWeight);
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
            Hist["a_den_H_truth_XMass"]->Fill(GenXMass, EventWeight);
        }

	      //MC truth merged topology
	      if(isMC && isGenZZ && isMerged) {
            GenZHadFatJetDR = reco::deltaR(p4GenZHad.eta(),p4GenZHad.phi(),FatJetsVect.at(0).eta(),FatJetsVect.at(0).phi());
            Hist["a_genZHad_recoFatJetDR"]->Fill(GenZHadFatJetDR, EventWeight);
            if(GenZHadFatJetDR<0.4){
	              Hist["a_num_HM_truth_Hpt0p4"]->Fill(GenZHadPt, EventWeight);
	              Hist["a_num_HM_truth_HadDR0p4"]->Fill(GenHadDR, EventWeight);
	              Hist["a_num_HM_truth_XMass0p4"]->Fill(GenXMass, EventWeight);
            }
            if(GenZHadFatJetDR<0.1){
	              Hist["a_num_HM_truth_Hpt0p1"]->Fill(GenZHadPt, EventWeight);
	              Hist["a_num_HM_truth_HadDR0p1"]->Fill(GenHadDR, EventWeight);
	              Hist["a_num_HM_truth_XMass0p1"]->Fill(GenXMass, EventWeight);
            }
        }

        //MC truth leading ak4 jets 
        if(isMC && JetsVect.at(0).genParton()!=NULL && JetsVect.at(1).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(0).genParton())==23 && Utilities::FindMotherId(JetsVect.at(1).genParton())==23){
                Hist["a_num_H_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
                Hist["a_num_H_truth_HadDR"]->Fill(GenHadDR, EventWeight);
                Hist["a_num_H_truth_XMass"]->Fill(GenXMass, EventWeight);
            }
        }

        //MC truth highest pT di-jet
        if(isMC && (ch1<=JetsVect.size() && ch2<=JetsVect.size())  && JetsVect.at(ch1).genParton()!=NULL && JetsVect.at(ch2).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(ch1).genParton())==23 && Utilities::FindMotherId(JetsVect.at(ch2).genParton())==23){
	        Hist["a_num_HHpt_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
	        Hist["a_num_HHpt_truth_HadDR"]->Fill(GenHadDR, EventWeight);
	        Hist["a_num_HHpt_truth_XMass"]->Fill(GenXMass, EventWeight);
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
        //MC truth closest to mZ jets
        if(isMC && (ch1<=JetsVect.size() && ch2<=JetsVect.size()) && JetsVect.at(ch1).genParton()!=NULL && JetsVect.at(ch2).genParton()!=NULL && isGenZZ){
            if(Utilities::FindMotherId(JetsVect.at(ch1).genParton())==23 && Utilities::FindMotherId(JetsVect.at(ch2).genParton())==23){
	        Hist["a_num_HDZ_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
	        Hist["a_num_HDZ_truth_HadDR"]->Fill(GenHadDR, EventWeight);
	        Hist["a_num_HDZ_truth_XMass"]->Fill(GenXMass, EventWeight);
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

        //MC truth closest in DR          
        if(isMC && (ch1<=JetsVect.size() && ch2<=JetsVect.size()) && JetsVect.at(ch1).genParton()!=NULL && JetsVect.at(ch2).genParton()!=NULL && isGenZZ) {
            if(Utilities::FindMotherId(JetsVect.at(ch1).genParton())==23 && Utilities::FindMotherId(JetsVect.at(ch2).genParton())==23) {
                Hist["a_num_HDR_truth_Hpt"]->Fill(GenZHadPt, EventWeight);
                Hist["a_num_HDR_truth_HadDR"]->Fill(GenHadDR, EventWeight);
                Hist["a_num_HDR_truth_XMass"]->Fill(GenXMass, EventWeight);
            }
        }
        
       //  
    }
    

    // Global candidate
    pat::CompositeCandidate theXMerged;
    pat::CompositeCandidate theXResolved;
    pat::CompositeCandidate theXResolvedPt;
    pat::CompositeCandidate theXResolvedHpt;
    pat::CompositeCandidate theXResolvedDZ;
    pat::CompositeCandidate theXResolvedDR;
    
    theXMerged.addDaughter(theV);
    if(theHMerged.numberOfDaughters()>0) theXMerged.addDaughter(theHMerged);
    addP4.set(theXMerged);
    
    theXResolved.addDaughter(theV);
    if(theHResolved.numberOfDaughters()>0) theXResolved.addDaughter(theHResolved);
    addP4.set(theXResolved);
    
    theXResolvedPt.addDaughter(theV);
    if(theHResolvedPt.numberOfDaughters()>0) theXResolvedPt.addDaughter(theHResolvedPt);
    addP4.set(theXResolvedPt);
    
    theXResolvedHpt.addDaughter(theV);
    if(theHResolvedHpt.numberOfDaughters()>0) theXResolvedHpt.addDaughter(theHResolvedHpt);
    addP4.set(theXResolvedHpt);
    
    theXResolvedDZ.addDaughter(theV);
    if(theHResolvedDZ.numberOfDaughters()>0) theXResolvedDZ.addDaughter(theHResolvedDZ);
    addP4.set(theXResolvedDZ);

    theXResolvedDR.addDaughter(theV);
    if(theHResolvedDR.numberOfDaughters()>0) theXResolvedDR.addDaughter(theHResolvedDR);
    addP4.set(theXResolvedDR);

//    if(isResolved || isMerged) {
//        Hist["a_nEvents"]->Fill(6., EventWeight);
//        if(isZtoMM) Hist["m_nEvents"]->Fill(6., EventWeight);
//        if(isZtoEE) Hist["e_nEvents"]->Fill(6., EventWeight);
//    }
//    else {
//        if(Verbose) std::cout << "No dilepton, no X!" << std::endl;
//        return;
//    }

    
//    else if(theH.numberOfDaughters()>0){//if is Z to nu nu: apply recoil mass formula
//        theX = recoilMassFormula(theH,MET);
//    }

//    else{//no H, no X
//        if(Verbose) std::cout << "No H, no X" << std::endl;
//        return;
//    }
    */
    
    
    // ------------------------------
    // ----------    AZh   ----------
    // ------------------------------
    /*
    pat::CompositeCandidate theX;
    pat::CompositeCandidate theH;
    pat::CompositeCandidate theA;
    
    reco::Candidate::LorentzVector thekH;
    reco::Candidate::LorentzVector thekA;
    if(nJets >= 2) {
        theH.addDaughter(JetsVect.at(0));
        theH.addDaughter(JetsVect.at(1));
        addP4.set(theH);
        
        theA.addDaughter(theV);
        theA.addDaughter(theH);
        addP4.set(theA);
        
        // ----------- KINEMATIC FIT -----------
        reco::Candidate::LorentzVector fJet1 = JetsVect.at(0).p4();
        reco::Candidate::LorentzVector fJet2 = JetsVect.at(1).p4();
        if(Verbose) std::cout << "Performing kinematic fit..." << std::endl;
        Chi2 = Utilities::PerformKinematicFit(&JetsVect.at(0), &JetsVect.at(1), &fJet1, &fJet2, 125.0);
        
        // Kinematic Fit Candidates
        thekH = fJet1 + fJet2;
        thekA = theV.p4() + thekH;
        
        // ########## PART 5: VARIABLES ##########
        if(isZtoMM || isZtoEE) {
            // ---------- Angular ----------
            CosThetaStar = Utilities::ReturnCosThetaStar(theA.p4(), theV.p4());
            CosTheta1    = Utilities::ReturnCosTheta1(theV.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4(), theH.daughter(0)->p4(), theH.daughter(1)->p4());
            CosTheta2    = fabs( Utilities::ReturnCosTheta2(theH.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4(), theH.daughter(0)->p4(), theH.daughter(1)->p4()) );
            Phi          = Utilities::ReturnPhi(theA.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4(), theH.daughter(0)->p4(), theH.daughter(1)->p4());
            Phi1         = Utilities::ReturnPhi1(theA.p4(), theV.daughter(0)->p4(), theV.daughter(1)->p4());
        }
        
        Hist["a_nEvents"]->Fill(5., EventWeight);
//        if(isZtoEE) Hist["e_nEvents"]->Fill(5., EventWeight);
//        if(isZtoMM) Hist["m_nEvents"]->Fill(5., EventWeight);
    }
    else {if(Verbose) std::cout << " - N jets < 2" << std::endl;}
    */
    if(Verbose) std::cout << " - Cleaning FatJet" << std::endl;
    for(unsigned int j = 0; j < FatJetsVect.size(); ) {
        if((isZtoEE || isZtoMM) && (deltaR(FatJetsVect[j], *theV.daughter(0)) < 0.8 || deltaR(FatJetsVect[j], *theV.daughter(1)) < 0.8)) FatJetsVect.erase(FatJetsVect.begin() + j);
        else j++;
    }
    if(FatJetsVect.size() < 1) {if(Verbose) std::cout << " - N fat jets < 1" << std::endl; return;}

    // ------------------------------
    // ------- RES CANDIDATE --------
    // ------------------------------
    pat::CompositeCandidate theX;
    theX.addDaughter(theV);
    theX.addDaughter(FatJetsVect.at(0));
    addP4.set(theX);
    if(isZtoNN && theX.numberOfDaughters()>0) {
        float D = pow(FatJetsVect.at(0).energy(),2) - (pow(FatJetsVect.at(0).mass(),2) - pow(90.18,2));
        if(D>0) {
            float s1 = FatJetsVect.at(0).energy() + sqrt(D);
            float s2 = FatJetsVect.at(0).energy() - sqrt(D);
            massRecoilFormula = (fabs(s1)>fabs(s2))? s1 : s2;
        }
        else massRecoilFormula = FatJetsVect.at(0).energy();
    }
    if(Verbose) std::cout << " - Candidate built" << std::endl;
    
    // ---------- Event Variables ----------
    
    // Max b-tagged jet in the event
    for(unsigned int i = 2; i < JetsVect.size(); i++) if(JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag")) > MaxJetBTag) MaxJetBTag = JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag"));
    // Max b-tagged jet in the event
    for(unsigned int i = 0; i < JetsVect.size(); i++) if(FatJetsVect.size() > 0 && JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag")) > MaxFatJetBTag && deltaR(FatJetsVect.at(0), JetsVect[i])>0.8) MaxFatJetBTag = JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag"));
    
    for(unsigned int i = 0; i < JetsVect.size(); i++) if(fabs(reco::deltaPhi(JetsVect[i].phi(), MET.phi())) < MinJetMetDPhi) MinJetMetDPhi = fabs(reco::deltaPhi(JetsVect[i].phi(), MET.phi()));
    
    // Jet variables
    theJetAnalyzer->AddVariables(JetsVect, MET);
    theFatJetAnalyzer->AddVariables(FatJetsVect, MET);
    // Leptons
    theElectronAnalyzer->AddVariables(ElecVect, MET);
    theMuonAnalyzer->AddVariables(MuonVect, MET);
    
    // Highest CSV bjet in the event

    // ---------- Print Summary ----------
    if(Verbose) {
        std::cout << " --- Event n. " << iEvent.id().event() << ", lumi " << iEvent.luminosityBlock() << ", run " << iEvent.id().run() << ", weight " << EventWeight << std::endl;
        std::cout << "isZtoEE: " << isZtoEE << ", is ZtoMM: " << isZtoMM << ", isZtoNN: " << isZtoNN << std::endl;
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
        std::cout << "V leptonic mass:     " << theV.mass() << std::endl;
//        std::cout << "Z merged hadronic mass:     " << theHMerged.mass() << ", generated: " << GenZHadMass << std::endl;
//        std::cout << "X merged candidate mass:    " << theXMerged.mass() << ", generated: " << GenXMass << std::endl;
//        std::cout << "Z resolved hadronic mass:     " << theHResolved.mass() << ", generated: " << GenZHadMass << std::endl;
//        std::cout << "X resolved candidate mass:    " << theXResolved.mass() << ", generated: " << GenXMass << std::endl;
    }

    
    // ---------- Fill objects ----------
    if(Verbose) std::cout << " - Filling objects" << std::endl;
    if(isZtoEE || isWtoEN) for(unsigned int i = 0; i < Leptons.size() && i < ElecVect.size(); i++) ObjectsFormat::FillElectronType(Leptons[i], &ElecVect[i], isMC);
    else if(isZtoMM || isWtoMN) for(unsigned int i = 0; i < Leptons.size() && i < MuonVect.size(); i++) ObjectsFormat::FillMuonType(Leptons[i], &MuonVect[i], isMC);
    else if(isTtoEM && Leptons.size() >= 2) {
        if(ElecVect[0].pt() > MuonVect[0].pt()) {
            ObjectsFormat::FillElectronType(Leptons[0], &ElecVect[0], isMC);
            ObjectsFormat::FillMuonType(Leptons[1], &MuonVect[0], isMC);
        }
        else {
            ObjectsFormat::FillMuonType(Leptons[0], &MuonVect[0], isMC);
            ObjectsFormat::FillElectronType(Leptons[1], &ElecVect[0], isMC);
        }
    }
    for(unsigned int i = 0; i < Taus.size() && i < TauVect.size(); i++) ObjectsFormat::FillTauType(Taus[i], &TauVect[i], isMC);
    for(unsigned int i = 0; i < Photons.size() && i < PhotonVect.size(); i++) ObjectsFormat::FillPhotonType(Photons[i], &PhotonVect[i], isMC);
    for(unsigned int i = 0; i < Jets.size() && i < JetsVect.size(); i++) ObjectsFormat::FillJetType(Jets[i], &JetsVect[i], isMC);
    for(unsigned int i = 0; i < FatJets.size() && i < FatJetsVect.size(); i++) ObjectsFormat::FillFatJetType(FatJets[i], &FatJetsVect[i], isMC);
    ObjectsFormat::FillMEtType(MEt, &MET, isMC);
    ObjectsFormat::FillCandidateType(V, &theV, isMC);
    ObjectsFormat::FillCandidateType(X, &theX, isMC);
    /*
    ObjectsFormat::FillCandidateType(H, &theH, isMC);
    ObjectsFormat::FillCandidateType(A, &theA, isMC);
    ObjectsFormat::FillLorentzType(kH, &thekH);
    ObjectsFormat::FillLorentzType(kA, &thekA);
    ObjectsFormat::FillCandidateType(HMerged, &theHMerged, isMC);
    ObjectsFormat::FillCandidateType(XMerged, &theXMerged, isMC);
    ObjectsFormat::FillCandidateType(HResolved, &theHResolved, isMC);
    ObjectsFormat::FillCandidateType(XResolved, &theXResolved, isMC);
    ObjectsFormat::FillCandidateType(HResolvedPt, &theHResolvedPt, isMC);
    ObjectsFormat::FillCandidateType(XResolvedPt, &theXResolvedPt, isMC);
    ObjectsFormat::FillCandidateType(HResolvedHpt, &theHResolvedHpt, isMC);
    ObjectsFormat::FillCandidateType(XResolvedHpt, &theXResolvedHpt, isMC);
    ObjectsFormat::FillCandidateType(HResolvedDZ, &theHResolvedDZ, isMC);
    ObjectsFormat::FillCandidateType(XResolvedDZ, &theXResolvedDZ, isMC);
    ObjectsFormat::FillCandidateType(HResolvedDR, &theHResolvedDR, isMC);
    ObjectsFormat::FillCandidateType(XResolvedDR, &theXResolvedDR, isMC);
    */
    if(V.pt < 100.) return;
    
    // Fill tree
    tree->Fill();
    
    /*
    // Fill tree for alpha only closer to the signal region
    
    // cut for VZ analysis
    if( 
        (isZtoMM || isZtoEE) &&
        (V.mass > 70. && V.mass < 110.) && 
        (nFatJets >= 1) && 
        (V.pt > 150.)
    ) {
        // Lepton1
        Lepton1_isMuon = Leptons[0].isMuon;
        Lepton1_isElectron = Leptons[0].isElectron;
        Lepton1_isLoose = Leptons[0].isLoose;
        Lepton1_isHighPt = Leptons[0].isHighPt;
        Lepton1_isTrackerHighPt = Leptons[0].isTrackerHighPt;
        Lepton1_isTight = Leptons[0].isTight;
        Lepton1_pt = Leptons[0].pt;
        Lepton1_trkIso = Leptons[0].trkIso;
        // Lepton2        
        Lepton2_isMuon = Leptons[1].isMuon;
        Lepton2_isElectron = Leptons[1].isElectron;
        Lepton2_isLoose = Leptons[1].isLoose;
        Lepton2_isHighPt = Leptons[1].isHighPt;
        Lepton2_isTrackerHighPt = Leptons[1].isTrackerHighPt;
        Lepton2_isTight = Leptons[1].isTight;
        Lepton2_pt = Leptons[1].pt;
        Lepton2_trkIso = Leptons[1].trkIso;
        // MET        
        MEt_pt = MEt.pt;
        // V        
        V_pt  = V.pt;
        V_dPhi = V.dPhi;
        V_mass = V.mass;
        V_tmass = V.tmass;
        // X        
        X_pt = X.pt;
        X_dPhi = X.dPhi;
        X_mass = X.mass;
        X_tmass = X.tmass;
        // FatJet1
        FatJet1_isTight = FatJets[0].isTight;
        FatJet1_pt = FatJets[0].pt;
        FatJet1_prunedMass = FatJets[0].prunedMass;
        FatJet1_softdropMass = FatJets[0].softdropMass;
        FatJet1_softdropPuppiMass = FatJets[0].softdropPuppiMass;
        FatJet1_prunedMassCorr = FatJets[0].prunedMassCorr;
        FatJet1_softdropMassCorr = FatJets[0].softdropMassCorr;
        FatJet1_softdropPuppiMassCorr = FatJets[0].softdropPuppiMassCorr;
        FatJet1_chsTau21 = FatJets[0].chsTau21;
        FatJet1_puppiTau21 = FatJets[0].puppiTau21;
        FatJet1_ddtTau21 = FatJets[0].ddtTau21;
        FatJet1_CSV1 = FatJets[0].CSV1;
        FatJet1_CSV2 = FatJets[0].CSV2;

        treealpha->Fill();
    }
    
    if(Verbose) std::cout << " - Tree filled, end of event" << std::endl;
    */
    /*
    if(theV.mass()<80. || theV.mass()>100.) {if(Verbose) std::cout << " - Z off-shell" << std::endl; return;}
    Hist["a_nEvents"]->Fill(6., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(6., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(6., EventWeight);
    
    if(theH.mass()<90. || theH.mass()>140.) {if(Verbose) std::cout << " - No h candidate" << std::endl; return;}
    Hist["a_nEvents"]->Fill(7., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(7., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(7., EventWeight);
    
    if(MET.pt()>60.)  {if(Verbose) std::cout << " - Failed Top veto" << std::endl; return;}
    Hist["a_nEvents"]->Fill(8., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(8., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(8., EventWeight);
    
    if(!(nJets >= 2 && (JetsVect.at(0).bDiscriminator(JetPSet.getParameter<std::string>("btag")) > 0.800 || JetsVect.at(1).bDiscriminator(JetPSet.getParameter<std::string>("btag")) > 0.800)) ) {if(Verbose) std::cout << " - BTag < 1" << std::endl; return;}
    Hist["a_nEvents"]->Fill(9., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(9., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(9., EventWeight);
    
    if(!(nJets >= 2 && JetsVect.at(0).bDiscriminator(JetPSet.getParameter<std::string>("btag")) > 0.800 && JetsVect.at(1).bDiscriminator(JetPSet.getParameter<std::string>("btag")) > 0.800) ) {if(Verbose) std::cout << " - BTag < 2" << std::endl; return;}
    Hist["a_nEvents"]->Fill(10., EventWeight);
//    if(isZtoEE) Hist["e_nEvents"]->Fill(10., EventWeight);
//    if(isZtoMM) Hist["m_nEvents"]->Fill(10., EventWeight);
    */
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
    tree->Branch("FacWeightUp", &FacWeightUp, "FacWeightUp/F");
    tree->Branch("FacWeightDown", &FacWeightDown, "FacWeightDown/F");
    tree->Branch("RenWeightUp", &RenWeightUp, "RenWeightUp/F");
    tree->Branch("RenWeightDown", &RenWeightDown, "RenWeightDown/F");
    tree->Branch("ScaleWeightUp", &ScaleWeightUp, "ScaleWeightUp/F");
    tree->Branch("ScaleWeightDown", &ScaleWeightDown, "ScaleWeightDown/F");
    tree->Branch("StitchWeight", &StitchWeight, "StitchWeight/F");
    tree->Branch("ZewkWeight", &ZewkWeight, "ZewkWeight/F");
    tree->Branch("WewkWeight", &WewkWeight, "WewkWeight/F");
    tree->Branch("PUWeight", &PUWeight, "PUWeight/F");
    tree->Branch("TriggerWeight", &TriggerWeight, "TriggerWeight/F");
    tree->Branch("LeptonWeight", &LeptonWeight, "LeptonWeight/F");
    
    // Set trigger branches
    for(auto it = TriggerMap.begin(); it != TriggerMap.end(); it++) tree->Branch(it->first.c_str(), &(it->second), (it->first+"/O").c_str());
    
    // Analysis variables
    tree->Branch("isZtoEE", &isZtoEE, "isZtoEE/O");
    tree->Branch("isZtoMM", &isZtoMM, "isZtoMM/O");
    tree->Branch("isTtoEM", &isTtoEM, "isTtoEM/O");
    tree->Branch("isWtoEN", &isWtoEN, "isWtoEN/O");
    tree->Branch("isWtoMN", &isWtoMN, "isWtoMN/O");
    tree->Branch("isZtoNN", &isZtoNN, "isZtoNN/O");
    tree->Branch("isMerged", &isMerged, "isMerged/O");
    tree->Branch("isResolved", &isResolved, "isResolved/O");
    
    // Objects
    tree->Branch("nPV", &nPV, "nPV/I");
    tree->Branch("nElectrons", &nElectrons, "nElectrons/I");
    tree->Branch("nVetoElectrons", &nVetoElectrons, "nVetoElectrons/I");
    tree->Branch("nLooseElectrons", &nLooseElectrons, "nLooseElectrons/I");
    tree->Branch("nMuons", &nMuons, "nMuons/I");
    tree->Branch("nLooseMuons", &nLooseMuons, "nLooseMuons/I");
    tree->Branch("nTrackerHighPtMuons", &nTrackerHighPtMuons, "nTrackerHighPtMuons/I");
    tree->Branch("nTaus", &nTaus, "nTaus/I");
    tree->Branch("nPhotons", &nPhotons, "nPhotons/I");
    tree->Branch("nJets", &nJets, "nJets/I");
    tree->Branch("nFatJets", &nFatJets, "nFatJets/I");
    tree->Branch("nBTagJets", &nBTagJets, "nBTagJets/I");
    
    tree->Branch("MaxJetBTag", &MaxJetBTag, "MaxJetBTag/F");
    tree->Branch("MaxFatJetBTag", &MaxFatJetBTag, "MaxFatJetBTag/F");
    tree->Branch("MinJetMetDPhi", &MinJetMetDPhi, "MinJetMetDPhi/F");
    tree->Branch("Chi2", &Chi2, "Chi2/F");
    // Angular variables
    tree->Branch("CosThetaStar", &CosThetaStar, "CosThetaStar/F");
    tree->Branch("CosTheta1", &CosTheta1, "CosTheta1/F");
    tree->Branch("CosTheta2", &CosTheta2, "CosTheta2/F");
    tree->Branch("Phi", &Phi, "Phi/F");
    tree->Branch("Phi1", &Phi1, "Phi1/F");
    // Mass recoil formula
    tree->Branch("massRecoilFormula", &massRecoilFormula, "massRecoilFormula/F");
  
    // Set Branches for objects
    for(int i = 0; i < WriteNElectrons; i++) tree->Branch(("Electron"+std::to_string(i+1)).c_str(), &(Electrons[i].pt), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNMuons; i++) tree->Branch(("Muon"+std::to_string(i+1)).c_str(), &(Muons[i].pt), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNLeptons; i++) tree->Branch(("Lepton"+std::to_string(i+1)).c_str(), &(Leptons[i].pt), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNTaus; i++) tree->Branch(("Tau"+std::to_string(i+1)).c_str(), &(Taus[i].pt), ObjectsFormat::ListTauType().c_str());
    for(int i = 0; i < WriteNPhotons; i++) tree->Branch(("Photon"+std::to_string(i+1)).c_str(), &(Photons[i].pt), ObjectsFormat::ListPhotonType().c_str());
    for(int i = 0; i < WriteNJets; i++) tree->Branch(("Jet"+std::to_string(i+1)).c_str(), &(Jets[i].pt), ObjectsFormat::ListJetType().c_str());
    for(int i = 0; i < WriteNFatJets; i++) tree->Branch(("FatJet"+std::to_string(i+1)).c_str(), &(FatJets[i].pt), ObjectsFormat::ListFatJetType().c_str());
    tree->Branch("MEt", &MEt.pt, ObjectsFormat::ListMEtType().c_str());
    tree->Branch("V", &V.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("X", &X.pt, ObjectsFormat::ListCandidateType().c_str());
    /*tree->Branch("H", &H.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("A", &A.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("kH", &kH.pt, ObjectsFormat::ListLorentzType().c_str());
    tree->Branch("kA", &kA.pt, ObjectsFormat::ListLorentzType().c_str());
    tree->Branch("HMerged", &HMerged.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XMerged", &XMerged.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolved", &HResolved.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedPt", &HResolvedPt.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedHpt", &HResolvedHpt.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedDZ", &HResolvedDZ.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("HResolvedDR", &HResolvedDR.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolved", &XResolved.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedPt", &XResolvedPt.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedHpt", &XResolvedHpt.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedDZ", &XResolvedDZ.pt, ObjectsFormat::ListCandidateType().c_str());
    tree->Branch("XResolvedDR", &XResolvedDR.pt, ObjectsFormat::ListCandidateType().c_str());
    */
    // -------------------    
    /*
    // Create Tree for alpha and set Branches    
    treealpha=fs->make<TTree>("treealpha", "treealpha");

    treealpha->Branch("isMC", &isMC, "isMC/O");
    treealpha->Branch("EventWeight", &EventWeight, "EventWeight/F");
    
    treealpha->Branch("EventNumber", &EventNumber, "EventNumber/L");
    treealpha->Branch("LumiNumber", &LumiNumber, "LumiNumber/L");
    treealpha->Branch("RunNumber", &RunNumber, "RunNumber/L");

    // Set trigger branches
    for(auto it = TriggerMap.begin(); it != TriggerMap.end(); it++) treealpha->Branch(it->first.c_str(), &(it->second), (it->first+"/O").c_str());
    
    // Analysis variables
    treealpha->Branch("isZtoEE", &isZtoEE, "isZtoEE/O");
    treealpha->Branch("isZtoMM", &isZtoMM, "isZtoMM/O");
    
    // Lepton1
    treealpha->Branch("Lepton1_isMuon", &Lepton1_isMuon, "Lepton1_isMuon/O");
    treealpha->Branch("Lepton1_isElectron", &Lepton1_isElectron, "Lepton1_isElectron/O");
    treealpha->Branch("Lepton1_isLoose", &Lepton1_isLoose, "Lepton1_isLoose/O");
    treealpha->Branch("Lepton1_isHighPt", &Lepton1_isHighPt, "Lepton1_isHighPt/O");
    treealpha->Branch("Lepton1_isTrackerHighPt", &Lepton1_isTrackerHighPt, "Lepton1_isTrackerHighPt/O");
    treealpha->Branch("Lepton1_pt", &Lepton1_pt, "Lepton1_pt/F");
    treealpha->Branch("Lepton1_trkIso", &Lepton1_trkIso, "Lepton1_trkIso/F");

    // Lepton2
    treealpha->Branch("Lepton2_isMuon", &Lepton2_isMuon, "Lepton2_isMuon/O");
    treealpha->Branch("Lepton2_isElectron", &Lepton2_isElectron, "Lepton2_isElectron/O");
    treealpha->Branch("Lepton2_isLoose", &Lepton2_isLoose, "Lepton2_isLoose/O");
    treealpha->Branch("Lepton2_isHighPt", &Lepton2_isHighPt, "Lepton2_isHighPt/O");
    treealpha->Branch("Lepton2_isTrackerHighPt", &Lepton2_isTrackerHighPt, "Lepton2_isTrackerHighPt/O");
    treealpha->Branch("Lepton2_pt", &Lepton2_pt, "Lepton2_pt/F");
    treealpha->Branch("Lepton2_trkIso", &Lepton2_trkIso, "Lepton2_trkIso/F");

    // MET        
    treealpha->Branch("MEt_pt", &MEt_pt, "MEt_pt/F");

    // V        
    treealpha->Branch("V_pt", &V_pt, "V_pt/F");
    treealpha->Branch("V_mass", &V_mass, "V_mass/F");
    treealpha->Branch("V_tmass", &V_tmass, "V_tmass/F");

    // X        
    treealpha->Branch("X_pt", &X_pt, "X_pt/F");
    treealpha->Branch("X_dPhi", &X_dPhi, "X_dPhi/F");
    treealpha->Branch("X_mass", &X_mass, "X_mass/F");
    treealpha->Branch("X_tmass", &X_tmass, "X_tmass/F");

    // FatJet1
    treealpha->Branch("FatJet1_pt", &FatJet1_pt, "FatJet1_pt/F");
    treealpha->Branch("FatJet1_prunedMass", &FatJet1_prunedMass, "FatJet1_prunedMass/F");
    treealpha->Branch("FatJet1_softdropMass", &FatJet1_softdropMass, "FatJet1_softdropMass/F");
    treealpha->Branch("FatJet1_softdropPuppiMass", &FatJet1_softdropPuppiMass, "FatJet1_softdropPuppiMass/F");
    treealpha->Branch("FatJet1_prunedMassCorr", &FatJet1_prunedMassCorr, "FatJet1_prunedMassCorr/F");
    treealpha->Branch("FatJet1_softdropMassCorr", &FatJet1_softdropMassCorr, "FatJet1_softdropMassCorr/F");
    treealpha->Branch("FatJet1_softdropPuppiMassCorr", &FatJet1_softdropPuppiMassCorr, "FatJet1_softdropPuppiMassCorr/F");
    treealpha->Branch("FatJet1_chsTau21", &FatJet1_chsTau21, "FatJet1_chsTau21/F");
    treealpha->Branch("FatJet1_puppiTau21", &FatJet1_puppiTau21, "FatJet1_puppiTau21/F");
    treealpha->Branch("FatJet1_ddtTau21", &FatJet1_ddtTau21, "FatJet1_ddtTau21/F");
    */
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
