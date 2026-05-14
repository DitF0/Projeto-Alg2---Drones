/*
    Projeto da Disciplina - Algoritmos 2
    Planejador de Entregas por Drones em Cidade Inteligente

    Neste projeto meu professor querido eu implementei dois algoritmos para resolver o problema de
    seleção de entregas para um drone: o Algoritmo Guloso e o Backtracking.
    O objetivo é maximizar a recompensa total das entregas respeitando os
    limites de peso, bateria, tempo e quantidade de entregas do drone.

    Compilar com: g++ -std=c++11 -o projeto_drones projeto_drones.cpp
    Executar com: ./projeto_drones
*/

// Bibliotecas que precisei incluir para o projeto funcionar
#include <iostream>   // para usar cout e cin (entrada e saída no terminal)
#include <vector>     // para usar vector, que é como uma lista dinâmica em C++
#include <string>     // para usar string (texto)
#include <fstream>    // para abrir e ler arquivos (usado na leitura do CSV)
#include <sstream>    // para o stringstream, que me ajuda a separar valores de uma linha do CSV
#include <algorithm>  // para usar a função sort (ordenação)
#include <chrono>     // para medir o tempo de execução de cada algoritmo
#include <iomanip>    // para formatar a saída no terminal (setw, fixed, setprecision)

using namespace std;

// =============================================================================
// CLASSE: Entrega
//
// Criei essa classe para representar cada entrega disponível no sistema.
// Cada entrega tem um conjunto de informações (id, peso, prioridade, etc.)
// que o drone precisa considerar na hora de decidir o que levar.
//
// Usei encapsulamento: os atributos são privados e só podem ser acessados
// pelos métodos getters. Isso é uma boa prática de orientação a objetos
// porque evita que o valor de um atributo seja alterado acidentalmente
// em outra parte do código.
// =============================================================================
class Entrega {
    // Atributos privados — só acessíveis dentro da própria classe
    int id;           // identificador único da entrega
    double peso;      // peso do pacote em kg
    int prioridade;   // nível de prioridade (1 = baixa, 2 = média, 3 = alta)
    double recompensa;// valor que o drone "ganha" ao completar essa entrega
    double bateria;   // porcentagem de bateria que essa entrega consome
    double tempo;     // tempo estimado para realizar a entrega (em minutos)
    double distancia; // distância do destino em km

public:
    // Construtor: inicializa todos os atributos usando lista de inicialização
    // (forma mais eficiente em C++ do que atribuir dentro do corpo)
    Entrega(int i, double p, int pr, double r, double b, double t, double d)
        : id(i), peso(p), prioridade(pr), recompensa(r), bateria(b), tempo(t), distancia(d) {}

    // Métodos getters — cada um retorna o valor de um atributo
    // O "const" no final significa que esse método não modifica o objeto
    int    getId()         const { return id; }
    double getPeso()       const { return peso; }
    int    getPrioridade() const { return prioridade; }
    double getRecompensa() const { return recompensa; }
    double getBateria()    const { return bateria; }
    double getTempo()      const { return tempo; }
    double getDistancia()  const { return distancia; }
};

// =============================================================================
// CLASSE: Drone
//
// Essa classe representa as limitações operacionais do drone.
// Toda vez que o sistema rodar, ele vai usar um objeto Drone para saber
// até onde pode ir com as seleções de entrega.
//
// Os valores ficam encapsulados aqui para não ficarem espalhados pelo código
// como "números mágicos" (tipo if peso > 10 ... if bateria > 100 ...).
// Assim fica mais fácil de mudar os limites em um único lugar.
// =============================================================================
class Drone {
    // Limites máximos que o drone consegue suportar
    double maxPeso;     // carga máxima em kg
    double maxBateria;  // bateria total disponível em %
    double maxTempo;    // tempo máximo de missão em minutos
    int    maxEntregas; // quantidade máxima de entregas por missão

public:
    // Construtor que recebe e inicializa todos os limites do drone
    Drone(double mp, double mb, double mt, int me)
        : maxPeso(mp), maxBateria(mb), maxTempo(mt), maxEntregas(me) {}

