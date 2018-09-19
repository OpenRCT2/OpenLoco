#include "environment.h"
#include "config.h"
#include "interop/interop.hpp"
#include "platform/platform.h"
#include "ui.h"
#include "utility/collection.hpp"
#include "utility/string.hpp"
#include <cstring>
#include <fstream>
#include <iostream>

using namespace openloco;
using namespace openloco::interop;

environment openloco::g_env;

static loco_global<char[260], 0x009D118E> _path_buffer;
static loco_global<char[257], 0x0050B0CE> _path_install;
static loco_global<char[257], 0x0050B1CF> _path_saves_single_player;
static loco_global<char[257], 0x0050B2EC> _path_saves_two_player;
static loco_global<char[257], 0x0050B406> _path_scenarios;
static loco_global<char[257], 0x0050B518> _path_landscapes;
static loco_global<char[257], 0x0050B635> _path_objects;

bool environment::validate_loco_install_path(const fs::path& path) const
{
    if (path.empty())
    {
        return false;
    }
    else
    {
        auto g1Path = path / get_sub_path(path_id::g1);
        bool g1Exists = fs::exists(g1Path);
#ifndef _WIN32
        if (!g1Exists)
        {
            g1Path = find_similar_file(g1Path);
            g1Exists = !g1Path.empty();
        }
#endif
        return g1Exists;
    }
}

openloco::fs::path environment::auto_detect_loco_install_path()
{
    static constexpr const char* searchPaths[] = {
        "C:/Program Files (x86)/Atari/Locomotion",
        "C:/GOG Games/Chris Sawyer's Locomotion",
    };

    std::cout << "Searching for Locomotion install path..." << std::endl;
    for (auto path : searchPaths)
    {
        if (validate_loco_install_path(path))
        {
            std::cout << "  found: " << path << std::endl;
            return path;
        }
    }
    return fs::path();
}

openloco::fs::path environment::resolve_loco_install_path()
{
    auto& cfg = config::get_new();
    auto path = fs::path(cfg.loco_install_path);
    if (!path.empty())
    {
        if (validate_loco_install_path(path))
        {
            config::write_new_config(*this);
            return path;
        }
        std::cerr << "Configured install path for Locomotion is missing Data/g1.DAT." << std::endl;
    }

    path = auto_detect_loco_install_path();
    if (!path.empty())
    {
#ifdef _OPENLOCO_USE_BOOST_FS_
        cfg.loco_install_path = path.make_preferred().string();
#else
        cfg.loco_install_path = path.make_preferred().u8string();
#endif
        config::write_new_config(*this);
        return path;
    }
    else
    {
        std::cerr << "Unable to find install path for Locomotion." << std::endl
                  << "You will need to manually provide it." << std::endl;
        ui::show_message_box("OpenLoco", "Select your Locomotion install path.");
        path = platform::prompt_directory("Select your Locomotion install path.");
        if (validate_loco_install_path(path))
        {
#ifdef _OPENLOCO_USE_BOOST_FS_
            cfg.loco_install_path = path.make_preferred().string();
#else
            cfg.loco_install_path = path.make_preferred().u8string();
#endif
            config::write_new_config(*this);
            return path;
        }

        std::cerr << "Path is missing g1.dat." << std::endl;
        ui::show_message_box("OpenLoco", "Path is missing Data/g1.DAT.");
        std::exit(-1);
    }
}

openloco::fs::path environment::get_loco_install_path() const
{
    return _path_install.get();
}

#ifndef _WIN32
/**
     * Performs a case-insensitive search on the containing directory of
     * the given file and returns the first match.
     */
openloco::fs::path environment::find_similar_file(const fs::path& path) const
{
    try
    {
        auto expectedFilename = path.filename().generic_string();
        auto directory = path.parent_path();
        for (auto& item : fs::directory_iterator(directory))
        {
            auto& p = item.path();
            if (utility::iequals(p.filename().generic_string(), expectedFilename))
            {
                return p;
            }
        }
    }
    catch (const std::exception&)
    {
        // Ignore errors when searching, most common will be that the
        // parent directory does not exist
    }
    return fs::path();
}
#endif // _WIN32

