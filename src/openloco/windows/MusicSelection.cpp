#include "../Config.h"
#include "../OpenLoco.h"
#include "../audio/audio.h"
#include "../graphics/colours.h"
#include "../graphics/image_ids.h"
#include "../interop/interop.hpp"
#include "../localisation/FormatArguments.hpp"
#include "../localisation/string_ids.h"
#include "../objects/interface_skin_object.h"
#include "../objects/objectmgr.h"
#include "../ui/WindowManager.h"

using namespace openloco::interop;

namespace openloco::ui::windows::music_selection
{
    static const gfx::ui_size_t window_size = { 360, 238 };

    static const uint8_t rowHeight = 12; // CJK: 15

    enum widx
    {
        frame,
        title,
        close,
        panel,
        scrollview,
    };

    static widget_t _widgets[] = {
        makeWidget({ 0, 0 }, { 360, 238 }, widget_type::frame, 0),
        makeWidget({ 1, 1 }, { 358, 13 }, widget_type::caption_25, 0, string_ids::music_selection_title),
        makeWidget({ 345, 2 }, { 13, 13 }, widget_type::wt_9, 0, image_ids::close_button, string_ids::tooltip_close_window),
        makeWidget({ 0, 15 }, { 360, 223 }, widget_type::panel, 1),
        makeWidget({ 4, 19 }, { 352, 218 }, widget_type::scrollview, 1, scrollbars::vertical, string_ids::music_selection_tooltip),
        widgetEnd(),
    };

    static window_event_list _events;

    static void draw(ui::window* window, gfx::drawpixelinfo_t* dpi);
    static void drawScroll(ui::window* window, gfx::drawpixelinfo_t* dpi, uint32_t scrollIndex);
    static void getScrollSize(ui::window* window, uint32_t scrollIndex, uint16_t* scrollWidth, uint16_t* scrollHeight);
    static void onMouseUp(ui::window* window, widget_index widgetIndex);
    static void onScrollMouseDown(ui::window* window, int16_t x, int16_t y, uint8_t scroll_index);
    static void onScrollMouseOver(ui::window* window, int16_t x, int16_t y, uint8_t scroll_index);
    static void onUpdate(window* window);
    static void tooltip(FormatArguments& args, ui::window* window, widget_index widgetIndex);

    static void initEvents()
    {
        _events.draw = draw;
        _events.draw_scroll = drawScroll;
        _events.get_scroll_size = getScrollSize;
        _events.on_mouse_up = onMouseUp;
        _events.on_update = onUpdate;
        _events.scroll_mouse_down = onScrollMouseDown;
        _events.scroll_mouse_over = onScrollMouseOver;
        _events.tooltip = tooltip;
    }

    // 0x004C1602
    window* open()
    {
        window* window = WindowManager::bringToFront(WindowType::musicSelection, 0);
        if (window != nullptr)
            return window;

        window = WindowManager::createWindow(
            WindowType::musicSelection,
            window_size,
            0,
            &_events);

        // TODO: only needs to be called once.
        initEvents();

        window->widgets = _widgets;
        window->enabled_widgets = 1 << widx::close;
        window->initScrollWidgets();

        auto interface = objectmgr::get<interface_skin_object>();
        window->colours[0] = interface->colour_0B;
        window->colours[1] = interface->colour_10;

        window->row_count = audio::num_music_tracks;
        window->row_hover = -1;

        return window;
    }

    // 0x004C165D
    static void draw(ui::window* window, gfx::drawpixelinfo_t* dpi)
    {
        // Draw widgets.
        window->draw(dpi);
    }

    // 0x004C1663
    static void drawScroll(ui::window* window, gfx::drawpixelinfo_t* dpi, uint32_t scrollIndex)
    {
        auto shade = colour::getShade(window->colours[1], 4);
        gfx::clearSingle(*dpi, shade);

        auto config = config::get();

        uint16_t y = 0;
        for (uint16_t i = 0; i < window->row_count; i++)
        {
            string_id text_colour_id = string_ids::black_stringid;

            // Draw hovered track
            if (i == window->row_hover)
            {
                gfx::drawRect(dpi, 0, y, 800, rowHeight, 0x2000030);
                text_colour_id = string_ids::wcolour2_stringid;
            }

            // Draw checkbox.
            gfx::fillRectInset(dpi, 2, y, 11, y + 10, window->colours[1], 0xE0);

            // Draw checkmark if track is enabled.
            if (config.enabled_music[i])
                gfx::drawString_494B3F(*dpi, 2, y, window->colours[1], string_ids::wcolour2_stringid, (void*)&string_ids::checkmark);

            // Draw track name.
            string_id music_title_id = audio::getMusicInfo(i)->title_id;
            gfx::drawString_494B3F(*dpi, 15, y, window->colours[1], text_colour_id, (void*)&music_title_id);

            y += rowHeight;
        }
    }

    // 0x004C176C
    static void getScrollSize(ui::window* window, uint32_t scrollIndex, uint16_t* scrollWidth, uint16_t* scrollHeight)
    {
        *scrollHeight = rowHeight * audio::num_music_tracks;
    }

    // 0x004C1757
    static void onMouseUp(ui::window* window, widget_index widgetIndex)
    {
        switch (widgetIndex)
        {
            case widx::close:
                WindowManager::close(window->type);
                break;
        }
    }

    // 0x004C1799
    static void onScrollMouseDown(ui::window* window, int16_t x, int16_t y, uint8_t scroll_index)
    {
        uint16_t currentTrack = y / rowHeight;
        if (currentTrack > window->row_count)
            return;

        auto& config = config::get();

        // Toggle the track in question.
        config.enabled_music[currentTrack] = !config.enabled_music[currentTrack];

        // Are any tracks enabled?
        uint8_t anyEnabled = 0;
        for (uint8_t i = 0; i < audio::num_music_tracks; i++)
            anyEnabled |= config.enabled_music[i];

        // Ensure at least this track is enabled.
        if (!anyEnabled)
            config.enabled_music[currentTrack] = true;

        config::write();
        audio::revalidateCurrentTrack();
        window->invalidate();
    }

    // 0x004C1771
    static void onScrollMouseOver(ui::window* window, int16_t x, int16_t y, uint8_t scroll_index)
    {
        uint16_t currentTrack = y / rowHeight;
        if (currentTrack > window->row_count || currentTrack == window->row_hover)
            return;

        window->row_hover = currentTrack;
        window->invalidate();
    }

    // 0x004C17E3
    static void onUpdate(window* window)
    {
        auto optionsWindow = WindowManager::find(WindowType::options);
        if (optionsWindow == nullptr || optionsWindow->current_tab != options::tab_offset_music)
        {
            WindowManager::close(window);
            return;
        }
    }

    // 0x004C1762
    static void tooltip(FormatArguments& args, ui::window* window, widget_index widgetIndex)
    {
        args.push(string_ids::tooltip_scroll_list);
    }
}
