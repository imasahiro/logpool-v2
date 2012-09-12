#include "../mold.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

extern struct format_api FILE_API;

int main(int argc, char const* argv[])
{
    molder_param_t param = {1024, 8, {"test.log"}};
    molder_t *mold = molder_open(&FILE_API, &param);
    long i;
    for (i = 0; i < 10; i++) {
        molder_record(mold, NULL, 1, "TEST", LOG_str, "a", "b", LOG_int, "c", i, LOG_END);
    }
    molder_close(mold);
    FILE *fp = fopen("test.log", "r");
    char buffer[128];
    assert(fp);
    for (i = 0; i < 10; i++) {
        assert(fgets(buffer, 128, fp));
        char text[128] = {};
        sprintf(text, "TraceIDTEST,a'b',c%ld", i);
        assert(strncmp(buffer, text, strlen(text)) == 0);
    }
    return 0;
}
