/*
 * Copyright (c) 2022, Filiph Sandström <filiph.sandstrom@filfatstudios.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/ByteBuffer.h>
#include <AK/Function.h>
#include <AK/HashMap.h>
#include <AK/String.h>
#include <LibGUI/Forward.h>
#include <LibGfx/Forward.h>

namespace GUI {

class ClipboardServerConnection;

class Clipboard {
public:
    static void initialize(Badge<Application>) { }

private:
    Clipboard() = default;
};

}
