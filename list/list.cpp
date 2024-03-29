#include "stackallocator.cpp"

template<typename T, typename Alloc = std::allocator<T>>
class List {
 public:
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef Alloc allocator_type;
  typedef std::allocator_traits<allocator_type> alloc_traits;

  template<bool is_const>
  class BidirectionalIterator;

  typedef BidirectionalIterator<false> iterator;
  typedef BidirectionalIterator<true> const_iterator;
  typedef std::reverse_iterator<BidirectionalIterator<false>> reverse_iterator;
  typedef std::reverse_iterator<BidirectionalIterator<true>> const_reverse_iterator;

  List();
  template<class U, typename OtherAlloc>
  List(const List<U, OtherAlloc>&);
  List(const List<T, Alloc>&);
  List(size_t);
  List(size_t, const_reference);

  List(allocator_type other_alloc);
  List(size_t, const_reference, allocator_type);
  List(size_t, allocator_type);
  List<T, Alloc>& operator=(const List<T, Alloc>& other);
  size_t size() const {
    return capacity;
  }
  allocator_type get_allocator() const {
    return alloc;
  }
  [[nodiscard]] bool empty() const {
    return capacity == 0;
  }
  void push_front(const_reference ref) {
    Node* new_node = alloc.allocate(1);
    try {
      std::allocator_traits<node_alloc>::construct(alloc, new_node, ref);
      fake_node->next->prev = new_node;
      new_node->next = fake_node->next;
      new_node->prev = fake_node;
      fake_node->next = new_node;
      ++capacity;
    } catch (...) {
      std::allocator_traits<node_alloc>::deallocate(alloc, new_node, ref);
      throw std::string("bad push_front");
    }
  }
  void push_back(const_reference ref) {
    Node* new_node = alloc.allocate(1);
    try {
      std::allocator_traits<node_alloc>::construct(alloc, new_node, ref);
      if (capacity == 0) {
        new_node->prev = fake_node;
        fake_node->next = new_node;
      } else {
        fake_node->prev->next = new_node;
        new_node->prev = fake_node->prev;
      }
      new_node->next = fake_node;
      fake_node->prev = new_node;
      ++capacity;
    } catch (...) {
      std::allocator_traits<node_alloc>::deallocate(alloc, new_node, 1);
      throw std::string("bad push_back");
    }
  }
  iterator insert(const_iterator iter, const_reference ref) {
    Node* new_node = alloc.allocate(1);
    try {
      std::allocator_traits<node_alloc>::construct(alloc, new_node, ref);
      new_node->next = iter.get_node();
      new_node->prev = iter.get_node()->prev;
      new_node->prev->next = new_node;
      iter.get_node()->prev = new_node;
      ++capacity;
      return iterator(new_node);
    } catch (...) {
      std::allocator_traits<node_alloc>::deallocate(alloc, new_node, ref);
      throw std::string("bad insert()");
    }
  }
  iterator erase(const_iterator iter) {
    try {
      if (iter.get_node()->prev == fake_node) {
        pop_front();
        return iterator(fake_node);
      } else {
        iterator tmp(iter.get_node());
        auto delete_address = static_cast<Node*>(iter.get_node());

        tmp.get_node()->next->prev = tmp.get_node()->prev;
        tmp.get_node()->prev->next = tmp.get_node()->next;
        --capacity;

        std::allocator_traits<node_alloc>::destroy(alloc, delete_address);
        std::allocator_traits<node_alloc>::deallocate(alloc, delete_address, 1);

        return iterator(tmp.get_node()->prev);
      }
    } catch (...) {
      throw std::string("bad erase()");
    }
  }
  void pop_front() {
    if (!empty()) {
      try {
        auto delete_address = static_cast<Node*>(fake_node->next);
        fake_node->next = fake_node->next->next;
        fake_node->next->prev = fake_node;
        std::allocator_traits<node_alloc>::destroy(alloc, delete_address);
        std::allocator_traits<node_alloc>::deallocate(alloc, delete_address, 1);
      } catch (...) {
        throw std::string("bad pop_front()");
      }
    } else {
      throw std::out_of_range("bad pop_front()");
    }
    --capacity;
  }
  void pop_back() {
    if (!empty()) {
      try {
        auto delete_address = static_cast<Node*>(fake_node->prev);
        fake_node->prev = fake_node->prev->prev;
        fake_node->prev->next = fake_node;
        std::allocator_traits<node_alloc>::destroy(alloc, delete_address);
        std::allocator_traits<node_alloc>::deallocate(alloc, delete_address, 1);
      } catch (...) {
        throw std::string("bad pop_back()");
      }
    } else {
      throw std::out_of_range("bad pop_front()");
    }
    --capacity;
  }
  void clear() {
    while (capacity > 0) {
      pop_back();
    }
  }
  void emplace_back() {
    Node* new_node = alloc.allocate(1);
    try {
      std::allocator_traits<node_alloc>::construct(alloc, new_node);
      if (capacity == 0) {
        new_node->prev = fake_node;
        fake_node->next = new_node;
      } else {
        fake_node->prev->next = new_node;
        new_node->prev = fake_node->prev;
      }
      new_node->next = fake_node;
      fake_node->prev = new_node;
    } catch (...) {
      std::allocator_traits<node_alloc>::deallocate(alloc, new_node, 1);
      throw std::string("bad emplace_back");
    }
    ++capacity;
  }

