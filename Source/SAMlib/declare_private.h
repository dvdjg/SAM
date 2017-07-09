#ifndef DECLARE_PRIVATE_H__
#define DECLARE_PRIVATE_H__

template <typename T> static inline T *jGetPtrHelper(T *ptr) { return ptr; }
template <typename Wrapper> static inline typename Wrapper::pointer jGetPtrHelper(const Wrapper &p) \
    { return const_cast<typename Wrapper::pointer>(p.data()); }

#define J_DECLARE_PRIVATE_D(Dptr, Class) \
    inline Class##Private* d_func() { return reinterpret_cast<Class##Private *>(jGetPtrHelper(Dptr)); } \
    inline const Class##Private* d_func() const { return reinterpret_cast<const Class##Private *>(jGetPtrHelper(Dptr)); } \
    friend class Class##Private;

#define J_DECLARE_PRIVATE(Class) J_DECLARE_PRIVATE_D(m_ptr, Class)

#define J_DECLARE_PUBLIC(Class)                                    \
    inline Class* q_func() { return static_cast<Class *>(q_ptr); } \
    inline const Class* q_func() const { return static_cast<const Class *>(q_ptr); } \
    friend class Class;

#define J_D(Class) Class##Private * const d = d_func()
#define J_Q(Class) Class * const q = q_func()


#endif // DECLARE_PRIVATE_H__

