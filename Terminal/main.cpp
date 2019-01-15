#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <Widgets/Font.h>
#include <Widgets/GraphicsBitmap.h>
#include <Widgets/Painter.h>
#include <gui.h>
#include "Terminal.h"

static void make_shell(int ptm_fd)
{
    pid_t pid = fork();
    if (pid == 0) {
        const char* tty_name = ptsname(ptm_fd);
        if (!tty_name) {
            perror("ptsname");
            exit(1);
        }
        close(ptm_fd);
        int pts_fd = open(tty_name, O_RDWR);
        dbgprintf("*** In child (%d), opening slave pty %s, pts_fd=%d\n", getpid(), tty_name, pts_fd);
        close(0);
        close(1);
        close(2);
        dup2(pts_fd, 0);
        dup2(pts_fd, 1);
        dup2(pts_fd, 2);
        close(pts_fd);
        int rc = execve("/bin/sh", nullptr, nullptr);
        if (rc < 0) {
            perror("execve");
            exit(1);
        }
        ASSERT_NOT_REACHED();
    } else {
        dbgprintf("*** In parent, child is %d\n", pid);
    }
}

int main(int, char**)
{
    int ptm_fd = open("/dev/ptm0", O_RDWR);
    if (ptm_fd < 0) {
        perror("open");
        return 1;
    }

    dbgprintf("ptm_fd = %d\n", ptm_fd);

    make_shell(ptm_fd);

    int event_fd = open("/dev/gui_events", O_RDONLY);
    if (event_fd < 0) {
        perror("open");
        return 1;
    }

    Terminal terminal;
    terminal.create_window();
    terminal.paint();

    for (;;) {
        byte buffer[1024];
        ssize_t ptm_nread = read(ptm_fd, buffer, sizeof(buffer));
        if (ptm_nread > 0) {
            for (ssize_t i = 0; i < ptm_nread; ++i) {
                terminal.on_char(buffer[i]);
            }
            terminal.paint();
        }
#if 0
        GUI_Event event;
        ssize_t nread = read(event_fd, &event, sizeof(event));
        if (nread < 0) {
            perror("read");
            return 1;
        }
        assert(nread == sizeof(event));
        dbgprintf("(Terminal:%d) ", getpid());
        switch (event.type) {
        case GUI_Event::Type::Paint: dbgprintf("WID=%x Paint [%d,%d %dx%d]\n", event.window_id, event.paint.rect.location.x, event.paint.rect.location.y, event.paint.rect.size.width, event.paint.rect.size.height); break;
        case GUI_Event::Type::MouseDown: dbgprintf("WID=%x MouseDown %d,%d\n", event.window_id, event.mouse.position.x, event.mouse.position.y); break;
        case GUI_Event::Type::MouseUp: dbgprintf("WID=%x MouseUp %d,%d\n", event.window_id, event.mouse.position.x, event.mouse.position.y); break;
        case GUI_Event::Type::MouseMove: dbgprintf("WID=%x MouseMove %d,%d\n", event.window_id, event.mouse.position.x, event.mouse.position.y); break;
        default:
            ASSERT_NOT_REACHED();
        }

        if (event.type == GUI_Event::Type::MouseDown)
            terminal.paint();
#endif
    }
    return 0;
}
