/*  DYNAMO:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
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

#include "fixedCollider.hpp"
#include <boost/foreach.hpp>
#include "../../base/is_simdata.hpp"
#include <magnet/xmlreader.hpp>
void 
SpFixedCollider::initialise()
{
  SpPoint::initialise();

  BOOST_FOREACH(size_t ID, *range)
    Sim->particleList[ID].clearState(Particle::DYNAMIC);
}

void 
SpFixedCollider::operator<<(const magnet::xml::Node& XML)
{
  range.set_ptr(CRange::getClass(XML, Sim));
  
  try {
    spName = XML.getAttribute("Name");
    intName = XML.getAttribute("IntName");
  } 
  catch (boost::bad_lexical_cast &)
    {
      M_throw() << "Failed a lexical cast in SpFixedCollider";
    }
}

void 
SpFixedCollider::outputXML(xml::XmlStream& XML) const
{
  XML << xml::attr("Name") << spName
      << xml::attr("IntName") << intName
      << xml::attr("Type") << "FixedCollider";
  
  XML << range;
}
