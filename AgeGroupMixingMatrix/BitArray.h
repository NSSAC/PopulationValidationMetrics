#ifndef _BITARRAY_H
#define _BITARRAY_H

// On the Sun, the storage order of bits is as follows:
//   imagine a word represented as a bit array stretching from 
//   0 on the left to 31 on the right. If "bits" is a BITS pointer
//   then bits[0] -> position 31 (end of 1st word)
//        bits[1] -> position 30, etc.
//        bits[32] -> position 63 (end of second word)
//

#include <iostream>
#include <assert.h>
#include <memory.h>		// for memset

using namespace std;

typedef unsigned int BITS;
typedef unsigned int BITNUM;

// MACHINE DEPENDENT VARIABLES
const short BitsPerByte = 8; 
const short BytesPerWord = 4;   // Ray: shouldn't sizeof(BITS) ?
const short BitsPerWord = BitsPerByte * BytesPerWord;
const short BitsPerWordMask = BitsPerWord - 1;
const short LogBitsPerByte = 3;  // for doing shift operations
const short LogBitsPerWord = 5;  // for doing shift operations

extern BITS notSingleBitMask[BitsPerWord];
extern BITS singleBitMask[BitsPerWord];
extern BITS reverseBitMask[BitsPerWord];
extern BITS bitMask[BitsPerWord+1];
extern BITS notBitMask[BitsPerWord+1];

// A Lookup Table for counting how many bits set
// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetTable
static const unsigned char BitsSetTable256[256] = 
{
#   define B2(n) n,     n+1,     n+1,     n+2
#   define B4(n) B2(n), B2(n+1), B2(n+1), B2(n+2)
#   define B6(n) B4(n), B4(n+1), B4(n+1), B4(n+2)
        B6(0), B6(1), B6(1), B6(2)
};

static const unsigned int setIdxTable[33] = 
{
    0,          //  1       0
    1,          //  2       1
    0, 1,       //  3       2
    2,          //  4       4
    0, 2,       //  5       5
    1, 2,       //  6       7
    0, 1, 2,    //  7       9
    3,          //  8       12
    0, 3,       //  9       13
    1, 3,       //  10      15
    0, 1, 3,    //  11      17
    2, 3,       //  12      20
    0, 2, 3,    //  13      22
    1, 2, 3,    //  14      25
    0, 1, 2, 3, //  15      28
    (unsigned int)-1 // end 32
};

static const unsigned int initIdxTable[16] =
{
    0,              // 1
    1,              // 2
    2,              // 3
    4,              // 4
    5,              // 5
    7,              // 6
    9,              // 7
    12,             // 8
    13,             // 9
    15,             // 10
    17,             // 11
    20,             // 12
    22,             // 13
    25,             // 14
    28,             // 15
    32              // end
};


inline int BitsToWords(BITNUM numbits)
{ return numbits ? (numbits-1)/BitsPerWord + 1 : 0; }

inline int BitsToBytes(BITNUM numbits)
{ return BitsToWords(numbits)*BytesPerWord; }

inline BITS *CopyBits(BITS *dest, const BITS *source, BITNUM numbits)
{
	memcpy(dest, source, BitsToBytes(numbits));
	return dest;
}

inline short GetBit(const BITS *pBit, BITNUM idx)
{
	return (pBit[idx >> LogBitsPerWord] >> (idx & BitsPerWordMask)) & 1;
}

// NOTE: return value must fit in a word!
inline BITS GetBits(const BITS *pBit, BITNUM from, BITNUM numbits)
{
	assert(numbits <= BitsPerWord);
	const BITS *pWord = &pBit[from >> LogBitsPerWord]; // pointer to start int
	BITNUM unused = from &  BitsPerWordMask; // number of bits not used from start int
	BITNUM used = BitsPerWord - unused;
	// shift off unused bits and mask off useful bits
	BITS result = ((*pWord++) >> unused) & bitMask[used];
	int rest = numbits - used;
	if (rest <= 0)	// if we've masked off too many bits
		result &= bitMask[numbits]; // mask off correct amount
	else
		result |= (*pWord & bitMask[rest]) << used;
	return result;
}

inline void ClearBit(BITS *pBit, BITNUM idx)
{ pBit[idx >> LogBitsPerWord] &= notSingleBitMask[idx & BitsPerWordMask]; }

inline void ClearBits(BITS *pBit, BITNUM numbits)
{ memset((char *)pBit, 0x00, BitsToBytes(numbits)); }

inline void SetBit(BITS *pBit, BITNUM idx)
{ pBit[idx >> LogBitsPerWord] |= singleBitMask[idx & BitsPerWordMask]; }

