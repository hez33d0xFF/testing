---

# Analisador ELF

Este programa é um analisador básico de arquivos ELF (Executable and Linkable Format). Ele lê um arquivo ELF e imprime informações sobre o cabeçalho, seções e símbolos contidos nele.

## Funcionalidades

- Exibe o cabeçalho ELF, incluindo informações como tipo, máquina, versão e endereço de entrada.
- Lista os cabeçalhos de seção e seus tipos.
- Exibe a tabela de símbolos, incluindo nomes e valores.


```bash
gcc -o fodase main.c
```

## Uso

Para usar o programa, execute-o passando o caminho para um arquivo ELF como argumento:

```bash
./fodase <caminho_para_o_arquivo_elf>
```

## Exemplo

```bash
./fodase malware.elf
```

## Saída

O programa imprimirá informações sobre o cabeçalho ELF, seções e símbolos no terminal, além de gerar um arquivo `relatorio.txt` com um resumo da análise.

## Contribuições

Contribuições são bem-vindas! Sinta-se à vontade para abrir problemas ou solicitar recursos.

---
