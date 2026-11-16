# Simulador RISC-V

Simulador del ISA base RISC-V de 32 bits (RV32I) implementado en C++.
Basado en la estructura de [toy-simulator](https://github.com/Grazen0/toy-simulator).

## Entorno de simulación

- 32 registros de 32 bits (`x0` siempre vale 0) y un PC.
- Memoria plana byte-addressable de 16 MB, little-endian.
- El programa (binario crudo) se carga a partir del address 0.
- `sp` (`x2`) empieza al final de la memoria.
- El programa termina al encontrar un salto a sí mismo (`j .`) o una instrucción inválida.

## Uso

```bash
cmake -S . -B build
cmake --build build

build/riscv-simulator programas/quicksort.bin
```

O directamente: `g++ -std=c++14 -o rvsim src/main.cpp`

## Comandos

| Comando       | Descripción                                   |
| ------------- | --------------------------------------------- |
| `step`        | Ejecuta una instrucción.                      |
| `run`         | Ejecuta hasta que termine el programa.        |
| `pc`          | Muestra el PC.                                |
| `regs [n...]` | Muestra todos los registros o solo `xn`.      |
| `mem ini fin` | Muestra los bytes de memoria entre ini y fin. |
| `exit`        | Sale del simulador.                           |

## Programas de prueba (`programas/`)

| Programa    | Resultado esperado                         |
| ----------- | ------------------------------------------ |
| `riscvtest` | 25 (0x19) en la dirección 100 (0x64)       |
| `quicksort` | {1, 2, 3, 4, 6, 8, 9} en 0x1000            |
| `simetrico` | `a0` (`x10`) = 1                           |
| `cargas`    | `F1 F0 81 80` en 0x2000; lb/lbu/lh/lhu     |
