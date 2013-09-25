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

#include "quickbarmenu.h"

#include "src/quickbarinventory.h"

#include "src/client/client.h"
//#include "src/client/gui/gui.h"
#include "src/client/gui/decoratorspritesheetinstancer.h"

#include <src/spritesheetrenderer.h>

#include <src/debug.h>
#include <src/item.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>

QuickBarMenu::QuickBarMenu(Client* client, QuickBarInventory* inventory, SpriteSheetRenderer* spriteSheetRenderer)
    :   m_inventory(inventory),
        m_client(client),
        m_spriteSheetRenderer(spriteSheetRenderer)
{
    DecoratorSpriteSheetInstancer* instancer = new DecoratorSpriteSheetInstancer();
    Rocket::Core::Factory::RegisterDecoratorInstancer("spritesheet-decorator", instancer);
    instancer->RemoveReference();

    loadDocument();

    selectSlot(0);
}

QuickBarMenu::~QuickBarMenu()
{
}

void QuickBarMenu::update()
{
    if (m_switchedTooltip->IsVisible()) {
        if (m_switchedTooltipTimer.milliseconds() >= switchedTooltipTimerDisplayTime) {
            m_switchedTooltip->Hide();
        }
    }
}

void QuickBarMenu::ProcessEvent(Rocket::Core::Event& event)
{
    const Rocket::Core::String& id = event.GetCurrentElement()->GetId();
    const Rocket::Core::String& type = event.GetType();

    if (id == "0") {
        selectSlot(0);
    } else if (id == "1") {
        selectSlot(1);
    } else if (id == "2") {
        selectSlot(2);
    } else if (id == "3") {
        selectSlot(3);
    } else if (id == "4") {
        selectSlot(4);
    } else if (id == "5") {
        selectSlot(5);
    } else if (id == "6") {
        selectSlot(6);
    } else if (id == "7") {
        selectSlot(7);
    }
}

void QuickBarMenu::loadDocument()
{
//    m_menu = GUI::instance()->context()->LoadDocument("../client/gui/assets/quickBarMenu.rml");
 //   m_switchedTooltip = GUI::instance()->context()->LoadDocument("../client/gui/assets/quickBarSwitchedTooltip.rml");

    float margin = m_menu->GetProperty<float>("width");
    margin += margin * 0.25f;
    m_switchedTooltip->SetProperty("margin-right", std::to_string(margin).c_str());

    //Rocket::Core::Colourb shit(255, 255, 0, 155);
    //m_menu->SetProperty("background-color", Rocket::Core::Property(shit, Rocket::Core::Property::COLOUR));

//    m_menu->GetElementById("0sub")->SetProperty("background-color", Rocket::Core::Property(shit2, Rocket::Core::Property::COLOUR));
//    m_chat->GetElementById("title")->SetInnerRML("Chat");

//    m_tabSet = dynamic_cast<Rocket::Controls::ElementTabSet*>(m_chat->GetElementById("tabset"));
//    background-image: ../../../../textures/entities.png 0px 0px 22px 22px;
//    background-image: ../../../../textures/entities.png 0px 0px 22px 22px;
//   m_menu->GetElementById("0sub")->SetProperty("background-decorator", Rocket::Core::Property("image", Rocket::Core::Property::STRING));
    m_menu->GetElementById("0sub")->SetAttribute("background-decorator", "image");
//    m_menu->GetElementById("0sub")->SetProperty("background-image", Rocket::Core::Property("../../../../textures/entities.png 0px 0px 22px 22px", Rocket::Core::Property::STRING));
//    m_menu->GetElementById("0sub")->SetProperty("background-image", Rocket::Core::Property("../../../../textures/entities.png 0px 0px 512px 512px", Rocket::Core::Property::KEYWORD));
    m_menu->GetElementById("0sub")->SetAttribute("background-image", "../../../../textures/entities.png 0px 0px 512px 512px");

    for (int i = 0; i < m_inventory->maxEquippedSlots(); ++i) {
        std::string str = std::to_string(i);
        m_menu->GetElementById(str.c_str())->AddEventListener("click", this);
    }
}

void QuickBarMenu::selectSlot(uint8_t index)
{
    m_inventory->selectSlot(index);

    Rocket::Core::Colourb unselectedColor(0, 0, 255, 255);

    for (int i = 0; i < m_inventory->maxEquippedSlots(); ++i) {
        std::string str = std::to_string(i);
        m_menu->GetElementById(str.c_str())->SetProperty("background-color", Rocket::Core::Property(unselectedColor, Rocket::Core::Property::COLOUR));
    }

    Rocket::Core::Colourb selectedColor(0, 255, 0, 255);

    std::string str = std::to_string(index);
    Rocket::Core::Element* currentElement = m_menu->GetElementById(str.c_str());
    assert(currentElement);

    currentElement->SetProperty("background-color", Rocket::Core::Property(selectedColor, Rocket::Core::Property::COLOUR));

    if (m_inventory->item(index) != nullptr) {
        showSwitchedTooltip(currentElement, index);
    }

    m_client->sendQuickBarInventorySlotSelectRequest(index);
}

