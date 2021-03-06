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

#ifndef __FRONTIER_STYLES_H_
#define __FRONTIER_STYLES_H_

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>

#include <geek/core-logger.h>
#include <frontier/value.h>

#define FRONTIER_COLOUR_TRANSPARENT 0xffffffff00000000ull
#define FRONTIER_HORIZONTAL_ALIGN_LEFT 0
#define FRONTIER_HORIZONTAL_ALIGN_CENTRE 1
#define FRONTIER_HORIZONTAL_ALIGN_RIGHT 2
#define FRONTIER_VERTICAL_ALIGN_TOP 0
#define FRONTIER_VERTICAL_ALIGN_MIDDLE 1
#define FRONTIER_VERTICAL_ALIGN_BOTTOM 2

namespace Frontier {

class Widget;

/**
 * \defgroup styles CSS Engine
 */

struct StyleSelector
{
    std::wstring widgetType;
    std::wstring className;
    std::wstring id;
    std::string state;
    bool descendant;

    bool matches(Frontier::Widget* widget);
    std::wstring getKey();
};

/**
 * \brief An individual CSS rule
 *
 * \ingroup styles
 */
class StyleRule
{
 private:
    uint64_t m_id;
    std::vector<StyleSelector> m_selectors;
    std::unordered_map<std::string, Value> m_properties;

 public:
    StyleRule() { m_id = 0; }
    virtual ~StyleRule() = default;

    const std::vector<StyleSelector>& getSelectors() { return  m_selectors; }
    void addSelector(StyleSelector selector);

    bool matches(Frontier::Widget* widget);

    void setId(uint64_t id) { m_id = id; }
    uint64_t getId() const { return m_id; }
    std::wstring getKey();

    void setProperty(std::string property, Value value);
    void applyProperty(std::string property, Value value);
    void applyProperty(std::string property, std::vector<Value> values);
    std::unordered_map<std::string, Value> getProperties() { return m_properties; }
};

class CssParser;

typedef std::function<bool(std::pair<StyleRule*, int>, std::pair<StyleRule*, int>)> StyleComparator;

/**
 * \brief A CSS rules engine
 *
 * \ingroup styles
 */
class StyleEngine : public Geek::Logger
{
 private:
    CssParser* m_parser;
    std::vector<std::pair<StyleRule*, int>> m_styleRules;
    uint64_t m_timestamp;
    uint64_t m_currentId;

 public:
    StyleEngine();
    ~StyleEngine();

    bool init();

    bool parse(std::string path);
    bool parseString(std::string css);

    void addRule(StyleRule* rule);
    StyleRule* findByKey(std::string key);

    std::unordered_map<std::string, Value> getProperties(Widget* widget);
    uint64_t getTimestamp() const { return m_timestamp; }
};

};

#endif
