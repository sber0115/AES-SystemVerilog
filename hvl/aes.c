/*
** Filename: aes.c
**
** Copyright (c) 2013, Intel Corporation
** All rights reserved
**
** Reference: http://csrc.nist.gov/publications/fips/fips197/fips-197.pdf
*/

#include "aes.h"

static byte_t sbox[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

static byte_t isbox[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};

static byte_t rcon[255] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40,
    0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a,
    0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
    0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25,
    0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
    0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08,
    0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6,
    0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61,
    0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
    0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01,
    0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
    0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e,
    0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4,
    0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8,
    0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
    0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d,
    0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
    0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91,
    0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d,
    0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c,
    0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
    0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa,
    0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
    0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66,
    0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb
};

/*
** Apply an S-box to each byte of *word*.
*/
static inline word_t
SubWord(word_t word)
{
    int i;
    word_t out;

    byte_t *bp_in  = (byte_t *) &word;
    byte_t *bp_out = (byte_t *) &out;

    //for (i = 0; i < sizeof(word_t); i++)
    //    *bp_out++ = sbox[*bp_in++];

    bp_out[0] = sbox[bp_in[0]];
    bp_out[1] = sbox[bp_in[1]];
    bp_out[2] = sbox[bp_in[2]];
    bp_out[3] = sbox[bp_in[3]];

    return out;
}

/*
** Rotate the lower byte of *word* into the upper byte.
*/
static inline word_t
RotWord(word_t word)
{
    return ((word & 0xff) << 3*8) | (word >> 8);
}

/*
** Add (using XOR) a round key to *state*.
**
** See section 5.1.4
**
** len(rkey) = Nb*(Nr+1)
*/
static inline void
AddRoundKey(byte_t state[Nb][4], word_t * rkey, int round)
{
    int col;
    word_t *wp_state = (word_t *) state;
    word_t *wp_rkey = rkey + (Nb * round);

    for (col = 0; col < Nb; col++)
        *wp_state++ ^= *wp_rkey++;
}

/*
** Apply S-box transformation to each word of *state*.
**
** See section 5.1.1
*/
static inline void
SubBytes(byte_t state[Nb][4])
{
    int row, col;
    for (col = 0; col < Nb; col++)
    for (row = 0; row < 4;  row++)
        state[col][row] = sbox[state[col][row]];
}

/*
** Inverse of SubBytes() operation
**
** See section 5.3.2
*/
static inline void
InvSubBytes(byte_t state[Nb][4])
{
    int row, col;
    for (col = 0; col < Nb; col++)
    for (row = 0; row < 4;  row++)
        state[col][row] = isbox[state[col][row]];
}

/*
** Cyclically shift the last three rows of *state* by different offsets.
**
** See section 5.1.2
*/
static inline void
ShiftRows(byte_t state[Nb][4])
{
    byte_t temp;

    temp = state[0][1];
    state[0][1] = state[1][1];
    state[1][1] = state[2][1];
    state[2][1] = state[3][1];
    state[3][1] = temp;

    temp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;
    temp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    temp = state[0][3];
    state[0][3] = state[3][3];
    state[3][3] = state[2][3];
    state[2][3] = state[1][3];
    state[1][3] = temp;
}

/*
** Inverse of ShiftRows() operation
**
** See section 5.3.1
*/
static inline void
InvShiftRows(byte_t state[Nb][4])
{
    byte_t temp;

    temp = state[3][1];
    state[3][1] = state[2][1];
    state[2][1] = state[1][1];
    state[1][1] = state[0][1];
    state[0][1] = temp;

    temp = state[0][2];
    state[0][2] = state[2][2];
    state[2][2] = temp;
    temp = state[1][2];
    state[1][2] = state[3][2];
    state[3][2] = temp;

    temp = state[0][3];
    state[0][3] = state[1][3];
    state[1][3] = state[2][3];
    state[2][3] = state[3][3];
    state[3][3] = temp;
}

#define _MULT(C0, C1, C2, C3) \
    Multiply(s0, C0) ^ Multiply(s1, C1) ^ Multiply(s2, C2) ^ Multiply(s3, C3)

