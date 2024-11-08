#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Endereco {
    char rua[50];
    int numero;
    char cidade[30];
    char estado[3];
};

struct Cliente {
    char cpf[12];
    char nome[50];
    char telefone[15];
    struct Endereco endereco;
    char dataNascimento[11];
};

struct Servico {
    int codigo;
    char descricao[100];
    float preco_mensal;
    float preco_anual;
    float preco_sessao;
    int duracao;
};

struct Agendamento {
    struct Cliente cliente;
    struct Servico servico;
    char data[11]; // formato: dd/mm/yyyy
    char hora[6]; // formato: hh:mm
};

// ############ FUNCOES GLOBAIS ####################
void verificaSeExisteArquivos() {
    char arquivos[3][20] = {"clientes.txt", "servicos.txt", "agendamentos.txt"};
    for (int i = 0; i < 3; i++) {
        FILE *file = fopen(arquivos[i], "r");
        if (!file) {
            // Se o arquivo nao existir, cria um novo arquivo
            file = fopen(arquivos[i], "w");
            if (!file) {
                printf("Erro ao criar o arquivo %s!\n", arquivos[i]);
            }
        }
        fclose(file); // Fecha o arquivo
    }
}

int ehBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int validarData(const char *data) {
    // Verifica se a data tem exatamente 10 caracteres (no formato "dd/mm/aaaa")
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') {
        return 0; // Formato inválido
    }

    // Extrai os valores de dia, mês e ano
    char diaStr[3], mesStr[3], anoStr[5];
    strncpy(diaStr, data, 2);
    diaStr[2] = '\0';
    strncpy(mesStr, data + 3, 2);
    mesStr[2] = '\0';
    strncpy(anoStr, data + 6, 4);
    anoStr[4] = '\0';

    // Converte as partes para inteiros
    int dia = atoi(diaStr);
    int mes = atoi(mesStr);
    int ano = atoi(anoStr);

    // Validações básicas
    if (mes < 1 || mes > 12 || dia < 1 || ano < 1) {
        return 0; // Mês ou dia ou ano inválido
    }

    // Dias máximos por mês
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Ajusta fevereiro para ano bissexto
    if (mes == 2 && ehBissexto(ano)) {
        diasPorMes[1] = 29;
    }

    // Verifica se o dia é válido para o mês
    if (dia > diasPorMes[mes - 1]) {
        return 0; // Dia inválido para o mês
    }

    return 1; // Data válida
}

int validarHora(const char *hora) {
    // Verifica se a hora tem exatamente 5 caracteres (no formato "hh:mm")
    if (strlen(hora) != 5 || hora[2] != ':') {
        return 0; // Formato inválido
    }

    // Extrai os valores de horas e minutos
    char horaStr[3], minStr[3];
    strncpy(horaStr, hora, 2);
    horaStr[2] = '\0';
    strncpy(minStr, hora + 3, 2);
    minStr[2] = '\0';

    // Converte as partes para inteiros
    int horas = atoi(horaStr);
    int minutos = atoi(minStr);

    // Validações para horas e minutos
    if (horas < 0 || horas > 23 || minutos < 0 || minutos > 59) {
        return 0; // Hora ou minutos inválidos
    }

    return 1; // Hora válida
}

// ###########  FUNCOES DE SERVICOS #################
int verificarServicoEmAgendamento(int codServico) {
    FILE *arquivo = fopen("agendamentos.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de agendamentos.\n");
        return 0;
    }

    struct Servico servico;

    char linha[200]; // Ajuste o tamanho conforme necessário
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 2) servico.codigo = atoi(token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }

        if (servico.codigo == codServico) {
            fclose(arquivo);
            return 1; // Serviço encontrado em um agendamento
        }
    }

    fclose(arquivo);
    return 0; // Serviço não encontrado em nenhum agendamento
}

