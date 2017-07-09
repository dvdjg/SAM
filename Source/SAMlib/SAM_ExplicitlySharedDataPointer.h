#ifndef SAM_EXPLICITLYSHAREDDATAPOINTER_H
#define SAM_EXPLICITLYSHAREDDATAPOINTER_H

#include <QtCore/qglobal.h>
#include <QtCore/qatomic.h>

#include "declare_private.h"

namespace SAM
{
    template <class T> class ExplicitlySharedDataPointer
    {
    public:
        typedef T Type;
        typedef T *pointer;

        inline T &operator*() const { return *d; }
        inline T *operator->() { return d; }
        inline T *operator->() const { return d; }
        inline T *data() const { return d; }
        inline const T *constData() const { return d; }

        //inline void detach() { if (d && d->ref != 1) detach_helper(); }

        inline void reset()
        {
            if(d && !d->ref.deref())
                delete d;

            d = 0;
        }

        inline operator bool () const { return d != 0; }

        inline bool operator==(const ExplicitlySharedDataPointer<T> &other) const { return d == other.d; }
        inline bool operator!=(const ExplicitlySharedDataPointer<T> &other) const { return d != other.d; }
        inline bool operator==(const T *ptr) const { return d == ptr; }
        inline bool operator!=(const T *ptr) const { return d != ptr; }

        inline ExplicitlySharedDataPointer() { d = 0; }
        inline ~ExplicitlySharedDataPointer() { if (d && !d->ref.deref()) delete d; }

        explicit ExplicitlySharedDataPointer(T *data);
        inline ExplicitlySharedDataPointer(const ExplicitlySharedDataPointer<T> &o) : d(o.d) { if (d) d->ref.ref(); }

        template<class X>
        inline ExplicitlySharedDataPointer(const ExplicitlySharedDataPointer<X> &o) : d(static_cast<T *>(o.data()))
        {
            if(d)
                d->ref.ref();
        }

        inline ExplicitlySharedDataPointer<T> & operator=(const ExplicitlySharedDataPointer<T> &o) {
            if (o.d != d) {
                if (o.d)
                    o.d->ref.ref();
                T *old = d;
                d = o.d;
                if (old && !old->ref.deref())
                    delete old;
            }
            return *this;
        }
        inline ExplicitlySharedDataPointer &operator=(T *o) {
            if (o != d) {
                if (o)
                    o->ref.ref();
                T *old = d;
                d = o;
                if (old && !old->ref.deref())
                    delete old;
            }
            return *this;
        }
    #ifdef Q_COMPILER_RVALUE_REFS
        inline ExplicitlySharedDataPointer(ExplicitlySharedDataPointer &&o) : d(o.d) { o.d = 0; }
        inline ExplicitlySharedDataPointer<T> &operator=(ExplicitlySharedDataPointer<T> &&other)
        { qSwap(d, other.d); return *this; }
    #endif

        inline bool operator!() const { return !d; }

        inline void swap(ExplicitlySharedDataPointer &other)
        { qSwap(d, other.d); }

    protected:
        //T *clone();

    private:
        //void detach_helper();

        T *d;
    };


//    template <class T>
//    Q_INLINE_TEMPLATE T *ExplicitlySharedDataPointer<T>::clone()
//    {
//        return new T(*d);
//    }

//    template <class T>
//    Q_OUTOFLINE_TEMPLATE void ExplicitlySharedDataPointer<T>::detach_helper()
//    {
//        T *x = clone();
//        x->ref.ref();
//        if (!d->ref.deref())
//            delete d;
//        d = x;
//    }

    template <class T>
    Q_INLINE_TEMPLATE ExplicitlySharedDataPointer<T>::ExplicitlySharedDataPointer(T *adata) : d(adata)
    { if (d) d->ref.ref(); }

    template <class T>
    Q_INLINE_TEMPLATE void qSwap(QSharedDataPointer<T> &p1, QSharedDataPointer<T> &p2)
    { p1.swap(p2); }

    template <class T>
    Q_INLINE_TEMPLATE void qSwap(ExplicitlySharedDataPointer<T> &p1, ExplicitlySharedDataPointer<T> &p2)
    { p1.swap(p2); }
}

#ifndef QT_NO_STL
namespace std {

    template <class T>
    Q_INLINE_TEMPLATE void swap(SAM::ExplicitlySharedDataPointer<T> &p1, SAM::ExplicitlySharedDataPointer<T> &p2)
    { p1.swap(p2); }
}
#endif


#endif // SAM_EXPLICITLYSHAREDDATAPOINTER_H
