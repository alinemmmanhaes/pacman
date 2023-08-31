#include <stdio.h>

typedef struct{
    int l; //linha
    int c; //coluna
    int iant; //linha da posição da jogada anterior
    int jant; //coluna da posição da jogada anterior
    int dir; //guarda 1 se estiver feito indo para a direita e 0 caso va para a esquerda
    int cima; //guarda 1 se estiver feito indo para cima e 0 caso va para baixo
    int existe; //guarda 1 se ele existir e 0 caso nao
    int comida; //guarda 1 se estiver em uma posição que tinha uma comida e 0 caso nao
    int especial; //EXTRA 01: guarda 1 se estiver em uma posição que tenha uma comida especial e 0 caso nao
    int alternativo; //EXTRA 01: guarda 1 se estiver em uma posição que tenha uma caractere alternativo e 0 caso nao
} tFantasma;

typedef struct{
    int posl; //linha
    int posc; //coluna
    int ultima; //guarda 1 se tiver feito ponto na ultima rodada e 0 caso nao
} tJogador;

typedef struct{
    int x; //x = numero de vezes que o movimento "comando" ocorreu
    int cx; //cx = numero de vezes que o movimento "comando" pegou comida
    int px; //px = numero de vezes que o movimento "comando" fez o pacman bater na parede
    char comando; //tipo do movimento
} tEstatistica;

typedef struct{
    int l1;
    int c1;
    int l2;
    int c2;
} tTunel; //l1 e c1: posicao da 1a parte do tunel/ l2 e c2: posicao da 2a parte do tunel

typedef struct{
    char mapa[40][100];
    int trilha[40][100];
    int linhas;
    int colunas;
    int jogadas;
    tJogador jogador;
    tEstatistica ranking[4];
    tFantasma B;
    tFantasma P;
    tFantasma I;
    tFantasma C;
    tTunel tunel;
    int pontos;
    int comida;
    int comespecial;
    int parede;
    int njogadas;
    int temtunel;
    int alternativo;
} tJogo; //guarda infos do jogo

//Le os parametros do mapa, jogadas max. e o mapa
tJogo LeMapa(FILE * pMapa, tJogo jogo){
    int i, j;
    
    fscanf(pMapa, "%d %d %d\n", &jogo.linhas, &jogo.colunas, &jogo.jogadas);
    
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            fscanf(pMapa, "%c", &jogo.mapa[i][j]);
        }
        fscanf(pMapa, "\n");
    }
    
    return jogo;
}

//Atualiza as posicoes do jogador '>' e zera 'ultima'
tJogador AtualizaJogador(int l, int c, tJogador jogador){
    jogador.posl = l;
    jogador.posc = c;
    jogador.ultima = 0;
    return jogador;
}

//Grava a posicao do fantasma no mapa e suas direcoes (direita e cima) como 0 ou 1
tFantasma IniciaFantasma(tFantasma fantasma, int i, int j, char nome){
    fantasma.existe = 1;
    fantasma.comida = 0;
    fantasma.especial = 0;
    fantasma.alternativo = 0;
    fantasma.l = i;
    fantasma.c = j;
    
    if(nome == 'B' || nome == 'I'){
        fantasma.dir = 0;
        fantasma.cima = 0;
    }
    if(nome == 'C'){
        fantasma.dir = 1;
        fantasma.cima = 0;
    }
    if(nome == 'P'){
        fantasma.dir = 0;
        fantasma.cima = 1;
    }
    return fantasma;
}

tFantasma ZeraFantasma(tFantasma fant){
    fant.existe = 0;
    return fant;
}

void ZeraRanking(tEstatistica * r){
    int i;
    for(i=0; i<4; i++){
        if(i==0){
            r[i].comando = 'a';
        }
        else if(i==1){
            r[i].comando = 'd';
        }
        else if(i==2){
            r[i].comando = 'w';
        }
        else if(i==3){
            r[i].comando = 's';
        }
        r[i].x = 0;
        r[i].cx = 0;
        r[i].px = 0;
    }
}

//Inicia todas as posicoes da trilha como -1
tJogo IniciaTrilha(tJogo jogo, int x, int y){
    int i, j;
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            jogo.trilha[i][j] = -1;
        }
    }
    jogo.trilha[x][y] = 0;
    return jogo;
}

