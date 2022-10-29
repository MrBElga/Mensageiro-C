// ######################################################################
// #   STRUCTS E FUNCOES PARA MANIPULACAO DAS LISTAS - VITOR MARTINEZ   #
// ######################################################################

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <conio2.h>

struct TpMensagem {
	char assunto[32];
	char destinatario[32];
	char remetente[32];
	char mensagem[1024];
	char horario[30];
	TpMensagem *prox, *ant;
};

struct TpDescMen {
	TpMensagem *inicio, *fim;
	int qtd;
};

struct TpUsuario {
	char login[32];
	char senha[16];
	char tipo;
	TpDescMen descMen;
	TpMensagem *listaMensagem;
	TpUsuario *prox, *ant;
};

struct TpDescUs {
	TpUsuario *inicio, *fim;
	int qtd;
};

struct TpServidor {
	char dominio[32];
	char local[32];
	TpDescUs descUs;
	TpUsuario *listaUsuario;
	TpServidor *prox, *ant;
};

struct TpDescServ {
	TpServidor *inicio, *fim;
	int qtd;
};

void iniciarDescServ(TpDescServ &descServ);
void iniciarDescUs(TpDescUs &descUs);
void iniciarDescMen(TpDescMen &descMen);

TpUsuario buscarUsuario(char login[], TpDescUs descUs);
TpUsuario buscarUsuarioServidor(char login[], TpDescServ descServ);
void inserirUsuario(TpUsuario usuario, TpDescUs &descUs);
void deletarUsuario(char login[], TpDescUs &descUs);
void alterarUsuario(char login[], TpDescUs &descUs, TpUsuario usuario);

void exibirUsuarios(TpDescUs &descUs);

TpServidor buscarServidor(char dominio[], TpDescServ descServ);
void inserirServidor(TpServidor servidor, TpDescServ &descServ);
void deletarServidor(char dominio[], TpDescServ &descServ);
void alterarServidor(char dominio[], TpDescServ &descServ, TpServidor servidor);

void exibirServidores(TpDescServ descServ);

TpMensagem buscarMensagem(TpDescMen descMen);
void inserirMensagem(TpMensagem mensagem, TpDescMen &descMen);
void deletarMensagem(char horario[],TpDescMen &descMen);
void alterarMensagem(char horario[], TpDescMen &descMen, TpMensagem mensagem);

// ######################################################################
// #                    INICIO DOS DESCRITORES                          #
// ######################################################################

void iniciarDescServ(TpDescServ &descServ) {
	descServ.inicio = descServ.fim = NULL;
	descServ.qtd = 0;
}

void iniciarDescUs(TpDescUs &descUs) {
	descUs.inicio = descUs.fim = NULL;
	descUs.qtd = 0;
}

void iniciarDescMen(TpDescMen &descMen) {
	descMen.inicio = descMen.fim = NULL;
	descMen.qtd = 0;
}

// ######################################################################
// #                            SERVIDORES                              #
// ######################################################################

TpServidor buscarServidor(char dominio[], TpDescServ descServ) {
	TpServidor reg, *lista;
	
	strcpy(reg.dominio,"");
	lista = descServ.inicio;
	
	while(lista != NULL && strcmp(dominio,lista -> dominio) != 0) {
		lista = lista -> prox;
	}
	
	if(lista != NULL) {
		reg = *lista;
	}
	
	return reg;
}

void inserirServidor(TpServidor servidor, TpDescServ &descServ) {
	TpServidor *novo, *aux;
	
	novo= new TpServidor();
	novo -> prox = NULL;
	novo -> ant = NULL;
	strcpy(novo -> dominio, servidor.dominio);
	strcpy(novo -> local, servidor.local);
	descServ.qtd++;
	
	if(descServ.inicio == NULL) {
		//vazio
		descServ.inicio = descServ.fim = novo;
	}
	else if(strcmp(descServ.inicio -> dominio,novo -> dominio) > 0) {
		//inicio
		novo -> prox = descServ.inicio;
		descServ.inicio -> ant = novo;
		descServ.inicio = novo;
	}
	else if(strcmp(descServ.fim-> dominio, novo -> dominio) < 0) {
		//ultimo
		novo -> ant = descServ.fim;
		descServ.fim -> prox = novo;
		descServ.fim = novo;
	}
	else {
		aux = descServ.inicio -> prox;
		while(aux -> prox != NULL && strcmp(aux -> dominio,novo -> dominio) < 0) {
			aux = aux -> prox;
		}
		
		//meio
		novo -> prox = aux;
		novo -> ant = aux -> ant;
		aux -> ant -> prox = novo;
		aux -> ant = novo;
		
	}
	
}

