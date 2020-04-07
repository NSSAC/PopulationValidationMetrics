#include "BitArray.h"

using namespace std;

bool BitArray::Intersects(const BitArray &ba) const
{
  int numWords = (Size() < ba.Size()) ? Words() : ba.Words();
  int bit = 0;
  for (int word = 0; word<numWords-1; word++, bit+=BitsPerWord)
    if ((pBit[word] != 0) || (ba.pBit[word] != 0))
      for (int i=0; i<BitsPerWord; i++)
	if (Get(bit+i) && ba.Get(bit+i))
	  return true;
  for (;(unsigned)bit < sz; bit++)
    if (Get(bit) && ba.Get(bit))
      return true;
  return false;
}

void BitArray::Print(ostream &os) const
{
  for (BITNUM b=0; b<sz; ++b) 
    os << (Get(b) ? '1' : '0');
  // os << endl;
}

BitArray::BitArray(BITNUM size) 
  : pBit(0), allocated(0), fNumSetBits(0), fNumSetIsValid(true)
{
  sz = size;
  if (sz == 0)
	return;

  allocated = BitsToWords(sz);
  pBit = new BITS[allocated];
  Clear();
}

BitArray::BitArray(const BitArray &ba) 
  : pBit(new BITS[BitsToWords(ba.sz)])
{
  sz = ba.sz;
  allocated = BitsToWords(sz);
  CopyBits(pBit, ba.pBit, sz);
  fNumSetIsValid = ba.fNumSetIsValid;
  fNumSetBits = ba.fNumSetBits;
}

// can't steal pointer, because it's const
BitArray::BitArray(const BITS * words, int numbits) 
    : pBit(0), allocated(0), sz(numbits) 
{
	allocated = BitsToWords(sz);
	pBit = new BITS[allocated];
	CopyBits(pBit, words, sz);
	fNumSetIsValid = false;
	fNumSetBits = 0;
}

BitArray::BitArray(const char *pBitString) 
  : pBit(NULL), allocated(0), sz(0)
{
	(*this) = pBitString;
	fNumSetIsValid = false;
	fNumSetBits = 0;
}

BitArray &BitArray::operator=(const BitArray &ba)
{
	Grow(ba.sz);
	allocated = BitsToWords(sz);
	CopyBits(pBit, ba.pBit, sz);
	fNumSetIsValid = ba.fNumSetIsValid;
	fNumSetBits = ba.fNumSetBits;
	return *this;
}

BitArray &BitArray::operator=(const char *pBitString)
{
	if (allocated)
		delete [] pBit;
	sz = strlen(pBitString);
	allocated = BitsToWords(sz);
	pBit = new BITS[allocated];
	fNumSetBits = 0;
	for (int i=0; (unsigned)i<sz; i++)
	{
		if (pBitString[i] == '0')
		{
			Clear(i);
		}
		else if (pBitString[i] == '1')
		{
			Sett(i);
			fNumSetBits++;
		}
		else
		{
			cerr << "ERROR: Bad bit " << pBitString[i] << " in bitstring "
	     	     	     << pBitString << endl;
			Clear(i);
		}
	}
	fNumSetIsValid = true;
	return *this;
}

// <NOTE: Added by Ray (yren2@vbi.vt.edu). 
// using rval-reference. i.e. BitArray foo ( BitArray() ); 
//                        or, BitArray foo = std::move( bar );
BitArray::BitArray( BitArray && ba )
{
	pBit = ba.pBit; // move the ownership of the allocated data to this->pBit. 
	sz = ba.sz;
	allocated = ba.allocated;  // BitsToWords(sz);
	ba.allocated = 0; // prevent ba to deallocate heap
	ba.sz = 0;
	ba.pBit = NULL;
	fNumSetBits = ba.fNumSetBits; // add by Ray
	fNumSetIsValid = ba.fNumSetIsValid;
}

BitArray & BitArray::operator=(BitArray && ba)
{
	if (allocated) delete [] pBit;
	pBit = ba.pBit; // move the ownership of the allocated data to this->pBit. 
	sz = ba.sz;
	allocated = ba.allocated;
	ba.allocated = 0; // prevent ba to deallocate heap
	ba.sz = 0;
	ba.pBit = NULL;

	fNumSetBits = ba.fNumSetBits; // add by Ray
	fNumSetIsValid = ba.fNumSetIsValid;

	return *this;
}
// END NOTE>

