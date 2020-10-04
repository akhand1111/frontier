/*
 * Frontier - A toolkit for creating simple OS-independent user interfaces
 * Copyright (C) 2018, 2020 Ian Parker <ian@geekprojects.com>
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

#ifndef __FRONTIER_WIDGETS_H_
#define __FRONTIER_WIDGETS_H_

#include <utility>
#include <vector>
#include <map>
#include <set>

#include <frontier/frontier.h>
#include <frontier/styles.h>
#include <frontier/types.h>
#include <frontier/value.h>

#include <sigc++/sigc++.h>

#define WIDGET_SIZE_UNLIMITED 0xffff

namespace Frontier {

class Menu;

/**
 * \brief Describes and caches the CSS box model of a widget
 */
struct BoxModel
{
    int marginTop;
    int marginRight;
    int marginBottom;
    int marginLeft;

    int paddingTop;
    int paddingRight;
    int paddingBottom;
    int paddingLeft;

    int borderTopWidth;
    int borderRightWidth;
    int borderBottomWidth;
    int borderLeftWidth;

    int getTop() const { return marginTop + paddingTop + borderTopWidth; }
    int getRight() const { return marginRight + paddingRight + borderRightWidth; }
    int getBottom() const { return marginBottom + paddingBottom + borderBottomWidth; }
    int getLeft() const { return marginLeft + paddingLeft + borderLeftWidth; }

    int getWidth() const { return getLeft() + getRight(); }
    int getHeight() const { return getTop() + getBottom(); }
};

/**
 * \defgroup widgets Widgets
 */

/**
 * \brief Base class for all widgets
 *
 * The Widget class provides all of the basic display, event handling and other basic
 * functionallity that all widgets share.
 *
 * \ingroup widgets
 */
class Widget : public FrontierObject, public Geek::Logger
{
 private:
    bool m_initialised;
    void* m_privateData;
    Menu* m_contextMenu;

    std::unordered_map<std::wstring, Value> m_properties;

    /// Set of all CSS classes associated with this widget
    std::set<std::wstring> m_widgetClasses;

    /// Timestamp of the Styles when cached
    uint64_t m_styleTimestamp;

    /// Styles set directly on the Widget
    StyleRule m_widgetStyleProperties;

    /// Cached style properties
    std::unordered_map<std::string, Value> m_cachedStyleProperties;

    /// Cached box model style properties
    BoxModel m_cachedBoxModel;
    uint64_t m_cachedBoxModelTimestamp;
    Geek::FontHandle* m_cachedTextFont;
    uint64_t m_cachedTextFontTimestamp;

    sigc::signal<void, bool> m_mouseEnterSignal;
    sigc::signal<void> m_activeSignal;
    sigc::signal<void> m_inactiveSignal;

    sigc::signal<bool, Widget*, Geek::Vector2D> m_dragOverSignal;
    sigc::signal<bool, Widget*, Geek::Vector2D> m_dragDropSignal;
    sigc::signal<bool, Widget*> m_dragCancelledSignal;
    sigc::signal<void, Widget*> m_clickSignal;
    sigc::signal<void, Widget*> m_doubleClickSignal;

    void initWidget(FrontierApp* app, std::wstring widgetName);
    void callInit();

    BoxModel& getBoxModel(std::unordered_map<std::string, Value>& properties);
    Value getStyle(std::string style, std::unordered_map<std::string, Value>& properties);
    static bool hasStyle(std::string style, std::unordered_map<std::string, Value>& properties);

 protected:
    /// The Application that this widget belongs to
    FrontierApp* m_app;

    /// The name of the type of widget
    std::wstring m_widgetName;

    /// The widget types extended by this widget
    std::set<std::wstring> m_widgetNames;

    /// Application-wide unique id for this widget
    std::wstring m_widgetId;

    /// Window that this widget is currently attached to
    FrontierWindow* m_window;

    /// Widget that contains this widget
    Widget* m_parent;

    /// Widgets that this Widget contains
    std::vector<Widget*> m_children;

    /// Position of the widget, relative to it's parent
    Geek::Vector2D m_position;

    /// Minimum size this Widget can be. Set during calculateSize()
    Frontier::Size m_minSize;

    /// Maximum size this Widget can be. Set during calculateSize()
    Frontier::Size m_maxSize;

    /// Maximum size this Widget can be. Set during calculateSize()
    Frontier::Size m_setSize;

    /// Flags indicating what aspects of this Widget are dirty
    unsigned int m_dirty;

    /// True if this widget is in a selected state
    bool m_selected;

