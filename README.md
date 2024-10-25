# Membros da equipe:

### Felipe Sousa de Almeida          RA:22018160
### Guilherme Domingues de Sousa     RA:22013871 

# Projeto: Detecção de Incêndios com Comunicação e Monitores

## 1. Introdução ao Projeto

Este projeto simula um sistema de detecção de incêndio em uma área florestal dividida em células monitoradas por sensores. A ideia é que, ao detectar um incêndio, os sensores alertem uma central de controle, que pode responder ao fogo em bordas da área monitorada. Este sistema usa **threads**, **mutexes** e **condições** para criar uma simulação onde os sensores, incêndios e controle se comunicam e coordenam ações.

## 2. Estrutura do Projeto

O código está organizado em três arquivos principais:

- **main.c**: Define a lógica principal e controla a criação de threads.
- **functions.c** e **functions.h**: Contêm funções auxiliares que inicializam, exibem e atualizam o estado da simulação.

## 3. Funcionamento Geral do Código

Abaixo, segue o funcionamento básico do projeto, dividido em três partes principais:

1. **Inicialização**:
    - A matriz (`forest`) representa a área florestal dividida em células. Cada célula pode estar vazia, conter um sensor, estar em chamas ou já queimada.
    - Cada sensor é configurado como uma thread, que "vive" em uma célula e responde ao surgimento de incêndios.

2. **Sensores e Incêndios**:
    - Cada thread de sensor monitora seu estado e o das células vizinhas.
    - Quando um sensor detecta um incêndio na célula em que se encontra, ele:
        - Propaga o fogo para as células vizinhas.
        - Notifica a central de controle se ele está localizado em uma borda (para facilitar o combate ao fogo).
    - A propagação do incêndio é controlada usando `mutexes` (para garantir que apenas uma thread altere o estado de uma célula por vez) e `condições` (para comunicar mudanças de estado entre threads).

3. **Central de Controle**:
    - A central monitora incêndios relatados em bordas. Quando notificada por um sensor de borda, a central tenta extinguir o fogo na célula, alterando o estado para "BURNED".
    - Para sincronizar o acesso, a central também usa `mutexes` e `condições`, garantindo que ela atue somente quando um sensor de borda solicita intervenção.

## 4. Comunicação e Uso de Monitores

O sistema utiliza duas técnicas principais para comunicação e controle entre as threads:

- **Mutexes**: São usados para impedir que múltiplas threads alterem o estado de uma mesma célula ao mesmo tempo. Assim, um sensor pode verificar ou modificar o estado de uma célula sem risco de conflito com outra thread.
- **Condições**: Permitem que threads aguardem até que certas condições sejam atendidas. Por exemplo, a central de controle "espera" até ser notificada por um sensor de borda de que há um incêndio a combater.

Esses mecanismos permitem que:
- Sensores detectem e respondam a incêndios de forma independente.
- A central só intervenha quando necessário, coordenando a resposta com sensores na borda para extinguir incêndios.

## 5. Link para o Projeto no GitHub

[Link para o projeto no GitHub](https://github.com/thelipesousa/fireDetection)