    // Getters para acessar cada restrição de fora da classe
    double getMaxPeso()     const { return maxPeso; }
    double getMaxBateria()  const { return maxBateria; }
    double getMaxTempo()    const { return maxTempo; }
    int    getMaxEntregas() const { return maxEntregas; }
};

// =============================================================================
// STRUCT: Resultado
//
// Usei uma struct (estrutura) aqui porque é só um agrupamento de dados,
// sem comportamento específico — diferente das classes acima que têm lógica.
//
// Esse Resultado é preenchido pelo Guloso ou pelo Backtracking e depois
// usado pelas funções de impressão e comparação.
// Os valores são inicializados como 0 diretamente na declaração (C++11).
// =============================================================================
struct Resultado {
    string metodo;              // nome do algoritmo que gerou esse resultado
    vector<Entrega> selecionadas; // lista das entregas que foram escolhidas
    double pesoTotal      = 0;  // soma dos pesos das entregas selecionadas
    double bateriaTotal   = 0;  // soma da bateria consumida
    double tempoTotal     = 0;  // soma do tempo das entregas selecionadas
    double recompensaTotal = 0; // soma das recompensas (critério que quero maximizar)
    double tempoExecucao  = 0;  // quanto tempo o algoritmo demorou para rodar (em ms)
};

// =============================================================================
// STRUCT: EstadoBT
//
// Criei essa struct para guardar o estado interno do Backtracking.
// Antes eu usava variáveis globais para isso (melhorRecBT e melhorSelBT),
// mas variáveis globais são uma má prática: se o algoritmo fosse chamado
// várias vezes seguidas, o resultado anterior poderia "contaminar" o próximo.
//
// Com essa struct, cada chamada ao Backtracking tem seu próprio estado
// isolado, o que deixa o código mais correto e mais fácil de entender.
// =============================================================================
struct EstadoBT {
    double melhorRec = 0;       // melhor recompensa encontrada até agora na busca
    vector<Entrega> melhorSel;  // lista de entregas que deram essa melhor recompensa
};

// =============================================================================
// FUNÇÃO: filtrarViaveis
//
// Antes de rodar qualquer algoritmo, preciso garantir que não vou tentar
// selecionar uma entrega que já seria impossível de fazer sozinha.
// Por exemplo, se uma entrega pesa 12 kg e o drone suporta só 10 kg,
// ela nunca vai caber — não importa o que mais eu escolha.
//
// O professor pediu explicitamente isso: "Entregas individualmente inviáveis
// devem ser desconsideradas." Então aqui eu faço essa triagem inicial.
//
// Retorna um novo vetor só com as entregas que o drone consegue fazer
// individualmente. Também avisa no terminal sobre o que foi descartado.
// =============================================================================
vector<Entrega> filtrarViaveis(const vector<Entrega>& entregas, const Drone& drone) {
    vector<Entrega> viaveis; // vai guardar só as entregas que passarem no filtro
    int descartadas = 0;     // contador para mostrar no final quantas foram removidas

    for (const auto& e : entregas) {
        // Verifico se essa entrega, sozinha, já respeita os três limites principais
        if (e.getPeso()    <= drone.getMaxPeso()    &&
            e.getBateria() <= drone.getMaxBateria() &&
            e.getTempo()   <= drone.getMaxTempo()) {

            viaveis.push_back(e); // entrega passou no filtro, entra na lista
        } else {
            // Avisa qual entrega foi descartada e por quê
            cout << "  [DESCARTADA] Entrega #" << e.getId()
                 << " e individualmente inviavel (excede restricoes do drone).\n";
            descartadas++;
        }
    }

    // Se alguma entrega foi removida, mostro o total para o usuário saber
    if (descartadas > 0) {
        cout << "  Total de entregas descartadas: " << descartadas << "\n\n";
    }

    return viaveis;
}

