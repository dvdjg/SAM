#ifndef SAM_FILTERTEXT_H
#define SAM_FILTERTEXT_H


#include <iterator>

namespace SAM_DeclarationParser
{
    using namespace std;

    template<int sizeT>
    struct typed_size
    {
        typedef char (&type)[sizeT];
    };

    template<typename T>
    class remove_reference
    {
    public:
        typedef T type;
    };

    template<typename T>
    class remove_reference<T&>
    {
    public:
        typedef T type;
    };

    template<typename T>
    class remove_const
    {
    public:
        typedef T type;
    };

    template<typename T>
    class remove_const<const T>
    {
    public:
        typedef T type;
    };

    template<typename T>
    class remove_pointer
    {
    public:
        typedef T type;
    };

    template<typename T>
    class remove_pointer<T*>
    {
    public:
        typedef T type;
    };

    ///////////////////////////////////////////////////////////////////////////////////
    struct Dummy
    {
    };

    template<typename containerT, typename iteratorT = typename containerT::iterator>
    class iterator_wrapper : public iteratorT
    {
    public:
        typedef containerT container_t;
        typedef iteratorT iterator_t;
    protected:
        container_t & m_container;

    public:
        iterator_wrapper(const iterator_wrapper & it) : iteratorT(it), m_container(it.m_container) {}
        iterator_wrapper(container_t & container, /*const*/ iterator_t it) :
            iteratorT(it), m_container(container) {}
        virtual ~iterator_wrapper() {}

        container_t & container() const { return m_container; }
        iterator_wrapper & operator=(const iterator_wrapper & it)
        {
            if(m_container == it.m_container)
                *this = it;

            // TODO: Check if container have the same type

            return *this;
        }

        iterator_wrapper & operator=(const iterator_t & it)
        {
            *(iterator_t*)this = it;

            // TODO: Check if container have the same type

            return *this;
        }
    };

    template<typename containerT, typename iteratorT>
    class iterator_wrapper<containerT, iteratorT*> : public iterator<random_access_iterator_tag , iteratorT>
    {
    public:
        typedef containerT container_t;
        typedef iteratorT* iterator_t;

        typedef iteratorT value_type;
        typedef random_access_iterator_tag iterator_category;
        typedef iterator_wrapper<containerT, iteratorT*> self_t;
        typedef iterator<random_access_iterator_tag , iteratorT> base_t;

        typedef ptrdiff_t difference_type;
        typedef const iteratorT* pointer;
        typedef const iteratorT& reference;

    protected:
        const container_t & m_container;
        iterator_t m_iterator;

    public:
        iterator_wrapper(const iterator_wrapper & it) : m_container(it.m_container), m_iterator(it.m_iterator) {}
        iterator_wrapper(const container_t & container, iterator_t it) :
            m_container(container), m_iterator(it) {}
        virtual ~iterator_wrapper() {}

        container_t & container() const { return m_container; }
        iterator_wrapper & operator=(const iterator_wrapper & it)
        {
            if(m_container == it.m_container)
                *this = it;

            // TODO: Check if container have the same type

            return *this;
        }

        iterator_wrapper & operator=(const iterator_t & it)
        {
            *(iterator_t*)this = it;

            // TODO: Check if container have the same type

            return *this;
        }

        reference operator*() const
        {	// return designated object
            return (*m_iterator);
        }

        pointer operator->() const
        {	// return pointer to class object
            return (&**this);
        }

        self_t& operator++()
        {	// preincrement
             ++m_iterator;
            return (*this);
        }

        self_t operator++(int)
        {	// postincrement
            self_t tmp = *this;
            ++*this;
            return (tmp);
        }

        self_t& operator--()
        {	// predecrement
             --m_iterator;
            return (*this);
        }

        self_t operator--(int)
        {	// postdecrement
            self_t tmp = *this;
            --*this;
            return (tmp);
        }

        self_t& operator+=(difference_type offset)
        {	// increment by integer
            m_iterator += offset;
            return (*this);
        }

        self_t operator+(difference_type offset) const
        {	// return this + integer
            self_t tmp = *this;
            return (tmp += offset);
        }

        self_t& operator-=(difference_type offset)
        {	// decrement by integer
            return (*this += -offset);
        }

