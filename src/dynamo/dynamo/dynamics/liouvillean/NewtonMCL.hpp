/*  dynamo:- Event driven molecular dynamics simulator 
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

#pragma once
#include <dynamo/dynamics/liouvillean/NewtonL.hpp>
#include <boost/unordered_map.hpp>

namespace dynamo {
  /*! \brief A Liouvillean which implements Newtonian dynamics, but with a deformed energy landscape.
   * 
   * This Liouvillean is designed for performing Multi-Canonical
   * simulations. A descriptive paper on the technique is
   * "Multicanonical Ensemble Generated by Molecular Dynamics Simulation
   * for Enhanced Conformational Sampling of Peptides"
   * (doi:10.1021/jp962142e).
   *
   * The following notes will be based around the above paper.
   *
   * If we are in the canonical ensemble, the probability of being in a
   * certain internal energy is
   *
   * \f[ P_c (E) = \frac{1}{Z_{c}} n(E) \exp\left[-E/k_BT\right] \f]
   *
   * But for efficient sampling of all energies we would prefer it if
   * the probability of each energy is constant and equal.
   *
   * \f[ P_{mc} (E) = \frac{1}{Z_{mc}} n(E) \exp\left[-E/k_BT-W(E)\right] = \textrm{constant} \f]
   *
   * This is the ideal multi-canonical simulation, as we sample all
   * energies equally. However, to perform the ideal multi-canonical
   * simulation we need to know the \f$W(E)\f$ function.
   *
   * \f[ W(E) = \ln n(E) - E / k_B T = \ln P_c(E) \f]
   *
   * But this requires knowing the density of states (\f$n(E)\f$), but
   * if this quantity is known then the system is already solved. So
   * what we need is an iterative way to determine the \f$W(E)\f$
   * function. Essentially, we can run a simulation with \f$W^{(0)}(E)=0\f$,
   * and then iterate towards the optimal weighting with this:
   *
   * \f[ W^{(i+1)}(E) = W^{(i)}(E) + \ln P^{(i)}_{mc}(E) \f]
   *
   */
  class LNewtonianMC: public LNewtonian
  {
  public:
    LNewtonianMC(dynamo::SimData* tmp, const magnet::xml::Node&);

    //Pair particle dynamics
    virtual PairEventData SphereWellEvent(const IntEvent&, const double&, 
					  const double&) const;
    virtual NEventData multibdyWellEvent(const Range&, const Range&, 
					 const double&, const double&, 
					 EEventType&) const;

    virtual void initialise();

    /*! \brief Returns the \f$W(E)\f$ function.
     *
     * The lookup is performed by taking the system energy, E, and
     * calculating the key like so:
     *
     * \f[\textrm{key}= \textrm{int}\left[E / \Delta E\right]\f]
     *
     * where \f$ \Delta E\f$ is the energy step returned from
     * getEnergyStep().
     */
    inline const boost::unordered_map<int, double>& getMap() const { return _W; }

    /*! \brief Returns \f$ \Delta E\f$.
     *  \sa getMap()
     */
    inline const double& getEnergyStep() const { return EnergyPotentialStep; }

    /*! \brief Returns \f$ W(E)\f$.
     */
    inline double W(double E) const 
    { 
      boost::unordered_map<int, double>::const_iterator 
	iPtr = _W.find(lrint(E / EnergyPotentialStep));
      if (iPtr != _W.end())
	return iPtr->second;
      return 0;
    }

    virtual void swapSystem(Liouvillean& oLiouvillean);

  protected:
    virtual void outputXML(magnet::xml::XmlStream& ) const;

    boost::unordered_map<int, double> _W; 

    double EnergyPotentialStep;
  
  };
}
