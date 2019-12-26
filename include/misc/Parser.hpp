// Author:          Julius Hülsmann
// Email:           juliusHuelsmann@gmail.com, julius.huelsmann@data-spree.com
// Creation Date:   12/24/19

// YYY: take extensions from my dbn framework.

#ifndef MISC_PARSER_HPP
#define MISC_PARSER_HPP

#include <zmq.hpp>

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <set>
#include <type_traits>

namespace parser {

using BasicType = char;

template <typename T, typename K, typename SizeType,
          typename std::enable_if_t<std::is_integral<SizeType>::value> * = nullptr>
inline void copy(T *anchor, K const *src, SizeType size, SizeType &offset) noexcept {
  std::memcpy(&(reinterpret_cast<BasicType *>(anchor)[std::exchange(offset, offset + size)]),
              reinterpret_cast<BasicType const *>(src), size);
}

template <typename T, typename K, typename SizeType,
          typename std::enable_if_t<std::is_integral<SizeType>::value> * = nullptr>
inline void copySwap(T *anchor, K const *src, SizeType size, SizeType &offset) noexcept {
  std::memcpy(reinterpret_cast<BasicType *>(anchor),
              &(reinterpret_cast<BasicType const *>(src)[std::exchange(offset, offset + size)]), size);
}

/// Return copy of the memory chunk
/// memory[#offset: #offset + sizeof(T)] interpreted as #ReturnType.
template <typename ReturnType, typename A, typename SizeType,
          typename std::enable_if_t<std::is_integral<SizeType>::value> * = nullptr>
inline ReturnType get(A const *memory, SizeType &offset) noexcept {
  return *reinterpret_cast<ReturnType const *>(
      &reinterpret_cast<BasicType const *>(memory)[std::exchange(offset, offset + sizeof(ReturnType))]);
}

template <typename ParameterType, typename A, typename SizeType,
          typename std::enable_if_t<std::is_integral<SizeType>::value> * = nullptr>
inline void get(A const *memory, ParameterType &val, SizeType &offset) noexcept {
  val = *reinterpret_cast<ParameterType const *>(
      &reinterpret_cast<BasicType const *>(memory)[std::exchange(offset, offset + sizeof(ParameterType))]);
}

template <typename ParameterType, typename A, typename SizeType,
          typename std::enable_if_t<std::is_integral<SizeType>::value> * = nullptr>
inline void set(A *memory, ParameterType val, SizeType &offset) noexcept {
  *reinterpret_cast<ParameterType *>(
      &(reinterpret_cast<BasicType *>(memory)[std::exchange(offset, offset + sizeof(ParameterType))])) = val;
}

/// Utility to the set function that can be used with arbitrary numeric (const) types of #val.
#define SET(pntr, val, idx) set<std::remove_const_t<decltype(val)>>(pntr, val, idx)

} // namespace parser

#endif // MISC_PARSER_HPP
