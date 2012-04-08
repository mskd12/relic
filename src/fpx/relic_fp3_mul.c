/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2012 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of multiplication in a cubic extension of a prime field.
 *
 * @version $Id$
 * @ingroup fpx
 */

#include "relic_core.h"
#include "relic_conf.h"
#include "relic_fp.h"
#include "relic_fp_low.h"
#include "relic_pp_low.h"
#include "relic_pp.h"
#include "relic_util.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

/**
 * Constant used to compute consecutive Frobenius maps in cubic extension.
 */
static fp_st const_base[2];

/**
 * Constant used to compute the Frobenius map in higher extensions.
 */
static fp_st const_frb[5];

/**
 * Constant used to compute consecutive Frobenius maps in higher extensions.
 */
static fp_st const_sqr[5];

/**
 * Constant used to compute consecutive Frobenius maps in higher extensions.
 */
static fp_st const_cub[5];

/**
 * Constant used to compute consecutive Frobenius maps in higher extensions.
 */
static fp_st const_qua[5];

/**
 * Constant used to compute consecutive Frobenius maps in higher extensions.
 */
static fp_st const_qui[5];

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void fp3_const_calc() {
	bn_t e;
	fp3_t t0, t1, t2;

	bn_null(e);
	fp3_null(t0);
	fp3_null(t1);
	fp3_null(t2);

	TRY {
		bn_new(e);
		fp3_new(t0);
		fp3_new(t1);
		fp3_new(t2);

		fp_set_dig(const_base[0], -fp_prime_get_cnr());
		fp_neg(const_base[0], const_base[0]);
		e->used = FP_DIGS;
		dv_copy(e->dp, fp_prime_get(), FP_DIGS);
		bn_sub_dig(e, e, 1);
		bn_div_dig(e, e, 3);
		fp_exp(const_base[0], const_base[0], e);
		fp_sqr(const_base[1], const_base[0]);

		fp3_zero(t0);
		fp_set_dig(t0[1], 1);
		dv_copy(e->dp, fp_prime_get(), FP_DIGS);
		bn_sub_dig(e, e, 1);
		bn_div_dig(e, e, 6);

		/* t0 = u^((p-1)/6). */
		fp3_exp(t0, t0, e);
		fp_copy(const_frb[0], t0[2]);
		fp3_sqr(t1, t0);
		fp_copy(const_frb[1], t1[1]);
		fp3_mul(t2, t1, t0);
		fp_copy(const_frb[2], t2[0]);
		fp3_sqr(t2, t1);
		fp_copy(const_frb[3], t2[2]);
		fp3_mul(t2, t2, t0);
		fp_copy(const_frb[4], t2[1]);

		fp_mul(const_sqr[0], const_frb[0], const_base[1]);
		fp_mul(t0[0], const_sqr[0], const_frb[0]);
		fp_neg(const_sqr[0], t0[0]);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_sub(const_sqr[0], const_sqr[0], t0[0]);
		}
		fp_mul(const_sqr[1], const_frb[1], const_base[0]);
		fp_mul(const_sqr[1], const_sqr[1], const_frb[1]);
		fp_sqr(const_sqr[2], const_frb[2]);
		fp_mul(const_sqr[3], const_frb[3], const_base[1]);
		fp_mul(t0[0], const_sqr[3], const_frb[3]);
		fp_neg(const_sqr[3], t0[0]);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_sub(const_sqr[3], const_sqr[3], t0[0]);
		}
		fp_mul(const_sqr[4], const_frb[4], const_base[0]);
		fp_mul(const_sqr[4], const_sqr[4], const_frb[4]);

		fp_mul(const_cub[0], const_frb[0], const_base[0]);
		fp_mul(t0[0], const_cub[0], const_sqr[0]);
		fp_neg(const_cub[0], t0[0]);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_sub(const_cub[0], const_cub[0], t0[0]);
		}
		fp_mul(const_cub[1], const_frb[1], const_base[1]);
		fp_mul(t0[0], const_cub[1], const_sqr[1]);
		fp_neg(const_cub[1], t0[0]);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_sub(const_cub[1], const_cub[1], t0[0]);
		}
		fp_mul(const_cub[2], const_frb[2], const_sqr[2]);
		fp_mul(const_cub[3], const_frb[3], const_base[0]);
		fp_mul(t0[0], const_cub[3], const_sqr[3]);
		fp_neg(const_cub[3], t0[0]);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_sub(const_cub[3], const_cub[3], t0[0]);
		}
		fp_mul(const_cub[4], const_frb[4], const_base[1]);
		fp_mul(t0[0], const_cub[4], const_sqr[4]);
		fp_neg(const_cub[4], t0[0]);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_sub(const_cub[4], const_cub[4], t0[0]);
		}

		for (int i = 0; i < 5; i++) {
			fp_mul(const_qua[i], const_frb[i], const_cub[i]);
			fp_mul(const_qui[i], const_sqr[i], const_cub[i]);
		}
	} CATCH_ANY {
		THROW(ERR_CAUGHT);
	} FINALLY {
		bn_free(e);
		fp3_free(t0);
		fp3_free(t1);
		fp3_free(t2);
	}
}

#if PP_CBC == BASIC || !defined(STRIP)

