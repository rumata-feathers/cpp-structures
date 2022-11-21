template<size_t N>
class StackStorage {
 private:
  char buffer_[N];
  size_t capacity_;
 public:
  StackStorage();
  char *allocate(size_t, size_t);
  void deallocate(const char *, size_t);
  ~StackStorage() = default;
};
template<size_t N>
StackStorage<N>::StackStorage() {
  capacity_ = 0;
}
template<size_t N>
char *StackStorage<N>::allocate(size_t size_of_type, size_t n) {
  void *ptr_{buffer_};
  if (capacity_ + n < N) {
    std::size_t space = sizeof(buffer_) - 1;
    ptr_ = buffer_ + capacity_;
    std::align(size_of_type, size_of_type, ptr_, space);
    capacity_ = ((char*) ptr_ - buffer_) + n * size_of_type;
    return (char*) ptr_;
  } else {
    throw std::out_of_range("bad allocate");
  }
}
template<size_t N>
void StackStorage<N>::deallocate(const char *ptr_, size_t n) {
  char *buf{buffer_};
  if (ptr_ - buf >= 0 && size_t(ptr_ - buf) < N) {
    if (ptr_ + n == buf + capacity_) {
      capacity_ -= n;
    }
  } else {
    throw std::out_of_range("bad deallocate");
  }
}

template<typename T, size_t N>
class StackAllocator {
 private:
  StackStorage<N> *storage_;
 public:
  typedef T value_type;
  typedef T *pointer;
  typedef const T *const_pointer;
  typedef T &reference;
  typedef const T &const_reference;
  typedef size_t size_type;

  StackAllocator() = default;
  StackAllocator(StackStorage<N> &other_) : storage_(&other_) {};
  template<class U>
  StackAllocator(const StackAllocator<U, N> &other_) : storage_(other_.storage_) {};
  StackAllocator<T, N> &operator=(const StackAllocator<T, N> &);
  ~StackAllocator() = default;
  pointer allocate(size_t);
  void deallocate(pointer, size_type);
  template<class U>
  struct rebind {
    typedef StackAllocator<U, N> other;
  };
  StackAllocator<T, N> select_on_container_copy_construction();
  template<typename U, size_t M>
  friend
  class StackAllocator;
};

template<typename T, size_t N>
StackAllocator<T, N> &StackAllocator<T, N>::operator=(const StackAllocator<T, N> &other_) {
  storage_ = other_.storage_;
  return *this;
}
template<typename T, size_t N>
void StackAllocator<T, N>::deallocate(StackAllocator::pointer ptr_, StackAllocator::size_type size_) {
  storage_->deallocate(reinterpret_cast<char *>(ptr_), size_ * sizeof(T));
}
template<typename T, size_t N>
T *StackAllocator<T, N>::allocate(size_t n) {
  return reinterpret_cast<T *>(storage_->allocate(sizeof(T), n));
}
template<typename T, size_t N>
StackAllocator<T, N> StackAllocator<T, N>::select_on_container_copy_construction() {
  return StackAllocator<T, N>(*this);
}
template<typename T, size_t N, typename U, size_t M>
bool operator==(const StackAllocator<T, N> &x, const StackAllocator<U, M> &y) {
  return x.storage_ == y.storage_;
}