// =============================================================================
// FUNÇÃO: imprimirTabela
//
// Responsável por mostrar os resultados de um algoritmo no terminal
// de forma organizada, usando o <iomanip> para alinhar as colunas.
//
// Essa função só exibe — não calcula nada. Recebe um Resultado já pronto
// e formata tudo bonitinho como uma tabela de painel logístico.
// =============================================================================
void imprimirTabela(const Resultado& res) {
    cout << "\n>>> RESULTADOS: " << res.metodo << " <<<\n";

    // Se não encontrou nenhuma entrega viável, mostra alerta e encerra
    if (res.selecionadas.empty()) {
        cout << "ALERTA: Nenhuma solucao viavel encontrada para as restricoes dadas.\n";
        return;
    }

    // Linha separadora e cabeçalho da tabela
    cout << setfill('-') << setw(80) << "-" << setfill(' ') << "\n";
    cout << setw(5)  << "ID"
         << setw(12) << "Peso(kg)"
         << setw(12) << "Prior."
         << setw(15) << "Recompensa"
         << setw(12) << "Bateria"
         << setw(12) << "Tempo"
         << setw(12) << "Dist.\n";
    cout << setfill('-') << setw(80) << "-" << setfill(' ') << "\n";

    // Percorre cada entrega selecionada e imprime seus dados na linha
    for (const auto& e : res.selecionadas) {
        cout << setw(5)  << e.getId()
             << setw(12) << fixed << setprecision(2) << e.getPeso()
             << setw(12) << e.getPrioridade()
             << setw(15) << e.getRecompensa()
             << setw(12) << e.getBateria()
             << setw(12) << e.getTempo()
             << setw(12) << e.getDistancia() << "\n";
    }

    // Linha final e resumo dos totais
    cout << setfill('-') << setw(80) << "-" << setfill(' ') << "\n";
    cout << "Entregas atendidas: " << res.selecionadas.size() << "\n";
    cout << "Peso total:         " << res.pesoTotal    << " / " << fixed << setprecision(2) << 10.0  << " kg\n";
    cout << "Bateria total:      " << res.bateriaTotal << " / " << fixed << setprecision(2) << 100.0 << " %\n";
    cout << "Tempo total:        " << res.tempoTotal   << " / " << fixed << setprecision(2) << 120.0 << " min\n";
    cout << "Recompensa total:   " << res.recompensaTotal << "\n";
    cout << "Tempo de execucao:  " << fixed << setprecision(4) << res.tempoExecucao << " ms\n";
    cout << "================================================================================\n";
}

// =============================================================================
// FUNÇÃO: resolverGuloso
//
// Esse é o Algoritmo Guloso. A ideia do algoritmo guloso é: a cada passo,
// faço a escolha que parece melhor naquele momento, sem voltar atrás.
//
// A heurística que escolhi foi: Recompensa / (Peso + Bateria/10)
// Isso mede o quanto de recompensa cada entrega oferece por unidade de
// recurso consumido (peso e bateria juntos). Divido a bateria por 10
// para deixar as duas grandezas em escalas parecidas.
//
// Funciona assim:
//   1. Ordeno todas as entregas do maior para o menor valor de eficiência
//   2. Vou pegando na ordem até não caber mais nenhuma
//
// Não garante a solução ótima, mas é muito rápido: O(n log n) para
// ordenar mais O(n) para percorrer a lista.
//
// Obs: recebo o vetor por cópia (não por referência) porque vou ordenar
// ele aqui dentro sem querer alterar o original.
// =============================================================================
Resultado resolverGuloso(vector<Entrega> entregas, const Drone& drone) {
    // Marco o tempo de início para medir o desempenho depois
    auto inicio = chrono::high_resolution_clock::now();

    // Ordeno as entregas pela minha heurística de eficiência (maior = melhor)
    // Uso uma função lambda como critério de comparação no sort
    sort(entregas.begin(), entregas.end(), [](const Entrega& a, const Entrega& b) {
        double eficienciaA = a.getRecompensa() / (a.getPeso() + a.getBateria() / 10.0);
        double eficienciaB = b.getRecompensa() / (b.getPeso() + b.getBateria() / 10.0);
        return eficienciaA > eficienciaB; // ordem decrescente
    });

    Resultado res;
    res.metodo = "ALGORITMO GULOSO (Heuristica: Recompensa / Peso+Bateria)";

    // Percorro as entregas já ordenadas e pego as que ainda couberem
    for (const auto& e : entregas) {
        // Verifico todas as restrições antes de incluir a entrega
        if ((int)res.selecionadas.size() < drone.getMaxEntregas()      &&
            res.pesoTotal    + e.getPeso()    <= drone.getMaxPeso()     &&
            res.bateriaTotal + e.getBateria() <= drone.getMaxBateria()  &&
            res.tempoTotal   + e.getTempo()   <= drone.getMaxTempo()) {

            // Entrega cabe! Adiciono na seleção e acumulo os totais
            res.selecionadas.push_back(e);
            res.pesoTotal       += e.getPeso();
            res.bateriaTotal    += e.getBateria();
            res.tempoTotal      += e.getTempo();
            res.recompensaTotal += e.getRecompensa();
        }
        // Se não coube, simplesmente pulo para a próxima (sem voltar atrás)
    }

    // Calculo o tempo total que o algoritmo demorou
    auto fim = chrono::high_resolution_clock::now();
    res.tempoExecucao = chrono::duration<double, milli>(fim - inicio).count();
    return res;
}

