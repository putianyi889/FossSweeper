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

#include <cstddef>
#include <fsweep/DesktopModel.hpp>
#include <fsweep/GameModel.hpp>
#include <fsweep/Point.hpp>
#include <fsweep/Sprite.hpp>
#include <fsweep/Timer.hpp>
#include <utility>

bool fsweep::DesktopModel::TryChangePixelScale(int new_pixel_scale)
{
  if (this->pixel_scale == new_pixel_scale) return false;
  this->pixel_scale = new_pixel_scale;
  return true;
}

int fsweep::DesktopModel::GetPixelScale() const noexcept { return this->pixel_scale; }

void fsweep::DesktopModel::LeftPress() { this->left_down = true; }

void fsweep::DesktopModel::LeftRelease(fsweep::Timer& timer)
{
  if (this->GetGameState() == fsweep::GameState::Playing)
  {
    this->game_time = timer.GetGameTime();
  }
  if (!this->buttons_locked)
  {
    auto initially_playing = this->GetGameState() == fsweep::GameState::Playing;
    if (this->hover_button_o.has_value())
    {
      auto& hover_button = this->hover_button_o.value();
      if (this->right_down)
      {
        this->AreaClickButton(hover_button.x, hover_button.y);
      }
      else
      {
        this->ClickButton(hover_button.x, hover_button.y);
      }
      if (this->GetGameState() == fsweep::GameState::Playing && !initially_playing)
      {
        timer.Start();
      }
      else if (this->GetGameState() == fsweep::GameState::Dead ||
               this->GetGameState() == fsweep::GameState::Cool && initially_playing)
      {
        timer.Stop();
      }
    }
    else if (this->hover_face)
    {
      this->NewGame();
      timer.Stop();
    }
  }
  else if (!this->right_down)
  {
    this->buttons_locked = false;
  }
  this->left_down = false;
}

void fsweep::DesktopModel::RightPress(fsweep::Timer& timer)
{
  this->right_down = true;
  if (this->GetGameState() == fsweep::GameState::Playing)
  {
    this->game_time = timer.GetGameTime();
  }
  if (!this->left_down && this->hover_button_o.has_value() && !this->buttons_locked)
  {
    auto& hover_button = this->hover_button_o.value();
    this->AltClickButton(hover_button.x, hover_button.y);
  }
}

void fsweep::DesktopModel::RightRelease()
{
  this->right_down = false;
  if (this->left_down && this->GetGameState() == fsweep::GameState::Playing)
  {
    this->buttons_locked = true;
    if (this->hover_button_o.has_value())
    {
      auto& hover_button = this->hover_button_o.value();
      this->AreaClickButton(hover_button.x, hover_button.y);
    }
  }
  else
  {
    this->buttons_locked = false;
  }
}

void fsweep::DesktopModel::MouseEnter() { this->mouse_hover = false; }

void fsweep::DesktopModel::MouseLeave() { this->mouse_hover = true; }

void fsweep::DesktopModel::MouseMove(int x, int y)
{
  this->hover_button_o = std::nullopt;
  if (x >= this->GetBorderSize() && x < this->GetSize().x - this->GetBorderSize() &&
      y >= this->GetHeaderHeight() && y < this->GetSize().y - this->GetBorderSize())
  {
    this->hover_button_o =
        fsweep::ButtonPosition((x - this->GetBorderSize()) / this->GetButtonDimension(),
                               (y - this->GetHeaderHeight()) / this->GetButtonDimension());
  }
  const auto face_point = this->GetFaceButtonPoint();
  this->hover_face = x >= face_point.x && x < face_point.x + this->GetFaceButtonDimension() &&
                     y >= face_point.y && y < face_point.y + this->GetFaceButtonDimension();
}

const int FACE_BUTTON_DIMENSION = 24;
const int BORDER_SIZE = 8;
const int BUTTON_DIMENSION = 16;
const int LCD_DIGIT_WIDTH = 16;
const int HEADER_HEIGHT = BORDER_SIZE * 2 + FACE_BUTTON_DIMENSION;

int fsweep::DesktopModel::GetFaceButtonDimension() const noexcept
{
  return this->pixel_scale * FACE_BUTTON_DIMENSION;
}

int fsweep::DesktopModel::GetBorderSize() const noexcept { return this->pixel_scale * BORDER_SIZE; }

int fsweep::DesktopModel::GetButtonDimension() const noexcept
{
  return this->pixel_scale * BUTTON_DIMENSION;
}

int fsweep::DesktopModel::GetLcdDigitWidth() const noexcept
{
  return this->pixel_scale * LCD_DIGIT_WIDTH;
}

int fsweep::DesktopModel::GetHeaderHeight() const noexcept
{
  return this->pixel_scale * HEADER_HEIGHT;
}