int servicoExiste(int codigoServico) {
    FILE *file = fopen("servicos.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return 0; // Retorna 0 se não for possível abrir o arquivo
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), file) != NULL) {
        char *token = strtok(linha, ";"); // Assume que o codigo do servico é o primeiro campo

        if (token != NULL && atoi(token) == codigoServico) {
            fclose(file);
            return 1; // servico encontrado
        }
    }

    fclose(file);
    return 0; // servico não encontrado
}

void inserirServico() {
    struct Servico novoServico;

    printf("Descricao: ");
    fflush(stdin);
    gets(novoServico.descricao);

    printf("Preco Mensal: ");
    scanf("%f", &novoServico.preco_mensal);

    printf("Preco Anual: ");
    scanf("%f", &novoServico.preco_anual);

    printf("Preco por Sessao: ");
    scanf("%f", &novoServico.preco_sessao);

    printf("Duracao (minutos): ");
    scanf("%d", &novoServico.duracao);

    // Pegando o proximo codigo disponivel
    FILE *arquivo = fopen("servicos.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) novoServico.codigo = atoi(token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }
    }
    novoServico.codigo++;
    fclose(arquivo);

    arquivo = fopen("servicos.txt", "a"); // "a" para adicionar no final do arquivo
    if (!arquivo) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    fprintf(arquivo, "%d;%s;%f;%f;%f;%d\n",
            novoServico.codigo,
            novoServico.descricao,
            novoServico.preco_mensal,
            novoServico.preco_anual,
            novoServico.preco_sessao,
            novoServico.duracao);

    fclose(arquivo);

    printf("Servico adicionado com sucesso!\n");
}

void excluirServico() {
    int codigoBusca, encontrado = 0, exclui;

    printf("Digite o codigo do servico a ser excluido: ");
    scanf("%d", &codigoBusca);

    if (verificarServicoEmAgendamento(codigoBusca)) {
        printf("Esse Servico nao pode ser excluido pois existe agendamento pra ele!\n");
        return;
    }

    FILE *arquivo = fopen("servicos.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        struct Servico servico;
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) servico.codigo = atoi(token);
            else if (i == 2) strcpy(servico.descricao, token);
            else if (i == 3) servico.preco_mensal = strtof(token, NULL);
            else if (i == 4) servico.preco_anual = strtof(token, NULL);
            else if (i == 5) servico.preco_sessao = strtof(token, NULL);
            else if (i == 6) servico.duracao = atoi(token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }
        if (servico.codigo == codigoBusca) {
            encontrado = 1;

            printf("Confirma a Exclusao do Serviço? [1 - Sim, 2 - Nao]");
            scanf("%d", &exclui);
            if (exclui == 1) {
                continue;
            }
        }
        fprintf(temp, "%d;%s;%f;%f;%f;%d\n",
                servico.codigo,
                servico.descricao,
                servico.preco_mensal,
                servico.preco_anual,
                servico.preco_sessao,
                servico.duracao);
    }

    fclose(arquivo);
    fclose(temp);

    // Substitui o arquivo original pelo temporário, caso algum agendamento tenha sido excluído
    if (encontrado) {
        remove("servicos.txt");
        rename("temp.txt", "servicos.txt");
        printf("Servico Excluido com sucesso!\n");
    } else {
        remove("temp.txt");
        printf("Nenhum Serviço correspondente encontrado.\n");
    }
}

void listarServicos() {
    FILE *arquivo = fopen("servicos.txt", "r");
    if (arquivo == NULL) {
        printf("Nenhum servico cadastrado.\n");
        return;
    }

    printf("\n=============== LISTA DE SERVICOS ===============  \n");
    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) printf("Codigo: %s\n", token);
            else if (i == 2) printf("\tDescricao: %s\n", token);
            else if (i == 3) printf("\tPreco Mensal: %s\n", token);
            else if (i == 4) printf("\tPreco Anual: %s\n", token);
            else if (i == 5) printf("\tPreco Sessao: %s\n", token);
            else if (i == 6) printf("\tDuracao: %s\n", token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }
    }
    printf("=================================================\n");

    fclose(arquivo);
}

