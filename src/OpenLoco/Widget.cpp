#include "Widget.h"
#include "Graphics/Colour.h"
#include "Graphics/ImageIds.h"
#include "Input.h"
#include "Interop/Interop.hpp"
#include "Ui/ScrollView.h"
#include "Window.h"
#include <cassert>

using namespace OpenLoco::Interop;

namespace OpenLoco::Ui
{
    int16_t Widget::mid_x() const
    {
        return (this->left + this->right) / 2;
    }

    int16_t Widget::mid_y() const
    {
        return (this->top + this->bottom) / 2;
    }

    uint16_t Widget::width() const
    {
        return (this->right - this->left) + 1;
    }

    uint16_t Widget::height() const
    {
        return (this->bottom - this->top) + 1;
    }

    static loco_global<int32_t, 0x112C876> _currentFontSpriteBase;
    static loco_global<char[512], 0x0112CC04> stringFormatBuffer;
    static loco_global<char[1], 0x112C826> _commonFormatArgs;

    static loco_global<char[2], 0x005045F8> _strCheckmark;

    void draw_11_c(Gfx::Context* context, const Window* window, Widget* widget, uint8_t colour, bool disabled, int16_t x, int16_t y, string_id string);
    void draw_14(Gfx::Context* context, Widget* widget, uint8_t colour, bool disabled, int16_t x, int16_t y, string_id string);

    static void sub_4CF3EB(const Gfx::Context* context, const Window* window, const Widget* widget, int16_t x, int16_t y, uint8_t colour, int16_t width)
    {
        registers regs;
        regs.eax = colour;
        regs.bx = width;
        regs.cx = x;
        regs.dx = y;
        regs.esi = (uint32_t)window;
        regs.edi = (uint32_t)context;
        regs.ebp = (uint32_t)widget;
        call(0x004CF3EB, regs);
    }