inline void SetBits(BITS *pBit, BITNUM numbits)
{ memset((char *)pBit, 0xFF, BitsToBytes(numbits)); }

inline BITS *SetBits(BITS *pBit, BITNUM from, BITNUM numbits, BITS value)
{
	BITS *pWord = &pBit[from >> LogBitsPerWord]; // pointer to start int
	BITNUM unset = from & BitsPerWordMask;
	BITNUM set = BitsPerWord - unset;
	int overlap = (numbits - set);
	if (overlap > 0)
	{
		*pWord = (*pWord & bitMask[unset]) | (value << unset);
		pWord++;
		*pWord = ((*pWord & notBitMask[overlap])|
		          ((value >> set) & bitMask[overlap]));
	}
	else if (overlap < 0)
      		*pWord = ((*pWord & bitMask[unset]) |
		          (*pWord & (bitMask[-overlap] << (unset + numbits))) |
		          ((value & bitMask[numbits]) << unset));
	else
		*pWord = ((*pWord & bitMask[unset]) |
		          ((value & bitMask[numbits]) << unset));
	return pBit;
}
  
// Number of bits set 
// NOTE: Following is Edited by Ray
//       Added GCC intrinsics popcount
inline int NumSet(const BITS *pBit, BITNUM numBits)
{
	int count = 0;
	int numWords = BitsToWords(numBits);
	for (int i=0; i<numWords; i++)
	{
		int bitsTested = 0;
		for (BITS bits = pBit[i]; (bits!=0) && (numBits>0); numBits--)
		{
			count += bits % 2;
			bits = bits >> 1;
			bitsTested++;
		}
		numBits -= (BitsPerWord - bitsTested);
	}

	return count;
}

class BitArray {
	public:
  // Constructors
	BitArray(BITNUM sz=0);
	BitArray(const BitArray &ba);
	BitArray(const char *pBitString);
	BitArray(const BITS *words, int numbits);
	BitArray(BITS *words, int numbits)              // grabs pointer !!
		: pBit(words), allocated(0), sz(numbits), fNumSetBits(0), fNumSetIsValid(false) {}
	~BitArray() { if (allocated) delete [] pBit; }

	const BITS * getPtr(void) const { return pBit; }
  
	BitArray &operator=(const BitArray &ba);
	BitArray &operator=(const char *pBitString);
 
	// Move Constructor
	BitArray( BitArray && ba );
	BitArray &operator=(BitArray && ba);

	// For Set<BitArray>
	bool operator<(const BitArray & rhs) const
	{
		if (operator==(rhs)) return false; 
		if (sz < rhs.sz) return false; if (sz > rhs.sz) return true; 
		return (memcmp(pBit, rhs.pBit, Bytes()) < 0);
	}
	int operator==(const BitArray &ba) const
	{ return (sz == ba.sz) && (memcmp(pBit, ba.pBit, Bytes()) == 0); }

	// useful comparison operators, interpreting the BitArray as an indicator function for set membership
	bool isSubset(const BitArray & ba) const
	{ return (sz == ba.sz) && (NumSet() == (ba & (*this)).NumSet()); }

	bool isSuperset(const BitArray & ba) const
	{ return (sz == ba.sz) && (ba.NumSet() == (ba & (*this)).NumSet()); }

	// Append operator
	BitArray &operator<<(const BitArray &ba);
	const BitArray &operator>>(BitArray &ba) const { ba << *this; return *this;}

	// Without this, there's a horrible mistake that's hard to
	// notice: ba[i] -> ((BITS *)ba)[i], which is not what I expect
	short operator[](BITNUM from) const { return Get(from); }

	// I/O operators
	void Read(istream& is);    // binary I/O (size not written)
	void Write(ostream& os) const;
	friend inline ostream &operator<<(ostream &os, const BitArray &ba);
	friend inline istream &operator>>(istream &is, const BitArray &ba);

	// BitArray maintenance functions
	void Grow(BITNUM newsize,int copy=1);
	unsigned int Size() const { return sz; }
	unsigned int size() const { return sz; }
	unsigned int Words() const { return BitsToWords(sz); }
	unsigned int Bytes() const { return BitsToBytes(sz); }

	// Bit accessor functions
	short Get(BITNUM from) const
	{ assert(from < sz); return GetBit(pBit, from); }

	// Note that return value must fit in a word !
	BITS Get(BITNUM from, BITNUM numbits) const
	{
		assert(from < sz);  
		assert(numbits <= BitsPerWord);
		if (numbits+from > sz) numbits = sz - from;
		return GetBits(pBit, from, numbits);
	}