// ###########  FUNCOES DE CLIENTES #################
int verificarClienteEmAgendamento(const char *cpf) {
    FILE *arquivo = fopen("agendamentos.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo de agendamentos.\n");
        return 0;
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token = strtok(linha, ";"); // Assume que o cpf do cliente é o primeiro campo

        if (token != NULL && strcmp(token, cpf) == 0) {
            fclose(arquivo);
            return 1; // Cliente encontrado
        }
    }

    fclose(arquivo);
    return 0; // Cliente não encontrado em nenhum agendamento
}

int clienteExiste(const char *cpfCliente) {
    FILE *arquivo = fopen("clientes.txt", "r");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return 0; // Retorna 0 se não for possível abrir o arquivo
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token = strtok(linha, ";"); // Assume que o cpf do cliente é o primeiro campo

        if (token != NULL && strcmp(token, cpfCliente) == 0) {
            fclose(arquivo);
            return 1; // Cliente encontrado
        }
    }

    fclose(arquivo);
    return 0; // Cliente não encontrado
}

void inserirCliente() {
    struct Cliente novoCliente;

    printf("CPF: ");
    fflush(stdin);
    gets(novoCliente.cpf);

    // verifica se o cpf tem 11 caracteres
    if (strlen(novoCliente.cpf) != 11) {
        printf("CPF Invalido (tem que possuir 11 caracteres), processo cancelado!\n");
        return;
    }

    // verificar se existe algum cadastrado
    if (clienteExiste(novoCliente.cpf)) {
        printf("Cliente Ja cadastrado, processo cancelado!\n");
        return;
    }

    printf("Nome: ");
    fflush(stdin);
    gets(novoCliente.nome);

    printf("Telefone: ");
    fflush(stdin);
    gets(novoCliente.telefone);

    printf("Rua: ");
    fflush(stdin);
    gets(novoCliente.endereco.rua);

    printf("numero: ");
    scanf("%d", &novoCliente.endereco.numero);

    printf("Cidade: ");
    fflush(stdin);
    gets(novoCliente.endereco.cidade);

    printf("Estado: ");
    scanf("%s", novoCliente.endereco.estado);

    printf("Data de Nascimento (dd/mm/aaaa): ");
    scanf("%s", &novoCliente.dataNascimento);
    if (!validarData(novoCliente.dataNascimento)) {
        printf("A data invalida.\n");
        return;
    }

    // se deu tudo certo, salva o cadastro
    FILE *arquivo = fopen("clientes.txt", "a"); // "a" para adicionar no final do arquivo
    if (!arquivo) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    fprintf(arquivo, "%s;%s;%s;%s;%d;%s;%s;%s\n",
            novoCliente.cpf,
            novoCliente.nome,
            novoCliente.telefone,
            novoCliente.endereco.rua,
            novoCliente.endereco.numero,
            novoCliente.endereco.cidade,
            novoCliente.endereco.estado,
            novoCliente.dataNascimento);

    fclose(arquivo);

    printf("Cliente inserido com sucesso!\n");
}