tTunel ZeraTunel(tTunel tunel){
    tunel.l1 = -1;
    tunel.c1 = -1;
    tunel.l2 = -1;
    tunel.c2 = -1;
    return tunel;
}

//Guarda a posição do '@' (tunel)
tTunel IniciaTunel(tTunel tunel, int i, int j){
    if(tunel.l1 == -1){
    	tunel.l1 = i;
    	tunel.c1 = j;
    }else if(tunel.l1 != -1){
    	tunel.l2 = i;
    	tunel.c2 = j;
    }
    return tunel;
}

//Localiza itens do jogo e zera itens necessarios para o inicio
tJogo Inicializa(tJogo jogo){
    int i, j, posl, posc;

    //Zera as variáveis para inicio do jogo
    jogo.B = ZeraFantasma(jogo.B);
    jogo.P = ZeraFantasma(jogo.P);
    jogo.I = ZeraFantasma(jogo.I);
    jogo.C = ZeraFantasma(jogo.C);
    jogo.comida = 0;
    jogo.pontos = 0;
    jogo.comespecial = 0;
    jogo.parede = 0;
    jogo.temtunel = 0;
    jogo.alternativo = 0;
    ZeraRanking(jogo.ranking);
    jogo.tunel = ZeraTunel(jogo.tunel);
    
    //Localiza jogador e fantasmas, inicia fantasmas que existem e contabiliza comidas
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            if(jogo.mapa[i][j] == '>'){
                posl = i;
                posc = j;
                jogo.jogador = AtualizaJogador(posl, posc, jogo.jogador);
            }
            if(jogo.mapa[i][j]=='*'){
                jogo.comida++;
            }
            if(jogo.mapa[i][j]=='B'){
                jogo.B = IniciaFantasma(jogo.B, i, j, jogo.mapa[i][j]);
            }
            if(jogo.mapa[i][j]=='P'){
                jogo.P = IniciaFantasma(jogo.P, i, j, jogo.mapa[i][j]);
            }
            if(jogo.mapa[i][j]=='I'){
                jogo.I = IniciaFantasma(jogo.I, i, j, jogo.mapa[i][j]);
            }
            if(jogo.mapa[i][j]=='C'){
                jogo.C = IniciaFantasma(jogo.C, i, j, jogo.mapa[i][j]);
            }
            if(jogo.mapa[i][j]=='@'){
                jogo.tunel = IniciaTunel(jogo.tunel, i, j);
            }
        }
    }
    jogo = IniciaTrilha(jogo, posl, posc);
    return jogo;
}

//Analisa se o fantasma ou jogador estah na ponta (ao lado da parede)
int EstaNaPonta(int i, int j, tJogo jogo, int c){    
    if(c == 1){
        if(jogo.mapa[i][j-1]=='#'){
            return 1;
        }
    }
    if(c == 2){
        if(jogo.mapa[i][j+1]=='#'){
            return 1;
        }
    }
    if(c == 3){
        if(jogo.mapa[i-1][j]=='#'){
            return 1;
        }
    }
    if(c == 4){
        if(jogo.mapa[i+1][j]=='#'){
            return 1;
        }
    }
    return 0;
}

int JogadorNaPonta(tJogador jogador, tJogo jogo, char c){
    int resp, direcao;
    if(c == 'a'){
        direcao = 1;
    }
    if(c == 'd'){
        direcao = 2;
    }
    if(c == 'w'){
        direcao = 3;
    }
    if(c == 's'){
        direcao = 4;
    }
    resp = EstaNaPonta(jogador.posl, jogador.posc, jogo, direcao);

    return resp;
}

//Move o jogador de acordo com o comando recebido
tJogador MexeJogador(tJogador jog, char c){
    if(c=='a'){
        jog.posc--;
    }
    else if(c=='d'){
        jog.posc++;
    }
    else if(c=='w'){
        jog.posl--;
    }
    else if(c=='s'){
        jog.posl++;
    }
    return jog;
}

//Analisa se o jogador não batera em parede e o move, se for o caso
tJogador AnalisaJogada(tJogador jogador, tJogo jogo, char c){
    int resp;
    
    resp = JogadorNaPonta(jogador, jogo, c);
    if(resp==0){
        jogador = MexeJogador(jogador, c);
    }
    
    return jogador;
}

