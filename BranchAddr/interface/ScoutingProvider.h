

#include "TFile.h"
#include "TTree.h"

#include "TROOT.h"

#include "DataFormats/FWLite/interface/Event.h"
#include "DataFormats/Scouting/interface/ScoutingVertex.h"
#include "DataFormats/Scouting/interface/Run3ScoutingVertex.h"

#include "DataFormats/Scouting/interface/ScoutingMuon.h"
#include "DataFormats/Scouting/interface/Run3ScoutingMuon.h"

#include "DataFormats/Scouting/interface/ScoutingCaloJet.h"
#include "DataFormats/Scouting/interface/Run3ScoutingCaloJet.h"

#include "DataFormats/Scouting/interface/ScoutingPFJet.h"
#include "DataFormats/Scouting/interface/Run3ScoutingPFJet.h"

#include "FwliteTestModule/BranchAddr/interface/VSDBase.h"
fwlite::Event* g_event = nullptr;

class ScoutingVSDCollection : public VSDCollection
{
   public:
   ScoutingVSDCollection(const std::string& n,const std::string& p) : VSDCollection(n,p) {}
   edm::InputTag m_tag;
};

class ScoutingEventInfoVSDCollection : public VSDCollection
{
   public:
   ScoutingEventInfoVSDCollection(const std::string& n,const std::string& p) : VSDCollection(n, p) {}
   void fill()
   {
      VSDEventInfo *e = new VSDEventInfo();
      e->m_run = g_event->id().run();
      e->m_lumi = g_event->id().luminosityBlock();
      e->m_event = g_event->id().event();
      m_list.push_back(e);
   }
};



struct ScoutingProvider : public VSDProvider
{
   TFile              *m_file;
   TTree              *m_event_tree;
   fwlite::Event      *m_event;

   //------------------------------------------------------


   ~ScoutingProvider()
   {
      delete m_event;
      delete m_file;
   }

   //------------------------------------------------------

   using VSDProvider::GotoEvent;
   void GotoEvent(int eventIdx)
   {
      try
      {
         printf("go to event \n");
         Long64_t tid = eventIdx; // ?? AMT
         m_event->to(tid);
         m_event_tree->LoadTree(tid);
         VSDProvider::GotoEvent(eventIdx);
      }
      catch (std::exception &e)
      {
         std::cout << "GotoEvent " << e.what() << "\n";
      }
   }

   ScoutingProvider(const char* fname)
   {
      m_file = TFile::Open(fname);
      m_tree = (TTree *)m_file->Get("Events");
      m_event_tree = dynamic_cast<TTree *>(m_file->Get("Events"));
      m_event = 0;
      try
      {
         m_event = new fwlite::Event(m_file);
         g_event = m_event;
      }
      catch (const cms::Exception &iE)
      {
         printf("can't create a fwlite::Event\n");
         std::cerr << iE.what() << std::endl;
         throw;
      }
      auto infoCollection = new ScoutingEventInfoVSDCollection("EventInfo", "EventInfo");
      m_collections.push_back(infoCollection);
   }
   
};

ScoutingVSDCollection *makeCollection(const std::string name, const std::string &ScoutingClassType, const std::string &vsdClassType)
{
   std::cout << "example of a print .... \n";

   nlohmann::json &j = g_provider->m_config->at(ScoutingClassType);
   // std::cout << j.dump(4) << std::endl;

   TString fillFunc;
   fillFunc += "void fill() {\n";
   //  fillFunc += TString::Format("\tedm::Handle<%sCollection> handle;\n", ScoutingClassType.c_str());
   fillFunc += TString::Format("\tedm::Handle <std::vector<%s> > handle;\n", ScoutingClassType.c_str());
   fillFunc += TString::Format("\tg_event->getByLabel(m_tag, handle);\n");
   fillFunc += TString::Format("\tfor (auto &i : *handle.product()) {\n");
   fillFunc += TString::Format("\t\tauto vsdObj = new VSD%s();\n", vsdClassType.c_str());
   for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
   {
      std::string k = it.key(), v = it.value();
      fillFunc += TString::Format("\t\tvsdObj->m_%s = %s; \n", k.c_str(), v.c_str());
   }
   fillFunc += "\t\tm_list.push_back(vsdObj);\n";
   fillFunc += "\t} // end loop through vsd array\n";
   fillFunc += "}\n"; // end fill function

   // vsd collection expression
   TString cname = TString::Format("%s_TEST_%sCollection", ScoutingClassType.c_str(), vsdClassType.c_str());
   TString expr;
   // expr += "#include \"FwliteTestModule/BranchAddr/interface/VSDBase.h\"\n";
   expr += TString::Format("class %s : public ScoutingVSDCollection { \npublic:\n", cname.Data());
   expr += TString::Format("%s(const std::string &n, const std::string &p) : ScoutingVSDCollection(n, p) {}\n", cname.Data());
   expr += fillFunc;
   expr += "};// end class \n\n";

   // instancing
   expr += TString::Format("g_provider->addCollection(new %s(\"%s\", \"%s\"));\n", cname.Data(), name.c_str(), vsdClassType.c_str());

   std::cout << expr.Data() << "\n";
   gROOT->ProcessLine(expr.Data());

   // config collection
   return dynamic_cast<ScoutingVSDCollection *>(g_provider->RefColl(name));
}