void BitArray::Sett(BITNUM from, const BitArray & rhs)
{  
	assert (from + rhs.sz <= sz);

	if (rhs.sz == 0)
		return;
	BITNUM alignedEnd = (BitsToWords(rhs.sz)-1) * BitsPerWord;
	BITNUM bn = 0;
	for (bn=0; bn<alignedEnd; bn += BitsPerWord)
		Sett(from+bn, BitsPerWord, rhs.Get(bn, BitsPerWord));
	Sett(from+bn, (rhs.sz-bn), rhs.Get(bn,(rhs.sz-bn)));
	fNumSetIsValid = false;
}

void BitArray::Grow(BITNUM newsize, int copy)
{
	unsigned int newalloc = BitsToWords(newsize);
	if (newalloc > allocated)
	{
		unsigned int oldalloc = allocated;
		unsigned int oldsize = sz;
		BITS *old = pBit;
		pBit = new BITS[newalloc];
		allocated = newalloc;
		sz = newsize;	      // so subsequent Clear clears all new bits
		Clear();
		if (copy && oldsize) CopyBits(pBit, old, oldsize);
		if (oldalloc) delete [] old;
	}
	else
	{
		sz = newsize;
	}
}


// NOTE: following is implemented by Ray (yren2@vbi.vt.edu)
// Please let me know if you find bugs
unsigned int BitArray::NumWords(void) const { return this->allocated ; }
const BITS & BitArray::GetBitsAt(unsigned int i) const { return this->pBit[i]; }
unsigned int BitArray::NumSetLT(void) const {
    unsigned int cnt = 0;
    for( unsigned int i = 0 ; i < this->NumWords(); ++i ){
        const unsigned char * p = (unsigned char *) &this->pBit[i];
        cnt += BitsSetTable256[p[0]] + 
               BitsSetTable256[p[1]] + 
               BitsSetTable256[p[2]] + 
               BitsSetTable256[p[3]];
    }
    return cnt;
}

unsigned int BitArray::NumSetGNUC(void) const {
#if __GNUC__ > 3 
    unsigned int count = 0;
    // clog << " in GNUC > 3, numBits = " << numWords << " \n " << endl;
    for( unsigned int i = 0; i < NumWords(); ++i ){
        count +=  __builtin_popcount( pBit[i] ) ;
    }
    return count ; 
#else
    return 0;
#endif
}

BitArray & BitArray::operator &= ( const BitArray & rhs ){
	assert( this->NumWords()  == rhs.NumWords() );
	for( unsigned int i = 0 ; i < this->NumWords(); ++i ){
		this->pBit[i] &= rhs.GetBitsAt(i);
	}
  	fNumSetIsValid = false;
	return *this;
}

BitArray & BitArray::operator |= ( const BitArray & rhs ){
	assert( this->NumWords()  == rhs.NumWords() );
	for( unsigned int i = 0 ; i < this->NumWords(); ++i ){
		this->pBit[i] |= rhs.GetBitsAt(i);
	}
  	fNumSetIsValid = false;
	return *this;
}

BitArray & BitArray::operator ^= ( const BitArray & rhs ){
	assert( this->NumWords()  == rhs.NumWords() );
	for( unsigned int i = 0 ; i < this->NumWords(); ++i ){
		this->pBit[i] ^= rhs.GetBitsAt(i);
	}
	fNumSetIsValid = false;
	return *this;
}


BitArray BitArray::operator &( const BitArray & opnd2 ) const {
    BitArray rnt(*this);
    return rnt &= opnd2;
}

BitArray BitArray::operator |( const BitArray & opnd2 ) const {
    BitArray rnt(*this);
    return rnt |= opnd2;
}

BitArray BitArray::operator ^( const BitArray & opnd2 ) const {
    BitArray rnt(*this);
    return rnt ^= opnd2;
}


BitArray & BitArray::operator ~( void ){
	for( unsigned int i = 0; i < this->NumWords(); ++i ){
		this->pBit[i] = ~(this->pBit[i]);
	}
	if (fNumSetIsValid)
		fNumSetBits = sz - fNumSetBits;
	return *this;
}

