/*Daniel Elias Fonseca Rumin 102113149
  Vitor Hugo Rossi Martinez 102113050 
*/

#include <stdio.h>
#include <conio.h>
#include <Windows.h>
#include <ctype.h>
#include <string.h>

#include "listas.h"

#define DESLX 18
#define DESLY 2
#define LARGURA 80
#define ALTURA 25

void verificarArquivo(TpDescServ &descServ);
void recuperarDadosUsuarios(TpDescUs &descUs, TpDescServ &descServ, FILE *ptr);
void recuperarDadosServidores(TpDescServ &descServ, TpDescUs &descUs, FILE *ptrArqServ);
void recuperarDadosMensagens(TpDescMen &descMen, TpDescUs &descUs, FILE *ptrArqMenn);
void criarContaUsuario(TpDescServ &descServ);
char login(TpDescServ &descServ);

void inserir();
void consultar();
void deletar();
void alterar();

void colorirFundo(short x1, short y1, short x2, short y2, char cor, char c);
void desenharRetangulo(short x, short y, short largura, short altura, char cor1, char cor2, char cor3);
void menuInicio(TpDescServ &descServ);
void interfaceUsuario(TpDescServ &descServ, char login[]);
void interfaceAdministrador(TpDescServ &descServ, char sessaoAtual[]);

char contaServidorValido(char login[32], TpDescServ descServ);

void admInserirUsuario(TpDescServ &descServ);
void admConsultarUsuario(TpDescServ &descServ, char modo);
void admAlterarUsuario(TpDescServ &descServ, char login[]);

void admInserirServidor(TpDescServ &descServ);
void admConsultarServidor(TpDescServ &descServ, char modo);
void admAlterarServidor(TpDescServ &descServ, char dominio[]);

void admInserirMensagem(TpDescServ &descServ, char remetente[]);
void admConsultarMensagem(TpDescServ &descServ, char modo);

void usuConsultarMensagem(TpDescServ &descServ, char modo, char login[]);
void usuAlterarUsuario(TpDescServ &descServ, char login[]);
void guardarDados(TpDescServ &descServ);

int main() {
	TpServidor *listaServidor;
	TpDescServ descServ;
	
	//iniciando => metodos que devem ser chamados assim que o programa comeca
	colorirFundo(0, 0, 119, 35, 119,' ');
	iniciarDescServ(descServ);
	verificarArquivo(descServ);
	
	//autenticacao
	menuInicio(descServ);
	
	exibirServidores(descServ);
	exibirMensagens(descServ);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1,31});
	
	//finalizar
	guardarDados(descServ);
}

// ######################################################################
// #                     RESGATAR DADOS INICIAIS                        #
// ######################################################################

void recuperarDadosUsuarios(TpDescUs &descUs, TpDescServ &descServ, FILE *ptrArqUs) {
	TpUsuario regUsuario;
	int i, j;
	char auxStr[31];
	TpServidor *listaServidor;

	fscanf(ptrArqUs,"%[^;];%c;%[^\n]\n",&regUsuario.login,&regUsuario.tipo,&regUsuario.senha);
	while(!feof(ptrArqUs)) {
		
		i = 0, j = 0;
		while(regUsuario.login[i] != '@') {
			i++;
		}
		while(regUsuario.login[i] != '\0') {
			auxStr[j++] = regUsuario.login[++i];
		}
		auxStr[j] = '\0';
		
		listaServidor = descServ.inicio; 
		while(strcmp(listaServidor -> dominio,auxStr) != 0) {
			listaServidor = listaServidor -> prox;
		}
		
		inserirUsuario(regUsuario, listaServidor -> descUs);
		
		fscanf(ptrArqUs,"%[^;];%c;%[^\n]\n",&regUsuario.login,&regUsuario.tipo,&regUsuario.senha); 
	}
	
	i = 0, j = 0;
	while(regUsuario.login[i] != '@') {
		i++;
	}
	while(regUsuario.login[i] != '\0') {
		auxStr[j++] = regUsuario.login[++i];
	}
	auxStr[j] = '\0';
	
	listaServidor = descServ.inicio;
	while(strcmp(listaServidor -> dominio,auxStr) != 0) {
		listaServidor = listaServidor -> prox;
	}
	
	inserirUsuario(regUsuario, listaServidor -> descUs);
	
}

void recuperarDadosServidores(TpDescServ &descServ, FILE *ptrArqServ) {
	TpServidor regServidor;

	fscanf(ptrArqServ,"%[^;];%[^\n]\n",&regServidor.dominio,&regServidor.local);
	while(!feof(ptrArqServ)) {
		inserirServidor(regServidor, descServ);
		fscanf(ptrArqServ,"%[^;];%[^\n]\n",&regServidor.dominio,&regServidor.local);
	}
	inserirServidor(regServidor, descServ);
}

void recuperarDadosMensagens(TpDescMen &descMen, TpDescUs &descUs,TpUsuario *listaUsuario, FILE *ptrArqMen) {
	TpMensagem regMensagem;
	rewind(ptrArqMen);
	fscanf(ptrArqMen,"%[^;];%[^;];%[^;];%[^;];%[^\n]\n",&regMensagem.assunto,&regMensagem.horario,&regMensagem.remetente,&regMensagem.destinatario,&regMensagem.mensagem);	
	while(!feof(ptrArqMen))
	{
		if(strcmp(regMensagem.remetente,listaUsuario->login)==0)
			inserirMensagem(regMensagem, descMen);
		fscanf(ptrArqMen,"%[^;];%[^;];%[^;];%[^;];%[^\n]\n",&regMensagem.assunto,&regMensagem.horario,&regMensagem.remetente,&regMensagem.destinatario,&regMensagem.mensagem);	
	}
	if(strcmp(regMensagem.remetente,listaUsuario->login)==0)
		inserirMensagem(regMensagem, descMen);

}

