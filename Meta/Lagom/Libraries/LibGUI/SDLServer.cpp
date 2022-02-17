/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Singleton.h>
#include <LibCore/EventLoop.h>
#include <LibCore/Timer.h>
#include <LibGUI/Event.h>
#include <LibGUI/SDLServer.h>
#include <LibGUI/Window.h>
#include <LibGUI/WindowServerConnection.h>
#include <LibGfx/Bitmap.h>

namespace GUI {

static Singleton<SDLServer> s_the;
SDLServer& SDLServer::the()
{
    return *s_the;
}

SDLServer::SDLServer()
{
    SDL_Init(SDL_INIT_VIDEO);
    dbgln("SDL: Brought up SDL");

    m_process_loop = Core::Timer::create_repeating(16, [this] {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                this->quit();
                return;
            }

            switch (event.type) {
            case SDL_MOUSEMOTION: {
                auto window = Window::from_window_id(get_window_from_sdl_id(event.window.windowID));
                WindowServerConnection::the().mouse_move(window->window_id(), Gfx::IntPoint(event.motion.x, event.motion.y), 0, 0, 0, 0, 0, false, Vector<String>());
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                auto window = Window::from_window_id(get_window_from_sdl_id(event.window.windowID));

                u32 buttons = 0;
                if (event.button.button == SDL_BUTTON_LEFT)
                    buttons = GUI::MouseButton::Primary;
                if (event.button.button == SDL_BUTTON_RIGHT)
                    buttons = GUI::MouseButton::Secondary;

                if (buttons > 0)
                    WindowServerConnection::the().mouse_down(window->window_id(), Gfx::IntPoint(event.motion.x, event.motion.y), buttons, buttons, 0, 0, 0);
                break;
            }
            case SDL_MOUSEBUTTONUP: {
                auto window = Window::from_window_id(get_window_from_sdl_id(event.window.windowID));

                u32 buttons = 0;
                if (event.button.button == SDL_BUTTON_LEFT)
                    buttons = GUI::MouseButton::Primary;
                if (event.button.button == SDL_BUTTON_RIGHT)
                    buttons = GUI::MouseButton::Secondary;

                if (buttons > 0)
                    WindowServerConnection::the().mouse_up(window->window_id(), Gfx::IntPoint(event.motion.x, event.motion.y), buttons, buttons, 0, 0, 0);
                break;
            }
            case SDL_WINDOWEVENT: {
                auto window = Window::from_window_id(get_window_from_sdl_id(event.window.windowID));
                switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED: {
                    WindowServerConnection::the().window_resized(window->window_id(), Gfx::IntRect(0, 0, event.window.data1, event.window.data2));
                    break;
                }
                }
                break;
            }
            }
        }

        for (auto& entry : m_windows) {
            auto window = Window::from_window_id(entry.key);
            if (!window->is_visible())
                continue;

            Vector<Gfx::IntRect> rects = { window->rect() };
            WindowServerConnection::the().paint(window->window_id(), window->size(), rects);

            auto sdl_window = entry.value;
            auto screen_surface = SDL_GetWindowSurface(sdl_window);

            auto bitmap = window->back_bitmap();
            if (!bitmap)
                continue;

            int sdl_width, sdl_height;
            SDL_GetWindowSize(sdl_window, &sdl_width, &sdl_height);

            // If we resize the window fast enough there will be a size missmatch; let's handle that.
            if (bitmap->width() != sdl_width || bitmap->height() != sdl_height) {
                warn("SDL: Window size mismatch! {} != {} || {} != {}\n", bitmap->width(), sdl_width, bitmap->height(), sdl_height);
                return;
            }

            // FIXME: only do this if the bitmap is dirty
            SDL_LockSurface(screen_surface);

            // FIXME: This is a horrible way to do it, but dealing with SDL_BlitSurface is a pain future me
            // will have to deal with :^)
            for (int x = 0; x < bitmap->width(); x++) {
                for (int y = 0; y < bitmap->height(); y++) {
                    auto pixel = bitmap->get_pixel(x, y);

                    Uint32* p_screen = (Uint32*)screen_surface->pixels;
                    p_screen += y * screen_surface->w + x;
                    *p_screen = SDL_MapRGBA(screen_surface->format, pixel.red(), pixel.green(), pixel.blue(), pixel.alpha());
                }
            }
            SDL_UnlockSurface(screen_surface);

            SDL_UpdateWindowSurface(sdl_window);
        }
    });
    m_process_loop->start();
}

i32 SDLServer::get_window_from_sdl_id(u32 sdl_id)
{
    for (auto& entry : m_windows) {
        auto sdl_window_id = SDL_GetWindowID(entry.value);

        if (sdl_window_id == sdl_id)
            return entry.key;
    }

    return -1;
}

void SDLServer::quit()
{
    dbgln("SDL: Quitting");
    m_process_loop->stop();

    SDL_Quit();
    auto& loop = Core::EventLoop::current();
    loop.quit(0);
}

void SDLServer::register_window(i32 window_id, SDL_Window* window)
{
    dbgln("SDL: Registering window with id {}", window_id);
    m_windows.set(window_id, window);
}

void SDLServer::deregister_window(i32 window_id)
{
    dbgln("SDL: Removing window with id {}", window_id);
    auto window = m_windows.get(window_id).value();
    SDL_DestroyWindow(window);
    m_windows.remove(window_id);
}

void SDLServer::set_window_title(i32 window_id, String title)
{
    SDL_SetWindowTitle(m_windows.get(window_id).value(), title.characters());
}

String SDLServer::get_window_title(i32 window_id)
{
    return String(SDL_GetWindowTitle(m_windows.get(window_id).value()));
}

void SDLServer::set_window_rect(i32 window_id, Gfx::IntRect const& rect)
{
    auto window = m_windows.get(window_id).value();
    SDL_SetWindowPosition(window, rect.x(), rect.y());
    SDL_SetWindowSize(window, rect.width(), rect.height());
}

Gfx::IntRect SDLServer::get_window_rect(i32 window_id)
{
    auto window = m_windows.get(window_id).value();
    auto size = SDL_GetWindowSurface(window);

    int x, y;
    SDL_GetWindowPosition(window, &x, &y);

    return Gfx::IntRect(x, y, size->w, size->h);
}

void SDLServer::set_window_resize_aspect_ratio(i32, Optional<Gfx::IntSize> const&)
{
    // NOP
}

}