// =============================================================================
// FUNÇÃO AUXILIAR: backtrack
//
// Essa é a função recursiva que faz o trabalho pesado do Backtracking.
// Ela explora todas as combinações possíveis de entregas uma por uma.
//
// Como ela funciona:
//   - A cada chamada, decido se incluo ou não a entrega na posição 'indice'
//   - Se incluo: verifico se ainda respeita as restrições e chamo de novo
//     para a próxima posição (indo mais fundo na recursão)
//   - Depois removo a entrega (pop_back) e tento a próxima — isso é o
//     "retrocesso" que dá nome ao algoritmo (backtrack = voltar atrás)
//   - Guardo no 'estado' a melhor solução encontrada em qualquer ponto
//
// Recebo o 'estado' por referência para atualizar a melhor solução
// sem precisar de variáveis globais (isso é importante para não misturar
// resultados quando o algoritmo é chamado várias vezes).
//
// Complexidade: O(2^n) no pior caso — para cada entrega há 2 opções
// (incluir ou não), então o número de combinações cresce exponencialmente.
// =============================================================================
void backtrack(int indice,
               const vector<Entrega>& entregas,
               const Drone& drone,
               vector<Entrega>& atualSel,   // seleção atual sendo construída
               double p, double b, double t, double r, // totais acumulados
               EstadoBT& estado) {           // melhor resultado até agora

    // Atualizo a melhor solução se a seleção atual for melhor do que a anterior
    if (r > estado.melhorRec) {
        estado.melhorRec = r;
        estado.melhorSel = atualSel;
    }

    // Condições de parada da recursão:
    // - Acabaram as entregas disponíveis para explorar
    // - Já atingimos o limite máximo de entregas do drone
    if (indice == (int)entregas.size() ||
        (int)atualSel.size() == drone.getMaxEntregas()) return;

    // Exploro as entregas a partir da posição 'indice' (sem repetir anteriores)
    for (int i = indice; i < (int)entregas.size(); ++i) {

        // Só entro nesse ramo se adicionar essa entrega ainda respeitar os limites
        if (p + entregas[i].getPeso()    <= drone.getMaxPeso()    &&
            b + entregas[i].getBateria() <= drone.getMaxBateria() &&
            t + entregas[i].getTempo()   <= drone.getMaxTempo()) {

            // Adiciono a entrega à seleção atual e desço na recursão
            atualSel.push_back(entregas[i]);

            backtrack(i + 1, entregas, drone, atualSel,
                      p + entregas[i].getPeso(),
                      b + entregas[i].getBateria(),
                      t + entregas[i].getTempo(),
                      r + entregas[i].getRecompensa(),
                      estado);

            // RETROCESSO: removo a entrega e experimento a próxima combinação
            atualSel.pop_back();
        }
    }
}

