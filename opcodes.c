typedef enum {
// r
    add = 0,
    sub = 1,
    mul = 2,
    div = 3,
    cmp_equal = 4,
    cmp_neq = 5,
    load = 15,
    store = 16,
    syscall = 63
} OpcodeI;

typedef enum {
    jump = 0,
    jump_cond = 1,
    mov = 3
} OpcodeR;