void fp3_mul_basic(fp3_t c, fp3_t a, fp3_t b) {
	dv_t t, t0, t1, t2, t3, t4, t5, t6;

	dv_null(t);
	dv_null(t0);
	dv_null(t1);
	dv_null(t2);
	dv_null(t3);
	dv_null(t4);
	dv_null(t5);
	dv_null(t6);

	TRY {
		dv_new(t);
		dv_new(t0);
		dv_new(t1);
		dv_new(t2);
		dv_new(t3);
		dv_new(t4);
		dv_new(t5);
		dv_new(t6);

		/* TODO: Move to BASIC/INTEG structure. */

		/* Karatsuba algorithm. */

		/* t0 = a_0 * b_0, t1 = a_1 * b_1, t2 = a_2 * b_2. */
		fp_muln_low(t0, a[0], b[0]);
		fp_muln_low(t1, a[1], b[1]);
		fp_muln_low(t2, a[2], b[2]);

		/* t3 = (a_1 + a_2) * (b_1 + b_2). */
		fp_add(t3, a[1], a[2]);
		fp_add(t4, b[1], b[2]);
		fp_muln_low(t, t3, t4);
		fp_addd_low(t6, t1, t2);
		fp_subd_low(t4, t, t6);
		fp_subc_low(t3, t0, t4);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_subc_low(t3, t3, t4);
		}

		fp_add(t4, a[0], a[1]);
		fp_add(t5, b[0], b[1]);
		fp_muln_low(t, t4, t5);
		fp_addd_low(t4, t0, t1);
		fp_subd_low(t4, t, t4);
		fp_subc_low(t4, t4, t2);
		for (int i = -1; i > fp_prime_get_cnr(); i--) {
			fp_subc_low(t4, t4, t2);
		}

		fp_add(t5, a[0], a[2]);
		fp_add(t6, b[0], b[2]);
		fp_muln_low(t, t5, t6);
		fp_addd_low(t6, t0, t2);
		fp_subd_low(t5, t, t6);
		fp_addc_low(t5, t5, t1);

		/* c_0 = t3 mod p. */
		fp_rdc(c[0], t3);

		/* c_1 = t4 mod p. */
		fp_rdc(c[1], t4);

		/* c_2 = t5 mod p. */
		fp_rdc(c[2], t5);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		dv_free(t);
		dv_free(t0);
		dv_free(t1);
		dv_free(t2);
		dv_free(t3);
		dv_free(t4);
		dv_free(t5);
		dv_free(t6);
	}
}

#endif

#if PP_CBC == INTEG || !defined(STRIP)

void fp3_mul_integ(fp3_t c, fp3_t a, fp3_t b) {
	fp3_mulm_low(c, a, b);
}

#endif

void fp3_mul_nor(fp3_t c, fp3_t a) {
	fp_copy(c[1], a[0]);
	fp_dbl(c[0], a[2]);
	fp_neg(c[0], c[0]);
	fp_copy(c[2], a[1]);
}

void fp3_mul_art(fp3_t c, fp3_t a) {
	fp_t t;

	fp_null(t);

	TRY {
		fp_new(t);

		/* (a_0 + a_1 * u + a_1 * u^2) * u = a_0 * u + a_1 * u^2 + a_1 * u^3. */
		fp_copy(t, a[0]);
		fp_dbl(c[0], a[2]);
		fp_neg(c[0], c[0]);
		fp_copy(c[2], a[1]);
		fp_copy(c[1], t);
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fp_free(t);
	}
}

void fp3_mul_frb(fp3_t c, fp3_t a, int i, int j, int k) {
	if (i == 0) {
		switch (j % 3) {
		case 0:
			fp3_copy(c, a);
			break;
		case 1:
			fp_copy(c[0], a[0]);
			fp_mul(c[1], a[1], const_base[0]);
			fp_mul(c[2], a[2], const_base[1]);
			break;
		case 2:
			fp_copy(c[0], a[0]);
			fp_mul(c[1], a[1], const_base[1]);
			fp_mul(c[2], a[2], const_base[0]);
			break;
		}
	} else {
		switch (j % 6) {
			case 0:
				fp3_copy(c, a);
				break;
			case 1:
				fp_mul(c[0], a[0], const_frb[k - 1]);
				fp_mul(c[1], a[1], const_frb[k - 1]);
				fp_mul(c[2], a[2], const_frb[k - 1]);
				if (k != 3) {
					for (int l = 0; l < 3 - (k % 3); l++) {
						fp3_mul_art(c, c);
					}
				}
				break;
			case 2:
				fp_mul(c[0], a[0], const_sqr[k - 1]);
				fp_mul(c[1], a[1], const_sqr[k - 1]);
				fp_mul(c[2], a[2], const_sqr[k - 1]);
				for (int l = 0; l < (k % 3); l++) {
					fp3_mul_art(c, c);
				}
				break;
			case 3:
				fp_mul(c[0], a[0], const_cub[k - 1]);
				fp_mul(c[1], a[1], const_cub[k - 1]);
				fp_mul(c[2], a[2], const_cub[k - 1]);
				break;
			case 4:
				fp_mul(c[0], a[0], const_qua[k - 1]);
				fp_mul(c[1], a[1], const_qua[k - 1]);
				fp_mul(c[2], a[2], const_qua[k - 1]);
				if (k != 3) {
					for (int l = 0; l < 3 - (k % 3); l++) {
						fp3_mul_art(c, c);
					}
				}
				break;
			case 5:
				fp_mul(c[0], a[0], const_qui[k - 1]);
				fp_mul(c[1], a[1], const_qui[k - 1]);
				fp_mul(c[2], a[2], const_qui[k - 1]);
				for (int l = 0; l < (k % 3); l++) {
					fp3_mul_art(c, c);
				}
				break;
		}
	}
}