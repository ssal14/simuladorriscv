# Quicksort recursivo in-place del arreglo {6,4,3,2,1,8,9} en 0x1000
.global _start
_start:
        li   s0, 0x1000          # s0 = direccion del arreglo
        li   t0, 6
        sw   t0, 0(s0)
        li   t0, 4
        sw   t0, 4(s0)
        li   t0, 3
        sw   t0, 8(s0)
        li   t0, 2
        sw   t0, 12(s0)
        li   t0, 1
        sw   t0, 16(s0)
        li   t0, 8
        sw   t0, 20(s0)
        li   t0, 9
        sw   t0, 24(s0)

        mv   a0, s0              # quicksort(arr, 0, 6)
        li   a1, 0
        li   a2, 6
        jal  ra, quicksort
fin:    j    fin

# quicksort(a0 = arr, a1 = lo, a2 = hi)
quicksort:
        bge  a1, a2, qs_ret      # si lo >= hi, retornar
        addi sp, sp, -16
        sw   ra, 12(sp)
        sw   a1, 8(sp)
        sw   a2, 4(sp)
        jal  ra, partition       # a0 <- arr (no cambia), a3 <- p
        sw   a3, 0(sp)
        lw   a1, 8(sp)           # quicksort(arr, lo, p-1)
        addi a2, a3, -1
        jal  ra, quicksort
        lw   a3, 0(sp)           # quicksort(arr, p+1, hi)
        addi a1, a3, 1
        lw   a2, 4(sp)
        jal  ra, quicksort
        lw   ra, 12(sp)
        addi sp, sp, 16
qs_ret: ret

# partition (Lomuto): pivote = arr[hi], devuelve indice final en a3
partition:
        slli t0, a2, 2
        add  t0, a0, t0
        lw   t1, 0(t0)           # t1 = pivote
        addi t2, a1, -1          # i = lo - 1
        mv   t3, a1              # j = lo
p_loop: bge  t3, a2, p_end
        slli t4, t3, 2
        add  t4, a0, t4
        lw   t5, 0(t4)           # t5 = arr[j]
        blt  t1, t5, p_next      # si arr[j] > pivote, no intercambia
        addi t2, t2, 1           # i++
        slli t6, t2, 2
        add  t6, a0, t6
        lw   a4, 0(t6)           # swap(arr[i], arr[j])
        sw   t5, 0(t6)
        sw   a4, 0(t4)
p_next: addi t3, t3, 1
        j    p_loop
p_end:  addi t2, t2, 1           # swap(arr[i+1], arr[hi])
        slli t6, t2, 2
        add  t6, a0, t6
        lw   a4, 0(t6)
        sw   t1, 0(t6)
        sw   a4, 0(t0)
        mv   a3, t2
        ret