fsweep::Sprite fsweep::DesktopModel::GetFaceSprite() const noexcept
{
  if (this->GetGameState() == fsweep::GameState::None ||
      this->GetGameState() == fsweep::GameState::Playing)
  {
    if (this->left_down && !this->buttons_locked && this->hover_face)
    {
      return fsweep::Sprite::ButtonSmileDown;
    }
    else if (this->left_down && !this->buttons_locked && this->hover_button_o.has_value())
    {
      const auto& hover_button = this->hover_button_o.value();
      const auto& button = this->GetButton(hover_button.x, hover_button.y);
      if (button.GetButtonState() != fsweep::ButtonState::Down &&
          button.GetButtonState() != fsweep::ButtonState::Flagged)
      {
        return fsweep::Sprite::ButtonShocked;
      }
    }
  }
  else if (this->GetGameState() == fsweep::GameState::Cool)
  {
    if (this->left_down && this->hover_face)
    {
      return fsweep::Sprite::ButtonSmileDown;
    }
    else
    {
      return fsweep::Sprite::ButtonCool;
    }
  }
  else  // if (model.GetGameState() == fsweep::GameState::Dead)
  {
    if (this->left_down && this->hover_face)
    {
      return fsweep::Sprite::ButtonSmileDown;
    }
    else
    {
      return fsweep::Sprite::ButtonDead;
    }
  }
  return fsweep::Sprite::ButtonSmile;
}

fsweep::Sprite fsweep::DesktopModel::GetButtonSprite(int x, int y) const noexcept
{
  if (x >= this->game_configuration.GetButtonsWide() ||
      y >= this->game_configuration.GetButtonsTall())
  {
    return fsweep::Sprite::ButtonNone;
  }
  const auto& button = this->GetButton(x, y);
  auto button_position = fsweep::ButtonPosition(x, y);
  if (this->GetGameState() == fsweep::GameState::None ||
      this->GetGameState() == fsweep::GameState::Playing)
  {
    if (button.GetButtonState() != fsweep::ButtonState::Flagged &&
        button.GetButtonState() != fsweep::ButtonState::Down && this->left_down &&
        this->hover_button_o.has_value() && !this->buttons_locked)
    {
      const auto& hover_button = this->hover_button_o.value();
      if ((hover_button == button_position) ||
          (this->GetGameState() == fsweep::GameState::Playing && this->right_down &&
           hover_button.IsNear(button_position)))
      {
        if (fsweep::ButtonState() == fsweep::ButtonState::Questioned)
        {
          return fsweep::Sprite::ButtonQuestionDown;
        }
        return fsweep::Sprite::ButtonDown;
      }
    }
    else if (button.GetButtonState() == fsweep::ButtonState::Flagged)
    {
      return fsweep::Sprite::ButtonFlag;
    }
    else if (button.GetButtonState() == fsweep::ButtonState::Questioned)
    {
      return fsweep::Sprite::ButtonQuestion;
    }
    else if (button.GetButtonState() == fsweep::ButtonState::Down)
    {
      return fsweep::getDownButtonSprite(button.GetSurroundingBombs());
    }
    else
    {
      return fsweep::Sprite::ButtonNone;
    }
  }
  else  // if (model.GetGameState() == fsweep::GameState::Cool || model.GetGameState() ==
        // fsweep::GameState::Dead)
  {
    if (button.GetButtonState() == fsweep::ButtonState::None ||
        button.GetButtonState() == fsweep::ButtonState::Questioned)
    {
      if (button.GetHasBomb())
      {
        return fsweep::Sprite::ButtonBomb;
      }
      else
      {
        return fsweep::Sprite::ButtonNone;
      }
    }
    else if (button.GetButtonState() == fsweep::ButtonState::Flagged)
    {
      if (button.GetHasBomb())
      {
        return fsweep::Sprite::ButtonFlagHit;
      }
      else
      {
        return fsweep::Sprite::ButtonFlagMiss;
      }
    }
    else if (button.GetButtonState() == fsweep::ButtonState::Down)
    {
      if (button.GetHasBomb())
      {
        return fsweep::Sprite::ButtonBombExplode;
      }
      else
      {
        return fsweep::getDownButtonSprite(button.GetSurroundingBombs());
      }
    }
  }
  return fsweep::Sprite::ButtonNone;
}

fsweep::Point fsweep::DesktopModel::GetFaceButtonPoint() const noexcept
{
  return fsweep::Point((this->GetSize().x / 2) - (this->GetFaceButtonDimension() / 2),
                       this->GetBorderSize());
}

fsweep::Point fsweep::DesktopModel::GetButtonPoint(int x, int y) const noexcept
{
  return fsweep::Point(this->GetBorderSize() + (x * this->GetButtonDimension()),
                       this->GetHeaderHeight() + (y * this->GetButtonDimension()));
}

fsweep::Point fsweep::DesktopModel::GetScorePoint(std::size_t digit) const noexcept
{
  return fsweep::Point(this->GetBorderSize() + (this->GetLcdDigitWidth() * digit),
                       this->GetBorderSize());
}

fsweep::Point fsweep::DesktopModel::GetTimerPoint(std::size_t digit) const noexcept
{
  return fsweep::Point(
      this->GetSize().x - (this->GetBorderSize() + (this->GetLcdDigitWidth() * (3 - digit))),
      this->GetBorderSize());
}

fsweep::Point fsweep::DesktopModel::GetSize() const noexcept
{
  return fsweep::Point(
      (this->game_configuration.GetButtonsWide() * (this->pixel_scale * BUTTON_DIMENSION)) +
          ((this->pixel_scale * BORDER_SIZE) * 2),
      (this->game_configuration.GetButtonsTall() * (this->pixel_scale * BUTTON_DIMENSION)) +
          ((this->pixel_scale * BORDER_SIZE) + (this->pixel_scale * HEADER_HEIGHT)));
}
