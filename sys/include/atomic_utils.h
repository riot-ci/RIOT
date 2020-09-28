/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    sys_atomic_utils    Utility functions for atomic access
 * @ingroup     sys
 *
 * This modules adds some utility functions to perform atomic accesses.
 *
 * # Usage
 *
 * The atomic utilitys allow atomic access to regular variables.
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *  uint32_t global_counter;
 *
 *  void irq_handler(void)
 *  {
 *      // No need to use atomic access in IRQ handlers: RIOT does not
 *      // enable nested interrupts, so IRQ handlers will not be interrupted
 *      // anyway
 *      global_counter++;
 *  }
 *
 *  void called_by_thread_a(void) {
 *      if (atomic_load_u32(&global_counter) > THRESHOLD) {
 *          on_threshold_reached();
 *          atomic_store_u32(&global_counter, 0);
 *      }
 *  }
 *
 *  void called_by_thread_b(void) {
 *      atomic_add_u32(&global_counter, 42);
 *  }
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * # Motivation
 * There are some reasons why these functions might be chosen over the
 * [C11 Atomic Operations Libary](https://en.cppreference.com/w/c/atomic) in
 * some advanced use cases:
 *
 * - The functions allow mixing of atomic and non-atomic accesses. E.g. while
 *   IRQs are disabled anyway, even plain accesses cannot be interrupted but
 *   are often more efficient.
 * - On platforms not supporting lock-free access, a library call is generated
 *   instead. The fallback implementation used here is more efficient in terms
 *   of both CPU instructions and ROM size.
 * - On platforms where some operations can be implemented lock free while
 *   others can't, at least LLVM will use the library call even for those
 *   accesses that can be implemented lock-free. This is because without
 *   assuming how the library call implements atomic access for the other
 *   functions, mixing library calls and lock free accesses could result in data
 *   corruption. But this implementation resorts to disabling IRQs when
 *   lock-free implementations are not possible, which mixes well with lock-free
 *   accesses. Thus, additional overhead for atomic accesses is only spent where
 *   needed.
 * - In some cases the fallback implementation performs better than the lock
 *   free implementation. E.g. if a specific platform has an atomic compare and
 *   swap instruction, this could be used to perform a read-modify-write in a
 *   loop until the value initially read was not changed in between. Just
 *   disabling IRQs to implement an atomic read-modify-write operation is likely
 *   more efficient. C11 atomics will however always use the lock free
 *   implementation (if such exists), assuming that this is more efficient.
 *   This assumption was made with desktop class hardware in mind, but is not
 *   generally true for bare metal targets. These function allow to optimize
 *   for the actual hardware RIOT is running on.
 * - This library provides "semi-atomic" read-modify-write operations, which are
 *   useful when at most one thread is ever writing to memory. In that case,
 *   only the write part of the read-modify-write operation needs to be
 *   performed in an atomic fashion in order for the reading threads to perceive
 *   atomic updates of the variable. This is significantly cheaper than atomic
 *   read-modify-write operations for many platforms
 *
 * # Guarantees
 *
 * - Every utility function here acts as a barrier for code reordering regarding
 * - For the `atomic_*()` family of functions: The whole operation will be done
 *   in an non-interruptible fashion
 * - For the `semi_atomic_*()` family of functions: The write part of the
 *   operation is done atomically. If at most one thread is ever performing
 *   changes to a variable using the `semi_atomic_()` functions, those changes
 *   will appear as if they were atomic to all other threads.
 *
 * # Porting to new CPUs
 *
 * At the bare minimum, create an empty `atomic_utils_arch.h` file. This will
 * result in the fallback implementations being used.
 *
 * To expose lock-free atomic operations, add an implementation to the
 * `atomic_utils_arch.h` file and disable the fallback implementation by
 * `#define`ing `HAS_<FN_NAME_ALL_CAPS>`, where `<FN_NAME_ALL_CAPS>` is the name
 * of the function provided in all upper case. E.g. most platforms will be able
 * to provide lock-free reads and writes up to their word size and can expose
 * this as follows:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 *  // All the user header boilerplate
 *  #define HAS_ATOMIC_LOAD_U8
 *  static inline uint8_t atomic_load_u8(const uint8_t *var)
 *  {
 *      return __atomic_load_1(var, __ATOMIC_SEQ_CST);
 *  }
 *
 *  #define HAS_ATOMIC_STORE_U8
 *  static inline void atomic_store_u8(uint8_t *dest, uint8_t val)
 *  {
 *      __atomic_store_1(dest, val, __ATOMIC_SEQ_CST);
 *  }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Note: The `semi_atomic_*()` family of functions is always provided using
 * `atomic_*()` functions in the cheapest way possible.
 *
 * @{
 *
 * @file
 * @brief       API of the utility functions for atomic accesses
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ATOMIC_UTILS_H
#define ATOMIC_UTILS_H

#include <stdint.h>

#include "irq.h"
#include "atomic_utils_arch.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Declarations and documentation: */

#if !defined(ATOMIC_BITMASK) || defined(DOXYGEN)
/**
 * @brief   Type qualifier to use for bitmasks accessed atomically
 *
 * Some platforms allow efficient access to a single bit with approaches like
 * bit banding. But for bit banding to work, the variable has to be allocated
 * within the bit banding region. This type qualifier ensures that a variable
 * can be used safely with the `atomic_set_bit_*()` and `atomic_clear_bit_*()`
 * family of functions.
 *
 * This macro is defined to an empty token on platforms not supporting bit
 * banding. Thus, it is safe to use this type qualifier for every platform and
 * it must be used for portable code.
 */
#define ATOMIC_BITMASK
#endif

/**
 * @name    Atomic Loads
 * @{
 */
/**
 * @brief   Load an `uint8_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint8_t atomic_load_u8(const uint8_t *var);
/**
 * @brief   Load an `uint16_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint16_t atomic_load_u16(const uint16_t *var);
/**
 * @brief   Load an `uint32_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint32_t atomic_load_u32(const uint32_t *var);
/**
 * @brief   Load an `uint64_t` atomically
 *
 * @param[in]       var     Variable to load atomically
 * @return  The value stored in @p var
 */
static inline uint64_t atomic_load_u64(const uint64_t *var);
/** @} */

/**
 * @name    Atomic Stores
 * @{
 */
/**
 * @brief  Store an `uint8_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u8(uint8_t *dest, uint8_t val);
/**
 * @brief  Store an `uint16_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u16(uint16_t *dest, uint16_t val);
/**
 * @brief  Store an `uint32_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u32(uint32_t *dest, uint32_t val);
/**
 * @brief  Store an `uint64_t` atomically
 * @param[out]      dest    Location to atomically write the new value to
 * @param[in]       val     Value to write
 */
static inline void atomic_store_u64(uint64_t *dest, uint64_t val);
/** @} */

/**
 * @name    Atomic In-Place Addition
 * @{
 */
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void atomic_fetch_add_u8(uint8_t *dest, uint8_t summand);
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void atomic_fetch_add_u16(uint16_t *dest, uint16_t summand);
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void atomic_fetch_add_u32(uint32_t *dest, uint32_t summand);
/**
 * @brief   Atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void atomic_fetch_add_u64(uint64_t *dest, uint64_t summand);
/** @} */

/**
 * @name    Atomic In-Place Subtraction
 * @{
 */
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void atomic_fetch_sub_u8(uint8_t *dest, uint8_t subtrahend);
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void atomic_fetch_sub_u16(uint16_t *dest, uint16_t subtrahend);
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void atomic_fetch_sub_u32(uint32_t *dest, uint32_t subtrahend);
/**
 * @brief   Atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void atomic_fetch_sub_u64(uint64_t *dest, uint64_t subtrahend);
/** @} */

/**
 * @name    Atomic In-Place Bitwise OR
 * @{
 */
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void atomic_fetch_or_u8(uint8_t *dest, uint8_t val);
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void atomic_fetch_or_u16(uint16_t *dest, uint16_t val);
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void atomic_fetch_or_u32(uint32_t *dest, uint32_t val);
/**
 * @brief   Atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void atomic_fetch_or_u64(uint64_t *dest, uint64_t val);
/** @} */

/**
 * @name    Atomic In-Place Bitwise XOR
 * @{
 */
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void atomic_fetch_xor_u8(uint8_t *dest, uint8_t val);
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void atomic_fetch_xor_u16(uint16_t *dest, uint16_t val);
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void atomic_fetch_xor_u32(uint32_t *dest, uint32_t val);
/**
 * @brief   Atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void atomic_fetch_xor_u64(uint64_t *dest, uint64_t val);
/** @} */

/**
 * @name    Atomic In-Place Bitwise AND
 * @{
 */
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void atomic_fetch_and_u8(uint8_t *dest, uint8_t val);
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void atomic_fetch_and_u16(uint16_t *dest, uint16_t val);
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void atomic_fetch_and_u32(uint32_t *dest, uint32_t val);
/**
 * @brief   Atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void atomic_fetch_and_u64(uint64_t *dest, uint64_t val);
/** @} */

/**
 * @name    Atomic Bit Setting
 * @{
 */
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   mask        Mask to set bit in
 * @param[in]       bit         bit to set
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_set_bit_u8(uint8_t *mask, uint8_t bit);
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   mask        Mask to set bit in
 * @param[in]       bit         bit to set
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_set_bit_u16(uint16_t *mask, uint8_t bit);
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   mask        Mask to set bit in
 * @param[in]       bit         bit to set
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_set_bit_u32(uint32_t *mask, uint8_t bit);
/**
 * @brief   Atomic version of `*dest |= (1 << bit)`
 * @param[in,out]   mask        Mask to set bit in
 * @param[in]       bit         bit to set
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_set_bit_u64(uint64_t *mask, uint8_t bit);
/** @} */

/**
 * @name    Atomic Bit Clearing
 * @{
 */
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   mask        Mask to clear bit in
 * @param[in]       bit         bit to clear
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_clear_bit_u8(uint8_t *mask, uint8_t bit);
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   mask        Mask to clear bit in
 * @param[in]       bit         bit to clear
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_clear_bit_u16(uint16_t *mask, uint8_t bit);
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   mask        Mask to clear bit in
 * @param[in]       bit         bit to clear
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_clear_bit_u32(uint32_t *mask, uint8_t bit);
/**
 * @brief   Atomic version of `*dest &= ~(1 << bit)`
 * @param[in,out]   mask        Mask to clear bit in
 * @param[in]       bit         bit to clear
 * @pre     The @ref ATOMIC_MASK type qualifier was applied to @p mask
 */
static inline void atomic_clear_bit_u64(uint64_t *mask, uint8_t bit);
/** @} */

/**
 * @name    Semi-Atomic In-Place Addition
 * @{
 */
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void semi_atomic_fetch_add_u8(uint8_t *dest, uint8_t summand);
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void semi_atomic_fetch_add_u16(uint16_t *dest, uint16_t summand);
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void semi_atomic_fetch_add_u32(uint32_t *dest, uint32_t summand);
/**
 * @brief   Semi-atomically add a value onto a given value
 * @param[in,out]   dest    Add @p summand onto this value atomically in-place
 * @param[in]       summand Value to add onto @p dest
 */
static inline void semi_atomic_fetch_add_u64(uint64_t *dest, uint64_t summand);
/** @} */

/**
 * @name    Semi-Atomic In-Place Subtraction
 * @{
 */
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void semi_atomic_fetch_sub_u8(uint8_t *dest, uint8_t subtrahend);
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void semi_atomic_fetch_sub_u16(uint16_t *dest, uint16_t subtrahend);
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void semi_atomic_fetch_sub_u32(uint32_t *dest, uint32_t subtrahend);
/**
 * @brief   Semi-atomically subtract a value from a given value
 * @param[in,out]   dest        Subtract @p subtrahend from this value atomically in-place
 * @param[in]       subtrahend  Value to subtract from @p dest
 */
static inline void semi_atomic_fetch_sub_u64(uint64_t *dest, uint64_t subtrahend);
/** @} */

/**
 * @name    Semi-atomic In-Place Bitwise OR
 * @{
 */
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void semi_atomic_fetch_or_u8(uint8_t *dest, uint8_t val);
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void semi_atomic_fetch_or_u16(uint16_t *dest, uint16_t val);
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void semi_atomic_fetch_or_u32(uint32_t *dest, uint32_t val);
/**
 * @brief   Semi-atomic version of `*dest |= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise or into @p dest in-place
 */
static inline void semi_atomic_fetch_or_u64(uint64_t *dest, uint64_t val);
/** @} */

/**
 * @name    Semi-Atomic In-Place Bitwise XOR
 * @{
 */
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void semi_atomic_fetch_xor_u8(uint8_t *dest, uint8_t val);
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void semi_atomic_fetch_xor_u16(uint16_t *dest, uint16_t val);
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void semi_atomic_fetch_xor_u32(uint32_t *dest, uint32_t val);
/**
 * @brief   Semi-atomic version of `*dest ^= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise xor into @p dest in-place
 */
static inline void semi_atomic_fetch_xor_u64(uint64_t *dest, uint64_t val);
/** @} */

/**
 * @name    Semi-Atomic In-Place Bitwise AND
 * @{
 */
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void semi_atomic_fetch_and_u8(uint8_t *dest, uint8_t val);
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void semi_atomic_fetch_and_u16(uint16_t *dest, uint16_t val);
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void semi_atomic_fetch_and_u32(uint32_t *dest, uint32_t val);
/**
 * @brief   Semi-atomic version of `*dest &= val`
 * @param[in,out]   dest        Replace this value with the result of `*dest | val`
 * @param[in]       val         Value to bitwise and into @p dest in-place
 */
static inline void semi_atomic_fetch_and_u64(uint64_t *dest, uint64_t val);
/** @} */

/* Fallback implementations of atomic utility functions: */

/**
 * @brief   Concatenate two tokens
 */
#define CONCAT(a, b) a ## b

/**
 * @brief   Concatenate four tokens
 */
#define CONCAT4(a, b, c, d) a ## b ## c ## d

#define ATOMIC_LOAD_IMPL(name, type) \
    static inline type CONCAT(atomic_load_, name)(const type *var) \
    { \
        unsigned state = irq_disable(); \
        type result = *var; \
        irq_restore(state); \
        return result; \
    }

#ifndef HAS_ATOMIC_LOAD_U8
ATOMIC_LOAD_IMPL(u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_LOAD_U16
ATOMIC_LOAD_IMPL(u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_LOAD_U32
ATOMIC_LOAD_IMPL(u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_LOAD_U64
ATOMIC_LOAD_IMPL(u64, uint64_t)
#endif

#define ATOMIC_STORE_IMPL(name, type) \
    static inline void CONCAT(atomic_store_, name)(type *dest, type val) \
    { \
        unsigned state = irq_disable(); \
        *dest = val; \
        irq_restore(state); \
    }

#ifndef HAS_ATOMIC_STORE_U8
ATOMIC_STORE_IMPL(u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_STORE_U16
ATOMIC_STORE_IMPL(u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_STORE_U32
ATOMIC_STORE_IMPL(u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_STORE_U64
ATOMIC_STORE_IMPL(u64, uint64_t)
#endif

#define ATOMIC_FETCH_OP_IMPL(opname, op, name, type) \
    static inline void CONCAT4(atomic_fetch_, opname, _, name)(type *dest, type val) \
    { \
        unsigned state = irq_disable(); \
        *dest = *dest op val; \
        irq_restore(state); \
    }

#ifndef HAS_ATOMIC_FETCH_ADD_U8
ATOMIC_FETCH_OP_IMPL(add, +, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_ADD_U16
ATOMIC_FETCH_OP_IMPL(add, +, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_ADD_U32
ATOMIC_FETCH_OP_IMPL(add, +, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_ADD_U64
ATOMIC_FETCH_OP_IMPL(add, +, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_SUB_U8
ATOMIC_FETCH_OP_IMPL(sub, -, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_SUB_U16
ATOMIC_FETCH_OP_IMPL(sub, -, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_SUB_U32
ATOMIC_FETCH_OP_IMPL(sub, -, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_SUB_U64
ATOMIC_FETCH_OP_IMPL(sub, -, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_OR_U8
ATOMIC_FETCH_OP_IMPL(or, |, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_OR_U16
ATOMIC_FETCH_OP_IMPL(or, |, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_OR_U32
ATOMIC_FETCH_OP_IMPL(or, |, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_OR_U64
ATOMIC_FETCH_OP_IMPL(or, |, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_XOR_U8
ATOMIC_FETCH_OP_IMPL(xor, ^, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_XOR_U16
ATOMIC_FETCH_OP_IMPL(xor, ^, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_XOR_U32
ATOMIC_FETCH_OP_IMPL(xor, ^, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_XOR_U64
ATOMIC_FETCH_OP_IMPL(xor, ^, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_FETCH_AND_U8
ATOMIC_FETCH_OP_IMPL(and, &, u8, uint8_t)
#endif
#ifndef HAS_ATOMIC_FETCH_AND_U16
ATOMIC_FETCH_OP_IMPL(and, &, u16, uint16_t)
#endif
#ifndef HAS_ATOMIC_FETCH_AND_U32
ATOMIC_FETCH_OP_IMPL(and, &, u32, uint32_t)
#endif
#ifndef HAS_ATOMIC_FETCH_AND_U64
ATOMIC_FETCH_OP_IMPL(and, &, u64, uint64_t)
#endif

#ifndef HAS_ATOMIC_SET_BIT_U8
static inline void atomic_set_bit_u8(uint8_t *mask, uint8_t bit)
{
    atomic_fetch_or_u8(mask, 1 << bit);
}
#endif
#ifndef HAS_ATOMIC_SET_BIT_U16
static inline void atomic_set_bit_u16(uint16_t *mask, uint8_t bit)
{
    atomic_fetch_or_u16(mask, 1 << bit);
}
#endif
#ifndef HAS_ATOMIC_SET_BIT_U32
static inline void atomic_set_bit_u32(uint32_t *mask, uint8_t bit)
{
    atomic_fetch_or_u32(mask, 1 << bit);
}
#endif
#ifndef HAS_ATOMIC_SET_BIT_U64
static inline void atomic_set_bit_u64(uint64_t *mask, uint8_t bit)
{
    atomic_fetch_or_u64(mask, 1 << bit);
}
#endif

#ifndef HAS_ATOMIC_CLEAR_BIT_U8
static inline void atomic_clear_bit_u8(uint8_t *mask, uint8_t bit)
{
    atomic_fetch_and_u8(mask, ~(1 << bit));
}
#endif
#ifndef HAS_ATOMIC_CLEAR_BIT_U16
static inline void atomic_clear_bit_u16(uint16_t *mask, uint8_t bit)
{
    atomic_fetch_and_u16(mask, ~(1 << bit));
}
#endif
#ifndef HAS_ATOMIC_CLEAR_BIT_U32
static inline void atomic_clear_bit_u32(uint32_t *mask, uint8_t bit)
{
    atomic_fetch_and_u32(mask, ~(1 << bit));
}
#endif
#ifndef HAS_ATOMIC_CLEAR_BIT_U64
static inline void atomic_clear_bit_u64(uint64_t *mask, uint8_t bit)
{
    atomic_fetch_and_u64(mask, ~(1 << bit));
}
#endif

/* Provide semi_atomic_*() functions on top.
 *
 * - If atomic_<FOO>() is provided: Use this for semi_atomic_<FOO>() as well
 * - Else:
 *      - If matching `atomic_store_u<BITS>()` is provided: Only make final
 *        store atomic, as we can avoid touching the IRQ state register that
 *        way
 *      - Else: We need to disable and re-enable IRQs anyway, we just use the
 *        fallback implementation of `atomic_<FOO>()` for `semi_atomic<FOO>()`
 *        as well
 */

/* FETCH_ADD */
#if defined(HAS_ATOMIC_FETCH_ADD_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline void semi_atomic_fetch_add_u8(uint8_t *dest, uint8_t val) {
    atomic_fetch_add_u8(dest, val);
}
#else
static inline void semi_atomic_fetch_add_u8(uint8_t *dest, uint8_t val) {
    atomic_store_u8(dest, *dest + val);
}
#endif /* HAS_ATOMIC_FETCH_ADD_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_ADD_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline void semi_atomic_fetch_add_u16(uint16_t *dest, uint16_t val) {
    atomic_fetch_add_u16(dest, val);
}
#else
static inline void semi_atomic_fetch_add_u16(uint16_t *dest, uint16_t val) {
    atomic_store_u16(dest, *dest + val);
}
#endif /* HAS_ATOMIC_FETCH_ADD_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_ADD_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline void semi_atomic_fetch_add_u32(uint32_t *dest, uint32_t val) {
    atomic_fetch_add_u32(dest, val);
}
#else
static inline void semi_atomic_fetch_add_u32(uint32_t *dest, uint32_t val) {
    atomic_store_u32(dest, *dest + val);
}
#endif /* HAS_ATOMIC_FETCH_ADD_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_ADD_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline void semi_atomic_fetch_add_u64(uint64_t *dest, uint64_t val) {
    atomic_fetch_add_u64(dest, val);
}
#else
static inline void semi_atomic_fetch_add_u64(uint64_t *dest, uint64_t val) {
    atomic_store_u64(dest, *dest + val);
}
#endif /* HAS_ATOMIC_FETCH_ADD_U32 || !HAS_ATOMIC_STORE_U32 */

/* FETCH_SUB */
#if defined(HAS_ATOMIC_FETCH_SUB_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline void semi_atomic_fetch_sub_u8(uint8_t *dest, uint8_t val) {
    atomic_fetch_sub_u8(dest, val);
}
#else
static inline void semi_atomic_fetch_sub_u8(uint8_t *dest, uint8_t val) {
    atomic_store_u8(dest, *dest - val);
}
#endif /* HAS_ATOMIC_FETCH_SUB_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_SUB_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline void semi_atomic_fetch_sub_u16(uint16_t *dest, uint16_t val) {
    atomic_fetch_sub_u16(dest, val);
}
#else
static inline void semi_atomic_fetch_sub_u16(uint16_t *dest, uint16_t val) {
    atomic_store_u16(dest, *dest - val);
}
#endif /* HAS_ATOMIC_FETCH_SUB_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_SUB_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline void semi_atomic_fetch_sub_u32(uint32_t *dest, uint32_t val) {
    atomic_fetch_sub_u32(dest, val);
}
#else
static inline void semi_atomic_fetch_sub_u32(uint32_t *dest, uint32_t val) {
    atomic_store_u32(dest, *dest - val);
}
#endif /* HAS_ATOMIC_FETCH_SUB_U32 || !HAS_ATOMIC_STORE_U64 */

#if defined(HAS_ATOMIC_FETCH_SUB_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline void semi_atomic_fetch_sub_u64(uint64_t *dest, uint64_t val) {
    atomic_fetch_sub_u64(dest, val);
}
#else
static inline void semi_atomic_fetch_sub_u64(uint64_t *dest, uint64_t val) {
    atomic_store_u64(dest, *dest - val);
}
#endif /* HAS_ATOMIC_FETCH_SUB_U64 || !HAS_ATOMIC_STORE_U64 */

/* FETCH_OR */
#if defined(HAS_ATOMIC_FETCH_OR_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline void semi_atomic_fetch_or_u8(uint8_t *dest, uint8_t val) {
    atomic_fetch_or_u8(dest, val);
}
#else
static inline void semi_atomic_fetch_or_u8(uint8_t *dest, uint8_t val) {
    atomic_store_u8(dest, *dest | val);
}
#endif /* HAS_ATOMIC_FETCH_OR_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_OR_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline void semi_atomic_fetch_or_u16(uint16_t *dest, uint16_t val) {
    atomic_fetch_or_u16(dest, val);
}
#else
static inline void semi_atomic_fetch_or_u16(uint16_t *dest, uint16_t val) {
    atomic_store_u16(dest, *dest | val);
}
#endif /* HAS_ATOMIC_FETCH_OR_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_OR_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline void semi_atomic_fetch_or_u32(uint32_t *dest, uint32_t val) {
    atomic_fetch_or_u32(dest, val);
}
#else
static inline void semi_atomic_fetch_or_u32(uint32_t *dest, uint32_t val) {
    atomic_store_u32(dest, *dest | val);
}
#endif /* HAS_ATOMIC_FETCH_OR_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_OR_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline void semi_atomic_fetch_or_u64(uint64_t *dest, uint64_t val) {
    atomic_fetch_or_u64(dest, val);
}
#else
static inline void semi_atomic_fetch_or_u64(uint64_t *dest, uint64_t val) {
    atomic_store_u64(dest, *dest | val);
}
#endif /* HAS_ATOMIC_FETCH_OR_U64 || !HAS_ATOMIC_STORE_U64 */

/* FETCH_XOR */
#if defined(HAS_ATOMIC_FETCH_XOR_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline void semi_atomic_fetch_xor_u8(uint8_t *dest, uint8_t val) {
    atomic_fetch_xor_u8(dest, val);
}
#else
static inline void semi_atomic_fetch_xor_u8(uint8_t *dest, uint8_t val) {
    atomic_store_u8(dest, *dest ^ val);
}
#endif /* HAS_ATOMIC_FETCH_XOR_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_XOR_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline void semi_atomic_fetch_xor_u16(uint16_t *dest, uint16_t val) {
    atomic_fetch_xor_u16(dest, val);
}
#else
static inline void semi_atomic_fetch_xor_u16(uint16_t *dest, uint16_t val) {
    atomic_store_u16(dest, *dest ^ val);
}
#endif /* HAS_ATOMIC_FETCH_XOR_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_XOR_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline void semi_atomic_fetch_xor_u32(uint32_t *dest, uint32_t val) {
    atomic_fetch_xor_u32(dest, val);
}
#else
static inline void semi_atomic_fetch_xor_u32(uint32_t *dest, uint32_t val) {
    atomic_store_u32(dest, *dest ^ val);
}
#endif /* HAS_ATOMIC_FETCH_XOR_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_XOR_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline void semi_atomic_fetch_xor_u64(uint64_t *dest, uint64_t val) {
    atomic_fetch_xor_u64(dest, val);
}
#else
static inline void semi_atomic_fetch_xor_u64(uint64_t *dest, uint64_t val) {
    atomic_store_u64(dest, *dest ^ val);
}
#endif /* HAS_ATOMIC_FETCH_XOR_U64 || !HAS_ATOMIC_STORE_U64 */

/* FETCH_AND */
#if defined(HAS_ATOMIC_FETCH_AND_U8) || !defined(HAS_ATOMIC_STORE_U8)
static inline void semi_atomic_fetch_and_u8(uint8_t *dest, uint8_t val) {
    atomic_fetch_and_u8(dest, val);
}
#else
static inline void semi_atomic_fetch_and_u8(uint8_t *dest, uint8_t val) {
    atomic_store_u8(dest, *dest & val);
}
#endif /* HAS_ATOMIC_FETCH_AND_U8 || !HAS_ATOMIC_STORE_U8 */

#if defined(HAS_ATOMIC_FETCH_AND_U16) || !defined(HAS_ATOMIC_STORE_U16)
static inline void semi_atomic_fetch_and_u16(uint16_t *dest, uint16_t val) {
    atomic_fetch_and_u16(dest, val);
}
#else
static inline void semi_atomic_fetch_and_u16(uint16_t *dest, uint16_t val) {
    atomic_store_u16(dest, *dest & val);
}
#endif /* HAS_ATOMIC_FETCH_AND_U16 || !HAS_ATOMIC_STORE_U16 */

#if defined(HAS_ATOMIC_FETCH_AND_U32) || !defined(HAS_ATOMIC_STORE_U32)
static inline void semi_atomic_fetch_and_u32(uint32_t *dest, uint32_t val) {
    atomic_fetch_and_u32(dest, val);
}
#else
static inline void semi_atomic_fetch_and_u32(uint32_t *dest, uint32_t val) {
    atomic_store_u32(dest, *dest & val);
}
#endif /* HAS_ATOMIC_FETCH_AND_U32 || !HAS_ATOMIC_STORE_U32 */

#if defined(HAS_ATOMIC_FETCH_AND_U64) || !defined(HAS_ATOMIC_STORE_U64)
static inline void semi_atomic_fetch_and_u64(uint64_t *dest, uint64_t val) {
    atomic_fetch_and_u64(dest, val);
}
#else
static inline void semi_atomic_fetch_and_u64(uint64_t *dest, uint64_t val) {
    atomic_store_u64(dest, *dest & val);
}
#endif /* HAS_ATOMIC_FETCH_AND_U64 || !HAS_ATOMIC_STORE_U64 */

#ifdef __cplusplus
}
#endif

#endif /* ATOMIC_UTILS_H */
/** @} */
