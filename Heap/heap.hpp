// http://leetcode.com/problems/minimum-operations-to-exceed-threshold-value-ii/description

#include <functional>
template <typename T, typename Comparator = std::less>
class Heap {
    Heap(Comparator &cmp): _cmp(move(cmp)) {}
    Heap();
    void push(T &value) {
        _data.push_back(value);
        upheap(_data.size() - 1);
    }
    auto empty() -> bool {
        return _data.size() == 0;
    }
    void pop();
    void pop(int index);

    private:
    static inline int get_parent(int index) {return ((index - 1)>>1);}
    static inline int left_child(int index) { return 2*index + 1; }
    static inline int right_child(int index) { return 2*index + 2; }
    static inline get_root() { return 0; }
    void upheap(int index) {
        T value = move(_data[index]);
        while(index > get_root()) {
            int parent = get_parent(index);
            if (_cmp(value, data[parent])) break;
            _data[index] = move(_data[parent]);
            if (_cmp(data[index], value)) swap(data[index], value);
            index = parent;
        }

        _data[index] = move(value);
    }

    void downheap(int index) {
        T value = move(_data[index]);
        int picked_child = 
    }
    vector<T> _data;
    Comparator _cmp;

}
template<typename T, typename Compare = std::less>
class BinaryHeap {
 public:
  BinaryHeap() { }
  explicit BinaryHeap(Compare cmp) : cmp_(std::move(cmp)) { }

  void push(const T& value) {
    data_.push_back(value);
    upheap(data_.size() - 1);
  }

  void push(T&& value) {
    data_.push_back(std::move(value));
    upheap(data_.size() - 1);
  }

  const T& top() const {
    return data_.front();
  }

  void replace_top(const T& value) {
    data_.front() = value;
    downheap(get_root());
  }

  void replace_top(T&& value) {
    data_.front() = std::move(value);
    downheap(get_root());
  }

  void pop(int index)
  {
      if (index >= data_.size())
          return;

      data_[index] = std::move(data_.back());
      data_.pop_back();

      auto parent = get_parent(index);
      if (cmp_(data_[parent], data_[index])) {
          return upheap(index);
      } else {
          return downheap(index);
      }
  }

  void pop() {
    data_.front() = std::move(data_.back());
    data_.pop_back();
    downheap(get_root());
  }

  void swap(BinaryHeap &other) {
    std::swap(cmp_, other.cmp_);
    data_.swap(other.data_);
    std::swap(root_cmp_cache_, other.root_cmp_cache_);
  }

  void clear() {
    data_.clear();
    reset_root_cmp_cache();
  }

  bool empty() const { return data_.empty(); }

  size_t size() const { return data_.size(); }

  void reset_root_cmp_cache() {
    root_cmp_cache_ = std::numeric_limits<size_t>::max();
  }

 private:
  static inline size_t get_root() { return 0; }
  static inline size_t get_parent(size_t index) { return (index - 1) / 2; }
  static inline size_t get_left(size_t index) { return 2 * index + 1; }
  static inline size_t get_right(size_t index) { return 2 * index + 2; }

  void upheap(size_t index) {
    T v = std::move(data_[index]);
    while (index > get_root()) {
      const size_t parent = get_parent(index);
      if (cmp_(v, data_[parent])) {
        break;
      }
      data_[index] = std::move(data_[parent]);
      index = parent;
    }
    data_[index] = std::move(v);
    reset_root_cmp_cache();
  }

  void downheap(size_t index) {
    T v = std::move(data_[index]);

    size_t picked_child = std::numeric_limits<size_t>::max();
    while (1) {
      const size_t left_child = get_left(index);
      if (get_left(index) >= data_.size()) {
        break;
      }
      const size_t right_child = left_child + 1;
      picked_child = left_child;
      if (index == 0 && root_cmp_cache_ < data_.size()) {
        picked_child = root_cmp_cache_;
      } else if (right_child < data_.size() &&
                 cmp_(data_[left_child], data_[right_child])) {
        picked_child = right_child;
      }
      if (cmp_(data_[picked_child], v)) {
        break;
      }
      data_[index] = std::move(data_[picked_child]);
      index = picked_child;
    }

    if (index == 0) {
      root_cmp_cache_ = picked_child;
    } else {
      reset_root_cmp_cache();
    }

    data_[index] = std::move(v);
  }

  Compare cmp_;
  std::vector<T> data_;
  size_t root_cmp_cache_ = std::numeric_limits<size_t>::max();
};
