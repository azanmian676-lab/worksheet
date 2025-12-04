#include "../drivers/screen.h"
#include "../drivers/key.h"
#include "../drivers/term.h"

int three_add(int x, int y, int z) {
    return x + y + z;
}

int mult(int a, int b) {
    return a * b;
}

int calc_fact(int num) {
    if (num <= 1) return 1;
    int ans = 1;
    for (int k = 2; k <= num; k++) {
        ans *= k;
    }
    return ans;
}

void core_main(void) {
    screen_wipe();
    
    screen_color(SCR_GREEN, SCR_BLACK);
    screen_text("========================================\n");
    screen_text("       SimpleOS v1.0 - Loading         \n");
    screen_text("========================================\n\n");
    
    screen_color(SCR_WHITE, SCR_BLACK);
    
    screen_text("Testing C functions from assembly:\n");
    screen_text("----------------------------------\n");
    
    screen_text("three_add(10, 20, 30) = ");
    screen_number(three_add(10, 20, 30));
    screen_text("\n");
    
    screen_text("mult(6, 7) = ");
    screen_number(mult(6, 7));
    screen_text("\n");
    
    screen_text("calc_fact(5) = ");
    screen_number(calc_fact(5));
    screen_text("\n\n");
    
    screen_color(SCR_CYAN, SCR_BLACK);
    screen_text("Initializing keyboard...\n");
    screen_color(SCR_WHITE, SCR_BLACK);
    
    key_init();
    
    screen_color(SCR_GREEN, SCR_BLACK);
    screen_text("\nSystem ready. Starting terminal...\n\n");
    screen_color(SCR_WHITE, SCR_BLACK);
    
    term_start();
}