BitArray  BitArray::operator !( void ) const {
    BitArray rnt(*this);
    return ~rnt;
}


/*
bool BitArray::setBitIdx_iterator::fill_word( BITNUM & word_idx, const BITS * pBit ){
    unsigned int internal_idx = 0;
    unsigned int v = pBit[word_idx];
    // naive implementation for now
    if( v != 0 ){
        for( short i = 0; i < BitsPerWord ; ++ i ){
            if( v & singleBitMask[i] ){
                temp_idx[internal_idx++] = i ;
            }
        }
        this->word_idx = word_idx;
        this->idxptr = &temp_idx[0];
        return true;
    }else{
        return false;
    }
}
*/

//  ===== BitArray::setBitIdx_iterator =====
//

BitArray::setBitIdx_iterator::setBitIdx_iterator(BITNUM widx, short sidx, 
        short digits, const BitArray * brptr) : 
    word_idx(widx), seg_idx(sidx), digits(digits), bitarray_ptr(brptr) {
    this -> v = brptr->GetBitsAt( widx );
    for( short i = 0 ; i < seg_idx ; ++ i ) this->v >>= 4;
   //  clog << " v = " << v << " digits - 1 = " << ( digits - 1 )  << endl;
   //  clog << " setIdxTable[ initIdxTable[ digits - 1 ] ] = " << 
        //setIdxTable[ initIdxTable[ digits - 1 ] ] << endl;
    this -> idxptr = &setIdxTable[ initIdxTable[ digits - 1 ] ];
};

BitArray::setBitIdx_iterator BitArray::setBitIdx_begin(void) const{
    BITNUM word_idx = 0 ;
    // clog << " this->pBit[" << word_idx << "] = " << this->pBit[word_idx] << endl;
    while( word_idx < this->NumWords() && this->pBit[word_idx] == 0 )  ++word_idx;
    // clog << " this->pBit[" << word_idx<< "] = " << this->pBit[word_idx] << endl;
    if( word_idx < this->NumWords() ){
       short seg_idx = 0;
       unsigned int v = pBit[word_idx];
       // shift 4 bits
       // clog  << " v&0xF = " << (v & 0xF) << endl;
       while( seg_idx < 8 &&  (v & 0xF) == 0 ){
           seg_idx ++;
           v >>= 4;
       }
       // clog  << " v&0xF = " << (v & 0xF) << endl;
       return setBitIdx_iterator( word_idx, seg_idx, v&0xF, this );
    }else{
        return setBitIdx_iterator( word_idx, 8, 16, this );
    }
}
        
BitArray::setBitIdx_iterator BitArray::setBitIdx_end(void) const{
    return setBitIdx_iterator( this->NumWords(), 8, 16, this );
}
    
bool BitArray::setBitIdx_iterator::operator == (const setBitIdx_iterator & rhs) const {
    return word_idx == rhs.word_idx && 
        seg_idx == rhs.seg_idx && 
        idxptr == rhs.idxptr;
}

bool BitArray::setBitIdx_iterator::operator != (const setBitIdx_iterator & rhs) const {
    return  ! ( *this == rhs ) ;
}

BITNUM BitArray::setBitIdx_iterator::operator * ( void ) const{
  //  clog << " word_idx = " << word_idx << " seg_idx = " 
  //      << seg_idx << " *idxptr = " << *idxptr << endl;
    return BitsPerWord * word_idx + 4*seg_idx + *idxptr;
}