void deletarServidor(char dominio[], TpDescServ &descServ) {
	TpServidor *aux;
	TpUsuario *listaUsuarios;
	descServ.qtd--;
	int i;
	
	//deletar usuarios do servidor
	aux = descServ.inicio;
	
	while(strcmp(aux -> dominio,dominio) != 0) {
		aux = aux -> prox;
	}
	
	listaUsuarios = aux -> descUs.inicio;
	
	while(listaUsuarios != NULL) {
		i = 0;
		
		deletarUsuario(listaUsuarios -> login, aux -> descUs);
		
		listaUsuarios = listaUsuarios -> prox;
	}
	
	// deletar
	
	if(descServ.qtd == 0) {
		aux = descServ.inicio;
		descServ.inicio = descServ.fim = NULL;;
		delete(aux);
	}
	else if(strcmp(descServ.inicio -> dominio,dominio) == 0) {
		//inicio
		aux = descServ.inicio;
		aux -> prox -> ant = NULL;
		descServ.inicio = aux -> prox;
		delete(aux);
	}
	else if(strcmp(descServ.fim -> dominio,dominio) == 0) {
		//fim
		aux = descServ.fim;
		aux -> ant -> prox = NULL;
		descServ.fim = aux -> ant;
		delete(aux);
	}
	else {
		//meio
		aux = descServ.inicio -> prox;
		
		while(strcmp(aux -> dominio,dominio) != 0) {
			aux = aux -> prox;
		}
		
		aux -> prox -> ant = aux -> ant;
		aux -> ant -> prox = aux -> prox;
		delete(aux);
		
	}
	
}

void alterarServidor(char dominio[], TpDescServ &descServ, TpServidor servidor) {
	TpServidor *lista = descServ.inicio;
	TpUsuario *listaUsuarios, regAux;
	int i, j;
	char aux[31];
	
	while(strcmp(lista -> dominio, dominio) != 0) {
		lista = lista -> prox;
	}
	
	strcpy(lista -> dominio, servidor.dominio);
	strcpy(lista -> local, servidor.local);
	
	// alterar usuarios no servidor
	listaUsuarios = lista -> descUs.inicio;
	
	while(listaUsuarios != NULL) {
		i = 0, j = 0;
		
		while(listaUsuarios -> login[i] != '@') {
			aux[i] = listaUsuarios -> login[i];
			i++;
		}
		aux[i] = '@';
		while(servidor.dominio[j] != '\0') {
			aux[++i] = servidor.dominio[j++];
		}
		aux[++i] = '\0';
		
		regAux = *listaUsuarios;
		strcpy(regAux.login,aux);
		alterarUsuario(listaUsuarios -> login, lista -> descUs, regAux);
		
		listaUsuarios = listaUsuarios -> prox;
	}
}

// ######################################################################
// #                             USUARIOS                               #
// ######################################################################

TpUsuario buscarUsuario(char login[], TpDescUs descUs) {
	TpUsuario reg, *lista;
	
	strcpy(reg.login,"");
	lista = descUs.inicio;
	
	while(lista != NULL && strcmp(login,lista -> login) != 0) {
		lista = lista -> prox;
	}
	
	if(lista != NULL) {
		reg = *lista;
	}
	
	return reg;
}

TpUsuario buscarUsuarioServidor(char login[], TpDescServ descServ) {
	TpUsuario reg, *lista;
	TpServidor *listaServidor = descServ.inicio;
	strcpy(reg.login,"");
	
	while(listaServidor != NULL && strcmp(reg.login,"") == 0) {
		lista = listaServidor -> descUs.inicio;
		
		while(lista != NULL && strcmp(login,lista -> login) != 0) {
			lista = lista -> prox;
		}
		
		if(lista != NULL) {
			reg = *lista;
		}
		
		listaServidor = listaServidor -> prox;
	}
	
	return reg;
}

