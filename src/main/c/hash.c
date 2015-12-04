//C hello world example
#include <stdio.h>
#include <stdint.h>

typedef unsigned int uint32;
typedef uintptr_t Datum;

#define SET_4_BYTES(value)  (((Datum) (value)) & 0xffffffff)

#define UInt32GetDatum(X) ((Datum) SET_4_BYTES(X))

#define UINT32_ALIGN_MASK (sizeof(uint32) - 1)

#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
  a -= c;  a ^= rot(c, 4);    c += b; \
  b -= a;  b ^= rot(a, 6);    a += c; \
  c -= b;  c ^= rot(b, 8);    b += a; \
  a -= c;  a ^= rot(c,16);    c += b; \
  b -= a;  b ^= rot(a,19);    a += c; \
  c -= b;  c ^= rot(b, 4);    b += a; \
}

#define final(a,b,c) \
{ \
  c ^= b; c -= rot(b,14); \
  a ^= c; a -= rot(c,11); \
  b ^= a; b -= rot(a,25); \
  c ^= b; c -= rot(b,16); \
  a ^= c; a -= rot(c, 4); \
  b ^= a; b -= rot(a,14); \
  c ^= b; c -= rot(b,24); \
}

void printData(const unsigned char *m, uint32 a, uint32 b, uint32 c) {
    printf("message: %s\n", m);
    printf("a: %u\n", a);
    printf("b: %u\n", b);
    printf("c: %u\n", c);
}