BitArray::setBitIdx_iterator & BitArray::setBitIdx_iterator::operator ++ ( void ){
    // move ptr in index table
    // clog << " operator ++ " << '\t';
    // clog << " digits = " << digits << '\t' ;
    // clog << (this->idxptr) << '\t';
    // clog << (& setIdxTable[ initIdxTable[ digits ] ]) << endl;
    ++ idxptr;
    if( this->idxptr != (& setIdxTable[ initIdxTable[ digits ] ]) ){
        // clog << " moving idxptr " << endl;
        return *this;
    }

    // otherwise move to the next segment
    do{
        seg_idx ++;
        v >>= 4;
    }while( (seg_idx < 8) && ( (v & 0xF) == 0) );
    if( seg_idx < 8 ){ // found a non-zero segment
        // clog << " moving seg " << endl;
        this -> digits = (v & 0xF);
        this -> idxptr = &setIdxTable[ initIdxTable[ digits - 1 ] ];
        return *this;
    }

    // otherwise move the next word
    do{
        word_idx ++;
    }while( word_idx < bitarray_ptr->NumWords() && bitarray_ptr->GetBitsAt(word_idx) == 0 );
    if( word_idx < bitarray_ptr->NumWords() ){
        // clog << " moving word " << endl;
        seg_idx = 0;
        this->v = bitarray_ptr->GetBitsAt(word_idx);
        while( (v & 0xF)  == 0 ){
            seg_idx ++;
            v >>= 4;
        }
        this -> digits = (v & 0xF);
        this -> idxptr = &setIdxTable[ initIdxTable[ digits - 1] ];
        return *this;
    }else{
        // clog << " moving to end " << endl;
        this -> idxptr = &setIdxTable[ initIdxTable[ 15 ] ];
        return *this;
    }
}

// End Note. -- Ray

// Returns the next combination that has the same number of elements as the input
bool BitArray::nextCombination(void)
{
	const long sz = Size();
	const long numSet = NumSet();
	fNumSetIsValid = false;

	if ((sz == 0) || (numSet == 0) || (numSet == sz))
	{
		Clear();
		return false;
	}

	// at least one bit is set and one is clear

	long lastBitSet = LastBitSet();
	Clear(lastBitSet);
	if (lastBitSet < sz-1)
	{
		Sett(lastBitSet + 1);
		return true;
	}
	long numToSet = 1;

	long firstBitClear = lastBitSet-1;
	while (firstBitClear >= 0 && Get(firstBitClear))
	{
		Clear(firstBitClear);
		numToSet++;
		firstBitClear--;
	}

	if (numToSet == numSet)  // all bits were packed at right end
	{
		Clear();
		return false;
	}

	// at least one bit is set in [0,firstBitClear-1]
	lastBitSet = firstBitClear-1;
	while (! Get(lastBitSet))
	{
		lastBitSet--;
	}
	Clear(lastBitSet);
	numToSet++;

	for (long i = 0; i<numToSet; i++)
	{
		lastBitSet++;
		Sett(lastBitSet);
	}
		
	return true;
}

// Returns the next combination in the order (1, 0,0,...) (1,1,0,...), (1,1,1,...)
bool BitArray::nextDepthFirstCombination(long maxDepth)
{
	// clog << "In depth " << maxDepth << " bits ";
	// Print(clog);
	if (sz == 0 || maxDepth == 0)
		return false;

	if (maxDepth == -1)
	{
		maxDepth = sz;
	}
	const long numSet = NumSet();
	long lastBitSet = LastBitSet();

	// This seems like an absurd number of special cases, but I think they're all necessary
	if (numSet == 0)
	{
		Sett(0);
		// clog << "Next ";
		// Print(clog);
		return true;
	}

	if (lastBitSet == sz - 1) 
	{
		if (numSet == 1)
		{
			return false;
		}

		Clear(lastBitSet);   // pop up one level
		lastBitSet = LastBitSet();
		Clear(lastBitSet);   
		Sett(lastBitSet+1);
		// clog << "Next ";
		// Print(clog);
		return true;
	}

	if (numSet == maxDepth)
	{
		Clear(lastBitSet);   
	}

	// lastBitSet < sz-1 && numSet < maxDepth
	Sett(lastBitSet+1);
	// clog << "Next ";
	// Print(clog);
		return true;
}
 
BitArray &BitArray::operator<<(const BitArray &ba)
{
  if (ba.sz == 0)
    return *this;
  BITNUM start = sz;
  Grow(sz+ba.sz);
  BITNUM alignedEnd = (BitsToWords(ba.sz)-1) * BitsPerWord;
  BITNUM bn = 0;
  for (bn=0; bn<alignedEnd; bn += BitsPerWord)
    Sett(start+bn, BitsPerWord, ba.Get(bn, BitsPerWord));
  Sett(start+bn, (ba.sz-bn), ba.Get(bn,(ba.sz-bn)));
	fNumSetIsValid = false;
  return *this;
}

void BitArray::Read(istream & is)
{
  is.read((char *) pBit, BitsToBytes(sz));
	fNumSetIsValid = false;
}