        self_t operator-(difference_type offset) const
        {	// return this - integer
            self_t tmp = *this;
            return (tmp -= offset);
        }

        difference_type operator-(const self_t& right) const
        {	// return difference of iterators
            return (m_iterator - right.m_iterator);
        }

        reference operator[](difference_type offset) const
        {	// subscript
            return (*(*this + offset));
        }

        bool operator==(const self_t& right) const
        {	// test for iterator equality
            return (m_iterator == right.m_iterator);
        }

        bool operator==(const iterator_t right) const
        {	// test for iterator equality
            return (m_iterator == right);
        }

        bool operator!=(const self_t& right) const
        {	// test for iterator inequality
            return (!(*this == right));
        }

        bool operator<(const self_t& right) const
        {	// test if this < right
            return (m_iterator < right.m_iterator);
        }

        bool operator>(const self_t& right) const
        {	// test if this > right
            return (right < *this);
        }

        bool operator<=(const self_t& right) const
        {	// test if this <= right
            return (!(right < *this));
        }

        bool operator>=(const self_t& right) const
        {	// test if this >= right
            return (!(*this < right));
        }
    };

    template<typename containerT, typename iteratorT>
    class iterator_wrapper<containerT, const iteratorT*> : public iterator<random_access_iterator_tag , iteratorT>
    {
    public:
        typedef containerT container_t;
        typedef const iteratorT* iterator_t;

        typedef iteratorT value_type;
        typedef random_access_iterator_tag iterator_category;
        typedef iterator_wrapper<containerT, const iteratorT*> self_t;
        typedef iterator<random_access_iterator_tag , iteratorT> base_t;

        typedef ptrdiff_t difference_type;
        typedef const iteratorT* pointer;
        typedef const iteratorT& reference;

    protected:
        const container_t & m_container;
        iterator_t m_iterator;

    public:
        iterator_wrapper(const iterator_wrapper & it) : m_container(it.m_container), m_iterator(it.m_iterator) {}
        iterator_wrapper(const container_t & container, iterator_t it) :
            m_container(container), m_iterator(it) {}
        virtual ~iterator_wrapper() {}

        container_t & container() const { return m_container; }
        iterator_wrapper & operator=(const iterator_wrapper & it)
        {
            if(m_container == it.m_container)
                *this = it;

            // TODO: Check if container have the same type

            return *this;
        }

        iterator_wrapper & operator=(const iterator_t & it)
        {
            *(iterator_t*)this = it;

            // TODO: Check if container have the same type

            return *this;
        }

        reference operator*() const
        {	// return designated object
            return (*m_iterator);
        }

        pointer operator->() const
        {	// return pointer to class object
            return (&**this);
        }

        self_t& operator++()
        {	// preincrement
             ++m_iterator;
            return (*this);
        }

        self_t operator++(int)
        {	// postincrement
            self_t tmp = *this;
            ++*this;
            return (tmp);
        }

        self_t& operator--()
        {	// predecrement
             --m_iterator;
            return (*this);
        }

        self_t operator--(int)
        {	// postdecrement
            self_t tmp = *this;
            --*this;
            return (tmp);
        }

        self_t& operator+=(difference_type offset)
        {	// increment by integer
            m_iterator += offset;
            return (*this);
        }

        self_t operator+(difference_type offset) const
        {	// return this + integer
            self_t tmp = *this;
            return (tmp += offset);
        }

        self_t& operator-=(difference_type offset)
        {	// decrement by integer
            return (*this += -offset);
        }

        self_t operator-(difference_type offset) const
        {	// return this - integer
            self_t tmp = *this;
            return (tmp -= offset);
        }

        difference_type operator-(const self_t& right) const
        {	// return difference of iterators
            return (m_iterator - right.m_iterator);
        }

        reference operator[](difference_type offset) const
        {	// subscript
            return (*(*this + offset));
        }

        bool operator==(const self_t& right) const
        {	// test for iterator equality
            return (m_iterator == right.m_iterator);
        }

        bool operator==(const iterator_t right) const
        {	// test for iterator equality
            return (m_iterator == right);
        }

        bool operator!=(const self_t& right) const
        {	// test for iterator inequality
            return (!(*this == right));
        }

        bool operator<(const self_t& right) const
        {	// test if this < right
            return (m_iterator < right.m_iterator);
        }