//Verifica se o fantasma esta na ponta. Caso esteja, altera o seu sentido e o move. Caso não esteja, só o move
tFantasma AndaFantasmaH(tFantasma f, tJogo jogo){ //Para fantasmas que se movem na horizontal
    if(f.existe){
        f.iant = f.l;
        f.jant = f.c;
        if(f.dir==0 && (EstaNaPonta(f.l, f.c, jogo, 1)==0)){
            f.c--;
        }
        else if(f.dir==0 && (EstaNaPonta(f.l, f.c, jogo, 1)==1)){
            f.c++;
            f.dir = 1;
        }
        else if(f.dir==1 && (EstaNaPonta(f.l, f.c, jogo, 2)==0)){
            f.c++;
        }
        else if(f.dir==1 && (EstaNaPonta(f.l, f.c, jogo, 2)==1)){
            f.c--;
            f.dir = 0;
        }
    }
    return f;
}

//Verifica se o fantasma esta na ponta. Caso esteja, altera o seu sentido e o move. Caso não esteja, só o move
tFantasma AndaFantasmaV(tFantasma f, tJogo jogo){ //Para fantasmas que se movem na vertical
    if(f.existe){
        f.iant = f.l;
        f.jant = f.c;
        if(f.cima==0 && (EstaNaPonta(f.l, f.c, jogo, 4)==0)){
            f.l++;
        }
        else if(f.cima==0 && (EstaNaPonta(f.l, f.c, jogo, 4)==1)){
            f.l--;
            f.cima = 1;
        }
        else if(f.cima==1 && (EstaNaPonta(f.l, f.c, jogo, 3)==0)){
            f.l--;
        }
        else if(f.cima==1 && (EstaNaPonta(f.l, f.c, jogo, 3)==1)){
            f.l++;
            f.cima = 0;
        }
    }
    return f;
}

//Verifica se o fantasma f estava em cima de uma posicao de comida na ultima rodada
int VerificaComida(tFantasma f){
    if(f.comida == 1){
        return 1;
    }
    return 0;
}

//Altera o valor da variavel comida para seu complemento
tFantasma TiraComida(tFantasma f){
    if(f.comida == 1){
        f.comida = 0;
    }
    else if(f.comida == 0){
        f.comida = 1;
    }
    return f;
}

//Jogador fez ponto na rodada
tJogador AtualizaUltima(tJogador jogador){
    jogador.ultima = 1;
    return jogador;
}

//Zera a variavel "ultima" para iniciar outra rodada
tJogador ZeraUltima(tJogador jogador){
    jogador.ultima = 0;
    return jogador;
}

//Confere em qual lado do tunel o jogador estah e o leva para o outro lado, alterando sua posicao
tJogador EntrouTunel(tTunel tunel, tJogador jogador){
    if(jogador.posl == tunel.l1 && jogador.posc == tunel.c1){
    	jogador.posl = tunel.l2;
    	jogador.posc = tunel.c2;
    }
    else if(jogador.posl == tunel.l2 && jogador.posc == tunel.c2){
        jogador.posl = tunel.l1;
    	jogador.posc = tunel.c1;
    }
    return jogador;
}

//EXTRA 01: Verifica se o fantasma f estava em cima de uma posicao de comida especial na ultima rodada
int VerificaEspecial(tFantasma f){
    if(f.especial == 1){
        return 1;
    }
    return 0;
}

//EXTRA 01: Altera o valor da variavel de comida especial para seu complemento
tFantasma TiraEspecial(tFantasma f){
    if(f.especial == 1){
        f.especial = 0;
    }
    else if(f.especial == 0){
        f.especial = 1;
    }
    return f;
}

//EXTRA 02: Verifica se o fantasma f estava em cima de uma posicao de caractere alternativo na ultima rodada
int VerificaAlternativo(tFantasma f){
    if(f.alternativo == 1){
        return 1;
    }
    return 0;
}

//EXTRA 02: Altera o valor da variavel de caractere alternativo para seu complemento
tFantasma TiraAlternativo(tFantasma f){
    if(f.alternativo == 1){
        f.alternativo = 0;
    }
    else if(f.alternativo == 0){
        f.alternativo = 1;
    }
    return f;
}

