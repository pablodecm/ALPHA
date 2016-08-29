// -*- C++ -*-
//
// Package:    Analysis/Ntuple
// Class:      Ntuple
// 
/**\class Ntuple Ntuple.cc Analysis/Ntuple/plugins/Ntuple.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Alberto Zucchetta
//         Created:  Thu, 28 Apr 2016 08:28:54 GMT
//
//

#ifndef DIBOSON_H
#define DIBOSON_H

// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "CommonTools/Utils/interface/TFileDirectory.h"

// Consumes
#include "FWCore/Framework/interface/EDConsumerBase.h"
#include "FWCore/Framework/interface/ConsumesCollector.h"

// miniAOD
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/PatCandidates/interface/MET.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
// Pat
#include "DataFormats/MuonReco/interface/MuonSelectors.h"
#include "DataFormats/PatCandidates/interface/Conversion.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "DataFormats/JetReco/interface/PileupJetIdentifier.h"
#include "DataFormats/PatCandidates/interface/PFParticle.h"
#include "DataFormats/PatCandidates/interface/CompositeCandidate.h"
#include "PhysicsTools/CandUtils/interface/EventShapeVariables.h"
#include "CommonTools/CandUtils/interface/AddFourMomenta.h"
// Vertex
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
// PU
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "PhysicsTools/Utilities/interface/LumiReWeighting.h"
// Trigger
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HLTReco/interface/TriggerObject.h"
#include "DataFormats/HLTReco/interface/TriggerEvent.h"
// Gen Info
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHERunInfoProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHEEventProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/LHECommonBlocks.h"
// dR and dPhi
#include "DataFormats/Math/interface/deltaR.h"
#include "DataFormats/Math/interface/deltaPhi.h"

#include "TTree.h"
#include "TH2.h"
#include "TH1.h"

#include "Objects.h"
#include "ObjectsFormat.h"
#include "GenAnalyzer.h"
#include "PileupAnalyzer.h"
#include "TriggerAnalyzer.h"
#include "ElectronAnalyzer.h"
#include "MuonAnalyzer.h"
#include "TauAnalyzer.h"
#include "PhotonAnalyzer.h"
#include "TauAnalyzer.h"
#include "JetAnalyzer.h"
//#include "BTagInterface.h"
#include "Utilities.h"


//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class Diboson : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
    public:
        explicit Diboson(const edm::ParameterSet&);
        ~Diboson();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        // ----------member data ---------------------------
        edm::ParameterSet GenPSet;
        edm::ParameterSet PileupPSet;
        edm::ParameterSet TriggerPSet;
        edm::ParameterSet ElectronPSet;
        edm::ParameterSet MuonPSet;
        edm::ParameterSet TauPSet;
        edm::ParameterSet PhotonPSet;
        edm::ParameterSet JetPSet;
        edm::ParameterSet FatJetPSet;
        
        boost::shared_ptr<FactorizedJetCorrector> jecAK8_;
        
        int WriteNElectrons, WriteNMuons, WriteNLeptons, WriteNTaus, WriteNPhotons, WriteNJets, WriteNFatJets;
        std::string HistFile;
        bool Verbose;

        GenAnalyzer* theGenAnalyzer;
        PileupAnalyzer* thePileupAnalyzer;
        TriggerAnalyzer* theTriggerAnalyzer;
        ElectronAnalyzer* theElectronAnalyzer;
        MuonAnalyzer* theMuonAnalyzer;
        TauAnalyzer* theTauAnalyzer;
        PhotonAnalyzer* thePhotonAnalyzer;
        JetAnalyzer* theJetAnalyzer;
        JetAnalyzer* theFatJetAnalyzer;
        //BTagInterface* theBTagInterface;
        std::map<std::string, bool> TriggerMap;
        std::map<std::string, TH1F*> Hist;
            
        edm::Service<TFileService> fs;
        TTree* tree;
        /*TTree* treealpha;*/
        bool isMC, isZtoEE, isZtoMM, isTtoEM, isWtoEN, isWtoMN, isZtoNN, isMerged, isResolved;
        long int EventNumber, RunNumber, LumiNumber;
        float EventWeight, StitchWeight, ZewkWeight, WewkWeight, PUWeight, TriggerWeight, LeptonWeight;
        float FacWeightUp, FacWeightDown, RenWeightUp, RenWeightDown, ScaleWeightUp, ScaleWeightDown;
        int nPV, nElectrons, nVetoElectrons, nLooseElectrons, nMuons, nLooseMuons, nTrackerHighPtMuons, nTaus, nPhotons, nJets, nFatJets, nBTagJets;
        float MaxJetBTag, MaxFatJetBTag, MinJetMetDPhi, Chi2;
        // Angular
        float CosThetaStar, CosTheta1, CosTheta2, Phi, Phi1, AngularLD;
        // Mass recoil formula
        float massRecoilFormula;
        /*
        // Lepton1
        bool Lepton1_isMuon, Lepton1_isElectron, Lepton1_isLoose, Lepton1_isHighPt, Lepton1_isTrackerHighPt, Lepton1_isTight;
        float Lepton1_pt, Lepton1_trkIso;
        // Lepton2        
        bool Lepton2_isMuon, Lepton2_isElectron, Lepton2_isLoose, Lepton2_isHighPt, Lepton2_isTrackerHighPt, Lepton2_isTight;
        float Lepton2_pt, Lepton2_trkIso;
        // MET        
        float MEt_pt;
        // V        
        float V_pt, V_dPhi, V_mass, V_tmass;
        // X        
        float X_pt, X_dPhi, X_mass, X_tmass;
        // FatJet1
        bool FatJet1_isTight;
        float FatJet1_pt, FatJet1_prunedMass, FatJet1_softdropMass, FatJet1_softdropPuppiMass, FatJet1_prunedMassCorr, FatJet1_softdropMassCorr, FatJet1_softdropPuppiMassCorr, FatJet1_chsTau21, FatJet1_puppiTau21, FatJet1_ddtTau21, FatJet1_CSV1, FatJet1_CSV2;
        */
        //
        std::vector<LeptonType> Electrons;
        std::vector<LeptonType> Muons;
        std::vector<LeptonType> Leptons;
        std::vector<TauType> Taus;
        std::vector<PhotonType> Photons;
        std::vector<JetType> Jets;
        std::vector<FatJetType> FatJets;
        MEtType MEt;
        CandidateType V, X;
        /*CandidateType H, A;
        CandidateType HMerged, HResolved, HResolvedPt, HResolvedHpt, HResolvedDZ, HResolvedDR;
        CandidateType XMerged, XResolved, XResolvedPt, XResolvedHpt, XResolvedDZ, XResolvedDR;
        LorentzType kH, kA;*/
};

#endif
