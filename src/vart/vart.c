/* Copyright (C) 2016-2017 Elias Kosunen */
/* This file is distributed under the 3-Clause BSD License */
/* See LICENSE for details */

#define VA_MANGLE(name) __va_##name

extern int VA_MANGLE(main)(void);

/* extern void VA_MANGLE(set_args)(int, char**); */

int main(int argc, char** argv)
{
    /* VA_MANGLE(set_args)(argc, argv); */
    return VA_MANGLE(main)();
}
