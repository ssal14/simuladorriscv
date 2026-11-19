#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

constexpr uint32_t MEMORY_SIZE = 0x1000000;

class Simulator {
private:
    bool halted = false;
    uint32_t pc = 0;
    uint32_t regs[32] = {};
    std::vector<uint8_t> memory;

    uint8_t load8(uint32_t addr) {
        if (addr >= MEMORY_SIZE) {
            printf("Error: direccion fuera de memoria 0x%08X\n", addr);
            halted = true;
            return 0;
        }
        return memory[addr];
    }
    uint16_t load16(uint32_t addr) {
        return load8(addr) | (load8(addr + 1) << 8);
    }
    uint32_t load32(uint32_t addr) {
        return load16(addr) | (load16(addr + 2) << 16);
    }

    void store8(uint32_t addr, uint8_t value) {
        if (addr >= MEMORY_SIZE) {
            printf("Error: direccion fuera de memoria 0x%08X\n", addr);
            halted = true;
            return;
        }
        memory[addr] = value;
    }
    void store16(uint32_t addr, uint16_t value) {
        store8(addr, value & 0xFF);
        store8(addr + 1, value >> 8);
    }
    void store32(uint32_t addr, uint32_t value) {
        store16(addr, value & 0xFFFF);
        store16(addr + 2, value >> 16);
    }

public:
    Simulator()
        : memory(MEMORY_SIZE) {
        regs[2] = MEMORY_SIZE;
    }

    bool load_from_file(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
            return false;

        file.read((char*)memory.data(), MEMORY_SIZE);
        return true;
    }

    bool is_halted() const { return halted; }
    uint32_t get_pc() const { return pc; }
    uint32_t get_reg(int i) const { return regs[i]; }
    uint8_t get_mem(uint32_t addr) const { return addr < MEMORY_SIZE ? memory[addr] : 0; }

