
#include "ObjectsFormat.h"

//*******************//
//  Leptons (e+mu)   //
//*******************//

void ObjectsFormat::FillElectronType(LeptonType& I, const pat::Electron* R, bool isMC) {
    if(!R) return;
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.mass        = R->mass();
    I.energy      = R->energy();
    I.inTrkPt     = R->hasUserFloat("inTrkPt") ? R->userFloat("inTrkPt") : -1.;
    I.pfIso03     = R->hasUserFloat("pfIso03") ? R->userFloat("pfIso03") : -1.;
    I.pfIso04     = R->hasUserFloat("pfIso04") ? R->userFloat("pfIso04") : -1.;
    I.trkIso      = R->hasUserFloat("trkIso") ? R->userFloat("trkIso") : R->pfIsolationVariables().sumChargedHadronPt;
    I.miniIso     = R->hasUserFloat("miniIso") ? R->userFloat("miniIso") : -1.;
    I.dxy         = R->hasUserFloat("dxy") ? R->userFloat("dxy") : R->dB();
    I.dz          = R->hasUserFloat("dz") ? R->userFloat("dz") : 0.;
    I.ip3d        = R->dB(pat::Electron::PV3D);
    I.sip3d       = R->dB(pat::Electron::PV3D)/R->edB(pat::Electron::PV3D);
    I.nPixelHits  = R->gsfTrack()->hitPattern().numberOfValidPixelHits();
    I.dPhi_met    = R->hasUserFloat("dPhi_met") ? R->userFloat("dPhi_met") : -1.;
    I.charge      = R->charge();
    I.pdgId       = R->pdgId();
    I.isElectron  = true;
    I.isMuon      = false;
    I.isVeto      = R->hasUserInt("isVeto") ? R->userInt("isVeto") : false;
    I.isLoose     = R->hasUserInt("isLoose") ? R->userInt("isLoose") : false;
    I.isMedium    = R->hasUserInt("isMedium") ? R->userInt("isMedium") : false;
    I.isTight     = R->hasUserInt("isTight") ? R->userInt("isTight") : false;
    I.isHighPt    = R->hasUserInt("isHEEP") ? R->userInt("isHEEP") : false;
//    I.isMVANonTrigMedium      = R->hasUserInt("isMVANonTrigMedium") ? R->userInt("isMVANonTrigMedium") : false;
//    I.isMVANonTrigTight      = R->hasUserInt("isMVANonTrigTight") ? R->userInt("isMVANonTrigTight") : false;
//    I.isMVATrigMedium      = R->hasUserInt("isMVATrigMedium") ? R->userInt("isMVATrigMedium") : false;
//    I.isMVATrigTight      = R->hasUserInt("isMVATrigTight") ? R->userInt("isMVATrigTight") : false;
    if(isMC && R->genLepton()) I.isMatched = false;//(Utilities::FindMotherId(dynamic_cast<const reco::Candidate*>(R->genLepton()))==23);
}

void ObjectsFormat::FillMuonType(LeptonType& I, const pat::Muon* R, bool isMC) {
    if(!R) return; 
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.mass        = R->mass();
    I.energy      = R->energy();
    I.inTrkPt     = R->hasUserFloat("inTrkPt") ? R->userFloat("inTrkPt") : -1.;
    I.pfIso03     = R->hasUserFloat("pfIso03") ? R->userFloat("pfIso03") : -1.;
    I.pfIso04     = R->hasUserFloat("pfIso04") ? R->userFloat("pfIso04") : -1.;
    I.trkIso      = R->hasUserFloat("trkIso") ? R->userFloat("trkIso") : R->trackIso();
    I.miniIso     = R->hasUserFloat("miniIso") ? R->userFloat("miniIso") : -1.;
    I.dxy         = R->hasUserFloat("dxy") ? R->userFloat("dxy") : R->dB();
    I.dz          = R->hasUserFloat("dz") ? R->userFloat("dz") : 0.;
    I.ip3d        = R->dB(pat::Muon::PV3D);
    I.sip3d       = R->dB(pat::Muon::PV3D)/R->edB(pat::Muon::PV3D);
    I.nPixelHits  = R->innerTrack()->hitPattern().numberOfValidPixelHits();
    I.dPhi_met    = R->hasUserFloat("dPhi_met") ? R->userFloat("dPhi_met") : -1.;
    I.charge      = R->charge();
    I.pdgId       = R->pdgId();
    I.isElectron  = false;
    I.isMuon      = true;
    I.isVeto      = R->isPFMuon();
    I.isLoose     = R->hasUserInt("isLoose") ? R->userInt("isLoose") : false;
    I.isMedium    = R->hasUserInt("isMedium") ? R->userInt("isMedium") : false;
    I.isTight     = R->hasUserInt("isTight") ? R->userInt("isTight") : false;
    I.isHighPt    = R->hasUserInt("isHighPt") ? R->userInt("isHighPt") : false;
    I.isTrackerHighPt = R->hasUserInt("isTrackerHighPt") ? R->userInt("isTrackerHighPt") : false;
    if(isMC && R->genLepton()) I.isMatched = false;//(Utilities::FindMotherId(dynamic_cast<const reco::Candidate*>(R->genLepton()))==23);
}