    void Widget::draw(Gfx::Context* context, Window* window, const uint64_t pressedWidgets, const uint64_t toolWidgets, const uint64_t hoveredWidgets, uint8_t& scrollviewIndex)
    {
        if ((window->flags & WindowFlags::no_background) == 0)
        {
            // Check if widget is outside the draw region
            if (window->x + left >= context->x + context->width && window->x + right < context->x)
            {
                if (window->y + top >= context->y + context->height && window->y + bottom < context->y)
                {
                    return;
                }
            }
        }

        uint16_t widgetFlags = 0;
        if (windowColour == WindowColour::primary && window->flags & WindowFlags::flag_11)
        {
            widgetFlags = 0x80;
        }

        uint8_t wndColour = window->getColour(windowColour);
        auto widgetIndex = this - &window->widgets[0];
        bool enabled = (window->enabled_widgets & (1ULL << widgetIndex)) != 0;
        bool disabled = (window->disabled_widgets & (1ULL << widgetIndex)) != 0;
        bool activated = (window->activated_widgets & (1ULL << widgetIndex)) != 0;
        activated |= (pressedWidgets & (1ULL << widgetIndex)) != 0;
        activated |= (toolWidgets & (1ULL << widgetIndex)) != 0;
        bool hovered = (hoveredWidgets & (1ULL << widgetIndex)) != 0;

        switch (type)
        {
            case WidgetType::none:
            case WidgetType::end:
                break;

            case WidgetType::panel:
                drawPanel(context, window, widgetFlags, wndColour);
                break;

            case WidgetType::frame:
                drawFrame(context, window, widgetFlags, wndColour);
                break;

            case WidgetType::wt_3:
                draw_3(context, window,  widgetFlags, wndColour, enabled, disabled, activated);
                break;

            case WidgetType::wt_4:
                assert(false); // Unused
                break;

            case WidgetType::wt_5:
            case WidgetType::wt_6:
            case WidgetType::wt_7:
            case WidgetType::wt_8:
                draw_5(context, window, widgetFlags, wndColour, enabled, disabled, activated);
                break;

            case WidgetType::wt_9:
                draw_9(context, window, widgetFlags, wndColour, enabled, disabled, activated, hovered);
                break;

            case WidgetType::wt_10:
                draw_10(context, window, widgetFlags, wndColour, enabled, disabled, activated, hovered);
                break;

            case WidgetType::wt_11:
            case WidgetType::wt_12:
            case WidgetType::wt_14:
                if (type == WidgetType::wt_12)
                {
                    assert(false); // Unused
                }
                draw_11_a(context, window, widgetFlags, wndColour, enabled, disabled, activated);
                draw_13(context, window, widgetFlags, wndColour, enabled, disabled, activated);
                break;

            case WidgetType::wt_13:
                draw_13(context, window, widgetFlags, wndColour, enabled, disabled, activated);
                break;

            case WidgetType::wt_15:
                draw_15(context, window, widgetFlags, wndColour, disabled);
                break;

            case WidgetType::groupbox:
                // NB: widget type 16 has been repurposed to add groupboxes; the original type 16 was unused.
                drawGroupbox(context, window);
                break;

            case WidgetType::wt_17:
            case WidgetType::wt_18:
            case WidgetType::viewport:
                draw_17(context, window, widgetFlags, wndColour);
                draw_15(context, window, widgetFlags, wndColour, disabled);
                break;

            case WidgetType::wt_20:
            case WidgetType::wt_21:
                assert(false); // Unused
                break;

            case WidgetType::caption_22:
                draw_22_caption(context, window, widgetFlags, wndColour);
                break;

            case WidgetType::caption_23:
                draw_23_caption(context, window, widgetFlags, wndColour);
                break;

            case WidgetType::caption_24:
                draw_24_caption(context, window, widgetFlags, wndColour);
                break;

            case WidgetType::caption_25:
                draw_25_caption(context, window, widgetFlags, wndColour);
                break;

            case WidgetType::scrollview:
                drawScrollview(context, window, widgetFlags, wndColour, enabled, disabled, activated, hovered, scrollviewIndex);
                scrollviewIndex++;
                break;

            case WidgetType::checkbox:
                draw_27_checkbox(context, window, widgetFlags, wndColour, enabled, disabled, activated);
                draw_27_label(context, window, widgetFlags, wndColour, disabled);
                break;

            case WidgetType::wt_28:
                assert(false); // Unused
                draw_27_label(context, window, widgetFlags, wndColour, disabled);
                break;

            case WidgetType::wt_29:
                assert(false); // Unused
                draw_29(context, window);
                break;
        }
    }
    // 0x004CF487
    void Widget::drawViewportCentreButton(Gfx::Context* context, const Window* window, const WidgetIndex_t widgetIndex)
    {
        auto& widget = window->widgets[widgetIndex];
        if (Input::isHovering(window->type, window->number, widgetIndex))
        {
            Gfx::drawRect(context, widget.left + window->x, widget.top + window->y, widget.width(), widget.height(), 0x2000000 | 54);
            Gfx::drawRect(context, widget.left + window->x, widget.top + window->y, widget.width(), widget.height(), 0x2000000 | 52);

            uint8_t flags = 0;
            if (Input::isPressed(window->type, window->number, widgetIndex))
                flags = 0x20;

            Gfx::drawRectInset(context, widget.left + window->x, widget.top + window->y, widget.width(), widget.height(), Colour::translucent(window->getColour(WindowColour::secondary)), flags);
        }

        Gfx::drawImage(context, widget.left + window->x, widget.top + window->y, Gfx::recolour(ImageIds::centre_viewport, window->getColour(WindowColour::secondary)));
    }

    // 0x004CAB8E
    static void draw_resize_handle(Gfx::Context* context, const Window* window, Widget* widget, uint8_t colour)
    {
        if (!(window->flags & WindowFlags::resizable))
        {
            return;
        }

        if (window->min_height == window->max_height || window->min_width == window->max_width)
        {
            return;
        }

        int16_t x = widget->right + window->x - 18;
        int16_t y = widget->bottom + window->y - 18;
        uint32_t image = 0x20000000 | 2305 | (colour << 19);
        Gfx::drawImage(context, x, y, image);
    }

