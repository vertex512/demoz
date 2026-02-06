Linux UApi Header

Cflags 'include' is replaced according to the actual path

X86_32
  $ -U__i386__ -D__i386__ -Iinclude -Iinclude/asm-x86

X86_64
  $ -U__i386__ -U__ILP32__ -Iinclude -Iinclude/asm-x86

ARM_32
  $ -U__ARM_EABI__ -D__ARM_EABI__ -Iinclude -Iinclude/asm-arm

ARM_64
  $ -Iinclude -Iinclude/asm-arm64

RISC-V_32
  $ -U__LP64__ -Iinclude -Iinclude/asm-riscv

RISC-V_64
  $ -U__LP64__ -D__LP64__ -Iinclude -Iinclude/asm-riscv

MIPS_32:
  $ -U_MIPS_SIM -D_MIPS_SIM=2 -Iinclude -Iinclude/asm-mips

MIPS_64:
  $ -U_MIPS_SIM -D_MIPS_SIM=3 -Iinclude -Iinclude/asm-mips

IA64
  $ -Iinclude -Iinclude/asm-ia64

PPC_64
  $ -U__powerpc64__ -D__powerpc64__ -Iinclude -Iinclude/asm-powerpc

S390_64
  $ -U__s390x__ -D__s390x__ -Iinclude -Iinclude/asm-s390

LoongArch_64
  $ -Iinclude -Iinclude/asm-loongarch