void ObjectsFormat::ResetLeptonType(LeptonType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.mass        = -1.;
    I.energy      = -1.;
    I.inTrkPt     = -1.;
    I.pfIso03     = -1.;
    I.pfIso04     = -1.;
    I.trkIso      = -1.;
    I.miniIso     = -1.;
    I.dxy         = -99.;
    I.dz          = -99.;
    I.ip3d        = -99.;
    I.sip3d       = -99.;
    I.nPixelHits  = -1.;
    I.dPhi_met    = -1.;
    I.charge      = 0;
    I.pdgId       = 0;
    I.isElectron  = false;
    I.isMuon      = false;
    I.isVeto      = false;
    I.isLoose     = false;
    I.isMedium    = false;
    I.isTight     = false;
    I.isHighPt    = false;
    I.isTrackerHighPt = false;
//    I.isMVANonTrigMedium = false;
//    I.isMVANonTrigTight = false;
//    I.isMVATrigMedium = false;
//    I.isMVATrigTight = false;
    I.isMatched   = false;
}

std::string ObjectsFormat::ListLeptonType() {return "pt/F:eta/F:phi/F:mass/F:energy/F:inTrkPt/F:pfIso03/F:pfIso04/F:trkIso/F:miniIso/F:dxy/F:dz/F:ip3d/F:sip3d/F:nPixelHits/F:dPhi_met/F:charge/I:pdgId/I:isElectron/O:isMuon/O:isVeto/O:isLoose/O:isMedium/O:isTight/O:isHighPt/O:isTrackerHighPt/O:isMatched/O";} // isHEEP/O:isMVANonTrigMedium/O:isMVANonTrigTight/O:isMVATrigMedium/O:isMVATrigTight/O:

//********************//
//    Photons         // 
//********************//

void ObjectsFormat::FillPhotonType(PhotonType& I, const pat::Photon* R, bool isMC) {
    if(!R) return;
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.mass        = R->mass();
    I.energy      = R->energy();
    I.pfIso       = R->hasUserFloat("pfIso") ? R->userFloat("pfIso") : -1.;
    I.dz          = R->hasUserFloat("dz") ? R->userFloat("dz") : 0.;
    I.charge      = R->charge();
    I.pdgId       = R->pdgId();
    I.isLoose     = R->hasUserInt("isLoose") ? R->userInt("isLoose") : false;
    I.isMedium    = R->hasUserInt("isMedium") ? R->userInt("isMedium") : false;
    I.isTight     = R->hasUserInt("isTight") ? R->userInt("isTight") : false;
    I.isMVANonTrigMedium      = R->hasUserInt("isMVANonTrigMedium") ? R->userInt("isMVANonTrigMedium") : false;
    if(isMC && R->genPhoton()) I.isMatched = false;//(Utilities::FindMotherId(dynamic_cast<const reco::Candidate*>(R->genLepton()))==23);
}


void ObjectsFormat::ResetPhotonType(PhotonType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.mass        = -1.;
    I.energy      = -1.;
    I.pfIso       = -1.;
    I.dz          = -99.;
    I.charge      = 0;
    I.pdgId       = 0;
    I.isLoose     = false;
    I.isMedium    = false;
    I.isTight     = false;
    I.isMVANonTrigMedium = false;
    I.isMatched   = false;
}

std::string ObjectsFormat::ListPhotonType() {return "pt/F:eta/F:phi/F:mass/F:energy/F:pfIso/F:dz/F:charge/I:pdgId/I:isLoose/O:isMedium/O:isTight/O:isMVANonTrigMedium/O:isMatched/O";}


//********************//
//       Taus         // 
//********************//

void ObjectsFormat::FillTauType(TauType& I, const pat::Tau* R, bool isMC) {
    if(!R) return;
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.mass        = R->mass();
    I.energy      = R->energy();
    I.pfIso       = R->hasUserFloat("pfIso") ? R->userFloat("pfIso") : -1.;
    I.dz          = R->hasUserFloat("dz") ? R->userFloat("dz") : 0.;
    I.charge      = R->charge();
    I.pdgId       = R->pdgId();
    I.isLoose     = R->hasUserInt("isLoose") ? R->userInt("isLoose") : false;
    I.isMedium    = R->hasUserInt("isMedium") ? R->userInt("isMedium") : false;
    I.isTight     = R->hasUserInt("isTight") ? R->userInt("isTight") : false;
    if(isMC) I.isMatched = false;//(Utilities::FindMotherId(dynamic_cast<const reco::Candidate*>(R->genLepton()))==23);
}


void ObjectsFormat::ResetTauType(TauType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.mass        = -1.;
    I.energy      = -1.;
    I.pfIso       = -1.;
    I.dz          = -99.;
    I.charge      = 0;
    I.pdgId       = 0;
    I.isLoose     = false;
    I.isMedium    = false;
    I.isTight     = false;
    I.isMatched   = false;
}

std::string ObjectsFormat::ListTauType() {return "pt/F:eta/F:phi/F:mass/F:energy/F:pfIso/F:dz/F:charge/I:pdgId/I:isLoose/O:isMedium/O:isTight/O:isMatched/O";}


//*******************//
//        Jets       //
//*******************//