    void Widget::sub_4CADE8(Gfx::Context* context, const Window* window, uint8_t colour, bool enabled, bool disabled, bool activated)
    {
        int16_t x_place_for_image = left + window->x;
        int16_t y_place_for_image = top + window->y;
        uint32_t new_image = image;
        if (type == WidgetType::wt_6 || type == WidgetType::wt_7 || type == WidgetType::wt_8 || type == WidgetType::wt_4)
        {
            if (activated)
            {
                // TODO: remove image addition
                new_image++;
            }
        }

        if (disabled)
        {
            if (new_image & (1 << 31))
            {
                return;
            }

            new_image &= 0x7FFFF;
            uint8_t c;
            if (colour & OpenLoco::Colour::translucent_flag)
            {
                c = OpenLoco::Colour::getShade(colour & 0x7F, 4);
                Gfx::drawImageSolid(context, x_place_for_image + 1, y_place_for_image + 1, new_image, c);
                c = OpenLoco::Colour::getShade(colour & 0x7F, 2);
                Gfx::drawImageSolid(context, x_place_for_image, y_place_for_image, new_image, c);
            }
            else
            {
                c = OpenLoco::Colour::getShade(colour & 0x7F, 6);
                Gfx::drawImageSolid(context, x_place_for_image + 1, y_place_for_image + 1, new_image, c);
                c = OpenLoco::Colour::getShade(colour & 0x7F, 4);
                Gfx::drawImageSolid(context, x_place_for_image, y_place_for_image, new_image, c);
            }

            return;
        }

        if (new_image & (1 << 31))
        {
            // 0x4CAE5F
            assert(false);
        }

        if ((new_image & (1 << 30)) == 0)
        {
            new_image |= colour << 19;
        }
        else
        {
            new_image &= ~(1 << 30);
        }

        Gfx::drawImage(context, x_place_for_image, y_place_for_image, new_image);
    }

    // 0x004CAB58
    void Widget::drawPanel(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        Gfx::fillRectInset(context, window->x + left, window->y + top, window->x + right, window->y + bottom, colour, flags);

        draw_resize_handle(context, window, this, colour);
    }

    // 0x004CAAB9
    void Widget::drawFrame(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        Gfx::Context* clipped = nullptr;
        if (Gfx::clipContext(&clipped, context, left + window->x, top + window->y, right - left, 41))
        {
            uint32_t frame = image;
            if (window->flags & WindowFlags::flag_11)
            {
                frame = 0x20000000 | 2322 | ((colour & 0x7F) << 19);
            }
            else
            {
                frame = 0x20000000 | 2323 | ((colour & 0x7F) << 19);
            }
            Gfx::drawImage(clipped, 0, 0, frame);
        }

        uint8_t shade;
        if (window->flags & WindowFlags::flag_11)
        {
            shade = OpenLoco::Colour::getShade(colour, 3);
        }
        else
        {
            shade = OpenLoco::Colour::getShade(colour, 1);
        }

        Gfx::fillRect(
            context,
            window->x + right,
            window->y + top,
            window->x + right,
            window->y + top + 40,
            shade);

        draw_resize_handle(context, window, this, colour);
    }

    void Widget::draw_3(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated)
    {
        int16_t top_rect, left_rect, bottom_rect, right_rect;
        top_rect = window->y + top;
        left_rect = window->x + left;
        right_rect = window->x + right;
        bottom_rect = window->y + bottom;

        if (activated)
        {
            flags |= 0x20;
        }

        if (content == -2)
        {
            flags |= 0x10;
            Gfx::fillRectInset(context, left_rect, top_rect, right_rect, bottom_rect, colour, flags);
            return;
        }

        if (window->flags & WindowFlags::flag_6)
        {
            Gfx::fillRect(context, left_rect, top_rect, right_rect, bottom_rect, 0x2000000 | 52);
        }

        Gfx::fillRectInset(context, left_rect, top_rect, right_rect, bottom_rect, colour, flags);

        if (content == -1)
        {
            return;
        }

        sub_4CADE8(context, window, colour, enabled, disabled, activated);
    }