void excluirCliente() {
    char cpfBusca[12];
    int encontrado = 0, exclui;

    printf("Digite o CPF do cliente que deseja excluir (somente numeros): ");
    scanf("%11s", cpfBusca);

    // falta validar se tem agendamento para o cliente.
    if (verificarClienteEmAgendamento(cpfBusca)) {
        printf("Esse Cliente nao pode ser excluido pois existe agendamento pra ele!\n");
        return;
    }

    FILE *arquivo = fopen("clientes.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    // Lê cada linha do arquivo original e copia para o temporário, exceto o cliente a ser excluído
    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        struct Cliente cliente;
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) strcpy(cliente.cpf, token);
            else if (i == 2) strcpy(cliente.nome, token);
            else if (i == 3) strcpy(cliente.telefone, token);
            else if (i == 4) strcpy(cliente.endereco.rua, token);
            else if (i == 5) cliente.endereco.numero = atoi(token);
            else if (i == 6) strcpy(cliente.endereco.cidade, token);
            else if (i == 7) strcpy(cliente.endereco.estado, token);
            else if (i == 8) strcpy(cliente.dataNascimento, token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }
        if (strcmp(cliente.cpf, cpfBusca) == 0) {
            encontrado = 1;

            printf("Confirma a Exclusao do Cliente? [1 - Sim, 2 - Nao]");
            scanf("%d", &exclui);
            if (exclui == 1) {
                continue;
            }
        }
        fprintf(temp, "%s;%s;%s;%s;%d;%s;%s;%s\n",
                cliente.cpf,
                cliente.nome,
                cliente.telefone,
                cliente.endereco.rua,
                cliente.endereco.numero,
                cliente.endereco.cidade,
                cliente.endereco.estado,
                cliente.dataNascimento);
    }

    fclose(arquivo);
    fclose(temp);

    // Substitui o arquivo original pelo temporário, caso algum agendamento tenha sido excluído
    if (encontrado) {
        remove("clientes.txt");
        rename("temp.txt", "clientes.txt");
        printf("Cliente Excluido com sucesso.\n");
    } else {
        remove("temp.txt");
        printf("Nenhum Cliente correspondente encontrado.\n");
    }
}

void listarClientes() {
    FILE *arquivo = fopen("clientes.txt", "r"); // lendo o arquivo de clientes
    if (!arquivo) {
        printf("Arquivo nao encontrado!\n");
        return;
    }

    printf("\n============== LISTA DE CLIENTES ===============  \n");

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) printf("CPF: %s\n", token);
            else if (i == 2) printf("\tNome: %s\n", token);
            else if (i == 3) printf("\tTelefone: %s\n", token);
            else if (i == 4) printf("\tRua: %s\n", token);
            else if (i == 5) printf("\tNumero: %s\n", token);
            else if (i == 6) printf("\tCidade: %s\n", token);
            else if (i == 7) printf("\tEstado: %s\n", token);
            else if (i == 8) printf("\tData Nascimento: %s\n", token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }
    }

    printf("=================================================\n");
    fclose(arquivo);
}

// ###########  FUNCOES DE AGENDAMENTO #################
int agendamentoExiste(const char *data, const char *hora) {
    FILE *file = fopen("agendamentos.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo para leitura.\n");
        return 0;
    }

    char linha[200];
    while (fgets(linha, sizeof(linha), file) != NULL) {
        char *token;
        char dataAgendada[11];
        char horaAgendada[6];
        int i = 1;

        // Extrai os campos de data e hora do arquivo
        token = strtok(linha, ";"); // Assume que o primeiro campo é o cliente

        while (token != NULL) {
            if (i == 3) strcpy(dataAgendada, token);
            if (i == 4) strcpy(horaAgendada, token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }

        // Verifica se a data e a hora coincidem
        if (strcmp(dataAgendada, data) == 0 && strcmp(horaAgendada, hora) == 0) {
            fclose(file);
            return 1; // Agendamento encontrado para a mesma data e hora
        }
    }

    fclose(file);
    return 0; // Nenhum agendamento encontrado para a mesma data e hora
}

