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
├── projeto_drones.cpp   → Código-fonte principal em C++
├── entregas.csv         → Dados de entregas para teste via arquivo
├── Relatório_Técnico_Individual.md  → Relatório do projeto
├── Roteiro_e_Guia_para_Apresentação.md → Roteiro da apresentação
└── README.md            → Este arquivo
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

| Opção | Descrição |
|---|---|
| CSV | Carrega entregas do arquivo `entregas.csv` |
| Teste 1 | 5 entregas equilibradas — cenário padrão |
| Teste 2 | 6 entregas em alta demanda — concorrência por recursos |
| Teste 3 | Todos os pacotes excedem os limites — nenhuma solução viável |
| Teste 4 | Mix com uma entrega individualmente inviável — testa o filtro |

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
    3        1.50           1           80.00       15.00       25.00        4.00
    5        2.50           2          120.00       30.00       40.00        7.00
    1        2.00           1          100.00       20.00       30.00        5.00
--------------------------------------------------------------------------------
Entregas atendidas: 3
Peso total:         6.00 / 10.00 kg
Bateria total:      65.00 / 100.00 %
Tempo total:        95.00 / 120.00 min
Recompensa total:   300.00
Tempo de execucao:  0.0120 ms
```

---

## 🧠 Sobre os Algoritmos

### Algoritmo Guloso
Ordena as entregas pela razão `recompensa / (peso + bateria/10)` e seleciona
na ordem até não caber mais. Rápido, mas não garante a solução ótima.

### Backtracking
Explora todas as combinações possíveis de entregas com retrocesso, garantindo
a **solução ótima**. Custo computacional cresce exponencialmente com o número
de entregas.

---

## 📄 Formato do CSV

O arquivo `entregas.csv` deve seguir o formato abaixo (com cabeçalho):

```
id,peso,prioridade,recompensa,bateria,tempo,distancia
1,2.0,1,100.0,20.0,30.0,5.0
2,3.5,2,150.0,40.0,50.0,10.0
```
