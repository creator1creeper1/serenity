/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/Window.h>
#include <LibGfx/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <LibGfx/SystemTheme.h>
#include <LibMain/Main.h>
#include <LibWasm/Types.h>

RefPtr<GUI::Application> app;

extern "C" {

__attribute__((constructor)) void ctor()
{
    char* argv[] {
        strdup("magic"),
        nullptr,
    };
    Main::Arguments arguments {
        1,
        argv,
        {},
    };

    // Set default font path.
    // TODO: figure out a better way to do this :^)
    Gfx::FontDatabase::set_default_fonts_lookup_path("../../Base/res/fonts");
    // Set the default font, this is required here since we're not running in a Serenity env
    Gfx::FontDatabase::set_default_font_query("Katica 10 400 0"sv);
    // Ditto but for the theme and palette
    Gfx::set_system_theme(Gfx::load_system_theme("../../Base/res/themes/Default.ini"));

    app = MUST(GUI::Application::try_create(arguments));
    app->set_system_palette(Gfx::current_system_theme_buffer());

    auto window = MUST(GUI::Window::try_create());
    window->set_title("Hello LibGUI World");
    window->resize(600, 400);

    auto widget = MUST(window->try_set_main_widget<GUI::Widget>());
    window->show();
}

__attribute__((destructor)) void dtor()
{
    app = nullptr;
}

Wasm::Value
}
