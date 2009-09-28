/*  DYNAMO:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
    Copyright (C) 2008  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 3 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "complex.hpp"
#include "../dynamics/interactions/intEvent.hpp"
#include "../simulation/particle.hpp"
#include "../dynamics/dynamics.hpp"
#include "../dynamics/liouvillean/liouvillean.hpp"
#include "../base/is_simdata.hpp"
#include "../base/is_base.hpp"
#include "../dynamics/systems/system.hpp"
#include <cmath> //for huge val
#include "../extcode/xmlParser.h"
#include "../dynamics/globals/global.hpp"
#include "../dynamics/globals/globEvent.hpp"
#include "../dynamics/globals/neighbourList.hpp"
#include "../dynamics/locals/local.hpp"
#include "../dynamics/locals/localEvent.hpp"
#include <boost/bind.hpp>
#include <boost/progress.hpp>

#include "complexentries/include.hpp"

void 
CSComplex::operator<<(const XMLNode& XML)
{
  sorter.set_ptr(CSSorter::getClass(XML.getChildNode("Sorter"), Sim));

  XMLNode xSubNode = XML.getChildNode("Entries");  
  for (long i=0; i < xSubNode.nChildNode("Entry"); i++)
    entries.push_back(smrtPlugPtr<CSCEntry>
		      (CSCEntry::getClass(xSubNode.getChildNode("Entry", i), Sim)));
}

void
CSComplex::initialise()
{
  I_cout() << "Reinitialising on collision " << Sim->lNColl;
  std::cout.flush();

  sorter->clear();
  //The plus one is because system events are stored in the last heap;
  sorter->resize(Sim->lN+1);
  eventCount.clear();
  eventCount.resize(Sim->lN+1, 0);

  //Now initialise the interactions
  {
    boost::progress_display prog(Sim->lN);
 
    BOOST_FOREACH(const CParticle& part, Sim->vParticleList)
      {
	addEventsInit(part);
	++prog;
      }
  }
  
  sorter->init();

  rebuildSystemEvents();
}

void 
CSComplex::outputXML(xmlw::XmlStream& XML) const
{
  XML << xmlw::attr("Type") << "Complex"
      << xmlw::tag("Sorter")
      << sorter
      << xmlw::endtag("Sorter")
      << xmlw::tag("Entries");
  
  BOOST_FOREACH(const smrtPlugPtr<CSCEntry>& ent,  entries)
    XML << xmlw::tag("Entry")
	<< ent
	<< xmlw::endtag("Entry");

  XML << xmlw::endtag("Entries");
}

CSComplex::CSComplex(const XMLNode& XML, 
				 DYNAMO::SimData* const Sim):
  CScheduler(Sim,"NeighbourListScheduler", NULL)
{ 
  I_cout() << "Neighbour List Scheduler Algorithmn Loaded";
  operator<<(XML);
}

CSComplex::CSComplex(DYNAMO::SimData* const Sim, CSSorter* ns):
  CScheduler(Sim,"NeighbourListScheduler", ns)
{ I_cout() << "Neighbour List Scheduler Algorithmn Loaded"; }

void 
CSComplex::addEvents(const CParticle& part)
{
  Sim->Dynamics.Liouvillean().updateParticle(part);
  
  //Add the global events
  BOOST_FOREACH(const smrtPlugPtr<CGlobal>& glob, Sim->Dynamics.getGlobals())
    if (glob->isInteraction(part))
      sorter->push(glob->getEvent(part), part.getID());
  
#ifdef DYNAMO_DEBUG
  if (dynamic_cast<const CGNeighbourList*>
      (Sim->Dynamics.getGlobals()[NBListID].get_ptr())
      == NULL)
    D_throw() << "Not a CGNeighbourList!";
#endif

//  //Grab a reference to the neighbour list
//  const CGNeighbourList& nblist(*static_cast<const CGNeighbourList*>
//				(Sim->Dynamics.getGlobals()[NBListID]
//				 .get_ptr()));
//  
//  //Add the local cell events
//  nblist.getParticleLocalNeighbourhood
//    (part, CGNeighbourList::getNBDelegate(&CScheduler::addLocalEvent, static_cast<const CScheduler*>(this)));
//
//  //Add the interaction events
//  nblist.getParticleNeighbourhood
//    (part, CGNeighbourList::getNBDelegate(&CScheduler::addInteractionEvent, static_cast<const CScheduler*>(this)));  
}

void 
CSComplex::addEventsInit(const CParticle& part)
{  
  Sim->Dynamics.Liouvillean().updateParticle(part);

  //Add the global events
  BOOST_FOREACH(const smrtPlugPtr<CGlobal>& glob, Sim->Dynamics.getGlobals())
    if (glob->isInteraction(part))
      sorter->push(glob->getEvent(part), part.getID());
  
#ifdef DYNAMO_DEBUG
  if (dynamic_cast<const CGNeighbourList*>
      (Sim->Dynamics.getGlobals()[NBListID].get_ptr())
      == NULL)
    D_throw() << "Not a CGNeighbourList!";
#endif

//  //Grab a reference to the neighbour list
//  const CGNeighbourList& nblist(*static_cast<const CGNeighbourList*>
//				(Sim->Dynamics.getGlobals()[NBListID]
//				 .get_ptr()));
//  
//  //Add the local cell events
//  nblist.getParticleLocalNeighbourhood
//    (part, CGNeighbourList::getNBDelegate
//     (&CScheduler::addLocalEvent, static_cast<const CScheduler*>(this)));
//
//  //Add the interaction events
//  nblist.getParticleNeighbourhood
//    (part, CGNeighbourList::getNBDelegate
//     (&CScheduler::addInteractionEventInit, static_cast<const CScheduler*>(this)));  
}
