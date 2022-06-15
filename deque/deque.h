#include <vector>
#include <iostream>

template<typename Type = int>
class Deque {
 public:

  void print() {
    std::cout << "DEQUE:\n";
    for (int j = 0; j < external_capacity_; ++j) {
      for (int i = 0; i < interior_capacity_; ++i) {
        std::cout << external_array_[j][i] << ' ';
      }
      std::cout << "\n";
    }
    std::cout << "\n-------\n";
  }

  Deque();
  explicit Deque(size_t);
  Deque(size_t, const Type &);
  Deque(const Deque &);

  Deque &operator=(const Deque &);
  Type &operator[](const size_t num) {
    return *(begin() + num);
  }
  const Type &operator[](const size_t num) const {
    return *(cbegin() + num);
  }
  Type &at(size_t num) {
    if (begin() + num >= end()) {
      throw std::out_of_range("deque::at");
    }
    return (*this)[num];
  }
  const Type &at(size_t num) const {
    if (begin() + num >= end()) {
      throw std::out_of_range("deque::at");
    }
    return (*this)[num];
  }

  void resize();
  void pop_front();
  void pop_back();
  void push_front(const Type &);
  void push_back(const Type &);
  size_t size() const noexcept;

  ~Deque();

  template<bool is_const>
  class Iterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;
  using reverse_iterator = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  template<bool is_const>
  class Iterator {
   public:
    using difference_type = long long;
    using pointer = typename std::conditional<is_const, const Type *, Type *>::type;
    using reference = typename std::conditional<is_const, const Type &, Type &>::type;

    Iterator() : car_num(nullptr), car_pos(nullptr) {};
    Iterator(Type **other_num, Type *other_pos) {
      car_num = other_num;
      car_pos = other_pos;
    };
    Iterator(const Iterator &other) noexcept: car_num(other.car_num), car_pos(other.car_pos) {};
    pointer operator->() const noexcept { return car_pos; }
    reference operator*() const noexcept { return *car_pos; }
    Iterator &operator++() {
      if (++car_pos - *car_num == interior_capacity_) {
        ++car_num;
        car_pos = *car_num;
      }
      return *this;
    }
    Iterator operator++(int) const {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }
    Iterator &operator--() {
      if (car_pos == *car_num) {
        --car_num;
        car_pos = *car_num + interior_capacity_;
      }
      --car_pos;
      return *this;
    }
    Iterator operator--(int) const {
      Iterator tmp = *this;
      --(*this);
      return tmp;
    }
    Iterator &operator+=(const difference_type dif) {
      if (dif != 0) {
        difference_type tmp = (car_pos - *car_num) + dif;
        if (dif > 0) {
          car_num += tmp / interior_capacity_;
          car_pos = *car_num + tmp % interior_capacity_;
        } else {
          tmp = interior_capacity_ - 1 - tmp;
          car_num -= tmp / interior_capacity_;
          car_pos = *car_num + (interior_capacity_ - 1 - tmp % interior_capacity_);
        }
      }
      return *this;
    }
    Iterator &operator-=(const difference_type dif) {
      (*this) += -dif;
      return (*this);
    }
    Iterator operator+(const difference_type dif) const {
      Iterator tmp = *this;
      tmp += dif;
      return tmp;
    }
    Iterator operator-(const difference_type dif) const {
      Iterator tmp = *this;
      tmp -= dif;
      return tmp;
    }
    friend Iterator operator+(const difference_type dif, const Iterator &x) {
      return x + dif;
    }
    friend Iterator operator-(const difference_type dif, const Iterator &x) {
      return x - dif;
    }
    friend difference_type operator-(const Iterator &x, const Iterator &y) {
      return (x.car_num - y.car_num) * interior_capacity_ +
          ((x.car_pos - *(x.car_num)) - (y.car_pos - *(y.car_num)));
    }
    friend bool operator==(const Iterator &x, const Iterator &y)
    noexcept {
      return x.car_pos == y.car_pos;
    }
    friend bool operator!=(const Iterator &x, const Iterator &y)
    noexcept {
      return x.car_pos != y.car_pos;
    }
    friend bool operator<(const Iterator &x, const Iterator &y)
    noexcept {
      return x.car_num < y.car_num || (x.car_num == y.car_num && x.car_pos < y.car_pos);
    }
    friend bool operator>(const Iterator &x, const Iterator &y)
    noexcept {
      return y < x;
    }
    friend bool operator<=(const Iterator &x, const Iterator &y)
    noexcept {
      return !(x > y);
    }
    friend bool operator>=(const Iterator &x, const Iterator &y)
    noexcept {
      return !(x < y);
    }
    explicit operator const_iterator() const
    noexcept { return const_iterator(car_num, car_pos); }
    ~Iterator() = default;

   private:
    Type **car_num;
    Type *car_pos;
  };

