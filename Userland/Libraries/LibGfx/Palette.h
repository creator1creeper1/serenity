/*
 * Copyright (c) 2018-2020, Andreas Kling <kling@serenityos.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#include <AK/Forward.h>
#include <AK/Noncopyable.h>
#include <AK/RefCounted.h>
#include <AK/RefPtr.h>
#include <LibGUI/Forward.h>
#include <LibGfx/SystemTheme.h>

namespace Gfx {

class PaletteImpl : public RefCounted<PaletteImpl> {
    AK_MAKE_NONCOPYABLE(PaletteImpl);
    AK_MAKE_NONMOVABLE(PaletteImpl);

public:
    ~PaletteImpl();
    static NonnullRefPtr<PaletteImpl> create_with_anonymous_buffer(Core::AnonymousBuffer);
    NonnullRefPtr<PaletteImpl> clone() const;

    Color color(ColorRole role) const
    {
        ASSERT((int)role < (int)ColorRole::__Count);
        return Color::from_rgba(theme().color[(int)role]);
    }

    int metric(MetricRole) const;
    String path(PathRole) const;
    const SystemTheme& theme() const { return *m_theme_buffer.data<SystemTheme>(); }

    void replace_internal_buffer(Badge<GUI::Application>, Core::AnonymousBuffer buffer);

private:
    explicit PaletteImpl(Core::AnonymousBuffer);

    Core::AnonymousBuffer m_theme_buffer;
};

class Palette {

public:
    explicit Palette(const PaletteImpl&);
    ~Palette();

    Color window() const { return color(ColorRole::Window); }
    Color window_text() const { return color(ColorRole::WindowText); }
    Color selection() const { return color(ColorRole::Selection); }
    Color selection_text() const { return color(ColorRole::SelectionText); }
    Color inactive_selection() const { return color(ColorRole::InactiveSelection); }
    Color inactive_selection_text() const { return color(ColorRole::InactiveSelectionText); }
    Color desktop_background() const { return color(ColorRole::DesktopBackground); }
    Color active_window_border1() const { return color(ColorRole::ActiveWindowBorder1); }
    Color active_window_border2() const { return color(ColorRole::ActiveWindowBorder2); }
    Color active_window_title() const { return color(ColorRole::ActiveWindowTitle); }
    Color active_window_title_stripes() const { return color(ColorRole::ActiveWindowTitleStripes); }
    Color active_window_title_shadow() const { return color(ColorRole::ActiveWindowTitleShadow); }
    Color inactive_window_border1() const { return color(ColorRole::InactiveWindowBorder1); }
    Color inactive_window_border2() const { return color(ColorRole::InactiveWindowBorder2); }
    Color inactive_window_title() const { return color(ColorRole::InactiveWindowTitle); }
    Color inactive_window_title_stripes() const { return color(ColorRole::InactiveWindowTitleStripes); }
    Color inactive_window_title_shadow() const { return color(ColorRole::InactiveWindowTitleShadow); }
    Color moving_window_border1() const { return color(ColorRole::MovingWindowBorder1); }
    Color moving_window_border2() const { return color(ColorRole::MovingWindowBorder2); }
    Color moving_window_title() const { return color(ColorRole::MovingWindowTitle); }
    Color moving_window_title_stripes() const { return color(ColorRole::MovingWindowTitleStripes); }
    Color moving_window_title_shadow() const { return color(ColorRole::MovingWindowTitleShadow); }
    Color highlight_window_border1() const { return color(ColorRole::HighlightWindowBorder1); }
    Color highlight_window_border2() const { return color(ColorRole::HighlightWindowBorder2); }
    Color highlight_window_title() const { return color(ColorRole::HighlightWindowTitle); }
    Color highlight_window_title_stripes() const { return color(ColorRole::HighlightWindowTitleStripes); }
    Color highlight_window_title_shadow() const { return color(ColorRole::HighlightWindowTitleShadow); }
    Color highlight_searching() const { return color(ColorRole::HighlightSearching); }
    Color highlight_searching_text() const { return color(ColorRole::HighlightSearchingText); }
    Color menu_stripe() const { return color(ColorRole::MenuStripe); }
    Color menu_base() const { return color(ColorRole::MenuBase); }
    Color menu_base_text() const { return color(ColorRole::MenuBaseText); }
    Color menu_selection() const { return color(ColorRole::MenuSelection); }
    Color menu_selection_text() const { return color(ColorRole::MenuSelectionText); }
    Color base() const { return color(ColorRole::Base); }
    Color base_text() const { return color(ColorRole::BaseText); }
    Color button() const { return color(ColorRole::Button); }
    Color button_text() const { return color(ColorRole::ButtonText); }
    Color threed_highlight() const { return color(ColorRole::ThreedHighlight); }
    Color threed_shadow1() const { return color(ColorRole::ThreedShadow1); }
    Color threed_shadow2() const { return color(ColorRole::ThreedShadow2); }
    Color hover_highlight() const { return color(ColorRole::HoverHighlight); }
    Color rubber_band_fill() const { return color(ColorRole::RubberBandFill); }
    Color rubber_band_border() const { return color(ColorRole::RubberBandBorder); }
    Color ruler() const { return color(ColorRole::Ruler); }
    Color ruler_border() const { return color(ColorRole::RulerBorder); }
    Color ruler_active_text() const { return color(ColorRole::RulerActiveText); }
    Color ruler_inactive_text() const { return color(ColorRole::RulerInactiveText); }
    Color text_cursor() const { return color(ColorRole::TextCursor); }
    Color focus_outline() const { return color(ColorRole::FocusOutline); }

    Color link() const { return color(ColorRole::Link); }
    Color active_link() const { return color(ColorRole::ActiveLink); }
    Color visited_link() const { return color(ColorRole::VisitedLink); }

    Color syntax_comment() const { return color(ColorRole::SyntaxComment); }
    Color syntax_number() const { return color(ColorRole::SyntaxNumber); }
    Color syntax_string() const { return color(ColorRole::SyntaxString); }
    Color syntax_identifier() const { return color(ColorRole::SyntaxIdentifier); }
    Color syntax_type() const { return color(ColorRole::SyntaxType); }
    Color syntax_punctuation() const { return color(ColorRole::SyntaxPunctuation); }
    Color syntax_operator() const { return color(ColorRole::SyntaxOperator); }
    Color syntax_keyword() const { return color(ColorRole::SyntaxKeyword); }
    Color syntax_control_keyword() const { return color(ColorRole::SyntaxControlKeyword); }
    Color syntax_preprocessor_statement() const { return color(ColorRole::SyntaxPreprocessorStatement); }
    Color syntax_preprocessor_value() const { return color(ColorRole::SyntaxPreprocessorValue); }

    int window_title_height() const { return metric(MetricRole::TitleHeight); }
    int window_title_button_width() const { return metric(MetricRole::TitleButtonWidth); }
    int window_title_button_height() const { return metric(MetricRole::TitleButtonHeight); }

    String title_button_icons_path() const { return path(PathRole::TitleButtonIcons); }
    String active_window_shadow_path() const { return path(PathRole::ActiveWindowShadow); }
    String inactive_window_shadow_path() const { return path(PathRole::InactiveWindowShadow); }
    String menu_bar_shadow_path() const { return path(PathRole::MenuBarShadow); }
    String menu_shadow_path() const { return path(PathRole::MenuShadow); }
    String task_bar_shadow_path() const { return path(PathRole::TaskBarShadow); }
    String tooltip_shadow_path() const { return path(PathRole::TooltipShadow); }

    Color color(ColorRole role) const { return m_impl->color(role); }
    int metric(MetricRole role) const { return m_impl->metric(role); }
    String path(PathRole role) const { return m_impl->path(role); }

    void set_color(ColorRole, Color);
    void set_metric(MetricRole, int);
    void set_path(PathRole, String);

    const SystemTheme& theme() const { return m_impl->theme(); }

    PaletteImpl& impl() { return *m_impl; }
    const PaletteImpl& impl() const { return *m_impl; }

private:
    NonnullRefPtr<PaletteImpl> m_impl;
};

}

using Gfx::Palette;
