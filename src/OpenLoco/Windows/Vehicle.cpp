#include "../Vehicles/Vehicle.h"
#include "../CompanyManager.h"
#include "../Config.h"
#include "../GameCommands.h"
#include "../Graphics/Colour.h"
#include "../Graphics/ImageIds.h"
#include "../Input.h"
#include "../Interop/Interop.hpp"
#include "../Localisation/FormatArguments.hpp"
#include "../Objects/CargoObject.h"
#include "../Objects/InterfaceSkinObject.h"
#include "../Objects/ObjectManager.h"
#include "../Objects/RoadObject.h"
#include "../Objects/TrackExtraObject.h"
#include "../Objects/TrackObject.h"
#include "../Objects/WaterObject.h"
#include "../OpenLoco.h"
#include "../StationManager.h"
#include "../Things/ThingManager.h"
#include "../TrackData.h"
#include "../Ui/Dropdown.h"
#include "../Ui/ScrollView.h"
#include "../Ui/WindowManager.h"
#include "../Vehicles/Orders.h"
#include "../ViewportManager.h"
#include "../Widget.h"
#include <map>

using namespace OpenLoco::Interop;

namespace OpenLoco::Ui::Vehicle
{
    namespace Common
    {
        enum widx
        {
            frame = 0,
            caption = 1,
            closeButton = 2,
            panel = 3,
            tabMain = 4,
            tabDetails = 5,
            tabCargo = 6,
            tabFinances = 7,
            tabRoute = 8,
        };

#define commonWidgets(frameWidth, frameHeight, windowCaptionId)                                                                                  \
    makeWidget({ 0, 0 }, { (frameWidth), (frameHeight) }, widget_type::frame, 0),                                                                \
        makeWidget({ 1, 1 }, { (frameWidth)-2, 13 }, widget_type::caption_24, 0, windowCaptionId),                                               \
        makeWidget({ (frameWidth)-15, 2 }, { 13, 13 }, widget_type::wt_9, 0, ImageIds::close_button, StringIds::tooltip_close_window),           \
        makeWidget({ 0, 41 }, { 265, 136 }, widget_type::panel, 1),                                                                              \
        makeRemapWidget({ 3, 15 }, { 31, 27 }, widget_type::wt_8, 1, ImageIds::tab_vehicle_background, StringIds::tooltip_vehicle_tab_main),     \
        makeRemapWidget({ 34, 15 }, { 31, 27 }, widget_type::wt_8, 1, ImageIds::tab_vehicle_background, StringIds::tooltip_vehicle_tab_details), \
        makeRemapWidget({ 65, 15 }, { 31, 27 }, widget_type::wt_8, 1, ImageIds::tab_vehicle_background, StringIds::tooltip_vehicle_tab_cargo),   \
        makeRemapWidget({ 96, 15 }, { 31, 27 }, widget_type::wt_8, 1, ImageIds::tab_vehicle_background, StringIds::tooltip_vehicle_tab_finance), \
        makeRemapWidget({ 158, 15 }, { 31, 27 }, widget_type::wt_8, 1, ImageIds::tab_vehicle_background, StringIds::tooltip_vehicle_tab_route)

        constexpr uint64_t enabledWidgets = (1 << closeButton) | (1 << tabMain) | (1 << tabDetails) | (1 << tabCargo) | (1 << tabFinances) | (1 << tabRoute);

        static Vehicles::VehicleHead* getVehicle(const window* self)
        {
            return ThingManager::get<Vehicles::VehicleHead>(self->number);
        }

        static void setActiveTabs(window* const self);
        static void textInput(window* const self, const widget_index callingWidget, char* const input);
        static void renameVehicle(window* const self, const widget_index widgetIndex);
        static void switchTab(window* const self, const widget_index widgetIndex);
        static void repositionTabs(window* const self);
        static void setCaptionEnableState(window* const self);
        static void onPickup(window* const self, const widget_index pickupWidx);
        static void event8(window* const self);
        static void event9(window* const self);
        static size_t getNumCars(Ui::window* const self);
        static void drawTabs(window* const window, Gfx::drawpixelinfo_t* const context);
        static void pickupToolUpdate(window& self, const int16_t x, const int16_t y);
        static void pickupToolDown(window& self, const int16_t x, const int16_t y);
        static void pickupToolAbort(window& self);
        static size_t getNumCars(Ui::window* const self);
        static void drawTabs(window* const window, Gfx::drawpixelinfo_t* const context);
        static std::optional<Vehicles::Car> getCarFromScrollView(window* const self, const int16_t y);
        static std::pair<uint32_t, string_id> getPickupImageIdandTooltip(const Vehicles::VehicleHead& head, const bool isPlaced);
    }

    namespace Details
    {
        static const Gfx::ui_size_t minWindowSize = { 192, 148 };
        static const Gfx::ui_size_t maxWindowSize = { 400, 440 };

        enum widx
        {
            buildNew = Common::widx::tabRoute + 1,
            pickup,
            remove,
            carList
        };

        // 0x00500434
        static window_event_list events;
        constexpr uint64_t enabledWidgets = (1 << widx::buildNew) | (1 << widx::pickup) | (1 << widx::remove) | (1 << widx::carList) | Common::enabledWidgets;
        constexpr uint64_t holdableWidgets = 0;

        static widget_t widgets[] = {
            commonWidgets(265, 177, StringIds::title_vehicle_details),
            makeWidget({ 240, 44 }, { 24, 24 }, widget_type::wt_9, 1, -1, StringIds::tooltip_build_new_vehicle_for),
            makeWidget({ 240, 68 }, { 24, 24 }, widget_type::wt_9, 1, -1, StringIds::tooltip_remove_from_track),
            makeWidget({ 240, 96 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::rubbish_bin, StringIds::tooltip_sell_or_drag_vehicle),
            makeWidget({ 3, 44 }, { 237, 110 }, widget_type::scrollview, 1, vertical),
            widgetEnd()
        };
    }

    namespace Cargo
    {
        static const Gfx::ui_size_t minWindowSize = { 192, 142 };
        static const Gfx::ui_size_t maxWindowSize = { 400, 440 };

        enum widx
        {
            refit = 9,
            cargoList = 10,
        };

        // 0x005004A8
        static window_event_list events;
        constexpr uint64_t enabledWidgets = (1 << widx::refit) | (1 << widx::cargoList) | Common::enabledWidgets;
        constexpr uint64_t holdableWidgets = 0;

        static widget_t widgets[] = {
            commonWidgets(265, 177, StringIds::title_vehicle_cargo),
            makeWidget({ 240, 44 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::refit_cargo_button, StringIds::refit_vehicle_tip),
            makeWidget({ 3, 44 }, { 259, 120 }, widget_type::scrollview, 1, vertical),
            widgetEnd()
        };
    }

    namespace Finances
    {
        static const Gfx::ui_size_t minWindowSize = { 400, 202 };
        static const Gfx::ui_size_t maxWindowSize = minWindowSize;

        static window_event_list events;
        constexpr uint64_t enabledWidgets = Common::enabledWidgets;
        constexpr uint64_t holdableWidgets = 0;

        // 0x00522470
        static widget_t widgets[] = {
            commonWidgets(636, 319, StringIds::title_company_finances),
            widgetEnd(),
        };
    }

    namespace Route
    {
        static const Gfx::ui_size_t minWindowSize = { 265, 178 };
        static const Gfx::ui_size_t maxWindowSize = { 600, 440 };

        enum widx
        {
            tool = Common::widx::tabRoute + 1, // Only used to hold the tool does nothing
            localMode,
            expressMode,
            routeList,
            orderForceUnload,
            orderWait,
            orderSkip,
            orderDelete,
            orderUp,
            orderDown
        };

        // 0x00500554
        static window_event_list events;
        constexpr uint64_t enabledWidgets = (1 << widx::routeList) | (1 << widx::orderForceUnload) | (1 << widx::orderWait) | (1 << widx::orderSkip) | (1 << widx::orderDelete) | (1 << widx::orderUp) | (1 << widx::orderDown) | Common::enabledWidgets;
        constexpr uint64_t holdableWidgets = 0;

