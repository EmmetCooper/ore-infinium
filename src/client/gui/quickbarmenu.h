/******************************************************************************
 *   Copyright (C) 2013 by Shaun Reich <sreich@kde.org>                       *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or            *
 *   modify it under the terms of the GNU General Public License as           *
 *   published by the Free Software Foundation; either version 2 of           *
 *   the License, or (at your option) any later version.                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *****************************************************************************/

#ifndef QUICKBARMENU_H
#define QUICKBARMENU_H

#include <vector>
#include <string>

#include <Rocket/Core.h>
#include <SDL_events.h>
#include <src/timer.h>

class SpriteSheetRenderer;
namespace Rocket
{
namespace Core
{
class ElementDocument;
}
}

class Client;
class QuickBarInventory;

class QuickBarMenu : public Rocket::Core::EventListener
{
public:
    QuickBarMenu(Client* client, QuickBarInventory* inventory, SpriteSheetRenderer* spriteSheetRenderer);
    ~QuickBarMenu();

    void ProcessEvent(Rocket::Core::Event& event);

    void loadDocument();
    Rocket::Core::ElementDocument* document();

    bool visible();

    void show();

    void update();

    void handleEvent(const SDL_Event& event);

    void nextSlot();
    void previousSlot();

    void reloadSlot(uint8_t index);

    QuickBarInventory* inventory() {
        return m_inventory;
    }

private:
    void selectSlot(uint8_t index);
    void showSwitchedTooltip(Rocket::Core::Element* element);

    /// non-ownership
    QuickBarInventory* m_inventory = nullptr;

    /// non-ownership
    Client* m_client = nullptr;

    /// non-ownership
    SpriteSheetRenderer* m_spriteSheetRenderer = nullptr;

    Timer m_switchedTooltipTimer;

    /// milliseconds
    const uint64_t switchedTooltipTimerDisplayTime = 2500;

    /// actual quickbar/equipped items gui
    Rocket::Core::ElementDocument* m_menu = nullptr;

    ///selection switched/changed tooltip popup
    Rocket::Core::ElementDocument* m_switchedTooltip = nullptr;
};

#endif
