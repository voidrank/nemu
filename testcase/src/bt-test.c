#include "trap.h"

void c() {
    set_bp();
}

void b() {
    c();
}

void a() {
    b();
}


int main() {
    a();
    return 0;
}