    /// Return the size of the borders of this Widget
    Size getBorderSize();

    /// Draw the borders to the specified surface
    bool drawBorder(Geek::Gfx::Surface* surface);

    /// Get font used for text, as specified by CSS
    Geek::FontHandle* getTextFont();

    /// Draw text using the Widget's CSS styling
    void drawText(Geek::Gfx::Surface* surface, int x, int y, std::wstring str, Geek::FontHandle* font = NULL);

 protected:
    /// Initialise this Widget. Widgets should override this to initialise themselves
    virtual void init();

 public:
    Widget(FrontierApp* ui, std::wstring name);
    ~Widget() override;

    /// Get the Widget type name
    std::wstring getWidgetName() { return m_widgetName; }

    /// Return whether this Widget is or overrides the given widget name
    virtual bool instanceOf(const std::wstring widgetName) { return (m_widgetName == widgetName || m_widgetNames.count(widgetName) > 0); }

    /// Set application-specific data. The Widget and Frontier library should not touch this data
    void setPrivateData(void* data) { m_privateData = data; }

    /// Get application-specific data
    void* getPrivateData() const { return m_privateData; }

    /// Return the child Widgets contained by this Widget
    virtual std::vector<Widget*> getChildren() { return m_children; }

    /// Return whether this Widget contains any children
    virtual bool hasChildren() { return !(getChildren().empty()); }

    virtual void add(Widget* widget);

    virtual Widget* findById (const std::wstring id);

    virtual void remove(Widget* widget);

    /// Calculate the minimum and maximum size this Widget can be
    virtual void calculateSize();

    /// Layout the child widgets of this Widget
    virtual void layout();

    /// Draw the widget
    virtual bool draw(Geek::Gfx::Surface* surface);

    /// Set the position of the widget relative to it's parent Widget. Should only used by layout()
    void setPosition(int x, int y);

    /// Set the size of the widget. Should only used by layout()
    Frontier::Size setSize(Frontier::Size size);

    /**
     * Draw the widget
     * Only the area in "rect" will be drawn to the window. By default, this just calls
     * draw(Surface*) above.
     */
    virtual bool draw(Geek::Gfx::Surface* surface, Rect visible);

    /*
     * Properties
     */
    void setProperty(std::wstring property, Value value);
    Value getProperty(std::wstring property);

    /*
     * Style
     */

    /// Return whether the specified style is set by the Style Sheet
    bool hasStyle(std::string style);

    /// Return whether the value of the style set by the Style Sheet for this Widget
    Value getStyle(std::string style);

    /// Set the specified CSS property on this Widget
    void setStyle(std::string style, Value value);

    /// Set the unique id of this Widget
    void setWidgetId(std::wstring id) { m_widgetId = std::move(id); }

    /// Get the uniqie id of this Widget
    std::wstring getWidgetId() { return m_widgetId; }

    /// Check whether this widget has the given style class
    bool hasWidgetClass(std::wstring className);

    /// Set the specified style class
    void setWidgetClass(std::wstring className);

    /// Remove the specified style class
    void clearWidgetClass(std::wstring className);

    /// Return all style classes applied to this Widget
    std::set<std::wstring> getWidgetClasses() { return m_widgetClasses; }

    /// Return all the styles applied directly to this Widget
    StyleRule* getWidgetStyle() { return &m_widgetStyleProperties; }

    // Return all style properties either directly or via CSS
    std::unordered_map<std::string, Value>& getStyleProperties();

    /// Return the CSS box model
    BoxModel& getBoxModel();

    /// Return the position of this Widget relative to it's parent
    Geek::Vector2D getPosition() const { return m_position; }
    int getX() const { return m_position.x; }
    int getY() const { return m_position.y; }

    Frontier::Size getSize() const { return m_setSize; }
    Frontier::Size getMinSize() { return m_minSize; }
    Frontier::Size getMaxSize() const { return m_maxSize; }
    virtual int getWidth() const { return m_setSize.width; }
    virtual int getHeight() const { return m_setSize.height; }

    void setParent(Widget* w);
    Widget* getParent() const { return m_parent; }
    Widget* findParent(const std::type_info& type);

    void setWindow(FrontierWindow* window) { m_window = window; }
    FrontierWindow* getWindow();
    FrontierApp* getApp() const { return m_app; }

    /// Return the position of thiw Widget relative to the Window
    virtual Geek::Vector2D getAbsolutePosition() const;

    /// Check whether the specified point lies inside this widget. The position is relative to it's parent
    bool intersects(int x, int y) const;

