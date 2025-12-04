#include "term.h"
#include "screen.h"
#include "key.h"

static int match(const char* a, const char* b) {
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static int prefix_match(const char* str, const char* pre) {
    while (*pre) {
        if (*str != *pre) return 0;
        str++;
        pre++;
    }
    return 1;
}

static const char* skip_ws(const char* s) {
    while (*s == ' ') s++;
    return s;
}

static void cmd_show(void) {
    screen_color(SCR_CYAN, SCR_BLACK);
    screen_text("\nAvailable commands:\n");
    screen_color(SCR_WHITE, SCR_BLACK);
    screen_text("  show     - Show this help message\n");
    screen_text("  erase    - Clear the screen\n");
    screen_text("  output   - Display the provided text\n");
    screen_text("  about    - Display OS version\n");
    screen_text("  exit     - Prepare system for shutdown\n");
    screen_text("  tint     - Change text color (0-15)\n");
    screen_text("  check    - Run test functions\n");
    screen_text("\n");
}

static void cmd_about(void) {
    screen_color(SCR_GREEN, SCR_BLACK);
    screen_text("\nSimpleOS v1.0\n");
    screen_color(SCR_LIGHT_GREY, SCR_BLACK);
    screen_text("A simple operating system for learning\n");
    screen_text("Developed for OS Development Course\n\n");
    screen_color(SCR_WHITE, SCR_BLACK);
}

static void cmd_erase(void) {
    screen_wipe();
}

static void cmd_output(const char* arg) {
    arg = skip_ws(arg);
    screen_text(arg);
    screen_text("\n");
}

static void cmd_exit(void) {
    screen_color(SCR_YELLOW, SCR_BLACK);
    screen_text("\nPreparing system for shutdown...\n");
    screen_text("It is now safe to turn off your computer.\n");
    screen_color(SCR_WHITE, SCR_BLACK);
    
    while(1) {
        __asm__ __volatile__("cli; hlt");
    }
}

static void cmd_tint(const char* arg) {
    arg = skip_ws(arg);
    if (*arg >= '0' && *arg <= '9') {
        int color = *arg - '0';
        if (*(arg+1) >= '0' && *(arg+1) <= '5') {
            color = color * 10 + (*(arg+1) - '0');
        }
        if (color >= 0 && color <= 15) {
            screen_color(color, SCR_BLACK);
            screen_text("Color changed.\n");
            return;
        }
    }
    screen_text("Usage: tint <0-15>\n");
}

extern int three_add(int a, int b, int c);
extern int mult(int a, int b);
extern int calc_fact(int n);

static void cmd_check(void) {
    screen_color(SCR_CYAN, SCR_BLACK);
    screen_text("\nRunning C function tests:\n");
    screen_color(SCR_WHITE, SCR_BLACK);
    
    screen_text("three_add(5, 10, 15) = ");
    screen_number(three_add(5, 10, 15));
    screen_text("\n");
    
    screen_text("mult(8, 9) = ");
    screen_number(mult(8, 9));
    screen_text("\n");
    
    screen_text("calc_fact(6) = ");
    screen_number(calc_fact(6));
    screen_text("\n\n");
}

static void run_cmd(const char* cmd) {
    cmd = skip_ws(cmd);
    
    if (*cmd == '\0') {
        return;
    } else if (match(cmd, "show")) {
        cmd_show();
    } else if (match(cmd, "erase")) {
        cmd_erase();
    } else if (match(cmd, "about")) {
        cmd_about();
    } else if (match(cmd, "exit")) {
        cmd_exit();
    } else if (match(cmd, "check")) {
        cmd_check();
    } else if (prefix_match(cmd, "output ")) {
        cmd_output(cmd + 7);
    } else if (prefix_match(cmd, "tint ")) {
        cmd_tint(cmd + 5);
    } else if (match(cmd, "output")) {
        screen_text("\n");
    } else {
        screen_color(SCR_RED, SCR_BLACK);
        screen_text("Unknown command: ");
        screen_text(cmd);
        screen_text("\nType 'show' for available commands.\n");
        screen_color(SCR_WHITE, SCR_BLACK);
    }
}

void term_start(void) {
    char line_buf[LINE_MAX];
    
    while (1) {
        screen_color(SCR_GREEN, SCR_BLACK);
        screen_text("simpleos");
        screen_color(SCR_WHITE, SCR_BLACK);
        screen_text("> ");
        
        key_line(line_buf, LINE_MAX);
        
        run_cmd(line_buf);
    }
}