    // 0x004CABFE
    void Widget::draw_5(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated)
    {
        if (content == -1)
        {
            return;
        }

        if (!disabled)
        {
            sub_4CADE8(context, window, colour, enabled, disabled, activated);
            return;
        }

        if (type == WidgetType::wt_8)
        {
            return;
        }

        if (type != WidgetType::wt_7)
        {
            sub_4CADE8(context, window, colour, enabled, disabled, activated);
            return;
        }

        // TODO: Remove added_image addition
        uint32_t added_image = image + 2;

        if ((added_image & (1 << 30)) == 0)
        {
            added_image |= colour << 19;
        }
        else
        {
            added_image &= ~(1 << 30);
        }

        Gfx::drawImage(context, window->x + left, window->y + top, added_image);
    }

    // 0x004CACD4
    void Widget::draw_9(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated, bool hovered)
    {
        if (!disabled && hovered)
        {
            // TODO: Fix mixed windows
            draw_3(context, window, flags, colour, enabled, disabled, activated);
            return;
        }

        int l = left + window->x;
        int t = top + window->y;
        int r = right + window->x;
        int b = bottom + window->y;

        if (activated)
        {
            flags |= 0x20;
            if (content == -2)
            {
                // 0x004CABE8

                flags |= 0x10;
                Gfx::fillRectInset(context, l, t, r, b, colour, flags);

                return;
            }

            Gfx::fillRectInset(context, l, t, r, b, colour, flags);
        }

        if (content == -1)
        {
            return;
        }

        sub_4CADE8(context, window, colour, enabled, disabled, activated);
    }

    // 0x004CAC5F
    void Widget::draw_10(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated, bool hovered)
    {
        if (content == -1)
        {
            return;
        }

        uint32_t added_image = image;

        if (enabled)
        {
            // TODO: Remove added_image addition
            added_image += 2;

            if (!activated)
            {
                added_image -= 1;

                if (!hovered)
                {
                    added_image -= 1;
                }
            }
        }

        if ((added_image & (1 << 30)) == 0)
        {
            added_image |= colour << 19;
        }
        else
        {
            added_image &= ~(1 << 30);
        }

        Gfx::drawImage(context, window->x + left, window->y + top, added_image);
    }

    // 0x004CB164
    void Widget::draw_11_a(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated)
    {
        int rect_left = window->x + left;
        int rect_right = window->x + right;
        int rect_top = window->y + top;
        int rect_bottom = window->y + bottom;

        if (activated)
        {
            flags |= 0x20;
        }

        Gfx::fillRectInset(context, rect_left, rect_top, rect_right, rect_bottom, colour, flags);
    }

    // 0x004CB1BE
    void Widget::draw_13(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated)
    {
        if (content == -1)
        {
            return;
        }

        int16_t x = window->x + left;
        int16_t y = window->y + std::max<int16_t>(top, (top + bottom) / 2 - 5);
        string_id string = text;

        // TODO: Refactor out Widget type check
        if (type == WidgetType::wt_12)
        {
            if (activated)
            {
                // TODO: Remove string addition
                string++;
            }
        }

        if (type == WidgetType::wt_14)
        {
            draw_14(context, this, colour, disabled, x, y, string);
        }
        else
        {
            draw_11_c(context, window, this, colour, disabled, x, y, string);
        }
    }

    // 0x004CB21D
    void draw_11_c(Gfx::Context* context, const Window* window, Widget* widget, uint8_t colour, bool disabled, int16_t x, int16_t y, string_id string)
    {
        colour &= 0x7F;
        if (disabled)
        {
            colour |= FormatFlags::textflag_6;
        }

        int16_t centreX = window->x + (widget->left + widget->right + 1) / 2 - 1;
        int16_t width = widget->right - widget->left - 2;
        Gfx::drawStringCentredClipped(*context, centreX, y, width, colour, string, _commonFormatArgs);
    }

