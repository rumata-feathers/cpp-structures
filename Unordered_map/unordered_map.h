#ifndef UNORDERED_MAP__UNORDERED_MAP_H_
#define UNORDERED_MAP__UNORDERED_MAP_H_
#include <iostream>
#include <algorithm>
#include <memory>
#include <array>
#include <vector>

template<class Key,
    class Value,
    class Hash = std::hash<Key>,
    class Equal = std::equal_to<Key>,
    class Alloc = std::allocator<
        std::pair<Key, Value>>>

class UnorderedMap {

 public:
  typedef Key key_type;
  typedef Value mapped_type;
  typedef Hash hasher;
  typedef Equal key_equal;
  typedef std::allocator_traits<Alloc> allocator_type;
  typedef std::pair<const key_type, mapped_type> value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef typename std::allocator_traits<allocator_type>::pointer pointer;
  typedef typename std::allocator_traits<allocator_type>::const_pointer const_pointer;
  typedef typename std::allocator_traits<allocator_type>::size_type size_type;
  typedef typename std::allocator_traits<allocator_type>::difference_type difference_type;
  typedef std::pair<Key, Value> NodeType;

  template<bool is_const>
  class NodeIterator;

  typedef NodeIterator<false> iterator;
  typedef NodeIterator<true> const_iterator;

  UnorderedMap();
  explicit UnorderedMap(const allocator_type&);
  UnorderedMap(const UnorderedMap&);
  UnorderedMap(const UnorderedMap&, const Alloc&);
  UnorderedMap(size_type, const allocator_type&);
  UnorderedMap(UnorderedMap&&) noexcept;

  UnorderedMap& operator=(const UnorderedMap&);
  UnorderedMap& operator=(UnorderedMap&&) noexcept;

  mapped_type& operator[](const key_type& k);
  mapped_type& operator[](key_type&& k);

  mapped_type& at(const key_type& k);
  const mapped_type& at(const key_type& k) const;

  size_type bucket_count() const noexcept;
  size_type max_bucket_count() const noexcept;

  size_type bucket_size(size_type n) const;
  size_type bucket(const key_type& k) const;

  float load_factor() const noexcept;
  float max_load_factor() const noexcept;
  void max_load_factor(float z);
  void rehash(size_type n);
  void reserve(size_type n);

  bool empty() const noexcept;
  size_type size() const noexcept;

  std::pair<iterator, bool> insert(const value_type& obj);
  std::pair<iterator, bool> insert(value_type&& obj);
  template<class P>
  std::pair<iterator, bool> insert(P&& obj);
  template<class InputIterator>
  void insert(InputIterator first, InputIterator last);

  template<class ... Args>
  std::pair<iterator, bool> emplace(Args&& ... args);
  template<class ... Args>
  std::pair<iterator, bool> try_emplace(const key_type& k, Args&& ... args);
  template<class ... Args>
  std::pair<iterator, bool> try_emplace(key_type&& k, Args&& ... args);

  iterator erase(const_iterator position);
  iterator erase(iterator position);
  size_type erase(const key_type& k);
  iterator erase(const_iterator first, const_iterator last);
  void clear();

  iterator find(const key_type& k);
  const_iterator find(const key_type& k) const;

  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  const_iterator cbegin() const noexcept;
  const_iterator cend() const noexcept;

 private:
  float max_load_factor_ = 0.67f;
  size_t map_size = 0;
  size_t start_bucket_amount = 10000;
  hasher map_hasher_;
  key_equal key_comparator;

  struct BaseNode {
    size_t hash = 0;
    BaseNode* prev = nullptr;
    BaseNode* next = nullptr;
    BaseNode() = default;
  };
  struct Node : public BaseNode {
    NodeType* kv;
    explicit Node(NodeType* other) : kv(other) {};
  };
  typedef typename allocator_type::template rebind_alloc<Node> node_allocator;
  typedef typename allocator_type::template rebind_alloc<NodeType> pair_allocator;
  node_allocator node_alloc_;
  pair_allocator pair_alloc_;
  mutable BaseNode* fake_node = std::allocator_traits<node_allocator>::allocate(node_alloc_, 1);
  std::vector<Node*> key_hash_map;
  size_t hash_by_size(size_t node_hash) { return node_hash % key_hash_map.size(); }
  void erase_hash(size_type k) {
    if (key_hash_map[hash_by_size(k)] != fake_node) {
      if (key_hash_map[hash_by_size(k)]->next->hash == hash_by_size(k)) {
        key_hash_map[hash_by_size(k)] = static_cast<Node*>(key_hash_map[hash_by_size(k)]->next);
      } else {
        key_hash_map[hash_by_size(k)] = static_cast<Node*>(fake_node);
      }
    }
  }