void ObjectsFormat::FillJetType(JetType& I, const pat::Jet* R, bool isMC) {
    if(!R) return;
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.mass        = R->mass();
    I.energy      = R->energy();
  //  if(isMC && R->genJet()) {
  //    I.ptGenJ    = R->genJet()->pt();
  //    I.etaGenJ   = R->genJet()->eta();
  //    I.phiGenJ   = R->genJet()->phi();
  //    I.massGenJ  = R->genJet()->mass();
  //  }
  //  if(isMC && R->genParton()) {
  //    I.ptGen     = R->genParton()->pt();
  //    I.etaGen    = R->genParton()->eta();
  //    I.phiGen    = R->genParton()->phi();
  //    I.massGen   = R->genParton()->mass();
  //  }
  //  if(isMC && R->genParton()) {
  //    I.ptLhe     = R->userFloat("ptLhe");
  //    I.etaLhe    = R->userFloat("etaLhe");
  //    I.phiLhe    = R->userFloat("phiLhe");
  //  }
    I.ptRaw       = R->correctedJet(0).pt();
    I.ptUnc       = R->userFloat("JESUncertainty");
    I.dPhi_met    = R->hasUserFloat("dPhi_met") ? R->userFloat("dPhi_met") : -1.;
    I.dPhi_Jet1   = R->hasUserFloat("dPhi_Jet1") ? R->userFloat("dPhi_Jet1") : -1.;
    I.puId        = -1.; //R->userFloat("pileupJetId:fullDiscriminant");
    I.CSV         = R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CSVR        = R->hasUserFloat("ReshapedDiscriminator") ? R->userFloat("ReshapedDiscriminator") : R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CSVRUp      = R->hasUserFloat("ReshapedDiscriminatorUp") ? R->userFloat("ReshapedDiscriminatorUp") : R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CSVRDown    = R->hasUserFloat("ReshapedDiscriminatorDown") ? R->userFloat("ReshapedDiscriminatorDown") : R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CMVA        = R->bDiscriminator("pfCombinedMVAV2BJetTags");
    I.CMVAR       = R->hasUserFloat("CMVAR") ? R->userFloat("CMVAR") : -99.;
    I.CMVARUp     = R->hasUserFloat("CMVARUp") ? R->userFloat("CMVARUp") : -99.;
    I.CMVARDown   = R->hasUserFloat("CMVARDown") ? R->userFloat("CMVARDown") : -99.;
  //  I.CSVV1       = R->bDiscriminator("combinedSecondaryVertexV1BJetTags");
  //  I.CSVSL       = R->bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags");
  //  I.JPro        = R->bDiscriminator("jetProbabilityBJetTags"); // jetBProbabilityBJetTags
    I.QGLikelihood = R->hasUserFloat("QGLikelihood") ? R->userFloat("QGLikelihood") : -1.;
  //  if(isMC) {
  //    if(abs(R->partonFlavour())==5 || abs(R->partonFlavour())==4) {
  //      I.LooseSF      = theBTagWeight->ReturnScaleFactor(1, R->pt(), R->eta());
  //      I.MediumSF     = theBTagWeight->ReturnScaleFactor(2, R->pt(), R->eta());
  //      I.TightSF      = theBTagWeight->ReturnScaleFactor(3, R->pt(), R->eta());
  //      I.LooseSFa     = (1.-0.7*theBTagWeight->ReturnScaleFactor(1, R->pt(), R->eta()))/(1.-0.7);
  //      I.MediumSFa    = (1.-0.6*theBTagWeight->ReturnScaleFactor(2, R->pt(), R->eta()))/(1.-0.6);
  //      I.TightSFa     = (1.-0.5*theBTagWeight->ReturnScaleFactor(3, R->pt(), R->eta()))/(1.-0.5);
  //    }
  //    else {
  //      I.LooseSF      = theBTagWeight->ReturnScaleFactorMistag(1, R->pt(), R->eta());
  //      I.MediumSF     = theBTagWeight->ReturnScaleFactorMistag(2, R->pt(), R->eta());
  //      I.TightSF      = theBTagWeight->ReturnScaleFactorMistag(3, R->pt(), R->eta());
  //      I.LooseSFa     = (1.-1.e-1*theBTagWeight->ReturnScaleFactorMistag(1, R->pt(), R->eta()))/(1.-1.e-1);
  //      I.MediumSFa    = (1.-1.e-2*theBTagWeight->ReturnScaleFactorMistag(2, R->pt(), R->eta()))/(1.-1.e-2);
  //      I.TightSFa     = (1.-1.e-3*theBTagWeight->ReturnScaleFactorMistag(3, R->pt(), R->eta()))/(1.-1.e-3);
  //    }
  //  }
    I.chf         = R->chargedHadronEnergyFraction();
    I.nhf         = R->neutralHadronEnergyFraction();
    I.phf         = R->neutralEmEnergyFraction();
    I.elf         = R->chargedEmEnergyFraction();
    I.muf         = R->muonEnergyFraction();
    I.chm         = R->chargedHadronMultiplicity();
    I.npr         = R->chargedMultiplicity() + R->neutralMultiplicity();
    I.flavour     = R->partonFlavour();
    if(isMC && R->genParton()) I.mother = false;//Utilities::FindMotherId(dynamic_cast<const reco::Candidate*>(R->genParton()));
    I.isLoose     = R->hasUserInt("isLoose") ? R->userInt("isLoose") : false;
    I.isMedium    = false;
    I.isTight     = R->hasUserInt("isTight") ? R->userInt("isTight") : false;
    I.isTightLepVeto     = R->hasUserInt("isTightLepVeto") ? R->userInt("isTightLepVeto") : false;
    I.isCSVL      = false;//IsBTagged(Tagger, 1, I.CSV);
    I.isCSVM      = false;//IsBTagged(Tagger, 2, I.CSV);
    I.isCSVT      = false;//IsBTagged(Tagger, 3, I.CSV);
    I.isMatched   = (I.mother==25);
}

