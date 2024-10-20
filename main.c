#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

void imprimir_cabecalho_elf(Elf64_Ehdr *cabecalho) {
    printf("Cabeçalho ELF:\n");
    printf("  Mágica:  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
           cabecalho->e_ident[0], cabecalho->e_ident[1], cabecalho->e_ident[2], cabecalho->e_ident[3],
           cabecalho->e_ident[4], cabecalho->e_ident[5], cabecalho->e_ident[6], cabecalho->e_ident[7],
           cabecalho->e_ident[8], cabecalho->e_ident[9], cabecalho->e_ident[10], cabecalho->e_ident[11],
           cabecalho->e_ident[12], cabecalho->e_ident[13], cabecalho->e_ident[14], cabecalho->e_ident[15]);
    printf("  Tipo: %u\n", cabecalho->e_type);
    printf("  Máquina: %u\n", cabecalho->e_machine);
    printf("  Versão: %u\n", cabecalho->e_version);
    printf("  Endereço do ponto de entrada: 0x%lx\n", cabecalho->e_entry);
    printf("  Início dos cabeçalhos de programa: %lu\n", cabecalho->e_phoff);
    printf("  Início dos cabeçalhos de seção: %lu\n", cabecalho->e_shoff);
    printf("  Flags: %u\n", cabecalho->e_flags);
    printf("  Tamanho deste cabeçalho: %u\n", cabecalho->e_ehsize);
    printf("  Tamanho dos cabeçalhos de programa: %u\n", cabecalho->e_phentsize);
    printf("  Número de cabeçalhos de programa: %u\n", cabecalho->e_phnum);
    printf("  Tamanho dos cabeçalhos de seção: %u\n", cabecalho->e_shentsize);
    printf("  Número de cabeçalhos de seção: %u\n", cabecalho->e_shnum);
    printf("  Índice da tabela de strings do cabeçalho da seção: %u\n", cabecalho->e_shstrndx);
}

const char* obter_tipo_secao(uint32_t tipo) {
    switch (tipo) {
        case SHT_NULL: return "NULO";
        case SHT_PROGBITS: return "Dados do Programa";
        case SHT_SYMTAB: return "Tabela de Símbolos";
        case SHT_STRTAB: return "Tabela de Strings";
        case SHT_RELA: return "Relocação com Adicionais";
        case SHT_HASH: return "Tabela Hash de Símbolos";
        case SHT_DYNAMIC: return "Informações de Ligação Dinâmica";
        case SHT_NOTE: return "Notas";
        case SHT_NOBITS: return "Sem Bits";
        case SHT_REL: return "Entradas de Relocação";
        case SHT_SHLIB: return "Reservado";
        case SHT_DYNSYM: return "Tabela de Símbolos Dinâmicos";
        case SHT_LOOS: return "Específico do OS";
        case SHT_HIOS: return "Específico do OS";
        case SHT_LOPROC: return "Específico do Processador";
        case SHT_HIPROC: return "Específico do Processador";
        case SHT_GNU_verneed: return "Versão Necessária";
        case SHT_GNU_verdef: return "Definições de Versão";
        case SHT_GNU_HASH: return "Tabela Hash de Símbolos GNU";
        default: return "Desconhecido";
    }
}

void imprimir_cabecalhos_secao(FILE *arquivo, Elf64_Ehdr *cabecalho) {
    fseek(arquivo, cabecalho->e_shoff, SEEK_SET);
    Elf64_Shdr *secao = malloc(cabecalho->e_shentsize * cabecalho->e_shnum);
    if (!secao) {
        perror("Falha ao alocar memória para seções");
        return;
    }
    fread(secao, cabecalho->e_shentsize, cabecalho->e_shnum, arquivo);

    printf("\nCabeçalhos de Seção:\n");
    char *shstrtab = malloc(secao[cabecalho->e_shstrndx].sh_size);
    if (!shstrtab) {
        perror("Falha ao alocar memória para a tabela de strings do cabeçalho da seção");
        free(secao);
        return;
    }
    fseek(arquivo, secao[cabecalho->e_shstrndx].sh_offset, SEEK_SET);
    fread(shstrtab, secao[cabecalho->e_shstrndx].sh_size, 1, arquivo);

    for (int i = 0; i < cabecalho->e_shnum; i++) {
        printf("  [%2d] Nome: %s Tipo: %s Offset: %lu Tamanho: %lu\n", 
               i, &shstrtab[secao[i].sh_name], obter_tipo_secao(secao[i].sh_type), 
               secao[i].sh_offset, secao[i].sh_size);
    }

    free(shstrtab);
    free(secao);
}

