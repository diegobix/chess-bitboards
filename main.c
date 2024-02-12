#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define north(bitboard) (bitboard << 8)
#define north_east(bitboard) ((bitboard << 9) & ~FILE_A)
#define east(bitboard) ((bitboard << 1) & ~FILE_A)
#define south_east(bitboard) ((bitboard >> 7) & ~FILE_A)
#define south(bitboard) (bitboard >> 8)
#define south_west(bitboard) ((bitboard >> 9) & ~FILE_H)
#define west(bitboard) ((bitboard >> 1) & ~FILE_H)
#define north_west(bitboard) ((bitboard << 7) & ~FILE_H)

#define set_bit(bitboard, square) (bitboard |= (1ULL << square))
#define get_bit(bitboard, square) (bitboard & (1ULL << square))
#define clear_bit(bitboard, square) (bitboard &= ~(1ULL << square))

typedef unsigned long long U64;

enum {
    a1,
    b1,
    c1,
    d1,
    e1,
    f1,
    g1,
    h1,
    a2,
    b2,
    c2,
    d2,
    e2,
    f2,
    g2,
    h2,
    a3,
    b3,
    c3,
    d3,
    e3,
    f3,
    g3,
    h3,
    a4,
    b4,
    c4,
    d4,
    e4,
    f4,
    g4,
    h4,
    a5,
    b5,
    c5,
    d5,
    e5,
    f5,
    g5,
    h5,
    a6,
    b6,
    c6,
    d6,
    e6,
    f6,
    g6,
    h6,
    a7,
    b7,
    c7,
    d7,
    e7,
    f7,
    g7,
    h7,
    a8,
    b8,
    c8,
    d8,
    e8,
    f8,
    g8,
    h8
};
enum { white, black };