void verificarArquivo(TpDescServ &descServ) {
	FILE *ptrArq;
	TpServidor *listaServidor;
	TpUsuario *listaUsuario;
	TpMensagem *listaMensagem;
	TpDescMen auxDesc;
	
	//checar existencia de dados dos servidores
	ptrArq = fopen("servidores.txt","r");
	if(ptrArq == NULL) {
		ptrArq = fopen("servidores.txt","w");
		fprintf(ptrArq,"VENATVS,BRASIL-SP-SAO PAULO\n");
	}
	else {
		recuperarDadosServidores(descServ, ptrArq);
	}
	fclose(ptrArq);
	
	listaServidor = descServ.inicio;
	
	//checar existencia de dados dos usuarios
	ptrArq = fopen("usuarios.txt","r");
	if(ptrArq == NULL) {
		ptrArq = fopen("usuarios.txt","w");
		fprintf(ptrArq,"Mediador;A;12345\n");
		iniciarDescUs(listaServidor -> descUs);
	}
	else {		
		recuperarDadosUsuarios(listaServidor -> descUs, descServ, ptrArq);		
	}
	fclose(ptrArq);
	
	listaServidor = descServ.inicio;
	
	//checar existencia de dados de mensagens
	ptrArq = fopen("mensagens.txt","r");
	if(ptrArq == NULL) {
		ptrArq = fopen("mensagens.txt","w");
		iniciarDescMen(listaServidor -> listaUsuario -> descMen);
	}
	else {
		
		while(listaServidor != NULL) {
			
			listaUsuario = listaServidor -> descUs.inicio;
			while(listaUsuario != NULL)
			{
				iniciarDescMen(listaUsuario -> descMen);
				recuperarDadosMensagens(listaUsuario -> descMen, listaServidor -> descUs,listaUsuario, ptrArq);
				listaUsuario=listaUsuario->prox;	
			}
		
			listaServidor = listaServidor -> prox;	
		}
	}
	fclose(ptrArq);
	
}

// ######################################################################
// #                              DESENHAR                              #
// ######################################################################

void colorirFundo(short x1, short y1, short x2, short y2, char cor, char c) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor); 
	for(int i = y1; i < y2; i++) { 
		for(int j = x1; j < x2; j++) { 
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {j,i});
			printf("%c",c);
		}
	}
}

void desenharRetangulo(short x, short y, short largura, short altura, char cor1, char cor2) {
	
	
	for(int i = x; i < x + largura; i++) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor1);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {i,y});
		printf("%c",196);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor2);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {i,y + altura});
		printf("%c",196);
	}
	
	for(int i = y; i < y + altura; i++) {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor1);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x,i});
		printf("%c",179);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor2);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x + largura,i});
		printf("%c",179);
	}
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor1);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x,y});
	printf("%c",218);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x + largura,y});
	printf("%c",191);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x,y + altura});
	printf("%c",192);
		
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cor2);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {x + largura,y + altura});
	printf("%c",217);
}

// ######################################################################
// #                MENU INICIAL - LOGIN/CRIAR CONTA                    #
// ######################################################################

void menuInicio(TpDescServ &descServ) {
	char op;
	
	do {
		// desenho da interface
		system("cls");
		desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
		colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
		printf("                             VITOR APP                               ");
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		desenharRetangulo(DESLX + 27,DESLY + 5,LARGURA - 57,ALTURA - 21,127,120);
		colorirFundo(DESLX + 28,DESLY + 6,DESLX + 50,DESLY + 9,121,177);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 28, DESLY + 7});
		printf("   A - CRIAR CONTA    ");	
	
		
		desenharRetangulo(DESLX + 27,DESLY + 10,LARGURA - 57,ALTURA - 21,127,120);
		colorirFundo(DESLX + 28,DESLY + 11,DESLX + 50,DESLY + 14,121,177);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 28, DESLY + 12});
		printf("      B - LOGIN       ");
		
		desenharRetangulo(DESLX + 27,DESLY + 15,LARGURA - 57,ALTURA - 21,127,120);
		colorirFundo(DESLX + 28,DESLY + 16,DESLX + 50,DESLY + 19,121,177);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 28, DESLY + 17});
		printf("      ESC - SAIR      ");
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
		
		op = toupper(getch());
		if(op == 'A') {
			criarContaUsuario(descServ);
		}
		else if(op == 'B') {
			login(descServ);
		}
		
	}while(op != 27);
		
}

void criarContaUsuario(TpDescServ &descServ) {
	TpServidor *listaServidor = descServ.inicio;
	TpUsuario aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                            CRIAR CONTA                              ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Senha");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	gets(aux.login);
	
	while(strcmp(aux.login,"") == 0 || strcmp(buscarUsuarioServidor(aux.login,descServ).login,"") != 0 || !contaServidorValido(aux.login,descServ)) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome      ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.login);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Senha             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.senha);
	
	while(strcmp(aux.senha,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar esta senha      ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.senha);
	}
	
	int i = 0, j = 0;
	char auxStr[31];
	while(aux.login[i] != '@') {
		i++;
	}
	while(aux.login[i] != '\0') {
		auxStr[j++] = aux.login[++i];
	}
	auxStr[j] = '\0';
	
	aux.tipo = 'C';
	listaServidor = descServ.inicio;
	while(strcmp(listaServidor -> dominio,auxStr) != 0) {
		listaServidor = listaServidor -> prox;
	}
	inserirUsuario(aux,listaServidor -> descUs);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: cadastro efetuado com sucesso!            ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar ao menu
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
}

char contaServidorValido(char login[32], TpDescServ descServ) {
	int i = 0, j = 0;
	char aux[31];
	while(login[i] != '@' && login[i] != '\0') {
		i++;
	}
	
	if(login[i] == '@') {
		while(login[i] != '\0') {
			aux[j++] = login[++i];
		}
	}
	aux[j] = '\0';
	
	if(strcmp(buscarServidor(aux,descServ).dominio,"") == 0) {
		return 0;
	}
	return 1;
}

