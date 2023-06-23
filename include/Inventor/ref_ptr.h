#ifndef COIN_REF_PTR_H
#define COIN_REF_PTR_H

template<typename T>
class ref_ptr
{
public:
    ref_ptr()
    {
    }
    ref_ptr(T* ptr)
        : _ptr(ptr)
    {
        if (_ptr)
            _ptr->ref();
    }

    ref_ptr(const ref_ptr& rp)
        : _ptr(rp._ptr)
    {
        if (_ptr)
            _ptr->ref();
    }
    template <class Other>
    ref_ptr(const ref_ptr<Other>& rp)
    {
        _ptr = static_cast<T*>(rp._ptr);
        if (_ptr)
            _ptr->ref();
    }
    ~ref_ptr()
    {
        if (_ptr)
            _ptr->unref();
        _ptr = 0;
    }
    ref_ptr& operator=(const ref_ptr& rp)
    {
        assign(rp);
        return *this;
    }

    template <class Other>
    ref_ptr& operator=(const ref_ptr<Other>& rp)
    {
        assign(rp);
        return *this;
    }
    inline ref_ptr& operator=(T* ptr)
    {
        if (_ptr == ptr)
            return *this;
        T* tmp_ptr = _ptr;
        _ptr = ptr;
        if (_ptr)
            _ptr->ref();
        if (tmp_ptr)
            tmp_ptr->unref();
        return *this;
    }
    inline ref_ptr& operator=(const T* ptr)
    {
        if (_ptr == ptr)
            return *this;
        T* tmp_ptr = _ptr;
        _ptr = (T*)ptr;
        if (_ptr)
            _ptr->ref();
        if (tmp_ptr)
            tmp_ptr->unref();
        return *this;
    }
    operator T* () const
    {
        return _ptr;
    }
    T** operator&()
    {
        return &_ptr;
    }
    T* operator->() const
    {
        return _ptr;
    }
    T* get() const
    {
        return _ptr;
    }
    bool operator!() const
    {
        return _ptr == 0;
    }
    bool valid() const
    {
        return _ptr != 0;
    }
    T* release()
    {
        T* tmp = _ptr;
        if (_ptr)
            _ptr->unref();
        _ptr = 0;
        return tmp;
    }

    bool operator<(const ref_ptr& rp) const
    {
        return (_ptr < rp._ptr);
    }
    bool operator==(const ref_ptr& rp) const
    {
        return (_ptr == rp._ptr);
    }
    bool operator==(const T* ptr) const
    {
        return (_ptr == ptr);
    }
    bool operator!=(const ref_ptr& rp) const
    {
        return (_ptr != rp._ptr);
    }
    bool operator!=(const T* ptr) const
    {
        return (_ptr != ptr);
    }

private:
    template <class Other>
    void assign(const ref_ptr<Other>& rp)
    {
        if (_ptr == rp._ptr)
            return;
        T* tmp_ptr = _ptr;
        _ptr = rp._ptr;
        if (_ptr)
            _ptr->ref();
        if (tmp_ptr)
            tmp_ptr->unref();
    }
    template <class Other>
    friend class ref_ptr;
    T* _ptr = nullptr;
};

#endif