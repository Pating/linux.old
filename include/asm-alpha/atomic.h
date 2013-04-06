#ifndef __ARCH_I386_ATOMIC__
#define __ARCH_I386_ATOMIC__

/*
 * Atomic operations that C can't guarantee us.  Useful for
 * resource counting etc..
 */

/*
 * Make sure gcc doesn't try to be clever and move things around
 * on us. We need to use _exactly_ the address the user gave us,
 * not some alias that contains the same information.
 */
#define __atomic_fool_gcc(x) (*(struct { int a[100]; } *)x)

typedef int atomic_t;

extern __inline__ void atomic_add(atomic_t i, atomic_t * v)
{
	unsigned long temp;
	__asm__ __volatile__(
		"\n1:\t"
		"ldl_l %0,%1\n\t"
		"addl %0,%2,%0\n\t"
		"stl_c %0,%1\n\t"
		"beq %0,1b\n"
		"2:"
		:"=&r" (temp),
		 "=m" (__atomic_fool_gcc(v))
		:"Ir" (i),
		 "m" (__atomic_fool_gcc(v)));
}

extern __inline__ void atomic_sub(atomic_t i, atomic_t * v)
{
	unsigned long temp;
	__asm__ __volatile__(
		"\n1:\t"
		"ldl_l %0,%1\n\t"
		"subl %0,%2,%0\n\t"
		"stl_c %0,%1\n\t"
		"beq %0,1b\n"
		"2:"
		:"=&r" (temp),
		 "=m" (__atomic_fool_gcc(v))
		:"Ir" (i),
		 "m" (__atomic_fool_gcc(v)));
}

#define atomic_inc(v) atomic_add(1,(v))
#define atomic_dec(v) atomic_sub(1,(v))

#endif