uint32
hash_any(register const unsigned char *k, register int keylen)
{
	register uint32 a,
				b,
				c,
				len;

	/* Set up the internal state */
	len = keylen;
	a = b = c = 0x9e3779b9 + len + 3923095;

	/* If the source pointer is word-aligned, we use word-wide fetches */
	if (((intptr_t) k & UINT32_ALIGN_MASK) == 0)
	{
		/* Code path for aligned source data */
		register const uint32 *ka = (const uint32 *) k;

		/* handle most of the key */
		while (len >= 12)
		{
			a += ka[0];
			b += ka[1];
			c += ka[2];
			mix(a, b, c);
			ka += 3;
			len -= 12;
		}

		/* handle the last 11 bytes */
		k = (const unsigned char *) ka;
#ifdef WORDS_BIGENDIAN
		switch (len)
		{
			case 11:
				c += ((uint32) k[10] << 8);
				/* fall through */
			case 10:
				c += ((uint32) k[9] << 16);
				/* fall through */
			case 9:
				c += ((uint32) k[8] << 24);
				/* the lowest byte of c is reserved for the length */
				/* fall through */
			case 8:
				b += ka[1];
				a += ka[0];
				break;
			case 7:
				b += ((uint32) k[6] << 8);
				/* fall through */
			case 6:
				b += ((uint32) k[5] << 16);
				/* fall through */
			case 5:
				b += ((uint32) k[4] << 24);
				/* fall through */
			case 4:
				a += ka[0];
				break;
			case 3:
				a += ((uint32) k[2] << 8);
				/* fall through */
			case 2:
				a += ((uint32) k[1] << 16);
				/* fall through */
			case 1:
				a += ((uint32) k[0] << 24);
				/* case 0: nothing left to add */
		}
#else							/* !WORDS_BIGENDIAN */
		switch (len)
		{
			case 11:
				c += ((uint32) k[10] << 24);
				/* fall through */
			case 10:
				c += ((uint32) k[9] << 16);
				/* fall through */
			case 9:
				c += ((uint32) k[8] << 8);
				/* the lowest byte of c is reserved for the length */
				/* fall through */
			case 8:
				b += ka[1];
				a += ka[0];
				break;
			case 7:
				b += ((uint32) k[6] << 16);
				/* fall through */
			case 6:
				b += ((uint32) k[5] << 8);
				/* fall through */
			case 5:
				b += k[4];
				/* fall through */
			case 4:
				a += ka[0];
				break;
			case 3:
				a += ((uint32) k[2] << 16);
				/* fall through */
			case 2:
				a += ((uint32) k[1] << 8);
				/* fall through */
			case 1:
				a += k[0];
				/* case 0: nothing left to add */
		}
#endif   /* WORDS_BIGENDIAN */
	}
	else
	{
		/* Code path for non-aligned source data */

		/* handle most of the key */
		while (len >= 12)
		{
#ifdef WORDS_BIGENDIAN
			a += (k[3] + ((uint32) k[2] << 8) + ((uint32) k[1] << 16) + ((uint32) k[0] << 24));
			b += (k[7] + ((uint32) k[6] << 8) + ((uint32) k[5] << 16) + ((uint32) k[4] << 24));
			c += (k[11] + ((uint32) k[10] << 8) + ((uint32) k[9] << 16) + ((uint32) k[8] << 24));
#else							/* !WORDS_BIGENDIAN */
			a += (k[0] + ((uint32) k[1] << 8) + ((uint32) k[2] << 16) + ((uint32) k[3] << 24));
			b += (k[4] + ((uint32) k[5] << 8) + ((uint32) k[6] << 16) + ((uint32) k[7] << 24));
			c += (k[8] + ((uint32) k[9] << 8) + ((uint32) k[10] << 16) + ((uint32) k[11] << 24));
#endif   /* WORDS_BIGENDIAN */
			mix(a, b, c);
			k += 12;
			len -= 12;
		}

		/* handle the last 11 bytes */
#ifdef WORDS_BIGENDIAN
		switch (len)			/* all the case statements fall through */
		{
			case 11:
				c += ((uint32) k[10] << 8);
			case 10:
				c += ((uint32) k[9] << 16);
			case 9:
				c += ((uint32) k[8] << 24);
				/* the lowest byte of c is reserved for the length */
			case 8:
				b += k[7];
			case 7:
				b += ((uint32) k[6] << 8);
			case 6:
				b += ((uint32) k[5] << 16);
			case 5:
				b += ((uint32) k[4] << 24);
			case 4:
				a += k[3];
			case 3:
				a += ((uint32) k[2] << 8);
			case 2:
				a += ((uint32) k[1] << 16);
			case 1:
				a += ((uint32) k[0] << 24);
				/* case 0: nothing left to add */
		}
#else							/* !WORDS_BIGENDIAN */
		switch (len)			/* all the case statements fall through */
		{
			case 11:
				c += ((uint32) k[10] << 24);
			case 10:
				c += ((uint32) k[9] << 16);
			case 9:
				c += ((uint32) k[8] << 8);
				/* the lowest byte of c is reserved for the length */
			case 8:
				b += ((uint32) k[7] << 24);
			case 7:
				b += ((uint32) k[6] << 16);
			case 6:
				b += ((uint32) k[5] << 8);
			case 5:
				b += k[4];
			case 4:
				a += ((uint32) k[3] << 24);
			case 3:
				a += ((uint32) k[2] << 16);
			case 2:
				a += ((uint32) k[1] << 8);
			case 1:
				a += k[0];
				/* case 0: nothing left to add */
		}
#endif   /* WORDS_BIGENDIAN */
	}

	final(a, b, c);

	/* report the result */
	return c;
}

int main()
{
  Datum hash11 = hash_any("4599900051861111", 16);
  Datum hash12 = hash_any("5489018406387201", 16);

  Datum hash21 = hash_any("5489018406383201", 16);
  Datum hash22 = hash_any("4599900051867111", 16);
  Datum hash23 = hash_any("5489018406383201", 16);

  Datum hash31 = hash_any("4599900051862111", 16);

  printf("hash11: %lu, node: %lu\n", hash11, hash11 % 3);
  printf("hash12: %lu, node: %lu\n", hash12, hash12 % 3);

  printf("hash21: %lu, node: %lu\n", hash21, hash21 % 3);
  printf("hash22: %lu, node: %lu\n", hash22, hash22 % 3);
  printf("hash23: %lu, node: %lu\n", hash23, hash23 % 3);

  printf("hash31: %lu, node: %lu\n", hash31, hash31 % 3);

//  Datum hash41 = hash_any("3836199178378192871", 19);
//  Datum hash42 = hash_any("913487234767623476123423", 24);
//  Datum hash43 = hash_any("100934297616532476", 18);
//
//  printf("hash41: %lu, node: %lu\n", hash41, hash41 % 3);
//  printf("hash42: %lu, node: %lu\n", hash42, hash42 % 3);
//  printf("hash43: %lu, node: %lu\n", hash43, hash43 % 3);

  printf("size of int %lu", sizeof(int));

  return 0;
}

