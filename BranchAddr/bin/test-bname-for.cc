#include <iostream>
#include <regex>

#include "TRint.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TEventList.h"
#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "DataFormats/Scouting/interface/ScoutingVertex.h"
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"

#include "DataFormats/Scouting/interface/ScoutingMuon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"

#include "DataFormats/Scouting/interface/ScoutingCaloJet.h"
#include "DataFormats/Scouting/interface/Run3ScoutingCaloJet.h"

#include "DataFormats/Scouting/interface/ScoutingPFJet.h"
#include "DataFormats/Scouting/interface/Run3ScoutingPFJet.h"

#include "FwliteTestModule/BranchAddr/interface/VSDBase.h"
#include "FwliteTestModule/BranchAddr/interface/VSDProvider.h"
#include "FwliteTestModule/BranchAddr/interface/evd.h"


fwlite::Event* g_event = nullptr;

//========================================================================


class ScoutingVertexVSDCollection : public VSDCollection
{
   public:
   ScoutingVertexVSDCollection(const std::string& n,const std::string& p) : VSDCollection(n,p) {}
   void fill()
   {
      edm::Handle<Run3ScoutingVertexCollection> handle;
      edm::InputTag tag("hltScoutingPrimaryVertexPacker", "primaryVtx");
      try
      {
         g_event->getByLabel(tag, handle);
         const Run3ScoutingVertexCollection* vertices = &*handle;//handle.product();
         printf(" entries from scouting vertices(addr=%p) \n", (void *)vertices);
         for (auto &v : *vertices)
         {
            VSDVertex *e = new VSDVertex();
            //printf("    vtx: x = %.3f\n", v.x());
            e->m_x = v.x();
            e->m_y = v.y();
            e->m_z = v.z();
            m_list.push_back(e);
         }
      }
      catch (const cms::Exception &iE)
      {
         std::cerr << iE.what() << std::endl << "e xit \n";
         exit(1);
      }
   }
};

class ScoutingMuonVSDCollection : public VSDCollection
{
   public:
   ScoutingMuonVSDCollection(const std::string& n,const std::string& p) : VSDCollection(n,p) {}
   void fill()
   {
      edm::Handle<Run3ScoutingMuonCollection> handle;
      edm::InputTag tag("hltScoutingMuonPacker", "");
      try
      {
         g_event->getByLabel(tag, handle);
         const Run3ScoutingMuonCollection *coll = handle.product();
         printf(" entries from scouting Muon(addr=%p) \n", (void *)coll);
         for (auto &v : *coll)
         {
            VSDMuon *e = new VSDMuon();
            // printf("    vtx: x = %.3f\n", v.x());
            e->m_pt = v.pt();
            e->m_eta = v.eta();
            e->m_phi = v.phi();
            m_list.push_back(e);
         }
      }
      catch (const cms::Exception &iE)
      {
         std::cerr << iE.what() << std::endl;
      }
   }
};

/*
class ScoutingJetVSDCollection : public VSDCollection
{
   public:
   ScoutingJetVSDCollection(const std::string& n,const std::string& p) : VSDCollection(n,p) {}
   void fill()
   {
      edm::Handle<Run3ScoutingJetCollection> handle;
      edm::InputTag tag("hltScoutingJetPacker", "");
      try
      {
         g_event->getByLabel(tag, handle);
         const Run3ScoutingJetCollection *coll = handle.product();
         printf(" entries from scouting Muon(addr=%p) \n", (void *)coll);
         for (auto &i : *coll)
         {
            VSDJet *e = new VSDJet();
            // printf("    vtx: x = %.3f\n", v.x());
            e->m_pt = i.pt();
            e->m_eta = i.eta();
            e->m_phi = i.phi();
            e->m_area = i.area();
            e->m_hadFraction = (i.hadEnergyInHB() + i.hadEnergyInHE()  + i.hadEnergyInHF())/
              (i.hadEnergyInHB() + i.hadEnergyInHE()  + i.hadEnergyInHF() + i.emEnergyInEB() + i.emEnergyInEE() + i.emEnergyInHF());
            m_list.push_back(e);
         }
      }
      catch (const cms::Exception &iE)
      {
         std::cerr << iE.what() << std::endl;
      }
   }
}
*/


class ScoutingPFJetVSDCollection : public VSDCollection
{
   public:
   ScoutingPFJetVSDCollection(const std::string& n,const std::string& p) : VSDCollection(n,p) {}
   void fill()
   {
      edm::Handle<Run3ScoutingPFJetCollection> handle;
      edm::InputTag tag("hltScoutingPFPacker", "");
      try
      {
         g_event->getByLabel(tag, handle);
         const Run3ScoutingPFJetCollection *coll = handle.product();
         printf(" entries from scouting PFJet(addr=%p) \n", (void *)coll);
         for (auto &i : *coll)
         {
            VSDJet *e = new VSDJet();
            e->m_pt = i.pt();
            e->m_eta = i.eta();
            e->m_phi = i.phi();
            e->m_coneR = i.jetArea()*0.5;
            m_list.push_back(e);
         }
      }
      catch (const cms::Exception &iE)
      {
         std::cerr << iE.what() << std::endl;
      }
   }
};

struct ScoutingProvider : public VSDProvider
{
   TFile              *m_file;
   TTree              *m_event_tree;
   fwlite::Event      *m_event;

   //------------------------------------------------------

   ScoutingProvider(const char* fname)
   {
      m_file = TFile::Open(fname);
      m_tree = (TTree*)m_file->Get("Events");
      m_event_tree = dynamic_cast<TTree*>(m_file->Get("Events"));
      m_event = 0;
      try
      {
         m_event = new fwlite::Event(m_file);
         g_event = m_event;
      }
      catch (const cms::Exception& iE)
      {
         printf("can't create a fwlite::Event\n");
         std::cerr << iE.what() <<std::endl;
         throw;
      }

      auto vertexCollection = new ScoutingVertexVSDCollection("Vertex", "Vertex");
      m_collections.push_back(vertexCollection);
      auto muonCollection = new ScoutingMuonVSDCollection("Muon", "Muon");
      m_collections.push_back(muonCollection);
      auto pfJetCollection = new ScoutingPFJetVSDCollection("PFJet", "Jet");
      m_collections.push_back(pfJetCollection);
   }
   ~ScoutingProvider()
   {
      delete m_event;
      delete m_file;
   }

   //------------------------------------------------------

   using VSDProvider::GotoEvent;
   void GotoEvent(int eventIdx)
   {
      Long64_t tid = eventIdx; // ?? AMT
      m_event->to(tid);
      m_event_tree->LoadTree(tid);
      VSDProvider::GotoEvent(eventIdx);
   }

};

//========================================================================

int main(int argc, char* argv[])
{
   if (argc < 2)
   {
      std::cout << "Need input file!" << std::endl;
      return 1;
   }

   const char* dummyArgvArray[] = {argv[0]};
   char** dummyArgv = const_cast<char**>(dummyArgvArray);
   int dummyArgc = 1;

   /* App app(argv[1]);
   for (Long64_t e = 0; e < 3; ++e)
   {
      printf("========================================\n");
      printf("Event %lld:\n", e);

      app.GotoEvent(e);

      // app.dump_some_event_stuff();
   }

   */  
  // app = new TApplication("fwShow", &dummyArgc, dummyArgv);
   auto app = new TRint("fwShowScouts", &dummyArgc, dummyArgv);

   ROOT::EnableThreadSafety();
   g_provider = new ScoutingProvider(argv[1]);
   evd();


   app->Run();

   return 0;
}