///FIXME:: there is a bit of a sync issue here at startup/connection. when the client joins he makes this, selects an inventory item. what *should* happen is the server
///tells the client which inventory slot it has selected, after all the quickbar inventory data was sent. because presently we're selecting it, trying to show a tooltip for an item that isn't there yet,
///and then receiving the inventory data from server but by then it's too late. this isn't something you can notice much though, but i'd prefer it to show the equipped item tooltip on connect and have all data synced by then.
void QuickBarMenu::showSwitchedTooltip(Rocket::Core::Element* element, uint8_t index)
{
    assert(m_inventory);

    m_switchedTooltipTimer.reset();

    float top = element->GetAbsoluteTop() + (element->GetProperty<float>("height") * 0.5f) + m_switchedTooltip->GetProperty<float>("height") - (m_switchedTooltip->GetOffsetHeight() * 0.5f);
    m_switchedTooltip->SetProperty("top", std::to_string(top).c_str());

    Item* item = m_inventory->item(index);
    if (item) {
        m_switchedTooltip->GetElementById("main")->SetInnerRML(item->name().c_str());
        m_switchedTooltip->Show();
    }
}

void QuickBarMenu::nextSlot()
{
    //at the right bound
    if (m_inventory->equippedIndex() == m_inventory->maxEquippedSlots() - 1) {
        return;
    }

    selectSlot(m_inventory->equippedIndex() + 1);
}

void QuickBarMenu::previousSlot()
{
    //at the left bound
    if (m_inventory->equippedIndex() == 0) {
        return;
    }

    selectSlot(m_inventory->equippedIndex() - 1);
}

void QuickBarMenu::reloadSlot(uint8_t index)
{
    Item* item = m_inventory->item(index);

    //stack size
    const std::string id = std::to_string(index) + "subcount";
    //main element (image)
    const std::string subid = std::to_string(index) + "sub";

    Rocket::Core::Element* stackSizeElement = m_menu->GetElementById(id.c_str());
    Rocket::Core::Element* subElement = m_menu->GetElementById(subid.c_str());

    if (item == nullptr) {
        stackSizeElement->SetInnerRML("empty");

        // set the icon image to null/empty
        subElement->SetProperty("image-x1", "0");
        subElement->SetProperty("image-x2", "0");
        subElement->SetProperty("image-y1", "0");
        subElement->SetProperty("image-y2", "0");
    } else {

        std::string itemStackSize = std::to_string(item->stackSize());
        stackSizeElement->SetInnerRML(itemStackSize.c_str());

        SpriteSheetRenderer::SpriteFrameIdentifier frameIdentifier = m_spriteSheetRenderer->spriteFrame(item->frameName());

        const std::string x1 = std::to_string(frameIdentifier.x);
        const std::string x2 = std::to_string(frameIdentifier.width);
        const std::string y1 = std::to_string(frameIdentifier.y);
        const std::string y2 = std::to_string(frameIdentifier.height);

        subElement->SetProperty("image-src", "../textures/entities.png");
        subElement->SetProperty("image-x1", x1.c_str());
        subElement->SetProperty("image-x2", x2.c_str());
        subElement->SetProperty("image-y1", y1.c_str());
        subElement->SetProperty("image-y2", y2.c_str());
    }
}

void QuickBarMenu::handleEvent(const SDL_Event& event)
{
    switch (event.type) {
    case SDL_KEYDOWN:
        if (event.key.keysym.sym == SDLK_1) {
            selectSlot(0);
        } else if (event.key.keysym.sym == SDLK_2) {
            selectSlot(1);
        } else if (event.key.keysym.sym == SDLK_3) {
            selectSlot(2);
        } else if (event.key.keysym.sym == SDLK_4) {
            selectSlot(3);
        } else if (event.key.keysym.sym == SDLK_5) {
            selectSlot(4);
        } else if (event.key.keysym.sym == SDLK_6) {
            selectSlot(5);
        } else if (event.key.keysym.sym == SDLK_7) {
            selectSlot(6);
        } else if (event.key.keysym.sym == SDLK_8) {
            selectSlot(7);
        }
        break;
    }
}

bool QuickBarMenu::visible()
{
    return m_menu->IsVisible();
}

Rocket::Core::ElementDocument* QuickBarMenu::document()
{
    return m_menu;
}

void QuickBarMenu::show()
{
    m_menu->Show();
}
