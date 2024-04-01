#ifndef SET_H
#define SET_H
typedef unsigned char     uint8_t;
#define maskBit(_bit) (1 << (_bit) )
#define isBitSelected(bitmap_p,_bit) (((uint8_t*)(void*)bitmap_p)[_bit>>3] & (maskBit((unsigned)(_bit) & 7 )))
#define markBit(bitmap_p,_bit) (((uint8_t*)(void*)bitmap_p)[_bit>>3] |= (maskBit((unsigned)(_bit) & 7 )))
#define unmarkBit(bitmap_p,_bit) (((uint8_t*)(void*)bitmap_p)[_bit>>3] &= (maskBit((unsigned)_bit & 7 )))

#define numBits (128)
#define SIZE (8)
#define bitmapSize (numBits/8)
#define TEMPSET (sets[6].SET)

typedef uint8_t set[bitmapSize];

typedef struct struct_set{
   const char *name;
   set SET;
} SET_S;

SET_S sets[7];
uint8_t* getSet(int);
void read_set(set, int [], int);
void reset_set(set);
void clone_set(set, set);
void add_to_set(set, int);
void stop();
void print_set(set);
void union_set(set, set, set);
void intersect_set(set, set, set);
void sub_set(set, set, set);
void symdiff_set(set, set, set);

#endif