// =============================================================================
// FUNÇÃO: resolverBacktracking
//
// Essa função é a "interface pública" do Backtracking — ela prepara tudo
// e chama a função recursiva backtrack().
//
// Criei o EstadoBT aqui dentro (variável local) em vez de usar variáveis
// globais. Assim, se eu chamar essa função várias vezes seguidas no mesmo
// programa, cada chamada começa do zero sem interferência da anterior.
//
// Após a recursão terminar, monto o Resultado com os dados da melhor
// solução encontrada pelo backtrack.
// =============================================================================
Resultado resolverBacktracking(const vector<Entrega>& entregas, const Drone& drone) {
    EstadoBT estado;           // estado local — armazena a melhor solução encontrada
    vector<Entrega> atualSel;  // seleção sendo construída durante a recursão

    // Mede o tempo antes e depois de toda a busca recursiva
    auto inicio = chrono::high_resolution_clock::now();
    backtrack(0, entregas, drone, atualSel, 0, 0, 0, 0, estado);
    auto fim = chrono::high_resolution_clock::now();

    // Monto o Resultado com o que o backtrack encontrou de melhor
    Resultado res;
    res.metodo          = "BACKTRACKING (Busca Exaustiva - Solucao Otima)";
    res.selecionadas    = estado.melhorSel;
    res.recompensaTotal = estado.melhorRec;

    // Recalculo os totais percorrendo as entregas da melhor seleção
    for (const auto& e : res.selecionadas) {
        res.pesoTotal    += e.getPeso();
        res.bateriaTotal += e.getBateria();
        res.tempoTotal   += e.getTempo();
    }

    res.tempoExecucao = chrono::duration<double, milli>(fim - inicio).count();
    return res;
}

// =============================================================================
// FUNÇÃO: carregarCSV
//
// Essa função lê um arquivo CSV e transforma cada linha em um objeto Entrega.
// O formato esperado é: id,peso,prioridade,recompensa,bateria,tempo,distancia
// A primeira linha (cabeçalho) é ignorada.
//
// Usei ifstream para abrir o arquivo e stringstream para separar os campos
// pelo caractere vírgula. Se o arquivo não existir, avisa o usuário.
// =============================================================================
vector<Entrega> carregarCSV(const string& filename) {
    vector<Entrega> entregas; // vetor que vai acumular as entregas lidas
    ifstream file(filename);  // tenta abrir o arquivo

    // Verifica se o arquivo foi aberto com sucesso
    if (!file.is_open()) {
        cout << "Erro ao abrir arquivo: " << filename << ". Verifique se o arquivo existe.\n";
        return {}; // retorna vetor vazio
    }

    string line, word;
    getline(file, line); // pula a primeira linha (cabeçalho: id,peso,prioridade,...)

    // Lê linha por linha até o fim do arquivo
    while (getline(file, line)) {
        if (line.empty()) continue; // ignora linhas em branco

        stringstream ss(line);     // transforma a linha num "stream" para separar campos
        vector<string> row;        // vai guardar cada campo da linha

        // Separa os campos usando vírgula como delimitador
        while (getline(ss, word, ',')) row.push_back(word);

        // Só cria a entrega se tiver pelo menos 7 campos (todos obrigatórios)
        if (row.size() >= 7) {
            // Converte as strings para os tipos numéricos corretos e cria o objeto
            entregas.emplace_back(
                stoi(row[0]),  // id → int
                stod(row[1]),  // peso → double
                stoi(row[2]),  // prioridade → int
                stod(row[3]),  // recompensa → double
                stod(row[4]),  // bateria → double
                stod(row[5]),  // tempo → double
                stod(row[6])   // distancia → double
            );
        }
    }

    file.close();
    cout << "  " << entregas.size() << " entrega(s) carregada(s) do arquivo '" << filename << "'.\n";
    return entregas;
}