void inserirUsuario(TpUsuario usuario, TpDescUs &descUs) {
	TpUsuario *novo, *aux;
	
	novo= new TpUsuario();
	novo -> prox = NULL;
	novo -> ant = NULL;
	novo -> tipo = usuario.tipo;
	strcpy(novo -> login, usuario.login);
	strcpy(novo -> senha,usuario.senha);
	descUs.qtd++;
	
	if(descUs.inicio == NULL) {
		//vazio
		descUs.inicio = descUs.fim = novo;
	}
	else if(strcmp(descUs.inicio -> login,novo -> login) > 0) {
		//inicio
		novo -> prox = descUs.inicio;
		descUs.inicio -> ant = novo;
		descUs.inicio = novo;
	}
	else if(strcmp(descUs.fim-> login, novo -> login) < 0) {
		//ultimo
		novo -> ant = descUs.fim;
		descUs.fim -> prox = novo;
		descUs.fim = novo;
	}
	else {
		aux = descUs.inicio -> prox;
		while(aux -> prox != NULL && strcmp(aux -> login,novo -> login) < 0) {
			aux = aux -> prox;
		}
		
		//meio
		novo -> prox = aux;
		novo -> ant = aux -> ant;
		aux -> ant -> prox = novo;
		aux -> ant = novo;
		
	}
	
}

void deletarUsuario(char login[], TpDescUs &descUs) {
	TpUsuario *aux;
	TpMensagem *listaMensagem;
	descUs.qtd--; 
	
	//deletar usuarios do servidor
	aux = descUs.inicio;
	
	while(strcmp(aux -> login,login) != 0) {
		aux = aux -> prox;
	}
	
	listaMensagem = aux -> descMen.inicio;
	
	while(listaMensagem != NULL) {
		deletarMensagem(listaMensagem -> horario, aux -> descMen);
		
		listaMensagem = listaMensagem -> prox;
	}
	
	//deletar usuarios
	if(descUs.qtd == 0) {
		aux = descUs.inicio; 
		descUs.inicio = descUs.fim = NULL;
		delete(aux);
	}
	else if(strcmp(descUs.inicio -> login,login) == 0) {
		//inicio
		aux = descUs.inicio; 
		aux -> prox -> ant = NULL;
		descUs.inicio = aux -> prox;
		delete(aux);
	}
	else if(strcmp(descUs.fim -> login,login) == 0) {
		//fim
		aux = descUs.fim; 
		aux -> ant -> prox = NULL;
		descUs.fim = aux -> ant;
		delete(aux);
	}
	else {
		//meio
		aux = descUs.inicio -> prox; 
		
		while(strcmp(aux -> login,login) != 0) {
			aux = aux -> prox;
		}
		
		aux -> prox -> ant = aux -> ant;
		aux -> ant -> prox = aux -> prox;
		delete(aux);
		
	}
	
}

void alterarUsuario(char login[], TpDescUs &descUs, TpUsuario usuario) {
	TpUsuario *lista = descUs.inicio, *aux;
	TpMensagem *listaMensagem;
	
	//alterar mensagens do usuario
	aux = descUs.inicio;
	
	while(aux != NULL) {
		
		listaMensagem = aux -> descMen.inicio;
		
		while(listaMensagem != NULL) {
			if(strcmp(login,listaMensagem -> remetente) == 0) {
				strcpy(listaMensagem -> remetente,usuario.login);
			}
			if(strcmp(login,listaMensagem -> destinatario) == 0) {
				strcpy(listaMensagem -> destinatario,usuario.login);
			}
			
			listaMensagem = listaMensagem -> prox;
		}
		
		aux = aux -> prox;
	}
	
	//alterar usuario
	lista = descUs.inicio;
	while(strcmp(lista -> login, login) != 0) {
		lista = lista -> prox;
	}
	
	lista -> tipo = usuario.tipo;
	strcpy(lista -> login, usuario.login);
	strcpy(lista -> senha, usuario.senha);
}

// ######################################################################
// #                            MENSAGENS                               #
// ######################################################################