void ObjectsFormat::ResetJetType(JetType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.mass        = -1.;
    I.energy      = -1.;
    I.ptRaw       = -1.;
    I.ptUnc       = -1.;
    I.dPhi_met    = -1.;
    I.dPhi_Jet1   = -1.;
    I.puId        = -1.;
    I.CSV         = -99.;
    I.CSVR        = -99.;
    I.CSVRUp      = -99.;
    I.CSVRDown    = -99.;
    I.CMVA        = -99.;
    I.CMVAR       = -99.;
    I.CMVARUp     = -99.;
    I.CMVARDown   = -99.;
    I.QGLikelihood = -1.;
    I.chf         = -1.;
    I.nhf         = -1.;
    I.phf         = -1.;
    I.elf         = -1.;
    I.muf         = -1.;
    I.chm         = -1.;
    I.npr         = -1.;
    I.flavour     = 0;
    I.mother      = false;
    I.isLoose     = false;
    I.isMedium    = false;
    I.isTight     = false;
    I.isTightLepVeto     = false;
    I.isCSVL      = false;
    I.isCSVM      = false;
    I.isCSVT      = false;
    I.isMatched   = false;
}

std::string ObjectsFormat::ListJetType() {return "pt/F:eta/F:phi/F:mass/F:energy/F:ptRaw/F:ptUnc/F:dPhi_met/F:dPhi_Jet1/F:puId/F:CSV/F:CSVR/F:CSVRUp/F:CSVRDown/F:CMVA/F:CMVAR/F:CMVARUp/F:CMVARDown/F:QGLikelihood/F:chf/F:nhf/F:phf/F:elf/F:muf/F:chm/I:npr/I:flavour/I:mother/I:isLoose/O:isMedium/O:isTight/O:isTightLepVeto/O:isCSVL/O:isCSVM/O:isCSVT/O:isMatched/O";}

//*******************//
//     Fat Jet       //
//*******************//

