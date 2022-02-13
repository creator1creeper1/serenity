/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/Singleton.h>
#include <LibGUI/WindowManagerServerConnection.h>

namespace GUI {

static Singleton<WindowManagerServerConnection> s_the;
WindowManagerServerConnection& WindowManagerServerConnection::the()
{
    return *s_the;
}

WindowManagerServerConnection::WindowManagerServerConnection()
{
}

void WindowManagerServerConnection::async_set_event_mask(i32)
{
    // NOP
}

void WindowManagerServerConnection::async_set_manager_window(i32)
{
    // NOP
}

}
