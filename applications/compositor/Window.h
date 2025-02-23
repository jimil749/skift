#pragma once

#include <libgraphic/Bitmap.h>
#include <libsystem/algebra/Rect.h>
#include <libwidget/Cursor.h>
#include <libwidget/Event.h>

#include "compositor/Protocol.h"

struct Client;

struct Window
{
private:
    int _id;
    WindowFlag _flags;
    WindowType _type;

    struct Client *_client;
    Rectangle _bound;
    CursorState _cursor_state{};

    RefPtr<Bitmap> _frontbuffer;
    RefPtr<Bitmap> _backbuffer;

public:
    int id() { return _id; }
    WindowFlag flags() { return _flags; };
    WindowType type() { return _type; };

    Client *client() { return _client; }

    CursorState cursor_state() { return _cursor_state; }

    void cursor_state(CursorState cursor_state) { _cursor_state = cursor_state; }

    Bitmap &frontbuffer()
    {
        assert(_frontbuffer);
        return *_frontbuffer;
    }

    Window(
        int id,
        WindowFlag flags,
        WindowType type,
        struct Client *client,
        Rectangle bound,
        RefPtr<Bitmap> frontbuffer,
        RefPtr<Bitmap> backbuffer);

    ~Window();

    Rectangle bound();

    Rectangle cursor_capture_bound();

    void move(Vec2i new_position);

    void resize(Rectangle new_bound);

    void send_event(Event event);

    void handle_mouse_move(Vec2i old_position, Vec2i position, MouseButton buttons);

    void handle_mouse_buttons(MouseButton old_buttons, MouseButton buttons, Vec2i position);

    void handle_double_click(Vec2i position);

    void get_focus();

    void lost_focus();

    void flip_buffers(int frontbuffer_handle, Vec2i frontbuffer_size, int backbuffer_handle, Vec2i backbuffer_size, Rectangle region);
};
