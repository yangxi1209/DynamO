/*  dynamo:- Event driven molecular dynamics simulator 
    http://www.dynamomd.org
    Copyright (C) 2011  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

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

#pragma once
#include <dynamo/ranges/IDRange.hpp>
#include <dynamo/ranges/IDPairRange.hpp>
#include <magnet/xmlwriter.hpp>
#include <magnet/xmlreader.hpp>

namespace dynamo {
  class IDPairRangeSelf:public IDPairRange
  {
  public:
    IDPairRangeSelf(IDRange* r1):range(r1) {}
  
    IDPairRangeSelf(const magnet::xml::Node& XML, const dynamo::Simulation* Sim)
    { range = shared_ptr<IDRange>(IDRange::getClass(XML.getNode("IDRange"), Sim)); }

    virtual bool isInRange(const Particle&p1, const Particle&p2) const
    { return (p1.getID() == p2.getID()) && range->isInRange(p1); }

    virtual bool isInRange(const Particle&p1) const
    { return range->isInRange(p1); }

    const shared_ptr<IDRange>& getRange() const { return range; }

  protected:
    virtual void outputXML(magnet::xml::XmlStream& XML) const
    {
      XML << magnet::xml::attr("Type") << "Self"
	  << range;
    }

    shared_ptr<IDRange> range;
  };
}