// =============================================================================
// FUNÇÃO: comparar
//
// Mostra uma tabela comparando os resultados do Guloso e do Backtracking
// lado a lado, facilitando a análise de qual foi melhor em cada métrica.
//
// Também imprime uma análise textual automática explicando o resultado.
// Essa comparação é obrigatória pelo enunciado do projeto.
// =============================================================================
void comparar(const Resultado& g, const Resultado& b) {
    cout << "\n================================================================================\n";
    cout << "                     COMPARACAO FINAL DAS ABORDAGENS\n";
    cout << "================================================================================\n";

    // Cabeçalho da tabela comparativa
    cout << left
         << setw(25) << "Metrica"
         << setw(25) << "Guloso"
         << setw(25) << "Backtracking\n";
    cout << setfill('-') << setw(75) << "-" << setfill(' ') << "\n";

    // Linhas da comparação
    cout << setw(25) << "Recompensa Total"
         << setw(25) << g.recompensaTotal
         << setw(25) << b.recompensaTotal << "\n";

    cout << setw(25) << "Entregas Realizadas"
         << setw(25) << g.selecionadas.size()
         << setw(25) << b.selecionadas.size() << "\n";

    cout << setw(25) << "Tempo Execucao (ms)"
         << setw(25) << fixed << setprecision(4) << g.tempoExecucao
         << setw(25) << b.tempoExecucao << "\n";

    cout << "--------------------------------------------------------------------------------\n";

    // Análise automática: comparo as recompensas para concluir qual foi melhor
    if (b.recompensaTotal > g.recompensaTotal) {
        // Backtracking achou uma solução melhor (o que era esperado — ele é exato)
        cout << "Analise: O Backtracking encontrou uma solucao melhor (+"
             << fixed << setprecision(2) << (b.recompensaTotal - g.recompensaTotal)
             << " de recompensa a mais).\n";
    } else if (b.recompensaTotal == g.recompensaTotal) {
        // Ambos chegaram no mesmo resultado — a heurística foi eficiente aqui
        cout << "Analise: Ambos alcancaram a mesma recompensa total.\n"
             << "         O Guloso se saiu tao bem quanto o otimo neste caso.\n";
    } else {
        // Situação inesperada — nunca deveria acontecer se o código estiver certo
        cout << "Analise: Situacao atipica. Verifique os dados de entrada.\n";
    }

    // Mostro quantas vezes o Guloso foi mais rápido que o Backtracking
    if (g.tempoExecucao > 0) {
        cout << "O Guloso foi " << fixed << setprecision(1)
             << (b.tempoExecucao / g.tempoExecucao)
             << "x mais rapido que o Backtracking neste caso.\n";
    }

    cout << "================================================================================\n\n";
}

// =============================================================================
// FUNÇÃO: executarCaso
//
// Essa função centraliza o fluxo completo de execução de um caso de teste:
//   1. Verifica se há entregas para processar
//   2. Filtra as entregas individualmente inviáveis
//   3. Roda o Algoritmo Guloso
//   4. Roda o Backtracking
//   5. Exibe os resultados de cada um
//   6. Mostra a comparação final
//
// Criei essa função para não repetir esse bloco de código dentro de cada
// opção do menu — é o conceito de "não se repita" (DRY: Don't Repeat Yourself).
// =============================================================================
void executarCaso(const vector<Entrega>& entregasBruto, const Drone& drone) {
    // Caso o vetor venha vazio (ex: CSV não encontrado), aviso e saio
    if (entregasBruto.empty()) {
        cout << "Nenhuma entrega disponivel para processar.\n";
        return;
    }

    // Passo 1: filtro as entregas que são individualmente inviáveis
    cout << "\n--- Verificando viabilidade individual das entregas ---\n";
    vector<Entrega> entregas = filtrarViaveis(entregasBruto, drone);

    // Se todas foram descartadas, não tem o que otimizar
    if (entregas.empty()) {
        cout << "ALERTA: Nenhuma entrega e individualmente viavel para este drone.\n\n";
        return;
    }

    // Passo 2: executo os dois algoritmos com as entregas viáveis
    Resultado rG = resolverGuloso(entregas, drone);
    Resultado rB = resolverBacktracking(entregas, drone);

    // Passo 3: exibo os resultados e a comparação
    imprimirTabela(rG);
    imprimirTabela(rB);
    comparar(rG, rB);
}

