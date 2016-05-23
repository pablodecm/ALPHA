import FWCore.ParameterSet.Config as cms

process = cms.Process('Analysis')
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(100))
process.source = cms.Source('PoolSource', fileNames = cms.untracked.vstring(
['/store/user/lbenato/BulkGraviton_ZZ_ZlepZhad_narrow_M1000_13TeV-madgraph_MINIAOD_10000ev/BulkGravToZZToZlepZhad_narrow_M-1000_13TeV-madgraph_PRIVATE-MC/BulkGraviton_ZZ_ZlepZhad_narrow_M1000_13TeV-madgraph_MINIAOD_10000ev/160515_095632/0000/BulkGraviton_ZZ_ZlepZhad_narrow_M1000_13TeV-madgraph_MINIAOD_1.root',
'/store/user/lbenato/BulkGraviton_ZZ_ZlepZhad_narrow_M1000_13TeV-madgraph_MINIAOD_10000ev/BulkGravToZZToZlepZhad_narrow_M-1000_13TeV-madgraph_PRIVATE-MC/BulkGraviton_ZZ_ZlepZhad_narrow_M1000_13TeV-madgraph_MINIAOD_10000ev/160515_095632/0000/BulkGraviton_ZZ_ZlepZhad_narrow_M1000_13TeV-madgraph_MINIAOD_10.root',]
) )
process.options = cms.untracked.PSet(wantSummary = cms.untracked.bool(False))

process.load('FWCore.MessageLogger.MessageLogger_cfi')
process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.myGenPlots = cms.EDAnalyzer("MyGenPlots",
                                   )

process.TFileService = cms.Service('TFileService', fileName=cms.string('myGenPlot.root'))

process.p = cms.Path( process.myGenPlots )