char login(TpDescServ &descServ) {
	TpUsuario aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                               LOGIN                                 ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Senha");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	gets(aux.login);
	
	while(strcmp(buscarUsuarioServidor(aux.login,descServ).login,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao foi encontrado um usuario com este nome      ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.login);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Senha                           ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.senha);
	
	while(strcmp(aux.senha,buscarUsuarioServidor(aux.login,descServ).senha) != 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: a senha nao esta correta                           ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.senha);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: login efetuado com sucesso!                               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	Sleep(3000);
	
	if(buscarUsuarioServidor(aux.login,descServ).tipo == 'C') {
		interfaceUsuario(descServ,aux.login);
	}
	else {
		interfaceAdministrador(descServ,aux.login);
	}
	
	// voltando ao menu
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(1000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
}

// ######################################################################
// #                       INTERFACE ADMNISTRADOR                       #
// ######################################################################

void interfaceAdministrador(TpDescServ &descServ, char sessaoAtual[]) {
	char op;
	
	// menu
	do {
		system("cls");
		desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
		colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
		printf("                           ADMINISTRADOR                             ");
		
		desenharRetangulo(DESLX + 4,DESLY + 5, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 5, DESLY + 6});
		printf("A - inserir servidor");
		
		desenharRetangulo(DESLX + 4,DESLY + 8, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 5, DESLY + 9});
		printf("B - alterar servidor");
		
		desenharRetangulo(DESLX + 4,DESLY + 11, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 5, DESLY + 12});
		printf("C - consultar servidor");
		
		desenharRetangulo(DESLX + 4,DESLY + 14, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 5, DESLY + 15});
		printf("D - deletar servidor");
		
		desenharRetangulo(DESLX + 28,DESLY + 5, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 29, DESLY + 6});
		printf("E - inserir usuario");
		
		desenharRetangulo(DESLX + 28,DESLY + 8, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 29, DESLY + 9});
		printf("F - alterar usuario");
		
		desenharRetangulo(DESLX + 28,DESLY + 11, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 29, DESLY + 12});
		printf("G - consultar usuario");
		
		desenharRetangulo(DESLX + 28,DESLY + 14, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 29, DESLY + 15});
		printf("H - deletar usuario");
		
		desenharRetangulo(DESLX + 53,DESLY + 5, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 54, DESLY + 6});
		printf("I - inserir mensagem");
		
		desenharRetangulo(DESLX + 53,DESLY + 8, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 54, DESLY + 9});
		printf("J - alterar mensagem");
		
		desenharRetangulo(DESLX + 53,DESLY + 11, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 54, DESLY + 12});
		printf("K - consultar mensagem");
		
		desenharRetangulo(DESLX + 53,DESLY + 14, 23,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 54, DESLY + 15});
		printf("L - deletar mensagem");
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 34, DESLY + 18});
		printf("ESC - voltar");
		
		op = toupper(getch());
		switch(op) {
			case 'A':
				admInserirServidor(descServ);
			break;
			case 'B':
				//alterar
				admConsultarServidor(descServ,1);
			break;
			case 'C':
				//consultar
				admConsultarServidor(descServ,0);
			break;
			case 'D':
				//deletar
				admConsultarServidor(descServ,2);
			break;
			case 'E':
				admInserirUsuario(descServ);
			break;
			case 'F':
				//alterar
				admConsultarUsuario(descServ,1);
			break;
			case 'G':
				//consultar
				admConsultarUsuario(descServ,0);
			break;
			case 'H':
				//deletar
				admConsultarUsuario(descServ,2);
			break;
			case 'I':
				admInserirMensagem(descServ,sessaoAtual);
			break;
			case 'J':
				admConsultarMensagem(descServ,1);
			break;
			case 'K':
				//consultar
				admConsultarMensagem(descServ,0);
			break;
			case 'L':
				//consultar
				admConsultarMensagem(descServ,2);
			break;
		}
		
		
	}while(op != 27);
	
	//retornando
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

// ######################################################################
// #                 ADMNISTRADOR - CONTROLE USUARIOS                   #
// ######################################################################