// =============================================================================
// FUNÇÃO PRINCIPAL: main
//
// Aqui é onde tudo começa. Crio o drone com suas restrições e apresento
// um menu para o usuário escolher qual caso de teste quer executar.
//
// Ofereci 5 opções de dados:
//   - CSV externo (dados reais ou simulados em arquivo)
//   - Caso Equilibrado (entregas balanceadas, sem pressão extrema)
//   - Alta Demanda (muitas entregas competindo pelos mesmos recursos)
//   - Caso Inviável (todos os pacotes excedem os limites do drone)
//   - Mix com entrega individualmente inviável (testa o filtro explícito)
//
// O loop while mantém o menu ativo até o usuário escolher sair (opção 6).
// =============================================================================
int main() {
    // Crio o drone com os limites definidos no enunciado do projeto:
    // 10 kg de carga, 100% de bateria, 120 min de missão, máx 4 entregas
    Drone drone(10.0, 100.0, 120.0, 4);

    int opcao = 0;

    // Loop principal do menu — fica rodando até o usuário digitar 6
    while (opcao != 6) {
        // Exibo o menu de opções
        cout << "=====================================\n";
        cout << "  SISTEMA DE ENTREGAS POR DRONE 2.0\n";
        cout << "=====================================\n";
        cout << "1. Carregar Dados de 'entregas.csv'\n";
        cout << "2. Caso de Teste 1 - Equilibrado\n";
        cout << "3. Caso de Teste 2 - Alta Demanda\n";
        cout << "4. Caso de Teste 3 - Inviavel (todos excedem limites)\n";
        cout << "5. Caso de Teste 4 - Com entrega individualmente inviavel\n";
        cout << "6. Sair\n";
        cout << "Escolha: ";
        cin >> opcao;

        vector<Entrega> entregas; // vetor que vai receber as entregas do caso escolhido

        if (opcao == 1) {
            // Lê as entregas do arquivo CSV na pasta atual
            entregas = carregarCSV("entregas.csv");
        }
        else if (opcao == 2) {
            // Caso equilibrado: 5 entregas com pesos e tempos variados
            // Boa mistura para testar os dois algoritmos sem pressão extrema
            entregas = {
                {1, 2.5, 1,  50, 20, 30,  5},
                {2, 4.0, 2,  80, 35, 40,  8},
                {3, 3.0, 1,  60, 25, 35,  6},
                {4, 5.0, 3, 100, 50, 60, 10},
                {5, 1.5, 2,  40, 15, 20,  3}
            };
        }
        else if (opcao == 3) {
            // Alta demanda: 6 entregas competindo pelos mesmos recursos limitados
            // Aqui é mais provável que o Backtracking encontre algo melhor que o Guloso
            entregas = {
                {1, 1.0, 1,  20, 10, 15,  2},
                {2, 2.0, 2,  50, 15, 20,  4},
                {3, 6.0, 3, 120, 60, 80, 12},
                {4, 4.5, 1,  90, 40, 50,  9},
                {5, 3.0, 3,  70, 20, 30,  6},
                {6, 2.5, 2,  60, 25, 25,  5}
            };
        }
        else if (opcao == 4) {
            // Caso inviável: todos os pacotes são pesados demais para o drone
            // Serve para testar a mensagem de "nenhuma solução viável"
            entregas = {
                {1, 15.0, 1, 50, 20, 30, 10},
                {2, 20.0, 2, 80, 35, 40, 15}
            };
        }
        else if (opcao == 5) {
            // Mix com entrega inviável individual:
            // A entrega #2 tem 12 kg, que já passa do limite de 10 kg sozinha
            // Serve para testar se o filtro de inviáveis está funcionando
            // (ela tem recompensa altíssima de 999, mas não pode ser selecionada)
            entregas = {
                {1,  2.0, 1,  70, 20, 25,  4},
                {2, 12.0, 3, 999, 50, 60, 20}, // INVIAVEL: peso 12 > limite 10
                {3,  3.5, 2,  90, 30, 40,  7},
                {4,  1.5, 1,  45, 10, 15,  3},
                {5,  4.0, 2, 110, 45, 55, 10}
            };
        }
        else if (opcao == 6) {
            cout << "Encerrando o sistema. Ate logo!\n";
            break;
        }
        else {
            // Usuário digitou um número fora das opções válidas
            cout << "Opcao invalida. Por favor, escolha entre 1 e 6.\n\n";
            continue; // volta pro topo do while sem executar nada
        }

        // Executa o caso: filtra, roda os algoritmos, imprime e compara
        executarCaso(entregas, drone);
    }

    return 0; // programa encerrado com sucesso
}