void BitArray::Write(ostream & os) const
{
  os.write((char *) pBit, BitsToBytes(sz));
}

// Bitmasks for bit operations
BITS notSingleBitMask[BitsPerWord] =
{
  (BITS) ~0x1, (BITS) ~0x2, (BITS) ~0x4, (BITS) ~0x8,
  (BITS) ~0x10, (BITS) ~0x20, (BITS) ~0x40, (BITS) ~0x80,
  (BITS) ~0x100, (BITS) ~0x200, (BITS) ~0x400, (BITS) ~0x800,
  (BITS) ~0x1000, (BITS) ~0x2000, (BITS) ~0x4000, (BITS) ~0x8000,
  (BITS) ~0x10000, (BITS) ~0x20000, (BITS) ~0x40000, (BITS) ~0x80000,
  (BITS) ~0x100000, (BITS) ~0x200000, (BITS) ~0x400000, (BITS) ~0x800000,
  (BITS) ~0x1000000, (BITS) ~0x2000000, (BITS) ~0x4000000, (BITS) ~0x8000000,
  (BITS) ~0x10000000, (BITS) ~0x20000000, (BITS) ~0x40000000, (BITS) ~0x80000000
  };

BITS singleBitMask[BitsPerWord] =
{
  0x1, 0x2, 0x4, 0x8,
  0x10, 0x20, 0x40, 0x80,
  0x100, 0x200, 0x400, 0x800,
  0x1000, 0x2000, 0x4000, 0x8000,
  0x10000, 0x20000, 0x40000, 0x80000,
  0x100000, 0x200000, 0x400000, 0x800000,
  0x1000000, 0x2000000, 0x4000000, 0x8000000,
  0x10000000, 0x20000000, 0x40000000, 0x80000000
  };

BITS reverseBitMask[BitsPerWord] =
{
  0xFFFFFFFF, 0x7FFFFFFF, 0x3FFFFFFF, 0x1FFFFFFF,
  0xFFFFFFF,  0x7FFFFFF,  0x3FFFFFF, 0x1FFFFFF,
  0xFFFFFF,   0x7FFFFF,   0x3FFFFF, 0x1FFFFF,
  0xFFFFF,    0x7FFFF,    0x3FFFF, 0x1FFFF,
  0xFFFF,     0x7FFF,     0x3FFF, 0x1FFF,
  0xFFF,      0x7FF,      0x3FF, 0x1FF,
  0xFF,       0x7F,       0x3F, 0x1F,
  0xF,        0x7,        0x3, 0x1,
  };

BITS bitMask[BitsPerWord+1] = {
  0,0x1, 0x3, 0x7, 0xF,
  0x1F, 0x3F, 0x7F, 0xFF,
  0x1FF, 0x3FF, 0x7FF, 0xFFF,
  0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF,
  0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF,
  0x1FFFFF, 0x3FFFFF, 0x7FFFFF, 0xFFFFFF,
  0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF, 0xFFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF
  };

BITS notBitMask[BitsPerWord+1] = {
  (BITS) ~0x0,
  (BITS) ~0x1, (BITS) ~0x3, (BITS) ~0x7, (BITS) ~0xF,
  (BITS) ~0x1F, (BITS) ~0x3F, (BITS) ~0x7F, (BITS) ~0xFF,
  (BITS) ~0x1FF, (BITS) ~0x3FF, (BITS) ~0x7FF, (BITS) ~0xFFF,
  (BITS) ~0x1FFF, (BITS) ~0x3FFF, (BITS) ~0x7FFF, (BITS) ~0xFFFF,
  (BITS) ~0x1FFFF, (BITS) ~0x3FFFF, (BITS) ~0x7FFFF, (BITS) ~0xFFFFF,
  (BITS) ~0x1FFFFF, (BITS) ~0x3FFFFF, (BITS) ~0x7FFFFF, (BITS) ~0xFFFFFF,
  (BITS) ~0x1FFFFFF, (BITS) ~0x3FFFFFF, (BITS) ~0x7FFFFFF, (BITS) ~0xFFFFFFF,
  (BITS) ~0x1FFFFFFF, (BITS) ~0x3FFFFFFF, (BITS) ~0x7FFFFFFF, (BITS) ~0xFFFFFFFF
  };
