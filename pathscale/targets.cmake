# message() string listing allowed target architectures.
set(_PATH64_SUPPORTED_TARGETS_STRING "Supported architectures are:
  x8664_32
  x8664_64
  mips_32
  mips_64
  rsk6_32
  rsk6_64")

# Target information table, keyed by entries of PATH64_ENABLE_TARGETS.
# Reference table entries with ${_PATH64_TARGET_ARCH_${arch}}.

set(_PATH64_TARGET_ARCH_x8664_32 x8664)
set(_PATH64_TARGET_FLAG_x8664_32 -m32)

set(_PATH64_TARGET_ARCH_x8664_64 x8664)
set(_PATH64_TARGET_FLAG_x8664_64 -m64)

set(_PATH64_TARGET_ARCH_mips_32 mips)
set(_PATH64_TARGET_FLAG_mips_32 -n32)

set(_PATH64_TARGET_ARCH_mips_64 mips)
set(_PATH64_TARGET_FLAG_mips_64 -n64)

set(_PATH64_TARGET_ARCH_rsk6_32 rsk6)
set(_PATH64_TARGET_FLAG_rsk6_32 -q32)

set(_PATH64_TARGET_ARCH_rsk6_64 rsk6)
set(_PATH64_TARGET_FLAG_rsk6_64 -q64)