  iterator begin()
  noexcept {
    auto mp = external_array_ + first_num_ / interior_capacity_;
    return iterator(mp, *mp + first_num_ % interior_capacity_);
  };
  iterator end()
  noexcept {
    auto amount = first_num_ + size_;
    auto mp = external_array_ + amount / interior_capacity_;
    return iterator(mp, *mp + amount % interior_capacity_);
  };
  const_iterator begin() const
  noexcept {
    return cbegin();
  };
  const_iterator end() const
  noexcept {
    return cend();
  };
  const_iterator cbegin() const
  noexcept {
    auto mp = external_array_ + first_num_ / interior_capacity_;
    return const_iterator(mp, *mp + first_num_ % interior_capacity_);
  }
  const_iterator cend() const
  noexcept {
    auto amount = first_num_ + size();
    auto mp = external_array_ + amount / interior_capacity_;
    return const_iterator(mp, size_ == 0 ? nullptr : *mp + amount % interior_capacity_);

  }

  reverse_iterator rbegin()
  noexcept { return reverse_iterator(end()); }
  reverse_iterator rend()
  noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const
  noexcept { return crbegin(); }
  const_reverse_iterator rend() const
  noexcept { return crend(); }
  const_reverse_iterator crbegin() const
  noexcept { return const_reverse_iterator(cend()); }
  const_reverse_iterator crend() const
  noexcept { return const_reverse_iterator(cbegin()); }

  void erase(iterator iter);

  void insert(iterator iter, const Type &ptr);

 protected:
  size_t external_capacity_ = 0;
  Type **external_array_ = nullptr;
  static const size_t interior_capacity_ = 2;
  size_t size_ = 0;
  size_t first_num_ = 0;
  size_t calculate_size(size_t number) noexcept {
    if (number == 0) return 1;
    if (number % interior_capacity_ == 0) return 3 * (number / interior_capacity_);
    return 3 * (number / interior_capacity_ + 1);
  }
};

template<typename Type>
size_t Deque<Type>::size() const noexcept {
return size_;
}

template<typename Type>
void Deque<Type>::resize() {
  Type **tmp = nullptr;
  try {
    tmp = new Type *[3 * external_capacity_];
  } catch (...) {
    delete[] tmp;
    throw;
  }
  uint8_t *buffer_first = nullptr;
  uint8_t *buffer_third = nullptr;
  try {
    for (size_t i = 0; i < external_capacity_; ++i) {
      try {
        buffer_first = new uint8_t[interior_capacity_ * sizeof(Type)];
        buffer_third = new uint8_t[interior_capacity_ * sizeof(Type)];
      } catch (...) {
        delete[] buffer_first;
        delete[] buffer_third;
        throw;
      }
      tmp[i] = new(buffer_first) Type();
      tmp[i + 2 * external_capacity_] = new(buffer_third) Type();
      tmp[external_capacity_ + i] = external_array_[i];
    }
    first_num_ += external_capacity_ * interior_capacity_;
    external_capacity_ *= 3;
    external_array_ = tmp;
  } catch (...) {
    for (int i = 0; i < 3 * external_capacity_; ++i) {
      delete[] tmp[i];
    }
    throw;
  }
}

template<typename Type>
void Deque<Type>::pop_front() {
  external_array_[(first_num_) / interior_capacity_][first_num_ % interior_capacity_].~Type();
  ++first_num_;
  --size_;
  if (size_ == 0) first_num_ = external_capacity_ / 3;
}

template<typename Type>
void Deque<Type>::pop_back() {
  external_array_[(first_num_ + size_) / interior_capacity_][(first_num_ + size_) % interior_capacity_].~Type();
  --size_;
  if (size_ == 0) first_num_ = external_capacity_ / 3;
}
template<typename Type>
void Deque<Type>::push_front(const Type &ptr) {
  if (first_num_ == 0) {
    resize();
  }
  external_array_[(first_num_ - 1) / interior_capacity_][(first_num_ - 1) % interior_capacity_] = ptr;
  --first_num_;
  ++size_;
}
template<typename Type>
void Deque<Type>::push_back(const Type &ptr) {
  if (first_num_ + size_ == external_capacity_ * interior_capacity_ - 1) {
    resize();
  }
  external_array_[(first_num_ + size_) / interior_capacity_][(first_num_ + size_) % interior_capacity_] = ptr;
  ++size_;
}
template<typename Type>
void Deque<Type>::erase(Deque::iterator iter) {
  try {
    Deque<Type> tmp;
    iterator new_iter = begin();
    while (new_iter != end()) {
      if (new_iter != iter) tmp.push_back(*new_iter);
      ++new_iter;
    }
    std::swap(*this, tmp);
  } catch (...) {
    throw std::out_of_range("bad erase");
  }
}
template<typename Type>
void Deque<Type>::insert(Deque::iterator iter, const Type &ptr) {
  try {
    Deque<Type> tmp;
    for (auto i = begin(); i < iter; ++i) {
      tmp.push_back(*i);
    }
    tmp.push_back(ptr);
    for (auto i = iter; i < end(); ++i) {
      tmp.push_back(*i);
    }
    std::swap(*this, tmp);
  } catch (...) {
    throw std::out_of_range("bad insert");
  }
}

