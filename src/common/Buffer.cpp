#include "Buffer.h"
#include "NetStatistics.h"

namespace XNet
{
    
    
    Buffer::Buffer(unsigned int capacity)
    {
        _capacity = capacity;
        NetStatistics::bufferSize.fetch_add(_capacity);
    }
    
    Buffer::~Buffer()
    {
        NetStatistics::bufferSize.fetch_sub(_capacity);
    }
    
    BufferPtr Buffer::create(unsigned int capacity)
    {
        char* p = new char[sizeof(Buffer) + capacity];
        Buffer* buf = new (p) Buffer(capacity);
        return BufferPtr(buf);
    }
    
    BufferPtr Buffer::create(const char* data, unsigned int size, unsigned int capacity)
    {
        capacity = std::max(size, capacity);
        BufferPtr buf = create(capacity);
        buf->append(data, size);
        return buf;
    }
        
    bool Buffer::append(const char* data, unsigned int size)
    {
        if (_size + size > _capacity)
        {
            return false;
        }
        memcpy(_data + _size, data, size);
        _size += size;
        return true;
    }
        
    bool Buffer::append(unsigned int size)
    {
        if (_size + size > _capacity)
        {
            return false;
        }
        _size += size;
        return true;
    }
        
    const char* Buffer::read(unsigned int size)
    {
        if (_size - _readPos < size)
        {
            return NULL;
        }
        const char* p = _data + _readPos;
        _readPos += size;
        return p;
    }
        
    char* Buffer::data()
    {
        return _data + _readPos;
    }

    const char* Buffer::data() const
    {
        return _data + _readPos;
    }
        
    unsigned int Buffer::size() const
    {
        return _size - _readPos;
    }
        
    unsigned int Buffer::capacity() const
    {
        return _capacity;
    }

    void Buffer::reset()
    {
        _readPos = 0;
        _size = 0;
    }

    bool Buffer::empty() const
    {
        return _size == 0;
    }
    
} // namespace XNet