void inserirAgendamento() {
    FILE *arquivo = fopen("agendamentos.txt", "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo para escrita.\n");
        return;
    }

    struct Agendamento agendamento;

    printf("Digite o CPF do cliente: ");
    scanf("%s", agendamento.cliente.cpf);

    if (!clienteExiste(agendamento.cliente.cpf)) {
        printf("Cliete nao cadastrado, processo cancelado!\n");
        return;
    }

    printf("Digite o codigo do servico: ");
    scanf("%d", &agendamento.servico.codigo);

    if (!servicoExiste(agendamento.servico.codigo)) {
        printf("Servico nao cadastrado, processo cancelado!\n");
        return;
    }

    printf("Digite a data (dd/mm/yyyy): ");
    scanf("%s", agendamento.data);
    if (!validarData(agendamento.data)) {
        printf("A data invalida.\n");
        return;
    }

    printf("Digite a hora (hh:mm): ");
    scanf("%s", agendamento.hora);
    if (!validarHora(agendamento.hora)) {
        printf("A hora valida.\n");
        return;
    }

    // verificando se existe alguem agendado pro msm dia e horario
    if (agendamentoExiste(agendamento.data, agendamento.hora)) {
        printf("Ja existe agendamento para o mesmo dia e horario, processo cancelado!\n");
        return;
    }

    // Escreve o agendamento no arquivo
    fprintf(arquivo, "%s;%d;%s;%s\n", agendamento.cliente.cpf, agendamento.servico.codigo, agendamento.data, agendamento.hora);
    fclose(arquivo);

    printf("Agendamento adicionado com sucesso!\n");
}

void listarAgendamento() {
    FILE *arquivo = fopen("agendamentos.txt", "r"); // lendo o arquivo de clientes
    if (!arquivo) {
        printf("Arquivo nao encontrado!\n");
        return;
    }

    printf("\n============== LISTA DE AGENDAMENTOS ===============  \n");

    char linha[200];
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) printf("CPF: %s\n", token);
            else if (i == 2) printf("\tServico: %s\n", token);
            else if (i == 3) printf("\tdata: %s\n", token);
            else if (i == 4) printf("\thora: %s\n", token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }
    }

    printf("=================================================\n");
    fclose(arquivo);
}

void excluirAgendamento() {
    struct Agendamento agendamento;
    struct Agendamento agendamentoTemp;
    int exclui;

    FILE *arquivo = fopen("agendamentos.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if (arquivo == NULL || temp == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        fclose(arquivo);
        return;
    }

    printf("Digite o CPF do cliente: ");
    scanf("%s", agendamento.cliente.cpf);

    if (!clienteExiste(agendamento.cliente.cpf)) {
        printf("Cliente nao cadastrado, processo cancelado!\n");
        return;
    }

    printf("Digite o codigo do servico: ");
    scanf("%d", &agendamento.servico.codigo);

    if (!servicoExiste(agendamento.servico.codigo)) {
        printf("Servico nao cadastrado, processo cancelado!\n");
        return;
    }

    printf("Digite a data (dd/mm/yyyy): ");
    scanf("%s", agendamento.data);

    printf("Digite a hora (hh:mm): ");
    scanf("%s", agendamento.hora);

    char linha[200];
    int encontrado = 0;
    while (fgets(linha, sizeof(linha), arquivo) != NULL) {
        struct Cliente cliente;
        char *token;
        int i = 1; // Inicializa o contador de campos

        // Remove o caractere de nova linha, se existir
        linha[strcspn(linha, "\n")] = 0;

        // Extrai o primeiro token
        token = strtok(linha, ";");

        // Enquanto houver tokens, processa-os
        while (token != NULL) {
            if (i == 1) strcpy(agendamentoTemp.cliente.cpf, token);
            else if (i == 2) agendamentoTemp.servico.codigo = atoi(token);
            else if (i == 3) strcpy(agendamentoTemp.data, token);
            else if (i == 4) strcpy(agendamentoTemp.hora, token);

            // Processa o próximo token
            token = strtok(NULL, ";");
            i++;
        }

        // Verifica se este registro corresponde aos critérios de exclusão
        if (strcmp(agendamento.cliente.cpf, agendamentoTemp.cliente.cpf) == 0 &&
            (agendamento.servico.codigo == agendamentoTemp.servico.codigo) &&
            strcmp(agendamento.data, agendamentoTemp.data) == 0 &&
            strcmp(agendamento.hora, agendamentoTemp.hora) == 0) {
            encontrado = 1;

            printf("Confirma a Exclusao do Agendamento? [1 - Sim, 2 - Nao]");
            scanf("%d", &exclui);
            if (exclui == 1) {
                continue;
            }
        }
        fprintf(temp, "%s;%d;%s;%s\n",
                agendamentoTemp.cliente.cpf,
                agendamentoTemp.servico.codigo,
                agendamentoTemp.data,
                agendamentoTemp.hora);
    }

    fclose(arquivo);
    fclose(temp);

    // Substitui o arquivo original pelo temporário, caso algum agendamento tenha sido excluído
    if (encontrado) {
        remove("agendamentos.txt");
        rename("temp.txt", "agendamentos.txt");
        printf("Agendamento Excluido com Sucesso.\n");
    } else {
        remove("temp.txt");
        printf("Nenhum agendamento correspondente encontrado.\n");
    }
}

