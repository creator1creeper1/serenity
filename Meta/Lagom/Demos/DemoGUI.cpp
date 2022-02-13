/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <LibCore/System.h>
#include <LibGUI/AboutDialog.h>
#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/Label.h>
#include <LibGUI/MessageBox.h>
#include <LibGUI/Window.h>
#include <LibGfx/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <LibGfx/SystemTheme.h>
#include <LibMain/Main.h>

ErrorOr<int> serenity_main(Main::Arguments arguments)
{
    // Set default font path.
    // TODO: figure out a better way to do this :^)
    Gfx::FontDatabase::set_default_fonts_lookup_path("../../Base/res/fonts");
    // Set the default font, this is required here since we're not running in a Serenity env
    Gfx::FontDatabase::set_default_font_query("Katica 10 400 0"sv);
    // Ditto but for the theme and palette
    Gfx::set_system_theme(Gfx::load_system_theme("../../Base/res/themes/Default.ini"));

    auto app = TRY(GUI::Application::try_create(arguments));
    app->set_system_palette(Gfx::current_system_theme_buffer());

    auto window = TRY(GUI::Window::try_create());
    window->set_title("Hello LibGUI World");
    window->resize(600, 400);

    auto widget = TRY(window->try_set_main_widget<GUI::Widget>());
    widget->set_fill_with_background_color(true);

    auto layout = TRY(widget->try_set_layout<GUI::VerticalBoxLayout>());
    layout->set_margins(16);

    auto label = TRY(widget->try_add<GUI::Label>("Hello World :^)"));

    auto first_button = TRY(widget->try_add<GUI::Button>("A very cool button"));
    first_button->on_click = [&](auto) {
        GUI::MessageBox::show(window, "Hello friends!", ":^)");
    };

    auto second_button = TRY(widget->try_add<GUI::Button>("This button is disabled :^("));
    second_button->set_enabled(false);

    auto third_button = TRY(widget->try_add<GUI::Button>("About"));
    third_button->on_click = [&](auto) {
        GUI::AboutDialog::show("SerenityOS", nullptr, nullptr, nullptr, Core::Version::read_long_version_string());
    };

    window->show();
    return app->exec();
}
