#pragma once
#include "CommonPublic.h"

namespace XNet
{
    class Buffer;
    typedef std::shared_ptr<Buffer> BufferPtr;
    class Buffer
    {
    protected:
        Buffer(unsigned int capacity);
    public:
        ~Buffer();

    public:
        static BufferPtr create(unsigned int capacity);
        static BufferPtr create(const char* data, unsigned int size, unsigned int capacity = 0);
        
        bool append(const char* data, unsigned int size);
        bool append(unsigned int size);
        const char* read(unsigned int size);
        void reset();

        char* data();
        const char* data() const;
        unsigned int size() const;
        unsigned int capacity() const;
        bool empty() const;
        
    protected:
        unsigned int _size = 0;
        unsigned int _capacity = 0;
        unsigned int _readPos = 0;
        char _data[1];
    };
} // namespace XNet