void admInserirUsuario(TpDescServ &descServ) {
	TpServidor *listaServidor = descServ.inicio;
	TpUsuario aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                          INSERIR USUARIO                            ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Senha");
	
	desenharRetangulo(DESLX + 20,DESLY + 15, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 16});
	printf("Tipo");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(aux.login);
	
	while(strcmp(aux.login,"") == 0 || strcmp(buscarUsuarioServidor(aux.login,descServ).login,"") != 0 || !contaServidorValido(aux.login,descServ)) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome         ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.login);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Senha               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.senha);
	
	while(strcmp(aux.senha,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar esta senha        ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.senha);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: qual o tipo de usuario? c - comum a - admin     ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 17});
	fflush(stdin);
	scanf("%c",&aux.tipo);
	aux.tipo = toupper(aux.tipo);
	
	while(toupper(aux.tipo) != 'C' && toupper(aux.tipo) != 'A') {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: os tipos possiveis sao C e A          ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 17,DESLX + 60,DESLY + 18,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 17});
		fflush(stdin);
		scanf("%c",&aux.tipo);
		aux.tipo = toupper(aux.tipo);
	}
	
	int i = 0, j = 0;
	char auxStr[31];
	while(aux.login[i] != '@') {
		i++;
	}
	while(aux.login[i] != '\0') {
		auxStr[j++] = aux.login[++i];
	}
	auxStr[j] = '\0';
	
	listaServidor = descServ.inicio;
	while(strcmp(listaServidor -> dominio,auxStr) != 0) {
		listaServidor = listaServidor -> prox;
	}
	inserirUsuario(aux,listaServidor -> descUs);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: cadastro efetuado com sucesso!                 ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void admConsultarUsuario(TpDescServ &descServ, char modo) {
	char login[32];
	TpUsuario aux;
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                         CONSULTAR USUARIO                           ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome do usuario");
	
	// console principal
	desenharRetangulo(DESLX + 18,DESLY + 9, 44,11,127,120);
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: indique o nome do usuario             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(login);
	
	aux = buscarUsuarioServidor(login,descServ);
	
	if(strcmp(aux.login,"")!=0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 10});
		printf("nome: %s",aux.login);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 11});
		printf("senha: %s",aux.senha);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 12});
		printf("tipo: %c",aux.tipo);
		
		if(modo == 1) {
			//alterar
			Sleep(3000);
			admAlterarUsuario(descServ,aux.login);
		}
		
		if(modo == 2) {
			//deletar
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
			printf("Status: aperte D caso queira deletar o registro               ");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
			
			if(toupper(getch()) == 'D') {
				
				int i = 0, j = 0;
				char auxStr[31];
				TpServidor *listaServidor;
				while(aux.login[i] != '@') {
					i++;
				}
				while(aux.login[i] != '\0') {
					auxStr[j++] = aux.login[++i];
				}
				auxStr[j] = '\0';
				
				listaServidor = descServ.inicio; 
				while(strcmp(listaServidor -> dominio,auxStr) != 0) {
					listaServidor = listaServidor -> prox;
				}
				
				deletarUsuario(aux.login,listaServidor -> descUs);
				
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
				printf("Status: registro deletado                            ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
			}
		}
	}
	else {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 10});
		printf("Usuario nao encontrado");
	}
	
	if(modo == 0) {
		//consultar
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
		printf("Status: pressione uma tecla para continuar               ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		fflush(stdin);
		getch();
	}
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(1000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void admAlterarUsuario(TpDescServ &descServ, char login[]) {
	TpServidor *listaServidor = descServ.inicio;
	TpUsuario aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                          ALTERAR USUARIO                            ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Senha");
	
	desenharRetangulo(DESLX + 20,DESLY + 15, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 16});
	printf("Tipo");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(aux.login);
	
	while(strcmp(aux.login,"") == 0 || strcmp(buscarUsuarioServidor(aux.login,descServ).login,"") != 0 || !contaServidorValido(aux.login,descServ)) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome         ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.login);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Senha               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.senha);
	
	while(strcmp(aux.senha,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar esta senha        ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.senha);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: qual o tipo de usuario? c - comum a - admin     ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 17});
	fflush(stdin);
	scanf("%c",&aux.tipo);
	aux.tipo = toupper(aux.tipo);
	
	while(toupper(aux.tipo) != 'C' && toupper(aux.tipo) != 'A') {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: os tipos possiveis sao C e A          ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 17,DESLX + 60,DESLY + 18,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 17});
		fflush(stdin);
		scanf("%c",&aux.tipo);
		aux.tipo = toupper(aux.tipo);
	}
	
	int i = 0, j = 0;
	char auxStr[31];
	while(aux.login[i] != '@') {
		i++;
	}
	while(aux.login[i] != '\0') {
		auxStr[j++] = aux.login[++i];
	}
	auxStr[j] = '\0';
	
	listaServidor = descServ.inicio;
	while(strcmp(listaServidor -> dominio,auxStr) != 0) {
		listaServidor = listaServidor -> prox;
	}
	alterarUsuario(login,listaServidor -> descUs,aux);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: usuario alterado com sucesso!                   ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

// ######################################################################
// #                 ADMNISTRADOR - CONTROLE SERVIDOR                   #
// ######################################################################