// ##################################################

int main() {
    int opcaoPrincipal, opcaoCliente, opcaoServico, opcaoAgendamento;

    verificaSeExisteArquivos();

    do {
        printf("\n##### SISTEMA DE GERENCIAMETO DE PILATES #####\n");
        printf("############### MENU PRINCIPAL ##############\n");
        printf("1. Cliente\n");
        printf("2. Servico\n");
        printf("3. Agendamento\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcaoPrincipal);

        switch (opcaoPrincipal) {
            case 1:
                do {
                    printf("\n############# MENU CLIENTES ###############\n");
                    printf("1. Inserir Cliente\n");
                    printf("2. Excluir Cliente\n");
                    printf("3. Listar Clientes\n");
                    printf("4. Voltar ao Menu Principal\n");
                    printf("Escolha uma opcao: ");
                    scanf("%d", &opcaoCliente);

                    switch (opcaoCliente) {
                        case 1:
                            inserirCliente();
                            break;
                        case 2:
                            excluirCliente();
                            break;
                        case 3:
                            listarClientes();
                            break;
                        case 4:
                            printf("Voltando ao menu principal...\n");
                            break;
                        default:
                            printf("Opcao invalida! Tente novamente.\n");
                    }
                } while (opcaoCliente != 4);
                break;

            case 2:
                do {
                    printf("\n############# MENU SERVICOS ###############\n");
                    printf("1. Inserir Servico\n");
                    printf("2. Excluir Servico\n");
                    printf("3. Listar Servicos\n");
                    printf("4. Voltar ao Menu Principal\n");
                    printf("Escolha uma opcao: ");
                    scanf("%d", &opcaoServico);

                    switch (opcaoServico) {
                        case 1:
                            inserirServico();
                            break;
                        case 2:
                            excluirServico();
                            break;
                        case 3:
                            listarServicos();
                            break;
                        case 4:
                            printf("Voltando ao menu principal...\n");
                            break;
                        default: printf("Opcao invalida. Tente novamente.\n");
                    }
                } while (opcaoServico != 4);
                break;

            case 3:
                do {
                    printf("\n############## AGENDAMENTO ################\n");
                    printf("1. Inserir Agendamento\n");
                    printf("2. Excluir Agendamento\n");
                    printf("3. Listar Agendamento\n");
                    printf("4. Voltar ao Menu Principal\n");
                    printf("Escolha uma opcao: ");
                    scanf("%d", &opcaoAgendamento);

                    switch (opcaoAgendamento) {
                        case 1:
                            inserirAgendamento();
                            break;
                        case 2:
                            excluirAgendamento();
                            break;
                        case 3:
                            listarAgendamento();
                            break;
                        case 4:
                            printf("Voltando ao menu principal...\n");
                            break;
                        default: printf("Opcao invalida. Tente novamente.\n");
                    }
                } while (opcaoAgendamento != 4);
                break;

            case 4:
                printf("Saindo do sistema...\n");
                break;

            default:
                printf("Opcao inválida! Tente novamente.\n");
        }
    } while (opcaoPrincipal != 4);

    return 0;
}
