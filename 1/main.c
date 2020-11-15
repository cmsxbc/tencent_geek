#include <stdio.h>
typedef unsigned int uint;
typedef unsigned char byte;

typedef struct Table
{
    uint	sin;	/* integer part of 4294967296 times abs(sin(i)) */
    byte	x;	/* index into data block */
    byte	rot;	/* amount to rotate left by */
}Table;

Table tab[] =
        {
                /* round 1 */
                { 0xd76aa478, 0, 7},
                { 0xe8c7b756, 1, 12},
                { 0x242070db, 2, 17},
                { 0xc1bdceee, 3, 22},
                { 0xf57c0faf, 4, 7},
                { 0x4787c62a, 5, 12},
                { 0xa8304613, 6, 17},
                { 0xfd469501, 7, 22},
                { 0x698098d8, 8, 7},
                { 0x8b44f7af, 9, 12},
                { 0xffff5bb1, 10, 17},
                { 0x895cd7be, 11, 22},
                { 0x6b901122, 12, 7},
                { 0xfd987193, 13, 12},
                { 0xa679438e, 14, 17},
                { 0x49b40821, 15, 22},

                /* round 2 */
                { 0xf61e2562, 1, 5},
                { 0xc040b340, 6, 9},
                { 0x265e5a51, 11, 14},
                { 0xe9b6c7aa, 0, 20},
                { 0xd62f105d, 5, 5},
                {  0x2441453, 10, 9},
                { 0xd8a1e681, 15, 14},
                { 0xe7d3fbc8, 4, 20},
                { 0x21e1cde6, 9, 5},
                { 0xc33707d6, 14, 9},
                { 0xf4d50d87, 3, 14},
                { 0x455a14ed, 8, 20},
                { 0xa9e3e905, 13, 5},
                { 0xfcefa3f8, 2, 9},
                { 0x676f02d9, 7, 14},
                { 0x8d2a4c8a, 12, 20},

                /* round 3 */
                { 0xfffa3942, 5, 4},
                { 0x8771f681, 8, 11},
                { 0x6d9d6122, 11, 16},
                { 0xfde5380c, 14, 23},
                { 0xa4beea44, 1, 4},
                { 0x4bdecfa9, 4, 11},
                { 0xf6bb4b60, 7, 16},
                { 0xbebfbc70, 10, 23},
                { 0x289b7ec6, 13, 4},
                { 0xeaa127fa, 0, 11},
                { 0xd4ef3085, 3, 16},
                {  0x4881d05, 6, 23},
                { 0xd9d4d039, 9, 4},
                { 0xe6db99e5, 12, 11},
                { 0x1fa27cf8, 15, 16},
                { 0xc4ac5665, 2, 23},

                /* round 4 */
                { 0xf4292244, 0, 6},
                { 0x432aff97, 7, 10},
                { 0xab9423a7, 14, 15},
                { 0xfc93a039, 5, 21},
                { 0x655b59c3, 12, 6},
                { 0x8f0ccc92, 3, 10},
                { 0xffeff47d, 10, 15},
                { 0x85845dd1, 1, 21},
                { 0x6fa87e4f, 8, 6},
                { 0xfe2ce6e0, 15, 10},
                { 0xa3014314, 6, 15},
                { 0x4e0811a1, 13, 21},
                { 0xf7537e82, 4, 6},
                { 0xbd3af235, 11, 10},
                { 0x2ad7d2bb, 2, 15},
                { 0xeb86d391, 9, 21},
        };

typedef struct MD5state
{
    uint len;
    uint state[4];
}MD5state;

void encode(byte*, uint*, uint);
void decode(uint*, byte*, uint);
void md5(byte*, uint, MD5state*);

int
main(int argc, char **argv)
{
    FILE *fd;
    fd = fopen(argv[0],"r");

    byte buf[15000];
    byte digest[16];
    int i, n;
    MD5state s = {
            0,
            {
                0x67452301,
                0xefcdab89,
                0x98badcfe,
                0x10325476
            }
    };

    n = fread(buf, 1, 15000, fd);
    md5(buf, n, &s);
    encode(digest, s.state, 16);
    for(i=0;i<16;i++) printf("%02x", digest[i]);
    printf("\n");
    return 0;
}

/*
 *  I require len to be a multiple of 64 for all but
 *  the last call
 */

void md5(byte *p, uint len, MD5state *s)
{
    uint a, b, c, d, tmp;
    uint i;
    Table *t;
    byte *end;
    uint x[16];

    s->len += len;

    i = len & 0x3f;
    if(i || len == 0){

        /* pad the input, assume there's room */
        if(i < 56)
            i = 56 - i;
        else
            i = 120 - i;
        if(i > 0){
            for (int j = 0; j < i; j++) {
                p[len+j] = 0;
            }
            p[len] = 0x80;
        }
        len += i;

        /* append the count */
        x[0] = s->len<<3;
        x[1] = s->len>>29;
        encode(p+len, x, 8);
    }

    for(end = p+len; p < end; p += 64){
        a = s->state[0];
        b = s->state[1];
        c = s->state[2];
        d = s->state[3];

        decode(x, p, 64);

        for(i = 0; i < 64; i++){
            t = tab + i;
            switch(i>>4){
                case 0:
                    a += (b & c) | (~b & d);
                    break;
                case 1:
                    a += (b & d) | (c & ~d);
                    break;
                case 2:
                    a += b ^ c ^ d;
                    break;
                case 3:
                    a += c ^ (b | ~d);
                    break;
            }
            a += x[t->x] + t->sin;
            a = (a << t->rot) | (a >> (32 - t->rot));
            a += b;

            /* rotate variables */
            tmp = d;
            d = c;
            c = b;
            b = a;
            a = tmp;
        }

        s->state[0] += a;
        s->state[1] += b;
        s->state[2] += c;
        s->state[3] += d;
    }
}

/*
 *	encodes input (uint) into output (byte). Assumes len is
 *	a multiple of 4.
 */
void inline
encode(byte *output, uint *input, uint len)
{
    uint x;
    byte *e;

    for(e = output + len; output < e;) {
        x = *input++;
        *output++ = x;
        *output++ = x >> 8;
        *output++ = x >> 16;
        *output++ = x >> 24;
    }
}

/*
 *	decodes input (byte) into output (uint). Assumes len is
 *	a multiple of 4.
 */
void inline
decode(uint *output, byte *input, uint len)
{
    byte *e;

    for(e = input+len; input < e; input += 4)
        *output++ = input[0] | (input[1] << 8) |
                    (input[2] << 16) | (input[3] << 24);
}
