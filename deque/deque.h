#include <iostream>
#include <vector>

static const size_t carriage_capacity = 256;

template<typename Type = int>
class Deque {
 public:

  Deque() {
    *this = Deque(1);
    --capacity;
  };

  explicit Deque<Type>(const size_t number) : capacity(number),
                                              external_size(((number > 0 ? number : 1) / carriage_capacity
                                                  + ((number > 0 ? number : 1) % carriage_capacity ? 1 : 0)) * 3),
                                              start((external_size * carriage_capacity) / 3),
                                              pointers(new Type* [external_size]) {
    for (size_t i = 0; i < external_size; ++i) {
      pointers[i] = reinterpret_cast<Type*>(new uint8_t[carriage_capacity * sizeof(Type)]);
    }
  };

  Deque(const size_t number, const Type& ptr) : capacity(number),
                                                external_size((number / carriage_capacity +
                                                    (number % carriage_capacity ? 1 : 0)) * 3),
                                                start((external_size * carriage_capacity) / 3),
                                                pointers(new Type* [external_size]) {
    size_t num = number;
    size_t j = start / carriage_capacity;
    for (size_t i = 0; i < external_size; ++i) {
      pointers[i] = reinterpret_cast<Type*>(new uint8_t[carriage_capacity * sizeof(Type)]);
    }
    while (num > carriage_capacity) {
      for (size_t i = 0; i < carriage_capacity; ++i) {
        pointers[j][i] = ptr;
      }
      ++j;
      num -= carriage_capacity;
    }
    for (size_t i = 0; i < num; ++i) {
      pointers[j][i] = ptr;
    }
  };

  Deque(const Deque& deq) {
    *this = Deque<Type>();
    for (const_iterator i = deq.cbegin(); i != deq.cend(); ++i) {
      push_back(*i);
    }
  }

  Type& operator[](const size_t num) {
    return *(begin() + num);
  }

  const Type& operator[](const size_t num) const {
    return *(cbegin() + num);
  }

  Type& at(size_t num) {
    if (begin() + num < end()) {
      return *(begin() + num);
    } else {
      throw std::out_of_range("bruh in at");
    }
  }

  const Type& at(size_t num) const {
    try {
      return *(cbegin() + num);
    } catch (...) {
      throw std::string("bruh in at");
    }
  }

  void resize();

  void pop_front();

  void pop_back();

  void push_front(const Type&);

  void push_back(const Type&);

  [[nodiscard]] size_t size() const;

  ~Deque() = default;

  template<bool is_const>
  class Iterator;

  template<typename Iter>
  class ReverseIterator;

  using iterator = Iterator<false>;
  using const_iterator = Iterator<true>;
  using reverse_iterator = ReverseIterator<iterator>;
  using const_reverse_iterator = ReverseIterator<const_iterator>;

  template<bool is_const>
  class Iterator {
   public:
    using difference_type = long long;
    using pointer = typename std::conditional<is_const, const Type*, Type*>::type;
    using reference = typename std::conditional<is_const, const Type&, Type&>::type;

    Iterator() : car_num(nullptr), car_pos(nullptr) {};

    Iterator(Type** other_num, Type* other_pos) {
      car_num = other_num;
      car_pos = other_pos;
    };

    Iterator(const Iterator& other) : car_num(other.car_num), car_pos(other.car_pos) {};

    pointer operator->() const { return car_pos; }

    reference operator*() const { return *car_pos; }