void ObjectsFormat::FillFatJetType(FatJetType& I, const pat::Jet* R, bool isMC) {
    if(!R) return;
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.mass        = R->mass();
    I.energy      = R->energy();
  //  if(isMC && R->genJet()) {
  //    I.ptGenJ    = R->genJet()->pt();
  //    I.etaGenJ   = R->genJet()->eta();
  //    I.phiGenJ   = R->genJet()->phi();
  //    I.massGenJ  = R->genJet()->mass();
  //  }
  //  if(isMC && R->genParton()) {
  //    I.ptGen     = R->genParton()->pt();
  //    I.etaGen    = R->genParton()->eta();
  //    I.phiGen    = R->genParton()->phi();
  //    I.massGen   = R->genParton()->mass();
  //  }
  //  if(isMC && R->genParton()) {
  //    I.ptLhe     = R->userFloat("ptLhe");
  //    I.etaLhe    = R->userFloat("etaLhe");
  //    I.phiLhe    = R->userFloat("phiLhe");
  //  }
    I.ptRaw       = R->correctedJet(0).pt();
    I.ptUnc       = R->userFloat("JESUncertainty");
    I.dPhi_met    = R->hasUserFloat("dPhi_met") ? R->userFloat("dPhi_met") : -1.;
    I.dPhi_Jet1   = R->hasUserFloat("dPhi_Jet1") ? R->userFloat("dPhi_Jet1") : -1.;
    I.puId        = -1.; //R->userFloat("pileupJetId:fullDiscriminant");
    I.CSV         = R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CSVR        = R->hasUserFloat("ReshapedDiscriminator") ? R->userFloat("ReshapedDiscriminator") : R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CSVRUp      = R->hasUserFloat("ReshapedDiscriminatorUp") ? R->userFloat("ReshapedDiscriminatorUp") : R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
    I.CSVRDown    = R->hasUserFloat("ReshapedDiscriminatorDown") ? R->userFloat("ReshapedDiscriminatorDown") : R->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags");
  //  I.CSVV1       = R->bDiscriminator("combinedSecondaryVertexV1BJetTags");
  //  I.CSVSL       = R->bDiscriminator("combinedSecondaryVertexSoftPFLeptonV1BJetTags");
  //  I.JPro        = R->bDiscriminator("jetProbabilityBJetTags"); // jetBProbabilityBJetTags
  //  if(isMC) {
  //    if(abs(R->partonFlavour())==5 || abs(R->partonFlavour())==4) {
  //      I.LooseSF      = theBTagWeight->ReturnScaleFactor(1, R->pt(), R->eta());
  //      I.MediumSF     = theBTagWeight->ReturnScaleFactor(2, R->pt(), R->eta());
  //      I.TightSF      = theBTagWeight->ReturnScaleFactor(3, R->pt(), R->eta());
  //      I.LooseSFa     = (1.-0.7*theBTagWeight->ReturnScaleFactor(1, R->pt(), R->eta()))/(1.-0.7);
  //      I.MediumSFa    = (1.-0.6*theBTagWeight->ReturnScaleFactor(2, R->pt(), R->eta()))/(1.-0.6);
  //      I.TightSFa     = (1.-0.5*theBTagWeight->ReturnScaleFactor(3, R->pt(), R->eta()))/(1.-0.5);
  //    }
  //    else {
  //      I.LooseSF      = theBTagWeight->ReturnScaleFactorMistag(1, R->pt(), R->eta());
  //      I.MediumSF     = theBTagWeight->ReturnScaleFactorMistag(2, R->pt(), R->eta());
  //      I.TightSF      = theBTagWeight->ReturnScaleFactorMistag(3, R->pt(), R->eta());
  //      I.LooseSFa     = (1.-1.e-1*theBTagWeight->ReturnScaleFactorMistag(1, R->pt(), R->eta()))/(1.-1.e-1);
  //      I.MediumSFa    = (1.-1.e-2*theBTagWeight->ReturnScaleFactorMistag(2, R->pt(), R->eta()))/(1.-1.e-2);
  //      I.TightSFa     = (1.-1.e-3*theBTagWeight->ReturnScaleFactorMistag(3, R->pt(), R->eta()))/(1.-1.e-3);
  //    }
  //  }
    I.prunedMass            = R->hasUserFloat("ak8PFJetsCHSPrunedMass") ? R->userFloat("ak8PFJetsCHSPrunedMass") : -1.;
    I.softdropMass          = R->hasUserFloat("ak8PFJetsCHSSoftDropMass") ? R->userFloat("ak8PFJetsCHSSoftDropMass") : -1.;
    I.softdropPuppiMass     = R->hasUserFloat("ak8PFJetsPuppiSoftDropMass") ? R->userFloat("ak8PFJetsPuppiSoftDropMass") : -1.;
    I.prunedMassCorr        = R->hasUserFloat("ak8PFJetsCHSPrunedMassCorr") ? R->userFloat("ak8PFJetsCHSPrunedMassCorr") : -1.;
    I.softdropMassCorr      = R->hasUserFloat("ak8PFJetsCHSSoftDropMassCorr") ? R->userFloat("ak8PFJetsCHSSoftDropMassCorr") : -1.;
    I.softdropPuppiMassCorr = R->hasUserFloat("ak8PFJetsPuppiSoftDropMassCorr") ? R->userFloat("ak8PFJetsPuppiSoftDropMassCorr") : -1.;
    I.pt1         = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->pt() : -1.;
    I.eta1        = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->eta() : -1.;
    I.phi1        = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->phi() : -1.;
    I.mass1       = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->mass() : -1.;
    I.CSV1        = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") : -99.;
    I.CSVR1       = R->hasUserFloat("ReshapedDiscriminator1") ? R->userFloat("ReshapedDiscriminator1") : -99.;
    I.CSVR1Up     = R->hasUserFloat("ReshapedDiscriminatorUp1") ? R->userFloat("ReshapedDiscriminatorUp1") : -99.;
    I.CSVR1Down   = R->hasUserFloat("ReshapedDiscriminatorDown1") ? R->userFloat("ReshapedDiscriminatorDown1") : -99.;
    I.CMVA1       = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->bDiscriminator("pfCombinedMVAV2BJetTags") : -99.;
    I.CMVAR1      = R->hasUserFloat("CMVAR1") ? R->userFloat("CMVAR1") : -99.;
    I.CMVAR1Up    = R->hasUserFloat("CMVAR1Up") ? R->userFloat("CMVAR1Up") : -99.;
    I.CMVAR1Down  = R->hasUserFloat("CMVAR1Down") ? R->userFloat("CMVAR1Down") : -99.;
    I.flavour1    = R->subjets("SoftDrop").size() > 0 ? R->subjets("SoftDrop")[0]->hadronFlavour() : -1.;
    I.pt2         = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->pt() : -1.;
    I.eta2        = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->eta() : -1.;
    I.phi2        = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->phi() : -1.;
    I.mass2       = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->mass() : -1.;
    I.CSV2        = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->bDiscriminator("pfCombinedInclusiveSecondaryVertexV2BJetTags") : -99.;
    I.CSVR2       = R->hasUserFloat("ReshapedDiscriminator2") ? R->userFloat("ReshapedDiscriminator2") : -99.;
    I.CSVR2Up     = R->hasUserFloat("ReshapedDiscriminatorUp2") ? R->userFloat("ReshapedDiscriminatorUp2") : -99.;
    I.CSVR2Down   = R->hasUserFloat("ReshapedDiscriminatorDown2") ? R->userFloat("ReshapedDiscriminatorDown2") : -99.;
    I.CMVA2       = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->bDiscriminator("pfCombinedMVAV2BJetTags") : -99.;
    I.CMVAR2      = R->hasUserFloat("CMVAR2") ? R->userFloat("CMVAR2") : -99.;
    I.CMVAR2Up    = R->hasUserFloat("CMVAR2Up") ? R->userFloat("CMVAR2Up") : -99.;
    I.CMVAR2Down  = R->hasUserFloat("CMVAR2Down") ? R->userFloat("CMVAR2Down") : -99.;
    I.flavour2    = R->subjets("SoftDrop").size() > 1 ? R->subjets("SoftDrop")[1]->hadronFlavour() : -1.;
    I.dR          = R->subjets("SoftDrop").size() > 1 ? deltaR(*R->subjets("SoftDrop")[0], *R->subjets("SoftDrop")[1]) : -1.;
    I.chsTau21    = R->userFloat("NjettinessAK8:tau1") != 0 ? R->userFloat("NjettinessAK8:tau2")/R->userFloat("NjettinessAK8:tau1") : -1.;
    I.puppiTau21  = R->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1") != 0 ? R->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau2")/R->userFloat("ak8PFJetsPuppiValueMap:NjettinessAK8PuppiTau1") : -1.;
    I.ddtTau21    = R->hasUserFloat("ddtTau21") ? R->userFloat("ddtTau21") : -1.;
    I.BDSV        = R->bDiscriminator("pfBoostedDoubleSecondaryVertexAK8BJetTags");
    I.chf         = R->chargedHadronEnergyFraction();
    I.nhf         = R->neutralHadronEnergyFraction();
    I.phf         = R->neutralEmEnergyFraction();
    I.elf         = R->chargedEmEnergyFraction();
    I.muf         = R->muonEnergyFraction();
    I.chm         = R->chargedHadronMultiplicity();
    I.npr         = R->chargedMultiplicity() + R->neutralMultiplicity();
    I.flavour     = R->partonFlavour();
    if(isMC && R->genParton()) I.mother = false;//Utilities::FindMotherId(dynamic_cast<const reco::Candidate*>(R->genParton()));
    I.isLoose     = R->hasUserInt("isLoose") ? R->userInt("isLoose") : false;
    I.isMedium    = false;
    I.isTight     = R->hasUserInt("isTight") ? R->userInt("isTight") : false;
    I.isTightLepVeto     = R->hasUserInt("isTightLepVeto") ? R->userInt("isTightLepVeto") : false;
    I.isMatched   = (I.mother==25);
}

