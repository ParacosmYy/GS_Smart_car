#include <stdio.h>
#include "utils.h"

static int expect_equal_int16(const char *name, int16_t actual, int16_t expected)
{
    if (actual != expected)
    {
        (void)printf("%s: expected %d, got %d\n", name, (int)expected, (int)actual);
        return 1;
    }
    return 0;
}

int main(void)
{
    int failures = 0;

    failures += expect_equal_int16("positive", my_abs(7), 7);
    failures += expect_equal_int16("zero", my_abs(0), 0);
    failures += expect_equal_int16("negative", my_abs(-7), 7);

    return failures;
}