    // 0x004CB263
    void draw_14(Gfx::Context* context, Widget* widget, uint8_t colour, bool disabled, int16_t x, int16_t y, string_id string)
    {
        x = x + 1;

        colour &= 0x7F;
        if (disabled)
        {
            colour |= FormatFlags::textflag_6;
        }

        int width = widget->right - widget->left - 2;
        Gfx::drawString_494BBF(*context, x, y, width, colour, string, _commonFormatArgs);
    }

    // 0x4CB2D6
    void Widget::draw_15(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool disabled)
    {
        if (content == -1 || content == -2)
        {
            return;
        }

        uint8_t c = FormatFlags::fd;
        if (disabled)
        {
            c = colour | FormatFlags::textflag_6;
        }

        drawString_494B3F(*context, window->x + left + 1, window->y + top, c, text, _commonFormatArgs);
    }

    // 0x4CB29C
    void Widget::draw_17(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        Gfx::fillRectInset(context, window->x + left, window->y + top, window->x + right, window->y + bottom, colour, flags | 0x60);
    }

    // 0x004CA6AE
    void Widget::draw_22_caption(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        int rect_left = window->x + left;
        int rect_right = window->x + right;
        int rect_top = window->y + top;
        int rect_bottom = window->y + bottom;
        Gfx::fillRectInset(context, rect_left, rect_top, rect_right, rect_bottom, colour, flags | 0x60);
        Gfx::fillRect(context, rect_left + 1, rect_top + 1, rect_right - 1, rect_bottom - 1, 0x2000000 | 46);

        int16_t width = rect_right - rect_left - 4 - 10;
        int16_t y = rect_top + window->y + 1;
        int16_t x = rect_left + window->x + 2 + (width / 2);

        Gfx::drawStringCentredClipped(*context, x, y, width, Colour::white | FormatFlags::textflag_5, text, _commonFormatArgs);
    }

    // 0x004CA750
    void Widget::draw_23_caption(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        stringFormatBuffer[0] = ControlCodes::colour_black;
        StringManager::formatString(&stringFormatBuffer[1], text, _commonFormatArgs);

        int16_t width = right - left - 4 - 14;
        int16_t x = left + window->x + 2 + (width / 2);

        _currentFontSpriteBase = Font::medium_bold;
        width = Gfx::clipString(width - 8, stringFormatBuffer);

        x -= width / 2;
        int16_t y = window->y + top + 1;

        sub_4CF3EB(context, window, this, x, y, colour, width);

        Gfx::drawString(context, x, y, Colour::black, stringFormatBuffer);
    }

    // 0x004CA7F6
    void Widget::draw_24_caption(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        stringFormatBuffer[0] = ControlCodes::window_colour_1;
        StringManager::formatString(&stringFormatBuffer[1], text, _commonFormatArgs);

        int16_t x = left + window->x + 2;
        int16_t width = right - left - 4 - 14;
        x = x + (width / 2);

        _currentFontSpriteBase = Font::medium_bold;
        int16_t stringWidth = Gfx::clipString(width - 8, stringFormatBuffer);
        x -= (stringWidth - 1) / 2;

        Gfx::drawString(context, x, window->y + top + 1, FormatFlags::textflag_5 | Colour::black, stringFormatBuffer);
    }

    // 0x004CA88B
    void Widget::draw_25_caption(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour)
    {
        stringFormatBuffer[0] = ControlCodes::colour_white;
        StringManager::formatString(&stringFormatBuffer[1], text, _commonFormatArgs);

        int16_t x = left + window->x + 2;
        int16_t width = right - left - 4 - 14;
        x = x + (width / 2);

        _currentFontSpriteBase = Font::medium_bold;
        int16_t stringWidth = Gfx::clipString(width - 8, stringFormatBuffer);
        x -= (stringWidth - 1) / 2;

        Gfx::drawString(context, x, window->y + top + 1, FormatFlags::textflag_5 | Colour::black, stringFormatBuffer);
    }

