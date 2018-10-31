/*
 * strx.h
 */
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void strx_lower_case(char * str)
{
   for (; *str; str++)
       *str = tolower(*str);
}

void strx_upper_case(char * str)
{
   for (; *str; str++)
       *str = toupper(*str);
}

void strx_replace_char(char * str, char find, char replace)
{
    for (; *str; str++)
        if (*str == find)
            *str = replace;
}

/*
 * Replace all occurrences of 'o' with 'r' in 'str'. We restrict the new size
 * to 'n'.
 *
 * Returns 0 if successful or -1 if the replaced string does not fit. In this
 * case the original string is untouched. The new string is terminated.
 */
int strx_replace_str(char * str, char * o, char * r, size_t n)
{
    char buffer[n];
    char * pos, * s;
    size_t l, size = 0, olen = strlen(o), rlen = strlen(r);

    for (s = str; (pos = strstr(s, o)) != NULL; s = pos + olen)
    {
        l = pos - s;

        if (size + l + rlen >= n)
        {
            return -1;
        }

        memcpy(buffer + size, s, l);
        size += l;

        memcpy(buffer + size, r, rlen);
        size += rlen;

    }

    if (s != str)
    {
        memcpy(str, buffer, size);
        str[size] = '\0';
    }

    return 0;
}

/*
 * Split and then join a given string.
 *
 * For example:
 *      string: "  this  is a   test  "
 *      split: ' ' and join with '_'
 *      result: "this_is_a_test"
 */
void strx_split_join(char * pt, char split_chr, char join_chr)
{
    int join = -1;
    char * dest = pt;

    for (; *pt != '\0'; pt++)
    {
        if (*pt != split_chr)
        {
            if (join > 0)
            {
                *dest = join_chr;
                dest++;
            }
            join = 0;
            *dest = *pt;
            dest++;
        }
        else if (!join)
        {
            join = 1;
        }
    }

    *dest = '\0';
}

void strx_trim(char ** str, char chr)
{
    /*
     * trim: when chr is 0 we will trim whitespace,
     * otherwise only the given char.
     */
    char * end;

    // trim leading chars
    while ((chr && **str == chr) || (!chr && isspace(**str)))
    {
        (*str)++;
    }

    // check all chars?
    if(**str == 0)
    {
        return;
    }

    // trim trailing chars
    end = *str + strlen(*str) - 1;
    while (end > *str && ((chr && *end == chr) || (!chr && isspace(*end))))
    {
        end--;
    }

    // write new null terminator
    *(end + 1) = 0;
}

/*
 * returns true or false
 */
_Bool strx_is_empty(const char * str)
{
    for (; *str; str++)
        if (!isspace(*str))
            return false;
    return true;
}

_Bool strx_is_int(const char * str)
{
   // Handle signed numbers.
   if (*str == '-' || *str == '+')
       ++str;

   // Handle empty string or only signed.
   if (!*str)
       return false;

   // Check for non-digit chars in the rest of the string.
   for (; *str; ++str)
       if (!isdigit(*str))
           return false;

   return true;
}

_Bool strx_is_float(const char * str)
{
   size_t dots = 0;

   // Handle signed float numbers.
   if (*str == '-' || *str == '+')
       ++str;

   // Handle empty string or only signed.
   if (!*str)
       return false;

   // Check for non-digit chars in the rest of the string.
   for (; *str; ++str)
       if (*str == '.')
           ++dots;
       else if (!isdigit(*str))
           return false;

   return dots == 1;
}

_Bool strx_is_graph(const char * str)
{
    for (; *str; str++)
        if (!isgraph(*str))
            return false;
    return true;
}

_Bool strx_is_graphn(const char * str, size_t n)
{
    for (; n--; str++)
        if (!isgraph(*str))
            return false;
    return true;
}

/*
 * Requires a match with regular expression: [-+]?[0-9]*\.?[0-9]+
 */
double strx_to_doublen(const char * str, size_t n)
{
    assert (n);
    double d = 0;
    double convert;
    uint64_t r1 = 0;

    switch (*str)
    {
    case '-':
        assert (n > 1);
        convert = -1.0;
        ++str;
        --n;
        break;
    case '+':
        assert (n > 1);
        convert = 1.0;
        ++str;
        --n;
        break;
    default:
        convert = 1.0;
    }

    for (; n && isdigit(*str); --n, ++str)
        r1 = 10 * r1 + *str - '0';

    d = (double) r1;

    if (n && --n)
    {
        uint64_t r2;
        double power;
        ++str;
        r2 = *str - '0';
        for (power = -1.0f; --n && isdigit(*(++str)); power--)
             r2 = 10 * r2 + *str - '0';

        d += pow(10.0f, power) * (double) r2;
    }

    return convert * d;
}

/*
 * Requires a match with regular expression:   [-+]?[0-9]+
 */
int64_t strx_to_int64n(const char * str, size_t n)
{
    assert (n);
    int64_t i;

    switch (*str)
    {
    case '-':
        assert (n > 1);
        i = -(*(++str) - '0');
        break;
    case '+':
        assert (n > 1);
        i = *(++str) - '0';
        break;
    default:
        i = *str - '0';
    }

    while (--n)
    {
        i = 10 * i + *(++str) - '0';
        assert (isdigit(*str));
    }

    return i;
}

char * strx_cat(const char * s1, const char * s2)
{
    size_t n1 = strlen(s1);
    size_t n2 = strlen(s2);

    char * s = (char *) malloc(n1 + n2 + 1);
    if (!s)
        return NULL;

    memcpy(s, s1, n1);
    memcpy(s + n1, s2, n2 + 1);

    return s;
}
