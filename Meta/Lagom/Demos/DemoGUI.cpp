/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <AK/ByteReader.h>
#include <LibGUI/Application.h>
#include <LibGUI/BoxLayout.h>
#include <LibGUI/Button.h>
#include <LibGUI/Painter.h>
#include <LibGUI/Window.h>
#include <LibGfx/FontDatabase.h>
#include <LibGfx/Palette.h>
#include <LibGfx/SystemTheme.h>
#include <LibMain/Main.h>
#include <LibWasm/AbstractMachine/Configuration.h>
#include <LibWasm/Types.h>

RefPtr<GUI::Application> app;
RefPtr<GUI::Window> window;

class Wasm4Widget final : public GUI::Widget {
    C_OBJECT(Wasm4Widget);

public:
    void mousemove_event(GUI::MouseEvent& event) override
    {
        if (!main_memory)
            return;

        i16 x = event.x() * 160 / window()->width(), y = event.y() * 160 / window()->height();
        memcpy(main_memory->data().offset_pointer(0x1a), &x, 2);
        memcpy(main_memory->data().offset_pointer(0x1c), &y, 2);
    }

    void mousedown_event(GUI::MouseEvent& event) override
    {
        if (!main_memory)
            return;

        u8 byte = (u8)((event.buttons() & GUI::MouseButton::Primary) != 0);
        *main_memory->data().offset_pointer(0x1e) = byte;
    }

    void mouseup_event(GUI::MouseEvent& event) override
    {
        if (!main_memory)
            return;

        u8 byte = (u8)((event.buttons() & GUI::MouseButton::Primary) == 0);
        *main_memory->data().offset_pointer(0x1e) = byte;
    }

    void set_memory(Wasm::MemoryInstance* ptr) { main_memory = ptr; }

private:
    Wasm::MemoryInstance* main_memory { nullptr };
};

RefPtr<Wasm4Widget> widget;

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

    window = MUST(GUI::Window::try_create());
    window->set_title("Hello LibGUI World");
    window->resize(160 * 4, 160 * 4);

    widget = MUST(window->try_set_main_widget<Wasm4Widget>());

    window->show();
}

__attribute__((destructor)) void dtor()
{
    app = nullptr;
}

static Array<Color, 4> palette(Wasm::MemoryInstance& memory)
{
    widget->set_memory(&memory);
    auto palette_data = memory.data().span().slice(4, 16);
    Array<u32, 4> values;
    values[0] = ByteReader::load32(palette_data.offset_pointer(0));
    values[1] = ByteReader::load32(palette_data.offset_pointer(4));
    values[2] = ByteReader::load32(palette_data.offset_pointer(8));
    values[3] = ByteReader::load32(palette_data.offset_pointer(12));

    Array<Color, 4> colors;
    colors[0] = Color::from_rgba(values[0]).with_alpha(255);
    colors[1] = Color::from_rgba(values[1]).with_alpha(255);
    colors[2] = Color::from_rgba(values[2]).with_alpha(255);
    colors[3] = Color::from_rgba(values[3]).with_alpha(255);

    return colors;
}

static Array<Color, 4> draw_colors(Wasm::MemoryInstance& memory)
{
    auto p = palette(memory);
    auto value = ByteReader::load16(memory.data().span().slice(0x14, 2).data());
    Array<Color, 4> colors;
    colors[0] = p[(value >> 0) & 0b1111];
    colors[1] = p[(value >> 4) & 0b1111];
    colors[2] = p[(value >> 8) & 0b1111];
    colors[3] = p[(value >> 12) & 0b1111];

    return colors;
}

Wasm::Result rect(Wasm::Configuration& configuration, size_t, Wasm::Value* values)
{

    auto bitmap = window->back_bitmap();
    if (bitmap) {
        auto x = values[0].to<i32>().value();
        auto y = values[1].to<i32>().value();
        auto w = values[2].to<i32>().value();
        auto h = values[3].to<i32>().value();
        auto colors = draw_colors(*configuration.store().get(Wasm::MemoryAddress(0)));
        GUI::Painter painter(*bitmap);
        painter.fill_rect({ x, y, w, h }, colors[1]);
        painter.draw_rect({ x, y, w, h }, colors[2]);
    }

    return Wasm::Result { Vector<Wasm::Value> {} };
}

Wasm::Result blit(Wasm::Configuration& configuration, size_t, Wasm::Value* values)
{
    auto bitmap = window->back_bitmap();
    if (bitmap) {
        auto sprite_ptr = values[0].to<i32>().value();
        auto x = values[1].to<i32>().value();
        auto y = values[2].to<i32>().value();
        auto w = values[3].to<i32>().value();
        auto h = values[4].to<i32>().value();
        auto flags = values[5].to<i32>().value();
        auto rect = Gfx::IntRect { x, y, w, h };

        dbgln("blit {} from {} flags={}", rect, sprite_ptr, flags);

        auto memory = configuration.store().get(Wasm::MemoryAddress(0));
        GUI::Painter painter(*bitmap);
        Gfx::AffineTransform transform;
        transform.set_scale(window->width() / 160, window->height() / 160);

        RefPtr<Gfx::Bitmap> src_bitmap = MUST(Gfx::Bitmap::try_create(Gfx::BitmapFormat::BGRA8888, { w, h }));
        auto colors = palette(*memory);
        auto slice = memory->data().span().offset_pointer(sprite_ptr);
        auto revert_u8 = [](u8 n){ return ((n & 0x1) << 7) | ((n & 0x2) << 5) | ((n & 0x4) << 3) | ((n & 0x8) << 1)
                                        | ((n & 0x10) >> 1) | ((n & 0x20) >> 3) | ((n & 0x40) >> 5) | ((n & 0x80) >> 7); };
        u8* new_data = (u8*)malloc(w * h * 2);
        for (int i = 0; i < w * h * 2; ++i)
            new_data[i] = revert_u8(slice[i]);
        AK::BitmapView bslice { new_data, (size_t)(w * h * (flags & 1 ? 2 : 1)) };
        for (int i = 0; i < w * h; ++i) {
            auto y_ = i / w;
            auto x_ = i % w;
            u8 index;
            if (flags & 1)
                index = ((u8)bslice.get(2 * i) << 1) | (u8)bslice.get(2 * i + 1);
            else
                index = bslice.get(i);
            auto color = colors[index];
            src_bitmap->set_pixel(x_, y_, color);
        }
        painter.draw_scaled_bitmap(transform.map(rect), *src_bitmap, src_bitmap->rect());
        free(new_data);
    }

    return Wasm::Result { Vector<Wasm::Value> {} };
}
}
