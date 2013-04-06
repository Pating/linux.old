/* $Id: fsubq.c,v 1.5 1999/08/02 14:08:06 jj Exp $
 * arch/sparc64/math-emu/fsubq.c
 *
 * Copyright (C) 1997, 1999 Jakub Jelinek (jj@ultra.linux.cz)
 * Copyright (C) 1999 David S. Miller (davem@redhat.com)
 *
 */

#include "sfp-util.h"
#include "soft-fp.h"
#include "quad.h"

int FSUBQ(void *rd, void *rs2, void *rs1)
{
	FP_DECL_EX;
	FP_DECL_Q(A); FP_DECL_Q(B); FP_DECL_Q(R);

	FP_UNPACK_QP(A, rs1);
	FP_UNPACK_QP(B, rs2);
	FP_SUB_Q(R, A, B);
	FP_PACK_QP(rd, R);
	FP_HANDLE_EXCEPTIONS;
}
