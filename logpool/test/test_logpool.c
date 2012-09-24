#include "../logpool.h"
#ifdef __cplusplus
extern "C" {
#endif

static const char *param_string = ""
"{"
"  \"host\" : \"localhost\","
"  \"port\" : 14801"
"}";

int main(int argc, char const* argv[])
{
    JSON param = JSON_parse(param_string);
    logpool_t *lp = logpool_open(param);
    logpool_close(lp);
    return 0;
}

#ifdef __cplusplus
} /* extern "C" */
#endif
