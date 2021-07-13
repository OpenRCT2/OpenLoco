#pragma once

#include <cstdint>

#define UNUSED_IMG(x) (x)

namespace OpenLoco::ImageIds
{
    constexpr uint32_t null = 0xFFFFFFFF;

    constexpr uint32_t currency_symbol = 1919;

    constexpr uint32_t text_palette = 2169;

    constexpr uint32_t window_resize_handle = 2305;
    constexpr uint32_t colour_swatch_recolourable = 2306;
    constexpr uint32_t colour_swatch_recolourable_raised = 2307;
    constexpr uint32_t colour_swatch_recolourable_pressed = 2308;

    constexpr uint32_t company_list_dropdown_icon = 2309;
    constexpr uint32_t icon_parent_folder = 2310;
    constexpr uint32_t icon_folder = 2311;

    constexpr uint32_t curved_border_left = 2315;
    constexpr uint32_t curved_border_right = 2316;

    constexpr uint32_t close_button = 2321;
    constexpr uint32_t frame_background_image = 2322;
    constexpr uint32_t frame_background_image_alt = 2323;
    constexpr uint32_t inline_green_up_arrow = 2324;
    constexpr uint32_t inline_red_down_arrow = 2325;
    constexpr uint32_t progressbar_style0_frame0 = 2326;
    constexpr uint32_t progressbar_style0_frame1 = 2327;
    constexpr uint32_t progressbar_style0_frame2 = 2328;
    constexpr uint32_t progressbar_style0_frame3 = 2329;
    constexpr uint32_t progressbar_track = 2330;
    constexpr uint32_t progressbar_style1_frame0 = 2331;
    constexpr uint32_t progressbar_style1_frame1 = 2332;
    constexpr uint32_t progressbar_style1_frame2 = 2333;
    constexpr uint32_t progressbar_style1_frame3 = 2334;
    constexpr uint32_t construction_straight = 2335;
    constexpr uint32_t step_back = 2336;
    constexpr uint32_t step_forward = 2337;
    constexpr uint32_t red_arrow_up = 2338;
    constexpr uint32_t red_arrow_down = 2339;
    constexpr uint32_t construction_left_hand_curve_very_small = 2340;
    constexpr uint32_t construction_right_hand_curve_very_small = 2341;
    constexpr uint32_t construction_left_hand_curve_small = 2342;
    constexpr uint32_t construction_right_hand_curve_small = 2343;
    constexpr uint32_t construction_left_hand_curve = 2344;
    constexpr uint32_t construction_right_hand_curve = 2345;
    constexpr uint32_t construction_left_hand_curve_large = 2346;
    constexpr uint32_t construction_right_hand_curve_large = 2347;
    constexpr uint32_t construction_steep_slope_down = 2348;
    constexpr uint32_t construction_slope_down = 2349;
    constexpr uint32_t construction_level = 2350;
    constexpr uint32_t construction_slope_up = 2351;
    constexpr uint32_t construction_steep_slope_up = 2352;
    constexpr uint32_t construction_s_bend_left = 2353;
    constexpr uint32_t construction_s_bend_right = 2354;
    constexpr uint32_t construction_s_bend_dual_track_left = 2355;
    constexpr uint32_t construction_s_bend_dual_track_right = 2356;
    constexpr uint32_t construction_s_bend_to_single_track_left = 2357;
    constexpr uint32_t construction_s_bend_to_single_track_right = 2358;
    constexpr uint32_t construction_right_turnaround = 2359;
    constexpr uint32_t construction_left_turnaround = 2360;
    constexpr uint32_t construction_remove = 2361;
    constexpr uint32_t construction_new_position = 2362;
    constexpr uint32_t rubbish_bin = 2363;
    constexpr uint32_t centre_viewport = 2364;
    constexpr uint32_t rotate_object = 2365;

    constexpr uint32_t red_flag = 2369;
    constexpr uint32_t green_flag = 2370;
    constexpr uint32_t yellow_flag = 2371;
    constexpr uint32_t airport_pickup = 2372;
    constexpr uint32_t airport_place = 2373;
    constexpr uint32_t pass_signal = 2374;
    constexpr uint32_t route_delete = 2375;
    constexpr uint32_t route_skip = 2376;
    constexpr uint32_t route_wait = 2377;
    constexpr uint32_t route_force_unload = 2378;
    constexpr uint32_t show_station_catchment = 2379;
    constexpr uint32_t plant_cluster_selected_tree = 2380;
    constexpr uint32_t plant_cluster_random_tree = 2381;
    constexpr uint32_t town_expand = 2382;
    constexpr uint32_t music_controls_stop = 2383;
    constexpr uint32_t music_controls_play = 2384;
    constexpr uint32_t music_controls_next = 2385;
    constexpr uint32_t refit_cargo_button = 2386;
    constexpr uint32_t tab_vehicle_background = 2387;