template<typename Type>
Deque<Type>::Deque() {
  try {
    external_capacity_ = 1;
    external_array_ = new Type *[1];
  } catch (...) {
    delete[] external_array_;
    throw;
  }
  uint8_t *buffer = nullptr;
  try {
    try {
      buffer = new uint8_t[interior_capacity_ * sizeof(Type)];
    } catch (...) {
      delete[] buffer;
      throw;
    }
    external_array_[0] = new(buffer) Type();
  } catch (...) {
    delete[] external_array_[0];
    throw;
  }
}
template<typename Type>
Deque<Type>::Deque(size_t number) {
  external_capacity_ = calculate_size(number);
  try {
    external_array_ = new Type *[external_capacity_];
  } catch (...) {
    delete[] external_array_;
    throw;
  }
  try {
    uint8_t *buffer = nullptr;
    size_ = number;
    first_num_ = (external_capacity_ / 3) * interior_capacity_;
    for (size_t i = 0; i < external_capacity_; ++i) {
      try {
        buffer = new uint8_t[interior_capacity_ * sizeof(Type)];
      } catch (...) {
        delete[] buffer;
        throw;
      }
      external_array_[i] = new(buffer) Type();
    }
  } catch (...) {
    for (int i = 0; i < external_capacity_; ++i) {
      delete[] external_array_[i];
    }
    delete[] external_array_;
    throw std::logic_error("deque::deque(): not default constructible type");
  }
}
template<typename Type>
Deque<Type> &Deque<Type>::operator=(const Deque &other_deque) {
  size_t initial_size = size();
  size_t pushed_amount = 0;
  try {
    for (auto &el : other_deque) {
      push_back(el);
      ++pushed_amount;
    }
    for (int i = 0; i < initial_size; ++i) {
      pop_front();
    }
    return *this;
  } catch (...) {
    for (int i = 0; i < pushed_amount; ++i) {
      pop_back();
    }
    throw std::invalid_argument("deque::operator=");
  }
}
template<typename Type>
Deque<Type>::Deque(const Deque &deq) {
  external_capacity_ = calculate_size(deq.size());
  first_num_ = (external_capacity_ / 3) * interior_capacity_;
  Type **tmp = nullptr;
  try {
    tmp = new Type *[external_capacity_];
  } catch (...) {
    delete[] tmp;
    throw;
  }
  uint8_t *buffer = nullptr;
  try {
    for (int i = 0; i < external_capacity_; ++i) {
      try {
        buffer = new uint8_t[interior_capacity_ * sizeof(Type)];
      } catch (...) {
        delete[] buffer;
        throw;
      }
      tmp[i] = new(buffer) Type();
    }
  } catch (...) {
    for (int i = 0; i < external_capacity_; ++i) {
      delete[] tmp[i];
    }
    delete[] tmp;
    external_capacity_ = 0;
    first_num_ = 0;
    throw std::invalid_argument("deque::deque(const deque&)");
  }
  try {
    for (size_t i = 0; i < deq.size(); ++i) {
      tmp[(first_num_ + i) / interior_capacity_][(first_num_ + i) % interior_capacity_] = deq[i];
    }
    external_array_ = tmp;
    size_ = deq.size();
  } catch (...) {
    for (int i = 0; i < external_capacity_; ++i) {
      for (int j = 0; j < interior_capacity_; ++j) {
        tmp[i][j].~Type();
      }
      delete[] tmp[i];
    }
    delete[] tmp;
    external_capacity_ = 0;
    first_num_ = 0;
    throw std::invalid_argument("deque::deque(const deque&)");
  }
}
template<typename Type>
Deque<Type>::Deque(size_t number, const Type &ptr) {
  size_ = number;
  external_capacity_ = calculate_size(number);
  try {
    external_array_ = new Type *[external_capacity_];
  } catch (...) {
    delete[] external_array_;
    throw;
  }
  uint8_t *buffer = nullptr;
  try {
    for (size_t i = 0; i < external_capacity_; ++i) {
      try {
        buffer = new uint8_t[interior_capacity_ * sizeof(Type)];
      } catch (...) {
        delete[] buffer;
        throw;
      }
      external_array_[i] = new(buffer) Type();
    }
  } catch (...) {
    for (int i = 0; i < external_capacity_; ++i) {
      delete[] external_array_[i];
    }
    delete[] external_array_;
  }

  try {
    first_num_ = external_capacity_ * interior_capacity_ / 3;
    for (int i = 0; i < number; ++i) {
      external_array_[(first_num_ + i) / interior_capacity_][(first_num_ + i) % interior_capacity_] = ptr;
    }
  } catch (...) {
    for (int i = 0; i < external_capacity_; ++i) {
      for (int j = 0; j < interior_capacity_; ++j) {
        external_array_[i][j].~Type();
      }
      delete[] external_array_[i];
    }
    delete[] external_array_;
    throw std::out_of_range("deque:deque(size_t, type&");
  }
}
template<typename Type>
Deque<Type>::~Deque<Type>() {
  for (int i = 0; i < external_capacity_; ++i) {
    delete[] external_array_[i];
  }
  delete[] external_array_;
}