        bool operator>(const self_t& right) const
        {	// test if this > right
            return (right < *this);
        }

        bool operator<=(const self_t& right) const
        {	// test if this <= right
            return (!(right < *this));
        }

        bool operator>=(const self_t& right) const
        {	// test if this >= right
            return (!(*this < right));
        }
    };


    template<typename containerT, typename iteratorT>
    iterator_wrapper<containerT, iteratorT> wrap_iterator(containerT & container, iteratorT & it)
    {
        return iterator_wrapper<containerT, iteratorT>(container, it);
    }

    template<typename containerT, typename iteratorT>
    iterator_wrapper<containerT, const iteratorT> const_wrap_iterator(containerT & container, const iteratorT & cit)
    {
        const iteratorT it = cit;
        return iterator_wrapper<containerT, const iteratorT>(container, it);
    }


    // TEMPLATE CLASS pointer_const_iterator
    template<typename  typeT, typename categoryT = random_access_iterator_tag>
    class pointer_const_iterator
        : public iterator<categoryT , typeT>
    {	// iterator for nonmutable vector
    public:
        typedef typeT value_type;
        typedef categoryT iterator_category;
        typedef pointer_const_iterator<typeT, categoryT> self_t;
        typedef iterator<categoryT, typeT> base_t;

        typedef ptrdiff_t difference_type;
        typedef const typeT* pointer;
        typedef const typeT& reference;

    protected:
        pointer m_iterator;

    public:
        pointer_const_iterator()
        {	// construct with null pointer
            m_iterator = 0;
        }

        /*explicit*/ pointer_const_iterator(pointer ptr)
        {	// construct with pointer ptr
            m_iterator = ptr;
        }

        reference operator*() const
        {	// return designated object
            return (*m_iterator);
        }

        pointer operator->() const
        {	// return pointer to class object
            return (&**this);
        }

        self_t& operator++()
        {	// preincrement
             ++m_iterator;
            return (*this);
        }

        self_t operator++(int)
        {	// postincrement
            self_t tmp = *this;
            ++*this;
            return (tmp);
        }

        self_t& operator--()
        {	// predecrement
             --m_iterator;
            return (*this);
        }

        self_t operator--(int)
        {	// postdecrement
            self_t tmp = *this;
            --*this;
            return (tmp);
        }

        self_t& operator+=(difference_type offset)
        {	// increment by integer
            m_iterator += offset;
            return (*this);
        }

        self_t operator+(difference_type offset) const
        {	// return this + integer
            self_t tmp = *this;
            return (tmp += offset);
        }

        self_t& operator-=(difference_type offset)
        {	// decrement by integer
            return (*this += -offset);
        }

        self_t operator-(difference_type offset) const
        {	// return this - integer
            self_t tmp = *this;
            return (tmp -= offset);
        }

        difference_type operator-(const self_t& right) const
        {	// return difference of iterators
            return (m_iterator - right.m_iterator);
        }

        reference operator[](difference_type offset) const
        {	// subscript
            //return **this;
            return (*(*this + offset));
        }

        bool operator==(const self_t& right) const
        {	// test for iterator equality
            return (m_iterator == right.m_iterator);
        }

        bool operator!=(const self_t& right) const
        {	// test for iterator inequality
            return (!(*this == right));
        }

        bool operator<(const self_t& right) const
        {	// test if this < right
            return (m_iterator < right.m_iterator);
        }

        bool operator>(const self_t& right) const
        {	// test if this > right
            return (right < *this);
        }

        bool operator<=(const self_t& right) const
        {	// test if this <= right
            return (!(right < *this));
        }

        bool operator>=(const self_t& right) const
        {	// test if this >= right
            return (!(*this < right));
        }
    };


    template<class typeT,
    class categoryT> inline
        pointer_const_iterator<typeT, categoryT> operator+(
        typename pointer_const_iterator<typeT, categoryT>::difference_type offset,
        pointer_const_iterator<typeT, categoryT> next)
    {	// add offset to iterator
        return (next += offset);
    }

