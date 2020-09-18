#include "../Config.h"
#include "../Graphics/Colour.h"
#include "../Graphics/Gfx.h"
#include "../Graphics/ImageIds.h"
#include "../Interop/Interop.hpp"
#include "../Localisation/FormatArguments.hpp"
#include "../Localisation/StringIds.h"
#include "../Objects/ObjectManager.h"
#include "../Ui/WindowManager.h"

using namespace OpenLoco::Interop;

namespace OpenLoco::Ui::Windows::ScenarioSelect
{
    constexpr Gfx::ui_size_t windowSize = { 610, 412 };

    namespace widx
    {
        enum
        {
            frame,
            title,
            close,
            panel,
            tab0,
            tab1,
            tab2,
            tab3,
            tab4,
            list,
        };
    }

    static widget_t _widgets[] = {
        makeWidget({ 0, 0 }, { 610, 412 }, widget_type::frame, 0),
        makeWidget({ 1, 1 }, { 608, 13 }, widget_type::caption_25, 0, StringIds::select_scenario_for_new_game),
        makeWidget({ 595, 2 }, { 13, 13 }, widget_type::wt_9, 0, ImageIds::close_button, StringIds::tooltip_close_window),
        makeWidget({ 0, 48 }, { 610, 364 }, widget_type::wt_3, 1),
        makeRemapWidget({ 3, 15 }, { 91, 34 }, widget_type::wt_8, 1, ImageIds::wide_tab),
        makeRemapWidget({ 94, 15 }, { 91, 34 }, widget_type::wt_8, 1, ImageIds::wide_tab),
        makeRemapWidget({ 185, 15 }, { 91, 34 }, widget_type::wt_8, 1, ImageIds::wide_tab),
        makeRemapWidget({ 276, 15 }, { 91, 34 }, widget_type::wt_8, 1, ImageIds::wide_tab),
        makeRemapWidget({ 367, 15 }, { 91, 34 }, widget_type::wt_8, 1, ImageIds::wide_tab),
        makeWidget({ 3, 52 }, { 431, 356 }, widget_type::scrollview, 1, scrollbars::vertical),
        widgetEnd(),
    };

    static window_event_list _events;

    static void initEvents();

    // 0x00443807
    static void initTabs()
    {
        call(0x00443807);
    }

    // 0x00443868
    window* open()
    {
        window* self = WindowManager::bringToFront(WindowType::scenarioSelect);
        if (self != nullptr)
            return self;

        initEvents();

        self = WindowManager::createWindow(
            WindowType::scenarioSelect,
            Gfx::point_t({ static_cast<int16_t>(width() / 2 - windowSize.width / 2),
                           std::max<int16_t>(height() / 2 - windowSize.height / 2, 28) }),
            windowSize,
            window_flags::transparent,
            &_events);

        self->widgets = _widgets;
        self->enabled_widgets = (1 << widx::close) | (1 << widx::tab0) | (1 << widx::tab1) | (1 << widx::tab2) | (1 << widx::tab3) | (1 << widx::tab4);
        self->initScrollWidgets();

        self->colours[0] = PaletteIndex::transparent;
        self->colours[1] = PaletteIndex::index_0B;

        self->var_846 = 0xFFFF;
        self->var_85A = static_cast<int32_t>(0xFFFFFFFF);

        initTabs();

        // auto& config = config::get();
        // config.scenario_selected_tab;

        return self;
    }

    // 0x00443995
    static void prepareDraw(window* self)
    {
        self->activated_widgets &= ~((1 << widx::tab0) | (1 << widx::tab1) | (1 << widx::tab2) | (1 << widx::tab3) | (1 << widx::tab4));
        self->activated_widgets |= (1 << (self->current_tab + static_cast<uint8_t>(widx::tab0)));
    }

    // 0x004439AF
    static void draw(window* self, Gfx::drawpixelinfo_t* dpi)
    {
        // Draw widgets.
        self->draw(dpi);
    }

    // 0x00443D02
    static void drawScroll(window*, Gfx::drawpixelinfo_t* const dpi, uint32_t)
    {
    }

    // 0x00443E9B
    static void onMouseUp(window* self, const widget_index widgetIndex)
    {
        switch (widgetIndex)
        {
            case widx::close:
                WindowManager::close(self->type);
                break;
        }
    }

    // 0x00443EA6
    static void onMouseDown(window* self, widget_index widgetIndex)
    {
    }

    // 0x00443EF6
    static void getScrollSize(window*, uint32_t, uint16_t*, uint16_t* const scrollHeight)
    {
    }

    // 0x00443F32
    static void onScrollMouseDown(window* self, int16_t x, int16_t y, uint8_t scroll_index)
    {
    }

    // 0x00443FB2
    static void onScrollMouseOver(window* self, int16_t x, int16_t y, uint8_t scroll_index)
    {
    }

    // 0x00444001
    static void tooltip(FormatArguments& args, window* self, widget_index widgetIndex)
    {
        args.push(StringIds::tooltip_scroll_scenario_list);
    }

    static void initEvents()
    {
        _events.prepare_draw = prepareDraw;
        _events.draw = draw;
        _events.draw_scroll = drawScroll;
        _events.on_mouse_up = onMouseUp;
        _events.on_mouse_down = onMouseDown;
        _events.get_scroll_size = getScrollSize;
        _events.scroll_mouse_down = onScrollMouseDown;
        _events.scroll_mouse_over = onScrollMouseOver;
        _events.tooltip = tooltip;
    }
}
