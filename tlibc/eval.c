#include "std.h"

static long eval(const char *expr);
static bool contains(const char *fmt, char tar);

int main(int argc, char **argv) {
    static struct buffered_reader br;
    static char buf[2048];
    br.start = br.end = 0;

    sys_write(1, "(eval) ", 7);
    while (fdgets(&br, buf, 0)) {
        Printf("%l\n", eval(buf));
        if (*(char *)buf == 'q') {
            break;
        }
        sys_write(1, "(eval) ", 7);
    }
    return 0;
}

static bool contains(const char *fmt, char tar) {
    if (tar == 0) {
        return true;
    }

    for (; *fmt; fmt++) {
        if (*fmt == tar) {
            return true;
        }
    }

    return false;
}

// operators
enum oper {
    OP_NULL = 0,
    OP_PLUS,
    OP_SUB,
    OP_MULT,
    OP_DIV
};

static void eval_simple(long *left, long *right, enum oper op) {
    switch (op) {
    case (OP_NULL): {
        *left = *right;
        *right = 0;
        break;
    }
    case (OP_PLUS): {
        *left += *right;
        *right = 0;
        break;
    }
    case (OP_SUB): {
        *left -= *right;
        *right = 0;
        break;
    }
    case (OP_MULT): {
        *left *= *right;
        *right = 0;
        break;
    }
    case (OP_DIV): {
        *left /= *right;
        *right = 0;
        break;
    }
    }
}

static const char *eval_recur(const char *expr, long *res) {
    long ret = 0;
    long cur = 0;
    enum oper op = OP_NULL;

    for (; *expr != 0; expr++) {
        switch (*expr) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {
                cur *= 10;
                cur += (long) (*expr - '0');
                break;
            }

            // brackets
            case ')': {
                eval_simple(&ret, &cur, op);
                goto end;
                break;
            }
            case '(': {
                // set cur to the value in the bracket
                expr = eval_recur(expr + 1, &cur);
                eval_simple(&ret, &cur, op);
                break;
            }

            // operators
            case '+': {
                eval_simple(&ret, &cur, op);
                op = OP_PLUS;
                break;
            }
            case '-': {
                eval_simple(&ret, &cur, op);
                op = OP_SUB;
                break;
            }
            case '*': {
                eval_simple(&ret, &cur, op);
                op = OP_MULT;
                break;
            }
            case '/': {
                eval_simple(&ret, &cur, op);
                op = OP_DIV;
                break;
            }

            default: {
                eval_simple(&ret, &cur, op);
                break;
            }
        }
    }

end:
    *res = ret;
    return *expr == 0 ? expr : (expr + 1);
}

static long eval(const char *expr) {
    long ret;
    eval_recur(expr, &ret);
    return ret;
}
