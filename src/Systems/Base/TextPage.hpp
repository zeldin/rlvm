// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
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
//
// -----------------------------------------------------------------------

#ifndef SRC_SYSTEMS_BASE_TEXTPAGE_HPP_
#define SRC_SYSTEMS_BASE_TEXTPAGE_HPP_

#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/function.hpp>

#include <algorithm>
#include <string>
#include <vector>

class TextPageElement;
class SetWindowTextPageElement;
class System;
class TextTextPageElement;

// A sequence of replayable commands that write to or modify a window, such as
// displaying characters and changing font information.
//
// The majority of public methods in TextPage simply call the private versions
// of these methods, and add the appropriate TextPageElement to this page's
// back log for replay.
class TextPage : public boost::noncopyable {
 public:
  TextPage(System& system, int window_num);
  TextPage(const TextPage& rhs);
  ~TextPage();

  TextPage& operator=(const TextPage& rhs);
  void swap(TextPage& rhs);

  // Replays every recordable action called on this TextPage.
  void replay(bool is_active_page);

  // Returns the number of characters printed with character() and name().
  int numberOfCharsOnPage() const { return number_of_chars_on_page_; }

  // Add this character to the most recent text render operation on
  // this page's backlog, and then render it, minding the kinsoku
  // spacing rules.
  bool character(const std::string& current, const std::string& rest);

  // Displays a name. This function will be called by the
  // TextoutLongOperation.
  void name(const std::string& name, const std::string& next_char);

  // Forces a hard line brake.
  void hardBrake();

  // Sets the indentation to the x part of the current insertion point.
  void setIndentation();

  // Resets the indentation.
  void resetIndentation();

  // Sets the text foreground to the colour passed in, up until the
  // next pause().
  void fontColour(const int colour);

  // Changes the size of text.
  void defaultFontSize();
  void fontSize(const int size);

  // Marks the current character as the beginning of a phrase that has
  // rubytext over it.
  void markRubyBegin();

  // Display the incoming phrase as the rubytext for the text since
  // markRubyBegin() was called.
  void displayRubyText(const std::string& utf8str);

  void setInsertionPointX(int x);
  void setInsertionPointY(int y);
  void offsetInsertionPointX(int offset);
  void offsetInsertionPointY(int offset);

  // Sets the face in slot |index| to filename.
  void faceOpen(const std::string& filename, int index);

  // Removes the face in slot |index|.
  void faceClose(int index);

  // This is a hack to get the backlog colour working. This adds a
  // SetToRightStartingColorElement element to the TextPage, which, on
  // replay, simply checks to see if we're redisplaying a backlog
  // page and sets the colour to the backlog colour if we are.
  void addSetToRightStartingColorElement();

  // Queries the corresponding TextWindow to see if it is full. Used
  // to implement implicit pauses when a page is full.
  bool isFull() const;

  // Queries to see if there has been an invocation of
  // markRubyBegin(), but not the closing displayRubyText().
  bool inRubyGloss() const { return in_ruby_gloss_; }

 private:
  // All subclasses of TextPageElement are friends of TextPage for
  // tight coupling.
  friend class TextPageElement;
  friend class TextTextPageElement;

  // Performs the passed in action and then adds it to |elements_to_replay_|.
  void addAction(const boost::function<void(TextPage&, bool)>& action);

  boost::ptr_vector<TextPageElement> elements_to_replay_;

  System* system_;

  // Current window that this page is rendering into
  int window_num_;

  // Number of characters on this page (used in automode)
  int number_of_chars_on_page_;

  // Whether markRubyBegin() was called but displayRubyText() hasn't yet been
  // called.
  bool in_ruby_gloss_;

  /**
   * @name Private implementations
   *
   * These methods are what actually does things. They output to the
   * screen, etc.
   *
   * @{
   */

  bool character_impl(const std::string& c, const std::string& rest);

  void name_impl(const std::string& name, const std::string& next_char,
                 bool is_active_page);

  void hard_brake_impl(bool is_active_page);

  void set_indentation_impl(bool is_active_page);

  void reset_indentation_impl(bool is_active_page);

  void font_colour_impl(const int colour, bool is_active_page);
  void default_font_size_impl(bool is_active_page);
  void font_size_impl(int size, bool is_active_page);
  void mark_ruby_begin_impl(bool is_active_page);

  void display_ruby_text_impl(const std::string& utf8str, bool is_active_page);

  void set_insertion_point_x_impl(int x, bool is_active_page);
  void set_insertion_point_y_impl(int y, bool is_active_page);
  void offset_insertion_point_x_impl(int offset, bool is_active_page);
  void offset_insertion_point_y_impl(int offset, bool is_active_page);

  void faceOpenImpl(std::string filename, int index, bool is_active_page);
  void faceCloseImpl(int index, bool is_active_page);

  void set_to_right_starting_colour_impl(bool is_active_page);
  /// @}
};

#endif  // SRC_SYSTEMS_BASE_TEXTPAGE_HPP_
