/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <AK/Singleton.h>
#include <LibCore/EventLoop.h>
#include <LibGUI/Event.h>
#include <LibGUI/SDLServer.h>
#include <LibGUI/Window.h>
#include <LibGUI/WindowServerConnection.h>
#include <SDL.h>

namespace GUI {

static Singleton<WindowServerConnection> s_the;
WindowServerConnection& WindowServerConnection::the()
{
    return *s_the;
}

WindowServerConnection::WindowServerConnection()
{
}

void WindowServerConnection::paint(i32 window_id, Gfx::IntSize const& window_size, Vector<Gfx::IntRect> const& rects)
{
    if (auto* window = Window::from_window_id(window_id))
        Core::EventLoop::current().post_event(*window, make<MultiPaintEvent>(rects, window_size));
}

void WindowServerConnection::window_resized(i32 window_id, Gfx::IntRect const& new_rect)
{
    if (auto* window = Window::from_window_id(window_id)) {
        Core::EventLoop::current().post_event(*window, make<ResizeEvent>(new_rect.size()));
    }
}

static MouseButton to_mouse_button(u32 button)
{
    switch (button) {
    case 0:
        return MouseButton::None;
    case 1:
        return MouseButton::Primary;
    case 2:
        return MouseButton::Secondary;
    case 4:
        return MouseButton::Middle;
    case 8:
        return MouseButton::Backward;
    case 16:
        return MouseButton::Forward;
    default:
        VERIFY_NOT_REACHED();
        break;
    }
}

void WindowServerConnection::mouse_move(i32 window_id, Gfx::IntPoint const& mouse_position, u32 button, u32 buttons, u32 modifiers, i32 wheel_delta_x, i32 wheel_delta_y, bool is_drag, Vector<String> const& mime_types)
{
    if (auto* window = Window::from_window_id(window_id)) {
        if (is_drag)
            Core::EventLoop::current().post_event(*window, make<DragEvent>(Event::DragMove, mouse_position, mime_types));
        else
            Core::EventLoop::current().post_event(*window, make<MouseEvent>(Event::MouseMove, mouse_position, buttons, to_mouse_button(button), modifiers, wheel_delta_x, wheel_delta_y));
    }
}

void WindowServerConnection::mouse_down(i32 window_id, Gfx::IntPoint const& mouse_position, u32 button, u32 buttons, u32 modifiers, i32 wheel_delta_x, i32 wheel_delta_y)
{
    if (auto* window = Window::from_window_id(window_id))
        Core::EventLoop::current().post_event(*window, make<MouseEvent>(Event::MouseDown, mouse_position, buttons, to_mouse_button(button), modifiers, wheel_delta_x, wheel_delta_y));
}

void WindowServerConnection::mouse_up(i32 window_id, Gfx::IntPoint const& mouse_position, u32 button, u32 buttons, u32 modifiers, i32 wheel_delta_x, i32 wheel_delta_y)
{
    if (auto* window = Window::from_window_id(window_id))
        Core::EventLoop::current().post_event(*window, make<MouseEvent>(Event::MouseUp, mouse_position, buttons, to_mouse_button(button), modifiers, wheel_delta_x, wheel_delta_y));
}

void WindowServerConnection::async_create_window(i32 window_id, Gfx::IntRect const& rect,
    bool auto_position, bool has_alpha_channel, bool modal, bool minimizable, bool closeable, bool resizable,
    bool fullscreen, bool frameless, bool forced_shadow, bool accessory, float opacity,
    float alpha_hit_threshold, Gfx::IntSize const& base_size, Gfx::IntSize const& size_increment,
    Gfx::IntSize const& minimum_size, Optional<Gfx::IntSize> const& resize_aspect_ratio, i32 type,
    String const& title, i32 parent_window_id, Gfx::IntRect const& launch_origin_rect)
{
    // FIXME: track child windows

    SDL_Window* window = SDL_CreateWindow(
        title.characters(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        rect.width(),
        rect.height(),
        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_SetWindowResizable(window, SDL_bool(resizable));
    SDL_SetWindowOpacity(window, opacity);
    SDLServer::the().register_window(window_id, window);
}

Vector<i32>& WindowServerConnection::destroy_window(i32 window_id)
{
    // FIXME: destroy child windows
    SDLServer::the().deregister_window(window_id);

    auto destroyed_window_ids = new Vector<i32>();
    destroyed_window_ids->append(window_id);
    return *destroyed_window_ids;
}

void WindowServerConnection::async_set_window_title(i32 window_id, String const& title)
{
    SDLServer::the().set_window_title(window_id, title);
}

String WindowServerConnection::get_window_title(i32 window_id)
{
    return SDLServer::the().get_window_title(window_id);
}

void WindowServerConnection::async_did_finish_painting(i32, Vector<Gfx::IntRect> const&)
{
    // NOP
}

void WindowServerConnection::async_invalidate_rect(i32, Vector<Gfx::IntRect> const&, bool)
{
    // NOP
}

void WindowServerConnection::async_set_forced_shadow(i32, bool)
{
    // NOP
}

void WindowServerConnection::async_refresh_system_theme()
{
    // NOP
}

void WindowServerConnection::async_set_fullscreen(i32, bool)
{
    // TODO
}

void WindowServerConnection::async_set_frameless(i32, bool)
{
    // TODO
}

void WindowServerConnection::async_set_maximized(i32, bool)
{
    // TODO
}

Gfx::IntPoint WindowServerConnection::get_global_cursor_position()
{
    // FIXME
    return Gfx::IntPoint(0, 0);
}

Gfx::IntRect const& WindowServerConnection::set_window_rect(i32 window_id, Gfx::IntRect const& rect)
{
    SDLServer::the().set_window_rect(window_id, rect);

    return rect;
}

Gfx::IntRect WindowServerConnection::get_window_rect(i32 window_id)
{
    return SDLServer::the().get_window_rect(window_id);
}

void WindowServerConnection::async_move_window_to_front(i32 window_id)
{
    // NOP
}

Gfx::IntRect WindowServerConnection::get_applet_rect_on_screen(i32 window_id)
{
    return Gfx::IntRect(0, 0, 0, 0);
}

Gfx::IntSize WindowServerConnection::get_window_minimum_size(i32 window_id)
{
    return Gfx::IntSize(0, 0);
}

void WindowServerConnection::async_set_window_minimum_size(i32 window_id, Gfx::IntSize size)
{
    // TODO
}

void WindowServerConnection::async_set_window_resize_aspect_ratio(i32 window_id, Optional<Gfx::IntSize> const& resize_aspect_ratio)
{
    SDLServer::the().set_window_resize_aspect_ratio(window_id, resize_aspect_ratio);
}

}
#pragma GCC diagnostic pop