    // TEMPLATE CLASS pointer_iterator
    template<class typeT, class categoryT = random_access_iterator_tag>
    class pointer_iterator
        : public pointer_const_iterator<typeT, categoryT>
    {	// iterator for mutable vector
    public:
        typedef pointer_iterator<typeT, categoryT> self_t;
        typedef pointer_const_iterator<typeT, categoryT> base_t;

        typedef categoryT iterator_category;
        typedef typeT value_type;
        typedef ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef value_type& reference;

        pointer_iterator()
        {	// construct with null vector pointer
            //m_iterator = 0;
        }

        /*explicit*/ pointer_iterator(pointer ptr)
            : base_t(ptr)
        {	// construct with pointer ptr
        }

        reference operator*() const
        {	// return designated object
            return ((reference)**(base_t *)this);
        }

        pointer operator->() const
        {	// return pointer to class object
            return (&**this);
        }

        self_t& operator++()
        {	// preincrement
            ++(*(base_t *)this);
            return (*this);
        }

        self_t operator++(int)
        {	// postincrement
            self_t tmp = *this;
            ++*this;
            return (tmp);
        }

        self_t& operator--()
        {	// predecrement
            --(*(base_t *)this);
            return (*this);
        }

        self_t operator--(int)
        {	// postdecrement
            self_t tmp = *this;
            --*this;
            return (tmp);
        }

        self_t& operator+=(difference_type offset)
        {	// increment by integer
            (*(base_t *)this) += offset;
            return (*this);
        }

        self_t operator+(difference_type offset) const
        {	// return this + integer
            self_t tmp = *this;
            return (tmp += offset);
        }

        self_t& operator-=(difference_type offset)
        {	// decrement by integer
            return (*this += -offset);
        }

        self_t operator-(difference_type offset) const
        {	// return this - integer
            self_t tmp = *this;
            return (tmp -= offset);
        }

        difference_type operator-(const base_t& right) const
        {	// return difference of iterators
            return (*(base_t *)this - right);
        }

        reference operator[](difference_type offset) const
        {	// subscript
            return (*(*this + offset));
        }
    };

    template<class typeT, class categoryT> inline
        pointer_iterator<typeT, categoryT> operator+(
        typename pointer_iterator<typeT, categoryT>::difference_type offset,
        pointer_iterator<typeT, categoryT> next)
    {	// add offset to iterator
        return (next += offset);
    }

    // TEMPLATE CLASS dummy_const_iterator
    template<class typeT, typename categoryT = random_access_iterator_tag>
    class dummy_const_iterator
        : public pointer_const_iterator<categoryT, typeT>
    {	// iterator for nonmutable vector
    public:
        typedef dummy_const_iterator<typeT, categoryT> self_t;
        typedef pointer_const_iterator<categoryT, typeT> base_t;

        typedef categoryT iterator_category;
        typedef typeT value_type;
        typedef ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef value_type& reference;
   public:

        dummy_const_iterator()
        {	// construct with null pointer
            //m_iterator = 0;
        }

        dummy_const_iterator(pointer ptr)
        {	// construct with pointer ptr
            //m_iterator = ptr;
        }

        reference operator*() const
        {	// return designated object
            //return (*m_iterator);
            static value_type val;
            return val;
        }

        ~dummy_const_iterator() {}
    };

    // TEMPLATE CLASS dummy_iterator
    template<class typeT, typename categoryT = random_access_iterator_tag>
    class dummy_iterator
        : public pointer_iterator<categoryT, typeT>
    {	// iterator for nonmutable vector
    public:
        typedef dummy_iterator<typeT, categoryT> self_t;
        typedef pointer_iterator<categoryT, typeT> base_t;

        typedef categoryT iterator_category;
        typedef typeT value_type;
        typedef ptrdiff_t difference_type;
        typedef value_type* pointer;
        typedef value_type& reference;
    public:

        dummy_iterator()
        {	// construct with null pointer
            //m_iterator = 0;
        }

        dummy_iterator(pointer ptr)
        {	// construct with pointer ptr
            //m_iterator = ptr;
        }

        reference operator*() const
        {	// return designated object
            //return (*m_iterator);
            static value_type val;
            return val;
        }

        ~dummy_iterator() {}

    };

    ///////////////////////////////////////////////////////////////////////////////////

    template<typename T>
    struct false_container
    {
        typedef T value_type;
        typedef size_t size_type;
        typedef T* iterator;
        typedef const T* const_iterator;
    };

