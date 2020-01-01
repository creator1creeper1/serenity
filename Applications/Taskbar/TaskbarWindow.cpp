#include "TaskbarWindow.h"
#include "TaskbarButton.h"
#include <AK/SharedBuffer.h>
#include <LibCore/CConfigFile.h>
#include <LibGUI/GBoxLayout.h>
#include <LibGUI/GButton.h>
#include <LibGUI/GDesktop.h>
#include <LibGUI/GFrame.h>
#include <LibGUI/GWindow.h>
#include <stdio.h>

//#define EVENT_DEBUG

TaskbarWindow::TaskbarWindow()
{
    set_window_type(GWindowType::Taskbar);
    set_title("Taskbar");

    on_screen_rect_change(GDesktop::the().rect());

    GDesktop::the().on_rect_change = [this](const Rect& rect) { on_screen_rect_change(rect); };

    auto widget = GFrame::construct();
    widget->set_fill_with_background_color(true);
    widget->set_layout(make<GBoxLayout>(Orientation::Horizontal));
    widget->layout()->set_margins({ 3, 2, 3, 2 });
    widget->layout()->set_spacing(3);
    widget->set_frame_thickness(1);
    widget->set_frame_shape(FrameShape::Panel);
    widget->set_frame_shadow(FrameShadow::Raised);
    set_main_widget(widget);

    WindowList::the().aid_create_button = [this](auto& identifier) {
        return create_button(identifier);
    };

    create_quick_launch_bar();
}

TaskbarWindow::~TaskbarWindow()
{
}

void TaskbarWindow::create_quick_launch_bar()
{
    auto quick_launch_bar = GFrame::construct(main_widget());
    quick_launch_bar->set_size_policy(SizePolicy::Fixed, SizePolicy::Fixed);
    quick_launch_bar->set_layout(make<GBoxLayout>(Orientation::Horizontal));
    quick_launch_bar->layout()->set_spacing(3);
    quick_launch_bar->layout()->set_margins({ 3, 0, 3, 0 });
    quick_launch_bar->set_frame_thickness(1);
    quick_launch_bar->set_frame_shape(FrameShape::Container);
    quick_launch_bar->set_frame_shadow(FrameShadow::Raised);

    int total_width = 6;
    bool first = true;

    auto config = CConfigFile::get_for_app("Taskbar");
    constexpr const char* quick_launch = "QuickLaunch";

    // FIXME: CConfigFile does not keep the order of the entries.
    for (auto& name : config->keys(quick_launch)) {
        auto af_name = config->read_entry(quick_launch, name);
        ASSERT(!af_name.is_null());
        auto af_path = String::format("/res/apps/%s", af_name.characters());
        auto af = CConfigFile::open(af_path);
        auto app_executable = af->read_entry("App", "Executable");
        auto app_icon_path = af->read_entry("Icons", "16x16");

        auto button = GButton::construct(quick_launch_bar);
        button->set_size_policy(SizePolicy::Fixed, SizePolicy::Fixed);
        button->set_preferred_size(22, 22);
        button->set_button_style(ButtonStyle::CoolBar);

        button->set_icon(GraphicsBitmap::load_from_file(app_icon_path));
        // FIXME: the tooltip ends up outside the screen rect.
        button->set_tooltip(name);
        button->on_click = [app_executable](auto&) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork");
            } else if (pid == 0) {
                execl(app_executable.characters(), app_executable.characters(), nullptr);
                perror("execl");
                ASSERT_NOT_REACHED();
            }
        };

        if (!first)
            total_width += 3;
        first = false;
        total_width += 22;
    }

    quick_launch_bar->set_preferred_size(total_width, 22);
}

void TaskbarWindow::on_screen_rect_change(const Rect& rect)
{
    Rect new_rect { rect.x(), rect.bottom() - taskbar_height() + 1, rect.width(), taskbar_height() };
    set_rect(new_rect);
}

NonnullRefPtr<GButton> TaskbarWindow::create_button(const WindowIdentifier& identifier)
{
    auto button = TaskbarButton::construct(identifier, main_widget());
    button->set_size_policy(SizePolicy::Fixed, SizePolicy::Fixed);
    button->set_preferred_size(140, 22);
    button->set_checkable(true);
    button->set_text_alignment(TextAlignment::CenterLeft);
    return button;
}

static bool should_include_window(GWindowType window_type)
{
    return window_type == GWindowType::Normal;
}

void TaskbarWindow::wm_event(GWMEvent& event)
{
    WindowIdentifier identifier { event.client_id(), event.window_id() };
    switch (event.type()) {
    case GEvent::WM_WindowRemoved: {
#ifdef EVENT_DEBUG
        auto& removed_event = static_cast<GWMWindowRemovedEvent&>(event);
        dbgprintf("WM_WindowRemoved: client_id=%d, window_id=%d\n",
            removed_event.client_id(),
            removed_event.window_id());
#endif
        WindowList::the().remove_window(identifier);
        update();
        break;
    }
    case GEvent::WM_WindowRectChanged: {
#ifdef EVENT_DEBUG
        auto& changed_event = static_cast<GWMWindowRectChangedEvent&>(event);
        dbgprintf("WM_WindowRectChanged: client_id=%d, window_id=%d, rect=%s\n",
            changed_event.client_id(),
            changed_event.window_id(),
            changed_event.rect().to_string().characters());
#endif
        break;
    }

    case GEvent::WM_WindowIconBitmapChanged: {
        auto& changed_event = static_cast<GWMWindowIconBitmapChangedEvent&>(event);
#ifdef EVENT_DEBUG
        dbgprintf("WM_WindowIconBitmapChanged: client_id=%d, window_id=%d, icon_buffer_id=%d\n",
            changed_event.client_id(),
            changed_event.window_id(),
            changed_event.icon_buffer_id());
#endif
        if (auto* window = WindowList::the().window(identifier)) {
            auto buffer = SharedBuffer::create_from_shared_buffer_id(changed_event.icon_buffer_id());
            ASSERT(buffer);
            window->button()->set_icon(GraphicsBitmap::create_with_shared_buffer(GraphicsBitmap::Format::RGBA32, *buffer, changed_event.icon_size()));
        }
        break;
    }

    case GEvent::WM_WindowStateChanged: {
        auto& changed_event = static_cast<GWMWindowStateChangedEvent&>(event);
#ifdef EVENT_DEBUG
        dbgprintf("WM_WindowStateChanged: client_id=%d, window_id=%d, title=%s, rect=%s, is_active=%u, is_minimized=%u\n",
            changed_event.client_id(),
            changed_event.window_id(),
            changed_event.title().characters(),
            changed_event.rect().to_string().characters(),
            changed_event.is_active(),
            changed_event.is_minimized());
#endif
        if (!should_include_window(changed_event.window_type()))
            break;
        auto& window = WindowList::the().ensure_window(identifier);
        window.set_title(changed_event.title());
        window.set_rect(changed_event.rect());
        window.set_active(changed_event.is_active());
        window.set_minimized(changed_event.is_minimized());
        if (window.is_minimized()) {
            window.button()->set_foreground_color(Color::DarkGray);
            window.button()->set_text(String::format("[%s]", changed_event.title().characters()));
        } else {
            window.button()->set_foreground_color(Color::Black);
            window.button()->set_text(changed_event.title());
        }
        window.button()->set_checked(changed_event.is_active());
        break;
    }
    default:
        break;
    }
}
