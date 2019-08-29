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
#include <frontier/widgets/label.h>

using namespace std;
using namespace Frontier;
using namespace Geek::Gfx;

Label::Label(FrontierApp* ui, wstring text) : Widget(ui, L"Label")
{
    m_text = text;
    m_align = ALIGN_CENTER;
    setStyle(STYLE_EXPAND_HORIZONTAL, true);
    setStyle(STYLE_EXPAND_VERTICAL, false);
}

Label::Label(FrontierApp* ui, wstring text, TextAlign align) : Widget(ui, L"Label")
{
    m_text = text;
    m_align = align;
    setStyle(STYLE_EXPAND_HORIZONTAL, true);
    setStyle(STYLE_EXPAND_VERTICAL, false);
}

Label::Label(FrontierWindow* window, wstring text) : Widget(window, L"Label")
{
    m_text = text;
    m_align = ALIGN_CENTER;
    setStyle(STYLE_EXPAND_HORIZONTAL, true);
    setStyle(STYLE_EXPAND_VERTICAL, false);
}

Label::Label(FrontierWindow* window, wstring text, TextAlign align) : Widget(window, L"Label")
{
    m_text = text;
    m_align = align;
    setStyle(STYLE_EXPAND_HORIZONTAL, true);
    setStyle(STYLE_EXPAND_VERTICAL, false);
}


Label::~Label()
{
}

void Label::setText(std::wstring text)
{
    m_text = text;
    setDirty();
}

void Label::setAlign(TextAlign align)
{
    m_align = align;
    setDirty();
}

void Label::calculateSize()
{
    m_lineHeight = m_app->getTheme()->getTextHeight();

    m_minSize.set(0, 0);

    unsigned int pos = 0;
    int lines = 1;
    wstring line = L"";
    for (pos = 0; pos < m_text.length(); pos++)
    {
        if (m_text[pos] == '\n' || pos == m_text.length() - 1)
        {
            if (pos == m_text.length() - 1)
            {
                line += m_text[pos];
            }
            else
            {
                lines++;
            }

            int w = m_app->getTheme()->getTextWidth(line);
            if (w > m_minSize.width)
            {
                m_minSize.width = w;
            }
            line = L"";
        }
        else
        {
            line += m_text[pos];
        }
    }

    int margin = (int)getStyle(STYLE_MARGIN);
    m_minSize.width += (margin * 2);
    m_minSize.height = (m_lineHeight * lines) + (margin * 2);

    if (getStyle(STYLE_EXPAND_HORIZONTAL))
    {
        m_maxSize.width = WIDGET_SIZE_UNLIMITED;
    }
    else
    {
        m_maxSize.width = m_minSize.width;
    }

    if (getStyle(STYLE_EXPAND_VERTICAL))
    {
        m_maxSize.height = WIDGET_SIZE_UNLIMITED;
    }
    else
    {
        m_maxSize.height = m_minSize.height;
    }

}

bool Label::draw(Surface* surface)
{
    Widget::draw(surface);

    int margin = (int)getStyle(STYLE_MARGIN);
    int y = margin;

    if (hasStyle(STYLE_BORDER) && getStyle(STYLE_BORDER))
    {
        surface->drawRectRounded(0, 0, getWidth(), getHeight(), 5, 0xff4b4b4b);
    }

    int lines = 1;
    unsigned int pos = 0;
    for (pos = 0; pos < m_text.length(); pos++)
    {
        if (m_text[pos] == '\n')
        {
            lines++;
        }
    }

    y = (m_setSize.height / 2) - (lines * m_lineHeight) / 2;
 
    wstring line = L"";
    for (pos = 0; pos < m_text.length(); pos++)
    {
        if (m_text[pos] == '\n' || pos == m_text.length() - 1)
        {
            if (pos == m_text.length() - 1)
            {
                line += m_text[pos];
            }
            int w = m_app->getTheme()->getTextWidth(line);
            int x = 0;

            switch (m_align)
            {
                case ALIGN_LEFT:
                    x = margin;
                    break;
                case ALIGN_CENTER:
                    x = (m_setSize.width / 2) - (w / 2);
                    break;
                case ALIGN_RIGHT:
                    x = (m_setSize.width - margin) - w;
                    break;
            }

            m_app->getTheme()->drawText(
                surface,
                x,
                y,
                line.c_str());
 
            y += m_lineHeight;
            line = L"";
        }
        else
        {
            line += m_text[pos];
        }
    }

   return true;
}