// 0x004416B5
openloco::fs::path environment::get_path(environment::path_id id) const
{
    auto basePath = get_base_path(id);
    auto subPath = get_sub_path(id);
    auto result = basePath / subPath;
    if (!fs::exists(result))
    {
#ifndef _WIN32
        auto similarResult = find_similar_file(result);
        if (similarResult.empty())
        {
            std::cerr << "File not found: " << result << std::endl;
        }
        else
        {
            result = similarResult;
        }
#else

        std::cerr << "File not found: " << result << std::endl;
#endif
    }
    return result;
}

template<typename T>
static void set_directory(T& buffer, openloco::fs::path path)
{
#ifdef _OPENLOCO_USE_BOOST_FS_
    utility::strcpy_safe(buffer, path.make_preferred().string().c_str());
#else
    utility::strcpy_safe(buffer, path.make_preferred().u8string().c_str());
#endif
}

// 0x004412CE
void environment::resolve_paths()
{
    auto basePath = resolve_loco_install_path();
    set_directory(_path_install, basePath);
    set_directory(_path_saves_single_player, basePath / "Single Player Saved Games/");
    set_directory(_path_saves_two_player, basePath / "Two Player Saved Games/");
    set_directory(_path_scenarios, basePath / "Scenarios/*.SC5");
    set_directory(_path_landscapes, basePath / "Scenarios/Landscapes/*.SC5");
    set_directory(_path_objects, basePath / "ObjData/*.DAT");
}

openloco::fs::path environment::get_base_path(path_id id) const
{
    switch (id)
    {
        case path_id::plugin1:
        case path_id::plugin2:
        case path_id::gamecfg:
        case path_id::scores:
        case path_id::openloco_yml:
            return platform::get_user_directory();
        default:
            return get_loco_install_path();
    }
}

openloco::fs::path environment::get_sub_path(environment::path_id id) const
{
    static constexpr const char* paths[] = {
        "Data/g1.DAT",
        "plugin.dat",
        "plugin2.dat",
        "Data/CSS1.DAT",
        "Data/CSS2.DAT",
        "Data/CSS3.DAT",
        "Data/CSS4.DAT",
        "Data/CSS5.DAT",
        "game.cfg",
        "Data/KANJI.DAT",
        "Data/20s1.DAT",
        "Data/20s2.DAT",
        "Data/20s4.DAT",
        "Data/50s1.DAT",
        "Data/50s2.DAT",
        "Data/70s1.DAT",
        "Data/70s2.DAT",
        "Data/70s3.DAT",
        "Data/80s1.DAT",
        "Data/90s1.DAT",
        "Data/90s2.DAT",
        "Data/rag3.DAT",
        "Data/Chrysanthemum.DAT",
        "Data/Eugenia.DAT",
        "Data/Rag2.DAT",
        "Data/Rag1.DAT",
        "Data/20s3.DAT",
        "Data/40s1.DAT",
        "Data/40s2.DAT",
        "Data/50s3.DAT",
        "Data/40s3.DAT",
        "Data/80s2.DAT",
        "Data/60s1.DAT",
        "Data/80s3.DAT",
        "Data/60s2.DAT",
        "Data/60s3.DAT",
        "Data/80s4.DAT",
        "Data/20s5.DAT",
        "Data/20s6.DAT",
        "Data/title.dat",
        "scores.dat",
        "Scenarios/Boulder Breakers.SC5",
        "Data/TUT1024_1.DAT",
        "Data/TUT1024_2.DAT",
        "Data/TUT1024_3.DAT",
        "Data/TUT800_1.DAT",
        "Data/TUT800_2.DAT",
        "Data/TUT800_3.DAT",
        "openloco.yml"
    };

    size_t index = (size_t)id;
    if (index >= utility::length(paths))
    {
        throw std::runtime_error("Invalid path_id: " + std::to_string((int32_t)id));
    }
    return paths[(size_t)id];
}
