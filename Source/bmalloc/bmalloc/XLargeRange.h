/*
 * Copyright (C) 2016 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#ifndef XLargeRange_h
#define XLargeRange_h

namespace bmalloc {

class XLargeRange : public Range {
public:
    XLargeRange()
        : Range()
        , m_physicalSize(0)
    {
    }

    XLargeRange(const Range& other, size_t physicalSize)
        : Range(other)
        , m_physicalSize(physicalSize)
    {
    }

    XLargeRange(void* begin, size_t size, size_t physicalSize)
        : Range(begin, size)
        , m_physicalSize(physicalSize)
    {
    }

    size_t physicalSize() const { return m_physicalSize; }
    void setPhysicalSize(size_t physicalSize) { m_physicalSize = physicalSize; }

    std::pair<XLargeRange, XLargeRange> split(size_t) const;

    bool operator<(const void* other) const { return begin() < other; }
    bool operator<(const XLargeRange& other) const { return begin() < other.begin(); }

private:
    size_t m_physicalSize;
};

inline bool canMerge(const XLargeRange& a, const XLargeRange& b)
{
    if (a.end() == b.begin())
        return true;
    
    if (b.end() == a.begin())
        return true;
    
    return false;
}

inline XLargeRange merge(const XLargeRange& a, const XLargeRange& b)
{
    const XLargeRange& left = std::min(a, b);
    if (left.size() == left.physicalSize()) {
        return XLargeRange(
            left.begin(),
            a.size() + b.size(),
            a.physicalSize() + b.physicalSize());
    }

    return XLargeRange(
        left.begin(),
        a.size() + b.size(),
        left.physicalSize());
}

inline std::pair<XLargeRange, XLargeRange> XLargeRange::split(size_t size) const
{
    BASSERT(size <= this->size());
    
    if (size <= physicalSize()) {
        XLargeRange left(begin(), size, size);
        XLargeRange right(left.end(), this->size() - size, physicalSize() - size);
        return std::make_pair(left, right);
    }

    XLargeRange left(begin(), size, physicalSize());
    XLargeRange right(left.end(), this->size() - size, 0);
    return std::make_pair(left, right);
}

} // namespace bmalloc

#endif // XLargeRange_h