    template<typename T>
    struct false_adaptor
    {
        typedef T value_type;
        typedef size_t size_type;
        typedef false_container<T> container_type;
    };


    template<typename T>
    class container_traits
    {
    public:
        typedef typename remove_const<T>::type self_type;
        typedef self_type* self_pointer;
        typedef self_type& self_reference;
        typedef typename self_type::value_type value_type;
        typedef typename self_type::size_type size_type;
        typedef typename self_type::const_iterator const_iterator_inner;
        typedef typename self_type::iterator iterator_inner;
        typedef iterator_wrapper<const self_type, const_iterator_inner> const_iterator;
        typedef iterator_wrapper<self_type, iterator_inner> iterator;

        static size_type size(const self_type & t) { return t.size(); }
        static iterator begin(self_type & t) { return wrap_iterator(t, t.begin()); }
        static const_iterator const_begin(self_type const & t) { return const_iterator(t, t.begin()); }
        static iterator end(self_type & t) { return wrap_iterator(t, t.end()); }
        static const_iterator const_end(self_type const & t) { return const_iterator(t, t.end()); }
        template<typename itT>
        static bool finished(const self_type & t, itT it) { return it == t.end(); }
        static bool push(self_type & t, iterator_inner & itWhere, const value_type & val)
        {
            itWhere = t.insert(itWhere, val);

            if( itWhere == t.end() )
                return false;

            ++itWhere;

            return true;
        }

        template<typename itT>
        static bool finished(itT it) {
            return finished(it.container(), static_cast<typename itT::iterator_t &>( it ));
        }

        static bool push(iterator & itWhere, const value_type & val) {
            return push(itWhere.container(), static_cast<typename iterator::iterator_t &>( itWhere ), val);
        }
    };

    template<>
    class container_traits<ostream>
    {
    public:
        typedef ostream self_type;
        typedef self_type* self_pointer;
        typedef self_type& self_reference;
        typedef char value_type;
        //typedef self_type::char_type value_type;
        //typedef self_type::streampos size_type;
        typedef size_t size_type;
        typedef ostream_iterator<value_type/*, value_type, self_type::traits_type*/> iterator_inner;
        typedef pointer_iterator<value_type> const_iterator_inner;
        typedef iterator_wrapper<const self_type, const_iterator_inner> const_iterator;
        typedef iterator_wrapper<self_type, iterator_inner> iterator;

        static size_type size(const self_type & t) { return ~0; }
        static iterator begin(self_type & t) { t.clear(); return iterator(t, iterator_inner(t)); }
        //static /*const_iterator*/iterator const_begin(self_type & t) { return const_wrap_iterator(t, iterator_inner(t)); }
        static iterator end(self_type & t) { return iterator(t, iterator_inner(t)); }
        //static /*const_iterator*/ iterator const_end(self_type & t) { return const_wrap_iterator(t, iterator_inner()); }
        static bool finished(const self_type & t, const_iterator_inner it) { return true; }
        static bool finished(const self_type & t, iterator_inner it) { return !t.good(); }
        static bool push(self_type & t, iterator_inner & itWhere, const value_type & val)
        {
            if(finished(t, itWhere))
                return false;

            *itWhere++ = val;

            return true;
        }

        static bool finished(const_iterator it) { return true; }
        static bool finished(iterator it) { return finished(it.container(), static_cast<iterator::iterator_t &>( it )); }
        static bool push(iterator & itWhere, const value_type & val) { return push(itWhere.container(), static_cast<iterator::iterator_t &>( itWhere ), val); }
    };


    template<>
    class container_traits<stringstream>
    {
    public:
        typedef iostream self_type;
        typedef self_type* self_pointer;
        typedef self_type& self_reference;
        typedef /*self_type::char_type*/char value_type;
        typedef /*self_type::streampos*/size_t size_type;
        typedef istream_iterator<value_type/*, value_type, self_type::traits_type*/> const_iterator_inner;
        typedef ostream_iterator<value_type/*, value_type, self_type::traits_type*/> iterator_inner;
        typedef iterator_wrapper<const self_type, const_iterator_inner> const_iterator;
        typedef iterator_wrapper<self_type, iterator_inner> iterator;

        static size_type size(const self_type & t) { return ~0; }
        static iterator begin(self_type & t) { t.clear(); return iterator(t, iterator_inner(t)); }
        static const_iterator const_begin(self_type & t)
        {
            t.clear();
            return const_iterator(t, const_iterator_inner(t));
        }

