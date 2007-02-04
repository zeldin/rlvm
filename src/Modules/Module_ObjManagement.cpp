// This file is part of RLVM, a RealLive virutal machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 El Riot
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
// -----------------------------------------------------------------------

/**
 * @file   Module_ObjManagement.cpp
 * @author Elliot Glaysher
 * @date   Wed Jan 31 18:33:08 2007
 * 
 * @brief  Modules 60, 61, and 62. Misc. object management.
 */

#include "Module_Obj.hpp"
#include "Module_ObjManagement.hpp"

#include "MachineBase/RLOperation.hpp"
#include "MachineBase/RLModule.hpp"
#include "Systems/Base/System.hpp"
#include "Systems/Base/GraphicsSystem.hpp"
#include "Systems/Base/GraphicsObject.hpp"

// -----------------------------------------------------------------------

template<typename FROMLAYER, typename TOLAYER>
struct Obj_objCopy : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  void operator()(RLMachine& machine, int sbuf, int dbuf) {
    GraphicsObject& go = FROMLAYER::get(machine, sbuf);
    TOLAYER::set(machine, dbuf, go);
  }
};

// -----------------------------------------------------------------------

template<typename LAYER>
struct Obj_objClear_0 : public RLOp_Void_1<IntConstant_T> {
  void operator()(RLMachine& machine, int buf) {
    LAYER::get(machine, buf).clearObject();
  }
};

// -----------------------------------------------------------------------

template<typename LAYER>
struct Obj_objClear_1 : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  void operator()(RLMachine& machine, int min, int max) {
    // Inclusive ranges make baby Kerrigan and Ritchie cry.
    max++;

    for(int i = min; i < max; ++i) {
      LAYER::get(machine, i).clearObject();
    }
  }
};

// -----------------------------------------------------------------------

template<typename LAYER>
struct Obj_objDelete_0 : public RLOp_Void_1<IntConstant_T> {
  void operator()(RLMachine& machine, int buf) {
    LAYER::get(machine, buf).deleteObject();
  }
};

// -----------------------------------------------------------------------

template<typename LAYER>
struct Obj_objDelete_1 : public RLOp_Void_2<IntConstant_T, IntConstant_T> {
  void operator()(RLMachine& machine, int min, int max) {
    // Inclusive ranges make baby Kerrigan and Ritchie cry.
    max++;

    for(int i = min; i < max; ++i) {
      LAYER::get(machine, i).deleteObject();
    }
  }
};

// -----------------------------------------------------------------------

ObjFgManagement::ObjFgManagement()
  : RLModule("ObjFgManagement", 1, 61)
{
  addOpcode(2, 0, "objCopy", new Obj_objCopy<FG_LAYER, FG_LAYER>);
  addOpcode(3, 0, "objCopyToBg", new Obj_objCopy<FG_LAYER, BG_LAYER>);

  addOpcode(10, 0, "objClear", new Obj_objClear_0<FG_LAYER>);
  addOpcode(10, 1, "objClear", new Obj_objClear_1<FG_LAYER>);
  addOpcode(11, 0, "objDelete", new Obj_objDelete_0<FG_LAYER>);
  addOpcode(11, 1, "objDelete", new Obj_objDelete_1<FG_LAYER>);
}

// -----------------------------------------------------------------------

ObjBgManagement::ObjBgManagement()
  : RLModule("ObjBgManagement", 1, 62)
{
  addOpcode(2, 0, "objBgCopyToFg", new Obj_objCopy<BG_LAYER, FG_LAYER>);
  addOpcode(3, 0, "objBgCopy", new Obj_objCopy<BG_LAYER, BG_LAYER>);

  addOpcode(10, 0, "objBgClear", new Obj_objClear_0<FG_LAYER>);
  addOpcode(10, 1, "objBgClear", new Obj_objClear_1<FG_LAYER>);
  addOpcode(11, 0, "objBgDelete", new Obj_objDelete_0<BG_LAYER>);
  addOpcode(11, 1, "objBgDelete", new Obj_objDelete_1<BG_LAYER>);
}