  iterator begin() { return iterator(fake_node->next); }
  iterator end() { return iterator(fake_node); }
  const_iterator begin() const { return cbegin(); }
  const_iterator end() const { return cend(); }
  const_iterator cbegin() const { return const_iterator(fake_node->next); }
  const_iterator cend() const { return const_iterator(fake_node); }
  reverse_iterator rbegin() { return reverse_iterator(end()); }
  reverse_iterator rend() { return reverse_iterator(begin()); }
  const_reverse_iterator rbegin() const { return crbegin(); }
  const_reverse_iterator rend() const { return crend(); }
  const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
  const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

  ~List();

 private:
  struct BaseNode {
    BaseNode* next;
    BaseNode* prev;
    BaseNode(BaseNode* first, BaseNode* second) : prev(first), next(second) {};
    BaseNode() = default;
    ~BaseNode() = default;
  };
  struct Node : BaseNode {
    T value;
    Node(const T& other) : value(other) {};
    Node() = default;
    ~Node() = default;
  };

  size_t capacity;
  typedef typename alloc_traits::template rebind_alloc<Node> node_alloc;
  node_alloc alloc;
  mutable BaseNode* fake_node = std::allocator_traits<node_alloc>::allocate(alloc, 1);

 public:
  template<bool is_const = false>
  class BidirectionalIterator {
   public:
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef T value_type;
    typedef T difference_type;
    typedef typename std::conditional<is_const, const T*, T*>::type pointer;
    typedef typename std::conditional<is_const, const T&, T&>::type reference;

    BidirectionalIterator() : node(nullptr) {};
    BidirectionalIterator(const BidirectionalIterator& other) : node(other.node) {};
    BidirectionalIterator& operator=(const BidirectionalIterator& other) {
      node = other.node;
      return *this;
    }
    BidirectionalIterator(BaseNode* new_node) : node(new_node) {};
    pointer operator->() const { return &static_cast<Node*>(node)->value; }
    reference operator*() const { return static_cast<Node*>(node)->value; }
    BidirectionalIterator& operator++() {
      node = node->next;
      return *this;
    }
    BidirectionalIterator operator++(int) {
      BidirectionalIterator tmp = *this;
      ++(*this);
      return tmp;
    }
    BidirectionalIterator& operator--() {
      node = node->prev;
      return *this;
    }
    BidirectionalIterator operator--(int) {
      BidirectionalIterator tmp = *this;
      --(*this);
      return tmp;
    }
    friend bool operator==(const BidirectionalIterator& x, const BidirectionalIterator& y) {
      return x.node == y.node;
    }
    friend bool operator!=(const BidirectionalIterator& x, const BidirectionalIterator& y) {
      return x.node != y.node;
    }
    BaseNode* get_node() const { return node; }
    operator const_iterator() const { return const_iterator(node); }
    ~BidirectionalIterator() = default;
   private:
    BaseNode* node;
  };
};