    constexpr uint32_t tab_display = 2391;
    constexpr uint32_t tab_control = 2392;
    constexpr uint32_t tab_sound = 2393;
    constexpr uint32_t tab_miscellaneous = 2394;
    constexpr uint32_t tab_globe_0 = 2395;
    constexpr uint32_t tab_globe_1 = 2396;
    constexpr uint32_t tab_globe_2 = 2397;
    constexpr uint32_t tab_globe_3 = 2398;
    constexpr uint32_t tab_globe_4 = 2399;
    constexpr uint32_t tab_globe_5 = 2400;
    constexpr uint32_t tab_globe_6 = 2401;
    constexpr uint32_t tab_globe_7 = 2402;
    constexpr uint32_t tab_globe_8 = 2403;
    constexpr uint32_t tab_globe_9 = 2404;
    constexpr uint32_t tab_globe_10 = 2405;
    constexpr uint32_t tab_globe_11 = 2406;
    constexpr uint32_t tab_globe_12 = 2407;
    constexpr uint32_t tab_globe_13 = 2408;
    constexpr uint32_t tab_globe_14 = 2409;
    constexpr uint32_t tab_globe_15 = 2410;
    constexpr uint32_t tab_globe_16 = 2411;
    constexpr uint32_t tab_globe_17 = 2412;
    constexpr uint32_t tab_globe_18 = 2413;
    constexpr uint32_t tab_globe_19 = 2414;
    constexpr uint32_t tab_globe_20 = 2415;
    constexpr uint32_t tab_globe_21 = 2416;
    constexpr uint32_t tab_globe_22 = 2417;
    constexpr uint32_t tab_globe_23 = 2418;
    constexpr uint32_t tab_globe_24 = 2419;
    constexpr uint32_t tab_globe_25 = 2420;
    constexpr uint32_t tab_globe_26 = 2421;
    constexpr uint32_t tab_globe_27 = 2422;
    constexpr uint32_t tab_globe_28 = 2423;
    constexpr uint32_t tab_globe_29 = 2424;
    constexpr uint32_t tab_globe_30 = 2425;
    constexpr uint32_t tab_globe_31 = 2426;
    constexpr uint32_t tab_music_0 = 2427;
    constexpr uint32_t tab_music_1 = 2428;
    constexpr uint32_t tab_music_2 = 2429;
    constexpr uint32_t tab_music_3 = 2430;
    constexpr uint32_t tab_music_4 = 2431;
    constexpr uint32_t tab_music_5 = 2432;
    constexpr uint32_t tab_music_6 = 2433;
    constexpr uint32_t tab_music_7 = 2434;
    constexpr uint32_t tab_music_8 = 2435;
    constexpr uint32_t tab_music_9 = 2436;
    constexpr uint32_t tab_music_10 = 2437;
    constexpr uint32_t tab_music_11 = 2438;
    constexpr uint32_t tab_music_12 = 2439;
    constexpr uint32_t tab_music_13 = 2440;
    constexpr uint32_t tab_music_14 = 2441;
    constexpr uint32_t tab_music_15 = 2442;
    constexpr uint32_t decrease_tool_area = 2443;

    constexpr uint32_t increase_tool_area = 2445;

    constexpr uint32_t tool_area = 2447;

    constexpr uint32_t tab = 2387;
    constexpr uint32_t selected_tab = 2388;

    constexpr uint32_t speed_pause = 2460;
    constexpr uint32_t speed_pause_active = 2461;
    constexpr uint32_t speed_normal = 2462;
    constexpr uint32_t speed_normal_active = 2463;
    constexpr uint32_t speed_fast_forward = 2464;
    constexpr uint32_t speed_fast_forward_active = 2465;
    constexpr uint32_t speed_extra_fast_forward = 2466;
    constexpr uint32_t speed_extra_fast_forward_active = 2467;
    constexpr uint32_t random_map_watermark = 2468;
    constexpr uint32_t height_map_compass = 2469;

