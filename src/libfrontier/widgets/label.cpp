
#include <frontier/frontier.h>
#include <frontier/widgets.h>

using namespace std;
using namespace Geek::Gfx;

Label::Label(FrontierApp* ui, wstring text) : Widget(ui)
{
    m_text = text;
}

Label::~Label()
{
}

void Label::setText(std::wstring text)
{
    m_text = text;
    setDirty();
}

void Label::calculateSize()
{
    m_width = 0;
    m_lineHeight = m_ui->getTheme()->getTextHeight();

    int pos = 0;
    int lines = 0;
    wstring line = L"";
    for (pos = 0; pos < m_text.length(); pos++)
    {
        if (m_text[pos] == '\n' || pos == m_text.length() - 1)
        {
            lines++;
            if (pos == m_text.length() - 1)
            {
                line += m_text[pos];
            }
            int w = m_ui->getTheme()->getTextWidth(line);
            if (w > m_width)
            {
                m_width = w;
            }
            line = L"";
        }
        else
        {
            line += m_text[pos];
        }
    }

    m_width += (5 * 2);
    m_height = (m_lineHeight * lines) + (5 * 2);

    if (m_width < m_minWidth)
    {
        m_width = m_minWidth;
    }

    m_dirty = false;
}

bool Label::draw(Surface* surface)
{
    Widget::draw(surface);

    int y = 5;

    int pos = 0;
    wstring line = L"";
    for (pos = 0; pos < m_text.length(); pos++)
    {
        if (m_text[pos] == '\n' || pos == m_text.length() - 1)
        {
            if (pos == m_text.length() - 1)
            {
                line += m_text[pos];
            }
            int w = m_ui->getTheme()->getTextWidth(line);
            int x = (m_width / 2) - (w / 2);

            m_ui->getTheme()->drawText(
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

Widget* Label::handleMessage(Message* msg)
{
    // We just swallow events
    return NULL;
}

