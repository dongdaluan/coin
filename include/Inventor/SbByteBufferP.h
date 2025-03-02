#ifndef COIN_SBBYTEBUFFERP_ICC
#define COIN_SBBYTEBUFFERP_ICC
#define COIN_ICC_INCLUDE
#include "SbByteBuffer.h"
#undef COIN_ICC_INCLUDE

#ifdef ABI_BREAKING_OPTIMIZE
#define PRIVATE(X) (X)
#define PRIVATE_STATIC SbByteBuffer
#define INLINE inline
#define PIMPL_IMPLEMENTATION
#else
#define PRIVATE(X) (X)->pimpl
#define PRIVATE_STATIC SbByteBufferP
#define INLINE
#endif

class SbByteBufferP {
	friend class SbByteBuffer;
	SbByteBufferP(size_t size_in)
		: size_(size_in),
		buffer(size_in ? std::shared_ptr<char[]>(new char[size_in]) : std::shared_ptr<char[]>()),
		invalid(FALSE)
	{
	}
	SBBYTEBUFFER_PRIVATE_VARIABLES
};

INLINE void
SbByteBuffer::makeUnique()
{
	if (PRIVATE(this)->size_ && !PRIVATE(this)->buffer.unique()) {
		std::shared_ptr<char[]> tmp_buffer(new char[PRIVATE(this)->size_]);
		memcpy(tmp_buffer.get(), PRIVATE(this)->buffer.get(), PRIVATE(this)->size_);
		PRIVATE(this)->buffer = tmp_buffer;
	}
}

INLINE SbByteBuffer::SbByteBuffer(size_t size_in, const unsigned char* buffer_in)
#ifdef ABI_BREAKING_OPTIMIZE
	: size_(size_in),
	buffer(size_in ? boost::shared_array<char>(new char[size_in]) : boost::shared_array<char>()),
	invalid(FALSE)
#else
	: pimpl(new SbByteBufferP(size_in))
#endif
{
	if (buffer_in)
		memcpy(PRIVATE(this)->buffer.get(), buffer_in, size_in);
}

INLINE SbByteBuffer::SbByteBuffer(const SbByteBuffer& buffer)
#ifdef ABI_BREAKING_OPTIMIZE
	: size_(buffer.size_),
	buffer(buffer.buffer),
	invalid(buffer.invalid)
#else
	: pimpl(new SbByteBufferP(buffer.size()))
#endif
{
#ifndef ABI_BREAKING_OPTIMIZE
	PRIVATE(this)->size_ = PRIVATE(&buffer)->size_;
	PRIVATE(this)->buffer = PRIVATE(&buffer)->buffer;
	PRIVATE(this)->invalid = PRIVATE(&buffer)->invalid;
#endif  
}

INLINE SbByteBuffer::SbByteBuffer(size_t size_in, const char* buffer_in)
#ifdef ABI_BREAKING_OPTIMIZE
	: size_(size_in),
	buffer(size_in ? boost::shared_array<char>(new char[size_in]) : boost::shared_array<char>()),
	invalid(FALSE)
#else
	: pimpl(new SbByteBufferP(size_in))
#endif
{
	if (buffer_in)
		memcpy(PRIVATE(this)->buffer.get(), buffer_in, size_in);
}

INLINE SbByteBuffer::SbByteBuffer(const char* buffer_in)
#ifdef ABI_BREAKING_OPTIMIZE
	: size_(buffer_in ? strlen(buffer_in) + 1 : 0),
	buffer(size_ ? boost::shared_array<char>(new char[size_]) : boost::shared_array<char>()),
	invalid(FALSE)
#else
	: pimpl(new SbByteBufferP(strlen(buffer_in) + 1))
#endif
{
	if (buffer_in)
		memcpy(PRIVATE(this)->buffer.get(), buffer_in, PRIVATE(this)->size_);
}

INLINE SbByteBuffer::~SbByteBuffer()
{
#ifndef ABI_BREAKING_OPTIMIZE
	delete this->pimpl;
#endif //ABI_BREAKING_OPTIMIZE  
}

INLINE SbBool
SbByteBuffer::isValid() const
{
	return !PRIVATE(this)->invalid;
}

INLINE size_t
SbByteBuffer::size() const
{
	return PRIVATE(this)->size_;
}

INLINE SbBool
SbByteBuffer::empty() const
{
	return size() == 0;
}

INLINE const char&
SbByteBuffer::operator[](size_t idx) const
{
	return this->constData()[idx];
}

INLINE SbByteBuffer&
SbByteBuffer::operator=(const SbByteBuffer& that)
{
	PRIVATE(this)->size_ = PRIVATE(&that)->size_;
	PRIVATE(this)->buffer = PRIVATE(&that)->buffer;
	PRIVATE(this)->invalid = PRIVATE(&that)->invalid;
	return *this;
}


INLINE SbBool
SbByteBuffer::operator==(const SbByteBuffer& that) const
{
	size_t n = this->size();
	if (that.size() != n)
		return FALSE;
	for (int i = (int)n - 1; i >= 0; --i) {
		if (this->constData()[i] != that.constData()[i])
			return FALSE;
	}
	return TRUE;

}

INLINE void
SbByteBuffer::push(const SbByteBuffer& that)
{
	SbByteBuffer n_buf(this->size() + that.size());
	memcpy(n_buf.data(), this->constData(), this->size());
	memcpy(&n_buf.data()[this->size()], that.constData(), that.size());
	*this = n_buf;
}

INLINE const char*
SbByteBuffer::constData() const
{
	return PRIVATE(this)->buffer.get();
}

INLINE char*
SbByteBuffer::data()
{
	this->makeUnique();
	return PRIVATE(this)->buffer.get();
}

INLINE SbByteBuffer&
SbByteBuffer::invalidBuffer()
{
	//FIXME: Do this once BFG 20080219
	PRIVATE(&(PRIVATE_STATIC::invalidBuffer_))->invalid = TRUE;
	return PRIVATE_STATIC::invalidBuffer_;
}

#undef PRIVATE
#undef PRIVATE_STATIC
#undef INLINE

#undef SBBYTEBUFFER_PRIVATE_VARIABLES

#endif // !COIN_SBBYTEBUFFERP_ICC