        static iterator end(self_type & t) { return iterator(t, iterator_inner(t)); }
        static const_iterator const_end(self_type & t) { return const_iterator(t, const_iterator_inner(t)); }
        template<typename itT>
        static bool finished(const self_type & t, itT it) { return !t.good(); }
        static bool push(self_type & t, iterator_inner & itWhere, const value_type & val)
        {
            if(finished(t, itWhere))
                return false;

            *itWhere++ = val;

            return true;
        }

        static bool finished(iterator it) { return finished(it.container(), static_cast<iterator::iterator_t &>( it )); }
        static bool finished(const_iterator it) { return finished(it.container(), static_cast<const_iterator::iterator_t &>( it )); }
        static bool push(iterator & itWhere, const value_type & val) { return push(itWhere.container(), static_cast<iterator::iterator_t &>( itWhere ), val); }
    };

    template<typename T>
    class container_traits< basic_string<T/*, char_traits<T>, allocator<T>*/ > >
    {
    public:
        typedef basic_string<T/*, char_traits<T>, allocator<T>*/ > T_type;
        typedef typename remove_const<T_type>::type self_type;
        typedef self_type* self_pointer;
        typedef self_type& self_reference;
        typedef /*typename self_type::value_type*/T value_type;
        typedef /*typename self_type::size_type*/size_t size_type;
        typedef typename self_type::const_iterator const_iterator_inner;
        typedef typename self_type::iterator iterator_inner;
        typedef iterator_wrapper<const self_type, const_iterator_inner> const_iterator;
        typedef iterator_wrapper<self_type, iterator_inner> iterator;

        static size_type size(const self_type & t) { return t.size(); }
        static iterator begin(self_type & t) { wrap_iterator(t, t.begin()); }
        static const_iterator const_begin(const self_type & t) { return const_iterator(t, t.begin()); }

        static iterator end(self_type & t) { return wrap_iterator(t, t.end()); }
        static const_iterator const_end(const self_type & t) { return const_iterator(t, t.end()); }
        template<typename itT>
        static bool finished(const self_type & t, itT it) { return it == t.end(); }
        static bool push(self_type & t, iterator_inner & itWhere, const value_type & val)
        {
            t.insert(itWhere, val);

            return true;
        }

        static bool finished(iterator it) { return finished(it.container(), static_cast<typename iterator::iterator_t &>( it )); }
        static bool finished(const_iterator it) { return finished(it.container(), static_cast<typename const_iterator::iterator_t &>( it )); }
        static bool push(iterator & itWhere, const value_type & val) { return push(itWhere.container(), static_cast<typename iterator::iterator_t &>( itWhere ), val); }
    };

    template<>
    class container_traits<stringbuf>
    {
    public:
        typedef stringbuf self_type;
        typedef self_type* self_pointer;
        typedef self_type& self_reference;
        typedef char value_type;
        //typedef self_type::char_type value_type;
        typedef size_t/*self_type::streampos*/ size_type;

        static size_type size(const self_type & t) { return ~0; }
    };

    template<typename charT, size_t N>
    class container_traits< charT [N] >
    {
    public:
        typedef charT (self_type)[N];
        typedef charT * self_pointer;
        typedef self_type self_reference;
        typedef charT value_type;
        typedef size_t size_type;
        typedef pointer_iterator<typename remove_const<value_type>::type> iterator_inner;
        typedef pointer_const_iterator<const typename remove_const<value_type>::type> const_iterator_inner;
        typedef iterator_wrapper<const self_type, const_iterator_inner> const_iterator;
        typedef iterator_wrapper<self_type, iterator_inner> iterator;

        static size_type size(self_type t) { return N; }
        static iterator begin(self_type & t) { return iterator(t, iterator_inner(t)); }
        static const_iterator const_begin(self_type & t) { return const_iterator(t, const_iterator_inner(t)); }
        static iterator end(self_type & t) { return iterator(t, iterator_inner(t)); }
        static const_iterator const_end(self_type & t) { return const_iterator(t, const_iterator_inner(t)); }
        static bool finished(const self_type & t, const_iterator_inner it) { return *it == 0; }
        static bool finished(const self_type & t, iterator_inner it) { return it == t + N; }
        static bool push(self_type & t, iterator_inner & itWhere, const value_type & val)
        {
            if(finished(t, itWhere))
                return false;

            *itWhere++ = val;

            return true;
        }

