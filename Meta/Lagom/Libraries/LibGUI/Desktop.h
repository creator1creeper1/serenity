/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/Function.h>
#include <AK/String.h>
#include <AK/Vector.h>
#include <LibGUI/WindowServerConnection.h>
#include <LibGfx/Rect.h>

namespace GUI {

class Desktop {
public:
    static constexpr size_t default_screen_rect_count = 4;

    static Desktop& the();
    Desktop();

    void set_background_color(StringView) { }

    void set_wallpaper_mode(StringView) { }

    String wallpaper() const { return ""; }
    bool set_wallpaper(StringView, bool) { return false; }

    Gfx::IntRect rect() const { return m_bounding_rect; }
    const Vector<Gfx::IntRect, 4>& rects() const { return m_rects; }
    size_t main_screen_index() const { return 0; }

    unsigned workspace_rows() const { return 0; }
    unsigned workspace_columns() const { return 0; }

    int taskbar_height() const { return 0; }

    void did_receive_screen_rects(Badge<WindowServerConnection>, const Vector<Gfx::IntRect, 4>&, size_t, unsigned, unsigned);

    template<typename F>
    void on_receive_screen_rects(F&& callback)
    {
        m_receive_rects_callbacks.append(forward<F>(callback));
    }

private:
    Vector<Gfx::IntRect, default_screen_rect_count> m_rects;
    size_t m_main_screen_index { 0 };
    Gfx::IntRect m_bounding_rect;
    unsigned m_workspace_rows { 1 };
    unsigned m_workspace_columns { 1 };
    Vector<Function<void(Desktop&)>> m_receive_rects_callbacks;
};

}
