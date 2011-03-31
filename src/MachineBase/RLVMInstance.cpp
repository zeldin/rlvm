// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2011 Elliot Glaysher
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
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// -----------------------------------------------------------------------

#include "MachineBase/RLVMInstance.hpp"

#include <iostream>

#include "MachineBase/GameHacks.hpp"
#include "MachineBase/RLMachine.hpp"
#include "MachineBase/Serialization.hpp"
#include "Modules/Modules.hpp"
#include "Modules/Module_Sys_Save.hpp"
#include "Systems/Base/SystemError.hpp"
#include "Systems/SDL/SDLSystem.hpp"
#include "Utilities/Exception.hpp"
#include "Utilities/File.hpp"
#include "Utilities/findFontFile.h"
#include "libReallive/gameexe.h"
#include "libReallive/reallive.h"

using namespace std;

namespace fs = boost::filesystem;

RLVMInstance::RLVMInstance()
    : seen_start_(-1),
      memory_(false),
      undefined_opcodes_(false),
      count_undefined_copcodes_(false),
      load_save_(-1) {
}

RLVMInstance::~RLVMInstance() {}

void RLVMInstance::Run(const boost::filesystem::path& gamerootPath) {
  try {
    fs::path gameexePath = correctPathCase(gamerootPath / "Gameexe.ini");
    if (gameexePath.empty()) {
      ostringstream oss;
      oss << "Could not open " << gameexePath
          << ". Please make sure it exists.";
      throw rlvm::UserPresentableError("Could not load game", oss.str());
    }

    fs::path seenPath = correctPathCase(gamerootPath / "Seen.txt");
    if (seenPath.empty()) {
      ostringstream oss;
      oss << "Could not open " << seenPath
          << ". Please make sure it exists.";
      throw rlvm::UserPresentableError("Could not load game", oss.str());
    }

    Gameexe gameexe(gameexePath);
    gameexe("__GAMEPATH") = gamerootPath.file_string();

    // Possibly force starting at a different seen
    if (seen_start_ != -1)
      gameexe("SEEN_START") = seen_start_;

    if (memory_)
      gameexe("MEMORY") = 1;

    SDLSystem sdlSystem(gameexe);
    sdlSystem.setPlatform(BuildNativePlatform(sdlSystem));

    libReallive::Archive arc(seenPath.file_string(), gameexe("REGNAME"));
    RLMachine rlmachine(sdlSystem, arc);
    addAllModules(rlmachine);
    addGameHacks(rlmachine);

    // Validate our font file
    // TODO(erg): Remove this when we switch to native font selection dialogs.
    fs::path fontFile = findFontFile(sdlSystem);
    if (fontFile.empty() || !fs::exists(fontFile)) {
      cerr << "Could not open font file. Please either: " << endl
           << endl
           << "1) Place a copy of msgothic.ttc in your home directory." << endl
           << "2) Place a copy of msgothic.ttc in \""
           << gamerootPath.file_string() << "\"" << endl
           << "3) Specify an alternate font with the --font option." << endl;
      return;
    }

    if (undefined_opcodes_)
      rlmachine.setPrintUndefinedOpcodes(true);

    if (count_undefined_copcodes_)
      rlmachine.recordUndefinedOpcodeCounts();

    Serialization::loadGlobalMemory(rlmachine);
    rlmachine.setHaltOnException(false);

    if (load_save_ != -1)
      Sys_load()(rlmachine, load_save_);

    while (!rlmachine.halted()) {
      // Give SDL a chance to respond to events, redraw the screen,
      // etc.
      sdlSystem.run(rlmachine);

      // Run the rlmachine through another instruction
      rlmachine.executeNextInstruction();

      // Maybe process native events that we don't otherwise care about.
      DoNativeWork();
    }

    Serialization::saveGlobalMemory(rlmachine);
  } catch (rlvm::UserPresentableError& e) {
    ReportFatalError(e.message_text(), e.informative_text());
  } catch (rlvm::Exception& e) {
    ReportFatalError("Fatal RLVM error", e.what());
  } catch (libReallive::Error& e) {
    ReportFatalError("Fatal libReallive error", e.what());
  } catch (SystemError& e) {
    ReportFatalError("Fatal local system error", e.what());
  } catch (std::exception& e) {
    ReportFatalError("Uncaught exception", e.what());
  } catch (const char* e) {
    ReportFatalError("Uncaught exception", e);
  }
}

boost::filesystem::path RLVMInstance::SelectGameDirectory() {
  return boost::filesystem::path();
}

void RLVMInstance::ReportFatalError(const std::string& message_text,
                                    const std::string& informative_text) {
  cerr << message_text << ": " << informative_text << endl;
}

Platform* RLVMInstance::BuildNativePlatform(System& system) {
  return NULL;
}