void ObjectsFormat::ResetFatJetType(FatJetType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.mass        = -1.;
    I.energy      = -1.;
    I.ptRaw       = -1.;
    I.ptUnc       = -1.;
    I.dPhi_met    = -1.;
    I.dPhi_Jet1   = -1.;
    I.puId        = -1.;
    I.CSV         = -99.;
    I.CSVR        = -99.;
    I.CSVRUp      = -99.;
    I.CSVRDown    = -99.;
    I.prunedMass            = -1.;
    I.softdropMass          = -1.;
    I.softdropPuppiMass     = -1.;
    I.prunedMassCorr        = -1.;
    I.softdropMassCorr      = -1.;
    I.softdropPuppiMassCorr = -1.;
    I.pt1         = -1.;
    I.eta1        = -9.;
    I.phi1        = -9.;
    I.mass1       = -1.;
    I.CSV1        = -99.;
    I.CSVR1       = -99.;
    I.CSVR1Up     = -99.;
    I.CSVR1Down   = -99.;
    I.CMVA1       = -99.;
    I.CMVAR1      = -99.;
    I.CMVAR1Up    = -99.;
    I.CMVAR1Down  = -99.;
    I.flavour1    = -1.;
    I.pt2         = -1.;
    I.eta2        = -9.;
    I.phi2        = -9.;
    I.mass2       = -1.;
    I.CSV2        = -99.;
    I.CSVR2       = -99.;
    I.CSVR2Up     = -99.;
    I.CSVR2Down   = -99.;
    I.CMVA2       = -99.;
    I.CMVAR2      = -99.;
    I.CMVAR2Up    = -99.;
    I.CMVAR2Down  = -99.;
    I.flavour2    = -1.;
    I.dR          = -1.;
    I.chsTau21    = -1.;
    I.puppiTau21  = -1.;
    I.ddtTau21    = -1.;
    I.BDSV        = -99.;
    I.chf         = -1.;
    I.nhf         = -1.;
    I.phf         = -1.;
    I.elf         = -1.;
    I.muf         = -1.;
    I.chm         = -1.;
    I.npr         = -1.;
    I.flavour     = 0;
    I.mother      = false;
    I.isLoose     = false;
    I.isMedium    = false;
    I.isTight     = false;
    I.isTightLepVeto     = false;
    I.isMatched   = false;
}

std::string ObjectsFormat::ListFatJetType() {return "pt/F:eta/F:phi/F:mass/F:energy/F:ptRaw/F:ptUnc/F:dPhi_met/F:dPhi_Jet1/F:puId/F:CSV/F:CSVR/F:CSVRUp/F:CSVRDown/F:prunedMass/F:softdropMass/F:softdropPuppiMass/F:prunedMassCorr/F:softdropMassCorr/F:softdropPuppiMassCorr/F:pt1/F:eta1/F:phi1/F:mass1/F:CSV1/F:CSVR1/F:CSVR1Up/F:CSVR1Down/F:CMVA1/F:CMVAR1/F:CMVAR1Up/F:CMVAR1Down/F:flavour1/F:pt2/F:eta2/F:phi2/F:mass2/F:CSV2/F:CSVR2/F:CSVR2Up/F:CSVR2Down/F:CMVA2/F:CMVAR2/F:CMVAR2Up/F:CMVAR2Down/F:flavour2/F:dR/F:chsTau21/F:puppiTau21/F:ddtTau21/F:BDSV/F:chf/F:nhf/F:phf/F:elf/F:muf/F:chm/I:npr/I:flavour/I:mother/I:isLoose/O:isMedium/O:isTight/O:isTightLepVeto/O:isCSVL/O:isCSVM/O:isCSVT/O:isMatched/O";}




//*******************//
//  Missing energy   //
//*******************//

//void ObjectsFormat::FillMEtType(MEtType& I, const pat::MET* R, bool isMC) {
//    I.pt          = R->pt();
//    I.eta         = R->eta();
//    I.phi         = R->phi();
//    I.sign        = R->metSignificance();
//}

//void ObjectsFormat::ResetMEtType(MEtType& I) {
//    I.pt          = -1.;
//    I.eta         = -9.;
//    I.phi         = -9.;
//    I.sign        = -1.;
//}

//std::string ObjectsFormat::ListMEtType() {return "pt/F:eta/F:phi/F:sign/F";}


