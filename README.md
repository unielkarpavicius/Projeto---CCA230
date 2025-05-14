# Projeto-CCA230
Projeto de Estrutura de Dados e Análise de Algoritmos

# Gerenciador de Atendimento Médico

Projeto desenvolvido para a disciplina **Estrutura de Dados e Análise de Algoritmos (CCA230)** do curso de **Engenharia de Robôs** – **Centro Universitário FEI**.

## Descrição

Este sistema tem como objetivo gerenciar os atendimentos em um serviço de saúde, com funcionalidades para cadastro, fila de atendimento, atendimento prioritário, pesquisa, desfazer operações e armazenamento de dados em arquivo.

---

## Funcionalidades

### Cadastro de Pacientes
- Inserção em lista encadeada (ordem de registro).
- Operações:
  - Cadastrar novo paciente  
  - Consultar paciente cadastrado  
  - Mostrar lista completa  
  - Atualizar dados  
  - Remover paciente  

### Atendimento
- Inserção em fila (lista encadeada).
- Operações:
  - Enfileirar paciente  
  - Desenfileirar paciente  
  - Mostrar fila  

### Atendimento Prioritário
- Utiliza um heap, ordenado pela idade do paciente (prioridade para idosos).
- Capacidade máxima: 20 pacientes.
- Operações:
  - Enfileirar paciente  
  - Desenfileirar paciente  
  - Mostrar fila  

### Pesquisa
- Estrutura: árvore binária de busca.
- Operações:
  - Listar registros ordenados por:
    - Ano de registro  
    - Mês de registro  
    - Dia de registro  
    - Idade do paciente  

### Desfazer
- Utiliza pilha para registrar operações realizadas sobre a fila.
- Operações:
  - Exibir log de operações  
  - Desfazer última operação (com confirmação)  

### Carregar / Salvar
- Carregamento de dados de pacientes a partir de arquivo.
- Salvamento dos dados mantendo a ordem de inserção.

### Sobre
- Dados dos desenvolvedores:
  - Nome: Luana Nascimento Karpavicius e Matheus Marcondes Pereira Rufino;
  - Ciclo: 7º semestre;
  - Curso: Engenharia de Robôs;
  - Disciplina: CCA230 - Estrutura de Dados e Análise de Algoritmos;
  - Data: 14/05/2025

---

## Estruturas de dados utilizadas utilizadas
 
  - Lista encadeada  
  - Fila  
  - Heap 
  - Árvore binária de busca  
  - Pilha  
