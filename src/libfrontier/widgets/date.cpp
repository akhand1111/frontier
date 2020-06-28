/*
 * Frontier - A toolkit for creating simple OS-independent user interfaces
 * Copyright (C) 2020 Ian Parker <ian@geekprojects.com>
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


#include <frontier/widgets/date.h>
#include <frontier/windows/datepicker.h>
#include <frontier/utils.h>

#include <time.h>

using namespace std;
using namespace std::chrono;
using namespace Frontier;
using namespace Geek;

Date::Date(FrontierApp* app, bool hasTime) : Frame(app, L"Date", true)
{
    m_hasTime = hasTime;
    auto now = system_clock::now().time_since_epoch();
    m_value = std::chrono::duration_cast<std::chrono::seconds>(now);
    m_hasValue = false;

    setup();
}

Date::Date(FrontierApp* app, bool hasTime, std::chrono::seconds value) : Frame(app, L"Date", true)
{
    m_hasTime = hasTime;
    m_value = value;
    m_hasValue = true;

    setup();
}

Date::~Date()
{
}

void Date::setup()
{
    getWidgetStyle()->applyProperty("margin", 0);

    Icon* calendarIcon = getApp()->getTheme()->getIcon(FRONTIER_ICON_CALENDAR);

    m_dateLabel = new Label(getApp(), L"", calendarIcon);
    m_dateLabel->clickSignal().connect(sigc::mem_fun(*this, &Date::onDateClick));
    m_dateLabel->setWidgetClass(L"dateInput");
    add(m_dateLabel);

    if (m_hasTime)
    {
        m_hourInput = new NumberInput(getApp());
        m_hourInput->setMaxLength(2);
        m_hourInput->setMin(0);
        m_hourInput->setMax(23);
        add(m_hourInput);

        Label* startTimeSpacer = new Label(getApp(), L":");
        startTimeSpacer->setStyle("expand-horizontal", false);
        startTimeSpacer->setStyle("margin-left", 0);
        startTimeSpacer->setStyle("margin-right", 0);
        startTimeSpacer->setStyle("padding-left", 0);
        startTimeSpacer->setStyle("padding-right", 0);
        add(startTimeSpacer);

        m_minuteInput = new NumberInput(getApp());
        m_minuteInput->setMaxLength(2);
        m_minuteInput->setMin(0);
        m_minuteInput->setMax(59);
        add(m_minuteInput);

        m_hourInput->signalTextChanged().connect(sigc::mem_fun(*this, &Date::onTimeChanged));
        m_minuteInput->signalTextChanged().connect(sigc::mem_fun(*this, &Date::onTimeChanged));

    }
    updateDateTime();
}

void Date::setHasValue(bool hasValue)
{
    m_hasValue = hasValue;
    updateDateTime();
}

void Date::setValue(std::chrono::seconds value)
{
    m_value = value;
    updateDateTime();
}

void Date::onDateClick(Widget* widget)
{

    time_t t = (time_t)m_value.count();
    tm tm;
    gmtime_r(&t, &tm);

    DatePickerWindow* datePicker = new DatePickerWindow(getApp(), tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    datePicker->dateSelectSignal().connect(sigc::mem_fun(*this, &Date::onDateSelect));
    datePicker->show();
}

bool Date::onDateSelect(int year, int month, int day)
{
    log(DEBUG, "onDateSelect: year=%d, month=%d, day=%d", year, month, day);

    time_t t = (time_t)m_value.count();
    tm tm;
    gmtime_r(&t, &tm);
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;

    t = mktime(&tm);

    m_value = std::chrono::seconds(t);
    m_hasValue = true;

    updateDateTime();

    m_signalValueChanged.emit(m_value);

    return true;
}

void Date::onTimeChanged(Frontier::TextInput* w)
{
    int hour = m_hourInput->getInteger();
    int minute = m_minuteInput->getInteger();

    time_t t = (time_t)m_value.count();
    tm tm;
    gmtime_r(&t, &tm);
    tm.tm_hour = hour;
    tm.tm_min = minute;

    t = mktime(&tm);

    m_value = std::chrono::seconds(t);

    m_signalValueChanged.emit(m_value);
}

void Date::updateDateTime()
{
    time_t t = (time_t)m_value.count();
    struct tm tm;
    gmtime_r(&t, &tm);

    if (m_hasValue)
    {

    char buf[50];
    strftime(buf, 50, "%d/%m/%Y", &tm);
    m_dateLabel->setText(Utils::string2wstring(buf));
    }
    else
    {
        m_dateLabel->setText(L"");
    }

    if (m_hasTime)
    {
        m_hourInput->setNumber(tm.tm_hour);
        m_minuteInput->setNumber(tm.tm_min);
    }
}

