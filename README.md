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

As instruções foram implementadas como funções.
Um array de pointeiros faz a indexação inicial baseado no *opcode*, em seguida as funções chamadas usam *switch-case* com base em *funct3*, *funct5* e *funct7* para selecionar a função correta.

## Descrição de como você testou seu projeto

Durante a primeira fase de implementação, verifiquei que os códigos de decodificação estavam extraindo corretamente as diferentes partes das codificações das instruções.
Num segundo momento, testei alguns códigos mais simples do ACStone no spike para comparar o log de execução com o do meu processador.

O spike pode ser executado com o seguinte comando:

`spike -d --pc=0x1040C --isa=rv32g -m0x10000:0x200000000 test/ACStone/122.loop.elf`

Considerando o resultado deste projeto, considero que preparar antecipadamente um conjunto de testes é a única maneira de atingir um bom resultado, pois isso permitiria testar continuamente o resultado da implementação. Uma boa abordagem seria implementar testes unitários para as diferentes instruções para garantir que a decodificação e a execução estivessem ocorrendo como esperado.

## Considerações gerais sobre seu aprendizado nesse projeto

Este foi um projeto que exige muito tempo para ser executado. 
A implementação de um simulador depende de diversas considerações e demanda muita atenção em cada etapa.
Dentro dos limites do meu tempo e saúde mental, não consegui o resultado que esperava, mas aprendi muito em diversas áreas.

Alguns pontos que considero importantes para garantir a boa execução do projeto:
- Aprender sobre o linker e as informações embutidas no arquivo crt.S, as quais são importantes para configurar o layout de memória do programa;
- Entender o layout de memória do programa e como isso afeta os registradores *sp* e *gp*;
- Entender a estrutura dos arquivos elf, como entender seu conteúdo e copiar corretamente as informações pra dentro da minha representação de memória;
- Entender o uso de syscalls e como elas afetam o estado do simulador;
- Compilar um programa estando ciente das necessidades de uma stdlib e como ela afeta os requisitos do simulador.

Uma possível fonte de problemas é o tamanho da liberdade deste projeto, de forma que não estudar corretamente a melhor maneira de compilar os programas e a escolha de implementação de stdlib pode consumir muito tempo. Por exemplo, a *newlib* que é disponibilizada junto ao *riscv-gnu-toolchain* requer mais syscalls que a *picolibc*, sendo que a segunda aparenta ser mais fácil de customizar as instruções.

## Desempenho do seu simulador conforme DryStone e CoreMark

Não foi possível rodar os benchmarks DryStone e CoreMark no momento, pois algum erro de implementação me impede de executar prints formatados com o resultado esperado.
Dados os logs de execução do ACStone, o problema está em alguma instrução usada na implementação dos laços.
