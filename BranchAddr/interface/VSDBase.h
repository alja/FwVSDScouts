#ifndef VSDBase_h
#define VSDBase_h

#include "nlohmann/json.hpp"
#include "TStyle.h"
#include "Rtypes.h"
class VSDProvider;

/////////////////////////////////////////////////
class VSDBase
{
public:
virtual ~VSDBase(){}
   virtual void dump() { printf("dump VSD Base class\n"); }
};

/////////////////////////////////////////////////
class VSDVertex : public VSDBase
{
public:
   //ROOT::Math::Polar3DPoint location;
   float m_x;
   float m_y; 
   float m_z;

public:
   // VSDVertex(){}
   // VSDVertex(float ix, float iy, float iz) { m_x= ix; m_y = iy; m_z = iz;}
   virtual ~VSDVertex(){}
   void dump() { printf("VSDVertex x:%.2f, y:%.2f, z:%.2f \n", m_x, m_y,m_z); }

   float x() const { return m_x; }
   float y() const { return m_y; }
   float z() const { return m_z; }
};

/////////////////////////////////////////////////
class VSDCandidate : public VSDBase
{
// protected:
public:
   // ROOT::Math::Polar3DPoint momentum;
   float m_eta{0.f}; float m_phi{0.f}; float m_pt{0.f};
   int m_charge{0};

public:
   virtual ~VSDCandidate(){}
   // VSDCandidate(float ipt, float ieta, float iphi, int charge = 0) :m_pt(ipt), m_eta(ieta), m_phi(iphi), m_charge(charge){}
   float phi() const { return m_phi; }
   float eta() const { return m_eta; }
   float pt() const { return m_pt; }
   float charge() const { return m_charge; }

   void dump() { printf("VSDCanidate %f\n", m_pt); }
};

/////////////////////////////////////////////////
class VSDJet : public VSDCandidate
{
// private:
public:
   float m_hadFraction{0.f};
   float m_coneR{0.2f}; // cone radius in eta phi space

public:
   float hadFraction() const { return m_hadFraction; }
   float coneR() const { return m_coneR; }

   //VSDJet(float pt, float eta, float phi, float had_fraction, float coneR = 0.2) : VSDCandidate(pt, eta, phi), m_hadFraction(had_fraction), m_coneR(coneR) {}

   using VSDBase::dump;
   void dump() { printf("VSDJet pt:%.2f, eta:%.2f, phi:%.2f / had_frac: %.2f\n", m_pt, m_eta, m_phi, m_hadFraction); }
};


/////////////////////////////////////////////////
class VSDMuon : public VSDCandidate
{
public:
   bool m_global{false};

   float global() const { return m_global; }

   // VSDMuon(float pt, float eta, float phi, int charge, bool global) : VSDCandidate(pt, eta, phi, charge), m_global(global) {}
};

////////////////////////////////////////////////
class VSDMET : public VSDCandidate
{
public:
  float m_sumEt{0.f};
public:
   // VSDMET(float pt, float eta, float phi, float sumEt) :  VSDCandidate(pt, eta, phi), m_sumEt(sumEt) {}
   float sumEt() { return m_sumEt; }
   void dump() { printf("VSDMET: phi: 2f, sumEt:%.2f / pt: %.2f\n", m_phi, m_sumEt); }
};

////////////////////////////////////////////////
struct VSDEventInfo : public VSDBase
{
   uint m_run{99};
   uint m_lumi{99};
   Long64_t m_event{99};
};
/////////////////////////////////////////////////
// Event structs
/////////////////////////////////////////////////

//typedef std::vector<VSDBase *> VSDCollection;

class VSDCollection
{
public:
   VSDCollection(const std::string& n, const std::string& p, Color_t c=kBlue, std::string f="") : 
                 m_name(n), m_purpose(p), m_color(c), m_filter(f) {}
   std::string m_name;
   std::string m_purpose;
   Color_t     m_color{kBlue};
   std::string m_filter;
   std::vector<VSDBase *> m_list;

   virtual void fill() {}
};


#endif // #ifdef VSDBase
