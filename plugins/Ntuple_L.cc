// -*- C++ -*-
//
// Package:    Analysis/Ntuple_L
// Class:      Ntuple_L
// 
/**\class Ntuple_L Ntuple_L.cc Analysis/Ntuple_L/plugins/Ntuple_L.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Alberto Zucchetta
//         Created:  Thu, 28 Apr 2016 08:28:54 GMT
//
//

#include "Ntuple_L.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
Ntuple_L::Ntuple_L(const edm::ParameterSet& iConfig):
    TriggerPSet(iConfig.getParameter<edm::ParameterSet>("triggerSet")),
    PileupPSet(iConfig.getParameter<edm::ParameterSet>("pileupSet")),
    ElectronPSet(iConfig.getParameter<edm::ParameterSet>("electronSet")),
    MuonPSet(iConfig.getParameter<edm::ParameterSet>("muonSet")),
    PhotonPSet(iConfig.getParameter<edm::ParameterSet>("photonSet")),
    JetPSet(iConfig.getParameter<edm::ParameterSet>("jetSet")),
    WriteNElectrons(iConfig.getParameter<int>("writeNElectrons")),
    WriteNMuons(iConfig.getParameter<int>("writeNMuons")),
    WriteNLeptons(iConfig.getParameter<int>("writeNLeptons")),
    WriteNJets(iConfig.getParameter<int>("writeNJets")),
    Verbose(iConfig.getParameter<bool>("verbose"))
{
    //now do what ever initialization is needed
    usesResource("TFileService");
    
    // Initialize Objects
    theGenAnalyzer=new GenAnalyzer();
    thePileupAnalyzer=new PileupAnalyzer(PileupPSet, consumesCollector());
    theTriggerAnalyzer=new TriggerAnalyzer(TriggerPSet, consumesCollector());
    theElectronAnalyzer=new ElectronAnalyzer(ElectronPSet, consumesCollector());
    theMuonAnalyzer=new MuonAnalyzer(MuonPSet, consumesCollector());
    thePhotonAnalyzer=new PhotonAnalyzer(PhotonPSet, consumesCollector());
    theJetAnalyzer=new JetAnalyzer(JetPSet, consumesCollector());
    //theBTagAnalyzer=new BTagAnalyzer(BTagAlgo);
    
    std::cout << "---------- STARTING ----------" << std::endl;
}


Ntuple_L::~Ntuple_L() {
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    std::cout << "---------- ENDING  ----------" << std::endl;
    
    
    delete theGenAnalyzer;
    delete thePileupAnalyzer;
    delete theTriggerAnalyzer;
    delete theElectronAnalyzer;
    delete theMuonAnalyzer;
    delete thePhotonAnalyzer;
    delete theJetAnalyzer;
    //delete theZLepAnalyzer;//L
    //delete theBTagAnalyzer;
    
}


//
// member functions
//

// ------------ method called for each event  ------------
void Ntuple_L::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    isMC = iEvent.isRealData();
    EventNumber = iEvent.id().event();
    LumiNumber = iEvent.luminosityBlock();
    RunNumber = iEvent.id().run();
    
    float EventWeight(1.), PUWeight(1.), TriggerWeight(1.), LeptonWeight(1.);
    
    // Initialize types
    for(int i = 0; i < WriteNElectrons; i++) ObjectsFormat::ResetLeptonType(Electrons[i]);
    for(int i = 0; i < WriteNMuons; i++) ObjectsFormat::ResetLeptonType(Muons[i]);
    for(int i = 0; i < WriteNLeptons; i++) ObjectsFormat::ResetLeptonType(Leptons[i]);
    for(int i = 0; i < WriteNJets; i++) ObjectsFormat::ResetJetType(Jets[i]);
    ObjectsFormat::ResetMEtType(MEt);
    ObjectsFormat::ResetCandidateType(ZLep);
    ObjectsFormat::ResetCandidateType(ZHad);
    ObjectsFormat::ResetCandidateType(X);
    
    // Electrons
    std::vector<pat::Electron> ElecVect=theElectronAnalyzer->FillElectronVector(iEvent);
    // Muons
    std::vector<pat::Muon> MuonVect=theMuonAnalyzer->FillMuonVector(iEvent);
    // Jets
    std::vector<pat::Jet> JetsVect=theJetAnalyzer->FillJetVector(iEvent);
    // Missing Energy
    pat::MET MET = theJetAnalyzer->FillMetVector(iEvent);
    
    
    // PU weight
    PUWeight = thePileupAnalyzer->GetPUWeight(iEvent);
    EventWeight *= PUWeight;
    
    // Trigger
    std::vector<std::string> TrigNames;
    TrigNames.push_back("HLT_Mu17_Mu8");
    TrigNames.push_back("HLT_Ele17_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL_Ele8_CaloIdT_CaloIsoVL_TrkIdVL_TrkIsoVL");
    int TrigBit=theTriggerAnalyzer->FillTriggerBitmap(iEvent, TrigNames);
    
    // ---------- Print Summary ----------
    if(Verbose) {
        std::cout << " --- Event n. " << iEvent.id().event() << ", lumi " << iEvent.luminosityBlock() << ", run " << iEvent.id().run() << ", PU weight " << PUWeight << std::endl;
        std::cout << "number of electrons: " << ElecVect.size() << std::endl;
        for(unsigned int i = 0; i < ElecVect.size(); i++) std::cout << "  electron [" << i << "]\tpt: " << ElecVect[i].pt() << "\teta: " << ElecVect[i].eta() << "\tphi: " << ElecVect[i].phi() << std::endl;
        std::cout << "number of muons:     " << MuonVect.size() << std::endl;
        for(unsigned int i = 0; i < MuonVect.size(); i++) std::cout << "  muon     [" << i << "]\tpt: " << MuonVect[i].pt() << "\teta: " << MuonVect[i].eta() << "\tphi: " << MuonVect[i].phi() << std::endl;
        std::cout << "number of AK4 jets:  " << JetsVect.size() << std::endl;
        for(unsigned int i = 0; i < JetsVect.size(); i++) std::cout << "  AK4 jet  [" << i << "]\tpt: " << JetsVect[i].pt() << "\teta: " << JetsVect[i].eta() << "\tphi: " << JetsVect[i].phi() << std::endl;
        std::cout << "Missing energy:      " << MET.pt() << std::endl;
    }
    
    // ---------- Do analysis selections ----------
    // ...
    
    // ---------- Fill objects ----------
    if(ElecVect.size() > MuonVect.size()) {
        for(unsigned int i = 0; i < Leptons.size() && i < ElecVect.size(); i++) ObjectsFormat::FillElectronType(Leptons[i], &ElecVect[i], isMC);
    }
    else {
        for(unsigned int i = 0; i < Leptons.size() && i < MuonVect.size(); i++) ObjectsFormat::FillMuonType(Leptons[i], &MuonVect[i], isMC);
    }
    for(unsigned int i = 0; i < Jets.size() && i < JetsVect.size(); i++) ObjectsFormat::FillJetType(Jets[i], &JetsVect[i], isMC);
    ObjectsFormat::FillMEtType(MEt, &MET, isMC);
    
    
    // ---------- Z TO LEPTONS ----------
    bool isZtoMM(false), isZtoEE(false);
    int l1(0), l2(-1);
    
    if(MuonVect.size()>=2 && ElecVect.size()>=2) {
        if(MuonVect.at(0).pt() > ElecVect.at(0).pt()) {isZtoMM=true; isZtoEE=false;}
        else {isZtoMM=false; isZtoEE=true;}
    }
    else if(ElecVect.size()>=2) {isZtoMM=false; isZtoEE=true;}
    else if(MuonVect.size()>=2) {isZtoMM=true; isZtoEE=false;}
    else {if(Verbose) std::cout << " - No Iso SF OS Leptons" << std::endl; return;}

    if(isZtoEE) {
        for(unsigned int i=1; i<ElecVect.size(); i++) if(l2<0 && ElecVect.at(i).charge()!=ElecVect.at(l1).charge()) l2=i;
    }
    else {
        for(unsigned int i=1; i<MuonVect.size(); i++) if(l2<0 && MuonVect.at(i).charge()!=MuonVect.at(l1).charge()) l2=i;
    }
    if(l1<0 || l2<0) {if(Verbose) std::cout << " - No OS SF leptons" << std::endl; return;}

    // Reconstruct Z candidate
    pat::CompositeCandidate theZ;
    if(isZtoMM) {
        theZ.addDaughter(MuonVect.at(l1));
        theZ.addDaughter(MuonVect.at(l2));
    }
    else {
        theZ.addDaughter(ElecVect.at(l1));
        theZ.addDaughter(ElecVect.at(l2));
    }
    AddFourMomenta addP4;
    addP4.set(theZ);

    pat::CompositeCandidate theX;
    isZLepSel = false;
    isZHadSel = false;

    //ObjectsFormat::FillCandidateType(theZ, &theZ, isMC);//L
    if(theZ.mass()>55 && theZ.mass()<120 && theZ.pt()>100){
        isZLepSel = true;
    }

    if(theZ.mass()<50.) {if(Verbose) std::cout << " - Z off-shell" << std::endl; return;}

    //Dummy ZHad
    pat::CompositeCandidate theZHad;
    if(JetsVect.size()>=1){
        theZHad.addDaughter(JetsVect.at(0));
        if(JetsVect.at(0).pt()>170 && abs(JetsVect.at(0).eta())<2.5){
             isZHadSel = true;
        }
    }
    AddFourMomenta addP4Had;
    addP4Had.set(theZHad);
    if(isZHadSel) std::cout << "LISA================== " << theZHad.mass() << std::endl;

    if(isZLepSel && isZHadSel){
       theX.addDaughter(theZ);
       theX.addDaughter(theZHad);    
    }

    std::cout << " Invariant mass graviton " << theX.mass() <<std::endl;
    ObjectsFormat::FillCandidateType(ZLep, &theZ, isMC);//L
    //ObjectsFormat::FillCandidateType(ZHad, &theZHad, isMC);//L
    //ObjectsFormat::FillCandidateType(X, &theX, isMC);//L

//    
//    // Lepton and Trigger SF
//    if(isMC) {
//        if(isZtoEE) {
//            TriggerWeight*=theElectronAnalyzer->GetDoubleElectronTriggerSF(ElecVect.at(l1), ElecVect.at(l2));
//            LeptonWeight*=theElectronAnalyzer->GetElectronIdSF(ElecVect.at(l1));
//            LeptonWeight*=theElectronAnalyzer->GetElectronIdSF(ElecVect.at(l2));
//            LeptonWeight*=theElectronAnalyzer->GetElectronIsoSF(ElecVect.at(l1));
//            LeptonWeight*=theElectronAnalyzer->GetElectronIsoSF(ElecVect.at(l2));
//        }
//        else {
//            TriggerWeight*=theMuonAnalyzer->GetDoubleMuonTriggerSF(MuonVect.at(l1), MuonVect.at(l2));
//            LeptonWeight*=theMuonAnalyzer->GetMuonIdSF(MuonVect.at(l1));
//            LeptonWeight*=theMuonAnalyzer->GetMuonIdSF(MuonVect.at(l2));
//            LeptonWeight*=theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(l1));
//            LeptonWeight*=theMuonAnalyzer->GetMuonIsoSF(MuonVect.at(l2));
//        }
//    }
//    EventWeight*=TriggerWeight;
//    EventWeight*=LeptonWeight;
//    
//    if(Verbose) {
//        std::cout << "\tReconstructed Z candidate from " << (isZtoMM ? "muons" : "electrons") << " " << l1 << " and " << l2 << " with mass: " << theZ.mass() << std::endl;
//    }
//    
//    // FatJet
//    if(JetsVect.size()<=0) {if(Verbose) std::cout << " - No Fat Jet" << std::endl; return;}
//    const pat::Jet* fatJet=&JetsVect.at(0);
//    int nSubJets=fatJet->numberOfDaughters();
//    const pat::Jet* subJet1 = dynamic_cast<const pat::Jet*>(fatJet->daughter(0));
//    const pat::Jet* subJet2 = dynamic_cast<const pat::Jet*>(fatJet->daughter(1));
//    //double subjet0Bdisc = subjet->bDiscriminator("combinedInclusiveSecondaryVertexV2BJetTags");
//    
//    
//    theJetAnalyzer->ApplyRecoilCorrections(MET, &MET.genMET()->p4(), &theZ.p4(), 0);

    tree->Fill();

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
void Ntuple_L::beginJob() {
    
    // Object objects are created only one in the begin job. The reference passed to the branch has to be the same
    for(int i = 0; i < WriteNElectrons; i++) Electrons.push_back( LeptonType() );
    for(int i = 0; i < WriteNMuons; i++) Muons.push_back( LeptonType() );
    for(int i = 0; i < WriteNLeptons; i++) Leptons.push_back( LeptonType() );
    for(int i = 0; i < WriteNJets; i++) Jets.push_back( JetType() );
    
    // Create Tree and set Branches
    tree=fs->make<TTree>("tree", "tree");
    tree->Branch("isMC", &isMC, "isMC/O");
    tree->Branch("EventNumber", &EventNumber, "EventNumber/L");
    tree->Branch("LumiNumber", &LumiNumber, "LumiNumber/L");
    tree->Branch("RunNumber", &RunNumber, "RunNumber/L");
    
    // Set Branches for objects
    for(int i = 0; i < WriteNElectrons; i++) tree->Branch(("Electron"+std::to_string(i+1)).c_str(), &(Electrons[i]), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNMuons; i++) tree->Branch(("Muon"+std::to_string(i+1)).c_str(), &(Muons[i]), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNLeptons; i++) tree->Branch(("Lepton"+std::to_string(i+1)).c_str(), &(Leptons[i]), ObjectsFormat::ListLeptonType().c_str());
    for(int i = 0; i < WriteNJets; i++) tree->Branch(("Jet"+std::to_string(i+1)).c_str(), &(Jets[i]), ObjectsFormat::ListJetType().c_str());
    tree->Branch("MEt", &MEt, ObjectsFormat::ListMEtType().c_str());
    tree->Branch("ZLep", &ZLep, ObjectsFormat::ListCandidateType().c_str());//L
    tree->Branch("ZHad", &ZHad, ObjectsFormat::ListCandidateType().c_str());//L
    tree->Branch("X", &X, ObjectsFormat::ListCandidateType().c_str());//L
    tree->Branch("isZLepSel", &isZLepSel, "isZLepSel/O");//L
    tree->Branch("isZHadSel", &isZHadSel, "isZHadSel/O");//L
    
}

// ------------ method called once each job just after ending the event loop  ------------
void Ntuple_L::endJob() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void Ntuple_L::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(Ntuple_L);
