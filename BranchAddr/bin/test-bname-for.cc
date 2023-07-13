#include <iostream>
#include <regex>

#include "TRint.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TFile.h"

#include "DataFormats/FWLite/interface/Event.h"
#include "FwliteTestModule/BranchAddr/interface/VSDBase.h"
#include "FwliteTestModule/BranchAddr/interface/VSDProvider.h"
#include "FwliteTestModule/BranchAddr/interface/ScoutingProvider.h"
#include "FwliteTestModule/BranchAddr/interface/evd.h"

#include "nlohmann/json.hpp"


//========================================================================

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      std::cout << "Need input file!" << std::endl;
      return 1;
   }

   const char *dummyArgvArray[] = {argv[0]};
   char **dummyArgv = const_cast<char **>(dummyArgvArray);
   int dummyArgc = 1;

   auto app = new TRint("fwShowScouts", &dummyArgc, dummyArgv);

   ROOT::EnableThreadSafety();

   g_provider = new ScoutingProvider(argv[1]);

   // member mapping for Run3 scouting data
   nlohmann::json j;
   g_provider->m_config = &j;

   j["Run3ScoutingVertex"] = {{"x", "i.x()"}, {"y", "i.y()"}, {"z", "i.z()"}};
   j["Run3ScoutingPFJet"] = {{"pt", "i.pt()"}, {"eta", "i.eta()"}, {"phi", "i.phi()"}, {"coneR", "i.jetArea() *0.5"}}; // do had fraction
   j["Run3ScoutingMuon"] = {{"pt", "i.pt()"}, {"eta", "i.eta()"}, {"phi", "i.phi()"}, {"charge", "i.charge()"}, {"global", "i.isGlobalMuon()"}};
   j["Run3ScoutingParticle"] = {{"pt", "i.pt()"}, {"eta", "i.eta()"}, {"phi", "i.phi()"}, {"charge", "TDatabasePDG::Instance()->GetParticle(i.pdgId())->Charge()"}};
   
 
    // add edm collection
   {
      auto vsdColl = makeCollection("Vertices", "Run3ScoutingVertex", "Vertex");
      vsdColl->m_tag = edm::InputTag("hltScoutingPrimaryVertexPacker", "primaryVtx");
      vsdColl->m_color = kOrange;
   }
   {
      auto vsdColl = makeCollection("fireworksPFJet", "Run3ScoutingPFJet", "Jet");
      vsdColl->m_tag = edm::InputTag("hltScoutingPFPacker", "");
   }
   {
      auto vsdColl = makeCollection("Muon", "Run3ScoutingMuon", "Muon");
      vsdColl->m_tag = edm::InputTag("hltScoutingMuonPacker", "");
      vsdColl->m_color = kRed;
   }
   {
      auto vsdColl = makeCollection("Particle", "Run3ScoutingParticle", "Candidate");
      vsdColl->m_tag = edm::InputTag("hltScoutingPFPacker", "");
      vsdColl->m_filter = "i.m_pt > 7";
      vsdColl->m_color = kGreen;
   }

   evd();
   app->Run();

   return 0;
}
