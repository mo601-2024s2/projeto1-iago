# Projeto 1 de MO601 - Simulador RISC-V

Neste projeto foi implementado um simulador do processador RISC-V RV32IM, que significa a versão de 32 bits com as instruções básicas e também as instruções de multiplicação e divisão. Veja a especificação detalhada do projeto na [página da disciplina](https://www.ic.unicamp.br/~rodolfo/mo601/projeto1).

## Identificação do Aluno

- RA: 198921
- Nome: Iago Caran Aquino

## Descrição Geral do Projeto

Para este projeto, retomei o código que fiz ao seguir o seguinte tutorial: [Writing a simple vm in less than 125 lines of c](https://www.andreinc.net/2021/12/01/writing-a-simple-vm-in-less-than-125-lines-of-c).
Isso me deu uma base para estruturar o código da simulação.

## Descrição do seu ambiente de desenvolvimento

O desenvolvimento do projeto aconteceu em um sistema Windows 10 utilizando o WSL2 configurado com um Ubuntu 24.04.
Os códigos de teste foram compilados utilizando o *riscv-gnu-toolchain* configurado com *newlib*.

### Instalar o compilador

`
sudo apt-get install autoconf automake autotools-dev curl python3 python3-pip libmpc-dev libmpfr-dev libgmp-dev gawk build-essential bison flex texinfo gperf libtool patchutils bc zlib1g-dev libexpat-dev ninja-build git cmake libglib2.0-dev libslirp-dev

git clone https://github.com/riscv/riscv-gnu-toolchain
cd riscv-gnu-toolchain

export RISCV=/opt/riscv
export PATH=$PATH:$RISCV/bin

./configure --prefix=/opt/riscv --with-arch=rv32gc --with-abi=ilp32  --enable-multilib
sudo make -j`nproc`
`

## Descrição do seu algoritmo de simulação

Antes da simulação começar, a função *ld_elf* carrega o arquivo binário do programa para a posição de memória necessária e define o valor de *pc* como o entry point do código.

Em seguida, a simulação começa na chamada da função *start*, a partir da qual um laço é iniciado enquanto a variável *running* estiver verdadeira.
Para cada iteração, uma nova instrução é lida da memória do programa e decodificada por meio de um array de funções indexado pelo OPCODE.
Ao final de cada instrução, o *pc* é incrementado de 4.

### Execução das instruções



## Descrição de como você testou seu projeto



## Considerações gerais sobre seu aprendizado nesse projeto



## Desempenho do seu simulador conforme DryStone e CoreMark