    Iterator& operator++() {
      if (++car_pos - *car_num == carriage_capacity) {
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

    Iterator& operator--() {
      if (car_pos == *car_num) {
        --car_num;
        car_pos = *car_num + carriage_capacity;
      }
      --car_pos;
      return *this;
    }

    Iterator operator--(int) const {
      Iterator tmp = *this;
      --(*this);
      return tmp;
    }

    Iterator& operator+=(const difference_type dif) {
      if (dif != 0) {
        difference_type tmp = (car_pos - *car_num) + dif;
        if (dif > 0) {
          car_num += tmp / carriage_capacity;
          car_pos = *car_num + tmp % carriage_capacity;
        } else {
          tmp = carriage_capacity - 1 - tmp;
          car_num -= tmp / carriage_capacity;
          car_pos = *car_num + (carriage_capacity - 1 - tmp % carriage_capacity);
        }
      }
      return *this;
    }

    Iterator& operator-=(const difference_type dif) {
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

    friend Iterator operator+(const difference_type dif, const Iterator& x) {
      return x + dif;
    }

    friend Iterator operator-(const difference_type dif, const Iterator& x) {
      return x - dif;
    }

    friend difference_type operator-(const Iterator& x, const Iterator& y) {
      return (x.car_num - y.car_num) * carriage_capacity +
          ((x.car_pos - *(x.car_num)) - (y.car_pos - *(y.car_num)));
    }

    friend bool operator==(const Iterator& x, const Iterator& y) {
      return x.car_pos == y.car_pos;
    }

    friend bool operator!=(const Iterator& x, const Iterator& y) {
      return x.car_pos != y.car_pos;
    }

    friend bool operator<(const Iterator& x, const Iterator& y) {
      return x.car_num < y.car_num || (x.car_num == y.car_num && x.car_pos < y.car_pos);
    }

    friend bool operator>(const Iterator& x, const Iterator& y) {
      return y < x;
    }

    friend bool operator<=(const Iterator& x, const Iterator& y) {
      return !(x > y);
    }

    friend bool operator>=(const Iterator& x, const Iterator& y) {
      return !(x < y);
    }

    explicit operator const_iterator() const { return const_iterator(car_num, car_pos); }

    ~Iterator() = default;

   private:
    Type** car_num;
    Type* car_pos;
  };

  template<typename Iter>
  class ReverseIterator {
   public:
    explicit ReverseIterator(Iter iter) : iter(iter - 1) {};

    typename Iter::pointer operator->() const { return &(*iter); }

    typename Iter::reference operator*() const { return *iter; }

    ReverseIterator operator++(int) const { return iter--; }

    ReverseIterator operator--(int) const { return iter++; }

    ReverseIterator& operator++() {
      --iter;
      return *this;
    }

    ReverseIterator& operator--() {
      ++iter;
      return *this;
    }

    ReverseIterator& operator+=(typename Iter::difference_type dif) {
      iter -= dif;
      return *this;
    }

    ReverseIterator& operator-=(typename Iter::difference_type dif) {
      iter += dif;
      return *this;
    }

    ReverseIterator operator+(typename Iter::difference_type dif) const {
      return iter - dif;
    }

    ReverseIterator operator-(typename Iter::difference_type dif) const {
      return iter + dif;
    }

    friend bool operator==(ReverseIterator rita_1, ReverseIterator rita_2) {
      return rita_1.iter == rita_2.iter;
    }

    friend bool operator!=(ReverseIterator rita_1, ReverseIterator rita_2) {
      return rita_1.iter != rita_2.iter;
    }

    friend bool operator>(ReverseIterator rita_1, ReverseIterator rita_2) {
      return rita_1.iter < rita_2.iter;
    }

    friend bool operator<(ReverseIterator rita_1, ReverseIterator rita_2) {
      return rita_1.iter > rita_2.iter;
    }

    friend bool operator>=(ReverseIterator rita_1, ReverseIterator rita_2) {
      return rita_1.iter <= rita_2.iter;
    }

    friend bool operator<=(ReverseIterator rita_1, ReverseIterator rita_2) {
      return rita_1.iter >= rita_2.iter;
    }

    friend typename Iter::difference_type operator-(const ReverseIterator& rita_1, const ReverseIterator& rita_2) {
      return -(rita_1.iter - rita_2.iter);
    }

    explicit operator const_reverse_iterator() const { return const_reverse_iterator(iter); }

    ~ReverseIterator() = default;

   private:
    Iter iter;
  };

  iterator begin() {
    auto mp = pointers + start / carriage_capacity;
    return iterator(mp, *mp + start % carriage_capacity);
  };

  iterator end() {
    auto amount = start + capacity;
    auto mp = pointers + amount / carriage_capacity;
    return iterator(mp, *mp + amount % carriage_capacity);
  };

  [[nodiscard]] const_iterator begin() const {
    return cbegin();
  };

  [[nodiscard]] const_iterator end() const {
    return cend();
  };

  [[nodiscard]] const_iterator cbegin() const {
    auto mp = pointers + start / carriage_capacity;
    return const_iterator(mp, *mp + start % carriage_capacity);
  }

  const_iterator cend() const {
    auto amount = start + size();
    auto mp = pointers + amount / carriage_capacity;
    return const_iterator(mp, capacity == 0 ? nullptr : *mp + amount % carriage_capacity);

  }

  reverse_iterator rbegin() { return reverse_iterator(end()); }

  reverse_iterator rend() { return reverse_iterator(begin()); }

  const_reverse_iterator rbegin() const { return crbegin(); }
  const_reverse_iterator rend() const { return crend(); }

  const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

  const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

  void erase(iterator iter);

  void insert(iterator iter, const Type& ptr);

 protected:
  size_t capacity = 0;
  size_t external_size = 0;
  size_t start = 0;
  Type** pointers = nullptr;
};

template<typename Type>
size_t Deque<Type>::size() const {
  return capacity;
}

template<typename Type>
void Deque<Type>::resize() {
  try {
    Deque<Type> tmp(external_size* carriage_capacity);
    for (size_t i = 0; i < external_size; ++i) {
      tmp.pointers[external_size + i] = pointers[i];
    }
    tmp.start = (tmp.external_size * carriage_capacity) / 3 + start;
    tmp.capacity = capacity;
    delete[] pointers;
    *this = tmp;
  } catch (...) {
    throw;
  }
}

template<typename Type>
void Deque<Type>::pop_front() {
  ++start;
  --capacity;
  if (capacity == 0) start = external_size / 3;
}

template<typename Type>
void Deque<Type>::pop_back() {
  --capacity;
  if (capacity == 0) start = external_size / 3;
}

template<typename Type>
void Deque<Type>::push_front(const Type& ptr) {
  if (start == 0) {
    resize();
  }
  --start;
  pointers[start / carriage_capacity][start % carriage_capacity] = ptr;
  ++capacity;
}

template<typename Type>
void Deque<Type>::push_back(const Type& ptr) {
  if (start + capacity == external_size * carriage_capacity) {
    resize();
  }
  pointers[(start + capacity) / carriage_capacity][(start + capacity) % carriage_capacity] = ptr;
  ++capacity;
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
    *this = tmp;
  } catch (...) {
    throw std::string("bad erase");
  }
}

template<typename Type>
void Deque<Type>::insert(Deque::iterator iter, const Type& ptr) {
  try {
    Deque<Type> tmp;
    for (auto i = begin(); i < iter; ++i) {
      tmp.push_back(*i);
    }
    tmp.push_back(ptr);
    for (auto i = iter; i < end(); ++i) {
      tmp.push_back(*i);
    }
    *this = tmp;
  } catch (...) {
    throw std::string("bad insert");
  }
}