    static void draw_hscroll(Gfx::Context* context, const Window* window, Widget* widget, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated, bool hovered, int16_t scrollview_index)
    {
        const auto* scroll_area = &window->scroll_areas[scrollview_index];

        uint16_t ax = window->x + widget->left + 1;
        uint16_t cx = window->y + widget->top + 1;
        uint16_t bx = window->x + widget->right - 1;
        uint16_t dx = window->y + widget->bottom - 1;

        cx = dx - 10;
        if (scroll_area->flags & Ui::ScrollView::ScrollFlags::vscrollbarVisible)
        {
            bx -= 11;
        }

        uint16_t f;

        // pusha
        f = 0;
        if (scroll_area->flags & Ui::ScrollView::ScrollFlags::hscrollbarLeftPressed)
        {
            f = flags | 0x20;
        }
        Gfx::fillRectInset(context, ax, cx, ax + 9, dx, colour, f);
        // popa

        // pusha
        Gfx::drawString(context, ax + 2, cx, Colour::black, (char*)0x005045F2);
        // popa

        // pusha
        f = 0;
        if (scroll_area->flags & Ui::ScrollView::ScrollFlags::hscrollbarRightPressed)
        {
            f = flags | 0x20;
        }
        Gfx::fillRectInset(context, bx - 9, cx, bx, dx, colour, f);
        // popa

        // pusha
        Gfx::drawString(context, bx - 6 - 1, cx, Colour::black, (char*)0x005045F5);
        // popa

        // pusha
        Gfx::fillRect(context, ax + 10, cx, bx - 10, dx, Colour::getShade(colour, 7));
        Gfx::fillRect(context, ax + 10, cx, bx - 10, dx, 0x1000000 | Colour::getShade(colour, 3));
        // popa

        // pusha
        Gfx::fillRect(context, ax + 10, cx + 2, bx - 10, cx + 2, Colour::getShade(colour, 3));
        Gfx::fillRect(context, ax + 10, cx + 3, bx - 10, cx + 3, Colour::getShade(colour, 7));
        Gfx::fillRect(context, ax + 10, cx + 7, bx - 10, cx + 7, Colour::getShade(colour, 3));
        Gfx::fillRect(context, ax + 10, cx + 8, bx - 10, cx + 8, Colour::getShade(colour, 7));
        // popa

        // pusha
        f = 0;
        if (scroll_area->flags & Ui::ScrollView::ScrollFlags::hscrollbarThumbPressed)
        {
            f = 0x20;
        }
        Gfx::fillRectInset(context, ax - 1 + scroll_area->h_thumb_left, cx, ax - 1 + scroll_area->h_thumb_right, dx, colour, f);
        // popa
    }

    static void draw_vscroll(Gfx::Context* context, const Window* window, Widget* widget, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated, bool hovered, int16_t scrollview_index)
    {
        const auto* scroll_area = &window->scroll_areas[scrollview_index];

        uint16_t ax = window->x + widget->left + 1;
        uint16_t cx = window->y + widget->top + 1;
        uint16_t bx = window->x + widget->right - 1;
        uint16_t dx = window->y + widget->bottom - 1;

        ax = bx - 10;
        if (scroll_area->flags & ScrollView::ScrollFlags::hscrollbarVisible)
        {
            dx -= 11;
        }

        uint16_t f;

        // pusha
        f = 0;
        if (scroll_area->flags & ScrollView::ScrollFlags::vscrollbarUpPressed)
        {
            f = flags | 0x20;
        }
        Gfx::fillRectInset(context, ax, cx, bx, cx + 9, colour, f);
        // popa

        // pusha
        Gfx::drawString(context, ax + 1, cx - 1, Colour::black, (char*)0x005045EC);
        // popa

        // pusha
        f = 0;
        if (scroll_area->flags & ScrollView::ScrollFlags::vscrollbarDownPressed)
        {
            f = flags | 0x20;
        }
        Gfx::fillRectInset(context, ax, dx - 9, bx, dx, colour, f);
        // popa

        // pusha
        Gfx::drawString(context, ax + 1, dx - 8 - 1, Colour::black, (char*)0x005045EF);
        // popa

        // pusha
        Gfx::fillRect(context, ax, cx + 10, bx, dx - 10, Colour::getShade(colour, 7));
        Gfx::fillRect(context, ax, cx + 10, bx, dx - 10, 0x1000000 | Colour::getShade(colour, 3));
        // popa

        // pusha
        Gfx::fillRect(context, ax + 2, cx + 10, ax + 2, dx - 10, Colour::getShade(colour, 3));
        Gfx::fillRect(context, ax + 3, cx + 10, ax + 3, dx - 10, Colour::getShade(colour, 7));
        Gfx::fillRect(context, ax + 7, cx + 10, ax + 7, dx - 10, Colour::getShade(colour, 3));
        Gfx::fillRect(context, ax + 8, cx + 10, ax + 8, dx - 10, Colour::getShade(colour, 7));
        // popa

        // pusha
        f = 0;
        if (scroll_area->flags & ScrollView::ScrollFlags::vscrollbarThumbPressed)
        {
            f = flags | 0x20;
        }
        Gfx::fillRectInset(context, ax, cx - 1 + scroll_area->v_thumb_top, bx, cx - 1 + scroll_area->v_thumb_bottom, colour, f);
        // popa
    }

