/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>

#include <stdio.h>
#include "duck.h"

int
main( void )
{
    printk("|__Creating duck objects:\n");

    Duck_t * Huey = duckCreate();
    Duck_t * Dewey = duckCreate();
    Duck_t * Louie = duckCreate();

    duckInit(Huey, "Huey");
    duckInit(Dewey, "Dewey");
    duckInit(Louie, "Louie");

    printk("|__Showing duck objects:\n");
    
    duckShow(Huey);
    duckShow(Dewey);
    duckShow(Louie);

    printk("|__Printing duck objects by directly accessing their names:\n");
    
    printk("\tAccessing name directly for: %s\n", Huey->name);
    printk("\tAccessing name directly for: %s\n", Dewey->name);
    printk("\tAccessing name directly for: %s\n", Louie->name);

    printk("|__Destroying duck objects:\n");

    duckDestroy(Huey);
    duckDestroy(Dewey);
    duckDestroy(Louie);

    return 0;
}