void imprimir_simbolos(FILE *arquivo, Elf64_Ehdr *cabecalho) {
    fseek(arquivo, cabecalho->e_shoff, SEEK_SET);
    Elf64_Shdr *secao = malloc(cabecalho->e_shentsize * cabecalho->e_shnum);
    if (!secao) {
        perror("Falha ao alocar memória para seções");
        return;
    }
    fread(secao, cabecalho->e_shentsize, cabecalho->e_shnum, arquivo);

    for (int i = 0; i < cabecalho->e_shnum; i++) {
        if (secao[i].sh_type == SHT_SYMTAB) {
            printf("\nTabela de Símbolos:\n");
            Elf64_Sym *simbolos = malloc(secao[i].sh_size);
            if (!simbolos) {
                perror("Falha ao alocar memória para símbolos");
                free(secao);
                return;
            }
            fseek(arquivo, secao[i].sh_offset, SEEK_SET);
            fread(simbolos, secao[i].sh_size, 1, arquivo);

            char *strtab = malloc(secao[secao[i].sh_link].sh_size);
            if (!strtab) {
                perror("Falha ao alocar memória para a tabela de strings");
                free(simbolos);
                free(secao);
                return;
            }
            fseek(arquivo, secao[secao[i].sh_link].sh_offset, SEEK_SET);
            fread(strtab, secao[secao[i].sh_link].sh_size, 1, arquivo);

            for (int j = 0; j < secao[i].sh_size / sizeof(Elf64_Sym); j++) {
                printf("  Símbolo %d: Nome: %s Valor: 0x%lx Tamanho: %u\n", 
                       j, &strtab[simbolos[j].st_name], simbolos[j].st_value, simbolos[j].st_size);
            }

            free(strtab);
            free(simbolos);
        }
    }

    free(secao);
}

void analisar_elf(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        perror("Falha ao abrir o arquivo");
        return;
    }

    Elf64_Ehdr cabecalho;
    if (fread(&cabecalho, sizeof(cabecalho), 1, arquivo) != 1) {
        perror("Falha ao ler o cabeçalho ELF");
        fclose(arquivo);
        return;
    }

    if (memcmp(cabecalho.e_ident, ELFMAG, SELFMAG) != 0) {
        printf("Não é um arquivo ELF.\n");
        fclose(arquivo);
        return;
    }

    imprimir_cabecalho_elf(&cabecalho);
    imprimir_cabecalhos_secao(arquivo, &cabecalho);
    imprimir_simbolos(arquivo, &cabecalho);

    fclose(arquivo);
}

void gerar_relatorio(const char *nome_arquivo, Elf64_Ehdr *cabecalho) {
    FILE *relatorio = fopen("relatorio.txt", "w");
    if (!relatorio) {
        perror("Falha ao criar o relatório");
        return;
    }

    fprintf(relatorio, "Análise do binário: %s\n", nome_arquivo);
    fprintf(relatorio, "Tipo ELF: %u\n", cabecalho->e_type);
    fprintf(relatorio, "Endereço do Ponto de Entrada: 0x%lx\n", cabecalho->e_entry);
    fprintf(relatorio, "Número de Cabeçalhos de Seção: %u\n", cabecalho->e_shnum);
    fclose(relatorio);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_binario>\n", argv[0]);
        return 1;
    }

    analisar_elf(argv[1]);
    return 0;
}
