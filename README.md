# Acordo de Chaves Pos-Quantico com ML-KEM em C

Projeto academico de Seguranca da Computacao sobre acordo de chaves pos-quantico
usando ML-KEM com a biblioteca Open Quantum Safe `liboqs`.

## Objetivo

Demonstrar, em C, o fluxo de um KEM entre duas partes:

1. Bob gera uma chave publica e uma chave privada.
2. Alice encapsula um segredo usando a chave publica de Bob.
3. Alice envia o `ciphertext` KEM para Bob.
4. Bob decapsula o `ciphertext` com sua chave privada.
5. O programa verifica se os segredos compartilhados de Alice e Bob coincidem.

O segredo compartilhado nao e impresso integralmente. A saida mostra apenas
tamanhos, tempos, resultado da comparacao e prefixos truncados em hexadecimal.

## Tecnologias

- Linguagem C
- `liboqs`, do projeto Open Quantum Safe
- Algoritmo principal: `ML-KEM-768`
- `make` para compilacao
- Scripts para geracao de evidencias

## Estrutura

```text
.
|-- Makefile
|-- run_demo.sh
|-- benchmark.sh
|-- src/
|   |-- main.c
|   |-- kem_demo.c
|   |-- kem_demo.h
|   |-- utils.c
|   `-- utils.h
```

## Requisitos

A `liboqs` precisa estar instalada com headers e biblioteca acessiveis ao compilador.
O `Makefile` usa `pkg-config` quando disponivel e tambem tenta linkar com `-loqs`.

Uma instalacao tipica a partir do codigo-fonte da `liboqs` segue o fluxo:

```bash
git clone https://github.com/open-quantum-safe/liboqs.git
cd liboqs
mkdir build
cd build
cmake -GNinja -DBUILD_SHARED_LIBS=ON ..
ninja
sudo ninja install
sudo ldconfig
```

Se a instalacao estiver em um prefixo nao padrao, ajuste `PKG_CONFIG_PATH`,
`LD_LIBRARY_PATH`, `CPPFLAGS` ou `LDLIBS` antes de compilar.

## Como compilar

```bash
make
```

## Como executar

```bash
make run
```

Execucao direta:

```bash
./kem_experiment
```

Selecionando outro parametro ML-KEM:

```bash
./kem_experiment --algorithm ML-KEM-512
./kem_experiment --algorithm ML-KEM-1024
```

## Como gerar evidencias

```bash
./run_demo.sh
```

O comando salva a execucao em:

```text
evidencias/execucoes/demo_output.txt
```

## Como executar benchmark

```bash
./benchmark.sh 30
```

O comando salva CSV em:

```text
evidencias/benchmarks/benchmark_mlkem.csv
```

## Resultado esperado

A demonstracao deve terminar com:

```text
Segredos iguais: SIM

Resultado final:
Acordo de chaves ML-KEM executado com sucesso.
```

## Autores

- Leonardo Guimaraes de Melo Brito
- Pedro Costa Casotti