    constexpr uint32_t number_circle_00 = 3238;
    constexpr uint32_t number_circle_01 = 3239;
    constexpr uint32_t number_circle_02 = 3240;
    constexpr uint32_t number_circle_03 = 3241;
    constexpr uint32_t number_circle_04 = 3242;
    constexpr uint32_t number_circle_05 = 3243;
    constexpr uint32_t number_circle_06 = 3244;
    constexpr uint32_t number_circle_07 = 3245;
    constexpr uint32_t number_circle_08 = 3246;
    constexpr uint32_t number_circle_09 = 3247;
    constexpr uint32_t number_circle_10 = 3248;
    constexpr uint32_t number_circle_11 = 3249;
    constexpr uint32_t number_circle_12 = 3250;
    constexpr uint32_t number_circle_13 = 3251;
    constexpr uint32_t number_circle_14 = 3252;
    constexpr uint32_t number_circle_15 = 3253;
    constexpr uint32_t number_circle_16 = 3254;
    constexpr uint32_t number_circle_17 = 3255;
    constexpr uint32_t number_circle_18 = 3256;
    constexpr uint32_t number_circle_19 = 3257;
    constexpr uint32_t number_circle_20 = 3258;
    constexpr uint32_t number_circle_21 = 3259;
    constexpr uint32_t number_circle_22 = 3260;
    constexpr uint32_t number_circle_23 = 3261;
    constexpr uint32_t number_circle_24 = 3262;
    constexpr uint32_t number_circle_25 = 3263;
    constexpr uint32_t number_circle_26 = 3264;
    constexpr uint32_t number_circle_27 = 3265;
    constexpr uint32_t number_circle_28 = 3266;
    constexpr uint32_t number_circle_29 = 3267;
    constexpr uint32_t number_circle_30 = 3268;
    constexpr uint32_t number_circle_31 = 3269;
    constexpr uint32_t number_circle_32 = 3270;
    constexpr uint32_t number_circle_33 = 3271;
    constexpr uint32_t number_circle_34 = 3272;
    constexpr uint32_t number_circle_35 = 3273;
    constexpr uint32_t number_circle_36 = 3274;
    constexpr uint32_t number_circle_37 = 3275;
    constexpr uint32_t number_circle_38 = 3276;
    constexpr uint32_t number_circle_39 = 3277;
    constexpr uint32_t number_circle_40 = 3278;
    constexpr uint32_t number_circle_41 = 3279;
    constexpr uint32_t number_circle_42 = 3280;
    constexpr uint32_t number_circle_43 = 3281;
    constexpr uint32_t number_circle_44 = 3282;
    constexpr uint32_t number_circle_45 = 3283;
    constexpr uint32_t number_circle_46 = 3284;
    constexpr uint32_t number_circle_47 = 3285;
    constexpr uint32_t number_circle_48 = 3286;
    constexpr uint32_t number_circle_49 = 3287;
    constexpr uint32_t number_circle_50 = 3288;
    constexpr uint32_t number_circle_51 = 3289;
    constexpr uint32_t number_circle_52 = 3290;
    constexpr uint32_t number_circle_53 = 3291;
    constexpr uint32_t number_circle_54 = 3292;
    constexpr uint32_t number_circle_55 = 3293;
    constexpr uint32_t number_circle_56 = 3294;
    constexpr uint32_t number_circle_57 = 3295;
    constexpr uint32_t number_circle_58 = 3296;
    constexpr uint32_t number_circle_59 = 3297;
    constexpr uint32_t number_circle_60 = 3298;
    constexpr uint32_t number_circle_61 = 3299;
    constexpr uint32_t number_circle_62 = 3300;
    constexpr uint32_t number_circle_63 = 3301;
    constexpr uint32_t vehicle_crash_0_00 = 3302;
    constexpr uint32_t vehicle_crash_0_01 = 3303;
    constexpr uint32_t vehicle_crash_0_02 = 3304;
    constexpr uint32_t vehicle_crash_0_03 = 3305;
    constexpr uint32_t vehicle_crash_0_04 = 3306;
    constexpr uint32_t vehicle_crash_0_05 = 3307;
    constexpr uint32_t vehicle_crash_0_06 = 3308;
    constexpr uint32_t vehicle_crash_0_07 = 3309;
    constexpr uint32_t vehicle_crash_0_08 = 3310;
    constexpr uint32_t vehicle_crash_0_09 = 3311;
    constexpr uint32_t vehicle_crash_0_10 = 3312;
    constexpr uint32_t vehicle_crash_0_11 = 3313;
    constexpr uint32_t vehicle_crash_1_00 = 3314;
    constexpr uint32_t vehicle_crash_1_01 = 3315;
    constexpr uint32_t vehicle_crash_1_02 = 3316;
    constexpr uint32_t vehicle_crash_1_03 = 3317;
    constexpr uint32_t vehicle_crash_1_04 = 3318;
    constexpr uint32_t vehicle_crash_1_05 = 3319;
    constexpr uint32_t vehicle_crash_1_06 = 3320;
    constexpr uint32_t vehicle_crash_1_07 = 3321;
    constexpr uint32_t vehicle_crash_1_08 = 3322;
    constexpr uint32_t vehicle_crash_1_09 = 3323;
    constexpr uint32_t vehicle_crash_1_10 = 3324;
    constexpr uint32_t vehicle_crash_1_11 = 3325;
    constexpr uint32_t vehicle_crash_2_00 = 3326;
    constexpr uint32_t vehicle_crash_2_01 = 3327;
    constexpr uint32_t vehicle_crash_2_02 = 3328;
    constexpr uint32_t vehicle_crash_2_03 = 3329;
    constexpr uint32_t vehicle_crash_2_04 = 3330;
    constexpr uint32_t vehicle_crash_2_05 = 3331;
    constexpr uint32_t vehicle_crash_2_06 = 3332;
    constexpr uint32_t vehicle_crash_2_07 = 3333;
    constexpr uint32_t vehicle_crash_2_08 = 3334;
    constexpr uint32_t vehicle_crash_2_09 = 3335;
    constexpr uint32_t vehicle_crash_2_10 = 3336;
    constexpr uint32_t vehicle_crash_2_11 = 3337;
    constexpr uint32_t vehicle_crash_3_00 = 3338;
    constexpr uint32_t vehicle_crash_3_01 = 3339;
    constexpr uint32_t vehicle_crash_3_02 = 3340;
    constexpr uint32_t vehicle_crash_3_03 = 3341;
    constexpr uint32_t vehicle_crash_3_04 = 3342;
    constexpr uint32_t vehicle_crash_3_05 = 3343;
    constexpr uint32_t vehicle_crash_3_06 = 3344;
    constexpr uint32_t vehicle_crash_3_07 = 3345;
    constexpr uint32_t vehicle_crash_3_08 = 3346;
    constexpr uint32_t vehicle_crash_3_09 = 3347;
    constexpr uint32_t vehicle_crash_3_10 = 3348;
    constexpr uint32_t vehicle_crash_3_11 = 3349;
    constexpr uint32_t vehicle_crash_4_00 = 3350;
    constexpr uint32_t vehicle_crash_4_01 = 3351;
    constexpr uint32_t vehicle_crash_4_02 = 3352;
    constexpr uint32_t vehicle_crash_4_03 = 3353;
    constexpr uint32_t vehicle_crash_4_04 = 3354;
    constexpr uint32_t vehicle_crash_4_05 = 3355;
    constexpr uint32_t vehicle_crash_4_06 = 3356;
    constexpr uint32_t vehicle_crash_4_07 = 3357;
    constexpr uint32_t vehicle_crash_4_08 = 3358;
    constexpr uint32_t vehicle_crash_4_09 = 3359;
    constexpr uint32_t vehicle_crash_4_10 = 3360;
    constexpr uint32_t vehicle_crash_4_11 = 3361;
    constexpr uint32_t explosion_smoke_00 = 3362;
    constexpr uint32_t explosion_smoke_01 = 3363;
    constexpr uint32_t explosion_smoke_02 = 3364;
    constexpr uint32_t explosion_smoke_03 = 3365;
    constexpr uint32_t explosion_smoke_04 = 3366;
    constexpr uint32_t explosion_smoke_05 = 3367;
    constexpr uint32_t explosion_smoke_06 = 3368;
    constexpr uint32_t explosion_smoke_07 = 3369;
    constexpr uint32_t explosion_smoke_08 = 3370;
    constexpr uint32_t explosion_smoke_09 = 3371;
    constexpr uint32_t explosion_cloud_00 = 3372;
    constexpr uint32_t explosion_cloud_01 = 3373;
    constexpr uint32_t explosion_cloud_02 = 3374;
    constexpr uint32_t explosion_cloud_03 = 3375;
    constexpr uint32_t explosion_cloud_04 = 3376;
    constexpr uint32_t explosion_cloud_05 = 3377;
    constexpr uint32_t explosion_cloud_06 = 3378;
    constexpr uint32_t explosion_cloud_07 = 3379;
    constexpr uint32_t explosion_cloud_08 = 3380;
    constexpr uint32_t explosion_cloud_09 = 3381;
    constexpr uint32_t explosion_cloud_10 = 3382;
    constexpr uint32_t explosion_cloud_11 = 3383;
    constexpr uint32_t explosion_cloud_12 = 3384;
    constexpr uint32_t explosion_cloud_13 = 3385;
    constexpr uint32_t explosion_cloud_14 = 3386;
    constexpr uint32_t explosion_cloud_15 = 3387;
    constexpr uint32_t explosion_cloud_16 = 3388;
    constexpr uint32_t explosion_cloud_17 = 3389;
    constexpr uint32_t fireball_00 = 3390;
    constexpr uint32_t fireball_01 = 3391;
    constexpr uint32_t fireball_02 = 3392;
    constexpr uint32_t fireball_03 = 3393;
    constexpr uint32_t fireball_04 = 3394;
    constexpr uint32_t fireball_05 = 3395;
    constexpr uint32_t fireball_06 = 3396;
    constexpr uint32_t fireball_07 = 3397;
    constexpr uint32_t fireball_08 = 3398;
    constexpr uint32_t fireball_09 = 3399;
    constexpr uint32_t fireball_10 = 3400;
    constexpr uint32_t fireball_11 = 3401;
    constexpr uint32_t fireball_12 = 3402;
    constexpr uint32_t fireball_13 = 3403;
    constexpr uint32_t fireball_14 = 3404;
    constexpr uint32_t fireball_15 = 3405;
    constexpr uint32_t fireball_16 = 3406;
    constexpr uint32_t fireball_17 = 3407;
    constexpr uint32_t fireball_18 = 3408;
    constexpr uint32_t fireball_19 = 3409;
    constexpr uint32_t fireball_20 = 3410;
    constexpr uint32_t fireball_21 = 3411;
    constexpr uint32_t fireball_22 = 3412;
    constexpr uint32_t fireball_23 = 3413;
    constexpr uint32_t fireball_24 = 3414;
    constexpr uint32_t fireball_25 = 3415;
    constexpr uint32_t fireball_26 = 3416;
    constexpr uint32_t fireball_27 = 3417;
    constexpr uint32_t fireball_28 = 3418;
    constexpr uint32_t fireball_29 = 3419;
    constexpr uint32_t fireball_30 = 3420;
    constexpr uint32_t splash_00 = 3421;
    constexpr uint32_t splash_01 = 3422;
    constexpr uint32_t splash_02 = 3423;
    constexpr uint32_t splash_03 = 3424;
    constexpr uint32_t splash_04 = 3425;
    constexpr uint32_t splash_05 = 3426;
    constexpr uint32_t splash_06 = 3427;
    constexpr uint32_t splash_07 = 3428;
    constexpr uint32_t splash_08 = 3429;
    constexpr uint32_t splash_09 = 3430;
    constexpr uint32_t splash_10 = 3431;
    constexpr uint32_t splash_11 = 3432;
    constexpr uint32_t splash_12 = 3433;
    constexpr uint32_t splash_13 = 3434;
    constexpr uint32_t splash_14 = 3435;
    constexpr uint32_t splash_15 = 3436;
    constexpr uint32_t splash_16 = 3437;
    constexpr uint32_t splash_17 = 3438;
    constexpr uint32_t splash_18 = 3439;
    constexpr uint32_t splash_19 = 3440;
    constexpr uint32_t splash_20 = 3441;
    constexpr uint32_t splash_21 = 3442;
    constexpr uint32_t splash_22 = 3443;
    constexpr uint32_t splash_23 = 3444;
    constexpr uint32_t splash_24 = 3445;
    constexpr uint32_t splash_25 = 3446;
    constexpr uint32_t splash_26 = 3447;
    constexpr uint32_t splash_27 = 3448;

