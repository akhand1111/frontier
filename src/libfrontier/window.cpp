/*
 * Frontier - A toolkit for creating simple OS-independent user interfaces
 * Copyright (C) 2018 Ian Parker <ian@geekprojects.com>
 *
 * This file is part of Frontier.
 *
 * Frontier is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Frontier is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Frontier.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <frontier/frontier.h>
#include <frontier/engine.h>
#include <frontier/widgets.h>

using namespace std;
using namespace Frontier;
using namespace Geek::Gfx;

FrontierWindow::FrontierWindow(FrontierApp* app)
{
    m_app = app;
    m_engineWindow = NULL;

    m_mouseOverWidget = NULL;

    m_surface = NULL;

    m_size.set(500, 50);
}

FrontierWindow::~FrontierWindow()
{
}

bool FrontierWindow::init()
{
    if (m_engineWindow == NULL)
    {
        return m_app->getEngine()->initWindow(this);
    }
    return true;
}

void FrontierWindow::setContent(Widget* content)
{
    m_widget = content;
    m_widget->setPosition(0, 0);
}

void FrontierWindow::show()
{
    init();

    m_engineWindow->show();

    update();
}

void FrontierWindow::setSize(Size size)
{
    m_size = size;
    update();
}

void FrontierWindow::update()
{
    init();

    m_widget->calculateSize();

    Size min = m_widget->getMinSize();
    Size max = m_widget->getMaxSize();

    m_size.setMax(min);
    m_size.setMin(max);

    m_size = m_widget->setSize(m_size);

    printf("FrontierWindow::update: Updating window size: %s\n", m_size.toString().c_str());

    m_widget->layout();
#if 0
    m_widget->dump(1);
#endif

    float scale = m_engineWindow->getScaleFactor();

    if (m_surface == NULL || (int)m_surface->getWidth() != m_size.width || (int)m_surface->getHeight() != m_size.height)
    {
        if (m_surface != NULL)
        {
            delete m_surface;
        }

        if (scale > 1.0)
        {
            m_surface = new HighDPISurface(m_size.width, m_size.height, 4);
        }
        else
        {
            m_surface = new Surface(m_size.width, m_size.height, 4);
        }
    }
    printf("FrontierWindow::update: Window surface=%p\n", m_surface);


    m_app->getTheme()->drawBackground(m_surface);

    m_widget->draw(m_surface);

    m_engineWindow->update();
    m_widget->clearDirty();
}

/*
int FrontierWindow::getWidth()
{
    if (m_widget != NULL)
    {
        return m_widget->getWidth();
    }
    return 0;
}

int FrontierWindow::getHeight()
{
    if (m_widget != NULL)
    {
        return m_widget->getHeight();
    }
    return 0;
}
*/

void FrontierWindow::postMessage(Message* message)
{
}

bool FrontierWindow::handleMessage(Message* message)
{
    Widget* destWidget = NULL;
    bool updateActive = false;

    if (message->messageType == FRONTIER_MSG_INPUT)
    {
        InputMessage* imsg = (InputMessage*)message;
        switch (imsg->inputMessageType)
        {
            case FRONTIER_MSG_INPUT_MOUSE_BUTTON:
                updateActive = true;
                destWidget = m_widget->handleMessage(message);
               break;

            case FRONTIER_MSG_INPUT_MOUSE_MOTION:
                destWidget = m_widget->handleMessage(message);

                if (m_mouseOverWidget != destWidget)
                {
                    if (m_mouseOverWidget != NULL)
                    {
                        m_mouseOverWidget->onMouseLeave();
                    }

                    if (destWidget != NULL)
                    {
                        destWidget->onMouseEnter();
                    }

                    m_mouseOverWidget = destWidget;
                }
                break;

            case FRONTIER_MSG_INPUT_KEY:
                if (m_activeWidget != NULL)
                {
                    destWidget = m_activeWidget->handleMessage(message);
                }
                break;
        }
    }

    if (destWidget != NULL)
    {
        if (updateActive)
        {
            m_activeWidget = destWidget;
        }

        if (m_widget->isDirty())
        {
            update();
        }
    }

    delete message;

    return true;
}