void admInserirServidor(TpDescServ &descServ) {
	TpServidor *listaServidor = descServ.inicio;
	TpServidor aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                         INSERIR SERVIDOR                            ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Localizacao (PAIS-ESTADO-CIDADE)");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(aux.dominio);
	
	while(strcmp(aux.dominio,"") == 0 || strcmp(buscarServidor(aux.dominio,descServ).dominio,"") != 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome         ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.dominio);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Localizacao               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.local);
	
	while(strcmp(aux.local,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar esta localizacao          ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.local);
	}
	
	inserirServidor(aux, descServ);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: cadastro efetuado com sucesso!                 ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void admConsultarServidor(TpDescServ &descServ, char modo) {
	char dominio[32];
	TpServidor aux;
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                        CONSULTAR SERVIDOR                           ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome do servidor");
	
	// console principal
	desenharRetangulo(DESLX + 18,DESLY + 9, 44,11,127,120);
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: indique o nome do servidor             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(dominio);
	
	aux = buscarServidor(dominio,descServ);
	
	if(strcmp(aux.dominio,"")!=0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 10});
		printf("nome: %s",aux.dominio);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 11});
		printf("senha: %s",aux.local);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 12});
		printf("quantidade de usuarios: %d",aux.descUs.qtd);
		
		if(modo == 1) {
			//alterar
			Sleep(3000);
			admAlterarServidor(descServ,aux.dominio);
		}
		
		if(modo == 2) {
			//deletar
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
			printf("Status: aperte D caso queira deletar o registro               ");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
			
			if(toupper(getch()) == 'D') {
				
				deletarServidor(aux.dominio,descServ);
				
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
				printf("Status: registro deletado                                 ");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
			}
		}
	}
	else {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 19, DESLY + 10});
		printf("Servidor nao encontrado");
	}
	
	if(modo == 0) {
		//consultar
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
		printf("Status: pressione uma tecla para continuar               ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		fflush(stdin);
		getch();
	}
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(1000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void admAlterarServidor(TpDescServ &descServ, char dominio[]) {
	TpServidor *listaServidor = descServ.inicio;
	TpServidor aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                         ALTERAR SERVIDOR                            ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Local (PAIS-ESTADO-CIDADE)");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(aux.dominio);
	
	while(strcmp(aux.dominio,"") == 0 || strcmp(buscarServidor(aux.dominio,descServ).dominio,"") != 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome         ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.dominio);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Localizacao               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.local);
	
	while(strcmp(aux.local,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar esta localizacao          ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.local);
	}
	
	
	alterarServidor(dominio, descServ,aux);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: servidor alterado com sucesso!                   ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

// ######################################################################
// #                 ADMNISTRADOR - CONTROLE MENSAGEM                   #
// ######################################################################

void admInserirMensagem(TpDescServ &descServ, char remetente[]) {
	TpServidor *listaServidor = descServ.inicio;
	TpMensagem aux;
	TpUsuario *listaUsuario;
	TpMensagem *listaMensagem;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                         INSERIR MENSAGEM                            ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Destinatario");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Assunto");
	
	desenharRetangulo(DESLX + 11,DESLY + 16, 59,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 12, DESLY + 17});
	printf("Mensagem");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	listaUsuario = listaServidor -> descUs.inicio;
	listaMensagem = listaUsuario -> descMen.inicio;
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Destinatario             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(aux.destinatario);
	
	//encontrar servidor que tem o usuario certo
	listaServidor = descServ.inicio;
	char achou = 0;
	while(listaServidor != NULL && achou == 0) {
		listaUsuario = listaServidor -> descUs.inicio;
		
		while(listaUsuario != NULL && strcmp(aux.destinatario,listaUsuario -> login) != 0) {
			listaUsuario = listaUsuario -> prox;
		}
		
		if(listaUsuario != NULL) {
			achou = 1;
		}
		else {
			listaServidor = listaServidor -> prox;
		}
		
	}	
	
	while(achou == 0 || strcmp(buscarUsuario(aux.destinatario,listaServidor -> descUs).login,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome         ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.destinatario);
		
		//encontrar servidor que tem o usuario certo
		listaServidor = descServ.inicio;
		while(listaServidor != NULL && achou == 0) {
			listaUsuario = listaServidor -> descUs.inicio;
			
			while(listaUsuario != NULL && strcmp(aux.destinatario,listaUsuario -> login) != 0) {
				listaUsuario = listaUsuario -> prox;
			}
			
			if(listaUsuario != NULL) {
				achou = 1;
			}
			else {
				listaServidor = listaServidor -> prox;
			}
			
		}	
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Assunto               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.assunto);
	
	while(strcmp(aux.assunto,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: o campo Assunto esta vazio!          ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.assunto);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: insira a mensagem                                 ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 12, DESLY + 18});
	gets(aux.mensagem);
	
	//pegar horario
	struct tm* ptr;
    time_t t;
    t = time(NULL);
    ptr = localtime(&t);
	strcpy(aux.horario,asctime(ptr));
	
	strcpy(aux.remetente,remetente);
	
	//encontrar serv
	achou = 0;
	listaServidor = descServ.inicio;
	while(listaServidor != NULL && achou == 0) {
		listaUsuario = listaServidor -> descUs.inicio;
		
		while(listaUsuario != NULL && strcmp(aux.remetente,listaUsuario -> login) != 0) {
			listaUsuario = listaUsuario -> prox;
		}
		
		if(listaUsuario != NULL) {
			achou = 1;
		}
		else {
			listaServidor = listaServidor -> prox;
		}
		
	}
	
	inserirMensagem(aux, listaUsuario -> descMen);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: mensagem enviada com sucesso!                     ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void admConsultarMensagem(TpDescServ &descServ, char modo) {
	TpServidor aux, regServ, *listaServ = descServ.inicio;
	TpUsuario *listaUsuario;
	TpMensagem *listaMensagem, *msgAux, regMen;
	char op = 0, filtro[32], achou;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                        CONSULTAR MENSAGEM                           ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Servidor");
	
	desenharRetangulo(DESLX + 20,DESLY + 9, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 10});
	printf("Filtro (palavra)");
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Servidor               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	// console principal
	desenharRetangulo(DESLX + 10,DESLY + 13, 60,6,127,120);
	
	//pegar informacoes
	
	fflush(stdin);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	gets(regServ.dominio);
	
	aux = buscarServidor(regServ.dominio, descServ);
	
	while(strcmp(aux.dominio,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: nao foi possivel obter um servidor               ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(regServ.dominio);
		
		aux = buscarServidor(regServ.dominio, descServ);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: caso queira, insira um filtro                      ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	gets(filtro);
	
	// exibir informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	if(modo == 0) {
		printf("Status: aperte A ou D para navegar e ESC para sair          ");	
	}
	else if(modo == 1) {
		printf("Status: A ou D: navegar, ESC: sair, M: modificar             ");
	}
	else if(modo == 2) {
		printf("Status: A ou D: navegar, ESC: sair, E: excluir              ");
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	listaServ = descServ.inicio;
	while(strcmp(listaServ -> dominio,regServ.dominio) != 0) {
		listaServ = listaServ -> prox;
	}
	
	listaUsuario = listaServ -> descUs.inicio;
	listaMensagem = listaUsuario -> descMen.inicio;
	
	if(strcmp(filtro,"")!=0) {
		achou = 0;
	
		while(listaUsuario != NULL && !achou) {
			listaMensagem = listaUsuario -> descMen.inicio;
			
			while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
				listaMensagem = listaMensagem -> prox;
			}
			if(listaMensagem == NULL) {
				listaUsuario = listaUsuario -> prox;
			}	
			else {
				achou = 1;
			}
		}
	
	}
	
	if(listaUsuario == NULL) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
		printf("Nao foi possivel encontrar mensagens com o filtro");
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: nao foi possivel obter mensagens                 ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	}
	else do {
		colorirFundo(DESLX + 11,DESLY + 14,DESLX + 70,DESLY + 19,112,' ');
		
		if(modo == 1 && op == 'M') {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Novo assunto:");
			fflush(stdin);
			gets(regMen.assunto);
			
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 16});
			printf("Nova mesagem:");
			fflush(stdin);
			gets(regMen.mensagem);
			
			strcpy(regMen.horario,listaMensagem -> horario);
			strcpy(regMen.destinatario,listaMensagem -> destinatario);
			strcpy(regMen.remetente,listaMensagem -> remetente);
			
			alterarMensagem(listaMensagem -> horario,listaUsuario -> descMen, regMen);
			colorirFundo(DESLX + 11,DESLY + 14,DESLX + 70,DESLY + 19,112,' ');
		}
		
		if(listaMensagem != NULL && modo == 2 && op == 'E') {
			deletarMensagem(listaMensagem -> horario,listaUsuario -> descMen);
			
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
			printf("Status: registro excluido                             ");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		}
		
		if(op == 'A') {
			
			if(listaMensagem != NULL && listaMensagem -> ant != NULL) {
				listaMensagem = listaMensagem -> ant;
			}
			else if(listaUsuario -> ant != NULL) {
				listaUsuario = listaUsuario -> ant;
				listaMensagem = listaUsuario -> descMen.fim;
			}
			
			if(strcmp(filtro,"")!=0) {
				achou = 0;
		
				while(listaUsuario != NULL && !achou) {
						
					while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
						listaMensagem = listaMensagem -> ant;
					}
					if(listaMensagem == NULL && listaUsuario -> ant != NULL) {
						listaUsuario = listaUsuario ->ant;
						listaMensagem = listaUsuario -> descMen.fim;
					}	
					else {
						achou = 1;
					}
		
				}
				
				//processo inverso
				if(listaUsuario == NULL || listaMensagem == NULL) {
					listaUsuario = listaServ -> descUs.inicio;
					listaMensagem = listaUsuario -> descMen.inicio;
					achou = 0;
		
					while(listaUsuario != NULL && !achou) {
						
						while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
							listaMensagem = listaMensagem -> prox;
						}
						if(listaMensagem == NULL && listaUsuario -> prox != NULL) {
							listaUsuario = listaUsuario -> prox;
							listaMensagem = listaUsuario -> descMen.inicio;
						}	
						else {
							achou = 1;
						}
						
					}
					
				}

			}	
	
			
		}
		else if(op == 'D') {
			
			if(listaMensagem != NULL && listaMensagem -> prox != NULL) {
				listaMensagem = listaMensagem -> prox;
			}
			else if(listaUsuario -> prox != NULL) {
				listaUsuario = listaUsuario -> prox;	
				listaMensagem = listaUsuario -> descMen.inicio;
			}
	
			if(strcmp(filtro,"")!=0) {
				achou = 0;
		
				while(listaUsuario != NULL && !achou) {
					
					while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
						listaMensagem = listaMensagem -> prox;
					}
					if(listaMensagem == NULL && listaUsuario -> prox != NULL) {
						listaUsuario = listaUsuario -> prox;
						listaMensagem = listaUsuario -> descMen.inicio;
					}	
					else {
						achou = 1;
					}
					
				}
				
				//processo inverso
				if(listaUsuario == NULL || listaMensagem == NULL) {
					listaUsuario = listaServ -> descUs.fim;
					listaMensagem = listaUsuario -> descMen.fim;
					achou = 0;
		
					while(listaUsuario != NULL && !achou) {
						
						while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
							listaMensagem = listaMensagem -> ant;
						}
						if(listaMensagem == NULL && listaUsuario -> ant != NULL) {
							listaUsuario = listaUsuario -> ant;
							listaMensagem = listaUsuario -> descMen.fim;
						}	
						else {
							achou = 1;
						}
						
					}
					
				}
	
			}	
			
		}
		
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 14});
		if(listaMensagem != NULL && modo == 2 && op == 'E') {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Mensagem apagada");
		}
		else if(modo == 1 && op == 'M') {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Mensagem alterada");
		}
		else if(listaMensagem != NULL && listaUsuario -> descMen.qtd > 0) {
			printf("remetente: %s", listaMensagem -> remetente);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("%s", listaMensagem -> horario);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 16});
			printf("para: %s", listaMensagem -> destinatario);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 17});
			printf("assunto: %s", listaMensagem -> assunto);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 18});
			printf("%s", listaMensagem -> mensagem);
		}
		else {
			printf("remetente: %s", listaUsuario -> login);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Nao ha mensagem");
		}
		
		op = toupper(getch());
		
	}while(op != 27);
	
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

