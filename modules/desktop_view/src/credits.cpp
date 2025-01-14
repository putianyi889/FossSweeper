// SPDX-FileCopyrightText: 2022 Daniel Valcour <fosssweeper@gmail.com>
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Copyright (c) 2022 Daniel Valcour
 *
 * This file is part of FossSweeper.
 *
 * FossSweeper is free software: you can redistribute it and/or modify it under the terms of the GNU
 * General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * FossSweeper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with FossSweeper. If not,
 * see <https://www.gnu.org/licenses/>.
 *
 */

#include "credits.hpp"

#include "TextDialog.hpp"

const char* const fsweep::CREDITS_TEXT =
    "FossSweeper Credits\n"
    " \n"
    "Thanks to all who have helped to make FossSweeper what it is today!\n"
    " \n"
    "Each contributor name is followed by their GitHub profile in parenthesis.\n"
    " \n"
    "Code Contributors:\n"
    "Daniel Valcour (@Journeyman-dev)\n"
    "";

fsweep::TextDialog fsweep::createCreditsDialog(wxWindow* parent)
{
  return fsweep::TextDialog(parent, "FossSweeper Credits", fsweep::CREDITS_TEXT);
}