/*
** Mix the data of the *state* columns.
**
** See section 5.1.3
*/
static void
MixColumns(byte_t state[Nb][4])
{
    int i;
    byte_t s0, s1, s2, s3;

    for(i = 0; i < Nb; i++)
    {
        s0 = state[i][0];
        s1 = state[i][1];
        s2 = state[i][2];
        s3 = state[i][3];

        state[i][0] = _MULT(0x2, 0x3, 0x1, 0x1);
        state[i][1] = _MULT(0x1, 0x2, 0x3, 0x1);
        state[i][2] = _MULT(0x1, 0x1, 0x2, 0x3);
        state[i][3] = _MULT(0x3, 0x1, 0x1, 0x2);

    }
}

/*
** Inverse of MixColumns() operation
**
** See section 5.3.3
*/
static void
InvMixColumns(byte_t state[Nb][4])
{
    int i;
    byte_t s0, s1, s2, s3;

    for (i = 0; i < Nb; i++)
    {
        s0 = state[i][0];
        s1 = state[i][1];
        s2 = state[i][2];
        s3 = state[i][3];

        state[i][0] = _MULT(0xe, 0xb, 0xd, 0x9);
        state[i][1] = _MULT(0x9, 0xe, 0xb, 0xd);
        state[i][2] = _MULT(0xd, 0x9, 0xe, 0xb);
        state[i][3] = _MULT(0xb, 0xd, 0x9, 0xe);
    }
}

#undef _MULT

/*
** Generate a round key schedule from *key*.
**
** See section 5.2
**
** len(key)  = 4*Nk
** len(rkey) = Nb*(Nr+1)
*/
void
KeyExpansion(int Nk, word_t *rkey, word_t *key)
{
    int i;
    int Nr = Nk + 6;
    word_t temp;

    word_t *wp_key = key;
    word_t *wp_rkey = rkey;

    for (i = 0; i < Nk; i++)
        *wp_rkey++ = *wp_key++;

    for (i = Nk; i < (Nb * (Nr + 1)); i++)
    {
        temp = rkey[i-1];
        if (i % Nk == 0)
            temp = SubWord(RotWord(temp)) ^ rcon[i/Nk];
        else if (Nk > 6 && i % Nk == 4)
            temp = SubWord(temp);
        rkey[i] = rkey[i-Nk] ^ temp;
    }
}

/*
** AES cipher transformation
**
** See section 5.1
**
** len(rkey) = Nb*(Nr+1)
*/
void
Cipher(int Nk, byte_t out[4*Nb], byte_t in[4*Nb], word_t * rkey)
{
    int row, col, round = 0;
    int Nr = Nk + 6;

    byte_t state[Nb][4];
    word_t *wp_state;
    word_t *wp_io;

    // state = in
    wp_state = (word_t *) state;
    wp_io = (word_t *) in;
    for(col = 0; col < Nb; col++)
        *wp_state++ = *wp_io++;

    AddRoundKey(state, rkey, 0);
    for(round = 1; round < Nr; round++)
    {
        SubBytes(state);
        ShiftRows(state);
        MixColumns(state);
        AddRoundKey(state, rkey, round);
    }
    SubBytes(state);
    ShiftRows(state);
    AddRoundKey(state, rkey, Nr);

    // out = state
    wp_io = (word_t *) out;
    wp_state = (word_t *) state;
    for(col = 0; col < Nb; col++)
        *wp_io++ = *wp_state++;
}

/*
** Inverse of Cipher() operation
**
** See section 5.3
**
** len(rkey) = Nb*(Nr+1)
*/
void
InvCipher(int Nk, byte_t out[4*Nb], byte_t in[4*Nb], word_t * rkey)
{
    int row, col, round = 0;
    int Nr = Nk + 6;

    byte_t state[Nb][4];
    word_t *wp_state;
    word_t *wp_io;

    // state = in
    wp_state = (word_t *) state;
    wp_io = (word_t *) in;
    for (col = 0; col < Nb; col++)
        *wp_state++ = *wp_io++;

    AddRoundKey(state, rkey, Nr);
    for (round = Nr - 1; round > 0; round--)
    {
        InvShiftRows(state);
        InvSubBytes(state);
        AddRoundKey(state, rkey, round);
        InvMixColumns(state);
    }
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(state, rkey, 0);

    // out = state
    wp_io = (word_t *) out;
    wp_state = (word_t *) state;
    for (col = 0; col < Nb; col++)
        *wp_io++ = *wp_state++;
}