	void Clear() { ::ClearBits(pBit, Bytes()<<LogBitsPerByte); fNumSetBits=0; fNumSetIsValid = true; }
	void Clear(BITNUM from) { assert(from < sz); if (fNumSetIsValid && GetBit(pBit, from)) fNumSetBits--; ClearBit(pBit, from); }

	void Sett()   { ::SetBits(pBit, sz); fNumSetBits = sz; fNumSetIsValid = true; }
	void Sett(BITNUM from) { assert(from < sz);  if (fNumSetIsValid && ! GetBit(pBit, from)) fNumSetBits++; SetBit(pBit, from);}
	void Sett(BITNUM from, BITNUM numbits, BITS value)  
	{
		assert(from < sz); // from >= 0 because it's unsigned
		assert(numbits <= BitsPerWord);
		if (numbits+from > sz) Grow(numbits+from);
		SetBits(pBit, from, numbits, value);
		fNumSetIsValid = false; 
	}
	void Sett(BITNUM from, const BitArray & rhs);

	// NOTE: following is implemented by Ray (yren2@vbi.vt.edu)
	// Please let me know if you find bugs
	unsigned int NumWords(void) const;
	const BITS & GetBitsAt(unsigned int ) const;
	unsigned int NumSetLT(void) const;
	unsigned int NumSetGNUC(void) const;

	BitArray & operator &=( const BitArray & rhs );
	BitArray & operator |=( const BitArray & rhs );
	BitArray & operator ^=( const BitArray & rhs );
	BitArray   operator & ( const BitArray & opnd2 ) const;
	BitArray   operator | ( const BitArray & opnd2 ) const;
	BitArray   operator ^ ( const BitArray & opnd2 ) const;
	BitArray & operator ~ ( void );       // in place negate
	BitArray   operator ! ( void ) const; // negate

	// BitSet Index Iterator, iterating through all bits set and return the 
	// It is an InputIterator, namely only increment and de-reference is supported
	class setBitIdx_iterator{
		public:
		setBitIdx_iterator(void){};
		setBitIdx_iterator(BITNUM widx, short sidx, short digits, const BitArray * brptr);
		setBitIdx_iterator & operator++(); // prefix
		setBitIdx_iterator operator++(int); // postfix
		bool operator == (const setBitIdx_iterator & rhs ) const;
		bool operator != (const setBitIdx_iterator & rhs ) const;
		BITNUM operator *() const ; 
		void disp( void ) const {
		    clog << this-> word_idx << '\t'  << this->seg_idx << '\t' <<
		        *(this -> idxptr) << endl;
		    clog << digits << '\t' << v << endl;
		}
		protected: 
		BITNUM           word_idx;   
		short            seg_idx;
		short            digits;       // a nibble 0 to 15, = v & 0xF
		const BitArray * bitarray_ptr; // access to the BitArray data
		const BITNUM   * idxptr;       // index pointer
		unsigned int     v;            // a temp unsigned int (word) for shifting
	};

	setBitIdx_iterator setBitIdx_begin(void) const;
	setBitIdx_iterator setBitIdx_end(void)   const;

	bool nextCombination(void);  // next combination with the same number set as the input
	bool nextDepthFirstCombination(long maxDepth = -1);

	// return total number of bits set
	BITNUM NumSet(void) const 
	{
		if (fNumSetIsValid){
			return fNumSetBits;
		} else {
			fNumSetBits = ::NumSet(this->pBit, this->sz);
			this->fNumSetIsValid = true;
			return fNumSetBits;
		}
	}

	// FIX optimize these functions, like Ray did for NumSet
	// return number of bits set in the range [start, end)
	BITNUM NumSet(long start, long end) const
	{BITNUM count=0; for (BITNUM i=start; i<end; i++) { if (Get(i)) count++; } return count;};

	BITNUM LastBitSet(void) const
	{if (NumSet() == 0) return -1; for (BITNUM i=sz-1; i>=1; i--) { if (Get(i)) return i;} return 0;};

	BITNUM LastBitClear(void) const
	{if (NumSet() == sz) return -1; for (BITNUM i=sz-1; i>=1; i--) { if (! Get(i)) return i;} return 0;};

	bool Intersects(const BitArray &ba) const;
	void Print(ostream &os) const;

private:
	BITS *pBit;
	unsigned int allocated;
	BITNUM sz;
	mutable BITNUM fNumSetBits;
	mutable bool fNumSetIsValid;
};	      

inline ostream &operator<<(ostream &os, const BitArray &ba)
// { os.write((char *)ba.pBit, BitsToBytes(ba.sz)); return os; }
{ ba.Print(os); return os;}

inline istream &operator>>(istream &is, const BitArray &ba)
{ is.read((char *)ba.pBit, BitsToBytes(ba.sz)); return is; }

#endif
