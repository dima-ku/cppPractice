#pragma once

#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>

template <class T>
class matrix {
private:
  template <class S>
  struct basic_col_iterator {
    using value_type = T;
    using reference = S&;
    using pointer = S*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::random_access_iterator_tag;

    basic_col_iterator() = default;

    basic_col_iterator& operator++() {
      _data += _cnt_col;
      return *this;
    };

    basic_col_iterator operator++(int) {
      basic_col_iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    basic_col_iterator& operator--() {
      _data -= _cnt_col;
      return *this;
    }

    basic_col_iterator operator--(int) {
      basic_col_iterator tmp = *this;
      --(*this);
      return tmp;
    }

    basic_col_iterator& operator+=(difference_type rhs) {
      _data += static_cast<difference_type>(rhs * _cnt_col);
      return *this;
    }

    basic_col_iterator& operator-=(difference_type rhs) {
      (*this) += -rhs;
      return *this;
    }

    friend basic_col_iterator operator+(basic_col_iterator lhs, difference_type rhs) {
      return rhs + lhs;
    }

    friend basic_col_iterator operator+(difference_type lhs, basic_col_iterator rhs) {
      return {static_cast<difference_type>(lhs * rhs._cnt_col) + rhs._data, rhs._col_index, rhs._cnt_col};
    }

    friend difference_type operator-(const basic_col_iterator lhs, const basic_col_iterator rhs) {
      assert(lhs._cnt_col == rhs._cnt_col && lhs._col_index == rhs._col_index);
      return (lhs._data - rhs._data) / (static_cast<difference_type>(lhs._cnt_col));
    }

    friend basic_col_iterator operator-(basic_col_iterator lhs, difference_type rhs) {
      return -rhs + lhs;
    }

    reference operator[](difference_type n) const {
      return (_data + static_cast<difference_type>(n * _cnt_col))[_col_index];
    }

    friend bool operator>(basic_col_iterator lhs, basic_col_iterator rhs) {
      return (lhs._data - rhs._data) > 0;
    }

    friend bool operator>=(basic_col_iterator lhs, basic_col_iterator rhs) {
      return (lhs._data - rhs._data) >= 0;
    }

    friend bool operator<(basic_col_iterator lhs, basic_col_iterator rhs) {
      return (lhs._data - rhs._data) < 0;
    }

    friend bool operator<=(basic_col_iterator lhs, basic_col_iterator rhs) {
      return (lhs._data - rhs._data) <= 0;
    }

    reference operator*() const {
      return _data[_col_index];
    }

    pointer operator->() const {
      return _data + _col_index;
    }

    friend bool operator==(const basic_col_iterator lhs, const basic_col_iterator rhs) {
      return lhs._data == rhs._data && lhs._col_index == rhs._col_index;
    }

    friend bool operator!=(const basic_col_iterator lhs, const basic_col_iterator rhs) {
      return !(lhs == rhs);
    }

    operator basic_col_iterator<const S>() const {
      return {_data, _col_index, _cnt_col};
    }

  private:
    basic_col_iterator(pointer data, size_t col_index, size_t cnt_col)
        : _data(data),
          _col_index(col_index),
          _cnt_col(cnt_col) {}

    friend matrix;

  private:
    pointer _data;
    size_t _col_index;
    size_t _cnt_col;
  };

  ////////////////////////////////////////////////////////////////////////////////////////

public:
  using value_type = T;

  using reference = T&;
  using const_reference = const T&;

  using pointer = T*;
  using const_pointer = const T*;

  using iterator = T*;
  using const_iterator = const T*;

  using row_iterator = T*;
  using const_row_iterator = const T*;

  using col_iterator = basic_col_iterator<T>;
  using const_col_iterator = basic_col_iterator<const T>;

public:
  matrix() : _data(nullptr), _rows(0), _cols(0) {}

  matrix(size_t rows, size_t cols)
      : _data((rows == 0 || cols == 0) ? nullptr : new T[rows * cols]{}),
        _rows(_data == nullptr ? 0 : rows),
        _cols(_data == nullptr ? 0 : cols) {}