//Atualiza as posicoes do mapa
tJogo AtualizaMapa(tJogo jogo, tJogador jogador, tFantasma B, tFantasma C, tFantasma I, tFantasma P){
    int i, j;
    
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            if(jogo.mapa[i][j] == 'B'){
                if(VerificaComida(jogo.B)){
                    jogo.mapa[i][j] = '*';
                    jogo.B = TiraComida(jogo.B);
                }
                else if(VerificaEspecial(jogo.B)){
                    jogo.mapa[i][j] = '%';
                    jogo.B = TiraEspecial(jogo.B);
                }
                else if(VerificaAlternativo(jogo.B)){
                    jogo.mapa[i][j] = '!';
                    jogo.B = TiraAlternativo(jogo.B);
                }
                else{ //Atualiza a posicao antiga do fantasma para um espaco, uma vez que ele se moveu
                    jogo.mapa[i][j] = ' ';
                }
            }
            else if(jogo.mapa[i][j] == 'C'){
                if(VerificaComida(jogo.C)){
                    jogo.mapa[i][j] = '*';
                    jogo.C = TiraComida(jogo.C);
                }
                else if(VerificaEspecial(jogo.C)){
                    jogo.mapa[i][j] = '%';
                    jogo.C = TiraEspecial(jogo.C);
                }
                else if(VerificaAlternativo(jogo.C)){
                    jogo.mapa[i][j] = '!';
                    jogo.C = TiraAlternativo(jogo.C);
                }
                else{
                    jogo.mapa[i][j] = ' ';
                }
            }
            else if(jogo.mapa[i][j] == 'I'){
                if(VerificaComida(jogo.I)){
                    jogo.mapa[i][j] = '*';
                    jogo.I = TiraComida(jogo.I);
                }
                else if(VerificaEspecial(jogo.I)){
                    jogo.mapa[i][j] = '%';
                    jogo.I = TiraEspecial(jogo.I);
                }
                else if(VerificaAlternativo(jogo.I)){
                    jogo.mapa[i][j] = '!';
                    jogo.I = TiraAlternativo(jogo.I);
                }
                else{
                    jogo.mapa[i][j] = ' ';
                }
            }
            else if(jogo.mapa[i][j] == 'P'){
                if(VerificaComida(jogo.P)){
                    jogo.mapa[i][j] = '*';
                    jogo.P = TiraComida(jogo.P);
                }
                else if(VerificaEspecial(jogo.P)){
                    jogo.mapa[i][j] = '%';
                    jogo.P = TiraEspecial(jogo.P);
                }
                else if(VerificaAlternativo(jogo.P)){
                    jogo.mapa[i][j] = '!';
                    jogo.P = TiraAlternativo(jogo.P);
                }
                else{
                    jogo.mapa[i][j] = ' ';
                }
            }
            else if(jogo.mapa[i][j] == '>' || jogo.mapa[i][j] == '&'){
                if(jogo.temtunel){ //Ve se o jogador estava em cima de um tunel na ultima rodada. 
                    //Caso sim, altera a posicao para '@' (simbolo de tunel)
                    jogo.mapa[i][j] = '@'; 
                    jogo.temtunel = 0;
                }else{
                    jogo.mapa[i][j] = ' ';
                }
            }
        }
    }

    if(jogo.mapa[jogador.posl][jogador.posc] == '*'){
        jogo.pontos++; //Jogador fez ponto
        jogo.jogador = AtualizaUltima(jogo.jogador); //
    }
    else if(jogo.mapa[jogador.posl][jogador.posc] == '%'){
        jogo.comespecial += 5; //EXTRA 01: Jogador fez ponto especial
    }
    else if(jogo.mapa[jogador.posl][jogador.posc] == '!'){
        jogo.alternativo = 1; //EXTRA 02: Jogador entrou no modo alternativo
    }
    else if(jogo.mapa[jogador.posl][jogador.posc] == '@'){
        jogo.temtunel = 1; //Jogador em cima de um tunel
        jogo.jogador = EntrouTunel(jogo.tunel, jogo.jogador);
    }
    jogador = jogo.jogador;
    if(jogo.alternativo>=1 && jogo.alternativo<=15){
    	jogo.mapa[jogador.posl][jogador.posc] = '&'; //EXTRA 02: Coloca o jogador alternativo no mapa
    	jogo.alternativo++;
    }
    else{
    	jogo.alternativo = 0;
    	jogo.mapa[jogador.posl][jogador.posc] = '>'; //Coloca o jogador no mapa
    }
    
    if(B.existe){
        if(jogo.mapa[B.l][B.c] == '*'){
            jogo.B = TiraComida(jogo.B); //Fantasma estah em cima de uma comida
        }
        else if(jogo.mapa[B.l][B.c] == '%'){
            jogo.B = TiraEspecial(jogo.B); //EXTRA 01: Fantasma estah em cima de uma comida especial
        }
        else if(jogo.mapa[B.l][B.c] == '!'){
            jogo.B = TiraAlternativo(jogo.B); //EXTRA 02: Fantasma em cima de um caractere alternativo
        }
        jogo.mapa[B.l][B.c] = 'B'; //Coloca o fantasma no mapa
    }
    
    if(C.existe){
        if(jogo.mapa[C.l][C.c] == '*'){
            jogo.C = TiraComida(jogo.C);
        }
        else if(jogo.mapa[C.l][C.c] == '%'){
            jogo.C = TiraEspecial(jogo.C);
        }
        else if(jogo.mapa[C.l][C.c] == '!'){
            jogo.C = TiraAlternativo(jogo.C);
        }
        jogo.mapa[C.l][C.c] = 'C';
    }
    
    if(I.existe){
        if(jogo.mapa[I.l][I.c] == '*'){
            jogo.I = TiraComida(jogo.I);
        }
        else if(jogo.mapa[I.l][I.c] == '%'){
            jogo.I = TiraEspecial(jogo.I);
        }
        else if(jogo.mapa[I.l][I.c] == '!'){
            jogo.I = TiraAlternativo(jogo.I);
        }
        jogo.mapa[I.l][I.c] = 'I';
    }
    
    if(P.existe){
        if(jogo.mapa[P.l][P.c] == '*'){
            jogo.P = TiraComida(jogo.P);
        }
        else if(jogo.mapa[P.l][P.c] == '%'){
            jogo.P = TiraEspecial(jogo.P);
        }
        else if(jogo.mapa[P.l][P.c] == '!'){
            jogo.P = TiraAlternativo(jogo.P);
        }
        jogo.mapa[P.l][P.c] = 'P';
    }
    
    if(jogo.alternativo>=1 && jogo.alternativo<=16){
    	jogo.mapa[jogador.posl][jogador.posc] = '&'; //EXTRA 03: Coloca o jogador alternativo no mapa depois da analise dos fantasmas
    }

    return jogo;
}