// ######################################################################
// #                        INTERFACE USUARIO                           #
// ######################################################################

void interfaceUsuario(TpDescServ &descServ, char login[]) {
	char op, confirm, deletou = 0;
	TpServidor *listaServidor;
	
	do {
			
		system("cls");
		desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
		colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
		
		//retornando
		
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
		printf("                              USUARIO                                ");
		
		desenharRetangulo(DESLX + 14,DESLY + 5, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 15, DESLY + 6});
		printf("A - enviar mensagem");
		
		desenharRetangulo(DESLX + 14,DESLY + 8, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 15, DESLY + 9});
		printf("B - alterar mensagem");
		
		desenharRetangulo(DESLX + 14,DESLY + 11, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 15, DESLY + 12});
		printf("C - excluir mensagem");
		
		desenharRetangulo(DESLX + 43,DESLY + 5, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 44, DESLY + 6});
		printf("D - ver mensagens");
		
		desenharRetangulo(DESLX + 43,DESLY + 8, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 44, DESLY + 9});
		printf("E - alterar dados");
		
		desenharRetangulo(DESLX + 43,DESLY + 11, 22,2,127,120);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 44, DESLY + 12});
		printf("F - excluir conta");
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 34, DESLY + 18});
		printf("ESC - voltar");
		
		op = toupper(getch());
		
		switch(op) {
			
			case 'A':
				admInserirMensagem(descServ,login);
			break;
			case 'B':
				usuConsultarMensagem(descServ,1,login);
			break;
			case 'C':
				usuConsultarMensagem(descServ,2,login);
			break;
			case 'D':
				usuConsultarMensagem(descServ,0,login);
			break;
			case 'E':
				usuAlterarUsuario(descServ,login);
			break;
			case 'F':
				desenharRetangulo(29,16,60,5,127,120);
				colorirFundo(30,17,87,21,124,'#');
				colorirFundo(31,18,86,20,127,' ');
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {32, 19});
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),124);
				printf("ATENCAO: APERTE D SE REALMENTE DESEJA DELETAR A CONTA");
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),127);
				
				confirm = getch();
				
				if(toupper(confirm) == 'D') {
					int i = 0, j = 0;
					char auxStr[31];
					
					TpServidor *listaServidor;
					while(login[i] != '@') {
						i++;
					}
					while(login[i] != '\0') {
						auxStr[j++] = login[++i];
					}
					auxStr[j] = '\0';
					
					listaServidor = descServ.inicio; 
					while(strcmp(listaServidor -> dominio,auxStr) != 0) {
						listaServidor = listaServidor -> prox;
					}
					
					deletarUsuario(login,listaServidor -> descUs);
					
					SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {32, 19});
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),124);
					printf("                   CONTA EXCLUIDA                    ");
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),127);
					
					deletou = 1;
				}
				
				Sleep(1000);
			break;
		}
		
		
		
	}while(op != 27 && !deletou);
	
}