const char *square_name[] = {
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "a2", "b2", "c2",
    "d2", "e2", "f2", "g2", "h2", "a3", "b3", "c3", "d3", "e3", "f3",
    "g3", "h3", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "a5",
    "b5", "c5", "d5", "e5", "f5", "g5", "h5", "a6", "b6", "c6", "d6",
    "e6", "f6", "g6", "h6", "a7", "b7", "c7", "d7", "e7", "f7", "g7",
    "h7", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

const U64 FILE_H = 0x8080808080808080ULL;
const U64 FILE_A = 0x0101010101010101ULL;

// Lookup table de peones [color][casilla]
U64 pawn_attacks[2][64];
// Lookup table de caballos [casilla]
U64 knight_attacks[64];
// Lookup table de reyes [casilla]
U64 king_attacks[64];

const int bishop_relevant_occupancy_bits[64] = {
    6, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 7, 7, 7, 7,
    5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 9, 9, 7, 5, 5, 5, 5, 7, 7,
    7, 7, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 5, 5, 5, 5, 5, 5, 6,
};
const int rook_relevant_occupancy_bits[64] = {
    12, 11, 11, 11, 11, 11, 11, 12, 11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11, 11, 10, 10, 10, 10, 10, 10, 11,
    11, 10, 10, 10, 10, 10, 10, 11, 12, 11, 11, 11, 11, 11, 11, 12,
};

const U64 rook_magic_numers[64] = {
    0x8a80104000800020ULL, 0x140002000100040ULL,  0x2801880a0017001ULL,
    0x100081001000420ULL,  0x200020010080420ULL,  0x3001c0002010008ULL,
    0x8480008002000100ULL, 0x2080088004402900ULL, 0x800098204000ULL,
    0x2024401000200040ULL, 0x100802000801000ULL,  0x120800800801000ULL,
    0x208808088000400ULL,  0x2802200800400ULL,    0x2200800100020080ULL,
    0x801000060821100ULL,  0x80044006422000ULL,   0x100808020004000ULL,
    0x12108a0010204200ULL, 0x140848010000802ULL,  0x481828014002800ULL,
    0x8094004002004100ULL, 0x4010040010010802ULL, 0x20008806104ULL,
    0x100400080208000ULL,  0x2040002120081000ULL, 0x21200680100081ULL,
    0x20100080080080ULL,   0x2000a00200410ULL,    0x20080800400ULL,
    0x80088400100102ULL,   0x80004600042881ULL,   0x4040008040800020ULL,
    0x440003000200801ULL,  0x4200011004500ULL,    0x188020010100100ULL,
    0x14800401802800ULL,   0x2080040080800200ULL, 0x124080204001001ULL,
    0x200046502000484ULL,  0x480400080088020ULL,  0x1000422010034000ULL,
    0x30200100110040ULL,   0x100021010009ULL,     0x2002080100110004ULL,
    0x202008004008002ULL,  0x20020004010100ULL,   0x2048440040820001ULL,
    0x101002200408200ULL,  0x40802000401080ULL,   0x4008142004410100ULL,
    0x2060820c0120200ULL,  0x1001004080100ULL,    0x20c020080040080ULL,
    0x2935610830022400ULL, 0x44440041009200ULL,   0x280001040802101ULL,
    0x2100190040002085ULL, 0x80c0084100102001ULL, 0x4024081001000421ULL,
    0x20030a0244872ULL,    0x12001008414402ULL,   0x2006104900a0804ULL,
    0x1004081002402ULL};

const U64 bishop_magic_numbers[64] = {
    0x40040844404084ULL,   0x2004208a004208ULL,   0x10190041080202ULL,
    0x108060845042010ULL,  0x581104180800210ULL,  0x2112080446200010ULL,
    0x1080820820060210ULL, 0x3c0808410220200ULL,  0x4050404440404ULL,
    0x21001420088ULL,      0x24d0080801082102ULL, 0x1020a0a020400ULL,
    0x40308200402ULL,      0x4011002100800ULL,    0x401484104104005ULL,
    0x801010402020200ULL,  0x400210c3880100ULL,   0x404022024108200ULL,
    0x810018200204102ULL,  0x4002801a02003ULL,    0x85040820080400ULL,
    0x810102c808880400ULL, 0xe900410884800ULL,    0x8002020480840102ULL,
    0x220200865090201ULL,  0x2010100a02021202ULL, 0x152048408022401ULL,
    0x20080002081110ULL,   0x4001001021004000ULL, 0x800040400a011002ULL,
    0xe4004081011002ULL,   0x1c004001012080ULL,   0x8004200962a00220ULL,
    0x8422100208500202ULL, 0x2000402200300c08ULL, 0x8646020080080080ULL,
    0x80020a0200100808ULL, 0x2010004880111000ULL, 0x623000a080011400ULL,
    0x42008c0340209202ULL, 0x209188240001000ULL,  0x400408a884001800ULL,
    0x110400a6080400ULL,   0x1840060a44020800ULL, 0x90080104000041ULL,
    0x201011000808101ULL,  0x1a2208080504f080ULL, 0x8012020600211212ULL,
    0x500861011240000ULL,  0x180806108200800ULL,  0x4000020e01040044ULL,
    0x300000261044000aULL, 0x802241102020002ULL,  0x20906061210001ULL,
    0x5a84841004010310ULL, 0x4010801011c04ULL,    0xa010109502200ULL,
    0x4a02012000ULL,       0x500201010098b028ULL, 0x8040002811040900ULL,
    0x28000010020204ULL,   0x6000020202d0240ULL,  0x8918844842082200ULL,
    0x4010011029020020ULL};

unsigned int state = 1804289383;
unsigned int get_random_U32_number() {
    unsigned int number = state;

    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    state = number;
    return number;
}

U64 get_random_U64_number() {
    U64 n1, n2, n3, n4;

    n1 = (U64)get_random_U32_number() & 0xffff;
    n2 = (U64)get_random_U32_number() & 0xffff;
    n3 = (U64)get_random_U32_number() & 0xffff;
    n4 = (U64)get_random_U32_number() & 0xffff;

    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

U64 generate_magic_number() {
    return get_random_U64_number() & get_random_U64_number() &
           get_random_U64_number();
}

static inline int count_bits(U64 bitboard) {
    int count = 0;

    while (bitboard > 0) {
        count++;
        bitboard &= bitboard - 1;
    }

    return count;
}

static inline int get_ls1b_index(U64 bitboard) {
    if (!bitboard) return -1;

    return (count_bits((bitboard & -bitboard) - 1));
}

void print_bb(U64 bb) {
    printf("\n");

    for (int rank = 7; rank >= 0; rank--) {
        for (int file = 0; file < 8; file++) {
            int square = rank * 8 + file;

            if (file == 0) {
                printf("  %d ", rank + 1);
            }
            printf(" %c", get_bit(bb, square) ? '1' : '.');
            // printf(" %02d ", square);
        }
        printf("\n");
    }
    printf("\n     a b c d e f g h");
    printf("\n\n\tBitboard: %llu", bb);
    printf("\n\tHex: %llx\n\n", bb);
}

U64 mask_pawn_attacks(int side, int square) {
    // Piece bb
    U64 bitboard = 0ULL;

    // Result attack bb
    U64 attacks = 0ULL;

    set_bit(bitboard, square);

    if (side == white) {
        // A la derecha
        attacks |= north_east(bitboard);

        // A la izquierda
        attacks |= north_west(bitboard);
    } else {
        // A la derecha
        attacks |= south_east(bitboard);

        // A la izq
        attacks |= south_west(bitboard);
    }

    return attacks;
}

U64 mask_knight_attacks(int square) {
    U64 piece_bb = 0;
    set_bit(piece_bb, square);

    U64 attacks = 0;

    // Arriba arriba derecha
    attacks |= north(north_east(piece_bb));
    // Arriba derecha derecha
    attacks |= north_east(east(piece_bb));

    // Abajo derecha derecha
    attacks |= south_east(east(piece_bb));
    // Abajo abajo derecha
    attacks |= south(south_east(piece_bb));

    // Abajo abajo izquierda
    attacks |= south(south_west(piece_bb));
    // Abajo izq izq
    attacks |= south_west(west(piece_bb));

    // Arriba izq izq
    attacks |= north_west(west(piece_bb));
    // Arriba arriba izq
    attacks |= north(north_west(piece_bb));

    return attacks;
}

U64 mask_king_attacks(int square) {
    U64 piece_bb = 0;
    set_bit(piece_bb, square);

    U64 attacks = 0;

    attacks |= north_west(piece_bb) | north(piece_bb) | north_east(piece_bb);
    attacks |= south_west(piece_bb) | south(piece_bb) | south_east(piece_bb);
    attacks |= west(piece_bb) | east(piece_bb);

    return attacks;
}

U64 mask_bishop_attacks(int square) {
    U64 attacks = 0ULL;

    int tr = square / 8;
    int tf = square % 8;

    for (int r = tr + 1, f = tf + 1; r <= 6 && f <= 6; r++, f++)
        attacks |= (1ULL << (r * 8 + f));

    for (int r = tr - 1, f = tf + 1; r >= 1 && f <= 6; r--, f++)
        attacks |= (1ULL << (r * 8 + f));

    for (int r = tr + 1, f = tf - 1; r <= 6 && f >= 1; r++, f--)
        attacks |= (1ULL << (r * 8 + f));

    for (int r = tr - 1, f = tf - 1; r >= 1 && f >= 1; r--, f--)
        attacks |= (1ULL << (r * 8 + f));

    return attacks;
}

U64 mask_rook_attacks(int square) {
    U64 attacks = 0;

    int tr = square / 8;
    int tf = square % 8;

    for (int r = tr + 1; r <= 6; r++) attacks |= (1ULL << (r * 8 + tf));
    for (int r = tr - 1; r >= 1; r--) attacks |= (1ULL << (r * 8 + tf));
    for (int f = tf + 1; f <= 6; f++) attacks |= (1ULL << (tr * 8 + f));
    for (int f = tf - 1; f >= 1; f--) attacks |= (1ULL << (tr * 8 + f));

    return attacks;
}

U64 bishop_attacks_on_the_fly(int square, U64 blockers) {
    U64 attacks = 0ULL;

    int tr = square / 8;
    int tf = square % 8;

    for (int r = tr + 1, f = tf + 1; r <= 7 && f <= 7; r++, f++) {
        U64 attacked_square = (1ULL << (r * 8 + f));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }

    for (int r = tr - 1, f = tf + 1; r >= 0 && f <= 7; r--, f++) {
        U64 attacked_square = (1ULL << (r * 8 + f));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }

    for (int r = tr + 1, f = tf - 1; r <= 7 && f >= 0; r++, f--) {
        U64 attacked_square = (1ULL << (r * 8 + f));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }

    for (int r = tr - 1, f = tf - 1; r >= 0 && f >= 0; r--, f--) {
        U64 attacked_square = (1ULL << (r * 8 + f));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }

    return attacks;
}

U64 rook_attacks_on_the_fly(int square, U64 blockers) {
    U64 attacks = 0;

    int tr = square / 8;
    int tf = square % 8;

    for (int r = tr + 1; r <= 7; r++) {
        U64 attacked_square = (1ULL << (r * 8 + tf));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }
    for (int r = tr - 1; r >= 0; r--) {
        U64 attacked_square = (1ULL << (r * 8 + tf));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }
    for (int f = tf + 1; f <= 7; f++) {
        U64 attacked_square = (1ULL << (tr * 8 + f));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }
    for (int f = tf - 1; f >= 0; f--) {
        U64 attacked_square = (1ULL << (tr * 8 + f));
        attacks |= attacked_square;
        if (attacked_square & blockers) break;
    }
    return attacks;
}

void init_lookup_tables() {
    for (int square = 0; square < 64; square++) {
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);

        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}

// Mas bien get_occupancy. Obtiene la posicion index de las 2^bits_in_mask
// posibles de ocupacion
U64 set_occupancy(int index, int bits_in_mask, U64 attack_mask) {
    U64 occupancy = 0;

    for (int count = 0; count < bits_in_mask; count++) {
        int square = get_ls1b_index(attack_mask);
        clear_bit(attack_mask, square);
        if (index & (1 << count)) occupancy |= (1ULL << square);
    }

    return occupancy;
}

U64 find_magic_number(int square, int relevant_bits, bool bishop) {
    U64 occupancies[4096];
    U64 attacks[4096];
    U64 used_attacks[4096];

    U64 attack_mask =
        bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    int occupancy_indexes = 1 << relevant_bits;

    for (int i = 0; i < occupancy_indexes; i++) {
        occupancies[i] = set_occupancy(i, relevant_bits, attack_mask);
        attacks[i] = bishop ? bishop_attacks_on_the_fly(square, occupancies[i])
                            : rook_attacks_on_the_fly(square, occupancies[i]);
    }

    // Test magic nubers
    for (int i = 0; i < 100000000; i++) {
        U64 magic = generate_magic_number();

        if (count_bits((attack_mask * magic) & 0xff00000000000000) < 6)
            continue;

        memset(used_attacks, 0ULL, sizeof(used_attacks));

        int fail = 0;
        for (int i = 0; !fail && i < occupancy_indexes; i++) {
            int magic_index =
                (int)((occupancies[i] * magic) >> (64 - relevant_bits));

            if (used_attacks[magic_index] == 0ULL)
                used_attacks[magic_index] = attacks[i];
            else if (used_attacks[magic_index] != attacks[i])
                fail = 1;
        }
        if (!fail) return magic;
    }
    printf("Magic number not found\n");
    return 0ULL;
}

void find_magic_numbers() {
    for (int square = 0; square < 64; square++) {
        printf(" 0x%llxULL,\n",
               find_magic_number(square, rook_relevant_occupancy_bits[square],
                                 false));
    }

    printf("\n\n\n");

    for (int square = 0; square < 64; square++) {
        printf(" 0x%llxULL,\n",
               find_magic_number(square, bishop_relevant_occupancy_bits[square],
                                 true));
    }
}

int main() {
    // init_lookup_tables();

    find_magic_numbers();

    return 0;
}