        static bool finished(iterator it) { return finished(it.container(), static_cast<typename iterator::iterator_t &>( it )); }
        static bool finished(const_iterator it) { return finished(it.container(), static_cast<typename const_iterator::iterator_t &>( it )); }
        static bool push(iterator & itWhere, const value_type & val) { return push(itWhere.container(), static_cast<typename iterator::iterator_t &>( itWhere ), val); }
    };

    template<typename charT>
    class container_traits< charT *>
    {
    public:
        typedef charT * self_type;
        typedef self_type* self_pointer;
        typedef self_type& self_reference;
        typedef charT value_type;
        typedef size_t size_type;
        typedef pointer_iterator<typename remove_const<value_type>::type> iterator_inner;
        typedef pointer_const_iterator< typename remove_const<value_type>::type> const_iterator_inner;
        typedef iterator_wrapper<const self_type, const_iterator_inner> const_iterator;
        typedef iterator_wrapper<self_type, iterator_inner> iterator;

        static size_type size(const self_type t) { return strlen(t); }
        static iterator begin(self_type & t) { return iterator(t, iterator_inner(t)); }
        static const_iterator const_begin(const self_type & t) { return const_iterator(t, const_iterator_inner(t)); }
        static iterator end(self_type & t) { return iterator(t, iterator_inner(t)); }
        static const_iterator const_end(self_type & t) { return const_iterator(t, const_iterator_inner(t)); }
        static bool finished(const self_type & t, const_iterator_inner it) { return *it == 0; }
        static bool finished(const self_type & t, iterator_inner it) { return false; }
        static bool push(self_type & t, iterator_inner & itWhere, const value_type & val)
        {
            if(finished(t, itWhere))
                return false;

            *itWhere++ = val;

            return true;
        }

        static bool finished(iterator it) { return finished(it.container(), static_cast<typename iterator::iterator_t &>( it )); }
        static bool finished(const_iterator it) { return finished(it.container(), static_cast<typename const_iterator::iterator_t &>( it )); }
        static bool push(iterator & itWhere, const value_type & val) { return push(itWhere.container(), itWhere, val); }
    };

    template<typename T>
    class container_traits_deref : public container_traits<typename remove_reference<T>::type >
    {
    };

    ///////////////////////////////////////////////////////////////////////////////////
    template<typename outT, typename inT, typename fnTransT>
    outT & filterFn(outT & out, inT & in, fnTransT fnTrans)
    {
        typedef typename remove_const<inT>::type in_type;
        typedef typename container_traits<in_type>::value_type in_t;
        typedef typename container_traits<outT>::value_type out_t;

        typename remove_const<in_t>::type buffIn[16] = {0}, * from_end = buffIn;

        out_t buffOut[16] = {0}, * to_end = buffOut+16;

        codecvt_base::result result;

        const in_t * from_next;
        out_t * to_next;

        typename container_traits<outT>::iterator outIt = container_traits<outT>::end(out); // Try to append

        for(typename container_traits<in_type>::const_iterator inIt = container_traits<in_type>::const_begin(in);
            !container_traits<in_type>::finished(in, inIt) && from_end != buffIn+16;
            ++inIt)
        {
            out_t *bufIt, *bufEnd;
            *from_end++ = *inIt;

            result = fnTrans(buffIn, from_end, from_next,
                             buffOut, to_end, to_next);

            if(result == codecvt_base::partial)
            {
                continue; // Give more input chars
            }
            else if(result == codecvt_base::ok)
            {
                bufIt = buffOut; bufEnd = to_next; // Copy transformed bytes
            }
            else if(result == codecvt_base::noconv)
            {
                bufIt = buffIn; bufEnd = from_end; // Copy untransformed source bytes
            }
            else
                break; // Error

            for(; bufIt != bufEnd; ++bufIt)
            {
                if(!container_traits<outT>::push(outIt, *bufIt))
                    return out;
            }

            from_end = buffIn;
        }

        return out;
    }

#ifndef PPC_LEGACY
    template<typename outT, typename inT>
    outT & filter(outT & out, inT & in)
    {
        typedef typename remove_const<inT>::type in_type;
        return filterFn(out, in, &SAM_DeclarationParser::rawToRaw<typename container_traits<in_type>::value_type, typename container_traits<outT>::value_type >);
    }