// ######################################################################
// #                        ACOES DO  USUARIO                           #
// ######################################################################

void usuConsultarMensagem(TpDescServ &descServ, char modo, char login[]) {
	TpServidor aux, regServ, *listaServ = descServ.inicio;
	TpUsuario *listaUsuario;
	TpMensagem *listaMensagem, *msgAux, regMen;
	char op = 0, filtro[32], achou;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                        CONSULTAR MENSAGEM                           ");
	
	desenharRetangulo(DESLX + 20,DESLY + 9, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 10});
	printf("Filtro (palavra)");
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Servidor               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	// console principal
	desenharRetangulo(DESLX + 10,DESLY + 13, 60,6,127,120);
	
	//pegar informacoes

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: caso queira, insira um filtro                      ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	gets(filtro);
	
	// exibir informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	if(modo == 0) {
		printf("Status: aperte A ou D para navegar e ESC para sair          ");	
	}
	else if(modo == 1) {
		printf("Status: A ou D: navegar, ESC: sair, M: modificar             ");
	}
	else if(modo == 2) {
		printf("Status: A ou D: navegar, ESC: sair, E: excluir              ");
	}

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	int i = 0, j = 0;
	char auxStr[31];
	while(login[i] != '@') {
		i++;
	}
	while(login[i] != '\0') {
		auxStr[j++] = login[++i];
	}
	auxStr[j] = '\0';
	
	while(strcmp(listaServ -> dominio,auxStr) != 0) {
		listaServ = listaServ -> prox;
	}

	listaUsuario = listaServ -> descUs.inicio;
	listaMensagem = listaUsuario -> descMen.inicio;
	
	achou = 0;
	
	while(listaUsuario != NULL && !achou) {
		listaMensagem = listaUsuario -> descMen.inicio;
		
		while(listaMensagem != NULL && strcmp(listaMensagem -> remetente,login) != 0) {
			listaMensagem = listaMensagem -> prox;
		}
		if(listaMensagem == NULL) {
			listaUsuario = listaUsuario -> prox;
		}	
		else {
			achou = 1;
		}
	}
	
	if(strcmp(filtro,"")!=0) {
		achou = 0;
	
		while(listaUsuario != NULL && !achou) {
			listaMensagem = listaUsuario -> descMen.inicio;
			
			while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
				listaMensagem = listaMensagem -> prox;
			}
			if(listaMensagem == NULL) {
				listaUsuario = listaUsuario -> prox;
			}	
			else {
				achou = 1;
			}
		}
	
	}
	
	if(listaUsuario == NULL) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
		printf("Nao foi possivel encontrar mensagens com o filtro");
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: nao foi possivel obter mensagens                 ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	}
	else do {
		colorirFundo(DESLX + 11,DESLY + 14,DESLX + 70,DESLY + 19,112,' ');
		
		if(modo == 1 && op == 'M') {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Novo assunto:");
			fflush(stdin);
			gets(regMen.assunto);
			
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 16});
			printf("Nova mesagem:");
			fflush(stdin);
			gets(regMen.mensagem);
			
			strcpy(regMen.horario,listaMensagem -> horario);
			strcpy(regMen.destinatario,listaMensagem -> destinatario);
			strcpy(regMen.remetente,listaMensagem -> remetente);
			
			alterarMensagem(listaMensagem -> horario,listaUsuario -> descMen, regMen);
			colorirFundo(DESLX + 11,DESLY + 14,DESLX + 70,DESLY + 19,112,' ');
		}
		
		if(listaMensagem != NULL && modo == 2 && op == 'E') {
			deletarMensagem(listaMensagem -> horario,listaUsuario -> descMen);
			
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
			printf("Status: registro excluido                             ");
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		}
		
		if(op == 'A') {
			
			if(listaMensagem != NULL && listaMensagem -> ant != NULL && strcmp(listaMensagem -> ant -> remetente,login)==0) {
				listaMensagem = listaMensagem -> ant;
			}
			
			if(strcmp(filtro,"")!=0) {
				achou = 0;
		
				while(listaUsuario != NULL && !achou) {
						
					while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
						listaMensagem = listaMensagem -> ant;
					}
					if(listaMensagem == NULL && listaUsuario -> ant != NULL) {
						listaUsuario = listaUsuario ->ant;
						listaMensagem = listaUsuario -> descMen.fim;
					}	
					else {
						achou = 1;
					}
		
				}
				
				//processo inverso
				if(listaUsuario == NULL || listaMensagem == NULL) {
					listaUsuario = listaServ -> descUs.inicio;
					listaMensagem = listaUsuario -> descMen.inicio;
					achou = 0;
		
					while(listaUsuario != NULL && !achou) {
						
						while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
							listaMensagem = listaMensagem -> prox;
						}
						if(listaMensagem == NULL && listaUsuario -> prox != NULL) {
							listaUsuario = listaUsuario -> prox;
							listaMensagem = listaUsuario -> descMen.inicio;
						}	
						else {
							achou = 1;
						}
						
					}
					
				}

			}	
	
			
		}
		else if(op == 'D') {
			
			if(listaMensagem != NULL && listaMensagem -> prox != NULL && strcmp(listaMensagem -> prox -> remetente,login)==0) {
				listaMensagem = listaMensagem -> prox;
			}
	
			if(strcmp(filtro,"")!=0) {
				achou = 0;
		
				while(listaUsuario != NULL && !achou) {
					
					while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
						listaMensagem = listaMensagem -> prox;
					}
					if(listaMensagem == NULL && listaUsuario -> prox != NULL) {
						listaUsuario = listaUsuario -> prox;
						listaMensagem = listaUsuario -> descMen.inicio;
					}	
					else {
						achou = 1;
					}
					
				}
				
				//processo inverso
				if(listaUsuario == NULL || listaMensagem == NULL) {
					listaUsuario = listaServ -> descUs.fim;
					listaMensagem = listaUsuario -> descMen.fim;
					achou = 0;
		
					while(listaUsuario != NULL && !achou) {
						
						while(listaMensagem != NULL && strstr(listaMensagem->assunto,filtro) == NULL && strstr(listaMensagem->mensagem,filtro) == NULL) {
							listaMensagem = listaMensagem -> ant;
						}
						if(listaMensagem == NULL && listaUsuario -> ant != NULL) {
							listaUsuario = listaUsuario -> ant;
							listaMensagem = listaUsuario -> descMen.fim;
						}	
						else {
							achou = 1;
						}
						
					}
					
				}
	
			}	
			
		}
		
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 14});
		if(modo == 2 && op == 'E') {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Mensagem apagada");
		}
		else if(modo == 1 && op == 'M') {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Mensagem alterada");
		}
		else if(listaMensagem != NULL && listaUsuario -> descMen.qtd > 0) {
			printf("remetente: %s", listaMensagem -> remetente);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("%s", listaMensagem -> horario);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 16});
			printf("para: %s", listaMensagem -> destinatario);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 17});
			printf("assunto: %s", listaMensagem -> assunto);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 18});
			printf("%s", listaMensagem -> mensagem);
		}
		else {
			printf("remetente: %s", listaUsuario -> login);
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 11, DESLY + 15});
			printf("Nao ha mensagem");
		}
		
		op = toupper(getch());
		
	}while(op != 27);
	
	
	// voltar
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void usuAlterarUsuario(TpDescServ &descServ, char login[]) {
	TpServidor *listaServidor = descServ.inicio;
	TpUsuario aux;
	
	system("cls");
	desenharRetangulo(DESLX,DESLY,LARGURA,ALTURA,127,120);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 25);
	colorirFundo(DESLX + 4,DESLY + 1,DESLX + 77,DESLY + 4,25,'#'); 
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 155);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 6, DESLY + 2});
	printf("                           ALTERAR DADOS                             ");
	
	desenharRetangulo(DESLX + 20,DESLY + 5, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 6});
	printf("Nome");
	
	desenharRetangulo(DESLX + 20,DESLY + 10, 40,3,127,120);
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 11});
	printf("Senha");
	
	desenharRetangulo(DESLX + 2,DESLY + 21, LARGURA - 3,2,127,120);
	
	//obter informacoes
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Nome             ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
	fflush(stdin);
	gets(aux.login);
	
	while(strcmp(aux.login,"") == 0 || strcmp(buscarUsuarioServidor(aux.login,descServ).login,"") != 0 || !contaServidorValido(aux.login,descServ)) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar este nome         ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 7,DESLX + 60,DESLY + 8,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 7});
		gets(aux.login);
	}
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: por favor, preencha o campo Senha               ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
	gets(aux.senha);
	
	while(strcmp(aux.senha,"") == 0) {
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 124);
		printf("Status: ERRO: nao e possivel usar esta senha        ");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
		
		colorirFundo(DESLX + 21, DESLY + 12,DESLX + 60,DESLY + 13,112,' ');
		
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 21, DESLY + 12});
		gets(aux.senha);
	}
	
	aux.tipo = 'C';
	
	int i = 0, j = 0;
	char auxStr[31];
	while(aux.login[i] != '@') {
		i++;
	}
	while(aux.login[i] != '\0') {
		auxStr[j++] = aux.login[++i];
	}
	auxStr[j] = '\0';
	
	listaServidor = descServ.inicio;
	while(strcmp(listaServidor -> dominio,auxStr) != 0) {
		listaServidor = listaServidor -> prox;
	}
	alterarUsuario(login,listaServidor -> descUs,aux);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {DESLX + 3, DESLY + 22});
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 114);
	printf("Status: usuario alterado com sucesso!                   ");
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
	
	// voltar
	
	strcpy(login,aux.login);
	
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), {1, 29});
	Sleep(3000);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 112);
}

