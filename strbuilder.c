#include "strbuilder.h"

#include <assert.h>
#include <stdbool.h>
#include <string.h>

#define CASE_RETURN_ENUM_STR(val) case val: return #val
#define EMPTY_DEFAULT_SWITCH_CASE() default: assert(0)

struct StrBuilder
{
    size_t size;
    size_t len;
    char *str;
};

static bool strbuilder_reallocate_str(StrBuilder *sb, size_t newSize)
{
    char *tmp = realloc(sb->str, sizeof(char) * newSize);
    if (tmp != NULL) {
        sb->size = newSize;
        sb->str = tmp;
        if (sb->len > newSize) {
            sb->len = newSize;
        }

        return true;
    }

    return false;
}

static bool strbuilder_grow_str(StrBuilder *sb, size_t requiredSize)
{
    if (requiredSize > sb->size) {
        size_t newSize = sb->size * 2;
        if (requiredSize > newSize) {
            newSize = requiredSize;
        }

        return strbuilder_reallocate_str(sb, newSize);
    }

    // The operation succeeded as there was no need to reallocate
    return true;
}

const char *strbuilder_get_error_str(StrBuilderErr err)
{
    switch (err) {
        CASE_RETURN_ENUM_STR(STRBUILDER_SUCCESS);
        CASE_RETURN_ENUM_STR(STRBUILDER_ERROR);
        CASE_RETURN_ENUM_STR(STRBUILDER_NOT_IMPLEMENTED);
        CASE_RETURN_ENUM_STR(STRBUILDER_MEM_ALLOC_FAILED);
        CASE_RETURN_ENUM_STR(STRBUILDER_INDEX_OUT_OF_BOUNDS);
        EMPTY_DEFAULT_SWITCH_CASE();
    }
}

StrBuilderErr strbuilder_create(StrBuilder **result)
{
    return strbuilder_create_sz(result, STRBUILDER_DEFAULT_SIZE);
}

StrBuilderErr strbuilder_create_sz(StrBuilder **result, size_t size)
{
    StrBuilder *sb = malloc(sizeof(StrBuilder));
    if (sb == NULL) {
        *result = NULL;
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    sb->str = calloc(size, sizeof(char));
    if (sb->str == NULL) {
        free(sb);
        *result = NULL;
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    // Everything is OK!
    sb->size = size;
    sb->len = 0;
    *result = sb;
    return STRBUILDER_SUCCESS;
}

void strbuilder_free(StrBuilder *sb)
{
    if (sb != NULL) {
        free(sb->str);
        free(sb);
    }
}

StrBuilderErr strbuilder_append(StrBuilder *sb, const StrBuilder *other)
{
    return strbuilder_append_str(sb, other->str, other->len);
}

StrBuilderErr strbuilder_append_c(StrBuilder *sb, char c)
{
    return strbuilder_append_str(sb, &c, 1);
}

StrBuilderErr strbuilder_append_str(StrBuilder *sb, const char *str, size_t len)
{
    size_t newLen = sb->len + len;
    if (!strbuilder_grow_str(sb, newLen)) {
        return STRBUILDER_MEM_ALLOC_FAILED;
    }

    char *dst = sb->str + sb->len;
    sb->len = newLen;
    memcpy(dst, str, len);
    return STRBUILDER_SUCCESS;
}
