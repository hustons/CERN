// $Id: MC.cpp, 14/04/2011 rhuston Exp $

//#####################//
//### Include files ###//
//#####################// 

//### from Gaudi ###//
#include "GaudiKernel/AlgFactory.h" 
//### local ###//
#include "MC.h"
#include "Extra.h" // SHUST: This is going to be used for new IP calculation but will need to test first!
//### MC ###//
#include "Event/MCParticle.h"
//### Determine if MC reconstructible ###//
#include "MCInterfaces/IMCReconstructible.h"
//### Extract L0 Decision ###//
#include "Event/L0DUReport.h"
//### Extract HLT decision ###//
#include "HltDecReports.h"

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : MC
//
// 14-04-2011 : Shane Huston
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MC );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
// Initialise variables defined in header file
MC::MC( const std::string& name, ISvcLocator* pSvcLocator)
  : DaVinciTupleAlgorithm ( name , pSvcLocator ),
    m_errorCode(-1234567),
    m_coneMax(1),
    m_numDivs(2),//5
    m_pi(3.1415926535897932384626434)
{
  m_local = pSvcLocator;
}

//=============================================================================
// Destructor
//=============================================================================
MC::~MC() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MC::initialize() {
  StatusCode sc = DaVinciTupleAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MC::execute() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;
  StatusCode sc = StatusCode::SUCCESS ;
  err() << "Execute phase of MC.cpp has been called." << endmsg; // Execute phase should not be called 
  return StatusCode::SUCCESS;
}