void guardarDados(TpDescServ &descServ) {
	
	FILE *ser = fopen("servidores.txt","w");
	FILE *usu = fopen("usuarios.txt","w");
	FILE *men = fopen("mensagens.txt","w");
	
	TpServidor *lista = descServ.inicio;
	TpUsuario *listaUsuario;
	TpMensagem *listaMensagem;
	
	while(lista != NULL) {
		fprintf(ser,"%s;%s\n",lista -> dominio,lista -> local);
		
		listaUsuario = lista -> descUs.inicio;
		
		while(listaUsuario != NULL) {
			fprintf(usu,"%s;%c;%s\n",listaUsuario -> login,listaUsuario -> tipo, listaUsuario -> senha);
			
			listaMensagem = listaUsuario -> descMen.inicio;
			
			while(listaMensagem != NULL) {

				fprintf(men,"%s;%s;%s;%s;%s\n",listaMensagem -> assunto,listaMensagem -> horario, listaMensagem -> remetente, listaMensagem -> destinatario, listaMensagem -> mensagem);
				listaMensagem = listaMensagem -> prox;
			}
						
			listaUsuario = listaUsuario -> prox;
		}
		
		lista = lista -> prox;
	}
	
	fclose(ser);
	fclose(usu);
	fclose(men);
}