void ObjectsFormat::FillMEtType(MEtType& I, const pat::MET* R, bool isMC) {
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.sign        = R->metSignificance();
    I.ptRaw       = R->hasUserFloat("ptRaw") ? R->userFloat("ptRaw") : -1.;
    I.phiRaw      = R->hasUserFloat("phiRaw") ? R->userFloat("phiRaw") : -9.;
    I.ptType1     = R->hasUserFloat("ptType1") ? R->userFloat("ptType1") : -1.;
    I.phiType1    = R->hasUserFloat("phiType1") ? R->userFloat("phiType1") : -9.;
    if(isMC && R->genMET()) {I.ptGen       = R->genMET()->pt();}
    if(isMC && R->genMET()) {I.phiGen      = R->genMET()->phi();}
    I.ptScaleUp   = R->hasUserFloat("ptScaleUp") ? R->userFloat("ptScaleUp") : -1.;
    I.ptScaleDown = R->hasUserFloat("ptScaleDown") ? R->userFloat("ptScaleDown") : -1.;
    I.ptResUp     = R->hasUserFloat("ptResUp") ? R->userFloat("ptResUp") : -1.;
    I.ptResDown   = R->hasUserFloat("ptResDown") ? R->userFloat("ptResDown") : -1.;
    I.ptNoMu      = R->hasUserFloat("ptNoMu") ? R->userFloat("ptNoMu") : -1.;
    I.phiNoMu     = R->hasUserFloat("phiNoMu") ? R->userFloat("phiNoMu") : -9.;
}

void ObjectsFormat::ResetMEtType(MEtType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.sign        = -1.;
    I.ptRaw       = -1.;
    I.phiRaw      = -9.;
    I.ptType1     = -1.;
    I.phiType1    = -9.;
    I.ptGen       = -1.;
    I.phiGen      = -9.;
    I.ptScaleUp   = -1.;
    I.ptScaleDown = -1.;
    I.ptResUp     = -1.;
    I.ptResDown   = -1.;
    I.ptNoMu      = -1.;
    I.phiNoMu     = -9.;
}

std::string ObjectsFormat::ListMEtType() {return "pt/F:eta/F:phi/F:sign/F:ptRaw/F:phiRaw/F:ptType1/F:phiType1/F:ptGen/F:phiGen/F:ptScaleUp/F:ptScaleDown/F:ptResUp/F:ptResDown/F:ptNoMu/F:phiNoMu/F";}



void ObjectsFormat::FillMEtFullType(MEtFullType& I, const pat::MET* R, bool isMC) {
    I.pt          = R->pt();
    I.eta         = R->eta();
    I.phi         = R->phi();
    I.sign        = R->metSignificance();
    I.ptRaw       = R->uncorPt();
    I.phiRaw      = R->uncorPhi();
    if(isMC && R->genMET()) {I.ptGen       = R->genMET()->pt();}
    if(isMC && R->genMET()) {I.phiGen      = R->genMET()->phi();}
    I.ptJERUp     = R->shiftedPt(pat::MET::METUncertainty::JetResUp);
    I.ptJERDown   = R->shiftedPt(pat::MET::METUncertainty::JetResDown);
    I.ptJESUp     = R->shiftedPt(pat::MET::METUncertainty::JetEnUp);
    I.ptJESDown   = R->shiftedPt(pat::MET::METUncertainty::JetEnDown);
    I.ptMUSUp     = R->shiftedPt(pat::MET::METUncertainty::MuonEnUp);
    I.ptMUSDown   = R->shiftedPt(pat::MET::METUncertainty::MuonEnDown);
    I.ptELSUp     = R->shiftedPt(pat::MET::METUncertainty::ElectronEnUp);
    I.ptELSDown   = R->shiftedPt(pat::MET::METUncertainty::ElectronEnDown);
    I.ptTAUUp     = R->shiftedPt(pat::MET::METUncertainty::TauEnUp);
    I.ptTAUDown   = R->shiftedPt(pat::MET::METUncertainty::TauEnDown);
    I.ptUNCUp     = R->shiftedPt(pat::MET::METUncertainty::UnclusteredEnUp);
    I.ptUNCDown   = R->shiftedPt(pat::MET::METUncertainty::UnclusteredEnDown);
    I.ptPHOUp     = R->shiftedPt(pat::MET::METUncertainty::PhotonEnUp);
    I.ptPHODown   = R->shiftedPt(pat::MET::METUncertainty::PhotonEnDown);
    I.phf         = R->NeutralEMFraction();
    I.nhf         = R->NeutralHadEtFraction();
    I.elf         = R->ChargedEMEtFraction();
    I.chf         = R->ChargedHadEtFraction();
    I.muf         = R->MuonEtFraction();
    I.ptNoMu      = R->hasUserFloat("ptNoMu") ? R->userFloat("ptNoMu") : -1.;
    I.phiNoMu     = R->hasUserFloat("phiNoMu") ? R->userFloat("phiNoMu") : -9.;
}

void ObjectsFormat::ResetMEtFullType(MEtFullType& I) {
    I.pt          = -1.;
    I.eta         = -9.;
    I.phi         = -9.;
    I.sign        = -1.;
    I.ptRaw       = -1.;
    I.phiRaw      = -9.;
    I.ptGen       = -1.;
    I.phiGen      = -9.;
    I.ptJERUp     = -1.;
    I.ptJERDown   = -1.;
    I.ptJESUp     = -1.;
    I.ptJESDown   = -1.;
    I.ptMUSUp     = -1.;
    I.ptMUSDown   = -1.;
    I.ptELSUp     = -1.;
    I.ptELSDown   = -1.;
    I.ptTAUUp     = -1.;
    I.ptTAUDown   = -1.;
    I.ptUNCUp     = -1.;
    I.ptUNCDown   = -1.;
    I.ptPHOUp     = -1.;
    I.ptPHODown   = -1.;
    I.phf         = -1.;
    I.nhf         = -1.;
    I.elf         = -1.;
    I.chf         = -1.;
    I.muf         = -1.;
    I.ptNoMu      = -1.;
    I.phiNoMu     = -9.;
}

