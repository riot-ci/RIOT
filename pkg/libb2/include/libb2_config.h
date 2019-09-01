#ifndef LIBB2_CONFIG_H
#define LIBB2_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Add no suffix to the functions */
#define SUFFIX

/* Test for a little-endian machine */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define NATIVE_LITTLE_ENDIAN
#endif

/* CPUs that don't support unaligned access, taken from GStreamer */
#if defined(__alpha__) || defined(__arc__) || defined(__arm__) || defined(__aarch64__) || defined(__bfin) || defined(__hppa__) || defined(__nios2__) || defined(__MICROBLAZE__) || defined(__mips__) || defined(__or1k__) || defined(__sh__) || defined(__SH4__) || defined(__sparc__) || defined(__sparc) || defined(__ia64__) || defined(_M_ALPHA) || defined(_M_ARM) || defined(_M_ARM64) || defined(_M_IA64) || defined(__xtensa__) || defined(__e2k__) || defined(__riscv)
#define HAVE_ALIGNED_ACCESS_REQUIRED
#endif

#ifdef __cplusplus
}
#endif
#endif /* LIBB2_CONFIG_H */