        static widget_t widgets[] = {
            commonWidgets(265, 189, StringIds::title_vehicle_route),
            makeWidget({ 0, 0 }, { 1, 1 }, widget_type::none, 0),
            makeWidget({ 3, 44 }, { 118, 12 }, widget_type::wt_11, 1, StringIds::local_seperator),
            makeWidget({ 121, 44 }, { 119, 12 }, widget_type::wt_11, 1, StringIds::express_seperator),
            makeWidget({ 3, 57 }, { 237, 120 }, widget_type::scrollview, 1, vertical, StringIds::tooltip_route_scrollview),
            makeWidget({ 240, 44 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::route_force_unload, StringIds::tooltip_route_insert_force_unload),
            makeWidget({ 240, 68 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::route_wait, StringIds::tooltip_route_insert_wait_full_cargo),
            makeWidget({ 240, 92 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::route_skip, StringIds::tooltip_route_skip_next_order),
            makeWidget({ 240, 116 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::route_delete, StringIds::tooltip_route_delete_order),
            makeWidget({ 240, 140 }, { 24, 12 }, widget_type::wt_9, 1, ImageIds::red_arrow_up, StringIds::tooltip_route_move_order_up),
            makeWidget({ 240, 152 }, { 24, 12 }, widget_type::wt_9, 1, ImageIds::red_arrow_down, StringIds::tooltip_route_move_order_down),
            widgetEnd(),
        };
    }

    static loco_global<uint8_t, 0x00525FC5> _525FC5;
    static loco_global<uint8_t, 0x00525FB0> _pickupDirection; // direction that the ghost points
    static loco_global<Vehicles::VehicleBogie*, 0x0113614E> _dragCarComponent;
    static loco_global<thing_id_t, 0x01136156> _dragVehicleHead;
    static loco_global<int32_t, 0x01136264> _1136264;
    static loco_global<string_id, 0x009C68E8> gGameCommandErrorTitle;
    static loco_global<uint8_t, 0x00508F14> _screenFlags;
    static loco_global<uint32_t[32], 0x00525E5E> currencyMultiplicationFactor;

    namespace Main
    {
        static const Gfx::ui_size_t windowSize = { 265, 177 };
        static const Gfx::ui_size_t minWindowSize = { 192, 177 };
        static const Gfx::ui_size_t maxWindowSize = { 600, 440 };

        enum widx
        {
            viewport = Common::widx::tabRoute + 1,
            status = 10,
            speedControl = 11,
            stopStart = 12,
            pickup = 13,
            passSignal = 14,
            changeDirection = 15,
            centreViewport = 16,
        };

        static widget_t widgets[] = {
            commonWidgets(265, 177, StringIds::stringid),
            makeWidget({ 3, 44 }, { 237, 120 }, widget_type::viewport, 1),
            makeWidget({ 3, 155 }, { 237, 21 }, widget_type::wt_13, 1),
            makeWidget({ 240, 46 }, { 24, 115 }, widget_type::wt_5, 1),
            makeWidget({ 240, 44 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::red_flag, StringIds::tooltip_stop_start),
            makeWidget({ 240, 68 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::null, StringIds::tooltip_remove_from_track),
            makeWidget({ 240, 92 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::pass_signal, StringIds::tooltip_pass_signal_at_danger),
            makeWidget({ 240, 116 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::construction_right_turnaround, StringIds::tooltip_change_direction),
            makeWidget({ 0, 0 }, { 24, 24 }, widget_type::wt_9, 1, ImageIds::null, StringIds::move_main_view_to_show_this),
            widgetEnd()
        };

        constexpr uint64_t interactiveWidgets = (1 << widx::stopStart) | (1 << widx::pickup) | (1 << widx::passSignal) | (1 << widx::changeDirection) | (1 << widx::centreViewport);
        constexpr uint64_t enabledWidgets = Common::enabledWidgets | (1 << widx::speedControl) | interactiveWidgets;
        constexpr uint64_t holdableWidgets = 1 << widx::speedControl;

        // 0x005003C0
        static window_event_list events;

        // 0x004B5D82
        static void resetDisabledWidgets(window* const self)
        {
            self->disabled_widgets = 0;
        }

        // 0x004B5D88
        // 0x004B32F9
        static void createViewport(window* const self)
        {
            if (self->current_tab != (Common::widx::tabMain - Common::widx::tabMain))
            {
                return;
            }

            self->callPrepareDraw();

            auto vehHead = Common::getVehicle(self);
            Vehicles::Vehicle train(vehHead);

            // If picked up no need for viewport drawn
            if (vehHead->tile_x == -1)
            {
                self->viewportRemove(0);
                self->invalidate();
                return;
            }

            // By default focus on the veh2 id and if there are cars focus on the body of the first car
            thing_id_t targetThing = train.veh2->id;
            if (!train.cars.empty())
            {
                targetThing = train.cars.firstCar.front->id;
                // Always true so above is pointless
                if (train.cars.firstCar.front->getSubType() == Vehicles::VehicleThingType::bogie)
                {
                    targetThing = train.cars.firstCar.body->id;
                }
            }

            // Compute views.
            SavedView view = {
                targetThing,
                (1 << 15) | (1 << 14),
                ZoomLevel::full,
                static_cast<int8_t>(self->viewports[0]->getRotation()),
                0
            };

            uint16_t flags = 0;
            if (self->viewports[0] != nullptr)
            {
                if (self->saved_view == view)
                    return;

                flags = self->viewports[0]->flags;
                self->viewportRemove(0);
                ViewportManager::collectGarbage();
            }
            else
            {
                if ((Config::get().flags & Config::flags::gridlines_on_landscape) != 0)
                    flags |= ViewportFlags::gridlines_on_landscape;
            }

            self->saved_view = view;

            // 0x004B5E88 start
            if (self->viewports[0] == nullptr)
            {
                auto widget = &self->widgets[widx::viewport];
                auto origin = Gfx::point_t(widget->left + self->x + 1, widget->top + self->y + 1);
                auto size = Gfx::ui_size_t(widget->width() - 2, widget->height() - 2);
                ViewportManager::create(self, 0, origin, size, self->saved_view.zoomLevel, targetThing);
                self->invalidate();
                self->flags |= WindowFlags::viewport_no_scrolling;
            }
            // 0x004B5E88 end

            if (self->viewports[0] != nullptr)
            {
                self->viewports[0]->flags = flags;
                self->invalidate();
            }
        }

        static const uint16_t rowHeights[vehicleTypeCount] = {
            22,
            22,
            22,
            22,
            82,
            45
        };

        static void initEvents();

        // 0x004B60DC
        static window* create(const uint16_t head)
        {
            auto* const self = WindowManager::createWindow(WindowType::vehicle, windowSize, WindowFlags::flag_11 | WindowFlags::flag_8 | WindowFlags::resizable, &events);
            self->widgets = widgets;
            self->enabled_widgets = enabledWidgets;
            self->number = head;
            const auto* vehicle = ThingManager::get<Vehicles::VehicleHead>(head);
            self->owner = vehicle->owner;
            self->row_height = rowHeights[static_cast<uint8_t>(vehicle->vehicleType)];
            self->current_tab = 0;
            self->frame_no = 0;
            resetDisabledWidgets(self);
            self->min_width = minWindowSize.width;
            self->min_height = minWindowSize.height;
            self->max_width = maxWindowSize.width;
            self->max_height = maxWindowSize.height;
            self->var_85C = -1;
            WindowManager::close(WindowType::dragVehiclePart, 0);
            _dragCarComponent = nullptr;
            _dragVehicleHead = ThingId::null;

            const auto* skin = ObjectManager::get<interface_skin_object>();
            self->colours[1] = skin->colour_0A;
            return self;
        }

        // 0x004B6033
        window* open(const Vehicles::VehicleBase* vehicle)
        {
            const auto head = vehicle->getHead();
            auto* self = WindowManager::find(WindowType::vehicle, head);
            if (self != nullptr)
            {
                if (Input::isToolActive(self->type, self->number))
                {
                    Input::toolCancel();
                }
                self = WindowManager::bringToFront(WindowType::vehicle, head);
            }
            if (self == nullptr)
            {
                self = create(head);
                self->saved_view.clear();
            }
            self->current_tab = 0;
            self->invalidate();
            self->widgets = widgets;
            self->enabled_widgets = enabledWidgets;
            self->holdable_widgets = holdableWidgets;
            self->event_handlers = &events;
            self->activated_widgets = 0;
            resetDisabledWidgets(self);
            self->initScrollWidgets();
            createViewport(self);
            return self;
        }

        // 0x004B288F
        static void onChangeDirection(window* const self)
        {
            if (Input::isToolActive(self->type, self->number, widx::pickup))
            {
                _pickupDirection = _pickupDirection ^ 1;
                return;
            }

            gGameCommandErrorTitle = StringIds::cant_reverse_train;
            GameCommands::do_3(self->number, Common::getVehicle(self));
        }

        // 0x004B24D1
        static void onMouseUp(window* const self, const widget_index widgetIndex)
        {
            switch (widgetIndex)
            {
                case Common::widx::closeButton:
                    WindowManager::close(self);
                    break;
                case Common::widx::caption:
                    Common::renameVehicle(self, widgetIndex);
                    break;
                case Common::widx::tabMain:
                case Common::widx::tabDetails:
                case Common::widx::tabCargo:
                case Common::widx::tabFinances:
                case Common::widx::tabRoute:
                    Common::switchTab(self, widgetIndex);
                    break;
                case widx::pickup:
                    Common::onPickup(self, widx::pickup);
                    break;
                case widx::changeDirection:
                    onChangeDirection(self);
                    break;
                case widx::passSignal:
                    gGameCommandErrorTitle = StringIds::cant_pass_signal_at_danger;
                    GameCommands::do_4(self->number);
                    break;
                case widx::centreViewport:
                    self->viewportCentreMain();
                    break;
            }
        }

        // 0x004B30F3
        static void onUpdate(window* const self)
        {
            self->frame_no += 1;
            self->callPrepareDraw();

            WindowManager::invalidateWidget(WindowType::vehicle, self->number, Common::widx::tabMain);
            WindowManager::invalidateWidget(WindowType::vehicle, self->number, widx::status);
            WindowManager::invalidateWidget(WindowType::vehicle, self->number, widx::pickup);
            WindowManager::invalidateWidget(WindowType::vehicle, self->number, widx::passSignal);
            WindowManager::invalidateWidget(WindowType::vehicle, self->number, widx::changeDirection);

            if (self->isDisabled(widx::pickup))
            {
                Input::toolCancel(WindowType::vehicle, self->number);
                return;
            }

            auto head = Common::getVehicle(self);

            // If vehicle not placed put into pickup mode if window in focus
            if (head->tile_x != -1 && (head->var_38 & Vehicles::Flags38::isGhost) == 0)
            {
                return;
            }

            if (!WindowManager::isInFront(self))
                return;

            if (head->owner != CompanyManager::getControllingId())
                return;

            if (!Input::isToolActive(WindowType::vehicle, self->number))
            {
                Common::onPickup(self, widx::pickup);
            }
        }

        // 0x004B3210
        static void onResize(window* const self)
        {
            Common::setCaptionEnableState(self);
            self->setSize(minWindowSize, maxWindowSize);

            if (self->viewports[0] != nullptr)
            {
                auto head = Common::getVehicle(self);
                uint16_t newWidth = self->width - 30;
                if (head->owner != CompanyManager::getControllingId())
                    newWidth += 22;

                uint16_t newHeight = self->height - 59;
                if (head->var_0C & Vehicles::Flags0C::manualControl && head->owner == CompanyManager::getControllingId())
                {
                    newWidth -= 27;
                }

                auto& viewport = self->viewports[0];
                if (newWidth != viewport->width || newHeight != viewport->height)
                {
                    self->invalidate();
                    viewport->width = newWidth;
                    viewport->height = newHeight;
                    viewport->view_width = newWidth << viewport->zoom;
                    viewport->view_height = newHeight << viewport->zoom;
                    self->saved_view.clear();
                }
            }
            createViewport(self);
        }

        // 0x004B274B
        static void stopStartOpen(window* const self)
        {
            Dropdown::add(0, StringIds::dropdown_stringid, StringIds::stop);
            Dropdown::add(1, StringIds::dropdown_stringid, StringIds::start);
            Dropdown::add(2, StringIds::dropdown_stringid, StringIds::manual);

            auto dropdownCount = 2;
            auto head = Common::getVehicle(self);
            if (head->mode == TransportMode::rail && (_screenFlags & ScreenFlags::driverCheatEnabled))
            {
                dropdownCount = 3;
            }

            widget_t* widget = &self->widgets[widx::stopStart];
            Dropdown::showText(
                self->x + widget->left,
                self->y + widget->top,
                widget->width(),
                widget->height(),
                self->colours[1],
                dropdownCount,
                0);

            auto selected = 0; // Stop
            if (!(head->var_0C & Vehicles::Flags0C::commandStop))
            {
                selected = 1; // Start
            }
            if (head->var_0C & Vehicles::Flags0C::manualControl)
            {
                selected = 2; // Manual
            }

            Dropdown::setItemSelected(selected);
            Dropdown::setHighlightedItem(selected == 0 ? 1 : 0); // Stop becomes start highlighted. Manual or Start becomes Stop highlighted
        }

        // 0x004B2637
        static void onSpeedControl(window* const self)
        {
            Input::setClickRepeatTicks(31);
            auto pos = Input::getScrollLastLocation();
            auto speed = pos.y - (self->y + self->widgets[widx::speedControl].top + 58);
            speed = -(std::clamp(speed, -40, 40));

            GameCommands::do_74(self->number, speed);
        }

        // 0x004B251A
        static void onMouseDown(window* const self, const widget_index widgetIndex)
        {
            switch (widgetIndex)
            {
                case widx::stopStart:
                    stopStartOpen(self);
                    break;
                case widx::speedControl:
                    onSpeedControl(self);
                    break;
            }
        }

        // 0x004B253A
        static void onDropdown(window* const self, const widget_index widgetIndex, const int16_t itemIndex)
        {
            if (widgetIndex != widx::stopStart)
            {
                return;
            }

            auto item = itemIndex == -1 ? Dropdown::getHighlightedItem() : itemIndex;
            if (item == -1 || item > 2)
            {
                return;
            }

            static const std::pair<string_id, uint8_t> itemToGameCommandInfo[3] = {
                { StringIds::cant_stop_string_id, 0 },
                { StringIds::cant_start_string_id, 1 },
                { StringIds::cant_select_manual_mode_string_id, 3 },
            };
            auto [errorTitle, mode] = itemToGameCommandInfo[item];
            gGameCommandErrorTitle = errorTitle;
            FormatArguments args{};
            auto head = Common::getVehicle(self);
            args.skip(6);
            args.push(head->var_22);
            args.push(head->var_44);
            GameCommands::do12(head->id, mode);
        }

        // 0x004B2545
        static void onToolUpdate(window& self, const widget_index widgetIndex, const int16_t x, const int16_t y)
        {
            if (widgetIndex != widx::pickup)
            {
                return;
            }
            Common::pickupToolUpdate(self, x, y);
        }

        // 0x004B2550
        static void onToolDown(window& self, const widget_index widgetIndex, const int16_t x, const int16_t y)
        {
            if (widgetIndex != widx::pickup)
            {
                return;
            }
            Common::pickupToolDown(self, x, y);
        }

        // 0x004B255B
        static void onToolAbort(window& self, const widget_index widgetIndex)
        {
            if (widgetIndex != widx::pickup)
            {
                return;
            }
            Common::pickupToolAbort(self);
        }

        // 0x004B31F2
        static std::optional<FormatArguments> tooltip(Ui::window* self, widget_index)
        {
            FormatArguments args{};

            auto head = Common::getVehicle(self);
            args.skip(2);
            args.push(StringIds::getVehicleType(head->vehicleType));
            return args;
        }

        // 0x004B1EB5
        static void prepareDraw(window* const self)
        {
            if (self->widgets != widgets)
            {
                self->widgets = widgets;
                self->initScrollWidgets();
            }

            Common::setActiveTabs(self);
            auto head = Common::getVehicle(self);
            Vehicles::Vehicle train(head);

            self->widgets[widx::stopStart].type = widget_type::wt_9;
            self->widgets[widx::pickup].type = widget_type::wt_9;
            self->widgets[widx::passSignal].type = widget_type::wt_9;
            self->widgets[widx::changeDirection].type = widget_type::wt_9;

            if (head->mode != TransportMode::rail)
            {
                self->widgets[widx::passSignal].type = widget_type::none;
            }

            if (head->mode == TransportMode::air || head->mode == TransportMode::water)
            {
                self->widgets[widx::changeDirection].type = widget_type::none;
            }

            self->disabled_widgets &= ~interactiveWidgets;

            auto veh1 = train.veh1;
            if (train.cars.empty())
            {
                self->disabled_widgets |= (1 << widx::pickup);
            }

            if (veh1->var_3C >= 0x3689)
            {
                self->disabled_widgets |= (1 << widx::pickup) | (1 << widx::changeDirection);
            }

            if (head->mode == TransportMode::air || head->mode == TransportMode::water)
            {
                if (head->status != Vehicles::Status::stopped && head->status != Vehicles::Status::loading && head->tile_x != -1)
                {
                    self->disabled_widgets |= (1 << widx::pickup);
                }
            }

            if (head->tile_x == -1)
            {
                self->disabled_widgets |= (1 << widx::stopStart) | (1 << widx::passSignal) | (1 << widx::changeDirection) | (1 << widx::centreViewport);
                if (Input::isToolActive(WindowType::vehicle, self->number))
                {
                    self->disabled_widgets &= ~(1 << widx::changeDirection); //???
                }
            }

            if (head->status != Vehicles::Status::unk_3)
            {
                self->disabled_widgets |= (1 << widx::passSignal);
            }

            auto company = CompanyManager::get(head->owner);
            FormatArguments args{};
            if (isPlayerCompany(head->owner))
            {
                args.push(StringIds::company_vehicle);
            }
            else
            {
                args.push(StringIds::competitor_vehicle);
            }
            args.push(company->name);
            args.skip(2);
            args.push(head->var_22);
            args.push(head->var_44);

            uint32_t stopStartImage = ImageIds::red_flag;
            if ((head->var_0C & Vehicles::Flags0C::manualControl) != 0)
            {
                stopStartImage = ImageIds::yellow_flag;
            }
            else if ((head->var_0C & Vehicles::Flags0C::commandStop) != 0)
            {
                stopStartImage = ImageIds::red_flag;
            }
            else
            {
                stopStartImage = ImageIds::green_flag;
            }
            self->widgets[widx::stopStart].image = stopStartImage;

            bool isPlaced = head->tile_x != -1 && !(head->var_38 & OpenLoco::Vehicles::Flags38::isGhost);
            auto [pickupImage, pickupTooltip] = Common::getPickupImageIdandTooltip(*head, isPlaced);
            self->widgets[widx::pickup].image = Gfx::recolour(pickupImage);
            self->widgets[widx::pickup].tooltip = pickupTooltip;

            self->widgets[widx::speedControl].type = widget_type::none;

            self->widgets[Common::widx::frame].right = self->width - 1;
            self->widgets[Common::widx::frame].bottom = self->height - 1;
            self->widgets[Common::widx::panel].right = self->width - 1;
            self->widgets[Common::widx::panel].bottom = self->height - 1;
            self->widgets[Common::widx::caption].right = self->width - 2;
            self->widgets[Common::widx::closeButton].left = self->width - 15;
            self->widgets[Common::widx::closeButton].right = self->width - 3;

            int viewportRight = self->width - 26;
            if (head->var_0C & Vehicles::Flags0C::manualControl)
            {
                if (isPlayerCompany(head->owner))
                {
                    viewportRight -= 27;
                    self->widgets[widx::speedControl].type = widget_type::wt_5;
                }
            }

            self->widgets[widx::viewport].right = viewportRight;
            self->widgets[widx::viewport].bottom = self->height - 1 - 13;

            self->widgets[widx::status].top = self->height - 1 - 13 + 2;
            self->widgets[widx::status].bottom = self->height - 1 - 13 + 2 + 9;
            self->widgets[widx::status].right = self->width - 14;

            self->widgets[widx::stopStart].right = self->width - 2;
            self->widgets[widx::pickup].right = self->width - 2;
            self->widgets[widx::passSignal].right = self->width - 2;
            self->widgets[widx::changeDirection].right = self->width - 2;

            self->widgets[widx::stopStart].left = self->width - 2 - 23;
            self->widgets[widx::pickup].left = self->width - 2 - 23;
            self->widgets[widx::passSignal].left = self->width - 2 - 23;
            self->widgets[widx::changeDirection].left = self->width - 2 - 23;

            self->widgets[widx::speedControl].left = self->width - 2 - 23 - 26;
            self->widgets[widx::speedControl].right = self->width - 2 - 23 - 26 + 23;

            if (!isPlayerCompany(head->owner))
            {
                self->widgets[widx::stopStart].type = widget_type::none;
                self->widgets[widx::pickup].type = widget_type::none;
                self->widgets[widx::passSignal].type = widget_type::none;
                self->widgets[widx::changeDirection].type = widget_type::none;
                self->widgets[widx::viewport].right += 22;
            }

            self->widgets[widx::centreViewport].right = self->widgets[widx::viewport].right - 1;
            self->widgets[widx::centreViewport].bottom = self->widgets[widx::viewport].bottom - 1;
            self->widgets[widx::centreViewport].left = self->widgets[widx::viewport].right - 1 - 23;
            self->widgets[widx::centreViewport].top = self->widgets[widx::viewport].bottom - 1 - 23;
            Common::repositionTabs(self);
        }

        struct VehicleStatus
        {
            thing_id_t status1;
            uint32_t status1Args;
            thing_id_t status2;
            uint32_t status2Args;
        };

        static VehicleStatus sub_4B671C(const Vehicles::VehicleHead* head)
        {
            registers regs = {};
            regs.esi = (int32_t)head;

            call(0x004B671C, regs);

            VehicleStatus status = {};
            status.status1 = regs.bx;
            status.status1Args = regs.eax;
            status.status2 = regs.cx;
            status.status2Args = regs.edx;
            return status;
        }

        // 0x004B226D
        static void draw(window* const self, Gfx::drawpixelinfo_t* const context)
        {
            self->draw(context);
            Common::drawTabs(self, context);

            widget_t& pickupButton = self->widgets[widx::pickup];
            if (pickupButton.type != widget_type::none)
            {
                if ((pickupButton.image & 0x20000000) != 0 && !self->isDisabled(widx::pickup))
                {
                    Gfx::drawImage(
                        context,
                        self->x + pickupButton.left,
                        self->y + pickupButton.top,
                        Gfx::recolour(pickupButton.image, CompanyManager::getCompanyColour(self->owner)));
                }
            }

            auto veh = Common::getVehicle(self);
            {
                auto status = sub_4B671C(veh);
                FormatArguments args = {};
                args.push(status.status1);
                args.push(status.status1Args);
                args.push(status.status2);
                args.push(status.status2Args);

                string_id strFormat = StringIds::black_stringid_stringid;
                if (status.status2 == StringIds::null)
                {
                    strFormat = StringIds::black_stringid;
                }

                Gfx::drawString_494BBF(
                    *context,
                    self->x + self->widgets[widx::status].left - 1,
                    self->y + self->widgets[widx::status].top - 1,
                    self->widgets[widx::status].width() - 1,
                    Colour::black,
                    strFormat,
                    &args);
            }

            widget_t& speedWidget = self->widgets[widx::speedControl];
            if (speedWidget.type != widget_type::none)
            {
                Gfx::drawImage(
                    context,
                    self->x + speedWidget.left,
                    self->y + speedWidget.top + 10,
                    Gfx::recolour(ImageIds::speed_control_track, self->colours[1]));

                Gfx::drawStringCentred(
                    *context,
                    self->x + speedWidget.mid_x(),
                    self->y + speedWidget.top + 4,
                    Colour::black,
                    StringIds::tiny_power);

                Gfx::drawStringCentred(
                    *context,
                    self->x + speedWidget.mid_x(),
                    self->y + speedWidget.bottom - 10,
                    Colour::black,
                    StringIds::tiny_brake);

                Gfx::drawImage(
                    context,
                    self->x + speedWidget.left + 1,
                    self->y + speedWidget.top + 57 - veh->var_6E,
                    Gfx::recolour(ImageIds::speed_control_thumb, self->colours[1]));
            }

            if (self->viewports[0] != nullptr)
            {
                self->drawViewports(context);
                Widget::drawViewportCentreButton(context, self, widx::centreViewport);
            }
            else if (Input::isToolActive(self->type, self->number))
            {
                FormatArguments args = {};
                args.push(StringIds::getVehicleType(veh->vehicleType));
                Gfx::point_t origin;
                widget_t& button = self->widgets[widx::viewport];
                origin.x = self->x + button.mid_x();
                origin.y = self->y + button.mid_y();
                Gfx::drawStringCentredWrapped(
                    context,
                    &origin,
                    button.width() - 6,
                    Colour::black,
                    StringIds::click_on_view_select_string_id_start,
                    &args);
            }
        }

        static void initEvents()
        {
            events.on_mouse_up = onMouseUp;
            events.on_resize = onResize;
            events.on_mouse_down = onMouseDown;
            events.on_dropdown = onDropdown;
            events.on_update = onUpdate;
            events.on_tool_update = onToolUpdate;
            events.on_tool_down = onToolDown;
            events.on_tool_abort = onToolAbort;
            events.text_input = Common::textInput;
            events.viewport_rotate = createViewport;
            events.tooltip = tooltip;
            events.prepare_draw = prepareDraw;
            events.draw = draw;
        }
    }

    namespace Details
    {
        // 0x4B60CC
        window* open(const Vehicles::VehicleBase* vehicle)
        {
            auto self = Main::open(vehicle);
            self->callOnMouseUp(Common::widx::tabDetails);
            return self;
        }

        static void cloneVehicle(window* self)
        {
            static loco_global<uint16_t, 0x0113642A> _113642A;
            auto head = Common::getVehicle(self);
            gGameCommandErrorTitle = StringIds::cant_clone_vehicle;
            if (GameCommands::do_80(head->head))
            {
                auto* newVehicle = ThingManager::get<Vehicles::VehicleBase>(_113642A);
                if (newVehicle != nullptr)
                {
                    OpenLoco::Ui::Vehicle::Details::open(newVehicle);
                }
            }
        }

        // 0x004B3823
        static void onMouseUp(window* self, widget_index widgetIndex)
        {
            switch (widgetIndex)
            {
                case Common::widx::closeButton:
                    WindowManager::close(self);
                    break;
                case Common::widx::caption:
                    Common::renameVehicle(self, widgetIndex);
                    break;
                case Common::widx::tabMain:
                case Common::widx::tabDetails:
                case Common::widx::tabCargo:
                case Common::widx::tabFinances:
                case Common::widx::tabRoute:
                    Common::switchTab(self, widgetIndex);
                    break;
                case widx::pickup:
                    Common::onPickup(self, widx::pickup);
                    break;
                case widx::remove:
                {
                    auto head = Common::getVehicle(self);
                    FormatArguments args{};
                    args.skip(10);
                    args.push(head->var_22);
                    args.push(head->var_44);
                    gGameCommandErrorTitle = StringIds::cant_sell_string_id;
                    GameCommands::do_6(head->id);
                    break;
                }
            }
        }

        // 0x004B3D73
        static void onResize(window* const self)
        {
            Common::setCaptionEnableState(self);
            self->setSize(minWindowSize, maxWindowSize);
        }

        static void onMouseDown(window* const self, const widget_index widgetIndex)
        {
            if (widgetIndex != widx::buildNew)
                return;

            Dropdown::add(0, StringIds::dropdown_stringid, StringIds::dropdown_modify_vehicle);
            Dropdown::add(1, StringIds::dropdown_stringid, StringIds::dropdown_clone_vehicle);

            widget_t* widget = &self->widgets[widx::buildNew];
            Dropdown::showText(
                self->x + widget->left,
                self->y + widget->top,
                widget->width(),
                widget->height(),
                self->colours[1],
                2,
                0);

            Dropdown::setItemSelected(0);
            Dropdown::setHighlightedItem(0);
        }

        // 0x004B253A
        static void onDropdown(window* const self, const widget_index widgetIndex, const int16_t itemIndex)
        {
            if (widgetIndex != widx::buildNew)
                return;

            if (itemIndex <= 0)
            {
                BuildVehicle::open(self->number, 0);
            }
            else if (itemIndex == 1)
            {
                cloneVehicle(self);
            }
        }

        // 0x004B3C45
        // "Show <vehicle> design details and options" tab in vehicle window
        static void onUpdate(window* self)
        {
            if (self->number == _dragVehicleHead)
            {
                if (WindowManager::find(WindowType::dragVehiclePart) == nullptr)
                {
                    _dragVehicleHead = ThingId::null;
                    _dragCarComponent = nullptr;
                    self->invalidate();
                }
            }

            self->frame_no += 1;
            self->callPrepareDraw();

            WindowManager::invalidateWidget(WindowType::vehicle, self->number, Common::widx::tabDetails);

            if (_dragVehicleHead == ThingId::null && self->isActivated(widx::remove))
            {
                self->activated_widgets &= ~(1ULL << widx::remove);
                WindowManager::invalidateWidget(WindowType::vehicle, self->number, widx::remove);
            }

            if (self->isDisabled(widx::pickup))
            {
                Input::toolCancel(WindowType::vehicle, self->number);
                return;
            }

            auto vehicle = Common::getVehicle(self);
            if (vehicle->tile_x != -1 && (vehicle->var_38 & Vehicles::Flags38::isGhost) == 0)
                return;

            if (!WindowManager::isInFrontAlt(self))
                return;

            if (vehicle->owner != CompanyManager::getControllingId())
                return;

            if (!Input::isToolActive(WindowType::vehicle, self->number))
            {
                Common::onPickup(self, widx::pickup);
            }
        }

        // 0x004B385F
        static void onToolUpdate(window& self, const widget_index widgetIndex, const int16_t x, const int16_t y)
        {
            if (widgetIndex != widx::pickup)
            {
                return;
            }
            Common::pickupToolUpdate(self, x, y);
        }

        // 0x004B386A
        static void onToolDown(window& self, const widget_index widgetIndex, const int16_t x, const int16_t y)
        {
            if (widgetIndex != widx::pickup)
            {
                return;
            }
            Common::pickupToolDown(self, x, y);
        }

        // 0x004B3875
        static void onToolAbort(window& self, const widget_index widgetIndex)
        {
            if (widgetIndex != widx::pickup)
            {
                return;
            }
            Common::pickupToolAbort(self);
        }

        // 0x4B38FA
        static void getScrollSize(Ui::window* const self, const uint32_t scrollIndex, uint16_t* const width, uint16_t* const height)
        {
            *height = static_cast<uint16_t>(Common::getNumCars(self) * self->row_height);
        }

        // 0x004B3B54
        static void scrollMouseDown(window* const self, const int16_t x, const int16_t y, const uint8_t scrollIndex)
        {
            auto head = Common::getVehicle(self);
            if (head->owner != CompanyManager::getControllingId())
            {
                return;
            }

            auto car = Common::getCarFromScrollView(self, y);
            if (!car)
            {
                return;
            }

            OpenLoco::Vehicles::Vehicle train{ head };
            for (auto c : train.cars)
            {
                if (c.front == car->front)
                {
                    Windows::DragVehiclePart::open(c);
                    break;
                }
            }
        }

        // 0x004B399E
        static void scrollMouseOver(window* const self, const int16_t x, const int16_t y, const uint8_t scrollIndex)
        {
            Input::setTooltipTimeout(2000);
            self->flags &= ~WindowFlags::not_scroll_view;
            auto car = Common::getCarFromScrollView(self, y);
            string_id tooltipFormat = StringIds::null;
            thing_id_t tooltipContent = ThingId::null;
            if (car)
            {
                tooltipFormat = StringIds::buffer_337;
                tooltipContent = car->front->id;
            }
            if (self->row_hover != tooltipContent)
            {
                self->row_hover = tooltipContent;
                self->invalidate();
            }

            char* buffer = const_cast<char*>(StringManager::getString(StringIds::buffer_337));
            if (strlen(buffer) != 0)
            {
                if (self->widgets[widx::carList].tooltip == tooltipFormat && self->var_85C == tooltipContent)
                {
                    return;
                }
            }

            self->widgets[widx::carList].tooltip = tooltipFormat;
            self->var_85C = tooltipContent;
            ToolTip::closeAndReset();

            if (tooltipContent == ThingId::null)
            {
                return;
            }

            ToolTip::set_52336E(true);

            auto vehicleObj = ObjectManager::get<vehicle_object>(car->front->object_id);
            {
                FormatArguments args{};
                args.push(vehicleObj->name);
                buffer = StringManager::formatString(buffer, StringIds::tooltip_stringid, &args);
            }

            {
                FormatArguments args{};
                args.push(car->front->creation_day);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_built, &args);
            }

            if (vehicleObj->power != 0 && (vehicleObj->mode == TransportMode::road || vehicleObj->mode == TransportMode::rail))
            {
                FormatArguments args{};
                args.push(vehicleObj->power);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_power, &args);
            }

            {
                FormatArguments args{};
                args.push(vehicleObj->weight);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_weight, &args);
            }

            {
                FormatArguments args{};
                args.push(vehicleObj->speed);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_max_speed, &args);
            }

            if (vehicleObj->flags & FlagsE0::rack_rail)
            {
                FormatArguments args{};
                args.push(vehicleObj->rack_speed);
                auto rackRailObj = ObjectManager::get<track_extra_object>(vehicleObj->rack_rail_type);
                args.push(rackRailObj->name);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_speed_on_stringid, &args);
            }

            {
                FormatArguments args{};
                args.push(car->front->refund_cost);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_value, &args);
            }

            if (car->front->reliability != 0)
            {
                FormatArguments args{};
                args.push(car->front->reliability / 256);
                buffer = StringManager::formatString(buffer, StringIds::vehicle_details_tooltip_reliability, &args);
            }
        }

        // 0x004B3880 TODO: common across 3 tabs
        static std::optional<FormatArguments> tooltip(Ui::window* self, widget_index)
        {
            FormatArguments args{};
            args.push(StringIds::tooltip_scroll_vehicle_list);

            auto vehicle = Common::getVehicle(self);
            args.push(StringIds::getVehicleType(vehicle->vehicleType));
            return args;
        }

        // 0x004B3B18
        static Ui::cursor_id cursor(window* const self, const int16_t widgetIdx, const int16_t x, const int16_t y, const Ui::cursor_id fallback)
        {
            if (widgetIdx != widx::carList)
            {
                return fallback;
            }

            auto head = Common::getVehicle(self);
            if (head->owner != CompanyManager::getControllingId())
            {
                return fallback;
            }

            auto selectedCar = Common::getCarFromScrollView(self, y);
            if (!selectedCar)
            {
                return fallback;
            }
            return cursor_id::unk_25;
        }

        static const std::map<VehicleType, uint32_t> additionalVehicleButtonByVehicleType{
            { VehicleType::train, InterfaceSkin::ImageIds::build_additional_train },
            { VehicleType::bus, InterfaceSkin::ImageIds::build_additional_bus },
            { VehicleType::truck, InterfaceSkin::ImageIds::build_additional_truck },
            { VehicleType::tram, InterfaceSkin::ImageIds::build_additional_tram },
            { VehicleType::aircraft, InterfaceSkin::ImageIds::build_additional_aircraft },
            { VehicleType::ship, InterfaceSkin::ImageIds::build_additional_ship },
        };

        // 0x004B3300
        static void prepareDraw(window* self)
        {
            if (self->widgets != widgets)
            {
                self->widgets = widgets;
                self->initScrollWidgets();
            }

            Common::setActiveTabs(self);

            auto head = Common::getVehicle(self);
            auto args = FormatArguments();
            args.push(head->var_22);
            args.push(head->var_44);

            self->widgets[Common::widx::frame].right = self->width - 1;
            self->widgets[Common::widx::frame].bottom = self->height - 1;
            self->widgets[Common::widx::panel].right = self->width - 1;
            self->widgets[Common::widx::panel].bottom = self->height - 1;
            self->widgets[Common::widx::caption].right = self->width - 2;
            self->widgets[Common::widx::closeButton].left = self->width - 15;
            self->widgets[Common::widx::closeButton].right = self->width - 3;
            Common::repositionTabs(self);

            self->widgets[widx::carList].right = self->width - 26;
            self->widgets[widx::carList].bottom = self->height - 24;

            self->widgets[widx::buildNew].right = self->width - 2;
            self->widgets[widx::buildNew].left = self->width - 25;
            self->widgets[widx::pickup].right = self->width - 2;
            self->widgets[widx::pickup].left = self->width - 25;
            self->widgets[widx::remove].right = self->width - 2;
            self->widgets[widx::remove].left = self->width - 25;

            self->widgets[widx::buildNew].type = widget_type::wt_9;
            self->widgets[widx::pickup].type = widget_type::wt_9;
            self->widgets[widx::remove].type = widget_type::wt_9;
            bool isPlaced = head->tile_x != -1 && !(head->var_38 & OpenLoco::Vehicles::Flags38::isGhost);
            // Differs to main tab! Unsure why.
            if (isPlaced)
            {
                self->widgets[widx::pickup].type = widget_type::none;
            }
            if (head->owner != CompanyManager::getControllingId())
            {
                self->widgets[widx::buildNew].type = widget_type::none;
                self->widgets[widx::pickup].type = widget_type::none;
                self->widgets[widx::remove].type = widget_type::none;
                self->widgets[widx::carList].right = self->width - 4;
            }

            auto skin = ObjectManager::get<interface_skin_object>();
            auto buildImage = skin->img + additionalVehicleButtonByVehicleType.at(head->vehicleType);
            self->widgets[widx::buildNew].image = Gfx::recolour(buildImage, CompanyManager::getCompanyColour(self->owner));

            Vehicles::Vehicle train{ head };
            if (train.cars.empty())
            {
                self->disabled_widgets |= 1 << widx::pickup;
            }
            else
            {
                self->disabled_widgets &= ~(1ULL << widx::pickup);
            }

            auto [pickupImage, pickupTooltip] = Common::getPickupImageIdandTooltip(*head, isPlaced);
            self->widgets[widx::pickup].image = Gfx::recolour(pickupImage);
            self->widgets[widx::pickup].tooltip = pickupTooltip;
        }

        // 0x004B3542
        static void draw(window* const self, Gfx::drawpixelinfo_t* const context)
        {
            self->draw(context);
            Common::drawTabs(self, context);

            // TODO: identical to main tab (doesn't appear to do anything useful)
            if (self->widgets[widx::pickup].type != widget_type::none)
            {
                if ((self->widgets[widx::pickup].image & (1 << 29)) && !self->isDisabled(widx::pickup))
                {
                    auto image = Gfx::recolour(self->widgets[widx::pickup].image, CompanyManager::getCompanyColour(self->owner));
                    Gfx::drawImage(context, self->widgets[widx::pickup].left + self->x, self->widgets[widx::pickup].top + self->y, image);
                }
            }

            auto head = Common::getVehicle(self);
            OpenLoco::Vehicles::Vehicle train{ head };
            Gfx::point_t pos = { static_cast<int16_t>(self->x + 3), static_cast<int16_t>(self->y + self->height - 23) };

            {
                FormatArguments args{};
                args.push(train.veh2->totalPower);
                args.push<uint32_t>(train.veh2->totalWeight);
                string_id str = StringIds::vehicle_details_weight;
                if (train.veh2->mode == TransportMode::rail || train.veh2->mode == TransportMode::road)
                {
                    str = StringIds::vehicle_details_total_power_and_weight;
                }
                Gfx::drawString_494BBF(*context, pos.x, pos.y, self->width - 6, Colour::black, str, &args);
                pos.y += 10;
            }

            {
                FormatArguments args{};
                args.push<uint16_t>(train.veh2->maxSpeed == -1 ? 0 : train.veh2->maxSpeed);
                args.push<uint16_t>(train.veh2->rackRailMaxSpeed == -1 ? 0 : train.veh2->rackRailMaxSpeed);
                args.push<uint16_t>(train.veh2->reliability == 0 ? 64 : train.veh2->reliability);
                string_id str = StringIds::vehicle_details_max_speed_and_reliability;
                if (train.veh1->var_49 != 0)
                {
                    str = StringIds::vehicle_details_max_speed_and_rack_rail_and_reliability;
                }
                Gfx::drawString_494BBF(*context, pos.x, pos.y, self->width - 16, Colour::black, str, &args);
            }
        }

        // 0x004B36A3
        static void drawScroll(window* const self, Gfx::drawpixelinfo_t* const context, const uint32_t i)
        {
            Gfx::clearSingle(*context, Colour::getShade(self->colours[1], 4));
            auto head = Common::getVehicle(self);
            OpenLoco::Vehicles::Vehicle train{ head };
            Gfx::point_t pos{ 0, 0 };
            for (auto& car : train.cars)
            {
                string_id carStr = StringIds::black_stringid;
                if (self->row_hover == car.front->id)
                {
                    carStr = StringIds::wcolour2_stringid;

                    int16_t top = pos.y;
                    int16_t bottom = pos.y + self->row_height - 1;
                    if (_dragCarComponent != nullptr)
                    {
                        bottom = pos.y;
                        top = pos.y - 1;
                        carStr = StringIds::black_stringid;
                    }
                    Gfx::fillRect(context, 0, top, self->width, bottom, 0x2000030);
                }

                int16_t y = pos.y + (self->row_height - 22) / 2;
                uint8_t al = 0;
                uint8_t ah = 0;
                if (car.front == _dragCarComponent)
                {
                    al = 12;
                    ah = self->colours[1];
                }
                auto x = Common::sub_4B743B(al, ah, 0, y, car.front, context);

                auto vehicleObj = ObjectManager::get<vehicle_object>(car.front->object_id);
                FormatArguments args{};
                args.push(vehicleObj->name);
                x += 2;
                y = pos.y + (self->row_height / 2) - 6;
                Gfx::drawString_494B3F(*context, x, y, Colour::black, carStr, &args);

                pos.y += self->row_height;
            }

            if (self->row_hover == train.tail->id && _dragCarComponent != nullptr)
            {
                Gfx::fillRect(context, 0, pos.y - 1, self->width, pos.y, 0x2000030);
            }
        }

        static void initEvents()
        {
            events.on_mouse_up = onMouseUp;
            events.on_resize = onResize;
            events.on_mouse_down = onMouseDown;
            events.on_dropdown = onDropdown;
            events.on_update = onUpdate;
            events.event_08 = Common::event8;
            events.event_09 = Common::event9;
            events.on_tool_update = onToolUpdate;
            events.on_tool_down = onToolDown;
            events.on_tool_abort = onToolAbort;
            events.get_scroll_size = getScrollSize;
            events.scroll_mouse_down = scrollMouseDown;
            events.scroll_mouse_over = scrollMouseOver;
            events.text_input = Common::textInput;
            events.tooltip = tooltip;
            events.cursor = cursor;
            events.prepare_draw = prepareDraw;
            events.draw = draw;
            events.draw_scroll = drawScroll;
        }

        static Ui::window* getVehicleDetailsWindow(const Gfx::point_t& pos)
        {
            auto vehicleWindow = WindowManager::findAt(pos);
            if (vehicleWindow == nullptr || vehicleWindow->type != WindowType::vehicle || vehicleWindow->current_tab != (Common::widx::tabDetails - Common::widx::tabMain))
            {
                return nullptr;
            }
            return vehicleWindow;
        }

        static Vehicles::VehicleBase* getCarFromScrollViewPos(Ui::window& self, const Gfx::point_t& pos)
        {
            int16_t scrollX;
            int16_t scrollY;
            ScrollView::scroll_part part;
            size_t scrollIndex;
            Input::setPressedWidgetIndex(widx::carList);
            Ui::ScrollView::getPart(&self, &self.widgets[widx::carList], pos.x, pos.y, &scrollX, &scrollY, &part, &scrollIndex);
            if (part != ScrollView::scroll_part::view)
            {
                return nullptr;
            }

            auto y = self.row_height / 2 + scrollY;
            auto car = Common::getCarFromScrollView(&self, y);
            if (!car)
            {
                auto head = Common::getVehicle(&self);
                Vehicles::Vehicle train(head);
                return train.tail;
            }
            return car->front;
        }

        void scrollDrag(const Gfx::point_t& pos)
        {
            auto vehicleWindow = getVehicleDetailsWindow(pos);
            if (vehicleWindow == nullptr)
            {
                return;
            }

            auto targetWidget = vehicleWindow->findWidgetAt(pos.x, pos.y);
            switch (targetWidget)
            {
                case widx::remove:
                {
                    if (!vehicleWindow->isActivated(widx::remove))
                    {
                        vehicleWindow->activated_widgets |= (1 << widx::remove);
                        WindowManager::invalidateWidget(WindowType::vehicle, vehicleWindow->number, widx::remove);
                    }
                    return;
                }
                case widx::carList:
                {
                    auto car = getCarFromScrollViewPos(*vehicleWindow, pos);
                    if (car != nullptr)
                    {
                        vehicleWindow->flags &= ~WindowFlags::not_scroll_view;
                        if (car->id != vehicleWindow->row_hover)
                        {
                            vehicleWindow->row_hover = car->id;
                        }
                        vehicleWindow->invalidate();
                    }

                    // TODO: define constant for hot zone
                    if (pos.y < vehicleWindow->widgets[widx::carList].top + vehicleWindow->y + 5)
                    {
                        Ui::ScrollView::verticalNudgeUp(vehicleWindow, vehicleWindow->getScrollDataIndex(widx::carList), widx::carList);
                    }
                    else if (pos.y > vehicleWindow->widgets[widx::carList].bottom + vehicleWindow->y - 5)
                    {
                        Ui::ScrollView::verticalNudgeDown(vehicleWindow, vehicleWindow->getScrollDataIndex(widx::carList), widx::carList);
                    }
                    break;
                }
            }
            if (vehicleWindow->isActivated(widx::remove))
            {
                vehicleWindow->activated_widgets &= ~(1ULL << widx::remove);
                WindowManager::invalidateWidget(WindowType::vehicle, vehicleWindow->number, widx::remove);
            }
        }

        void scrollDragEnd(const Gfx::point_t& pos)
        {
            auto vehicleWindow = getVehicleDetailsWindow(pos);
            if (vehicleWindow == nullptr && _dragCarComponent != nullptr)
            {
                return;
            }

            auto targetWidget = vehicleWindow->findWidgetAt(pos.x, pos.y);
            switch (targetWidget)
            {
                case widx::remove:
                    GameCommands::do_6((*_dragCarComponent)->id);
                    break;
                case widx::carList:
                {
                    auto car = getCarFromScrollViewPos(*vehicleWindow, pos);
                    if (car != nullptr)
                    {
                        GameCommands::do_0((*_dragCarComponent)->id, car->id);
                    }
                    break;
                }
            }
        }
    }

    namespace Cargo
    {
        static void onRefitButton(window* const self, const widget_index wi);

        static bool canRefit(Vehicles::VehicleHead* headVehicle)
        {
            if (!isPlayerCompany(headVehicle->owner))
            {
                return false;
            }

            OpenLoco::Vehicles::Vehicle train(headVehicle);

            if (train.cars.empty())
            {
                return false;
            }

            auto object = ObjectManager::get<vehicle_object>(train.cars.firstCar.front->object_id);
            return (object->flags & FlagsE0::refittable);
        }

        // 004B3DDE
        static void prepareDraw(window* const self)
        {
            if (self->widgets != widgets)
            {
                self->widgets = widgets;
                self->initScrollWidgets();
            }

            Common::setActiveTabs(self);

            auto* headVehicle = Common::getVehicle(self);
            auto ax = headVehicle->var_44;
            auto cx = headVehicle->var_22;

            FormatArguments args = {};
            args.push(cx);
            args.push(ax);

            widgets[Common::widx::frame].right = self->width - 1;
            widgets[Common::widx::frame].bottom = self->height - 1;
            widgets[Common::widx::panel].right = self->width - 1;
            widgets[Common::widx::panel].bottom = self->height - 1;
            widgets[Common::widx::caption].right = self->width - 2;
            widgets[Common::widx::closeButton].left = self->width - 15;
            widgets[Common::widx::closeButton].right = self->width - 3;
            widgets[widx::cargoList].right = self->width - 26;
            widgets[widx::cargoList].bottom = self->height - 14;
            widgets[widx::refit].right = self->width - 2;
            widgets[widx::refit].left = self->width - 25;
            widgets[widx::refit].type = widget_type::wt_9;
            if (!canRefit(headVehicle))
            {
                widgets[widx::refit].type = widget_type::none;
                widgets[widx::cargoList].right = self->width - 26 + 22;
            }

            Common::repositionTabs(self);
        }

        static void generateCargoTotalString(Vehicles::VehicleHead* vehicle, char* buffer)
        {
            uint32_t cargoTotals[ObjectManager::getMaxObjects(object_type::cargo)]{};
            Vehicles::Vehicle train(vehicle);
            for (auto& car : train.cars)
            {
                auto front = car.front;
                auto body = car.body;
                if (front->cargo_type != 0xFF)
                {
                    cargoTotals[front->cargo_type] += front->secondaryCargoQty;
                }
                if (body->cargo_type != 0xFF)
                {
                    cargoTotals[body->cargo_type] += body->primaryCargoQty;
                }
            }

            bool hasCargo = false;
            for (size_t cargoType = 0; cargoType < ObjectManager::getMaxObjects(object_type::cargo); ++cargoType)
            {
                auto cargoTotal = cargoTotals[cargoType];
                if (cargoTotal == 0)
                {
                    continue;
                }

                // On all but first loop insert a ", "
                if (hasCargo)
                {
                    *buffer++ = ',';
                    *buffer++ = ' ';
                }
                hasCargo = true;
                auto cargoObj = ObjectManager::get<cargo_object>(cargoType);
                auto unitNameFormat = cargoTotal == 1 ? cargoObj->unit_name_singular : cargoObj->unit_name_plural;
                FormatArguments args{};
                args.push(cargoTotal);
                buffer = StringManager::formatString(buffer, unitNameFormat, &args);
            }

            if (!hasCargo)
            {
                StringManager::formatString(buffer, StringIds::cargo_empty_2);
            }
        }

        // 004B3F0D
        static void draw(Ui::window* const self, Gfx::drawpixelinfo_t* const context)
        {
            self->draw(context);
            Common::drawTabs(self, context);

            char* buffer = const_cast<char*>(StringManager::getString(StringIds::buffer_1250));
            generateCargoTotalString(Common::getVehicle(self), buffer);

            FormatArguments args = {};
            args.push<string_id>(StringIds::buffer_1250);

            Gfx::drawString_494BBF(*context, self->x + 3, self->y + self->height - 13, self->width - 15, Colour::black, StringIds::total_stringid, &args);
        }

        // based on 0x004B40C7
        static void drawCargoText(Gfx::drawpixelinfo_t* const pDrawpixelinfo, const int16_t x, int16_t& y, const string_id strFormat, uint8_t cargoQty, uint8_t cargoType, station_id_t stationId)
        {
            if (cargoQty == 0)
            {
                return;
            }

            auto cargoObj = ObjectManager::get<cargo_object>(cargoType);
            auto unitNameFormat = cargoQty == 1 ? cargoObj->unit_name_singular : cargoObj->unit_name_plural;
            auto station = StationManager::get(stationId);
            FormatArguments args{};
            args.push(StringIds::cargo_from);
            args.push(unitNameFormat);
            args.push<uint32_t>(cargoQty);
            args.push(station->name);
            args.push(station->town);
            Gfx::drawString_494B3F(*pDrawpixelinfo, x, y, Colour::black, strFormat, &args);
            y += 10;
        }

        // 004B3F62
        static void drawScroll(window* const self, Gfx::drawpixelinfo_t* const pDrawpixelinfo, const uint32_t i)
        {
            Gfx::clearSingle(*pDrawpixelinfo, Colour::getShade(self->colours[1], 4));
            Vehicles::Vehicle train{ Common::getVehicle(self) };
            int16_t y = 0;
            for (auto& car : train.cars)
            {
                string_id strFormat = StringIds::black_stringid;
                auto front = car.front;
                auto body = car.body;
                if (front->id == self->row_hover)
                {
                    Gfx::fillRect(pDrawpixelinfo, 0, y, self->width, y + self->row_height - 1, 0x2000030);
                    strFormat = StringIds::wcolour2_stringid;
                }
                // Get width of the drawing
                auto width = Common::sub_4B743B(1, 0, 0, y, front, pDrawpixelinfo);
                // Actually draw it
                width = Common::sub_4B743B(0, 0, 24 - width, (self->row_height - 22) / 2 + y, car.front, pDrawpixelinfo);

                if (body->cargo_type != 0xFF)
                {

                    int16_t cargoTextHeight = self->row_height / 2 + y - ((self->row_height - 22) / 2) - 10;
                    if (front->secondaryCargoQty != 0 || body->primaryCargoQty != 0)
                    {
                        if (body->primaryCargoQty == 0 || front->secondaryCargoQty == 0)
                        {
                            cargoTextHeight += 5;
                        }
                        drawCargoText(pDrawpixelinfo, width, cargoTextHeight, strFormat, body->primaryCargoQty, body->cargo_type, body->townCargoFrom);
                        drawCargoText(pDrawpixelinfo, width, cargoTextHeight, strFormat, front->secondaryCargoQty, front->cargo_type, front->townCargoFrom);
                    }
                    else
                    {
                        FormatArguments args{};
                        args.push<string_id>(StringIds::cargo_empty);
                        Gfx::drawString_494B3F(*pDrawpixelinfo, width, cargoTextHeight + 5, Colour::black, strFormat, &args);
                    }
                }

                y += self->row_height;
            }
        }

        // 004B41BD
        static void onMouseUp(window* const self, const widget_index i)
        {
            switch (i)
            {
                case Common::widx::closeButton:
                    WindowManager::close(self);
                    break;

                case Common::widx::tabMain:
                case Common::widx::tabDetails:
                case Common::widx::tabCargo:
                case Common::widx::tabFinances:
                case Common::widx::tabRoute:
                    Common::switchTab(self, i);
                    break;

                case Common::widx::caption:
                    Common::renameVehicle(self, i);
                    break;
            }
        }

        // 004B41E2
        static void onMouseDown(window* const self, const widget_index i)
        {
            switch (i)
            {
                case widx::refit:
                    onRefitButton(self, i);
                    break;
            }
        }

        // 004B41E9
        static void onDropdown(window* const self, const widget_index i, const int16_t dropdownIndex)
        {
            switch (i)
            {
                case widx::refit:
                    if (dropdownIndex == -1)
                        break;

                    gGameCommandErrorTitle = StringIds::cant_refit_vehicle;
                    GameCommands::do_64(self->number, Dropdown::getItemArgument(dropdownIndex, 3));
                    break;
            }
        }

        // 0x0042F6B6
        static uint32_t getNumUnitsForCargo(uint32_t maxPrimaryCargo, uint8_t primaryCargoId, uint8_t newCargoId)
        {
            auto cargoObjA = ObjectManager::get<cargo_object>(primaryCargoId);
            auto cargoObjB = ObjectManager::get<cargo_object>(newCargoId);
            return (cargoObjA->unitSize * maxPrimaryCargo) / cargoObjB->unitSize;
        }

        static void onRefitButton(window* const self, const widget_index wi)
        {
            Vehicles::Vehicle train(Common::getVehicle(self));
            auto vehicleObject = ObjectManager::get<vehicle_object>(train.cars.firstCar.front->object_id);
            auto maxPrimaryCargo = vehicleObject->max_primary_cargo;
            auto primaryCargoId = Utility::bitScanForward(vehicleObject->primary_cargo_types);

            int32_t index = 0;
            for (uint16_t cargoId = 0; cargoId < ObjectManager::getMaxObjects(object_type::cargo); cargoId++)
            {
                auto cargoObject = ObjectManager::get<cargo_object>(cargoId);
                if (cargoObject == nullptr)
                    continue;

                if ((cargoObject->flags & CargoObject::Flags::refit) == 0)
                    continue;

                string_id format = StringIds::dropdown_stringid;
                if (cargoId == train.cars.firstCar.body->cargo_type)
                {
                    format = StringIds::dropdown_stringid_selected;
                }

                auto args = FormatArguments();
                args.push<string_id>(cargoObject->unit_name_plural);
                args.push<uint32_t>(getNumUnitsForCargo(maxPrimaryCargo, primaryCargoId, cargoId));
                args.push<uint16_t>(cargoId);
                Dropdown::add(index, format, args);
                index++;
            }

            widget_t& button = self->widgets[wi];

            Dropdown::showText(
                self->x + button.left,
                self->y + button.top,
                button.width(),
                button.height(),
                self->colours[1],
                index,
                0);
            Dropdown::setHighlightedItem(0);
        }

        // 0x004B4339
        static std::optional<FormatArguments> tooltip(Ui::window* self, widget_index)
        {
            FormatArguments args{};
            args.push(StringIds::tooltip_scroll_vehicle_list);

            auto vehicle = Common::getVehicle(self);
            args.push(StringIds::getVehicleType(vehicle->vehicleType));
            return args;
        }

        // 0x004B4360
        static void getScrollSize(Ui::window* const self, const uint32_t scrollIndex, uint16_t* const width, uint16_t* const height)
        {
            *height = static_cast<uint16_t>(Common::getNumCars(self) * self->row_height);
        }

        static char* generateCargoTooltipDetails(char* buffer, const string_id cargoFormat, const uint8_t cargoType, const uint8_t maxCargo, const uint32_t acceptedCargoTypes)
        {
            if (cargoType == 0xFF)
            {
                return buffer;
            }

            {
                auto cargoObj = ObjectManager::get<cargo_object>(cargoType);
                FormatArguments args{};
                args.push(maxCargo == 1 ? cargoObj->unit_name_singular : cargoObj->unit_name_plural);
                args.push<uint32_t>(maxCargo);
                buffer = StringManager::formatString(buffer, cargoFormat, &args);
            }

            auto availableCargoTypes = acceptedCargoTypes & ~(1 << cargoType);
            if (availableCargoTypes != 0)
            {
                *buffer++ = ' ';
                *buffer++ = '(';

                while (availableCargoTypes != 0)
                {
                    auto type = Utility::bitScanForward(availableCargoTypes);
                    availableCargoTypes &= ~(1 << type);

                    auto cargoObj = ObjectManager::get<cargo_object>(type);
                    FormatArguments args{};
                    args.push(cargoObj->name);
                    buffer = StringManager::formatString(buffer, StringIds::stats_or_string, &args);
                    *buffer++ = ' ';
                }
                --buffer;
                *buffer++ = ')';
                *buffer++ = '\0';
            }
            return buffer;
        }

        // 0x004B4404
        static void scrollMouseOver(window* const self, const int16_t x, const int16_t y, const uint8_t scrollIndex)
        {
            Input::setTooltipTimeout(2000);
            self->flags &= ~WindowFlags::not_scroll_view;
            auto car = Common::getCarFromScrollView(self, y);
            string_id tooltipFormat = StringIds::null;
            thing_id_t tooltipContent = ThingId::null;
            if (car)
            {
                tooltipFormat = StringIds::buffer_337;
                tooltipContent = car->front->id;
                if (self->row_hover != tooltipContent)
                {
                    self->row_hover = tooltipContent;
                    self->invalidate();
                }
            }

            char* buffer = const_cast<char*>(StringManager::getString(StringIds::buffer_337));
            if (*buffer != '\0')
            {
                if (self->widgets[widx::cargoList].tooltip == tooltipFormat && self->var_85C == tooltipContent)
                {
                    return;
                }
            }

            self->widgets[widx::cargoList].tooltip = tooltipFormat;
            self->var_85C = tooltipContent;
            ToolTip::closeAndReset();

            if (tooltipContent == ThingId::null)
            {
                return;
            }

            ToolTip::set_52336E(true);

            {
                auto vehicleObj = ObjectManager::get<vehicle_object>(car->front->object_id);
                FormatArguments args{};
                args.push(vehicleObj->name);
                buffer = StringManager::formatString(buffer, StringIds::cargo_capacity_tooltip, &args);
            }

            auto body = car->body;
            auto front = car->front;
            buffer = generateCargoTooltipDetails(buffer, StringIds::cargo_capacity, body->cargo_type, body->max_cargo, body->accepted_cargo_types);
            buffer = generateCargoTooltipDetails(buffer, StringIds::cargo_capacity_plus, front->cargo_type, front->max_cargo, front->accepted_cargo_types);
        }

        // 0x004B4607
        static void onUpdate(window* const self)
        {
            self->frame_no += 1;
            self->callPrepareDraw();
            WindowManager::invalidateWidget(self->type, self->number, 6);
        }

        // 0x004B4621
        static void onResize(window* const self)
        {
            Common::setCaptionEnableState(self);
            self->setSize(minWindowSize, maxWindowSize);
        }

        static void initEvents()
        {
            events.on_mouse_up = onMouseUp;
            events.on_resize = onResize;
            events.on_mouse_down = onMouseDown;
            events.draw = draw;
            events.draw_scroll = drawScroll;
            events.prepare_draw = prepareDraw;
            events.on_dropdown = onDropdown;
            events.text_input = Common::textInput;
            events.tooltip = tooltip;
            events.get_scroll_size = getScrollSize;
            events.scroll_mouse_over = scrollMouseOver;
            events.event_08 = Common::event8;
            events.event_09 = Common::event9;
            events.on_update = onUpdate;
            events.on_resize = onResize;
        }
    }

    namespace Finances
    {
        // 0x004B56CE
        static void prepareDraw(window* self)
        {
            if (self->widgets != widgets)
            {
                self->widgets = widgets;
                self->initScrollWidgets();
            }

            Common::setActiveTabs(self);

            auto vehicle = Common::getVehicle(self);
            auto args = FormatArguments();
            args.push(vehicle->var_22);
            args.push(vehicle->var_44);

            self->widgets[Common::widx::frame].right = self->width - 1;
            self->widgets[Common::widx::frame].bottom = self->height - 1;
            self->widgets[Common::widx::panel].right = self->width - 1;
            self->widgets[Common::widx::panel].bottom = self->height - 1;
            self->widgets[Common::widx::caption].right = self->width - 2;
            self->widgets[Common::widx::closeButton].left = self->width - 15;
            self->widgets[Common::widx::closeButton].right = self->width - 3;

            Common::repositionTabs(self);
        }

        // 0x004C3BA6
        static int32_t getMonthlyRunningCost(Vehicles::Vehicle& train)
        {
            int32_t totalCost = 0;
            for (const auto& car : train.cars)
            {
                auto vehicleObj = ObjectManager::get<vehicle_object>(car.front->object_id);
                // TODO: use FixedPoint with 10 {(1 << 10) == 1024} decimals for cost_index
                auto cost = (vehicleObj->run_cost_factor * currencyMultiplicationFactor[vehicleObj->run_cost_index]) / 1024;
                totalCost += cost;
            }
            return totalCost;
        }

        // 0x004B576C
        static void draw(Ui::window* const self, Gfx::drawpixelinfo_t* const context)
        {
            self->draw(context);
            Common::drawTabs(self, context);

            auto pos = Gfx::point_t(self->x + 4, self->y + 46);

            auto head = Common::getVehicle(self);
            Vehicles::Vehicle train(head);
            auto veh1 = train.veh1;
            if (veh1->var_53 != -1)
            {
                auto args = FormatArguments();
                args.push<uint32_t>(veh1->var_53);
                // Last income on: {DATE DMY}
                Gfx::drawString_494B3F(*context, pos.x, pos.y, Colour::black, StringIds::last_income_on_date, &args);
                pos.y += 10;
                for (int i = 0; i < 4; i++)
                {
                    auto cargoType = veh1->var_57[i];
                    if (cargoType == -1)
                        continue;

                    auto cargoObject = ObjectManager::get<cargo_object>(cargoType);

                    auto str = veh1->var_5b[i] == 1 ? cargoObject->unit_name_singular : cargoObject->unit_name_plural;

                    args = FormatArguments();
                    args.push(str);
                    args.push<uint32_t>(veh1->var_5b[i]);
                    args.push(veh1->var_63[i]);
                    args.push<uint16_t>(veh1->var_6B[i]);
                    args.push<currency32_t>(veh1->var_6F[i]);
                    // {STRINGID} transported {INT16} blocks in {INT16} days = {CURRENCY32}
                    Gfx::drawString_495224(*context, pos.x + 4, pos.y, self->width - 12, Colour::black, StringIds::transported_blocks_in_days, &args);

                    // TODO: fix function to take pointer to offset
                    pos.y += 12;
                }
            }
            else
            {
                // Last income: N/A"
                Gfx::drawString_494B3F(*context, pos.x, pos.y, Colour::black, StringIds::last_income_na);
                pos.y += 10;
            }

            pos.y += 5;

            if (head->lastAverageSpeed != 0)
            {
                // Last journey average speed: {VELOCITY}
                auto args = FormatArguments();
                args.push(head->lastAverageSpeed);
                Gfx::drawString_494B3F(*context, pos.x, pos.y, Colour::black, StringIds::last_journey_average_speed, &args);
                pos.y += 10 + 5;
            }

            {
                // Monthly Running Cost: {CURRENCY32}
                auto args = FormatArguments();
                args.push(getMonthlyRunningCost(train));
                Gfx::drawString_494B3F(*context, pos.x, pos.y, Colour::black, StringIds::vehicle_monthly_running_cost, &args);
                pos.y += 10;
            }

            {
                // Monthly Profit: {CURRENCY32}
                auto args = FormatArguments();
                auto monthlyProfit = (train.veh2->refund_cost + train.veh2->var_66 + train.veh2->var_6A + train.veh2->var_6E) / 4;
                args.push(monthlyProfit);
                Gfx::drawString_494B3F(*context, pos.x, pos.y, Colour::black, StringIds::vehicle_monthly_profit, &args);
                pos.y += 10 + 5;
            }

            {
                // Sale value of vehicle: {CURRENCY32}
                auto args = FormatArguments();
                args.push(train.head->var_69);
                pos.y = self->y + self->height - 14;
                Gfx::drawString_494B3F(*context, pos.x, pos.y, Colour::black, StringIds::sale_value_of_vehicle, &args);
            }
        }

        // 0x004B5945
        static void onMouseUp(window* self, widget_index widgetIndex)
        {
            switch (widgetIndex)
            {
                case Common::widx::closeButton:
                    WindowManager::close(self);
                    break;
                case Common::widx::caption:
                    Common::renameVehicle(self, widgetIndex);
                    break;
                case Common::widx::tabMain:
                case Common::widx::tabDetails:
                case Common::widx::tabCargo:
                case Common::widx::tabFinances:
                case Common::widx::tabRoute:
                    Common::switchTab(self, widgetIndex);
                    break;
            }
        }

        // 0x004B5977
        static std::optional<FormatArguments> tooltip(Ui::window* self, widget_index)
        {
            FormatArguments args{};
            auto veh0 = Common::getVehicle(self);
            args.skip(2);
            args.push(StringIds::getVehicleType(veh0->vehicleType));
            return args;
        }

        // 0x004B5995
        static void onUpdate(window* const self)
        {
            self->frame_no += 1;
            self->callPrepareDraw();
            WindowManager::invalidateWidget(self->type, self->number, Common::widx::tabFinances);
        }

        // 0x004B59AF
        static void onResize(window* const self)
        {
            Common::setCaptionEnableState(self);
            self->setSize(minWindowSize, maxWindowSize);
        }

        static void initEvents()
        {
            events.on_mouse_up = onMouseUp;
            events.on_resize = onResize;
            events.on_update = onUpdate;
            events.text_input = Common::textInput;
            events.tooltip = tooltip;
            events.prepare_draw = prepareDraw;
            events.draw = draw;
        }
    }

    namespace Route
    {
        static loco_global<uint8_t, 0x00113646A> _113646A;

        // 0x00470824
        static void sub_470824(Vehicles::VehicleHead* head)
        {
            registers regs{};
            regs.esi = reinterpret_cast<uint32_t>(head);
            call(0x00470824, regs);
        }

        // 0x004B509B
        static void close(window* const self)
        {
            if (Input::isToolActive(self->type, self->number))
            {
                Input::toolCancel();
            }
        }

        static void orderDeleteCommand(Vehicles::VehicleHead* const head, const uint32_t orderOffset)
        {
            gGameCommandErrorTitle = StringIds::empty;
            GameCommands::do_36(head->id, orderOffset - head->orderTableOffset);
            sub_470824(head);
        }

        static Vehicles::OrderTableView getOrderTable(const Vehicles::VehicleHead* const head)
        {
            return Vehicles::OrderTableView(head->orderTableOffset);
        }

        // 0x004B4F6D
        static void onOrderDelete(Vehicles::VehicleHead* const head, const int16_t orderId)
        {
            // No deleteable orders
            if (head->sizeOfOrderTable <= 1)
            {
                return;
            }

            // orderId can be -1 at this point for none selected
            auto i = 0;
            Vehicles::Order* last = nullptr;
            for (auto& order : getOrderTable(head))
            {
                if (i == orderId)
                {
                    orderDeleteCommand(head, order.getOffset());
                    return;
                }
                last = &order;
                i++;
            }
            // No order selected so delete the last one
            if (last != nullptr)
            {
                orderDeleteCommand(head, last->getOffset());
            }
        }

        // 0x004B4C14
        static bool orderUpCommand(Vehicles::VehicleHead* const head, const uint32_t orderOffset)
        {
            gGameCommandErrorTitle = StringIds::empty;
            auto result = GameCommands::do_75(head->id, orderOffset - head->orderTableOffset);
            sub_470824(head); // Note order changed check if this matters.
            return result != GameCommands::FAILURE;
        }

        // 0x004B4CCB based on
        static bool orderDownCommand(Vehicles::VehicleHead* const head, const uint32_t orderOffset)
        {
            gGameCommandErrorTitle = StringIds::empty;
            auto result = GameCommands::do_76(head->id, orderOffset - head->orderTableOffset);
            sub_470824(head); // Note order changed check if this matters.
            return result != GameCommands::FAILURE;
        }

        // 0x004B4BC1 / 0x004B4C78 based on
        static bool onOrderMove(Vehicles::VehicleHead* const head, const int16_t orderId, bool(orderMoveFunc)(Vehicles::VehicleHead*, uint32_t))
        {
            // No moveable orders
            if (head->sizeOfOrderTable <= 1)
                return false;
            // Valid orderId should be positive (avoid -1 / null)
            if (orderId < 0)
                return false;

            auto* order = getOrderTable(head).atIndex(orderId);
            if (order != nullptr)
            {
                return orderMoveFunc(head, order->getOffset());
            }
            return false;
        }

        // 0x004B4B43
        static void onMouseUp(window* const self, const widget_index widgetIndex)
        {
            switch (widgetIndex)
            {
                case Common::widx::closeButton:
                    WindowManager::close(self);
                    break;
                case Common::widx::caption:
                    Common::renameVehicle(self, widgetIndex);
                    break;
                case Common::widx::tabMain:
                case Common::widx::tabDetails:
                case Common::widx::tabCargo:
                case Common::widx::tabFinances:
                case Common::widx::tabRoute:
                    Common::switchTab(self, widgetIndex);
                    break;
                case widx::orderDelete:
                {

                    onOrderDelete(Common::getVehicle(self), self->var_842);
                    if (self->var_842 == -1)
                    {
                        return;
                    }

                    // Refresh selection (check if we are now at no order selected)
                    auto* order = getOrderTable(Common::getVehicle(self)).atIndex(self->var_842);

                    // If no order selected anymore
                    if (order == nullptr)
                    {
                        self->var_842 = -1;
                    }
                    break;
                }
                case widx::localMode:
                {
                    auto head = Common::getVehicle(self);
                    if (!isPlayerCompany(head->owner))
                        return;

                    Vehicles::Vehicle train(head);
                    if (train.veh1->var_48 & (1 << 1))
                    {
                        gGameCommandErrorTitle = StringIds::empty;
                        GameCommands::do12(head->id, 2);
                    }
                    break;
                }
                case widx::expressMode:
                {
                    auto head = Common::getVehicle(self);
                    if (!isPlayerCompany(head->owner))
                        return;

                    Vehicles::Vehicle train(head);
                    if (!(train.veh1->var_48 & (1 << 1)))
                    {
                        gGameCommandErrorTitle = StringIds::empty;
                        GameCommands::do12(head->id, 2);
                    }
                    break;
                }
                case widx::orderSkip:
                    gGameCommandErrorTitle = StringIds::empty;
                    GameCommands::do_37(self->number);
                    break;
                case widx::orderUp:
                    if (onOrderMove(Common::getVehicle(self), self->var_842, orderUpCommand))
                    {
                        if (self->var_842 <= 0)
                        {
                            return;
                        }
                        self->var_842--;
                    }
                    break;
                case widx::orderDown:
                    if (onOrderMove(Common::getVehicle(self), self->var_842, orderDownCommand))
                    {
                        if (self->var_842 < 0)
                        {
                            return;
                        }
                        auto* order = getOrderTable(Common::getVehicle(self)).atIndex(self->var_842);
                        if (order != nullptr)
                        {
                            self->var_842++;
                        }
                    }
                    break;
            }
        }

        // 0x004B564E
        static void onResize(window* const self)
        {
            Common::setCaptionEnableState(self);
            self->setSize(minWindowSize, maxWindowSize);
        }

        // 0x004B4DD3
        static void createOrderDropdown(window* const self, const widget_index i, const string_id orderType)
        {
            auto head = Common::getVehicle(self);
            auto index = 0;
            for (uint16_t cargoId = 0; cargoId < ObjectManager::getMaxObjects(object_type::cargo); ++cargoId)
            {
                if (!(head->var_4E & (1 << cargoId)))
                {
                    continue;
                }

                auto cargoObj = ObjectManager::get<cargo_object>(cargoId);
                FormatArguments args{};
                args.push(cargoObj->name);
                args.push(cargoObj->unit_inline_sprite);
                args.push(cargoId);
                Dropdown::add(index, orderType, args);
                index++;
            }

            auto x = self->widgets[i].left + self->x;
            auto y = self->widgets[i].top + self->y;
            auto width = self->widgets[i].width();
            auto height = self->widgets[i].height();
            Dropdown::showText(x, y, width, height, self->colours[1], index, 0);
            Dropdown::setHighlightedItem(0);
        }

        // 0x004B4B8C
        static void onMouseDown(window* const self, const widget_index i)
        {
            switch (i)
            {
                case widx::orderForceUnload:
                    createOrderDropdown(self, i, StringIds::orders_unload_all2);
                    break;
                case widx::orderWait:
                    createOrderDropdown(self, i, StringIds::orders_wait_for_full_load_of2);
                    break;
            }
        }

        // order : al (first 3 bits)
        // order argument : eax (3 - 32 bits), cx
        // Note will move orders so do not use while iterating OrderTableView
        // 0x004B4ECB
        static void addNewOrder(window* const self, const Vehicles::Order& order)
        {
            auto head = Common::getVehicle(self);
            auto chosenOffset = head->sizeOfOrderTable - 1;
            if (self->var_842 != -1)
            {
                auto* chosenOrder = getOrderTable(head).atIndex(self->var_842);
                if (chosenOrder != nullptr)
                {
                    chosenOffset = chosenOrder->getOffset() - head->orderTableOffset;
                }
            }
            gGameCommandErrorTitle = StringIds::orders_cant_insert;
            auto previousSize = head->sizeOfOrderTable;
            GameCommands::do_35(head->id, order.getRaw(), chosenOffset);
            sub_470824(head);
            if (head->sizeOfOrderTable == previousSize)
            {
                return;
            }

            if (self->var_842 == -1)
            {
                return;
            }

            self->var_842++;
        }

        // 0x004B4BAC
        static void onDropdown(window* const self, const widget_index i, const int16_t dropdownIndex)
        {
            auto item = dropdownIndex == -1 ? Dropdown::getHighlightedItem() : dropdownIndex;
            if (item == -1)
            {
                return;
            }
            switch (i)
            {
                case widx::orderForceUnload:
                {
                    Vehicles::OrderUnloadAll unload(Dropdown::getItemArgument(item, 3));
                    addNewOrder(self, unload);
                    break;
                }
                case widx::orderWait:
                {
                    Vehicles::OrderWaitFor wait(Dropdown::getItemArgument(item, 3));
                    addNewOrder(self, wait);
                    break;
                }
            }
        }

        // 0x004B55D1
        // "Show <vehicle> route details" tab in vehicle window
        static void onUpdate(window* const self)
        {
            self->frame_no += 1;
            self->callPrepareDraw();

            WindowManager::invalidateWidget(WindowType::vehicle, self->number, 8);

            if (!WindowManager::isInFront(self))
                return;

            if (Common::getVehicle(self)->owner != CompanyManager::getControllingId())
                return;

            if (!Input::isToolActive(WindowType::vehicle, self->number))
            {
                if (Input::toolSet(self, widx::tool, 12))
                {
                    self->invalidate();
                    sub_470824(Common::getVehicle(self));
                }
            }
        }

        // 0x004B4D74
        static std::optional<FormatArguments> tooltip(Ui::window* self, widget_index)
        {
            FormatArguments args{};
            args.push(StringIds::tooltip_scroll_orders_list);
            auto head = Common::getVehicle(self);
            args.push(StringIds::getVehicleType(head->vehicleType));
            return args;
        }

        // 0x004B5A1A
        static std::pair<Ui::ViewportInteraction::InteractionItem, Ui::ViewportInteraction::InteractionArg> sub_4B5A1A(window& self, const int16_t x, const int16_t y)
        {
            registers regs{};
            regs.esi = reinterpret_cast<uint32_t>(&self);
            regs.ax = x;
            regs.cx = y;
            regs.bl = 0; // Not set during function but needed to indicate failure
            call(0x004B5A1A, regs);
            static loco_global<int16_t, 0x0113623C> _mapX;
            static loco_global<int16_t, 0x0113623E> _mapY;
            Ui::ViewportInteraction::InteractionArg output;
            output.x = _mapX;
            output.y = _mapY;
            output.object = reinterpret_cast<void*>(regs.edx);
            return std::make_pair(static_cast<Ui::ViewportInteraction::InteractionItem>(regs.bl), output);
        }

        // 0x004B5088
        static void toolCancel(window& self, const widget_index widgetIdx)
        {
            self.invalidate();
            Input::resetMapSelectionFlag(Input::MapSelectionFlags::unk_04);
            Gfx::invalidateScreen();
        }

        static void onToolDown(window& self, const widget_index widgetIndex, const int16_t x, const int16_t y)
        {
            auto [type, args] = sub_4B5A1A(self, x, y);
            switch (type)
            {
                case Ui::ViewportInteraction::InteractionItem::track:
                {
                    // 0x004B5160
                    auto tileElement = static_cast<tile_element*>(args.object);
                    auto trackElement = tileElement->asTrack();
                    if (trackElement == nullptr)
                        break;
                    auto height = trackElement->baseZ() * 4;
                    auto trackId = trackElement->trackId();
                    const auto& trackPiece = OpenLoco::Map::TrackData::getTrackPiece(trackId);
                    const auto& trackPart = trackPiece[trackElement->unk_5l()];

                    auto pos = Map::rotate2dCoordinate({ trackPart.x, trackPart.y }, trackElement->unkDirection());
                    pos.x += args.x;
                    pos.y += args.y;
                    TilePos tPos{ pos };
                    height -= trackPart.z;

                    Vehicles::OrderRouteWaypoint waypoint(tPos, height / 8, trackElement->unkDirection(), trackId);
                    Audio::playSound(Audio::sound_id::waypoint, { x, y, Input::getDragLastLocation().x }, Input::getDragLastLocation().x);
                    addNewOrder(&self, waypoint);
                    break;
                }
                case Ui::ViewportInteraction::InteractionItem::t_11:
                {
                    // Water
                    auto heights = Map::tileElementHeight(args.x, args.y);
                    auto height = heights.landHeight;
                    if (heights.waterHeight != 0)
                    {
                        height = heights.waterHeight;
                    }
                    Audio::playSound(Audio::sound_id::waypoint, { x, y, Input::getDragLastLocation().x }, Input::getDragLastLocation().x);
                    TilePos tPos{
                        map_pos{ args.x, args.y }
                    };

                    Vehicles::OrderRouteWaypoint waypoint(tPos, height / 8, 0, 0);
                    addNewOrder(&self, waypoint);
                    break;
                }
                case Ui::ViewportInteraction::InteractionItem::station:
                {
                    Audio::playSound(Audio::sound_id::waypoint, { x, y, Input::getDragLastLocation().x }, Input::getDragLastLocation().x);
                    station_id_t stationId = args.value;
                    Vehicles::OrderStopAt station(stationId);
                    addNewOrder(&self, station);
                    break;
                }
                case Ui::ViewportInteraction::InteractionItem::road:
                {
                    // 0x004B5223
                    auto tileElement = static_cast<tile_element*>(args.object);
                    auto trackElement = tileElement->asTrack();
                    if (trackElement == nullptr)
                        break;
                    auto height = trackElement->baseZ() * 4;
                    auto roadId = trackElement->trackId();
                    const auto& roadPiece = OpenLoco::Map::TrackData::getRoadPiece(roadId);
                    const auto& roadPart = roadPiece[trackElement->unk_5l()];

                    auto pos = Map::rotate2dCoordinate({ roadPart.x, roadPart.y }, trackElement->unkDirection());
                    pos.x += args.x;
                    pos.y += args.y;
                    TilePos tPos{ pos };
                    height -= roadPart.z;

                    Vehicles::OrderRouteWaypoint waypoint(tPos, height / 8, trackElement->unkDirection(), roadId);
                    Audio::playSound(Audio::sound_id::waypoint, { x, y, Input::getDragLastLocation().x }, Input::getDragLastLocation().x);
                    addNewOrder(&self, waypoint);
                    break;
                }

                default:
                    break;
            }
        }

        // 0x004B50CE
        static Ui::cursor_id event15(window& self, const int16_t x, const int16_t y, const Ui::cursor_id fallback, bool& out)
        {
            auto typeP = sub_4B5A1A(self, x, y);
            out = typeP.first != Ui::ViewportInteraction::InteractionItem::t_0;
            if (out)
            {
                return cursor_id::arrows_inward;
            }
            return fallback;
        }

        // 0x004B4D9B
        static void getScrollSize(Ui::window* const self, const uint32_t scrollIndex, uint16_t* const width, uint16_t* const height)
        {
            auto head = Common::getVehicle(self);
            auto table = getOrderTable(head);
            *height = 10 * std::distance(table.begin(), table.end());
        }

        static void scrollMouseDown(window* const self, const int16_t x, const int16_t y, const uint8_t scrollIndex)
        {
            auto head = Common::getVehicle(self);
            auto item = y / 10;
            Vehicles::Order* selectedOrder = getOrderTable(head).atIndex(item);
            if (selectedOrder == nullptr)
            {
                item = -1;
            }

            auto toolWindow = Input::toolGetActiveWindow();
            // If another vehicle window is open and has focus (tool)
            if (toolWindow != nullptr && toolWindow->type == self->type && toolWindow->number != self->number)
            {
                if (item == -1)
                {
                    // Copy complete order list
                    Audio::playSound(Audio::sound_id::waypoint, { x, y, Input::getDragLastLocation().x }, Input::getDragLastLocation().x);
                    std::vector<std::shared_ptr<Vehicles::Order>> clonedOrders;
                    for (auto& existingOrders : Vehicles::OrderTableView(head->orderTableOffset))
                    {
                        clonedOrders.push_back(existingOrders.clone());
                    }
                    for (auto& order : clonedOrders)
                    {
                        addNewOrder(toolWindow, *order);
                    }
                    WindowManager::bringToFront(toolWindow);
                }
                else
                {
                    // Copy a single entry on the order list
                    Audio::playSound(Audio::sound_id::waypoint, { x, y, Input::getDragLastLocation().x }, Input::getDragLastLocation().x);
                    auto clonedOrder = selectedOrder->clone();
                    addNewOrder(toolWindow, *clonedOrder);
                    WindowManager::bringToFront(toolWindow);
                }
                return;
            }

            if (item != self->var_842)
            {
                self->var_842 = item;
                self->invalidate();
                return;
            }

            if (selectedOrder == nullptr)
            {
                return;
            }
            switch (selectedOrder->getType())
            {
                case Vehicles::OrderType::StopAt:
                case Vehicles::OrderType::RouteThrough:
                {
                    auto* stationOrder = static_cast<Vehicles::OrderStation*>(selectedOrder);
                    auto station = StationManager::get(stationOrder->getStation());
                    auto main = WindowManager::getMainWindow();
                    if (main)
                    {
                        main->viewportCentreOnTile({ station->x, station->y, static_cast<coord_t>(station->z + 32) });
                    }

                    break;
                }
                case Vehicles::OrderType::RouteWaypoint:
                {
                    auto* routeOrder = selectedOrder->as<Vehicles::OrderRouteWaypoint>();
                    if (routeOrder != nullptr)
                    {
                        auto main = WindowManager::getMainWindow();
                        if (main)
                        {
                            main->viewportCentreOnTile(routeOrder->getWaypoint());
                        }
                    }
                    break;
                }
                case Vehicles::OrderType::UnloadAll:
                case Vehicles::OrderType::WaitFor:
                case Vehicles::OrderType::End:
                    // These orders don't have a location to centre on
                    break;
            }
        }

        // 0x004B530C
        static void scrollMouseOver(window* const self, const int16_t x, const int16_t y, const uint8_t scrollIndex)
        {
            self->flags &= ~WindowFlags::not_scroll_view;
            auto item = y / 10;
            if (self->row_hover != item)
            {
                self->row_hover = item;
                self->invalidate();
            }
        }

        // 0x004B5339
        static Ui::cursor_id cursor(window* const self, const int16_t widgetIdx, const int16_t x, const int16_t y, const Ui::cursor_id fallback)
        {
            if (widgetIdx != widx::routeList)
            {
                return fallback;
            }

            if (Input::isToolActive(self->type, self->number))
            {
                return cursor_id::arrows_inward;
            }
            return fallback;
        }

        // 0x004B56B8 TODO Rename
        static void createViewport(window* const self)
        {
            auto head = Common::getVehicle(self);
            sub_470824(head);
        }

        // 0x004B468C
        static void prepareDraw(window* const self)
        {
            if (self->widgets != widgets)
            {
                self->widgets = widgets;
                self->initScrollWidgets();
            }

            Common::setActiveTabs(self);
            auto head = Common::getVehicle(self);
            FormatArguments args{};
            args.push(head->var_22);
            args.push(head->var_44);

            self->widgets[widx::routeList].tooltip = Input::isToolActive(self->type, self->number) ? StringIds::tooltip_route_scrollview_copy : StringIds::tooltip_route_scrollview;

            self->widgets[Common::widx::frame].right = self->width - 1;
            self->widgets[Common::widx::frame].bottom = self->height - 1;
            self->widgets[Common::widx::panel].right = self->width - 1;
            self->widgets[Common::widx::panel].bottom = self->height - 1;
            self->widgets[Common::widx::caption].right = self->width - 2;
            self->widgets[Common::widx::closeButton].left = self->width - 15;
            self->widgets[Common::widx::closeButton].right = self->width - 3;

            self->widgets[widx::routeList].right = self->width - 26;
            self->widgets[widx::routeList].bottom = self->height - 14;
            self->widgets[widx::orderForceUnload].right = self->width - 2;
            self->widgets[widx::orderWait].right = self->width - 2;
            self->widgets[widx::orderSkip].right = self->width - 2;
            self->widgets[widx::orderDelete].right = self->width - 2;
            self->widgets[widx::orderUp].right = self->width - 2;
            self->widgets[widx::orderDown].right = self->width - 2;
            self->widgets[widx::orderForceUnload].left = self->width - 25;
            self->widgets[widx::orderWait].left = self->width - 25;
            self->widgets[widx::orderSkip].left = self->width - 25;
            self->widgets[widx::orderDelete].left = self->width - 25;
            self->widgets[widx::orderUp].left = self->width - 25;
            self->widgets[widx::orderDown].left = self->width - 25;

            self->disabled_widgets |= (1 << widx::orderForceUnload) | (1 << widx::orderWait) | (1 << widx::orderSkip) | (1 << widx::orderDelete);
            if (head->sizeOfOrderTable != 1)
            {
                self->disabled_widgets &= ~((1 << widx::orderSkip) | (1 << widx::orderDelete));
            }
            if (head->var_4E != 0)
            {
                self->disabled_widgets &= ~((1 << widx::orderWait) | (1 << widx::orderForceUnload));
            }

            // Express / local
            self->activated_widgets = 0;
            Vehicles::Vehicle train(head);
            if (train.veh1->var_48 & (1 << 1))
                self->activated_widgets |= (1 << widx::expressMode);
            else
                self->activated_widgets |= (1 << widx::localMode);

            widget_type type = head->owner == CompanyManager::getControllingId() ? widget_type::wt_9 : widget_type::none;
            self->widgets[widx::orderForceUnload].type = type;
            self->widgets[widx::orderWait].type = type;
            self->widgets[widx::orderSkip].type = type;
            self->widgets[widx::orderDelete].type = type;
            self->widgets[widx::orderUp].type = type;
            self->widgets[widx::orderDown].type = type;
            if (type == widget_type::none)
            {
                self->widgets[widx::routeList].right += 22;
                self->enabled_widgets &= ~(1 << widx::expressMode | 1 << widx::localMode);
            }
            else
            {
                self->enabled_widgets |= (1 << widx::expressMode | 1 << widx::localMode);
            }

            self->widgets[widx::expressMode].right = self->widgets[widx::routeList].right;
            self->widgets[widx::expressMode].left = (self->widgets[widx::expressMode].right - 3) / 2 + 3;
            self->widgets[widx::localMode].right = self->widgets[widx::expressMode].left - 1;

            self->disabled_widgets |= (1 << widx::orderUp) | (1 << widx::orderDown);
            if (self->var_842 != -1)
            {
                self->disabled_widgets &= ~((1 << widx::orderUp) | (1 << widx::orderDown));
            }
            Common::repositionTabs(self);
        }

        // 0x004B4866
        static void draw(window* const self, Gfx::drawpixelinfo_t* const context)
        {
            self->draw(context);
            Common::drawTabs(self, context);

            if (Input::isToolActive(WindowType::vehicle, self->number))
            {
                // Location at bottom left edge of window
                Gfx::point_t loc{ static_cast<int16_t>(self->x + 3), static_cast<int16_t>(self->y + self->height - 13) };

                Gfx::drawString_494BBF(*context, loc.x, loc.y, self->width - 14, Colour::black, StringIds::route_click_on_waypoint);
            }
        }

        const std::array<string_id, 6> orderString = {
            {
                StringIds::orders_end,
                StringIds::orders_stop_at,
                StringIds::orders_route_through,
                StringIds::orders_route_thought_waypoint,
                StringIds::orders_unload_all,
                StringIds::orders_wait_for_full_load_of,
            }
        };

        static const std::array<uint32_t, 63> numberCircle = {
            {
                ImageIds::number_circle_01,
                ImageIds::number_circle_02,
                ImageIds::number_circle_03,
                ImageIds::number_circle_04,
                ImageIds::number_circle_05,
                ImageIds::number_circle_06,
                ImageIds::number_circle_07,
                ImageIds::number_circle_08,
                ImageIds::number_circle_09,
                ImageIds::number_circle_10,
                ImageIds::number_circle_11,
                ImageIds::number_circle_12,
                ImageIds::number_circle_13,
                ImageIds::number_circle_14,
                ImageIds::number_circle_15,
                ImageIds::number_circle_16,
                ImageIds::number_circle_17,
                ImageIds::number_circle_18,
                ImageIds::number_circle_19,
                ImageIds::number_circle_20,
                ImageIds::number_circle_21,
                ImageIds::number_circle_22,
                ImageIds::number_circle_23,
                ImageIds::number_circle_24,
                ImageIds::number_circle_25,
                ImageIds::number_circle_26,
                ImageIds::number_circle_27,
                ImageIds::number_circle_28,
                ImageIds::number_circle_29,
                ImageIds::number_circle_30,
                ImageIds::number_circle_31,
                ImageIds::number_circle_32,
                ImageIds::number_circle_33,
                ImageIds::number_circle_34,
                ImageIds::number_circle_35,
                ImageIds::number_circle_36,
                ImageIds::number_circle_37,
                ImageIds::number_circle_38,
                ImageIds::number_circle_39,
                ImageIds::number_circle_40,
                ImageIds::number_circle_41,
                ImageIds::number_circle_42,
                ImageIds::number_circle_43,
                ImageIds::number_circle_44,
                ImageIds::number_circle_45,
                ImageIds::number_circle_46,
                ImageIds::number_circle_47,
                ImageIds::number_circle_48,
                ImageIds::number_circle_49,
                ImageIds::number_circle_50,
                ImageIds::number_circle_51,
                ImageIds::number_circle_52,
                ImageIds::number_circle_53,
                ImageIds::number_circle_54,
                ImageIds::number_circle_55,
                ImageIds::number_circle_56,
                ImageIds::number_circle_57,
                ImageIds::number_circle_58,
                ImageIds::number_circle_59,
                ImageIds::number_circle_60,
                ImageIds::number_circle_61,
                ImageIds::number_circle_62,
                ImageIds::number_circle_63,
            }
        };

        // 0x004B4A58 based on
        static void sub_4B4A58(window* const self, Gfx::drawpixelinfo_t* const context, const string_id strFormat, FormatArguments& args, Vehicles::Order& order, int16_t& y)
        {
            Gfx::point_t loc = { 8, static_cast<int16_t>(y - 1) };
            Gfx::drawString_494B3F(*context, &loc, Colour::black, strFormat, &args);
            if (order.hasFlag(Vehicles::OrderFlags::HasNumber))
            {
                if (Input::isToolActive(self->type, self->number))
                {
                    auto imageId = numberCircle[_113646A - 1];
                    Gfx::drawImage(context, loc.x + 3, loc.y + 1, Gfx::recolour(imageId, Colour::white));
                }
                _113646A++;
            }
        }

        // 0x004B48BA
        static void drawScroll(window* const self, Gfx::drawpixelinfo_t* const pDrawpixelinfo, const uint32_t i)
        {
            Gfx::clearSingle(*pDrawpixelinfo, Colour::getShade(self->colours[1], 4));

            auto head = Common::getVehicle(self);
            Vehicles::Vehicle train(head);

            auto rowNum = 0;
            if (head->sizeOfOrderTable == 1)
            {
                Gfx::drawString_494B3F(*pDrawpixelinfo, 8, 0, Colour::black, StringIds::no_route_defined);
                rowNum++; // Used to move down the text
            }

            _113646A = 1; // Number ?symbol? TODO: make not a global
            for (auto& order : getOrderTable(head))
            {
                int16_t y = rowNum * 10;
                auto strFormat = StringIds::black_stringid;
                if (self->var_842 == rowNum)
                {
                    Gfx::fillRect(pDrawpixelinfo, 0, y, self->width, y + 9, Colour::aquamarine);
                    strFormat = StringIds::white_stringid;
                }
                if (self->row_hover == rowNum)
                {
                    strFormat = StringIds::wcolour2_stringid;
                    Gfx::fillRect(pDrawpixelinfo, 0, y, self->width, y + 9, 0x2000030);
                }

                FormatArguments args{};
                args.push(orderString[static_cast<uint8_t>(order.getType())]);
                switch (order.getType())
                {
                    case Vehicles::OrderType::End:
                    case Vehicles::OrderType::RouteWaypoint:
                        // Fall through
                        break;
                    case Vehicles::OrderType::StopAt:
                    case Vehicles::OrderType::RouteThrough:
                    {
                        auto* stationOrder = static_cast<Vehicles::OrderStation*>(&order);
                        stationOrder->setFormatArguments(args);
                        break;
                    }
                    case Vehicles::OrderType::UnloadAll:
                    case Vehicles::OrderType::WaitFor:
                    {

                        auto* cargoOrder = static_cast<Vehicles::OrderCargo*>(&order);
                        cargoOrder->setFormatArguments(args);
                        break;
                    }
                }

                sub_4B4A58(self, pDrawpixelinfo, strFormat, args, order, y);
                if (head->currentOrder + head->orderTableOffset == order.getOffset())
                {
                    Gfx::drawString_494B3F(*pDrawpixelinfo, 1, y - 1, Colour::black, StringIds::orders_current_order);
                }

                rowNum++;
            }

            // Output the end of orders
            Gfx::point_t loc = { 8, static_cast<int16_t>(rowNum * 10) };
            auto strFormat = StringIds::black_stringid;
            if (self->var_842 == rowNum)
            {
                Gfx::fillRect(pDrawpixelinfo, 0, loc.y, self->width, loc.y + 10, Colour::aquamarine);
                strFormat = StringIds::white_stringid;
            }
            if (self->row_hover == rowNum)
            {
                strFormat = StringIds::wcolour2_stringid;
                Gfx::fillRect(pDrawpixelinfo, 0, loc.y, self->width, loc.y + 10, 0x2000030);
            }

            loc.y -= 1;
            auto args = FormatArguments::common(orderString[0]);
            Gfx::drawString_494B3F(*pDrawpixelinfo, &loc, Colour::black, strFormat, &args);
        }

        static void initEvents()
        {
            events.on_close = close;
            events.on_mouse_up = onMouseUp;
            events.on_resize = onResize;
            events.on_mouse_down = onMouseDown;
            events.on_dropdown = onDropdown;
            events.on_update = onUpdate;
            events.event_08 = Common::event8;
            events.event_09 = Common::event9;
            events.on_tool_down = onToolDown;
            events.on_tool_abort = toolCancel;
            events.event_15 = event15;
            events.get_scroll_size = getScrollSize;
            events.scroll_mouse_down = scrollMouseDown;
            events.scroll_mouse_over = scrollMouseOver;
            events.text_input = Common::textInput;
            events.viewport_rotate = createViewport;
            events.tooltip = tooltip;
            events.cursor = cursor;
            events.prepare_draw = prepareDraw;
            events.draw = draw;
            events.draw_scroll = drawScroll;
        }
    }

    namespace Common
    {

        struct TabInformation
        {
            const widx widgetIndex;
            widget_t* widgets;
            window_event_list* events;
            const uint64_t* enabledWidgets;
            const uint64_t* holdableWidgets;
        };

        static TabInformation tabInformationByTabOffset[] = {
            { widx::tabMain, Main::widgets, &Main::events, &Main::enabledWidgets, &Main::holdableWidgets },
            { widx::tabDetails, Details::widgets, &Details::events, &Details::enabledWidgets, &Details::holdableWidgets },
            { widx::tabCargo, Cargo::widgets, &Cargo::events, &Cargo::enabledWidgets, &Cargo::holdableWidgets },
            { widx::tabFinances, Finances::widgets, &Finances::events, &Finances::enabledWidgets, &Finances::holdableWidgets },
            { widx::tabRoute, Route::widgets, &Route::events, &Route::enabledWidgets, &Route::holdableWidgets }
        };

        static void setActiveTabs(window* const self)
        {
            self->activated_widgets &= ~((1 << widx::tabMain) | (1 << widx::tabDetails) | (1 << widx::tabCargo) | (1 << widx::tabFinances) | (1 << widx::tabRoute));
            self->activated_widgets |= 1ULL << (widx::tabMain + self->current_tab);
        }

        static std::pair<uint32_t, string_id> getPickupImageIdandTooltip(const Vehicles::VehicleHead& head, const bool isPlaced)
        {
            uint32_t image = 0;
            string_id tooltip = 0;
            switch (head.mode)
            {
                case TransportMode::rail:
                {
                    auto trackObj = ObjectManager::get<track_object>(head.track_type);
                    image = trackObj->image + (isPlaced ? 16 : 17);
                    tooltip = isPlaced ? StringIds::tooltip_remove_from_track : StringIds::tooltip_place_on_track;
                    break;
                }
                case TransportMode::road:
                {
                    auto roadObjId = head.track_type == 0xFF ? _525FC5 : head.track_type;
                    auto roadObj = ObjectManager::get<road_object>(roadObjId);
                    image = roadObj->image + (isPlaced ? 32 : 33);
                    tooltip = isPlaced ? StringIds::tooltip_remove_from_track : StringIds::tooltip_place_on_track;
                    break;
                }
                case TransportMode::air:
                {
                    image = isPlaced ? ImageIds::airport_pickup : ImageIds::airport_place;
                    tooltip = isPlaced ? StringIds::tooltip_remove_from_airport : StringIds::tooltip_place_on_airport;
                    break;
                }
                case TransportMode::water:
                {
                    auto waterObj = ObjectManager::get<water_object>();
                    image = waterObj->image + (isPlaced ? 58 : 59);
                    tooltip = isPlaced ? StringIds::tooltip_remove_from_water : StringIds::tooltip_place_on_dock;
                    break;
                }
            }
            return std::make_pair(image, tooltip);
        }

        // 0x004B26C0
        static void textInput(window* const self, const widget_index callingWidget, char* const input)
        {
            if (callingWidget != widx::caption)
            {
                return;
            }

            if (strlen(input) == 0)
            {
                return;
            }

            gGameCommandErrorTitle = StringIds::cant_rename_this_vehicle;
            const uint32_t* buffer = reinterpret_cast<const uint32_t*>(input);
            GameCommands::do_10(self->number, 1, buffer[0], buffer[1], buffer[2]);
            GameCommands::do_10(0, 2, buffer[3], buffer[4], buffer[5]);
            GameCommands::do_10(0, 0, buffer[6], buffer[7], buffer[8]);
        }

        // 0x004B29C0
        static void pickupToolUpdate(window& self, const int16_t x, const int16_t y)
        {
            registers regs;
            regs.esi = reinterpret_cast<int32_t>(&self);
            regs.ax = x;
            regs.bx = y;
            call(0x004B29C0, regs);
        }

        // 0x004B2C74
        static void pickupToolDown(window& self, const int16_t x, const int16_t y)
        {
            registers regs;
            regs.esi = reinterpret_cast<int32_t>(&self);
            regs.ax = x;
            regs.bx = y;
            call(0x004B2C74, regs);
        }

        // 0x004B3035
        static void pickupToolAbort(window& self)
        {
            auto head = getVehicle(&self);
            if (head->tile_x == -1 || !(head->var_38 & Vehicles::Flags38::isGhost))
            {
                self.invalidate();
                return;
            }

            switch (head->mode)
            {
                case TransportMode::rail:
                case TransportMode::road:
                    GameCommands::do_2(head->id);
                    break;
                case TransportMode::air:
                    GameCommands::do_59(head->id);
                    break;
                case TransportMode::water:
                    GameCommands::do_63(head->id);
                    break;
            }
            self.invalidate();
        }

        // 0x004B2680
        static void renameVehicle(window* self, widget_index widgetIndex)
        {
            auto vehicle = getVehicle(self);
            if (vehicle != nullptr)
            {
                FormatArguments args{};
                args.push(StringIds::getVehicleType(vehicle->vehicleType)); // 0
                args.skip(6);
                args.push(StringIds::getVehicleType(vehicle->vehicleType)); // 8
                TextInput::openTextInput(self, StringIds::title_name_vehicle, StringIds::prompt_enter_new_vehicle_name, vehicle->var_22, widgetIndex, &vehicle->var_44);
            }
        }

        // 0x004B2566
        static void switchTab(window* self, widget_index widgetIndex)
        {
            Input::toolCancel(self->type, self->number);
            TextInput::sub_4CE6C9(self->type, self->number);

            self->current_tab = widgetIndex - Common::widx::tabMain;
            self->frame_no = 0;
            self->flags &= ~WindowFlags::flag_16;
            self->var_85C = -1;
            if (self->viewports[0] != nullptr)
            {
                self->viewports[0]->width = 0;
                self->viewports[0] = nullptr;
                ViewportManager::collectGarbage();
            }

            auto tabInfo = tabInformationByTabOffset[widgetIndex - widx::tabMain];

            self->enabled_widgets = *tabInfo.enabledWidgets;
            self->holdable_widgets = *tabInfo.holdableWidgets;

            self->event_handlers = tabInfo.events;
            self->activated_widgets = 0;
            self->widgets = tabInfo.widgets;
            self->disabled_widgets = 0;
            Main::resetDisabledWidgets(self);
            self->invalidate();
            self->row_hover = -1;
            self->var_842 = -1;
            self->callOnResize();
            self->callPrepareDraw();
            self->initScrollWidgets();
            self->invalidate();
            self->moveInsideScreenEdges();
        }

        // 0x004B5ECD
        static void repositionTabs(window* const self)
        {
            int16_t xPos = self->widgets[widx::tabMain].left;
            const int16_t tabWidth = self->widgets[widx::tabMain].right - xPos;

            for (uint8_t i = widx::tabMain; i <= widx::tabRoute; i++)
            {
                if (self->isDisabled(i))
                    continue;

                self->widgets[i].left = xPos;
                self->widgets[i].right = xPos + tabWidth;
                xPos = self->widgets[i].right + 1;
            }
        }

        // 0x004B1E94
        static void setCaptionEnableState(window* const self)
        {
            self->enabled_widgets |= 1 << widx::caption;
            auto head = getVehicle(self);
            if (head->owner != CompanyManager::getControllingId())
            {
                self->enabled_widgets &= ~static_cast<uint64_t>(1 << widx::caption);
            }
        }

        // 0x004B45DD, 0x004B55A7, 0x004B3C1B
        static void event8(window* const self)
        {
            self->flags |= WindowFlags::not_scroll_view;
        }

        // 0x004B45E5, 0x004B55B6, 0x004B3C23
        static void event9(window* const self)
        {
            if (self->flags & WindowFlags::not_scroll_view)
            {
                if (self->row_hover != -1)
                {
                    self->row_hover = -1;
                    self->invalidate();
                }
            }
        }

        // 0x0050029C
        static const std::array<std::array<uint8_t, 2>, 6> typeToTool = {
            { std::array<uint8_t, 2>{ { 27, 28 } },
              std::array<uint8_t, 2>{ { 29, 30 } },
              std::array<uint8_t, 2>{ { 31, 32 } },
              std::array<uint8_t, 2>{ { 33, 34 } },
              std::array<uint8_t, 2>{ { 35, 35 } },
              std::array<uint8_t, 2>{ { 36, 36 } } }
        };

        // 0x004B28E2
        static void onPickup(window* const self, const widget_index pickupWidx)
        {
            self->invalidate();
            auto head = getVehicle(self);
            if (head->tile_x == -1 || head->var_38 & Vehicles::Flags38::isGhost)
            {
                auto tool = typeToTool[static_cast<uint8_t>(head->vehicleType)][_pickupDirection != 0 ? 1 : 0];
                if (Input::toolSet(self, pickupWidx, tool))
                {
                    _1136264 = -1;
                }
                return;
            }

            gGameCommandErrorTitle = StringIds::cant_remove_string_id;
            FormatArguments args{};
            args.skip(10);
            args.push(head->var_22);
            args.push(head->var_44);

            bool success = false;
            switch (head->mode)
            {
                case TransportMode::rail:
                case TransportMode::road:
                    success = GameCommands::do_2(head->id);
                    break;
                case TransportMode::air:
                    success = GameCommands::do_59(head->id);
                    break;
                case TransportMode::water:
                    success = GameCommands::do_63(head->id);
                    break;
            }
            if (success)
            {
                self->callOnMouseUp(widx::tabDetails);
            }
        }

        static size_t getNumCars(Ui::window* const self)
        {
            Vehicles::Vehicle train(Common::getVehicle(self));

            if (train.cars.empty())
            {
                return 0;
            }

            return train.cars.size();
        }

        // TODO: Move to a more appropriate file used by many windows
        int16_t sub_4B743B(uint8_t al, uint8_t ah, int16_t cx, int16_t dx, Vehicles::VehicleBase* vehicle, Gfx::drawpixelinfo_t* const pDrawpixelinfo)
        {
            registers regs{};
            regs.al = al;
            regs.ah = ah;
            regs.cx = cx;
            regs.dx = dx;
            regs.esi = reinterpret_cast<uint32_t>(vehicle);
            regs.edi = reinterpret_cast<uint32_t>(pDrawpixelinfo);
            call(0x004B743B, regs);
            return regs.cx;
        }

        // 0x004B5CC1
        static std::optional<Vehicles::Car> getCarFromScrollView(window* const self, const int16_t y)
        {
            Vehicles::Vehicle train(getVehicle(self));

            auto heightOffset = y;
            for (auto& car : train.cars)
            {
                heightOffset -= self->row_height;
                if (heightOffset <= 0)
                {
                    return { car };
                }
            }
            return {};
        }

        struct TabIcons
        {
            uint32_t image;
            uint8_t frameSpeed;
        };

        static const std::map<VehicleType, TabIcons> tabIconByVehicleType{
            { VehicleType::train, { InterfaceSkin::ImageIds::tab_vehicle_train_frame0, 1 } },
            { VehicleType::bus, { InterfaceSkin::ImageIds::tab_vehicle_bus_frame0, 1 } },
            { VehicleType::truck, { InterfaceSkin::ImageIds::tab_vehicle_truck_frame0, 1 } },
            { VehicleType::tram, { InterfaceSkin::ImageIds::tab_vehicle_tram_frame0, 1 } },
            { VehicleType::aircraft, { InterfaceSkin::ImageIds::tab_vehicle_aircraft_frame0, 2 } },
            { VehicleType::ship, { InterfaceSkin::ImageIds::tab_vehicle_ship_frame0, 3 } },
        };

        // 0x004B5F0D
        static void drawTabs(window* const self, Gfx::drawpixelinfo_t* const context)
        {
            auto skin = OpenLoco::ObjectManager::get<interface_skin_object>();

            auto vehicle = Common::getVehicle(self);
            auto vehicleType = vehicle->vehicleType;

            auto mainTab = tabIconByVehicleType.at(vehicleType);
            int frame = 0;
            if (self->current_tab == 0)
            {
                frame = (self->frame_no >> mainTab.frameSpeed) & 0x7;
            }

            Widget::draw_tab(
                self,
                context,
                Gfx::recolour(skin->img + mainTab.image + frame, CompanyManager::getCompanyColour(self->owner)),
                widx::tabMain);

            frame = 0;
            if (self->current_tab == 1)
            {
                frame = (self->frame_no >> 1) & 0xF;
            }
            Widget::draw_tab(
                self,
                context,
                skin->img + InterfaceSkin::ImageIds::tab_wrench_frame0 + frame,
                widx::tabDetails);

            frame = 0;
            if (self->current_tab == 2)
            {
                frame = (self->frame_no >> 3) & 0x3;
            }
            Widget::draw_tab(
                self,
                context,
                skin->img + InterfaceSkin::ImageIds::tab_cargo_delivered_frame0 + frame,
                widx::tabCargo);

            frame = 0;
            if (self->current_tab == 4)
            {
                frame = (self->frame_no >> 4) & 0x3;
            }
            Widget::draw_tab(
                self,
                context,
                skin->img + InterfaceSkin::ImageIds::tab_routes_frame_0 + frame,
                widx::tabRoute);

            frame = 0;
            if (self->current_tab == 3)
            {
                frame = (self->frame_no >> 1) & 0xF;
            }
            Widget::draw_tab(
                self,
                context,
                skin->img + InterfaceSkin::ImageIds::tab_finances_frame0 + frame,
                widx::tabFinances);
        }
    }

    // 0x004B949C
    bool rotate()
    {
        if (Input::isToolActive(WindowType::vehicle))
        {
            if (Input::getToolWidgetIndex() == Main::widx::pickup || Input::getToolWidgetIndex() == Details::widx::pickup)
            {
                _pickupDirection = _pickupDirection ^ 1;
                return true;
            }
        }

        return false;
    }

    void registerHooks()
    {
        Main::initEvents();
        Cargo::initEvents();
        Details::initEvents();
        Finances::initEvents();
        Route::initEvents();
    }
}