    constexpr uint32_t smoke_00 = 3465;
    constexpr uint32_t smoke_01 = 3466;
    constexpr uint32_t smoke_02 = 3467;
    constexpr uint32_t smoke_03 = 3468;
    constexpr uint32_t smoke_04 = 3469;
    constexpr uint32_t smoke_05 = 3470;
    constexpr uint32_t smoke_06 = 3471;
    constexpr uint32_t smoke_07 = 3472;
    constexpr uint32_t smoke_08 = 3473;
    constexpr uint32_t smoke_09 = 3474;
    constexpr uint32_t smoke_10 = 3475;
    constexpr uint32_t smoke_11 = 3476;

    constexpr uint32_t tab_object_settings = 3505;
    constexpr uint32_t tab_object_audio = 3506;
    constexpr uint32_t tab_object_currency = 3507;
    constexpr uint32_t tab_object_smoke = 3508;
    constexpr uint32_t tab_object_cliff = 3509;
    constexpr uint32_t tab_object_water = 3510;
    constexpr uint32_t tab_object_landscape = 3511;
    constexpr uint32_t tab_object_town_names = 3512;
    constexpr uint32_t tab_object_cargo = 3513;
    constexpr uint32_t tab_object_walls = 3514;
    constexpr uint32_t tab_object_signals = 3515;
    constexpr uint32_t tab_object_level_crossings = 3516;
    constexpr uint32_t tab_object_streetlights = 3517;
    constexpr uint32_t tab_object_tunnels = 3518;
    constexpr uint32_t tab_object_bridges = 3519;
    constexpr uint32_t tab_object_track_stations = 3520;
    constexpr uint32_t tab_object_track_mods = 3521;
    constexpr uint32_t tab_object_track = 3522;
    constexpr uint32_t tab_object_road_stations = 3523;
    constexpr uint32_t tab_object_road_mods = 3524;
    constexpr uint32_t tab_object_road = 3525;
    constexpr uint32_t tab_object_airports = 3526;
    constexpr uint32_t tab_object_docks = 3527;
    constexpr uint32_t tab_object_vehicles = 3528;
    constexpr uint32_t tab_object_trees = 3529;
    constexpr uint32_t tab_object_snow = 3530;
    constexpr uint32_t tab_object_climate = 3531;
    constexpr uint32_t tab_object_map = 3532;
    constexpr uint32_t tab_object_buildings = 3533;
    constexpr uint32_t tab_object_construction = 3534;
    constexpr uint32_t tab_object_industries = 3535;
    constexpr uint32_t tab_object_world = 3536;
    constexpr uint32_t tab_object_companies = 3537;
    constexpr uint32_t tab_object_scenarios = 3538;
    constexpr uint32_t news_background_old_left = 3539;
    constexpr uint32_t news_background_old_right = 3540;
    constexpr uint32_t news_background_new_left = 3541;
    constexpr uint32_t news_background_new_right = 3542;
    constexpr uint32_t volume_slider_track = 3543;
    constexpr uint32_t volume_slider_thumb = 3544;
    constexpr uint32_t speed_control_track = 3545;
    constexpr uint32_t speed_control_thumb = 3546;
    constexpr uint32_t title_menu_sparkle = 3547;
    constexpr uint32_t title_menu_save = 3548;
    constexpr uint32_t title_menu_lesson_l = 3549;
    constexpr uint32_t title_menu_lesson_a = 3550;
    constexpr uint32_t title_menu_lesson_p = 3551;
    constexpr uint32_t title_menu_globe_spin_0 = 3552;
    constexpr uint32_t title_menu_globe_spin_1 = 3553;
    constexpr uint32_t title_menu_globe_spin_2 = 3554;
    constexpr uint32_t title_menu_globe_spin_3 = 3555;
    constexpr uint32_t title_menu_globe_spin_4 = 3556;
    constexpr uint32_t title_menu_globe_spin_5 = 3557;
    constexpr uint32_t title_menu_globe_spin_6 = 3558;
    constexpr uint32_t title_menu_globe_spin_7 = 3559;
    constexpr uint32_t title_menu_globe_spin_8 = 3560;
    constexpr uint32_t title_menu_globe_spin_9 = 3561;
    constexpr uint32_t title_menu_globe_spin_10 = 3562;
    constexpr uint32_t title_menu_globe_spin_11 = 3563;
    constexpr uint32_t title_menu_globe_spin_12 = 3564;
    constexpr uint32_t title_menu_globe_spin_13 = 3565;
    constexpr uint32_t title_menu_globe_spin_14 = 3566;
    constexpr uint32_t title_menu_globe_spin_15 = 3567;
    constexpr uint32_t title_menu_globe_spin_16 = 3568;
    constexpr uint32_t title_menu_globe_spin_17 = 3569;
    constexpr uint32_t title_menu_globe_spin_18 = 3570;
    constexpr uint32_t title_menu_globe_spin_19 = 3571;
    constexpr uint32_t title_menu_globe_spin_20 = 3572;
    constexpr uint32_t title_menu_globe_spin_21 = 3573;
    constexpr uint32_t title_menu_globe_spin_22 = 3574;
    constexpr uint32_t title_menu_globe_spin_23 = 3575;
    constexpr uint32_t title_menu_globe_spin_24 = 3576;
    constexpr uint32_t title_menu_globe_spin_25 = 3577;
    constexpr uint32_t title_menu_globe_spin_26 = 3578;
    constexpr uint32_t title_menu_globe_spin_27 = 3579;
    constexpr uint32_t title_menu_globe_spin_28 = 3580;
    constexpr uint32_t title_menu_globe_spin_29 = 3581;
    constexpr uint32_t title_menu_globe_spin_30 = 3582;
    constexpr uint32_t title_menu_globe_spin_31 = 3583;
    constexpr uint32_t title_menu_globe_construct_0 = 3584;
    constexpr uint32_t title_menu_globe_construct_1 = 3585;
    constexpr uint32_t title_menu_globe_construct_2 = 3586;
    constexpr uint32_t title_menu_globe_construct_3 = 3587;
    constexpr uint32_t title_menu_globe_construct_4 = 3588;
    constexpr uint32_t title_menu_globe_construct_5 = 3589;
    constexpr uint32_t title_menu_globe_construct_6 = 3590;
    constexpr uint32_t title_menu_globe_construct_7 = 3591;
    constexpr uint32_t title_menu_globe_construct_8 = 3592;
    constexpr uint32_t title_menu_globe_construct_9 = 3593;
    constexpr uint32_t title_menu_globe_construct_10 = 3594;
    constexpr uint32_t title_menu_globe_construct_11 = 3595;
    constexpr uint32_t title_menu_globe_construct_12 = 3596;
    constexpr uint32_t title_menu_globe_construct_13 = 3597;
    constexpr uint32_t title_menu_globe_construct_14 = 3598;
    constexpr uint32_t title_menu_globe_construct_15 = 3599;
    constexpr uint32_t title_menu_globe_construct_16 = 3600;
    constexpr uint32_t title_menu_globe_construct_17 = 3601;
    constexpr uint32_t title_menu_globe_construct_18 = 3602;
    constexpr uint32_t title_menu_globe_construct_19 = 3603;
    constexpr uint32_t title_menu_globe_construct_20 = 3604;
    constexpr uint32_t title_menu_globe_construct_21 = 3605;
    constexpr uint32_t title_menu_globe_construct_22 = 3606;
    constexpr uint32_t title_menu_globe_construct_23 = 3607;
    constexpr uint32_t title_menu_globe_construct_24 = 3608;
    constexpr uint32_t title_menu_globe_construct_25 = 3609;
    constexpr uint32_t title_menu_globe_construct_26 = 3610;
    constexpr uint32_t title_menu_globe_construct_27 = 3611;
    constexpr uint32_t title_menu_globe_construct_28 = 3612;
    constexpr uint32_t title_menu_globe_construct_29 = 3613;
    constexpr uint32_t title_menu_globe_construct_30 = 3614;
    constexpr uint32_t title_menu_globe_construct_31 = 3615;
    constexpr uint32_t chris_sawyer_logo_small = 3616;
    constexpr uint32_t chris_sawyer_logo_intro_left = 3617;
    constexpr uint32_t chris_sawyer_logo_intro_right = 3618;

    constexpr uint32_t atari_logo_intro_left = 3620;
    constexpr uint32_t atari_logo_intro_right = 3620;
    constexpr uint32_t atari_logo_small = UNUSED_IMG(3623);
    constexpr uint32_t locomotion_logo = 3624;
    constexpr uint32_t wide_tab = 3625;

    constexpr uint32_t scenario_completed_tick = 3629;
    constexpr uint32_t owner_jailed = 3630;

}