template<typename T, typename Alloc>
List<T, Alloc>::List() {
  capacity = 0;
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t n) {
  capacity = 0;
  for (size_t i = 0; i < n; ++i) {
    try {
      emplace_back();
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        pop_back();
      }
      std::allocator_traits<node_alloc>::deallocate(alloc, static_cast<Node*>(fake_node), 1);
      throw std::string("bad constructor(size_t)");
    }
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t n, const_reference ref) {
  capacity = 0;
  for (size_t i = 0; i < n; ++i) {
    try {
      push_back(ref);
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        pop_back();
      }
      std::allocator_traits<node_alloc>::deallocate(alloc, static_cast<Node*>(fake_node), 1);
      throw std::string("bad constructor(size_t, ref)");
    }
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t n, const_reference ref, allocator_type other_alloc) : alloc(node_alloc(other_alloc)) {
  capacity = 0;
  for (size_t i = 0; i < n; ++i) {
    try {
      push_back(ref);
    } catch (...) {
      for (size_t j = 0; j < i; ++j) {
        pop_back();
      }
      std::allocator_traits<node_alloc>::deallocate(alloc, static_cast<Node*>(fake_node), 1);
      throw std::string("bad constructor(size_t, ref, alloc)");
    }
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(size_t n, allocator_type other_alloc)  : alloc(node_alloc(other_alloc)) {
  capacity = 0;
  for (size_t i = 0; i < n; ++i) {
    emplace_back();
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::~List() {
  clear();
  std::allocator_traits<node_alloc>::deallocate(alloc, static_cast<Node*>(fake_node), 1);
}

template<typename T, typename Alloc>
List<T, Alloc>& List<T, Alloc>::operator=(const List<T, Alloc>& other) {
  if (alloc_traits::propagate_on_container_copy_assignment::value)
    alloc = other.alloc;
  size_t counter = 0;
  for (auto it = other.begin(); it != other.end(); ++it) {
    try {
      push_back(*it);
      ++counter;
    } catch (...) {
      for (size_t i = 0; i < counter; ++i) {
        pop_back();
      }
      throw std::string("bad operator=");
    }
  }
  for (size_t i = 0; capacity > counter; ++i) {
    pop_front();
  }
  return *this;
}

template<typename T, typename Alloc>
List<T, Alloc>::List(allocator_type other_alloc) : alloc(node_alloc(other_alloc)) {
  capacity = 0;
}

template<typename T, typename Alloc>
template<class U, typename OtherAlloc>
List<T, Alloc>::List(const List<U, OtherAlloc>& other) : alloc(other.alloc) {
  size_t counter = 0;
  capacity = 0;
  for (auto it = other.begin(); it != other.end(); ++it) {
    try {
      push_back(*it);
      ++counter;
    } catch (...) {
      for (size_t i = 0; i < counter; ++i) {
        pop_back();
      }
      throw std::string("bad copy constructor");
    }
  }
}

template<typename T, typename Alloc>
List<T, Alloc>::List(const List<T, Alloc>& other) : alloc(other.alloc) {
  size_t counter = 0;
  alloc = std::allocator_traits<node_alloc>::select_on_container_copy_construction(other.alloc);
  capacity = 0;
  for (auto it = other.begin(); it != other.end(); ++it) {
    try {
      push_back(*it);
      ++counter;
    } catch (...) {
      for (size_t i = 0; i < counter; ++i) {
        pop_back();
      }
      throw std::string("bad copy constructor");
    }
  }
}