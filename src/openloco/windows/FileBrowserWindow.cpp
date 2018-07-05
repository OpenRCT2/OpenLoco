#include <algorithm>
#include <cstring>
#ifdef _OPENLOCO_USE_BOOST_FS_
#include <boost/filesystem.hpp>
#else
#include <experimental/filesystem>
#endif
#include "../graphics/colours.h"
#include "../input.h"
#include "../interop/interop.hpp"
#include "../openloco.h"
#include "../ui.h"
#include "../utility/string.hpp"
#include "../windowmgr.h"

using namespace openloco::interop;
using namespace openloco::ui;

#ifdef _OPENLOCO_USE_BOOST_FS_
namespace fs = boost::filesystem;
#else
namespace fs = std::experimental::filesystem;
#endif

namespace openloco::windows::FileBrowserWindow
{
    static fs::path getDirectory(const fs::path& path);
    static std::string getBasename(const fs::path& path);

    enum class BrowseFileType
    {
        saved_game,
        landscape
    };

    loco_global<ui::widget_t[9], 0x0050AD58> _widgets;

    loco_global<uint8_t, 0x009D9D63> _type;
    loco_global<uint8_t, 0x009DA284> _fileType;
    loco_global<char[256], 0x009D9D64> _title;
    loco_global<char[32], 0x009D9E64> _filter;
    loco_global<char[512], 0x009D9E84> _directory;
    loco_global<char[512], 0x011369A0> _text_input_buffer;

    static void sub_446A93()
    {
        call(0x00446A93);
    }

    static void sub_4CEB67(int16_t dx)
    {
        registers regs;
        regs.dx = dx;
        call(0x004CEB67, regs);
    }

    // 0x00445AB9
    // ecx: path
    // edx: filter
    // ebx: title
    // eax: {return}
    bool open(
        BrowseType type,
        char* szPath,
        const char* filter,
        const char* title)
    {
        auto path = fs::path(szPath);
        auto directory = getDirectory(path);
        auto baseName = getBasename(path);

        TextInputWindow::cancel();

        _type = (uint8_t)type;
        _fileType = (uint8_t)BrowseFileType::saved_game;
        if (utility::iequals(filter, "*.sc5"))
        {
            _fileType = (uint8_t)BrowseFileType::landscape;
        }
        utility::strcpy_safe(_title, title);
        utility::strcpy_safe(_filter, filter);

#ifdef _OPENLOCO_USE_BOOST_FS_
        utility::strcpy_safe(_directory, directory.make_preferred().string().c_str());
#else
        utility::strcpy_safe(_directory, directory.make_preferred().u8string().c_str());
#endif
        utility::strcpy_safe(_text_input_buffer, baseName.c_str());

        sub_446A93();
        auto window = windowmgr::create_window_centred(WindowType::fileBrowser, 500, 380, ui::WindowFlags::stickToFront | ui::WindowFlags::resizable | ui::WindowFlags::flag_12, (ui::window_event_list*)0x004FB308);
        if (window != nullptr)
        {
            window->widgets = &_widgets[0];
            window->setEnabledWidgets(2, 4, 6);
            window->initScrollWidgets();
            addr<0x01136FA2, int16_t>() = -1;
            addr<0x011370A9, uint8_t>() = 0;
            addr<0x01136FA4, int16_t>() = 0;
            window->var_83E = 11;
            window->var_85A = 0xFFFF;
            addr<0x009DA285, uint8_t>() = 0;
            sub_4CEB67(_widgets[5].width());
            window->colours[0] = colour::black;
            window->colours[1] = colour::saturated_green;
            windowmgr::current_modal_type(WindowType::fileBrowser);
            prompt_tick_loop(
                []() {
                    input::handle_keyboard();
                    sub_48A18C();
                    windowmgr::dispatch_update_all();
                    call(0x004BEC5B);
                    windowmgr::update();
                    call(0x004C98CF);
                    call(0x004CF63B);
                    return windowmgr::find(WindowType::fileBrowser) != nullptr;
                });
            windowmgr::current_modal_type(WindowType::undefined);
            // TODO: use utility::strlcpy with the buffer size instead of std::strcpy, if possible
            std::strcpy(szPath, _directory);
            if (szPath[0] != '\0')
            {
                return true;
            }
        }
        return false;
    }

    static fs::path getDirectory(const fs::path& path)
    {
        if (path.has_extension())
        {
            std::basic_string<fs::path::value_type> sep(1, fs::path::preferred_separator);
            return path.parent_path().concat(sep);
        }
        else
        {
            auto str = path.string();
            if (str.size() > 0)
            {
                auto lastCharacter = str[str.size() - 1];
                if (lastCharacter == fs::path::preferred_separator)
                {
                    return path;
                }
            }
            return fs::path();
        }
    }

    static std::string getBasename(const fs::path& path)
    {
#ifdef _OPENLOCO_USE_BOOST_FS_
        auto baseName = path.stem().string();
#else
        auto baseName = path.stem().u8string();
#endif
        if (baseName == ".")
        {
            baseName = "";
        }
        return baseName;
    }

    // 0x00446A93
    static void refreshDirectoryList()
    {
        call(0x00446A93);
    }

    // 0x00446E2F
    static void upOneLevel()
    {
        char* ptr = _directory;
        while (*ptr != '\0')
            ptr++;

        ptr--;
        if (*ptr == fs::path::preferred_separator)
            ptr--;

        while (ptr != _directory && *ptr != fs::path::preferred_separator)
            ptr--;

        if (*ptr == fs::path::preferred_separator)
            ptr++;

        *ptr = '\0';

        refreshDirectoryList();
    }

    // 0x00446E62
    static void appendDirectory(const char* to_append)
    {
        char* dst = _directory;
        while (*dst != '\0')
            dst++;

        const char* src = to_append;
        while (*src != '\0')
        {
            *dst++ = *src++;
        }

        *dst++ = fs::path::preferred_separator;
        *dst = '\0';

        refreshDirectoryList();
    }

    void registerHooks()
    {
        register_hook(
            0x00445AB9,
            [](registers& regs) FORCE_ALIGN_ARG_POINTER -> uint8_t {
                auto result = open(
                    (BrowseType)regs.al,
                    (char*)regs.ecx,
                    (const char*)regs.edx,
                    (const char*)regs.ebx);
                regs.eax = result ? 1 : 0;
                return 0;
            });

        register_hook(
            0x00446E2F,
            [](registers& regs) FORCE_ALIGN_ARG_POINTER -> uint8_t {
                upOneLevel();
                return 0;
            });

        register_hook(
            0x00446E62,
            [](registers& regs) FORCE_ALIGN_ARG_POINTER -> uint8_t {
                appendDirectory((char*)regs.ebp);
                return 0;
            });
    }
}