//Analisa se o jogador morreu por um dos fantasmas
//Retorna 1 se as posicoes forem iguais
//Retorna 2 se eles se cruzarem
int AnalisaMorte(tJogador jogador, tFantasma f, int lant, int cant, tJogo jogo){
    // (lant,cant) = posicao anterior do jogador
    if(jogo.alternativo>=1 && jogo.alternativo<=16){
    	return 0; //EXTRA 03: jogador invencivel
    }
    if(f.existe){
        if(jogador.posc == f.c && jogador.posl == f.l){
            return 1;
        }else if(jogador.posc == f.jant && f.c == cant && jogador.posl == f.iant && f.l == lant){
            return 2;
        }
    }
    return 0;
}

//Imprime mapa e pontuacao atuais
void ImprimeEstadoAtual(int c, tJogo jogo){
    int i, j;
    
    printf("Estado do jogo apos o movimento '%c':\n", c);
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            printf("%c", jogo.mapa[i][j]);
        }
        printf("\n");
    }
    printf("Pontuacao: %d\n\n", jogo.pontos+jogo.comespecial);
}

//Printa a pontuacao final e se o jogador ganhou ou perdeu
void PrintaFim(int caso, tJogo jogo){
    if(caso == 2){
        printf("Voce venceu!\n");
    }
    else{
        printf("Game over!\n");
    }
    printf("Pontuacao final: %d\n", jogo.pontos+jogo.comespecial);
}

//Soma +1 no numero de vezes que o ultimo comando foi realizado
void ContaJogadas(char c, tEstatistica * jogo){
    if(c == 'a'){
        jogo[0].x++;
    }
    if(c == 's'){
        jogo[3].x++;
    }
    if(c == 'd'){
        jogo[1].x++;
    }
    if(c == 'w'){
        jogo[2].x++;
    }
}

