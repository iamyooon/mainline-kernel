// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2013 Davidlohr Bueso <davidlohr.bueso@hp.com>
 *
 *  Based on the shift-and-subtract algorithm for computing integer
 *  square root from Guy L. Steele.
 */

#include <linux/kernel.h>
#include <linux/export.h>

/**
 * int_sqrt - rough approximation to sqrt
 * @x: integer of which to calculate the sqrt
 *
 * A very rough approximation to the sqrt() function.
 */
unsigned long int_sqrt(unsigned long x)
{
	unsigned long b, m, y = 0;

	if (x <= 1)
		return x;

	// if CONFIG_64BIT, BITS_PER_LONG=64
	// 1<<62
	m = 1UL << (BITS_PER_LONG - 2);
	// 총 31번 반복함
	while (m != 0) {
		// 1st) b = 0 + 2^61
		// 2nd) b = 0 + 2^61
		b = y + m;
		// y = y/2
		y >>= 1;

		if (x >= b) {
			x -= b;
			y += m;
		}
		m >>= 2;
	}

	return y;
}
EXPORT_SYMBOL(int_sqrt);
