#include <stdio.h>
#include <string.h>
#include <stdlib.h>


typedef struct{
    int **M;
    char comando[10];
    int numero;
    int linha;
    int coluna;
}Jogo;

void alocaMatriz(Jogo *m);
void leMatriz(Jogo *m, FILE *arquivo);
void liberaMatriz(Jogo *m);
void imprimeJogo(Jogo *m);
void comandos (Jogo *m, char *arqvTexto);
int confereSeAcabou(Jogo *m);
int tipoDoComando(Jogo *m);
int verificaRegiao(Jogo *m);
int existeNaLinha(Jogo *m);
int existeNaColuna(Jogo *m);
int existeNaRegiao(Jogo *m, int x);

int main(){
    Jogo jogo;
    printf("Bem vindo ao SUDOKU!\n");

    char nomeArquivo[20];

    printf("Digite o nome do arquivo texto do jogo: ");
    fgets(nomeArquivo, 20, stdin);
    nomeArquivo[strlen(nomeArquivo)-1]  = '\0'; //tirando o \n do nome do arquivo
    
    FILE *arquivo = fopen(nomeArquivo, "r"); //abrindo o arquivo
    
    if(arquivo == NULL){ 
        printf("Erro ao abrir o arquivo\n");
        return 1;
    } //caso nao exista o arquivo com nome desejado o programa detecta erro e encerra

    alocaMatriz(&jogo); //alocando a matriz da struct dinamicamente
    leMatriz(&jogo, arquivo);

    fclose(arquivo);

    imprimeJogo(&jogo);
    comandos(&jogo, nomeArquivo);
    liberaMatriz(&jogo);
    printf("Obrigado por jogar SUDOKU!\n");

    return 0;
}

void alocaMatriz(Jogo *m){
    m->M = malloc(9 * sizeof(int *));
    for (int i = 0; i < 9; i++){
        m->M[i] = malloc(9 * sizeof(int));
    }
}

void leMatriz(Jogo *m, FILE *arquivo){
    char c[9];
    int i = 0;
    while(!feof(arquivo)){
        fscanf(arquivo, "%s", c);
        for(int j = 0; j < 9; j++){
            m->M[i][j] = c[j] - '0'; //transformando cada caracter da string do arquivo em inteiro e armazenando na matriz
        }
        i++;
    }//enquanto o arquivo nao acabar ele repete o processo para pegar todos os dados e armazenar na matriz
}

void liberaMatriz(Jogo *m){
    for (int i = 0; i < 9; i++)
    {
        free(m->M[i]);
    }
    free(m->M);
}//libera a matriz alocada dinamicamente

void imprimeJogo(Jogo *m){
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            if (m->M[i][j] == 0) printf(" _"); //caso o numero da matriz for 0, ele imprime _ no lugar
            else printf(" %d", m->M[i][j]); //caso contrario ele imprime o proprio numero
            if (j == 2 || j == 5) printf("  "); // da espacos entre as colunas das submatrizes
        }
        if (i == 2 || i == 5) printf("\n\n"); //da espaco entre as linhas das submatrizes
        else printf("\n"); //pula linha quando termina uma linha
    }
}

void comandos (Jogo *m, char *arqvTexto){
    printf("Digite um comando ou indique a celula a alterar: ");
    fgets(m->comando, 10, stdin);
    m->comando[strlen(m->comando) - 1] = '\0'; //tira o \n da string
    int comando = tipoDoComando(m);
    int acabou = confereSeAcabou(m);
    
    if(comando != 2){ //se comando for diferente de sair
        do{
            if (comando == 1){ //se o comando for salvar
                FILE *arquivo = fopen(arqvTexto, "w");
                
                for (int i = 0; i < 9; i++){
                    for (int j = 0; j < 9; j++){
                        fprintf(arquivo, "%d", m->M[i][j]);
                    }
                    if(i < 8) fprintf(arquivo, "\n");
                }
                fclose(arquivo);
                printf("Jogo foi salvo em %s\n", arqvTexto);
            }
            else if(comando == 3){ //se o comando indica a linha, coluna e numero propriamente
                m->linha = m->comando[0] - '0'; //transforma a linha digitada em int
                m->linha = m->linha - 1; //subtrai 1 do numero inteiro pois eh como sera utilizado para verificar a matriz
                m->coluna = m->comando[1] - '0'; //transforma a coluna digitada em int
                m->coluna = m->coluna - 1; //subtrai 1 do numero inteiro pois eh como sera utilizado para verificar a matriz
                m->numero = m->comando[2] - '0';

                int regiao = verificaRegiao(m); //qual regiao a matriz esta
                int condicaoLinha = existeNaLinha(m); //retorna se existe o numero digitado na linha
                int condicaoColuna = existeNaColuna(m);//retorna se existe o numero digitado na coluna
                int condicaoRegiao = existeNaRegiao(m, regiao); //retorna se existe o numero digitado na regiao/submatriz
                
                if(m->M[m->linha][m->coluna] == 0){
                    if (condicaoRegiao && condicaoLinha && condicaoColuna){
                        printf("Erro! A regiao %d, a linha %d e a coluna %d ja tem o valor %d\n", regiao, m->linha+1, m->coluna+1, m->numero);
                    }
                    else if(condicaoRegiao && condicaoLinha)
                        printf("Erro! A regiao %d e a linha %d ja tem o valor %d\n", regiao, m->linha+1, m->numero);
                    else if(condicaoRegiao && condicaoColuna){
                        printf("Erro! A regiao %d e a coluna %d ja tem o valor %d\n", regiao, m->coluna+1, m->numero);
                    }
                    else if(condicaoLinha && condicaoColuna){
                        printf("Erro! A linha %d e a coluna %d ja tem o valor %d\n", m->linha+1, m->coluna+1, m->numero);
                    }
                    else if(condicaoRegiao){ 
                        printf("Erro! A regiao %d ja tem o valor %d\n", regiao, m->numero);
                    }
                    else if(condicaoLinha){
                        printf("Erro! A linha numero %d ja possui o valor %d.\n", m->linha+1, m->numero);
                    }
                    else if (condicaoColuna){
                        printf("Erro! A coluna numero %d ja possui o valor %d.\n", m->coluna+1, m->numero);
                    }
                    else{
                        m->M[m->linha][m->coluna] = m->numero;
                        imprimeJogo(m);
                    } //caso nao tenha nenhum problema a matriz recebe o numero digitado na posicao digitada e ela eh imprimida
                }
                else {
                    printf("A cedula %d,%d ja possui valor\n", m->linha+1, m->coluna+1);
                }  
            }
            else{
                printf("Comando Invalido!\n");
            }
            acabou = confereSeAcabou(m);
            if(acabou == 0){
                printf("PARABENS, VOCE CONSEGUIU!\n");
                break;
            }
            printf("Digite um comando ou indique a celula a alterar: ");
            fgets(m->comando, 10, stdin);
            m->comando[strlen(m->comando) - 1] = '\0';
            comando = tipoDoComando(m);
            acabou = confereSeAcabou(m);
        }while(comando != 2); //repetir enquanto o comando nao for sair
    }
}