//Soma +1 no numero de vezes que o ultimo comando bateu na parede
void ContaJogadasParede(char c, tEstatistica * jogo){
    if(c == 'a'){
        jogo[0].px++;
    }
    if(c == 's'){
        jogo[3].px++;
    }
    if(c == 'd'){
        jogo[1].px++;
    }
    if(c == 'w'){
        jogo[2].px++;
    }
}

//Soma +1 no numero de vezes que o ultimo comando somou pontos
void ContaJogadasComida(char c, tEstatistica * jogo){
    if(c == 'a'){
        jogo[0].cx++;
    }
    if(c == 's'){
        jogo[3].cx++;
    }
    if(c == 'd'){
        jogo[1].cx++;
    }
    if(c == 'w'){
        jogo[2].cx++;
    }
}

int ObtemLinhaJog(tJogador jogador){
    return jogador.posl;
}

int ObtemColunaJog(tJogador jogador){
    return jogador.posc;
}

//Retorna a variavel "ultima", que quando vale 1 significa que o jogador fez ponto na ultima rodada
int Ultima(tJogador jogador){
    return jogador.ultima;
}

//Coloca o numero da jogada n na posicao do jogador (x,y) da matriz
//Se for um tunel na posicao do jogador, coloca o numero da jogada n nas duas posicoes do tunel
tJogo AtualizaTrilha(tJogo jogo, int x, int y, int n, tTunel tunel){
    if(jogo.temtunel){
        jogo.trilha[tunel.l1][tunel.c1] = n;
        jogo.trilha[tunel.l2][tunel.c2] = n;
    }else{
    	jogo.trilha[x][y] = n;
    }
    return jogo;
}

//Remove jogador do mapa
tJogo TiraJogador(tJogo jogo, tJogador jogador){
    if(Ultima(jogador)){
    	jogo.mapa[jogador.posl][jogador.posc] = '*';
    	return jogo;
    }
    jogo.mapa[jogador.posl][jogador.posc] = ' ';
    return jogo;
}