//=============================================================================
// Get Invariant Mass of two MC Daughters
//=============================================================================
StatusCode MC::invMassMC(LHCb::MCParticle::Vector& DaMinus, LHCb::MCParticle::Vector& DaPlus, LHCb::MCParticle::Vector mcparts,
                         Tuple mcTuple, const std::string& head)
{
  StatusCode sc = StatusCode::SUCCESS;
  
  for ( unsigned int i = 0; i<DaPlus.size(); ++i )
  {
    //##############################################//
    //### Check the trigger lines for this event (May need to update to include other lines?) ###//
    //##############################################//
    int L0GlobalDecision=m_errorCode, L0MuonDecision=m_errorCode, L0MuonHighDecision=m_errorCode, L0DiMuonDecision=m_errorCode;
    int Hlt1GlobalDecision=m_errorCode, Hlt2GlobalDecision=m_errorCode, Hlt1SingleMuonNoIPL0HighPTDecision = m_errorCode, 
      Hlt2ZmmDecision=m_errorCode;
    
    //### Get L0 decision ###//
    if ( exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) )
    {
      const LHCb::L0DUReport* L0Rep = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
      /*unsigned int channelID=0;
        std::string channelName = ""
        while (1>0) {
        channelName = L0Rep->channelName(channelID);
        info() << "Channel Name is " << channelName << endmsg;
        if (channelName.size()>20) break;
        if (L0Rep->channelDecisionByName(channelName)) 
          info()<<"Trigger "<<channelName<<", "<<L0Rep->channelDecisionByName(channelName)<<endmsg;
        channelID++; }
      */
      //### NOTE: There are some other lines to consider such as "MUON,minbias", "Muon,lowMult", "DiMuon,lowMult" ###//
      L0GlobalDecision   = L0Rep->decision();
      L0MuonDecision     = L0Rep->channelDecisionByName("Muon");
      L0MuonHighDecision = L0Rep->channelDecisionByName("MuonHigh");
      L0DiMuonDecision   = L0Rep->channelDecisionByName("DiMuon");
    }
    else Warning("Can't get LHCb::L0DUReportLocation::Default (" + LHCb::L0DUReportLocation::Default + ")" );

    //### Get HLT decision ###//
    if ( exist<LHCb::HltDecReports>(LHCb::HltDecReportsLocation::Default) )
    {
      const LHCb::HltDecReports* HltReps = get<LHCb::HltDecReports>( LHCb::HltDecReportsLocation::Default );
      //std::vector< std::string > decNames = HltReps->decisionNames();
      //for (int i=0; i<decNames.size(); i++) info() << "Decision Name: " << decNames[i] << endmsg;
      if (HltReps->decReport("Hlt1Global")) Hlt1GlobalDecision = HltReps->decReport("Hlt1Global")->decision();
      if (HltReps->decReport("Hlt2Global")) Hlt2GlobalDecision = HltReps->decReport("Hlt2Global")->decision();
      //### Currently only set up for MC. ALso, consider the line with no HighPt requirement ###//
      if (HltReps->decReport("Hlt1SingleMuonNoIPL0HighPTDecision"))
        Hlt1SingleMuonNoIPL0HighPTDecision = HltReps->decReport("Hlt1SingleMuonNoIPL0HighPTDecision")->decision();
      //### TEST - Not sure if this is the right/only Z2MuMu line to consider! ###//
      if (HltReps->decReport("Hlt2DiMuonUnbiasedZmmDecision"))
        Hlt2ZmmDecision = HltReps->decReport("Hlt2DiMuonUnbiasedZmmDecision")->decision();
    }
    else Warning("No HltDecReports at " + LHCb::HltDecReportsLocation::Default + "buddy.", StatusCode::FAILURE, 1);
    
    mcTuple->column("L0GlobalDecision",                   L0GlobalDecision);
    mcTuple->column("L0MuonDecision",                     L0MuonDecision);
    mcTuple->column("L0MuonHighDecision",                 L0MuonHighDecision);
    mcTuple->column("L0DiMuonDecision",                   L0DiMuonDecision);
    mcTuple->column("Hlt1GlobalDecision",                 Hlt1GlobalDecision);
    mcTuple->column("Hlt2GlobalDecision",                 Hlt2GlobalDecision);
    mcTuple->column("Hlt1SingleMuonNoIPL0HighPTDecision", Hlt1SingleMuonNoIPL0HighPTDecision);
    mcTuple->column("Hlt2ZmmDecision",                    Hlt2ZmmDecision);
    
    //### Plot some quantites for 1st daughter ###//
    LHCb::MCParticle *DaP = DaPlus[i];
    //const LHCb::MCParticle *motherP = DaP->mother();
    sc = plotMC(DaP, DaP->mother(), mcparts, mcTuple, head + "1");
        
    for ( unsigned int j = 0; j<DaMinus.size(); ++j)
    {
      LHCb::MCParticle *DaM = DaMinus[j];
      //const LHCb::MCParticle *motherM = DaM->mother();
      //### Finish plotting quantites for 1st daughter ###//
      Gaudi::LorentzVector twoDa=DaP->momentum()+DaM->momentum();
      mcTuple->column(head + "1InvMass", twoDa.M());
      
      //### Plot quantities for 2nd daughter ###//
      sc = plotMC(DaM, DaM->mother(), mcparts, mcTuple, head + "2");
      mcTuple->column(head + "2InvMass", twoDa.M());
      
      IMCReconstructible* recoTool = tool<IMCReconstructible>("MCReconstructible");
      if (recoTool->reconstructible(DaM)==2) counter("MuMinusReconstructible")++;
      if (recoTool->reconstructible(DaP)==2) counter("MuPlusReconstructible")++;
      if ((recoTool->reconstructible(DaM)==2)||(recoTool->reconstructible(DaP)==2)) counter("OneMuonReconstructible")++;
      if ((recoTool->reconstructible(DaM)==2)&&(recoTool->reconstructible(DaP)==2)) counter("DiMuonReconstructible")++;
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Plot MC data
//=============================================================================
StatusCode MC::plotMC(const LHCb::MCParticle* part, const LHCb::MCParticle* mother, LHCb::MCParticle::Vector mcparts,
                            Tuple mcTuple, const std::string& head)
{
  StatusCode sc = StatusCode::SUCCESS;
  
  IMCReconstructible* recoTool = tool<IMCReconstructible>("MCReconstructible");
  if ((recoTool->reconstructible(part)!=0)&&(recoTool->reconstructible(part)!=1)&&(recoTool->reconstructible(part)!=2)
      &&(recoTool->reconstructible(part)!=3)&&(recoTool->reconstructible(part)!=4)&&(recoTool->reconstructible(part)!=5)
      &&(recoTool->reconstructible(part)!=6))
  {
    info() << "MCParticle is a " << part->particleID().pid() << endmsg;
    info() << "MCParticle reconstructible..." << recoTool->text(recoTool->reconstructible(part)) << endmsg;
    info() << "MCParticle reconstructible..." << recoTool->reconstructible(part) << endmsg;
  }
    
  mcTuple->column(head + "ParticleID",         part->particleID().pid());
  mcTuple->column(head + "Mass",               part->momentum().M());
  mcTuple->column(head + "P",                  part->p());
  mcTuple->column(head + "Pt",                 part->pt());
  mcTuple->column(head + "E",                  part->momentum().E());
  mcTuple->column(head + "Phi",                part->momentum().phi());
  mcTuple->column(head + "Eta",                part->momentum().Eta());
  mcTuple->column(head + "MotherID",           mother->particleID().pid());
  if (mother->mother() != NULL)
  {
    const LHCb::MCParticle *gMother = mother->mother();
    mcTuple->column(head + "GrandMotherID",    gMother->particleID().pid());
  }
  else mcTuple->column(head + "GrandMotherID", m_errorCode);
  mcTuple->column(head + "Reco",               recoTool->reconstructible(part));
    
  //### Calculate Impact Parameter ###//
  //### Make LHCb:Particle from LHCb:MCParticle so that distanceCalculator can be used ###//
  const LHCb::MCVertex *vert = part->primaryVertex(), *vertRef = part->originVertex();
  Gaudi::XYZPoint point = vert->position(), pointRef = vertRef->position();
  
  //LHCb::Particle ipPart = LHCb::Particle::Particle(part->particleID());
  LHCb::Particle ipPart = LHCb::Particle(part->particleID());
  
  ipPart.setMeasuredMass(part->momentum().M());
  ipPart.setMomentum(part->momentum());
  ipPart.setReferencePoint(pointRef);
  
  mcTuple->column(head + "RefPoint",           ipPart.referencePoint().Z());
  
  double mcIP, mcIPE;
  if (msgLevel(MSG::DEBUG)) debug() << vert->position() << endmsg;

  IDistanceCalculator* distTool = tool<IDistanceCalculator>("LoKi::DistanceCalculator");

  // SHUST: New way to calculate IP with sign (needs to be tested)
  const std::string& strExtra = "Extra";
  Extra* extra = new Extra(strExtra, m_local);
  sc = extra->SignedImpactParameter(&ipPart, point, mcIP, mcIPE);
  delete extra;

  sc = distTool->distance(&ipPart, point, mcIP, mcIPE); // Old way to calculate IP. Remove when ready
  
  mcTuple->column(head + "IP",                 mcIP);
  mcTuple->column(head + "IPchi2",             mcIPE);
    
  //##########################//
  //### Isolation Criteria ###//
  //##########################//
  
  //### Identify different cone sizes based on m_coneMax ###//
  double coneDivs = double(m_coneMax)/m_numDivs;
  double coneSize[m_numDivs];
  
  for (int i = 0; i<m_numDivs; i++) coneSize[i] = m_coneMax - (i * coneDivs);
  
  //### Define quantities to be recorded in tuple and set all values to zero ###//
  int num_tracks[m_numDivs];
  double dist_PhiEta = 0;
  Gaudi::LorentzVector ConeP[m_numDivs];
  
  for (int i=0; i<m_numDivs; i++)
  {
    num_tracks[i] = 0;
    ConeP[i] = Gaudi::LorentzVector(0,0,0,0);
  }

  //### TEST - Loop on all MCParticles and keep only those with no daughters             ###//
  //### No daughters is defined as: (a) No end vertex (b) End vertices with no daughters ###//
  int loop;                           // Used to count number of daughters in an endVertex
  LHCb::MCParticle::Vector mcStable;  // Store particles satisfying the criteria
  for ( LHCb::MCParticle::Vector::iterator it = mcparts.begin(); it != mcparts.end(); ++it )
  {
    loop = 0;
    LHCb::MCParticle* isStable = *it;
    int t_id = abs(isStable->particleID().pid());
    const SmartRefVector<LHCb::MCVertex> endVert = isStable->endVertices(); // Get reference to any end vertices
    debug() << "Number of end vertices  = " << endVert.size() << endmsg;
    if (t_id==11||t_id==13||t_id==22||t_id==321||t_id==211||t_id==2112||t_id==2212) mcStable.push_back(isStable);
    /*
      if (endVert.size()!=0 )//&&(t_id==11||t_id==12||t_id==13||t_id==14||t_id==22||t_id==321||t_id==211||t_id==2112||t_id==2212))
      {
      debug() << t_id << " may not be a stable particle.";
      for (int j=0; j<endVert.size(); j++)
      {
      const SmartRefVector<LHCb::MCParticle> daughters = endVert[j]->products();
      // Extract any decay products
      for (int i=0; i<daughters.size(); i++)
      {
      const LHCb::MCParticle* other = daughters[i];
      debug() << " Another particle in this decay chain: " << other->particleID().pid();
      loop++;   // Flag if there are decay products
      }
      }
      debug() << endmsg;
      if (loop==0)
      {
      debug() << "Couldn't find any daughters for " << t_id << " (but could find at least one end vertex)" << endmsg;
      mcStable.push_back(isStable);  // Store particle in vector
      }
      }
      else if (endVert.size()==0) mcStable.push_back(isStable); // store particle in vector
    */
  }
  debug() << "Number of 'stable' MCParticles in this event is " << mcStable.size()
          << " out of " << mcparts.size() << " MCParticles." << endmsg;
    
  //### Loop on other MCParticles ###//
  //for ( LHCb::MCParticle::Vector::iterator it = mcparts.begin(); it != mcparts.end(); ++it )
  //### Test - Loop on 'stable' MCParticles instead ###//
  for ( LHCb::MCParticle::Vector::iterator it = mcStable.begin(); it != mcStable.end(); ++it )
  {
    LHCb::MCParticle* prt = *it;
      
    //### Calculate the distance in Phi-Eta space ###//
    dist_PhiEta = fabs(part->momentum().phi() - prt->momentum().phi());
    if ( dist_PhiEta > m_pi) dist_PhiEta = fabs(dist_PhiEta - 2*m_pi);
    dist_PhiEta = sqrt(pow(dist_PhiEta,2) + pow(part->momentum().Eta() - prt->momentum().Eta(), 2));
    debug() << "Distance in Phi-Eta space from this track to the Muon is " << dist_PhiEta << endmsg;
      
    //### Update variables if MCParticle is within cone but is not the MC Muon itself! ###//
    for (int i=0; i<m_numDivs; i++)
    {
      if (dist_PhiEta < coneSize[i] && prt != part)
      {
        debug() << "### Distance in Phi-Eta space from this track to the Muon is " << dist_PhiEta << endmsg;
        num_tracks[i]++;
        ConeP[i] += prt->momentum();
      }
    }
    
    //### Plot data for tracks inside cones ###//
    char num [] = "123456789";
    for (int i=0; i<m_numDivs; i++)
    {
      mcTuple->column(head + "MCParts_" + num[i] + "_ConeSize", coneSize[i]);
      mcTuple->column(head + "MCParts_" + num[i] + "_Num",      num_tracks[i]);
      mcTuple->column(head + "MCParts_" + num[i] + "_E",        ConeP[i].E());
      mcTuple->column(head + "MCParts_" + num[i] + "_P",        ConeP[i].R());
      mcTuple->column(head + "MCParts_" + num[i] + "_Pt",       ConeP[i].R()*sin(ConeP[i].theta()));
      mcTuple->column(head + "MCParts_" + num[i] + "_Phi",      ConeP[i].phi());
      mcTuple->column(head + "MCParts_" + num[i] + "_Eta",      ConeP[i].Eta());
    }
  }
  
  return StatusCode::SUCCESS;
}
  
//=============================================================================
//  Finalize
//=============================================================================
StatusCode MC::finalize() {
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return DaVinciTupleAlgorithm::finalize(); 
} 
//=============================================================================