    void step() {
        uint32_t instr = load32(pc);

        uint32_t opcode = instr & 0x7F;         // Bits 6:0
        uint32_t rd = (instr >> 7) & 0x1F;      // Bits 11:7
        uint32_t funct3 = (instr >> 12) & 0x7;  // Bits 14:12
        uint32_t rs1 = (instr >> 15) & 0x1F;    // Bits 19:15
        uint32_t rs2 = (instr >> 20) & 0x1F;    // Bits 24:20
        uint32_t funct7 = instr >> 25;          // Bits 31:25

        int32_t imm_i = (int32_t)instr >> 20;
        int32_t imm_s = (((int32_t)instr >> 25) << 5) | ((instr >> 7) & 0x1F);
        int32_t imm_b = (((int32_t)instr >> 31) << 12) | (((instr >> 7) & 0x1) << 11) |
                        (((instr >> 25) & 0x3F) << 5) | (((instr >> 8) & 0xF) << 1);
        int32_t imm_u = instr & 0xFFFFF000;
        int32_t imm_j = (((int32_t)instr >> 31) << 20) | (instr & 0xFF000) |
                        (((instr >> 20) & 0x1) << 11) | (((instr >> 21) & 0x3FF) << 1);

        uint32_t a = regs[rs1];
        uint32_t b = regs[rs2];
        uint32_t result = 0;
        bool write_rd = true;  // casi todas las instrucciones escriben en rd
        bool invalid = false;

        uint32_t old_pc = pc;
        pc += 4;

        switch (opcode) {
            case 0x03:  // Loads
                switch (funct3) {
                    case 0: result = (int8_t)load8(a + imm_i); break;    // lb
                    case 1: result = (int16_t)load16(a + imm_i); break;  // lh
                    case 2: result = load32(a + imm_i); break;           // lw
                    case 4: result = load8(a + imm_i); break;            // lbu
                    case 5: result = load16(a + imm_i); break;           // lhu
                    default: invalid = true;
                }
                break;

            case 0x13:  // Operaciones con inmediato
                switch (funct3) {
                    case 0: result = a + imm_i; break;                     // addi
                    case 1: result = a << rs2; break;                      // slli (shamt = bits 24:20)
                    case 2: result = (int32_t)a < imm_i; break;            // slti
                    case 3: result = a < (uint32_t)imm_i; break;           // sltiu
                    case 4: result = a ^ imm_i; break;                     // xori
                    case 5:
                        if (funct7 == 0x20) result = (int32_t)a >> rs2;    // srai
                        else result = a >> rs2;                            // srli
                        break;
                    case 6: result = a | imm_i; break;                     // ori
                    case 7: result = a & imm_i; break;                     // andi
                }
                break;

            case 0x17:  // auipc
                result = old_pc + imm_u;
                break;

            case 0x37:  // lui
                result = imm_u;
                break;

            case 0x23:  // Stores
                write_rd = false;
                switch (funct3) {
                    case 0: store8(a + imm_s, b); break;   // sb
                    case 1: store16(a + imm_s, b); break;  // sh
                    case 2: store32(a + imm_s, b); break;  // sw
                    default: invalid = true;
                }
                break;

            case 0x33:  // Operaciones entre registros
                switch (funct3) {
                    case 0:
                        if (funct7 == 0x20) result = a - b;                // sub
                        else result = a + b;                               // add
                        break;
                    case 1: result = a << (b & 0x1F); break;               // sll
                    case 2: result = (int32_t)a < (int32_t)b; break;       // slt
                    case 3: result = a < b; break;                         // sltu
                    case 4: result = a ^ b; break;                         // xor
                    case 5:
                        if (funct7 == 0x20) result = (int32_t)a >> (b & 0x1F);  // sra
                        else result = a >> (b & 0x1F);                          // srl
                        break;
                    case 6: result = a | b; break;                         // or
                    case 7: result = a & b; break;                         // and
                }
                break;

            case 0x63: {  // Branches
                write_rd = false;
                bool taken = false;
                switch (funct3) {
                    case 0: taken = (a == b); break;                    // beq
                    case 1: taken = (a != b); break;                    // bne
                    case 4: taken = ((int32_t)a < (int32_t)b); break;   // blt
                    case 5: taken = ((int32_t)a >= (int32_t)b); break;  // bge
                    case 6: taken = (a < b); break;                     // bltu
                    case 7: taken = (a >= b); break;                    // bgeu
                    default: invalid = true;
                }
                if (taken)
                    pc = old_pc + imm_b;
                break;
            }

            case 0x6F:  // jal
                result = old_pc + 4;
                pc = old_pc + imm_j;
                break;

            case 0x67:  // jalr
                result = old_pc + 4;
                pc = (a + imm_i) & ~1u;
                break;

            default:
                invalid = true;
        }

        if (invalid) {
            printf("Error: instruccion invalida 0x%08X en pc = 0x%08X\n", instr, old_pc);
            halted = true;
            pc = old_pc;
            return;
        }

        if (write_rd && rd != 0)  // x0 siempre vale 0
            regs[rd] = result;

        // Un salto a si mismo
        if (pc == old_pc) {
            printf("Fin del programa (bucle infinito en 0x%08X).\n", pc);
            halted = true;
        }
    }
};

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Debes especificar un archivo de entrada.\n";
        return 1;
    }

    Simulator sim{};

    if (!sim.load_from_file(argv[1])) {
        std::cerr << "No se pudo cargar el archivo.\n";
        return 1;
    }
    std::cout << "\"" << argv[1] << "\" cargado a memoria.\n";
    std::cout << "Comandos: step, run, pc, regs [n...], mem ini fin, exit\n";

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line))
            break;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "step") {
            if (sim.is_halted()) std::cout << "El programa ya termino.\n";
            else {
                uint32_t pc = sim.get_pc();
                uint32_t instr = sim.get_mem(pc) | (sim.get_mem(pc + 1) << 8) |
                                 (sim.get_mem(pc + 2) << 16) | (sim.get_mem(pc + 3) << 24);
                printf("Ejecutando 0x%08X en pc = 0x%08X\n", instr, pc);
                sim.step();
            }

        } else if (cmd == "run") {
            while (!sim.is_halted())
                sim.step();

        } else if (cmd == "pc") {
            printf("pc = 0x%08X\n", sim.get_pc());

        } else if (cmd == "regs") {
            int n;
            bool any = false;
            while (ss >> n && n >= 0 && n < 32) {
                printf("x%d = 0x%08X (%d)\n", n, sim.get_reg(n), (int32_t)sim.get_reg(n));
                any = true;
            }
            if (!any)
                for (int i = 0; i < 32; i++)
                    printf("x%d = 0x%08X (%d)\n", i, sim.get_reg(i), (int32_t)sim.get_reg(i));

        } else if (cmd == "mem") {
            std::string s1, s2;
            ss >> s1 >> s2;
            uint32_t start = std::stoul(s1, nullptr, 0);
            uint32_t end = std::stoul(s2, nullptr, 0);
            printf("Memoria (0x%X-0x%X):", start, end);
            for (uint32_t addr = start; addr <= end; addr++)
                printf(" %02X", sim.get_mem(addr));
            printf("\n");

        } else if (cmd == "exit") {
            break;

        } else if (!cmd.empty()) {
            std::cout << "Comando desconocido.\n";
        }
    }

    std::cout << "See you next time...\n";
    return 0;
}