int tipoDoComando(Jogo *m){
    if(strcmp(m->comando, "Salvar") == 0 || strcmp(m->comando, "salvar") == 0 || strcmp(m->comando, "SALVAR") == 0) return 1;
    else if(strcmp(m->comando, "Sair") == 0 || strcmp(m->comando, "sair") == 0 || strcmp(m->comando, "SAIR") == 0) return 2;
    else if (m->comando[0] > 48 && m->comando[0] < 58 && m->comando[1] > 48 && m->comando[1] < 58 && m->comando[2] > 48 && m->comando[2] < 58 && m->comando[3] == '\0') return 3; 
    return 0;
}

int confereSeAcabou(Jogo *m){
    int cont = 0;
    for(int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            if(m->M[i][j] == 0) cont++;
        }
    }
    return cont; //se o contador for maior que 0 os espacos nao foram completados
}

int verificaRegiao(Jogo *m){
    if(m->linha <= 2 && m->coluna <= 2) return 1;
    else if(m->linha <= 2 && m->coluna <= 5) return 2;
    else if(m->linha <= 2 && m->coluna <= 8) return 3;
    else if(m->linha <= 5 && m->coluna <= 2) return 4;
    else if(m->linha <= 5 && m->coluna <= 5) return 5;
    else if(m->linha <= 5 && m->coluna <= 8) return 6;
    else if(m->linha <= 8 && m->coluna <= 2) return 7;
    else if(m->linha <= 8 && m->coluna <= 5) return 8;
    return 9; //se a funcao nao entrar em nenhum dos if's, a regiao eh a 9
}// a funcao retorna a regiao do respectivo comando digitado

int existeNaLinha(Jogo *m){
    int cont = 0;
    for (int i = 0; i < 9; i++){
        if(m->numero == m->M[m->linha][i]) cont++;
    }
    return cont;
}// a funcao retorna o contador, pq se ele for maior que 0, nao eh possivel colocar na posicao desejada

int existeNaColuna(Jogo *m){
    int cont = 0;
    for (int i = 0; i < 9; i++){
        if(m->numero == m->M[i][m->coluna]) cont++;
    }
    return cont;
}// a funcao retorna o contador, pq se ele for maior que 0, nao eh possivel colocar na posicao desejada

int existeNaRegiao(Jogo *m, int x){
    int cont = 0;
    
    if(x == 1) { //se ta na submatriz 1    
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 2) { //se ta na submatriz 2
         for (int i = 0; i < 3; i++)
        {
            for (int j = 3; j < 6; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 3) { //se ta na submatriz 3
         for (int i = 0; i < 3; i++)
        {
            for (int j = 6; j < 9; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 4) { //se ta na submatriz 4
         for (int i = 3; i < 6; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 5) { //se ta na submatriz 5
        for (int i = 3; i < 6; i++)
        {
            for (int j = 3; j < 6; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 6) { //se ta na submatriz 6
        for (int i = 3; i < 6; i++)
        {
            for (int j = 6; j < 9; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 7) {  //se ta na submatriz 7
        for (int i = 6; i < 9; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else if(x == 8) { //se ta na submatriz 8
        for (int i = 6; i < 9; i++)
        {
            for (int j = 3; j < 6; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }
    else{ //se ta na 9
        for (int i = 6; i < 9; i++)
        {
            for (int j = 6; j < 9; j++)
            {
                if(m->M[i][j] == m->numero) cont++;
            }
        }
    }

    return cont;
}// a funcao retorna o contador, pq se ele for maior que 0, nao eh possivel colocar na posicao desejada