std::string ObjectsFormat::ListMEtFullType() {return "pt/F:eta/F:phi/F:sign/F:ptRaw/F:phiRaw/F:ptGen/F:phiGen/F:ptJERUp/F:ptJERDown/F:ptJESUp/F:ptJESDown/F:ptMUSUp/F:ptMUSDown/F:ptELSUp/F:ptELSDown/F:ptTAUUp/F:ptTAUDown/F:ptUNCUp/F:ptUNCDown/F:ptPHOUp/F:ptPHODown/F:phf/F:nhf/F:elf/F:chf/F:muf/F:ptNoMu/F:phiNoMu/F";}


void ObjectsFormat::FillCandidateType(CandidateType& I, pat::CompositeCandidate* R, bool isMC) {
  if(!R) return;
  if(R->numberOfDaughters() == 0) return;
  I.pt          = R->pt();
  I.eta         = R->eta();
  I.phi         = R->phi();
  I.mass        = R->mass();
//  I.tmass       = R->numberOfDaughters()>1 ? sqrt( 2.*R->daughter(0)->pt()*R->daughter(1)->pt()*(1.-cos(deltaPhi(R->daughter(0)->phi(), R->daughter(1)->phi())) ) ) : -1.;
//Lisa
  I.tmass       = R->numberOfDaughters()>1 ? sqrt( 2.*R->daughter(0)->pt()*R->daughter(1)->pt()*(1.-cos(deltaPhi(R->daughter(0)->phi(), R->daughter(1)->phi())) ) ) : (R->numberOfDaughters()==1 ? R->mt() : -1.);
//  I.softdropMass= R->hasUserFloat("softdropMass") ? R->userFloat("softdropMass") : -1.; 
  I.dR          = R->numberOfDaughters()>1 ? deltaR(*R->daughter(0), *R->daughter(1)) : -1.;
  I.dEta        = R->numberOfDaughters()>1 ? fabs( R->daughter(0)->eta() - R->daughter(1)->eta() ) : -1.;
  I.dPhi        = R->numberOfDaughters()>1 ? fabs( deltaPhi(R->daughter(0)->phi(), R->daughter(1)->phi()) ) : -1.;
  I.twist       = R->numberOfDaughters()>1 ? fabs(atan( deltaPhi(R->daughter(0)->phi(), R->daughter(1)->phi())/fabs(R->daughter(0)->eta()-R->daughter(1)->eta()) )) : -1.;
//  I.angle       = (R->daughter(0)->momentum().unit()).Dot(R->daughter(1)->momentum().unit()); // acos()
//  I.ptBalance   = ((R->daughter(0)->pt()-R->daughter(1)->pt()) / (R->daughter(0)->pt()+R->daughter(1)->pt())/2.);
//  I.centrality  = (R->daughter(0)->pt()+R->daughter(0)->pt()) / (R->daughter(0)->p()+R->daughter(0)->p());// /R->mass();
//  I.charge      = R->charge();
}

void ObjectsFormat::ResetCandidateType(CandidateType& I) {
  I.pt          = -1.;
  I.eta         = -9.;
  I.phi         = -9.;
  I.mass        = -1.;
  I.tmass       = -1.;
//  I.softdropMass = -1.;
  I.dR          = -1.;
  I.dEta        = -1.;
  I.dPhi        = -1.;
  I.twist       = -1.;
//  I.angle       = -1.;
//  I.ptBalance   = -1.;
//  I.centrality  = -1.;
//  I.charge      = -1.;
}

std::string ObjectsFormat::ListCandidateType() {return "pt/F:eta/F:phi/F:mass/F:tmass/F:dR/F:dEta/F:dPhi/F:twist/F";}

/*
void ObjectsFormat::FillCandidateType(CandidateType& I, const reco::Candidate::LorentzVector* V1, const reco::Candidate::LorentzVector* V2) {
  if(!V1 || !V2) return;
  reco::Candidate::LorentzVector V(*V1+*V2);
  I.pt          = V.pt();
  I.eta         = V.eta();
  I.phi         = V.phi();
  I.et          = V.Et();
  I.p           = V.P();
  I.energy      = V.energy();
  I.mass        = V.mass();
  I.dEta        = fabs( V1->eta() - V2->eta() );
  I.dPhi        = M_PI - fabs(fabs(V1->phi()-V2->phi()) - M_PI);
  I.dR          = sqrt( I.dEta*I.dEta + I.dPhi*I.dPhi );
  I.twist       = fabs(atan( I.dPhi/I.dEta ));
//  I.angle       = (R->daughter(0)->momentum().unit()).Dot(R->daughter(1)->momentum().unit()); // acos()
//  I.ptBalance   = ((R->daughter(0)->pt()-R->daughter(1)->pt()) / (R->daughter(0)->pt()+R->daughter(1)->pt())/2.);
//  I.centrality  = (R->daughter(0)->pt()+R->daughter(0)->pt()) / (R->daughter(0)->p()+R->daughter(0)->p());// /R->mass();
//  I.charge      = R->charge();
}
*/

void ObjectsFormat::FillLorentzType(LorentzType& I, const reco::Candidate::LorentzVector* V) {
  I.pt          = V->pt();
  I.eta         = V->eta();
  I.phi         = V->phi();
  I.energy      = V->energy();
  I.mass        = V->mass();
}

void ObjectsFormat::ResetLorentzType(LorentzType& I) {
  I.pt          = -1.;
  I.eta         = -9.;
  I.phi         = -9.;
  I.energy      = -1.;
  I.mass        = -1.;
}

std::string ObjectsFormat::ListLorentzType() {return "pt/F:eta/F:phi/F:energy/F:mass/F";}


