
#ifndef VSDProvider_h
#define VSDProvider_h

#include "TTree.h"
////////////////////////////////////////////////
class VSDProvider;
VSDProvider *g_provider = nullptr;
class VSDProvider
{
public:
    /*
       VSDProvider(TTree *t, nlohmann::json* cfg) : m_tree(t), m_config(cfg)
       {
          // m_data = new VSDReader(t);
       }
      */

    virtual ~VSDProvider(){}

    TTree *m_tree{nullptr};
    VSDEventInfo m_eventInfo;
    //  VSDReader* m_data{nullptr};
    Long64_t m_eventIdx{0};
    std::vector<VSDCollection *> m_collections;
    nlohmann::json *m_config{nullptr};

    virtual Long64_t GetNumEvents() { return (int)m_tree->GetEntriesFast(); }

    void addCollection(VSDCollection *h)
    {
        m_collections.push_back(h);
    }

    virtual void GotoEvent(int eventIdx)
    {
        printf("goto   start \n");
        // m_tree->GetEntry(eventIdx);
        m_eventIdx = eventIdx;

        // fill VSD collections
        for (auto h : m_collections)
        {
            h->m_list.clear();

            // h->fill(*(this->m_data));
            h->fill();

            // debug
            if (0)
            {
                for (auto e : h->m_list)
                    e->dump();
            }
        }
        set_event_info();
    }

    virtual void set_event_info()
    {
        printf("vsd provier %lld events total %lld !!!!! \n", m_eventIdx, GetNumEvents());

        for (auto &vsdc : m_collections)
        {
            if (vsdc->m_purpose == "EventInfo")
            {
                VSDEventInfo *ei = (VSDEventInfo *)vsdc->m_list[0];
                m_eventInfo = *ei;
                // printf("...... setting event info %lld \n", m_eventInfo.m_event);
                return;
            }
        }
    }

    VSDCollection *RefColl(const std::string &name)
    {
        for (auto collection : m_collections)
        {
            if (name == collection->m_name)
                return collection;
        }
        return nullptr;
    };

    // nlohmann::json &RefVSDMemberConfig() { return m_config; }

    ////////////////////////////////////////////////////////////
    void registerCollection(const std::string &desc, const std::string &vsdClassType, Color_t color = kBlue, std::string filter = "")
    {
    }
};
/*
  try
  {
     using namespace nlohmann;
     TString cmd;
     json &j = m_config->at(vsdClassType);

     std::cout << j.dump(4) << "\n";
     std::string numKey;
     try {
           numKey = j["N"];
     }
     catch (std::exception &e) {
        std::cout << "missing collection size info " << e.what() << "\n";
     }

     // single element collection
     if (numKey == "undef")
     {
        cmd += TString::Format("auto vsdObj = new VSD%s();\n", vsdClassType.c_str());
        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
           if (it.key() == "N")
              continue;

           std::string k = it.key(), v = it.value();
           cmd += TString::Format("vsdObj->m_%s = r.ZZZ%s;\n", k.c_str(), v.c_str());
        }
        cmd += "m_list.push_back(vsdObj);\n";
     }
     else // create from array
     {
        cmd += TString::Format("for (int i = 0; i < r.%sZZZ; ++i) {\n", numKey.c_str());
        cmd += TString::Format("auto vsdObj = new VSD%s();\n", vsdClassType.c_str());

        for (json::iterator it = j.begin(); it != j.end(); ++it)
        {
           if (it.key() == "N")
              continue;

           std::string k = it.key(), v = it.value();
           cmd += TString::Format("vsdObj->m_%s = r.ZZZ%s[i]; \n", k.c_str(), v.c_str());
        }
        cmd += "m_list.push_back(vsdObj);\n";
        cmd += "}\n // end loop through vsd array";
     }
     // printf("VSD collection fill body  %s \n ", cmd.Data());

     // make sources for class
     std::string cname = desc;
     cname += vsdClassType;
     cname += "Collection";
     std::stringstream ss;
     ss << "class " << cname << " : public VSDCollection \n"
        << "{\n"
        << "public:\n"
        << cname << "(const std::string &n, const std::string &p) : VSDCollection(n, p) {}\n"
        << "  virtual void fill(VSDReader &r) {\n"
        << cmd.Data() << "\n}\n"
        << "};\n"
        << "\n"
        << "g_provider->addCollection(new " << cname << "(\"ZZZ\",\"" << vsdClassType << "\"));\n";

     std::cout << ss.str() << "\n\n.....\n";
     std::string exp = std::regex_replace(ss.str(), std::regex("ZZZ"), desc);
     std::cout << "Expression to evaluate" << exp << "\n";
     gROOT->ProcessLine(exp.c_str());

     // config collection
     VSDCollection *coll = RefColl(desc);
     coll->m_color = color;
     coll->m_filter = filter;
     coll->m_name = desc;
  }
  catch (std::exception &e)
  {
     std::cerr << e.what() << "\n";
  }*/
#endif