  template <size_t Rows, size_t Cols>
  matrix(const T (&arr)[Rows][Cols]) : _data(new T[Rows * Cols]),
                                       _rows(Rows),
                                       _cols(Cols) {
    iterator it = begin();
    for (const T(&row)[Cols] : arr) {
      it = std::copy_n(row, cols(), it);
    }
  }

  matrix(const matrix<T>& other)
      : _data(other.empty() ? nullptr : new T[other.size()]),
        _rows(other.empty() ? 0 : other.rows()),
        _cols(other.empty() ? 0 : other.cols()) {
    std::copy(other.begin(), other.end(), begin());
  }

  void swap(matrix<T>& other) {
    std::swap(_data, other._data);
    std::swap(_rows, other._rows);
    std::swap(_cols, other._cols);
  }

  matrix& operator=(const matrix<T>& other) {
    if (&other != this) {
      matrix(other).swap(*this);
    }
    return *this;
  }

  ~matrix() {
    delete[] _data;
  }

  // Iterators

  iterator begin() {
    return data();
  };

  const_iterator begin() const {
    return data();
  };

  iterator end() {
    return data() + size();
  };

  const_iterator end() const {
    return data() + size();
  }

  row_iterator row_begin(size_t row) {
    return data() + row * cols();
  }

  const_row_iterator row_begin(size_t row) const {
    return data() + row * cols();
  }

  row_iterator row_end(size_t row) {
    return data() + (row + 1) * cols();
  }

  const_row_iterator row_end(size_t row) const {
    return data() + (row + 1) * cols();
  }

  col_iterator col_begin(size_t col) {
    return {begin(), col, cols()};
  }

  const_col_iterator col_begin(size_t col) const {
    return {begin(), col, cols()};
  };

  col_iterator col_end(size_t col) {
    return {end(), col, cols()};
  }

  const_col_iterator col_end(size_t col) const {
    return {end(), col, cols()};
  }

  // Size

  size_t rows() const {
    return _rows;
  }

  size_t cols() const {
    return _cols;
  }

  size_t size() const {
    return rows() * cols();
  }

  bool empty() const {
    return size() == 0;
  }

  // Elements access

  reference operator()(size_t row, size_t col) {
    return _data[row * cols() + col];
  }

  const_reference operator()(size_t row, size_t col) const {
    return _data[row * cols() + col];
  };

  pointer data() {
    return _data;
  }

  const_pointer data() const {
    return _data;
  }

  // Comparison

  friend bool operator==(const matrix& left, const matrix& right) {
    return left.cols() == right.cols() && left.rows() == right.rows() &&
           std::equal(left.begin(), left.end(), right.begin(), right.end());
  };

  friend bool operator!=(const matrix& left, const matrix& right) {
    return !(left == right);
  }

  // Arithmetic operations

  matrix& operator+=(const matrix& other) {
    std::transform(begin(), end(), other.begin(), begin(), std::plus<>{});
    return *this;
  }

  matrix& operator-=(const matrix& other) {
    std::transform(begin(), end(), other.begin(), begin(), std::minus<>{});
    return *this;
  }

  matrix& operator*=(const matrix& other) {
    matrix result = (*this) * other;
    (*this).swap(result);
    return *this;
  }

  matrix& operator*=(const_reference factor) {
    std::transform(begin(), end(), begin(), [factor](value_type el) { return factor * el; });
    return *this;
  }

  friend matrix operator+(const matrix& left, const matrix& right) {
    matrix result = left;
    result += right;
    return result;
  }

  friend matrix operator-(const matrix& left, const matrix& right) {
    matrix result = left;
    result -= right;
    return result;
  }

  friend matrix operator*(const matrix& left, const matrix& right) {
    matrix result = matrix(left.rows(), right.cols());
    for (int i = 0; i < left.rows(); i++) {
      for (int j = 0; j < right.cols(); j++) {
        result(i, j) = std::inner_product(left.row_begin(i), left.row_end(i), right.col_begin(j), value_type{});
      }
    }
    return result;
  }

  friend matrix operator*(const matrix& left, const_reference right) {
    matrix result = left;
    result *= right;
    return result;
  }

  friend matrix operator*(const_reference left, const matrix& right) {
    return right * left;
  }

private:
  T* _data;
  size_t _rows;
  size_t _cols;
};
