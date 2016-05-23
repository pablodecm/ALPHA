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

#ifndef NTUPLE_H
#define NTUPLE_H

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
//// KinFitter
//#include "PhysicsTools/KinFitter/interface/TKinFitter.h"
//#include "PhysicsTools/KinFitter/interface/TFitConstraintM.h"
////#include "PhysicsTools/KinFitter/interface/TFitParticlePtEtaPhi.h"

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
#include "PhotonAnalyzer.h"
#include "JetAnalyzer.h"
//#include "BTagInterface.h"



//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<> and also remove the line from
// constructor "usesResource("TFileService");"
// This will improve performance in multithreaded jobs.

class Ntuple_L : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
    public:
        explicit Ntuple_L(const edm::ParameterSet&);
        ~Ntuple_L();

        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


    private:
        virtual void beginJob() override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        virtual void endJob() override;

        // ----------member data ---------------------------
        edm::ParameterSet TriggerPSet;
        edm::ParameterSet PileupPSet;
        edm::ParameterSet ElectronPSet;
        edm::ParameterSet MuonPSet;
        edm::ParameterSet PhotonPSet;
        edm::ParameterSet JetPSet;
        int WriteNElectrons, WriteNMuons, WriteNLeptons, WriteNJets;
        bool Verbose;

        GenAnalyzer* theGenAnalyzer;
        PileupAnalyzer* thePileupAnalyzer;
        TriggerAnalyzer* theTriggerAnalyzer;
        ElectronAnalyzer* theElectronAnalyzer;
        MuonAnalyzer* theMuonAnalyzer;
        PhotonAnalyzer* thePhotonAnalyzer;
        JetAnalyzer* theJetAnalyzer;
        //BTagInterface* theBTagInterface;
        
        edm::Service<TFileService> fs;
        TTree* tree;
        bool isMC;
        bool isZLepSel, isZHadSel;
        long int EventNumber, RunNumber, LumiNumber;
        
        //
        std::vector<LeptonType> Electrons;
        std::vector<LeptonType> Muons;
        std::vector<LeptonType> Leptons;
        std::vector<JetType> Jets;
        MEtType MEt;
        CandidateType ZLep;
        CandidateType ZHad;
        CandidateType X;
};

#endif
