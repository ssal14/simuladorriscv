# Prueba de loads/stores de byte y halfword, shifts y comparaciones sin signo
.global _start
_start:
        li   s0, 0x2000
        li   t0, 0x8081F0F1      # lui + addi
        sw   t0, 0(s0)           # memoria (little-endian): F1 F0 81 80
        lb   a0, 0(s0)           # a0 = 0xFFFFFFF1 (con signo)
        lbu  a1, 0(s0)           # a1 = 0x000000F1
        lh   a2, 2(s0)           # a2 = 0xFFFF8081
        lhu  a3, 2(s0)           # a3 = 0x00008081
        sb   a1, 4(s0)           # [0x2004] = F1
        sh   a3, 6(s0)           # [0x2006] = 81 80
        srai a4, t0, 4           # a4 = 0xF8081F0F
        srli a5, t0, 4           # a5 = 0x08081F0F
        li   t1, -1
        sltu a6, zero, t1        # a6 = 1  (0 < 0xFFFFFFFF sin signo)
        slt  a7, zero, t1        # a7 = 0  (0 < -1 con signo es falso)
        auipc s1, 0              # s1 = pc de esta instruccion
fin:    j    fin