    /// Set all dirty flags
    void setDirty();

    /**
     * Set specified dirty flags.
     * If children is true, recurse through children, otherwise, set on parent Widget
     * \see Frontier::DirtyFlag
     */
    void setDirty(unsigned int flags, bool children = false);

    /// Check if any dirty flags are set
    bool isDirty() const { return !!(m_dirty); }

    /// Check if specified DirtyFlag is set
    bool isDirty(DirtyFlag flag) const { return !!(m_dirty & flag); }

    /// Clear all DirtyFlags and recurse through the Widget's children
    virtual void clearDirty();

    /// Mark this widget as being active
    void setActive();

    /// Return whether this Widget is active or not
    bool isActive();

    /// Activate the next Widget. Used with tabbing through Widgets
    void activateNext() { activateNext(NULL); }
    virtual void activateNext(Widget* activeChild);

    bool isVisible() { return isVisible(NULL); }
    virtual bool isVisible(Frontier::Widget* child);

    /// Return whether the mouse is currently over this Widget
    bool isMouseOver();

    /// Return whether the Widget is in a selected state or not
    bool isSelected() const { return m_selected; }

    /// Associate a Menu with this widget
    void setContextMenu(Menu* menu) { m_contextMenu = menu; }

    /// Return the associated context Menu
    Menu* getContextMenu() const { return m_contextMenu; }

    /// Open the context menu for this Widget as a popup window
    void openContextMenu();

    /// Open the context menu for this Widget as a popup window at the specified Window position
    void openContextMenu(Geek::Vector2D pos);

    /// Handle user input event
    virtual Widget* handleEvent(Frontier::Event* msg);

    /// Called when the mouse pointer moves over this Widget. Widgets can override this
    virtual void onMouseEnter();

    /// Called when the mouse pointer leaves this Widget. Widgets can override this
    virtual void onMouseLeave();

    /// Signal that fires when the mouse pointer moves over this Widget
    virtual sigc::signal<void, bool> signalMouseEnter() { return m_mouseEnterSignal; }

    /// Signal that fires when this Widget is activated
    virtual sigc::signal<void> signalActive() { return m_activeSignal; }

    /// Signal that fires when this Widget is no longer active
    virtual sigc::signal<void> signalInactive() { return m_inactiveSignal; }

    /// Signal that fires when a Widget being dragged moves
    virtual sigc::signal<bool, Widget*, Geek::Vector2D> dragOverSignal() { return m_dragOverSignal; }

    /// Signal that fires when a Widget being dragged is dropped
    virtual sigc::signal<bool, Widget*, Geek::Vector2D> dragDropSignal() { return m_dragDropSignal; }

    /// Signal that fires when a Widget being dragged is no longer being dragged
    virtual sigc::signal<bool, Widget*> dragCancelledSignal() { return m_dragCancelledSignal; }

    /// Signal that fires when the Widget receives a single mouse click
    virtual sigc::signal<void, Widget*> clickSignal() { return m_clickSignal; }

    /// Signal that fires when the Widget receives a double mouse click
    virtual sigc::signal<void, Widget*> doubleClickSignal() { return m_doubleClickSignal; }

    /// Returns the current mouse cursor shape. Override this to display different cursors
    virtual Frontier::WindowCursor getCursor() { return Frontier::CURSOR_ARROW; }

    /// For debug. Dump this widget to stdout
    virtual void dump(int level);
};

class WidgetInit
{
 private:
 public:
    WidgetInit() = default;
    virtual ~WidgetInit() = default;

    virtual Widget* create(FrontierApp* app) { return NULL; }

    virtual const std::string getName() { return ""; }
};

class WidgetRegistry
{
 private:
    std::map<std::string, WidgetInit*> m_widgets;

 public:
    WidgetRegistry();
    virtual ~WidgetRegistry();

    static void registerWidget(WidgetInit* init);
    static Widget* createWidget(FrontierApp* app, std::string name);
};

#define FRONTIER_WIDGET(_name, _class)  \
    class _name##Init : public Frontier::WidgetInit \
    { \
     private: \
        static _name##Init* const init __attribute__ ((unused)); \
     public: \
        _name##Init() \
        { \
            WidgetRegistry::registerWidget(this); \
        } \
        const std::string getName() \
        { \
            return #_name; \
        } \
        _name* create(FrontierApp* app) \
        { \
            return new _name(app); \
        } \
    }; \
    _name##Init* const _name##Init::init = new _name##Init();

}

#endif