TpMensagem buscarMensagem(TpDescMen descMen) {
	
}
void inserirMensagem(TpMensagem mensagem, TpDescMen &descMen) {
	TpMensagem *novo, *aux;
	
	novo= new TpMensagem();
	novo -> prox = NULL;
	novo -> ant = NULL;
	strcpy(novo -> assunto, mensagem.assunto);
	strcpy(novo -> destinatario,mensagem.destinatario);
	strcpy(novo -> remetente,mensagem.remetente);
	strcpy(novo -> horario,mensagem.horario);
	strcpy(novo -> mensagem,mensagem.mensagem);
	descMen.qtd++;
	
	if(descMen.inicio == NULL) {
		//vazio
		descMen.inicio = descMen.fim = novo;
	}
	else if(strcmp(descMen.inicio -> horario,novo -> horario) > 0) {
		//inicio
		novo -> prox = descMen.inicio;
		descMen.inicio -> ant = novo;
		descMen.inicio = novo;
	}
	else if(strcmp(descMen.fim-> horario, novo -> horario) < 0) {
		//ultimo
		novo -> ant = descMen.fim;
		descMen.fim -> prox = novo;
		descMen.fim = novo;
	}
	else {
		aux = descMen.inicio -> prox;
		while(aux -> prox != NULL && strcmp(aux -> horario,novo -> horario) < 0) {
			aux = aux -> prox;
		}
		
		//meio
		novo -> prox = aux;
		novo -> ant = aux -> ant;
		aux -> ant -> prox = novo;
		aux -> ant = novo;
		
	}
}

void deletarMensagem(char horario[], TpDescMen &descMen) {
	TpMensagem *aux;
	descMen.qtd--; 
	
	if(descMen.qtd == 0) {
		aux = descMen.inicio; 
		descMen.inicio = descMen.fim = NULL;
		delete(aux);
	}
	else if(strcmp(descMen.inicio ->  horario, horario) == 0) {
		//inicio
		aux = descMen.inicio; 
		aux -> prox -> ant = NULL;
		descMen.inicio = aux -> prox;
		delete(aux);
	}
	else if(strcmp(descMen.fim ->  horario, horario) == 0) {
		//fim
		aux = descMen.fim; 
		aux -> ant -> prox = NULL;
		descMen.fim = aux -> ant;
		delete(aux);
	}
	else {
		//meio
		aux = descMen.inicio -> prox; 
		
		while(strcmp(aux -> horario, horario) != 0) {
			aux = aux -> prox;
		}
		
		aux -> prox -> ant = aux -> ant;
		aux -> ant -> prox = aux -> prox;
		delete(aux);
		
	}
}

void alterarMensagem(char horario[], TpDescMen &descMen, TpMensagem mensagem) {
	TpMensagem *lista;
	 
	lista = descMen.inicio;
	while(strcmp(lista -> horario, horario) != 0) {
		lista = lista -> prox;
	}
	
	strcpy(lista -> assunto, mensagem.assunto);
	strcpy(lista -> mensagem, mensagem.mensagem);
}

// ######################################################################
// #                          EXIBIR LISTAS                             #
// ######################################################################

void exibirUsuarios(TpDescUs &descUs) {
	TpUsuario *lista = descUs.inicio;
	while(lista != NULL) {
		printf("%s\n",lista -> login);
		lista = lista -> prox;
	}
}

void exibirServidores(TpDescServ descServ) {
	TpServidor *lista = descServ.inicio;
	while(lista != NULL) {
		printf("%s\n",lista -> dominio);
		exibirUsuarios(lista -> descUs);
		lista = lista -> prox;
	}
	getch();
}

void exibirMensagens(TpDescServ descServ) {
	TpServidor *lista = descServ.inicio;
	TpUsuario *listaUsuario;
	TpMensagem *listaMensagem;
	
	printf("\nmensagens\n");
	while(lista != NULL) {
		printf("\nSERV %s\n", lista -> dominio);
		
		listaUsuario = lista -> descUs.inicio;
		
		while(listaUsuario != NULL) {
			//printf("USUARIO %s\n", listaUsuario -> login);
			
			listaMensagem = listaUsuario -> descMen.inicio;
			
			while(listaMensagem != NULL) {
				printf("%s%s - %s\n%s\npara %s\n", listaMensagem -> horario, listaMensagem -> remetente, listaMensagem -> assunto, listaMensagem ->mensagem, listaMensagem -> destinatario);
				listaMensagem = listaMensagem -> prox;
			}
			
			listaUsuario = listaUsuario -> prox;
		}
		
		lista = lista -> prox;
	}
	getch();
}
