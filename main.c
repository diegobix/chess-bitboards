#include <stdio.h>

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
enum {
    white,
    black
};

const U64 FILE_H = 0x8080808080808080ULL;
const U64 FILE_A = 0x0101010101010101ULL;

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

// Lookup table de peones [color][casilla]
U64 pawn_attacks[2][64];
// Lookup table de caballos [casilla]
U64 knight_attacks[64];
// Lookup table de reyes [casilla]
U64 king_attacks[64];

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

void init_lookup_tables() {
    for (int square = 0; square < 64; square++) {
        pawn_attacks[white][square] = mask_pawn_attacks(white, square);
        pawn_attacks[black][square] = mask_pawn_attacks(black, square);

        knight_attacks[square] = mask_knight_attacks(square);
        king_attacks[square] = mask_king_attacks(square);
    }
}

int main() {
    init_lookup_tables();
    // for (int i = 0; i < 64; i++) {
    //     print_bb(king_attacks[i]);
    // }

    return 0;
}