    template<typename outT, typename inT>
    outT & filterRawToXml(outT & out, inT & in)
    {
        //typedef typename remove_reference<inT>::type in_type0;
        typedef typename remove_const<inT>::type in_type;
        return filterFn(out, in, &SAM_DeclarationParser::rawToXml<typename container_traits<in_type>::value_type, typename container_traits<outT>::value_type >);
    }

    template<typename outT, typename inT>
    outT & filterXmlToRaw(outT & out, inT & in)
    {
        typedef typename remove_const<inT>::type in_type;
        return filterFn(out, in, &SAM_DeclarationParser::xmlToRaw<typename container_traits<in_type>::value_type, typename container_traits<outT>::value_type >);
    }


#else
    template<typename outT, typename inT>
    outT & filter(outT & out, inT & in)
    {
        return filterFn(out, in, &SAM_DeclarationParser::rawToRaw);
    }

    template<typename outT, typename inT>
    outT & filterRawToXml(outT & out, inT & in)
    {
        return filterFn(out, in, &SAM_DeclarationParser::rawToXml);
    }

    template<typename outT, typename inT>
    outT & filterXmlToRaw(outT & out, inT & in)
    {
        typedef typename remove_const<inT>::type in_type;
        return filterFn(out, in, &SAM_DeclarationParser::xmlToRaw);
    }

#endif

 //   template<>
    //ostream & filterRawToXml(ostream & out, const char * & in)
    //{
 //       return filterFn(out, in, &SAM_DeclarationParser::rawToXml);
 //   }


    ///////////////////////////////////////////////////////////////////////

    template<typename T>
    class raw2xml_
    {
        T & m_t;

    public:
        raw2xml_(T & t) : m_t(t) {}
        ~raw2xml_() {}

        T &operator()() const { return m_t; }
    };

    //template<typename T>
    //class const_raw2xml_
    //{
    //    T & m_t;

    //public:
    //    const_raw2xml_(T & t) : m_t(t) {}
    //    ~const_raw2xml_() {}

    //    T &operator()() const { return m_t; }
    //};

    template<typename T>
    raw2xml_<T> raw2xml(T & t)
    {
        return raw2xml_<T>(t);
    }


    template<typename outT, typename inT>
    outT & operator << (outT & o, const raw2xml_<inT> & data)
    {
        return filterRawToXml(o, data());
    }

    template<typename outT, typename inT>
    inT & operator >> (inT & i, raw2xml_<outT> & data)
    {
        filterRawToXml(data(), i);

        return i;
    }

    ///////////////////////////////////////////////////////////////////////

    template<typename T>
    class xml2raw_
    {
        T & m_t;

    public:
        xml2raw_(T & t) : m_t(t) {}
        ~xml2raw_() {}

        T &operator()() const { return m_t; }
    };

    template<typename T>
    xml2raw_<T> xml2raw(T & t)
    {
        return xml2raw_<T>(t);
    }


    template<typename outT, typename inT>
    outT & operator << (outT & o, const xml2raw_<inT> & data)
    {
        return filterXmlToRaw(o, data());
    }

    template<typename outT, typename inT>
    inT & operator >> (inT & i, xml2raw_<outT> & data)
    {
        filterXmlToRaw(data(), i);

        return i;
    }

    ///////////////////////////////////////////////////////////////////////

    template<typename T>
    class raw2raw_
    {
        T & m_t;

    public:
        raw2raw_(T & t) : m_t(t) {}
        ~raw2raw_() {}

        T &operator()() const
        {
            return m_t;
        }
    };

    template<typename T>
    raw2raw_<T> raw2raw(T & t)
    {
        return raw2raw_<T>(t);
    }


    template<typename outT, typename inT>
    outT & operator << (outT & o, const raw2raw_<inT> & data)
    {
        return filter(o, data());
    }

    template<typename outT, typename inT>
    inT & operator >> (inT & i, raw2raw_<outT> & data)
    {
        filter(data(), i);

        return i;
    }

}

#endif // SAM_FILTERTEXT_H