 public:
  template<bool is_const>
  class NodeIterator {
   public:
    typedef NodeType value_type;
    typedef long long difference_type;
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef typename std::conditional<is_const, const NodeType*, NodeType*>::type pointer;
    typedef typename std::conditional<is_const, const NodeType&, NodeType&>::type reference;

    NodeIterator() : node(nullptr) {};
    NodeIterator(const NodeIterator& other) noexcept : node(other.get_node()) {};
    NodeIterator(const NodeIterator<!is_const>& other) noexcept : node(other.get_node()) {};
    NodeIterator(BaseNode* new_node) noexcept : node(new_node) {};
    NodeIterator& operator=(const NodeIterator& other) {
      node = other.get_node();
      return *this;
    }
    pointer operator->() const noexcept { return &(*static_cast<Node*>(node)->kv); }
    reference operator*() const noexcept { return *(static_cast<Node*>(node)->kv); }
    NodeIterator& operator++() noexcept {
      node = node->next;
      return *this;
    }
    NodeIterator operator++(int) noexcept {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }
    NodeIterator& operator--() noexcept {
      node = node->prev;
      return *this;
    }
    NodeIterator operator--(int) noexcept {
      auto tmp = *this;
      (*this)--;
      return tmp;
    }
    friend bool operator==(const NodeIterator& bruce, const NodeIterator& barnie) {
      return bruce.node == barnie.node;
    }
    friend bool operator!=(const NodeIterator& bruce, const NodeIterator& barnie) {
      return bruce.node != barnie.node;
    }
    BaseNode* get_node() const noexcept { return node; }
    explicit operator const_iterator() const { return const_iterator(node); }
    ~NodeIterator() = default;
   private:
    BaseNode* node;
  };
};

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap() {
  std::allocator_traits<node_allocator>::construct(node_alloc_, fake_node);
  fake_node->prev = fake_node;
  fake_node->next = fake_node;
  key_hash_map = std::vector<Node*>(start_bucket_amount, static_cast<Node*>(fake_node));
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const allocator_type& other_alloc)
    : node_alloc_(std::allocator_traits<node_allocator>(other_alloc)) {
  std::allocator_traits<node_allocator>::construct(node_alloc_, fake_node);
  fake_node->prev = fake_node;
  fake_node->next = fake_node;
  key_hash_map = std::vector<Node*>(start_bucket_amount, static_cast<Node*>(fake_node));
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap& other_map)
    : node_alloc_(other_map.node_alloc_), pair_alloc_(other_map.pair_alloc_) {
  std::allocator_traits<node_allocator>::construct(node_alloc_, fake_node);
  fake_node->prev = fake_node;
  fake_node->next = fake_node;
  key_hash_map = std::vector<Node*>(start_bucket_amount, static_cast<Node*>(fake_node));
  insert(other_map.begin(), other_map.end());
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap&& other_map) noexcept
    : node_alloc_(other_map.node_alloc_), pair_alloc_(other_map.pair_alloc_) {
  std::allocator_traits<node_allocator>::construct(node_alloc_, fake_node);
  fake_node->prev = fake_node;
  fake_node->next = fake_node;
  key_hash_map = std::vector<Node*>(start_bucket_amount, static_cast<Node*>(fake_node));
}

