# 🚁 Planejador de Entregas por Drones em Cidade Inteligente

Projeto da disciplina **Algoritmos 2** — Faculdade ESEG (Grupo Etapa)  
Professor: Bruno de Abreu Iizuka Moritani

---

## 📋 Descrição do Projeto

Sistema de planejamento de missões para drones de entrega em ambiente urbano.  
O objetivo é **maximizar a recompensa total** das entregas selecionadas, respeitando as restrições operacionais do drone: peso máximo, bateria, tempo de missão e quantidade de entregas.

Foram implementadas e comparadas duas abordagens algorítmicas:

| Abordagem | Estratégia | Complexidade |
|---|---|---|
| **Algoritmo Guloso** | Heurística: Recompensa / (Peso + Bateria/10) | O(n log n) |
| **Backtracking** | Busca exaustiva com retrocesso | O(2ⁿ) |

---

## 📦 Arquivos do Projeto

```
📁 projeto-drones-alg2/
├── projeto_drones.cpp                  → Código-fonte principal em C++
├── entregas.csv                        → 20 entregas para teste via arquivo
├── Relatorio_Tecnico_Individual.md     → Relatório do projeto
├── Roteiro_e_Guia_para_Apresentacao.md → Roteiro da apresentação
└── README.md                           → Este arquivo
```

---

## ⚙️ Como Compilar e Executar

### Pré-requisito
Ter o compilador `g++` instalado (GCC). Para verificar:
```bash
g++ --version
```

### Compilar
```bash
g++ -std=c++11 -o projeto_drones projeto_drones.cpp
```

### Executar
```bash
./projeto_drones
```
> No Windows: `projeto_drones.exe`

> ⚠️ O arquivo `entregas.csv` deve estar na **mesma pasta** do executável.

---

## 🖥️ Menu do Sistema

Ao executar, o programa exibe um menu interativo:

```
=====================================
  SISTEMA DE ENTREGAS POR DRONE 2.0
=====================================
1. Carregar Dados de 'entregas.csv'
2. Caso de Teste 1 - Equilibrado
3. Caso de Teste 2 - Alta Demanda
4. Caso de Teste 3 - Inviavel
5. Caso de Teste 4 - Com entrega individualmente inviavel
6. Sair
```

---

## 🚀 Casos de Teste

| Opção | Descrição | Entregas |
|---|---|---|
| CSV | Carrega do arquivo `entregas.csv` | 20 entregas variadas |
| Teste 1 | Cenário equilibrado | 5 entregas |
| Teste 2 | Alta demanda, concorrência por recursos | 6 entregas |
| Teste 3 | Todos os pacotes excedem os limites | 2 entregas inviáveis |
| Teste 4 | Mix com uma entrega individualmente inviável | 5 entregas (1 filtrada) |

O caso via **CSV é o mais completo e interessante**: com 20 entregas disponíveis
e o drone podendo levar no máximo 4, há uma grande quantidade de combinações
possíveis — o que evidencia bem a diferença entre o Guloso e o Backtracking.

---

## 🔒 Restrições do Drone

| Restrição | Valor |
|---|---|
| Peso máximo | 10 kg |
| Bateria máxima | 100 % |
| Tempo máximo de missão | 120 min |
| Máximo de entregas por missão | 4 |

---

## 📊 Exemplo de Saída

```
>>> RESULTADOS: ALGORITMO GULOSO (Heuristica: Recompensa / Peso+Bateria) <<<
--------------------------------------------------------------------------------
   ID    Peso(kg)      Prior.      Recompensa     Bateria       Tempo       Dist.
--------------------------------------------------------------------------------
    7        1.00           2           60.00       10.00       15.00        3.00
   13        1.00           1           55.00        8.00       12.00        2.50
   20        1.00           1           50.00        9.00       14.00        2.00
    3        1.50           1           80.00       15.00       25.00        4.00
--------------------------------------------------------------------------------
Entregas atendidas: 4
Peso total:         4.50 / 10.00 kg
Bateria total:      42.00 / 100.00 %
Tempo total:        66.00 / 120.00 min
Recompensa total:   245.00
Tempo de execucao:  0.0180 ms
```

---

## 🧠 Sobre os Algoritmos

### Algoritmo Guloso
Ordena as entregas pela razão `recompensa / (peso + bateria/10)` e seleciona
na ordem até não caber mais. Rápido, mas não garante a solução ótima.

### Backtracking
Explora todas as combinações possíveis de entregas com retrocesso, garantindo
a **solução ótima**. Com 20 entregas disponíveis e as podas pelas restrições
do drone, o algoritmo permanece eficiente o suficiente para demonstração.

---

## 📄 Formato do CSV

O arquivo `entregas.csv` contém **20 entregas** e segue o formato:

```
id,peso,prioridade,recompensa,bateria,tempo,distancia
1,2.0,1,100.0,20.0,30.0,5.0
2,3.5,2,150.0,40.0,50.0,10.0
...
```

| Campo | Tipo | Descrição |
|---|---|---|
| id | inteiro | Identificador único da entrega |
| peso | decimal | Peso do pacote em kg |
| prioridade | inteiro | Nível de prioridade (1=baixa, 2=média, 3=alta) |
| recompensa | decimal | Valor ganho ao completar a entrega |
| bateria | decimal | Consumo de bateria em % |
| tempo | decimal | Tempo estimado em minutos |
| distancia | decimal | Distância do destino em km |