//Realiza as ações do jogo e gera conteudo do arquivo resumo.txt
tJogo JogaJogo(tJogo jogo, FILE * resumo){
    int i, resp, caso=0, comida, x, y, z, w;
    char comando;
    for(i=1; i<=jogo.jogadas; i++){
        scanf("%c%*c", &comando);
        ContaJogadas(comando, jogo.ranking);
        comida = jogo.pontos; //quantidade de pontos antes da rodada
        x = ObtemLinhaJog(jogo.jogador); //(x,y) = posicao do jogador antes da movimentacao
        y = ObtemColunaJog(jogo.jogador);
        
        //Movimenta jogador e fantasmas
        jogo.jogador = AnalisaJogada(jogo.jogador, jogo, comando);
        jogo.B = AndaFantasmaH(jogo.B, jogo);
        jogo.C = AndaFantasmaH(jogo.C, jogo);
        jogo.P = AndaFantasmaV(jogo.P, jogo);
        jogo.I = AndaFantasmaV(jogo.I, jogo);
        
        jogo = AtualizaMapa(jogo, jogo.jogador, jogo.B, jogo.C, jogo.I, jogo.P);
        w = ObtemLinhaJog(jogo.jogador); //(w,z) = posicao do jogador depois da movimentacao
        z = ObtemColunaJog(jogo.jogador);
        
        //Analisa se o jogador morreu por algum dos fantasmas
        resp = AnalisaMorte(jogo.jogador, jogo.B, x, y, jogo);
        if(resp == 1 || resp == 2){
            caso = 1;
        }
        if(resp == 2){ //se a resposta for 2, fastasma e jogador se cruzaram
            jogo = TiraJogador(jogo, jogo.jogador); //jogador deve ser removido do mapa
        }
        resp = AnalisaMorte(jogo.jogador, jogo.C, x, y, jogo);
        if(resp == 1 || resp == 2){
            caso = 1;
        }
        if(resp == 2){
            jogo = TiraJogador(jogo, jogo.jogador);
        }
        resp = AnalisaMorte(jogo.jogador, jogo.P, x, y, jogo);
        if(resp == 1 || resp == 2){
            caso = 1;
        }
        if(resp == 2){
            jogo = TiraJogador(jogo, jogo.jogador);
        }
        resp = AnalisaMorte(jogo.jogador, jogo.I, x, y, jogo);
        if(resp == 1 || resp == 2){
            caso = 1;
        }
        if(resp == 2){
            jogo = TiraJogador(jogo, jogo.jogador);
        }
        
        if(Ultima(jogo.jogador) && caso){ //Se jogador morreu por fantasmas e somou pontos na ultima rodada,
            jogo.pontos--; //o ultimo ponto deve ser removido, nao pode ser contabilizado, ja que o ">" morreu
        }
        
        ImprimeEstadoAtual(comando, jogo); //Imprime mapa e pontuacao atuais
        if(comida != jogo.pontos){ //se o numero de pontos antes e depois da rodada for diferente
            fprintf(resumo, "Movimento %d (%c) pegou comida\n", i, comando);
            ContaJogadasComida(comando, jogo.ranking);
        }
        if(caso){ //Se morreu por fantasmas:
            fprintf(resumo, "Movimento %d (%c) fim de jogo por encostar em um fantasma\n", i, comando);
            if(x==w && y==z){ //posicoes antes e depois da rodada sao iguais = bateu na parede
                fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", i, comando);
                ContaJogadasParede(comando, jogo.ranking);
                jogo.parede++;
            }
            break;
        }
        if(x==w && y==z){ //posicoes antes e depois da rodada sao iguais = bateu na parede
            fprintf(resumo, "Movimento %d (%c) colidiu na parede\n", i, comando);
            ContaJogadasParede(comando, jogo.ranking);
            jogo.parede++;
        }
        jogo = AtualizaTrilha(jogo, w, z, i, jogo.tunel);
        
        if(jogo.pontos == jogo.comida){ //Se jogador ja pegou todas as comidas:
            caso = 2;
            break;
        }
        if(i == jogo.jogadas){ //Se excedeu o numero de jogadas maximas:
            caso = 3;
            break;
        }
        jogo.jogador = ZeraUltima(jogo.jogador); 
    }
    jogo.njogadas = i;
    PrintaFim(caso, jogo);

    return jogo;
}

//Gera conteudo do arquivo inicializa.txt
void PrintaInicializa(FILE * pInicializa, tJogo jogo){
    int i, j, x, y;
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            fprintf(pInicializa, "%c", jogo.mapa[i][j]);
            if(jogo.mapa[i][j] == '>'){
                x = i;
                y = j;
            }
        }
        fprintf(pInicializa, "\n");
    }
    
    fprintf(pInicializa, "Pac-Man comecara o jogo na linha %d e coluna %d\n", x+1, y+1);
}

//Gera conteudo do arquivo ranking.txt
void PrintaRanking(FILE * ranking, tEstatistica * r){
    tEstatistica m;
    int i, j, mi;
    for(i=0; i<4; i++){ //ordena os movimentos com base nos criterios dados
        m.x = r[i].x;
        m.cx = r[i].cx;
        m.px = r[i].px;
        m.comando = r[i].comando;
        mi = i;
        for(j=i+1; j<4; j++){
            if(r[j].cx > m.cx){ //quantidade de comida que o movimento pegou
                m.x = r[j].x;
                m.cx = r[j].cx;
                m.px = r[j].px;
                m.comando = r[j].comando;
                mi = j; //armazena o indice do melhor comando
            }
            else if(r[j].cx == m.cx){
                if(r[j].px < m.px){ //quantidade de vezes que o movimento bateu na parede
                    m.x = r[j].x;
                    m.cx = r[j].cx;
                    m.px = r[j].px;
                    m.comando = r[j].comando;
                    mi = j;
                }
                else if(r[j].px == m.px){
                    if(r[j].x > m.x){ //quantidade de vezes que o movimento foi realizado
                        m.x = r[j].x;
                        m.cx = r[j].cx;
                        m.px = r[j].px;
                        m.comando = r[j].comando;
                        mi = j;
                    }
                    else if(r[j].x == m.x){
                        if(r[j].comando < m.comando){ //analisa ordem alfabetica
                            m.x = r[j].x;
                            m.cx = r[j].cx;
                            m.px = r[j].px;
                            m.comando = r[j].comando;
                            mi = j;
                        }
                    }
                }
            }
        }
        fprintf(ranking, "%c,%d,%d,%d\n", m.comando, m.cx, m.px, m.x); //printa o melhor comando
        r[mi] = r[i]; //troca o melhor comando de posicao com o comando i (que foi analisado inicialmente)
        r[i] = m;
    }
}