    // 0x004CB31C
    void Widget::drawScrollview(Gfx::Context* context, Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated, bool hovered, int scrollview_index)
    {
        int16_t scroll_view_left = window->x + left;
        int16_t scroll_view_top = window->y + top;
        int16_t scroll_view_right = window->x + right;
        int16_t scroll_view_bottom = window->y + bottom;

        Gfx::fillRectInset(context, scroll_view_left, scroll_view_top, scroll_view_right, scroll_view_bottom, colour, flags | 0x60);

        scroll_view_left++;
        scroll_view_top++;
        scroll_view_right--;
        scroll_view_bottom--;

        const auto* scroll_area = &window->scroll_areas[scrollview_index];

        _currentFontSpriteBase = Font::medium_bold;
        if (scroll_area->flags & Ui::ScrollView::ScrollFlags::hscrollbarVisible)
        {
            draw_hscroll(context, window, this, flags, colour, enabled, disabled, activated, hovered, scrollview_index);
            scroll_view_bottom -= 11;
        }

        if (scroll_area->flags & Ui::ScrollView::ScrollFlags::vscrollbarVisible)
        {
            draw_vscroll(context, window, this, flags, colour, enabled, disabled, activated, hovered, scrollview_index);
            scroll_view_right -= 11;
        }

        Gfx::Context cropped = *context;
        scroll_view_bottom++;
        scroll_view_right++;

        if (scroll_view_left > cropped.x)
        {
            int offset = scroll_view_left - cropped.x;
            cropped.width -= offset;
            cropped.x = scroll_view_left;
            cropped.pitch += offset;

            cropped.bits += offset;
        }

        int16_t bp = cropped.x + cropped.width - scroll_view_right;
        if (bp > 0)
        {
            cropped.width -= bp;
            cropped.pitch += bp;
        }

        if (scroll_view_top > cropped.y)
        {
            int offset = scroll_view_top - cropped.y;
            cropped.height -= offset;
            cropped.y = scroll_view_top;

            int aex = (cropped.pitch + cropped.width) * offset;
            cropped.bits += aex;
        }

        bp = cropped.y + cropped.height - scroll_view_bottom;
        if (bp > 0)
        {
            cropped.height -= bp;
        }

        if (cropped.width > 0 && cropped.height > 0)
        {
            cropped.x -= scroll_view_left - scroll_area->contentOffsetX;
            cropped.y -= scroll_view_top - scroll_area->contentOffsetY;

            window->callDrawScroll(&cropped, scrollview_index);
        }
    }

    // 0x004CB00B
    void Widget::draw_27_checkbox(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool enabled, bool disabled, bool activated)
    {
        if (enabled)
        {
            Gfx::fillRectInset(
                context,
                window->x + left,
                window->y + top,
                window->x + left + 9,
                window->y + bottom - 1,
                colour,
                flags | 0x60);
        }

        if (activated)
        {
            _currentFontSpriteBase = Font::medium_bold;
            Gfx::drawString(context, window->x + left, window->y + top, colour & 0x7F, _strCheckmark);
        }
    }

