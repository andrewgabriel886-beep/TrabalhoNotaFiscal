#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

/*
- Grupo de configuração da impressora Elgin em C, usando a DLL E1_Impressora01.dll.
 *
- GRUPO: Andrew Gabriel de Assis Moreira RA:260655
				 Kawan Ribeiro  de oliveira Fernandes RA: 078360
				 Leonardo da Silva Alves RA: 194574							
 */

#include <stdio.h>
#include <windows.h>
#include <string.h>

/* TIPOS DAS FUNCOES */

typedef int (*TipoAbrirConexao)(int, const char *, const char *, int);
typedef int (*TipoFecharConexao)(void);
typedef int (*TipoImprimirTexto)(const char *, int, int, int);
typedef int (*TipoCortarPapel)(int);
typedef int (*TipoImprimirQRCode)(const char *, int, int);
typedef int (*TipoImprimirCodigoBarras)(int, const char *, int, int, int);
typedef int (*TipoAvancarPapel)(int);
typedef int (*TipoConsultarStatus)(int);
typedef int (*TipoAbrirGavetaElgin)(int, int, int);
typedef int (*TipoAbrirGaveta)(int, int, int);
typedef int (*TipoEmitirSom)(int, int, int);
typedef int (*TipoAtivarModoPagina)(void);
typedef int (*TipoLimparBufferPagina)(void);
typedef int (*TipoImprimirPagina)(void);
typedef int (*TipoModoPadrao)(void);
typedef int (*TipoPosicaoHorizontal)(int);
typedef int (*TipoPosicaoVertical)(int);
typedef int (*TipoImprimirXMLSAT)(const char *, int);
typedef int (*TipoImprimirCancelamentoSAT)(const char *, const char *, int);


/* PONTEIROS DAS FUNCOES */

static TipoAbrirConexao abrirConexao;
static TipoFecharConexao fecharConexao;
static TipoImprimirTexto imprimirTexto;
static TipoCortarPapel cortarPapel;
static TipoImprimirQRCode imprimirQRCode;
static TipoImprimirCodigoBarras imprimirCodigoBarras;
static TipoAvancarPapel avancarPapel;
static TipoConsultarStatus consultarStatus;
static TipoAbrirGavetaElgin abrirGavetaElgin;
static TipoAbrirGaveta abrirGaveta;
static TipoEmitirSom emitirSom;
static TipoAtivarModoPagina ativarModoPagina;
static TipoLimparBufferPagina limparBufferPagina;
static TipoImprimirPagina imprimirPagina;
static TipoModoPadrao modoPadrao;
static TipoPosicaoHorizontal posicaoHorizontal;
static TipoPosicaoVertical posicaoVertical;
static TipoImprimirXMLSAT imprimirXMLSAT;
static TipoImprimirCancelamentoSAT imprimirCancelamentoSAT;


/* BIBLIOTECA DA IMPRESSORA */

static HMODULE biblioteca = NULL;


/* BUSCA UMA FUNCAO DENTRO DA DLL */

static int buscarFuncao(void *enderecoDestino, const char *nomeDaFuncao)
{
    FARPROC enderecoFuncao = GetProcAddress(biblioteca, nomeDaFuncao);

    if (enderecoFuncao == NULL)
    {
        printf("Funcao nao encontrada: %s\n", nomeDaFuncao);
        return 0;
    }

    memcpy(enderecoDestino, &enderecoFuncao, sizeof(enderecoFuncao));

    return 1;
}


/* CARREGA A BIBLIOTECA E SUAS FUNCOES */

