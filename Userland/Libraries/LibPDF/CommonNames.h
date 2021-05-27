/*
 * Copyright (c) 2021, Matthew Olsson <mattco@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <AK/FlyString.h>

#define ENUMERATE_COMMON_NAMES(V) \
    V(ASCII85Decode)              \
    V(ASCIIHexDecode)             \
    V(BaseFont)                   \
    V(BlackPoint)                 \
    V(C)                          \
    V(CalRGB)                     \
    V(CCITTFaxDecode)             \
    V(ColorSpace)                 \
    V(Contents)                   \
    V(Count)                      \
    V(CropBox)                    \
    V(Crypt)                      \
    V(DCTDecode)                  \
    V(Dest)                       \
    V(DeviceCMYK)                 \
    V(DeviceGray)                 \
    V(DeviceRGB)                  \
    V(E)                          \
    V(F)                          \
    V(Filter)                     \
    V(First)                      \
    V(Fit)                        \
    V(FitB)                       \
    V(FitBH)                      \
    V(FitBV)                      \
    V(FitH)                       \
    V(FitR)                       \
    V(FitV)                       \
    V(FlateDecode)                \
    V(Font)                       \
    V(Gamma)                      \
    V(H)                          \
    V(JBIG2Decode)                \
    V(JPXDecode)                  \
    V(Kids)                       \
    V(L)                          \
    V(LZWDecode)                  \
    V(Last)                       \
    V(Length)                     \
    V(Linearized)                 \
    V(Matrix)                     \
    V(MediaBox)                   \
    V(N)                          \
    V(Next)                       \
    V(O)                          \
    V(Outlines)                   \
    V(P)                          \
    V(Pages)                      \
    V(Parent)                     \
    V(Pattern)                    \
    V(Prev)                       \
    V(Resources)                  \
    V(Root)                       \
    V(Rotate)                     \
    V(RunLengthDecode)            \
    V(T)                          \
    V(Title)                      \
    V(Type)                       \
    V(UserUnit)                   \
    V(WhitePoint)                 \
    V(XYZ)

namespace PDF {

class CommonNames {
public:
#define ENUMERATE(name) static FlyString name;
    ENUMERATE_COMMON_NAMES(ENUMERATE)
#undef ENUMERATE
};

}