    // 0x004CB080
    void Widget::draw_27_label(Gfx::Context* context, const Window* window, uint16_t flags, uint8_t colour, bool disabled)
    {
        if (content == -1)
        {
            return;
        }

        colour &= 0x7F;

        if (disabled)
        {
            colour |= FormatFlags::textflag_6;
        }

        Gfx::drawString_494B3F(*context, window->x + left + 14, window->y + top, colour, text, _commonFormatArgs);
    }

    // 0x004CA679
    void Widget::draw_29(Gfx::Context* context, const Window* window)
    {
        int rect_left = window->x + left;
        int rect_right = window->x + right;
        int rect_top = window->y + top;
        int rect_bottom = window->y + bottom;
        Gfx::fillRect(context, rect_left, rect_top, rect_right, rect_bottom, Colour::getShade(Colour::black, 5));
    }

    void Widget::drawGroupbox(Gfx::Context* const context, const Window* window)
    {
        const uint8_t colour = window->getColour(windowColour) & 0x7F;
        int32_t l = window->x + left + 5;
        int32_t t = window->y + top;
        int32_t r = window->x + right;
        int32_t b = window->y + bottom;
        int32_t textEndPos = l;

        // First, draw the label text, if any.
        if (text != StringIds::null)
        {
            char buffer[512] = { 0 };
            StringManager::formatString(buffer, sizeof(buffer), text);

            Gfx::drawString(context, l, t, colour, buffer);
            textEndPos = l + Gfx::getStringWidth(buffer) + 1;
        }

        // Prepare border dimensions
        l = window->x + left;
        t = window->y + top + 4;
        r = window->x + right;
        b = window->y + bottom;

        // Border left of text
        Gfx::fillRect(context, l, t, l + 4, t, Colour::getShade(colour, 4));
        Gfx::fillRect(context, l + 1, t + 1, l + 4, t + 1, Colour::getShade(colour, 7));

        // Border right of text
        Gfx::fillRect(context, textEndPos, t, r - 1, t, Colour::getShade(colour, 4));
        Gfx::fillRect(context, textEndPos, t + 1, r - 2, t + 1, Colour::getShade(colour, 7));

        // Border right
        Gfx::fillRect(context, r - 1, t + 1, r - 1, b - 1, Colour::getShade(colour, 4));
        Gfx::fillRect(context, r, t, r, b, Colour::getShade(colour, 7));

        // Border bottom
        Gfx::fillRect(context, l, b - 1, r - 2, b - 1, Colour::getShade(colour, 4));
        Gfx::fillRect(context, l, b, r - 1, b, Colour::getShade(colour, 7));

        // Border left
        Gfx::fillRect(context, l, t + 1, l, b - 2, Colour::getShade(colour, 4));
        Gfx::fillRect(context, l + 1, t + 2, l + 1, b - 2, Colour::getShade(colour, 7));
    }

    // 0x004CF194
    void Widget::drawTab(Window* w, Gfx::Context* ctx, int32_t imageId, WidgetIndex_t index)
    {
        auto widget = &w->widgets[index];

        Gfx::point_t pos = {};
        pos.x = widget->left + w->x;
        pos.y = widget->top + w->y;

        if (w->isDisabled(index))
        {
            return; // 0x8000
        }

        bool isActivated = false;
        if (w->isActivated(index))
        {
            isActivated = true;
        }
        else if (Input::state() == Input::State::widgetPressed)
        {
            isActivated = Input::isPressed(w->type, w->number, index);
        }

        if (imageId == -1)
        {
            return;
        }

        if (isActivated)
        {
            if (imageId != -2)
            {
                Gfx::drawImage(ctx, pos.x, pos.y, imageId);
            }
        }
        else
        {
            if (imageId != -2)
            {
                Gfx::drawImage(ctx, pos.x, pos.y + 1, imageId);
            }
            Gfx::drawImage(ctx, pos.x, pos.y, (1 << 30) | (51 << 19) | ImageIds::tab);
            Gfx::drawRect(ctx, pos.x, pos.y + 26, 31, 1, Colour::getShade(w->getColour(WindowColour::secondary), 7));
        }
    }
}
