// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2007 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
//
// -----------------------------------------------------------------------

#ifndef SRC_MACHINEBASE_RLOPERATION_SPECIAL_T_HPP_
#define SRC_MACHINEBASE_RLOPERATION_SPECIAL_T_HPP_

#include <string>
#include <vector>

#include <boost/ptr_container/ptr_vector.hpp>

#include "Utilities/Exception.hpp"
#include "libReallive/bytecode_fwd.h"
#include "libReallive/expression.h"
#include "libReallive/expression_pieces.h"

#include <sstream>

// In bytecode, special tags can be any char, though they're usually
// 0. Starting in at least Little Busters, some commands use a new format where
// there are multiple tags, which require a specialized mapping between the tag
// and the internal parameter thing below.
//
// This default mapper is used by most Special_Ts, and returns the tag as the
// type.
struct DefaultSpecialMapper {
  static int GetTypeForTag(const libReallive::SpecialExpressionPiece& sp) {
    return sp.getOverloadTag();
  }
};

// Type definition that implements the special parameter concept; the
// way to expect multiple different types in a parameter slot.
template<typename Mapper,
         typename A, typename B = Empty_T, typename C = Empty_T,
         typename D = Empty_T, typename E = Empty_T,
         typename F = Empty_T, typename G = Empty_T,
         typename H = Empty_T, typename I = Empty_T>
struct Special_T {
  // Internal unionish structure which we pass in to the
  struct Parameter {
    // 0 = A, 1 = B
    int type;

    typename A::type first;
    typename B::type second;
    typename C::type third;
    typename D::type fourth;
    typename E::type fifth;
    typename F::type sixth;
    typename G::type seventh;
    typename H::type eighth;
    typename I::type ninth;
  };

  // Export our internal struct as our external type
  typedef Parameter type;

  // Special<Complex, Complex, ...> requires a special construct...
  template<typename TYPE>
  static typename TYPE::type getDataFor(
    RLMachine& machine,
    const boost::ptr_vector<libReallive::ExpressionPiece>& p,
    unsigned int& position,
    const libReallive::SpecialExpressionPiece& sp) {
    if (TYPE::is_complex) {
      return TYPE::getData(machine, p, position);
    } else {
      unsigned int contained_position = 0;
      position++;
      return TYPE::getData(machine, sp.getContainedPieces(),
                           contained_position);
    }
  }

  // Convert the incoming parameter objects into the resulting type.
  static type getData(RLMachine& machine,
                      const boost::ptr_vector<libReallive::ExpressionPiece>& p,
                      unsigned int& position) {
    if (position >= p.size()) {
      std::ostringstream oss;
      oss << "Illegal position in Special_T: " << position << " (Size of p: "
          << p.size() << ")";
      throw std::runtime_error(oss.str());
    }

    const libReallive::SpecialExpressionPiece& sp =
      static_cast<const libReallive::SpecialExpressionPiece&>(p[position]);

    if (sp.getContainedPieces().size() == 0)
      throw rlvm::Exception("Empty special construct in Special_T");

    Parameter par;
    par.type = Mapper::GetTypeForTag(sp);
    switch (par.type) {
      case 0:
        par.first = getDataFor<A>(machine, p, position, sp);
        break;
      case 1:
        par.second = getDataFor<B>(machine, p, position, sp);
        break;
      case 2:
        par.third = getDataFor<C>(machine, p, position, sp);
        break;
      case 3:
        par.fourth = getDataFor<D>(machine, p, position, sp);
        break;
      case 4:
        par.fifth = getDataFor<E>(machine, p, position, sp);
        break;
      case 5:
        par.sixth = getDataFor<F>(machine, p, position, sp);
        break;
      case 6:
        par.seventh = getDataFor<G>(machine, p, position, sp);
        break;
      case 7:
        par.eighth = getDataFor<H>(machine, p, position, sp);
        break;
      case 8:
        par.ninth = getDataFor<I>(machine, p, position, sp);
        break;
      default: {
        std::ostringstream oss;
        oss << "Illegal overload in Special_T::getData(). Bytecode tag was "
            << sp.getOverloadTag() << ", Mapped position was "
            << par.type;
        throw rlvm::Exception(oss.str());
      }
    };

    return par;
  }

  static void parseParameters(
      unsigned int& position,
      const std::vector<std::string>& input,
      boost::ptr_vector<libReallive::ExpressionPiece>& output) {
    const char* data = input.at(position).c_str();
    std::auto_ptr<libReallive::ExpressionPiece> ep(libReallive::get_data(data));
    output.push_back(ep.release());
    position++;
  }

  enum {
    is_complex = false
  };
};

#endif  // SRC_MACHINEBASE_RLOPERATION_SPECIAL_T_HPP_