//Gera conteudo do arquivo estatisticas.txt
void PrintaEstatistica(FILE * Estatistica, tJogo jogo, tEstatistica * r){
    fprintf(Estatistica, "Numero de movimentos: %d\n", jogo.njogadas);
    fprintf(Estatistica, "Numero de movimentos sem pontuar: %d\n", (jogo.njogadas - jogo.pontos));
    fprintf(Estatistica, "Numero de colisoes com parede: %d\n", jogo.parede);
    fprintf(Estatistica, "Numero de movimentos para baixo: %d\n", r[3].x);
    fprintf(Estatistica, "Numero de movimentos para cima: %d\n", r[2].x);
    fprintf(Estatistica, "Numero de movimentos para esquerda: %d\n", r[0].x);
    fprintf(Estatistica, "Numero de movimentos para direita: %d\n", r[1].x);
}

//Gera conteudo do arquivo trilha.txt
void PrintaTrilha(FILE * trilha, tJogo jogo){
    int i, j;
    for(i=0; i<jogo.linhas; i++){
        for(j=0; j<jogo.colunas; j++){
            if(jogo.trilha[i][j] == -1 && j!=(jogo.colunas-1)){ //se a posicao (i,j) for -1, printa "#"
                fprintf(trilha, "# "); //se j nao for a coluna, printa com um espaco depois do #
            }else if(jogo.trilha[i][j] == -1){
                fprintf(trilha, "#");
            }else{ //se a posicao (i,j) nao for -1, printa o numero presente nela
                fprintf(trilha, "%d ", jogo.trilha[i][j]);
            }
        }
        fprintf(trilha, "\n");
    }
}

int main(int argc, char * argv[]){
    FILE * pMapa, * pInicializa, *pResumo, *pRanking, *pEstatistica, *pTrilha;
    char dirmapa[1000], dirinicio[1000], diresumo[1000], diranking[1000], direst[1000], dirtrilha[1000];
    int i, j;
    tJogo jogo;

    //Analisa se o diretório foi informado, avisa e finaliza o programa caso não tenha sido
    if(argc <= 1){ 
        printf("ERRO: O diretorio de arquivos de configuracao nao foi informado\n");
        return 1;
    }

    //Cria diretórios dos arquivos do jogo e os abre
    sprintf(dirmapa, "%s/mapa.txt", argv[1]);
    pMapa = fopen(dirmapa,"r");
    sprintf(dirinicio, "%s/saida/inicializacao.txt", argv[1]);
    pInicializa = fopen(dirinicio, "w");
    sprintf(diresumo, "%s/saida/resumo.txt", argv[1]);
    pResumo = fopen(diresumo, "w");
    sprintf(diranking, "%s/saida/ranking.txt", argv[1]);
    pRanking = fopen(diranking, "w");
    sprintf(direst, "%s/saida/estatisticas.txt", argv[1]);
    pEstatistica = fopen(direst, "w");
    sprintf(dirtrilha, "%s/saida/trilha.txt", argv[1]);
    pTrilha = fopen(dirtrilha, "w");

    //Analisa se o arquivo de mapa foi achado, informa e finaliza o programa caso não tenha sido
    if(!pMapa){
        printf("Arquivo mapa.txt em %s nao foi encontrado!\n", dirmapa);
        return 1;
    }
    
    jogo = LeMapa(pMapa, jogo);
    
    PrintaInicializa(pInicializa, jogo);
    
    jogo = Inicializa(jogo);
    jogo = JogaJogo(jogo, pResumo);
    
    PrintaEstatistica(pEstatistica, jogo, jogo.ranking);
    PrintaRanking(pRanking, jogo.ranking);
    PrintaTrilha(pTrilha, jogo);
    
    //Fecha os arquivos do jogo
    fclose(pMapa);
    fclose(pInicializa);
    fclose(pResumo);
    fclose(pEstatistica);
    fclose(pRanking);
    fclose(pTrilha);
    
    return 0;
}
