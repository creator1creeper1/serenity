/*
 * Copyright (c) 2020, Andreas Kling <kling@serenityos.org>
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

namespace Web {

class CanvasRenderingContext2D;
class Document;
class Element;
class Event;
class EventListener;
class EventTarget;
class Frame;
class HTMLBodyElement;
class HTMLCanvasElement;
class HTMLDocumentParser;
class HTMLElement;
class HTMLFormElement;
class HTMLHeadElement;
class HTMLHtmlElement;
class HTMLImageElement;
class HTMLScriptElement;
class PageView;
class ImageData;
class LayoutDocument;
class LayoutNode;
class LoadRequest;
class MouseEvent;
class Node;
class Origin;
class Resource;
class ResourceLoader;
class Selector;
class StyleResolver;
class StyleRule;
class StyleSheet;
class Text;
class Window;
class XMLHttpRequest;

namespace Bindings {

class CanvasRenderingContext2DWrapper;
class DocumentWrapper;
class ElementWrapper;
class EventWrapper;
class EventListenerWrapper;
class EventTargetWrapper;
class HTMLCanvasElementWrapper;
class HTMLImageElementWrapper;
class ImageDataWrapper;
class LocationObject;
class MouseEventWrapper;
class NodeWrapper;
class WindowObject;
class Wrappable;
class Wrapper;
class XMLHttpRequestConstructor;
class XMLHttpRequestPrototype;
class XMLHttpRequestWrapper;

}

}
