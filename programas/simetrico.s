# Arbol completo aplanado {6,4,4,1,2,2,1} en 0x1000. a0 = 1 si es simetrico
.global _start
_start:
        li   s0, 0x1000
        li   s1, 7               # s1 = numero de nodos
        li   t0, 6
        sw   t0, 0(s0)
        li   t0, 4
        sw   t0, 4(s0)
        sw   t0, 8(s0)
        li   t0, 1
        sw   t0, 12(s0)
        sw   t0, 24(s0)
        li   t0, 2
        sw   t0, 16(s0)
        sw   t0, 20(s0)

        jal  ra, is_symmetric
fin:    j    fin

is_symmetric:
        addi sp, sp, -4
        sw   ra, 0(sp)
        li   a0, 1               # espejo(1, 2): hijo izq vs hijo der de la raiz
        li   a1, 2
        jal  ra, espejo
        lw   ra, 0(sp)
        addi sp, sp, 4
        ret

# espejo(a0 = i, a1 = j): 1 si el subarbol i es espejo del subarbol j
espejo:
        bge  a0, s1, esp_si      # nodos vacios -> son espejo
        slli t0, a0, 2
        add  t0, s0, t0
        lw   t0, 0(t0)
        slli t1, a1, 2
        add  t1, s0, t1
        lw   t1, 0(t1)
        bne  t0, t1, esp_no      # valores distintos -> no
        addi sp, sp, -12
        sw   ra, 8(sp)
        sw   a0, 4(sp)
        sw   a1, 0(sp)
        slli a0, a0, 1           # espejo(2i+1, 2j+2)
        addi a0, a0, 1
        slli a1, a1, 1
        addi a1, a1, 2
        jal  ra, espejo
        beq  a0, zero, esp_pop   # si ya fallo, retornar 0
        lw   a0, 4(sp)           # espejo(2i+2, 2j+1)
        lw   a1, 0(sp)
        slli a0, a0, 1
        addi a0, a0, 2
        slli a1, a1, 1
        addi a1, a1, 1
        jal  ra, espejo
esp_pop:
        lw   ra, 8(sp)
        addi sp, sp, 12
        ret
esp_si: li   a0, 1
        ret
esp_no: li   a0, 0
        ret