template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(const UnorderedMap& other_map, const Alloc& other_alloc)
    : node_alloc_(other_alloc) {
  std::allocator_traits<node_allocator>::construct(node_alloc_, fake_node);
  fake_node->prev = fake_node;
  fake_node->next = fake_node;
  key_hash_map = std::vector<Node*>(start_bucket_amount, static_cast<Node*>(fake_node));
  insert(other_map.begin(), other_map.end());
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>::UnorderedMap(UnorderedMap::size_type amount,
                                                           const allocator_type& other_alloc)
    : node_alloc_(other_alloc) {
  std::allocator_traits<node_allocator>::construct(node_alloc_, fake_node);
  fake_node->prev = fake_node;
  fake_node->next = fake_node;
  key_hash_map = std::vector<Node*>(start_bucket_amount, static_cast<Node*>(fake_node));
  for (int i = 0; i < amount; ++i) emplace();
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>& UnorderedMap<Key,
                                                           Value,
                                                           Hash,
                                                           Equal,
                                                           Alloc>::operator=(const UnorderedMap& other) {
  try {
    if (std::allocator_traits<node_allocator>::propagate_on_container_copy_assignment::value)
      node_alloc_ = other.node_alloc_;
    if (std::allocator_traits<pair_allocator>::propagate_on_container_copy_assignment::value)
      pair_alloc_ = other.pair_alloc_;
    auto tmp = UnorderedMap(other, std::allocator_traits<pair_allocator>::propagate_on_container_copy_assignment::value
                                   ? other.node_alloc_
                                   : node_allocator());
    clear();
    key_hash_map = tmp.key_hash_map;
    fake_node = tmp.fake_node;
  } catch (...) {
    throw std::string("unordered_map::operator=: bad map construct");
  }
  return *this;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
UnorderedMap<Key, Value, Hash, Equal, Alloc>& UnorderedMap<Key,
                                                           Value,
                                                           Hash,
                                                           Equal,
                                                           Alloc>::operator=(UnorderedMap&& other) noexcept {
  if (std::allocator_traits<node_allocator>::propagate_on_container_copy_assignment::value)
    node_alloc_ = other.node_alloc_;
  if (std::allocator_traits<pair_allocator>::propagate_on_container_copy_assignment::value)
    pair_alloc_ = other.pair_alloc_;
  UnorderedMap tmp = UnorderedMap(std::move(other));
  clear();
  key_hash_map = tmp.key_hash_map;
  fake_node = tmp.fake_node;
  return *this;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::mapped_type& UnorderedMap<Key,
                                                                                 Value,
                                                                                 Hash,
                                                                                 Equal,
                                                                                 Alloc>::operator[](const key_type& k) {
  auto it = find(k);
  if (it == end()) {
    return try_emplace(k).first->second;
  } else {
    return it->second;
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::mapped_type& UnorderedMap<Key,
                                                                                 Value,
                                                                                 Hash,
                                                                                 Equal,
                                                                                 Alloc>::operator[](key_type&& k) {
  auto it = find(k);
  if (it == end()) {
    return try_emplace(k, mapped_type()).first->second;
  } else {
    return it->second;
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::mapped_type& UnorderedMap<Key, Value, Hash, Equal, Alloc>::at(
    const key_type& k) {
  iterator it = find(k);
  if (it == end())
    throw std::out_of_range("unordered_map::at: key not found");
  return it->second;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
const typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::mapped_type& UnorderedMap<Key,
                                                                                       Value,
                                                                                       Hash,
                                                                                       Equal,
                                                                                       Alloc>::at(const key_type& k) const {
  const_iterator it = find(k);
  if (it == cend())
    throw std::out_of_range("unordered_map::at: key not found");
  return it->second;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::size_type UnorderedMap<Key,
                                                                              Value,
                                                                              Hash,
                                                                              Equal,
                                                                              Alloc>::bucket_count() const noexcept {
  return key_hash_map.size();
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::size_type UnorderedMap<Key,
                                                                              Value,
                                                                              Hash,
                                                                              Equal,
                                                                              Alloc>::max_bucket_count() const noexcept {
  return key_hash_map.size();
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::size_type UnorderedMap<Key,
                                                                              Value,
                                                                              Hash,
                                                                              Equal,
                                                                              Alloc>::bucket_size(UnorderedMap::size_type n) const {
  try {
    Node* bucket_node = key_hash_map[n];
    hasher bucket_hash = bucket_node->hash;
    size_type bsize = 0;
    while (bucket_node != fake_node && hash_by_size(bucket_node->hash) == hash_by_size(bucket_hash)) {
      ++bsize;
      bucket_node = bucket_node->next;
    }
    return bsize;
  } catch (...) {
    throw std::out_of_range("unordered_map::bucket_size: bucket not found");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::size_type UnorderedMap<Key, Value, Hash, Equal, Alloc>::bucket(
    const key_type& k) const {
  try {
    return map_hasher_(k) % max_bucket_count();
  } catch (...) {
    throw std::invalid_argument("unordered_map::bucket: cannot get hash");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
float UnorderedMap<Key, Value, Hash, Equal, Alloc>::load_factor() const noexcept {
  size_type bc = bucket_count();
  return bc != 0 ? (float) size() / bc : 0.f;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
float UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor() const noexcept {
  return max_load_factor_;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::max_load_factor(float z) {
  if (z <= 0)
    throw std::out_of_range("unordered_map::max_load_factor(lf): called with lf <= 0");
  try {
    max_load_factor_ = std::max(max_load_factor_, z);
    rehash(0);
  } catch (...) {
    throw std::out_of_range("unordered_map::max_load_factor(lf): too low lf");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::rehash(UnorderedMap::size_type count) {
  if ((count + size()) / bucket_count() >= max_load_factor_) {
    try {
      std::vector<Node*> new_vec = std::vector<Node*>(2 * (count + size()), static_cast<Node*>(fake_node));
      BaseNode* move_node = fake_node->next;
      BaseNode* next_node = move_node->next;
      while (move_node != fake_node) {
        move_node->next->prev = move_node->prev;
        move_node->prev->next = move_node->next;
        erase_hash(move_node->hash);

        size_t new_hash = move_node->hash % new_vec.size();
        BaseNode* front_node = new_vec[new_hash];

        front_node->prev->next = move_node;
        move_node->prev = front_node->prev;
        front_node->prev = move_node;
        move_node->next = front_node;

        new_vec[new_hash] = static_cast<Node*>(move_node);

        move_node = next_node;
        next_node = next_node->next;
      }
      key_hash_map = new_vec;
    } catch (...) {
      throw;
    }
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::reserve(UnorderedMap::size_type amount) {
  if (amount < 0)
    throw std::invalid_argument("unordered_map::reserve: invalid amount");
  try {
    rehash(amount);
  } catch (...) {
    throw std::invalid_argument("unordered_map::reserve: invalid amount");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
bool UnorderedMap<Key, Value, Hash, Equal, Alloc>::empty() const noexcept {
  return map_size == 0;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::size_type UnorderedMap<Key,
                                                                              Value,
                                                                              Hash,
                                                                              Equal,
                                                                              Alloc>::size() const noexcept {
  return map_size;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool> UnorderedMap<Key,
                                                                                              Value,
                                                                                              Hash,
                                                                                              Equal,
                                                                                              Alloc>::insert(
    const UnorderedMap::value_type& obj) {
  try {
    if (find(obj.first) == end()) {
      iterator ins_it = try_emplace(obj.first, obj.second).first;
      return std::pair<iterator, bool>(ins_it, true);
    } else {
      return std::pair<iterator, bool>(find(obj.first), false);
    }
  } catch (...) {
    throw std::invalid_argument("unordered_map::insert(vl_t&): cannot insert vl_t");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class P>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool> UnorderedMap<Key,
                                                                                              Value,
                                                                                              Hash,
                                                                                              Equal,
                                                                                              Alloc>::insert(P&& obj) {
  try {
    return emplace(std::forward<P>(obj));
  } catch (...) {
    throw std::invalid_argument("unordered_map::insert(&&): cannot insert these arguments");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class InputIterator>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::insert(InputIterator first, InputIterator last) {
  for (; first != last; ++first)
    insert(*first);
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool> UnorderedMap<Key,
                                                                                              Value,
                                                                                              Hash,
                                                                                              Equal,
                                                                                              Alloc>::insert(
    UnorderedMap::value_type&& obj) {
  try {
    return emplace(std::move(obj));
  }
  catch (...) {
    throw std::invalid_argument("unordered_map::insert(vl_t&&): cannot insert");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(
    UnorderedMap::const_iterator position) {
  try {
    auto it = iterator(position);
    if (it != end()) {
      it.get_node()->next->prev = it.get_node()->prev;
      it.get_node()->prev->next = it.get_node()->next;
      auto res = iterator(it.get_node()->next);
      auto pair_address = static_cast<Node*>(it.get_node())->kv;
      auto delete_address = static_cast<Node*>(it.get_node());
      std::allocator_traits<pair_allocator>::destroy(pair_alloc_, pair_address);
      std::allocator_traits<pair_allocator>::deallocate(pair_alloc_, pair_address, 1);
      std::allocator_traits<node_allocator>::destroy(node_alloc_, delete_address);
      std::allocator_traits<node_allocator>::deallocate(node_alloc_, delete_address, 1);
      --map_size;
      erase_hash(it.get_node()->hash);
      return res;
    }
    return end();
  } catch (...) {
    throw std::invalid_argument("unordered_map::erase: cannot erase");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(
    UnorderedMap::iterator position) {
  return erase(const_iterator(position));
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::size_type UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(
    const key_type& k) {
  auto it = find(k);
  return erase(it);
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key, Value, Hash, Equal, Alloc>::erase(
    UnorderedMap::const_iterator first,
    UnorderedMap::const_iterator last) {
  //test for valid iterators
  auto s_it = iterator(first);
  auto l_it = iterator(last);
  while (s_it != l_it) {
    if (s_it == end()) {
      throw std::out_of_range("unordered_map::erase: bad iterators");
    }
    ++s_it;
  }
  auto start_it = iterator(first);
  auto last_it = iterator(last);
  while (start_it != last_it) {
    start_it = erase(start_it);
  }
  return last_it;
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
void UnorderedMap<Key, Value, Hash, Equal, Alloc>::clear(){
  try {
    while (begin() != end()) erase(begin());
  }catch (...) {
    throw std::bad_alloc();
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key,
                                                                             Value,
                                                                             Hash,
                                                                             Equal,
                                                                             Alloc>::find(const key_type& k) {
  try {
    size_type buc_number = bucket(k);
    iterator buc_it = const_iterator(key_hash_map[buc_number]);
    while (buc_it != end() && bucket(buc_it->first) == bucket(k)) {
      if (key_comparator(buc_it->first, k)) return buc_it;
      ++buc_it;
    }
    return end();
  } catch (...) {
    throw std::invalid_argument("unordered_map::find: cannot compare");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key,
                                                                                   Value,
                                                                                   Hash,
                                                                                   Equal,
                                                                                   Alloc>::find(const key_type& k) const {
  try{
    size_type buc_number = bucket(k);
    const_iterator buc_it = const_iterator(key_hash_map[buc_number]);
    while (buc_it != cend() && bucket(buc_it->first) == bucket(k)) {
      if (key_comparator(buc_it->first, k)) return buc_it;
      ++buc_it;
    }
    return cend();
  }catch(...) {
    throw std::invalid_argument("unordered_map:find: cannot find this key");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key,
                                                                             Value,
                                                                             Hash,
                                                                             Equal,
                                                                             Alloc>::begin() noexcept {
  return UnorderedMap::iterator(fake_node->next);
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator UnorderedMap<Key,
                                                                             Value,
                                                                             Hash,
                                                                             Equal,
                                                                             Alloc>::end() noexcept {
  return UnorderedMap::iterator(fake_node);
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key,
                                                                                   Value,
                                                                                   Hash,
                                                                                   Equal,
                                                                                   Alloc>::begin() const noexcept {
  return cbegin();
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key,
                                                                                   Value,
                                                                                   Hash,
                                                                                   Equal,
                                                                                   Alloc>::end() const noexcept {
  return cend();
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key,
                                                                                   Value,
                                                                                   Hash,
                                                                                   Equal,
                                                                                   Alloc>::cbegin() const noexcept {
  return UnorderedMap::const_iterator(fake_node->next);
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::const_iterator UnorderedMap<Key,
                                                                                   Value,
                                                                                   Hash,
                                                                                   Equal,
                                                                                   Alloc>::cend() const noexcept {
  return UnorderedMap::const_iterator(fake_node);
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool> UnorderedMap<Key,
                                                                                              Value,
                                                                                              Hash,
                                                                                              Equal,
                                                                                              Alloc>::try_emplace(const key_type& k,
                                                                                                                  Args&& ... args) {
  auto bc_node = key_hash_map[bucket(k)];
  auto s_it = iterator(bc_node);

  NodeType* new_pair = nullptr;
  Node* new_node = nullptr;
  try {
    try {
      new_pair = std::allocator_traits<pair_allocator>::allocate(pair_alloc_, 1);
      std::allocator_traits<pair_allocator>::construct(pair_alloc_, new_pair, k, std::forward<Args>(args)...);
    } catch (...) {
      std::allocator_traits<pair_allocator>::deallocate(pair_alloc_, new_pair, 1);
    }
    try {
      new_node = std::allocator_traits<node_allocator>::allocate(node_alloc_, 1);
      std::allocator_traits<node_allocator>::construct(node_alloc_, new_node, new_pair);
    } catch (...) {
      std::allocator_traits<node_allocator>::deallocate(node_alloc_, new_node, 1);
    }

    new_node->hash = map_hasher_(k);

    auto new_it = iterator(new_node);
    new_node->next = s_it.get_node();
    new_node->prev = s_it.get_node()->prev;
    s_it.get_node()->prev = new_node;
    new_node->prev->next = new_node;
    key_hash_map[new_node->hash % bucket_count()] = new_node;
    ++map_size;
    rehash(0);
    return std::pair<iterator, bool>(new_it, true);
  } catch (...) {
    throw std::string("unordered_map::try_emplace: cannot construct with this arguments");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool> UnorderedMap<Key,
                                                                                              Value,
                                                                                              Hash,
                                                                                              Equal,
                                                                                              Alloc>::emplace(Args&& ... args) {

  NodeType* new_pair = nullptr;
  Node* new_node = nullptr;
  try {
    new_pair = std::allocator_traits<pair_allocator>::allocate(pair_alloc_, 1);
    std::allocator_traits<pair_allocator>::construct(pair_alloc_, new_pair, std::forward<Args>(args)...);

    auto s_it = find(new_pair->first);
    if (s_it.get_node() == fake_node) {
      s_it = iterator(key_hash_map[bucket(new_pair->first)]);
      new_node = std::allocator_traits<node_allocator>::allocate(node_alloc_, 1);
      std::allocator_traits<node_allocator>::construct(node_alloc_, new_node, new_pair);

      new_node->hash = map_hasher_(new_pair->first);
      new_node->next = s_it.get_node();
      new_node->prev = s_it.get_node()->prev;
      s_it.get_node()->prev = new_node;
      new_node->prev->next = new_node;
      key_hash_map[hash_by_size(new_node->hash)] = new_node;
      ++map_size;
      rehash(0);
      return std::pair<iterator, bool>(iterator(new_node), true);
    } else {
      std::allocator_traits<pair_allocator>::destroy(pair_alloc_, new_pair);
      std::allocator_traits<pair_allocator>::deallocate(pair_alloc_, new_pair, 1);
      return std::pair<iterator, bool>(s_it, false);
    }
  } catch (...) {
    std::allocator_traits<pair_allocator>::deallocate(pair_alloc_, new_pair, 1);

    throw std::invalid_argument("unordered_map::emplace: cannot construct with this arguments");
  }
}
template<class Key, class Value, class Hash, class Equal, class Alloc>
template<class... Args>
std::pair<typename UnorderedMap<Key, Value, Hash, Equal, Alloc>::iterator, bool> UnorderedMap<Key,
                                                                                              Value,
                                                                                              Hash,
                                                                                              Equal,
                                                                                              Alloc>::try_emplace(
    key_type&& k,
    Args&& ... args) {

  auto bc_node = key_hash_map[bucket(k)];
  auto s_it = iterator(bc_node);
  while (s_it != end() && bucket(s_it->first) == bucket(k)) {
    if (key_comparator(s_it->first, k)) {
      return std::pair<iterator, bool>(s_it, false);
    }
    ++s_it;
  }
  try {
    NodeType* new_pair = std::allocator_traits<pair_allocator>::allocate(pair_alloc_, 1);
    std::allocator_traits<pair_allocator>::construct(pair_alloc_, new_pair, k, std::forward<Args>(args)...);
    Node* new_node = std::allocator_traits<node_allocator>::allocate(node_alloc_, 1);
    std::allocator_traits<node_allocator>::construct(node_alloc_, new_node, new_pair);
    new_node->hash = map_hasher_(k);

    auto new_it = iterator(new_node);
    new_node->next = s_it.get_node();
    s_it.get_node()->prev = new_node;
    new_node->prev = s_it.get_node()->prev;
    new_node->prev->next = new_node;
    key_hash_map[new_node->hash % bucket_count()] = new_node;
    ++map_size;
    if (bc_node == fake_node) key_hash_map[bucket(k)] = new_node;
    rehash(0);
    return std::pair<iterator, bool>(new_it, true);
  } catch (...) {
    throw std::string("unordered_map::try_emplace: cannot construct with this arguments");
  }
}

#endif //UNORDERED_MAP__UNORDERED_MAP_H_