static int carregarBiblioteca(void)
{
    biblioteca = LoadLibraryA("E1_Impressora01.dll");

    if (biblioteca == NULL)
    {
        printf("Erro ao carregar a DLL: %lu\n", GetLastError());
        printf("Verifique se a DLL esta na pasta do projeto e se possui a arquitetura correta.\n");

        return 0;
    }

    int todasFuncoesCarregadas = 1;

    todasFuncoesCarregadas &= buscarFuncao(&abrirConexao, "AbreConexaoImpressora");
    todasFuncoesCarregadas &= buscarFuncao(&fecharConexao, "FechaConexaoImpressora");
    todasFuncoesCarregadas &= buscarFuncao(&imprimirTexto, "ImpressaoTexto");
    todasFuncoesCarregadas &= buscarFuncao(&cortarPapel, "Corte");
    todasFuncoesCarregadas &= buscarFuncao(&imprimirQRCode, "ImpressaoQRCode");
    todasFuncoesCarregadas &= buscarFuncao(&imprimirCodigoBarras, "ImpressaoCodigoBarras");
    todasFuncoesCarregadas &= buscarFuncao(&avancarPapel, "AvancaPapel");
    todasFuncoesCarregadas &= buscarFuncao(&consultarStatus, "StatusImpressora");
    todasFuncoesCarregadas &= buscarFuncao(&abrirGavetaElgin, "AbreGavetaElgin");
    todasFuncoesCarregadas &= buscarFuncao(&abrirGaveta, "AbreGaveta");
    todasFuncoesCarregadas &= buscarFuncao(&emitirSom, "SinalSonoro");
    todasFuncoesCarregadas &= buscarFuncao(&ativarModoPagina, "ModoPagina");
    todasFuncoesCarregadas &= buscarFuncao(&limparBufferPagina, "LimpaBufferModoPagina");
    todasFuncoesCarregadas &= buscarFuncao(&imprimirPagina, "ImprimeModoPagina");
    todasFuncoesCarregadas &= buscarFuncao(&modoPadrao, "ModoPadrao");
    todasFuncoesCarregadas &= buscarFuncao(&posicaoHorizontal, "PosicaoImpressaoHorizontal");
    todasFuncoesCarregadas &= buscarFuncao(&posicaoVertical, "PosicaoImpressaoVertical");
    todasFuncoesCarregadas &= buscarFuncao(&imprimirXMLSAT, "ImprimeXMLSAT");
    todasFuncoesCarregadas &= buscarFuncao(&imprimirCancelamentoSAT, "ImprimeXMLCancelamentoSAT");

    if (todasFuncoesCarregadas == 0)
    {
        FreeLibrary(biblioteca);
        biblioteca = NULL;

        return 0;
    }

    return 1;
}


/* CONFIGURACOES DA IMPRESSORA */

static int impressoraConectada = 0;
static int tipoConexao = 1;

static char modeloImpressora[50] = "i9";
static char portaConexao[50] = "USB";

static int configuracao = 0;


/* CHAVE QR CODE DO CANCELAMENTO SAT */

static const char *CHAVE_QRCODE =
    "Q5DLkpdRijIRGY6YSSNsTWK1TztHL1vD0V1Jc4spo/CEUqICEb9SFy82ym8EhBRZjbh3btsZhF+sjHqEMR159i4agru9x6KsepK/"
    "q0E2e5xlU5cv3m1woYfgHyOkWDNcSdMsS6bBh2Bpq6s89yJ9Q6qh/J8YHi306ce9Tqb/drKvN2XdE5noRSS32TAWuaQEVd7u+"
    "TrvXlOQsE3fHR1D5f1saUwQLPSdIv01NF6Ny7jZwjCwv1uNDgGZONJdlTJ6p0ccqnZvuE70aHOI09elpjEO6Cd+orI7XHHrFCwhFhAcbalc+"
    "ZfO5b/+vkyAHS6CYVFCDtYR9Hi5qgdk31v23w==";


/* LE TEXTO DIGITADO PELO USUARIO */

static void capturarEntrada(const char *mensagemUsuario,
                            char *textoDigitado,
                            int limiteCaracteres)
{
    printf("%s", mensagemUsuario);

    if (fgets(textoDigitado, limiteCaracteres, stdin) != NULL)
    {
        textoDigitado[strcspn(textoDigitado, "\n")] = '\0';
    }
}
