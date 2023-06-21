#include <stdio.h>
#include <time.h>
#include <nds.h>
#include <maxmod9.h>

#include "soundbank.h"
#include "soundbank_bin.h"


// Includes librerias propias
#include <nf_lib.h>

// Define numero de paletas

#define PJPAL               0
#define MDINEROPIEZASPAL    1
#define MARMASTORRETAPAL    2
#define ENEMIGOPAL          3

//------------------------------------------------
//          DECLARACIÓN DE FUNCIONES
//------------------------------------------------

void ComprobarEstadoPieza(int);                 // Función que comprueba el estado de la pieza de la maquina de piezas seleccionada
void VendePieza(int);                           // Función que vende una pieza de la maquina de piezas seleccionada
void ComprarTorreta(int);                       // Función que crea una torreta en la maquina de armamento seleccionada
void CreaPieza(int);                            // Función que crea una pieza en la maquina de piezas seleccionada
void MejoraMaquina(int);                        // Función que mejora la maquina de piezas seleccionada al siguiente nivel
void ConfigurarInterrupciones();                // Configura las interrupciones
void Controles();                               // Función que comprueba los inputs y delimita el movimiento del jugador
void InicializarPartida();                      // Carga los datos necesarios en las estructuras de datos para inciar la partida
void Timer();                                   // Función que se ejecuta cada vez que se produce una interrupción del timer
void DeteccionMaquinaJugador();                 // Detección de la maquina que se ha tocado
void MostrarMenu();                             // Muestra el menu de la pantalla inferior para usar el panel tactil
void ComprobarTactil();                         // Comprueba si se ha tocado la pantalla tactil y donde lo ha hecho
void HUD();                                     // Muestra la interfaz de usuario
void CargarYDescargarMapa(u8, u8);              // Carga y descarga los sprites de la pantalla indicada
double GetRandNumber(double, double);           // Devuelve un número aleatorio entre los dos valores indicados
void ColocarTorreta(int);                       // Coloca la torreta en la posición indicada
void CargarPaletas();                           // Carga las paletas de la FAT a la RAM
void CargarSpritesMA();                         // Carga los sprites de las maquinas de armamento
void CargarSpritesMD();                         // Carga los sprites de las maquinas de dinero de la FAT a la RAM
void CargarSpritesMP();                         // Carga los sprites de las maquinas de piezas de la FAT a la RAM
void CargarSpritesPersonaje();                  // Carga los sprites de las torretas de la FAT a la RAM
void CargarSpritesProblematicos();              // Carga los sprites de las torretas de la FAT a la RAM (Lo hacemos en varias veces porque por alguna razon da problemas)
void CargarFondos();                            // Carga los fondos de la FAT a la RAM
void IniciarFondos();                           // Inicia los fondos
void CargarVram();                              // Carga los sprites y fondos a la VRAM
void CrearSprites();                            // Crea los sprites de las maquinas de armas
void RepararTorreta(int);                       // Función que repara la torreta seleccionada
void DestruirTorreta(int);                      // Función que destruye la torreta seleccionada
void GenerarEnemigos();                         // Función que genera enemigos de forma pseudoaleatoria
void ControlEnemigos();                         // Función que controla el comportamiento de los enemigos
void MoverEnemigos();                           // Función que mueve los enemigos en la pantalla
void AtaqueEnemigos(int);                       // Función que controla el ataque de los enemigos y su animacion
void MuerteEnemigo(int);                        // Funcion que se encarga de matar a los enemigos
void CambiarDePantalla(int);                    // Funcion que se encarga de pasar los enemigos de una pantalla a otra de forma fluida
void ControlProyectiles();                      // Función que controla el comportamiento de los proyectiles
void DisparoTorreta();                          // Función que controla el disparo de las torretas
void MejorarTorretas(int);                      // Función que aplica una mejora temporal a las torretas
void CargarMenuPrincipal();                     // Carga el menu principal
void ComprobarCondiciones();                    // Se comprueba si se ha ganado o perdido la partida
void CargarPantallaFinal();                     // Carga la pantalla de final

//------------------------------------------------
//          DECLARACIÓN DE STRUCTS
//------------------------------------------------

typedef struct 
{
	float posX, posY;               // Posición del personaje
	u8 id;                          // Identificador del personaje
	u8 direccion;                   // Dirección del personaje
	u8 anim_frame;                  // Frame de la animación
	int fabrica;                    // Indica la fabrica que donde se encuentra el personaje
	int dinero;                     // Dinero que tiene el personaje
	int armamento[5];               // En este array se almacena la cantidad de armamento que se tiene --> del tipo 0 se tiene Armamento[0], del tipo 1 se tiene Armamento[1] ...
	int piezas[6];                  // En este array se almacena la cantidad de piezas que se tiene --> del tipo 0 se tiene pieza[0], del tipo 1 se tiene pieza[1] ...
	u8 daño;                        // Daño que hace el personaje
    int maquina;                    // Indica la maquina que se ha seleccionado
    int salud;                      // Indica la salud del personaje
    u8 pantalla;                    // Indica la pantalla en la que se encuentra el personaje
    int linea;                      // Indica la columna de maquinas en la que se encuentra el personaje
    bool atacando;                  // Indica si el personaje esta atacando
    bool sonido;                    // Indica si el personaje aun no ha hecho el sonido de ataque
    int contAtaque;                 // Contador que se usa para controlar cada cuanto ataca del personaje (se sua por errores de audio)
} Personaje;

typedef struct 
{
	float posX, posY;               // Posición de la maquina 
	u8 id;                          // Identificador de la maquina
	u8 anim_frame;                  // Frame de la animación
	int costePieza;                 // En este array se almacena el beneficio de vender x pieza
} MaquinaDinero;

typedef struct 
{
	float posX, posY;               // Posición de la maquina
	u8 id;                          // Identificador de la maquina
	u8 anim_frame;                  // Frame de la animación
	u8 state;                       // Estado de la maquina (rota, medio rota, sana)
	u8 salud;                       // Indica la salud de la fabrica
	int tiempoPieza;                // Almacena el tiempo necesario para hacer una pieza
    float tiempoEmpiece;            // Almacena el tiempo en el que se ha ordenado producir la pieza
    int costeMejora;                // Almacena el coste de mejorar la fabrica
    int nivel;                      // Indica si la fabrica esta sin comprar (valor 0) o si esta comprada y su nivel 

} MaquinaPiezas;

typedef struct 
{
	float posX, posY;               // Posición de la maquina
	u8 id;                          // Identificador de la maquina
	u8 anim_frame;                  // Frame de la animación
	int costeArmamento;           	// Se almacena el costa de producir x arma
    bool desbloqueadoArmamento;     // Indica si la fabrica esta sin comprar (valor 0), o si esta comprada y lo que hay que comprar es munición.
} MaquinaArmas;

typedef struct 
{
    float posX, posY;               // Posición de la torreta
    u8 id;                          // Identificador de la torreta
    u8 anim_frame;                  // Frame de la animación
    int salud;                      // Indica la salud de la torreta
    int daño;                       // Indica el daño que hace la torreta
    int tipo;                       // Indica el tipo de torreta
    float velocidadAtaque;           // Indica la velocidad de ataque de la torreta
    int reparacion;                 // Indica el coste de reparación de la torreta
} Torreta;

typedef struct
{
    float posX, posY;               // Posición del proyectil
    u8 id;                          // Identificador del proyectil
    int daño;                       // Daño del proyectil
    bool disparado;                 // Indica si el proyectil ha sido disparado
} Proyectil;

//------------------------------------------------
//          DECLARACIÓN DE VARIABLES
//------------------------------------------------

MaquinaDinero maquinasDinero[6];                                                    // Array de maquinas de dinero para poder hacer referencia a ellas
MaquinaPiezas maquinasPiezas[6];                                                    // Array de maquinas de piezas para poder hacer referencia a ellas
MaquinaArmas maquinasArmas[6];                                                      // Array de maquinas de armamento para poder hacer referencia a ellas
Torreta torretas[6];                                                                // Array de torretas para poder hacer referencia a ellas
Torreta torretasGenericas[5];                                                       // Array de torretas genericas que se usan como molde para los datos de las que se colocan durante la partida
Personaje enemigos[15];                                                             // Array de enemigos para poder hacer referencia a ellos
Proyectil proyectiles[20];                                                          // Array de proyectiles para poder hacer referencia a ellos
float cooldown[6] = {-1, -1, -1, -1, -1, -1};                                       // Indica el cooldown de cada torreta incial
bool existeTorreta[6] = {false, false, false, false, false, false};                 // Indica si existe una torreta en la posicion del array
int posicionMaquinasDineroX[6] = {64, 64, 64, 80, 112, 144};                        // Posicion en x de las maquinas de dinero
int posicionMaquinasDineroY[6] = {112, 80, 48, 16, 16, 16};                         // Posicion en y de las maquinas de dinero
int posicionMaquinasArmasX[6] = {16, 48, 80, 112, 144, 176};                        // Posicion en x de las maquinas de armamento
int posicionMaquinasArmasY[6] = {16, 16, 16, 16, 16, 16};                           // Posicion en y de las maquinas de armamento
int costePieza[6] = {10, 35, 130, 350, 750, 1000};                                  // Precio de venta de las piezas
int timepoPieza[6] = {1, 4, 10, 15, 30, 60};                                        // Tiempo que tarda en hacerse una pieza
int costeArmamento[6] = {20, 60, 120, 350, 800, 1200};                              // Coste de comprar una torreta
int costeMejora[6] = {10, 40, 130, 300, 500, 750};                                  // Coste de mejorar una maquina de piezas
bool lineaDisponible[6];                                                            // Indica si la columna esta disponible para colocar enemigos
int cantidadEnemigos = 1;                                                           // Indica la cantidad de enemigos que hay en la pantalla
int enemigosLinea[6];                                                               // Indica la cantidad de enemigos que hay en cada columna
Personaje pj = {90, 96, .id = 0, .fabrica = 0, .daño = 0};                          // Datos del personaje
float tiempoActual;                                                                 // Almacena el tiempo que ha transcurrido desde que se ha iniciado el juego                                                               
bool moving = false;                                                                // Indica si el personaje se esta moviendo
bool mostrandoMenu = false;                                                         // Indica si se esta mostrando el menu de la pantalla inferior
touchPosition posicionXY;                                                           // Almacena la posicion de la pantalla tactil
int cantidadVenta = 1;                                                              // Indica la cantidad de piezas que se van a vender
int tiempoActivacionDaño = -1;                                                      // Tiempo de activacion del boost de daño
int tiempoActivacionVAtq = -1;                                                      // Tiempo de activacion del boost de velocidad de ataque
int faseJuego = 0;                                                                  // Indica el estado del juego 0, 1 y 2 = menuPrincipal, 3 = enPartida,  = menuFinPartida
int tiempoJuego = 0;                                                                // Indica el tiempo que lleva la partida
bool empezarTimer = false;                                                          // Indica cuando el timer de los enemigos debe comenzar
int cantadorSonbido;                                                                // Retrasa la reproduccion de sonidos entre paso y paso porque en el timer crashea
bool reset;                                                                         // Indica si se ha reiniciado la cntidad de enemigos para dar un respiro al jugador
int cantEnemMax;                                                                    // Indica la cantidad maxima de enemigos que hay en la pantalla
float tiempoEnemigos;                                                               // Contador del timer de spawn de enemigos
bool primeraVez = true;                                                             // Indica si es la primera vez que se entra en el juego

//-----------------------------------------------------------------------------------------------------------------------------------
// A partir de aqui solo son chars que se usan para poder mostrar datos mediante el motor de texto de ambas pantallas  
//-----------------------------------------------------------------------------------------------------------------------------------

char nivel[20];                                                                      
char tiempo[20];                                                                    
char costeDeMejora[20];
char salud[20];
char cantidadAVender[20];
char ganancia[20];
char costePorPieza[20];
char pieza1[20];
char pieza2[20];
char pieza3[20];
char pieza4[20];
char pieza5[20];
char pieza6[20];
char dineros[200];
char reparar[20];
char comprar[20];
char saludTorreta[20];
char dañoTorreta[20];
char velocidadAtaqueTorreta[20];
char cantidadTorretas1[5];
char cantidadTorretas2[5];
char cantidadTorretas3[5];
char cantidadTorretas4[5];
char cantidadTorretas5[5];
char tiempoRestante[20];

//--------------------------------------------
//            Programa principal
//--------------------------------------------

int main(int argc, char **argv) 
{   
	// Pantalla de espera inicializando NitroFS
	NF_Set2D(0, 0);
	NF_Set2D(1, 0);	
	consoleDemoInit();

	// Define el ROOT e inicializa el sistema de archivos
	NF_SetRootFolder("NITROFS");	// Define la carpeta ROOT para usar NITROFS

	// Inicializa el motor 2D
	NF_Set2D(0, 0);				    // Modo 2D_0 enla pantalla superior
	NF_Set2D(1, 0);                 // Modo 2D_0 en la pantalla inferior

    // Inicializamos MAXMOD 
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // Bloqueamos canales de sonido para libnds
    mmLockChannels(BIT(0) | BIT(1) | BIT(2) | BIT (4) | BIT (5) | BIT (6) | BIT (7) | BIT (8) | BIT (9) | BIT (10));

    // Inicializa el API de sonidos de la libreria
    soundEnable();
    
    // Cargamos los sonidos .mod
    mmLoad( MOD_PRINCIPAL );
    mmLoad( MOD_JUEGO );

    // Inicializa el buffer de los sonidos
	NF_InitRawSoundBuffers();

    // Carga los archivos de sonido desde la FAT / NitroFS a la RAM
	// NF_LoadRawSound(File, ID, freq, format)
    NF_LoadRawSound("sounds/pulsarBoton", 0,  22050, 0);    // Cargar sonido de Pulsar Boton
    NF_LoadRawSound("sounds/error", 1,  22050, 0);          // Cargar sonido de No Puedes Pulsar Boton
    NF_LoadRawSound("sounds/andar", 2,  22050, 0);          // Cargar sonido de Andar
    NF_LoadRawSound("sounds/producirPieza", 3,  22050, 0);  // Cargar sonido de Producir Pieza
    NF_LoadRawSound("sounds/disparo", 4,  22050, 0);        // Cargar sonido de Disparo
    NF_LoadRawSound("sounds/torreta", 5,  22050, 0);        // Cargar sonido de Colocar Torreta
    NF_LoadRawSound("sounds/romper", 6,  22050, 0);         // Cargar sonido de Romper y ataque enemigos
    NF_LoadRawSound("sounds/comprar", 7,  22050, 0);        // Cargar sonido de Comprar
    NF_LoadRawSound("sounds/perder", 8,  22050, 0);         // Cargar sonido de Derrota
    NF_LoadRawSound("sounds/victoria", 9,  22050, 0);       // Cargar sonido de Victoria

	// Inicializa los fondos tileados
	NF_InitTiledBgBuffers();	    // Inicializa los buffers para almacenar fondos
	NF_InitTiledBgSys(0);		    // Inicializa los fondos Tileados para la pantalla superior
	NF_InitTiledBgSys(1);		    // Iniciliaza los fondos Tileados para la pantalla inferior

	// Inicializa los Sprites
	NF_InitSpriteBuffers();		    // Inicializa los buffers para almacenar sprites y paletas
	NF_InitSpriteSys(0, 128);		// Inicializa los sprites para la pantalla superior
	NF_InitSpriteSys(1, 128);		// Inicializa los sprites para la pantalla inferior

    // Inicializa los cuadros de texto
    NF_InitTextSys(1);  // Inicializa el sistema de Texto para la pantalla inferior
    NF_InitTextSys(0);  // Inicializa el sistema de Texto para la pantalla inferior

    // Carga una fuente para usar como texto
    NF_LoadTextFont("fonts/default", "fuente", 256, 256, 0);
    NF_LoadTextFont("fonts/default", "fuente2", 256, 256, 0);

    // Crea un fondo tileado para usarlo con texto en ambas pantallas
    NF_CreateTextLayer(1, 0, 0, "fuente");  // Se usan 2 fuentes iguales pero en diferentes variables para que no se duplique el texto
    NF_CreateTextLayer(0, 0, 0, "fuente2");

    // Define el color negro para el texto, en formato RGB
    NF_DefineTextColor(1, 0, 0, 0, 0 ,0);
    NF_DefineTextColor(0, 0, 0, 0, 0 ,0);

    // Aplica el color creado justo antes al texto
    NF_SetTextColor(1, 0, 0);

    // Inicializa el contador del primer temporizador por interrupción encargado de atrasar la carga de los sprites problemáticos
    irqEnable(IRQ_TIMER0);
    irqSet(IRQ_TIMER0, CargarSpritesProblematicos);
    TIMER_DATA(0)= 32000;       // 1 segundo
    TIMER_CR(0) = TIMER_DIV_1024 | TIMER_ENABLE | TIMER_IRQ_REQ;

    CargarMenuPrincipal();      // Carga el menu principal

    ConfigurarInterrupciones(); // Configura las interrupciones
    InicializarPartida();       // Inicializa los datos necesarios para que se inicie la partida
    
    // Estas funciones se encargan de copiar de la FAT a la RAM y de la RAM a la VRAM lo necesario para que el juego funcione
    // el orden en el que se ejecutan no es importante, pero si se ejecutan todas seguidas por razones que desconocemos el juego crashea
    // para solucionarlo, se ejecutan ahora unas pocas, se espera unos segundos y se ejecutan el resto

    CargarPaletas();            // Carga las paletas de los fondos y sprites a la RAM
    CargarFondos();             // Carga los fondos de ambas pantallas a la RAM
    CargarSpritesMA();          // Carga los sprites de las maquinas de armas a la RAM
    CargarSpritesMD();          // Carga los sprites de las maquinas de dinero a la RAM
    CargarSpritesMP();          // Carga los sprites de las maquinas de piezas a la RAM
    CargarSpritesPersonaje();   // Carga los sprites de las torretas a la RAM
    IniciarFondos();            // Inicia los fondos de ambas pantallas
    CargarVram();               // Carga los fondos y sprites en la VRAM
    CrearSprites();             // Crea los sprites de las maquinas necesarias para el inicio de la partida;


    // Bucle principal del juego (esta separado en fases de inicio, juego y final)
	while (1)
	{
        if(faseJuego == 0)
        {
            CargarMenuPrincipal();      // Carga el menu principal
            ConfigurarInterrupciones(); // Configura las interrupciones
            InicializarPartida();       // Inicializa los datos necesarios para que se inicie la partida
            IniciarFondos();            // Inicia los fondos de ambas pantallas
            CargarVram();               // Carga los fondos y sprites en la VRAM
            CrearSprites();             // Crea los sprites de las maquinas necesarias para el inicio de la partida
        }
        if(faseJuego == 3)
        {
            Controles();                // Inicializa los controles 
            DeteccionMaquinaJugador();  // Comprueba la máquina en la que está el jugador
            MostrarMenu();              // Función para mostrar el menú en caso de que sea necesario       
            ComprobarTactil();          // Comprueba la posición de la pantalla táctil para la interacción en el juego
            HUD();                      // Interfaz de Usuario para ver las piezas y dinero disponible 
            MoverEnemigos();            // Controla el movimiento de los enemigos
            ControlEnemigos();          // Controla el estado de enemigos
            ControlProyectiles();       // Controla el estado de proyectiles
            ComprobarCondiciones();     // Comprueba si has ganado o no 
        }
		else if(faseJuego == 4 || faseJuego == 5) // Victoria o  derrota
        {
            CargarPantallaFinal();      // Carga la pantalla de fin de juego
        }
        
		// Actualiza el array de OAM
		NF_SpriteOamSet(0);
		NF_SpriteOamSet(1);
        NF_UpdateTextLayers();

		swiWaitForVBlank();		        // Espera al sincronismo vertical

		// Actualiza el OAM de ambas pantallas
		oamUpdate(&oamMain);
		oamUpdate(&oamSub);
	}
	return 0; 
}

//------------------------------------------------
//          FUNCIONES DEL PROGRAMA
//------------------------------------------------

void CargarMenuPrincipal() // Carga los fondos y textos del menú principal y los tutoriales
{
    
    NF_ClearTextLayer(0, 0);
    NF_ClearTextLayer(1, 0);
    NF_LoadTiledBg("bg/menuPrincipal", "menuPrincipal", 256, 256);
    NF_LoadTiledBg("bg/manualMenu", "menuManual", 256, 256);
    NF_CreateTiledBg(0, 3, "menuPrincipal");
    NF_CreateTiledBg(1, 3, "menuManual");
    NF_WriteText(1, 0, 8, 20, "Comenzar partida");
    NF_WriteText(1, 0, 3, 9, "Hecho por:");
    NF_WriteText(1, 0, 3, 11, "Edward Mena Ordonez");
    NF_WriteText(1, 0, 3, 13, "Mario Beltran Garcia");
    NF_WriteText(1, 0, 3, 15, "Alvaro Chuan Diaz-Maroto");
    NF_UpdateTextLayers();
    
    if(primeraVez)
    { 
        mmStart(MOD_PRINCIPAL, MM_PLAY_LOOP);
        mmSetModuleVolume(100);
        primeraVez = false;
    }
    while(faseJuego == 0)   // Mientras no se inicie la partida se muestra el menu principal
    {
        scanKeys();
        ComprobarTactil();
    }
    NF_ClearTextLayer(1,0);
    NF_UpdateTextLayers();
    NF_DeleteTiledBg(1,3);
    NF_UnloadTiledBg("menuManual");
    NF_LoadTiledBg("bg/menuTutorial", "menuTutorial", 256, 256);
    NF_CreateTiledBg(1, 3, "menuTutorial");
    while(faseJuego == 1)   // Mientras no se vuelva a presionar la pantalla se seguirá mostrando el primer tutorial
    {
        scanKeys();
        ComprobarTactil();
    }
    NF_ClearTextLayer(1,0);
    NF_UpdateTextLayers();
    NF_DeleteTiledBg(1,3);
    NF_UnloadTiledBg("menuTutorial");
    NF_LoadTiledBg("bg/menuTutorial2", "menuTutorial2", 256, 256);
    NF_CreateTiledBg(1, 3, "menuTutorial2");
    while(faseJuego == 2)  // Mientras no se vuelva a presionar la pantalla se seguirá mostrando el segundo tutorial
    {
        scanKeys();
        ComprobarTactil();
    }
    NF_DeleteTiledBg(1,3);
    NF_DeleteTiledBg(0,3);
    NF_UnloadTiledBg("menuPrincipal");
    NF_UnloadTiledBg("menuTutorial2");
    NF_ClearTextLayer(1, 0);
    NF_UpdateTextLayers();  
    mmStart(MOD_JUEGO, MM_PLAY_LOOP);
    
}

void CargarPaletas()    // Pasa las paletas de la FAT a la RAM
{
	NF_LoadSpritePal("sprite/pj1", PJPAL);
    NF_LoadSpritePal("sprite/mdp", MDINEROPIEZASPAL);
    NF_LoadSpritePal("sprite/ma", MARMASTORRETAPAL);
}

void CargarSpritesMA() // Pasa los sprites de la FAT a la VRAM
{
    NF_LoadSpriteGfx("sprite/ma1", maquinasArmas[0].id, 16, 32);    // Maquinas Armas  
    NF_LoadSpriteGfx("sprite/ma2", maquinasArmas[1].id, 16, 32);
    NF_LoadSpriteGfx("sprite/ma3", maquinasArmas[2].id, 16, 32);
    NF_LoadSpriteGfx("sprite/ma4", maquinasArmas[3].id, 16, 32);
    NF_LoadSpriteGfx("sprite/ma5", maquinasArmas[4].id, 16, 32);
    NF_LoadSpriteGfx("sprite/ma6", maquinasArmas[5].id, 16, 32);
}

void CargarSpritesMD() // Pasa los sprites de la FAT a la VRAM
{
    NF_LoadSpriteGfx("sprite/md1", maquinasDinero[0].id, 16, 32);	// Maquinas Dinero
    NF_LoadSpriteGfx("sprite/md2", maquinasDinero[1].id, 16, 32);
    NF_LoadSpriteGfx("sprite/md3", maquinasDinero[2].id, 16, 32);
    NF_LoadSpriteGfx("sprite/md4", maquinasDinero[3].id, 16, 32);
    NF_LoadSpriteGfx("sprite/md5", maquinasDinero[4].id, 16, 32);
    NF_LoadSpriteGfx("sprite/md6", maquinasDinero[5].id, 16, 32);
}

void CargarSpritesMP() // Pasa los sprites de la FAT a la VRAM
{
    NF_LoadSpriteGfx("sprite/mp1", maquinasPiezas[0].id, 16, 32);	// Maquinas Piezas
    NF_LoadSpriteGfx("sprite/mp2", maquinasPiezas[1].id, 16, 32);
    NF_LoadSpriteGfx("sprite/mp3", maquinasPiezas[2].id, 16, 32);
    NF_LoadSpriteGfx("sprite/mp4", maquinasPiezas[3].id, 16, 32);
    NF_LoadSpriteGfx("sprite/mp5", maquinasPiezas[4].id, 16, 32);
    NF_LoadSpriteGfx("sprite/mp6", maquinasPiezas[5].id, 16, 32);
}

void CargarSpritesPersonaje() // Carga los sprites del personaje a la RAM
{
    NF_LoadSpriteGfx("sprite/pj1", pj.id, 16, 32);
}

void CargarSpritesProblematicos() // Esta funcion se ejecuta con retraso ya que por alguna razon si hacemos muchas operaciones de carga sin pausas explota
{
    NF_LoadSpriteGfx("sprite/t1", 20, 16, 32);
    NF_LoadSpriteGfx("sprite/t2", 21, 16, 32);
    NF_LoadSpriteGfx("sprite/t3", 22, 16, 32);
    NF_LoadSpriteGfx("sprite/t4", 23, 16, 32);
    NF_LoadSpriteGfx("sprite/t5", 24, 16, 32);
    NF_LoadSpriteGfx("sprite/enemigo", 30, 16, 32);
    NF_LoadSpriteGfx("sprite/bala", 32, 16, 32);
    NF_LoadSpritePal("sprite/enemigo", ENEMIGOPAL);
    NF_VramSpritePal(0, ENEMIGOPAL, ENEMIGOPAL);
    NF_VramSpritePal(1, ENEMIGOPAL, ENEMIGOPAL);
    irqDisable(IRQ_TIMER0);
}


void CargarFondos() //Carga los fondos
{
    NF_LoadTiledBg("bg/fp", "fp", 256, 256);			            // Carga el fondo de la fabrica principal para la pantalla superior
	NF_LoadTiledBg("bg/fd", "fd", 256, 256);			            // Carga el fondo de la fabrica de dinero para la pantalla superior
	NF_LoadTiledBg("bg/fa", "fa", 256, 256);			            // Carga el fondo de la fabrica de armas para la pantalla superior
	NF_LoadTiledBg("bg/fe", "fe", 256, 256);			            // Carga el fondo de la fabrica del enemigo para la pantalla inferior
    NF_LoadTiledBg("bg/manualMenu", "menuManual", 256, 256);	    // Carga el fondo del menu de manual para la pantalla inferior
    NF_LoadTiledBg("bg/manualTorreta", "menuTorreta", 256, 256);	// Carga el fondo del menu de manual para la pantalla inferior
    NF_LoadTiledBg("bg/comprar", "menuTorreta2", 256, 256);	        // Carga el fondo del menu de manual para la pantalla inferior
}


void IniciarFondos() // Inicia los fondos
{
    // Crea los fondos de la pantalla superior
	NF_CreateTiledBg(0, 1, "fp");	// Crea el fondo de la fabrica principal

	// Crea los fondos de la pantalla inferior
	NF_CreateTiledBg(1, 3, "fe");	// Crea el fondo de la fabrica enemiga
}

void CargarVram() // Pasa los sprites y las paletas de la RAM a la VRAM
{
    NF_VramSpriteGfx(0, pj.id, pj.id, false);	                                                                                // Personaje, copia todos los frames a la VRAM
    
    for (int i = 0; i < 6; i++)
    {
        NF_VramSpriteGfx(0, maquinasPiezas[i].id,   maquinasPiezas[i].id, false);	                                            // Maquina piezas 1, copia todos los frames a la VRAM
        NF_VramSpriteGfx(0, maquinasArmas[i].id,   maquinasArmas[i].id, false);	                                                // Maquina piezas 2, copia todos los frames a la VRAM
        NF_VramSpriteGfx(0, maquinasDinero[i].id,   maquinasDinero[i].id, false);	                                            // Maquina piezas 3, copia todos los frames a la VRAM
    }

    if (!NF_SPRPALSLOT[0][PJPAL].inuse)NF_VramSpritePal(0, PJPAL, PJPAL);                                                       // Carga la paleta del personaje solo si no esta ya cargada
    if (!NF_SPRPALSLOT[0][MDINEROPIEZASPAL].inuse)NF_VramSpritePal(0, MDINEROPIEZASPAL,   MDINEROPIEZASPAL);                    // Carga la paleta de las maquinas de piezas solo si no esta ya cargada
    if (!NF_SPRPALSLOT[0][MARMASTORRETAPAL].inuse)NF_VramSpritePal(0, MARMASTORRETAPAL,   MARMASTORRETAPAL);                    // Carga la paleta de las maquinas de armas solo si no esta ya cargada

    for (int i = 0; i<15; i++)
    {
        NF_VramSpriteGfx(0, 30, enemigos[i].id, false);	                                                                        // Enemigos, copia todos los frames a la VRAM
        NF_CreateSprite(0, enemigos[i].id, enemigos[i].id, ENEMIGOPAL, enemigos[i].posX, enemigos[i].posY);	                    // Crea el sprite de la maquina de piezas 1
        NF_ShowSprite(0, enemigos[i].id, false);
        NF_VramSpriteGfx(0, 32, proyectiles[i].id, false);	                                                                    // Proyectiles, copia todos los frames a la VRAM
        NF_CreateSprite(0, proyectiles[i].id, proyectiles[i].id, MARMASTORRETAPAL, proyectiles[i].posX, proyectiles[i].posY);	// Crea el sprite de la maquina de piezas 1
        NF_ShowSprite(0, proyectiles[i].id, false);
    }

    for (int i = 15; i<20; i++)
    {
        NF_VramSpriteGfx(0, 32, proyectiles[i].id, false);	                                                                    // Proyectiles, copia todos los frames a la VRAM
        NF_CreateSprite(0, proyectiles[i].id, proyectiles[i].id, MARMASTORRETAPAL, proyectiles[i].posX, proyectiles[i].posY);	// Crea el sprite de la maquina de piezas 1
        NF_ShowSprite(0, proyectiles[i].id, false);
    }
}

void CrearSprites() // Crea los sprites en pantalla
{
    // 0 = pantalla, 1 = id del sprite, 2 = id de la imagen, 3 = id de la paleta, 4 = posicion X, 5 = posicion Y
    NF_CreateSprite(0, pj.id, pj.id, PJPAL, pj.posX, pj.posY);	                                                                            // Crea el sprite del personaje
    for (int i = 0; i < 6; i++)
    {
        NF_CreateSprite(0, maquinasPiezas[i].id, maquinasPiezas[i].id, MDINEROPIEZASPAL, maquinasPiezas[i].posX, maquinasPiezas[i].posY);	// Crea el sprite de la maquina de piezas 1
        NF_CreateSprite(0, maquinasArmas[i].id, maquinasArmas[i].id, MARMASTORRETAPAL, maquinasArmas[i].posX, maquinasArmas[i].posY);	    // Crea el sprite de la maquina de piezas 2
        NF_ShowSprite(0, maquinasArmas[i].id, false);                                                                                       // Oculta el sprite de la maquina de armas
        NF_CreateSprite(0, maquinasDinero[i].id, maquinasDinero[i].id, MDINEROPIEZASPAL, maquinasDinero[i].posX, maquinasDinero[i].posY);	// Crea el sprite de la maquina de piezas 3
        NF_ShowSprite(0, maquinasDinero[i].id, false);                                                                                      // Oculta el sprite de la maquina de dinero
    }	
}

void ConfigurarInterrupciones() // Configura las interrupciones de los timers
{
    irqEnable(IRQ_TIMER1);
    irqSet(IRQ_TIMER1, Timer); 
    irqEnable(IRQ_TIMER2);
    irqSet(IRQ_TIMER2, DisparoTorreta); 

    TIMER_DATA(1) = 62259; // 1/60 segundos
    TIMER_CR(1) = TIMER_DIV_1024 | TIMER_ENABLE | TIMER_IRQ_REQ;
    TIMER_DATA(2) = 62259; // 1/60 segundos
    TIMER_CR(2) = TIMER_DIV_1024 | TIMER_ENABLE | TIMER_IRQ_REQ;
}


void InicializarPartida() // Inicializa la partida
{
    for (int i = 0; i < 6 ; i++) // Carga los datos necesarios para el funcionamiento de la partida en las estructuras de datos
    {
	    MaquinaDinero MaquiDin = {posicionMaquinasDineroX[i], posicionMaquinasDineroY[i], .id = i + 1, .costePieza = costePieza[i]};
	    MaquinaArmas MaquiArma = {posicionMaquinasArmasX[i], posicionMaquinasArmasY[i], .id = i + 7, .costeArmamento = costeArmamento[i]};
	    MaquinaPiezas MaquiPiez = {32*(i + 1), 16, .id = i+13, .salud = 3, .tiempoPieza =  timepoPieza[i], .tiempoEmpiece = -1, .costeMejora = costeMejora[i]};
        Torreta Torretilla = {32*(i + 1), 64, .id = i + 19, .salud = 10, .tipo = -1, .daño = 1, .velocidadAtaque = 1};
        Personaje Enemigo = {32*(i + 1), 80, .id = i + 25, .salud = -1, .daño = 1, .pantalla = -1};
        Proyectil Proyectile = {0, 0, .id = i + 40, .daño = -1, .disparado = false};
	    maquinasDinero[i] = MaquiDin;
	    maquinasPiezas[i] = MaquiPiez;
	    maquinasArmas[i] = MaquiArma;
        torretas[i] = Torretilla;
        enemigos[i] = Enemigo;
        proyectiles[i] = Proyectile;
        lineaDisponible[i] = false;
        existeTorreta[i] = false;
        pj.piezas[i] = 0;
        enemigosLinea[i] = 0;
    }

    for (int i = 6; i < 15; i++)    // Continua el for anterior para los elementos que los necestias
    {
        Personaje Enemigo = {32*(i + 1), 80, .id = i + 25, .salud = -1, .daño = 1, .pantalla = -1};
        Proyectil Proyectile = {0, 0, .id = i + 40, .daño = -1, .disparado = false};
        enemigos[i] = Enemigo;
        proyectiles[i] = Proyectile;
    }

    for (int i = 15; i < 20; i++)   // Continua el for anterior para los elementos que los necestias
    {
        Proyectil Proyectile = {0, 0, .id = i + 40, .daño = -1, .disparado = false};
        proyectiles[i] = Proyectile;
    }

    // Se establecen los valores de las torretas genericas
    Torreta tipo1 = {.salud = 10, .daño = 4, .tipo = 20, .velocidadAtaque = 0.9, .reparacion = 3};
    Torreta tipo2 = {.salud = 10, .daño = 5, .tipo = 21, .velocidadAtaque = 1, .reparacion = 7};
    Torreta tipo3 = {.salud = 10, .daño = 10, .tipo = 22, .velocidadAtaque = 1.6, .reparacion = 14};
    Torreta tipo4 = {.salud = 10, .daño = 10, .tipo = 23, .velocidadAtaque = 1.2, .reparacion = 39}; 
    Torreta tipo5 = {.salud = 10, .daño = 10, .tipo = 24, .velocidadAtaque = 0.7, .reparacion = 85};

    torretasGenericas[0] = tipo1;
    torretasGenericas[1] = tipo2;
    torretasGenericas[2] = tipo3;
    torretasGenericas[3] = tipo4;
    torretasGenericas[4] = tipo5;

    tiempoActual = 0;
    tiempoJuego = 0;
    tiempoEnemigos = 0;
    pj.posX = 90;
    pj.posY = 96;
    pj.dinero = 10;
    pj.fabrica = 0;
    moving = false;
    cantidadEnemigos = 1;
    mostrandoMenu = false;
    cantidadVenta = 1;
    tiempoActivacionDaño = -1;
    tiempoActivacionVAtq = -1;
    empezarTimer = false;
    cantEnemMax = 0;
}

void Controles() // Controles del jugador y sus respectivas restricciones de movimiento
{
	scanKeys();                 // Se usa consula de estado en vez de interrupciones para poder usar keysHeld() y no tener que avanzar pixel a pixel
	int pressed = keysHeld();   // Se comprueba si se esta maneteniendo pulsada una tecla

	if (pressed & KEY_UP || pressed & KEY_DOWN || pressed & KEY_RIGHT || pressed & KEY_LEFT) 
	{
        cantadorSonbido++;
        if(cantadorSonbido >= 30)
        {
            NF_PlayRawSound(2, 127, 64, false, 0);      //Ejecutar sonido de Andar
            cantadorSonbido = 0;
        }
        moving = true;
		if (pressed & KEY_DOWN) //Abajo
		{
            if(pj.fabrica == 0)
            {
                if(pj.posX + 16 > 32 && pj.posX < 207)
                {
                    if((pj.posY + 32 > 97 && pj.posY + 32 < 192) || (pj.posY + 32 > 48 && pj.posY + 32 < 64)) pj.posY++;
                }
                else if(pj.posY + 32 < 192) pj.posY++;
            }
            else if(pj.fabrica == 1)
            {
                if(pj.posY + 32 < 192) pj.posY++;
            }
            else if(pj.fabrica == -1)
            {
                if(pj.posY + 32 < 192) pj.posY++;
            }
            pj.direccion = 0;
		}
		else if (pressed & KEY_UP) //Arriba
		{
            if(pj.fabrica == 0)
            {
                if(pj.posX + 16 > 32 && pj.posX < 207)
                {
                    if(pj.posY + 32 > 98 || (pj.posY + 32 > 49 && pj.posY + 32 < 65)) pj.posY--;
                }
                else if(pj.posY + 32 > 49) pj.posY--;
            }
            else if(pj.fabrica == 1)
            {
                if(pj.posX < 192)
                {
                    if(pj.posY + 32 > 48) pj.posY--;
                }
                else if(pj.posY + 10 > 0) pj.posY--;
            }
            else if(pj.fabrica == -1)
            {
                if(pj.posX < 160)
                {
                    if(pj.posY + 32 > 48) pj.posY--;
                }
                else if(pj.posY + 10 > 0) pj.posY--;
            }
            pj.direccion = 2;
		}
		if (pressed & KEY_LEFT) //Izquierda
		{	
            if(pj.fabrica == 0)
            {
                if(pj.posY + 32 < 98 && pj.posY + 32 > 64)
                {
                    if(pj.posX > 207 || pj.posX < 32) pj.posX--;
                }
                else if(pj.posX > 0) pj.posX--;
            }
			else if(pj.fabrica == 1)
            {
                if(pj.posY + 32 < 48)
                {
                    if(pj.posX > 191) pj.posX--;
                }
                else if(pj.posX > 0) pj.posX--;
            }
            else if(pj.fabrica == -1)
            {
                if(pj.posY + 32 > 47)
                {
                    if(pj.posX > 80) pj.posX--;
                }
                else if(pj.posX > 160) pj.posX--;
            }
            pj.direccion = 3;
		}
		else if (pressed & KEY_RIGHT) //Derecha
		{
            if(pj.fabrica == 0)
            {
                if(pj.posY + 32 < 98 && pj.posY + 32 > 64)
                {
                    if(pj.posX + 16 < 32 || pj.posX > 205) pj.posX++;
                }
                else if(pj.posX + 16 < 256) pj.posX++;
            }
            else if(pj.fabrica == 1)
            {
                if(pj.posX + 16 < 256) pj.posX++;
            }
            else if (pj.fabrica == -1)
            {
                if(pj.posX + 16 < 256) pj.posX++;
            }
            pj.direccion = 1;
		}

		NF_MoveSprite(0, pj.id, pj.posX, pj.posY); // Se mueve el sprite del jugador

		if(pj.posX + 16 == 256 && (pj.posY + 32 >= 64) && (pj.posY + 32 <= 111))    // En caso de llegar a una de las puertas de la fabrica se cambia de mapa
		{
			if(pj.fabrica == 0)
			{
                pj.fabrica = 1;
				NF_DeleteTiledBg(0,1);
				NF_CreateTiledBg(0, 1, "fa");
                CargarYDescargarMapa(1, 2);
				NF_MoveSprite(0, pj.id, 10, pj.posY);
				pj.posX = 10;
			} 
			else if (pj.fabrica == -1)
			{
                pj.fabrica = 0;
				NF_DeleteTiledBg(0,1);
				NF_CreateTiledBg(0, 1, "fp");
                CargarYDescargarMapa(0, 1);
				NF_MoveSprite(0, pj.id, 10, pj.posY);
				pj.posX = 10;
			}
		}
		else if(pj.posX == 0 && (pj.posY + 32 >= 64) && (pj.posY + 32 <= 111))
		{
			if(pj.fabrica == 0)
			{
                pj.fabrica = -1;
				NF_DeleteTiledBg(0,1);
				NF_CreateTiledBg(0, 1, "fd");
                CargarYDescargarMapa(1, 0);
				NF_MoveSprite(0, pj.id, 230, pj.posY);
				pj.posX = 230;
			} 
			else if (pj.fabrica == 1)
			{
                pj.fabrica = 0;
				NF_DeleteTiledBg(0,1);
				NF_CreateTiledBg(0, 1, "fp");
                CargarYDescargarMapa(2, 1);
				NF_MoveSprite(0, pj.id, 230, pj.posY);
				pj.posX = 230;
			}
		}
	}
    else 
    {
        cantadorSonbido = 20;
        moving = false;
    }
}

void CargarYDescargarMapa(u8 origen, u8 destino) // Carga, Descarga, oculta y revela los elementos del mapa necearios para poder pasar entre patallas
{
    switch (origen) // Dependiendo de la sala de la que sale el personaje se ocultan unas cosas u otras
    {
        case 0:
            for (int i = 0; i < 6; i++)
            {
                NF_ShowSprite(0, maquinasDinero[i].id, false);
            }
            break;
        case 1:
            NF_UnloadTiledBg("menuTorreta");	// Descarga el fondo del menu de colocación de la torreta
            NF_UnloadTiledBg("menuTorreta2");	// Descarga el fondo del menu de gestion de la torreta
            for (int i = 0; i < 6; i++)
            {
                NF_ShowSprite(0, maquinasPiezas[i].id, false);
                if (existeTorreta[i])
                {
                    NF_ShowSprite(0, torretas[i].id, false);
                }

                if (enemigos[i].pantalla == 0)
                {
                    NF_ShowSprite(0, enemigos[i].id, false);
                }

                if (proyectiles[i].daño != -1)
                {
                    NF_ShowSprite(0, proyectiles[i].id, false);
                }
            }

            for (int i = 6; i < 15; i++)
            {
                if (enemigos[i].pantalla == 0)
                {
                    NF_ShowSprite(0, enemigos[i].id, false);
                }

                if (proyectiles[i].daño != -1)
                {
                    NF_ShowSprite(0, proyectiles[i].id, false);
                }
            }

            for (int i = 15; i < 20; i++)
            {
                if (proyectiles[i].daño != -1)
                {
                    NF_ShowSprite(0, proyectiles[i].id, false);
                }
            }
            break;
        case 2:
            NF_UnloadTiledBg("menuArma");	    // Descarga el fondo del menu de manual para la pantalla inferior
            
            for (int i = 0; i < 6; i++)
            {
                NF_ShowSprite(0, maquinasArmas[i].id, false);
            }
            break;
    }

    switch (destino) // Dependiendo de la sala a la que va el personaje se muetran unas cosas u otras
    {
        case 0:
            for (int i = 0; i < 6; i++)
            {
                NF_ShowSprite(0, maquinasDinero[i].id, true);
            }
            break;
        case 1:
            NF_LoadTiledBg("bg/manualTorreta", "menuTorreta", 256, 256);	// Carga el fondo del menu de manual para la pantalla inferior
            NF_LoadTiledBg("bg/comprar", "menuTorreta2", 256, 256);	        // Carga el fondo del menu de manual para la pantalla inferior
            for (int i = 0; i < 6; i++)
            {
                NF_ShowSprite(0, maquinasPiezas[i].id, true);
                if(existeTorreta[i])
                {
                    NF_ShowSprite(0, torretas[i].id, true);
                }

                if (enemigos[i].pantalla == 0)
                {
                    NF_ShowSprite(0, enemigos[i].id, true);
                }

                if (proyectiles[i].daño != -1)
                {
                    NF_ShowSprite(0, proyectiles[i].id, true);
                }
            }

            for (int i = 6; i < 15; i++)
            {
                if (enemigos[i].pantalla == 0)
                {
                    NF_ShowSprite(0, enemigos[i].id, true);
                }

                if (proyectiles[i].daño != -1)
                {
                    NF_ShowSprite(0, proyectiles[i].id, true);
                }
            }

            for (int i = 15; i < 20; i++)
            {
                if (proyectiles[i].daño != -1)
                {
                    NF_ShowSprite(0, proyectiles[i].id, true);
                }
            }  
            break;
        case 2:
         
            NF_LoadTiledBg("bg/manualArma", "menuArma", 256, 256);	        // Carga el fondo del menu de manual para la pantalla inferior
            for (int i = 0; i < 6; i++)
            {
                NF_ShowSprite(0, maquinasArmas[i].id, true);
            } 
            break;
     }
 }


void DeteccionMaquinaJugador()  // Detecta si el jugador esta en una posicion valida para interactuar con una maquina y le asigna un valor a la variable maquina
{
    if(pj.fabrica == 0)
    {
        if(pj.posY + 32 < 56)
        {
            if(pj.posX + 8  > 26 && pj.posX + 8  < 53) pj.maquina = 0;
            else if(pj.posX + 8 > 58 && pj.posX + 8  < 85) pj.maquina = 1;
            else if(pj.posX + 8 > 90 && pj.posX + 8  < 117) pj.maquina = 2;
            else if(pj.posX + 8 > 122 && pj.posX + 8  < 149) pj.maquina = 3;
            else if(pj.posX + 8  > 154 && pj.posX + 8  < 181) pj.maquina = 4;
            else if(pj.posX + 8  > 186 && pj.posX + 8  < 213) pj.maquina = 5;
            else pj.maquina = -1;
        }
        else if(pj.posY + 32 < 104 && pj.posY + 32 > 79)
        {
            if(pj.posX + 8  > 26 && pj.posX + 8  < 53) pj.maquina = 6;
            else if(pj.posX + 8  > 58 && pj.posX + 8  < 85) pj.maquina = 7;
            else if(pj.posX + 8 > 90 && pj.posX + 8  < 117) pj.maquina = 8;
            else if(pj.posX + 8  > 122 && pj.posX + 8  < 149) pj.maquina = 9;
            else if(pj.posX + 8  > 154 && pj.posX + 8  < 181) pj.maquina = 10;
            else if(pj.posX + 8  > 186 && pj.posX + 8  < 213) pj.maquina = 11;
            else pj.maquina = -1;
        }
        else pj.maquina = -1;
    }
    else if(pj.fabrica == 1)
    {
        if(pj.posY + 32 < 56 && pj.posY + 32 > 31)
        {
            if(pj.posX + 8 > 10 && pj.posX + 8 < 37) pj.maquina = 0;
            else if(pj.posX + 8 > 42 && pj.posX + 8 < 69) pj.maquina = 1;
            else if(pj.posX + 8 > 74 && pj.posX + 8 < 101) pj.maquina = 2;
            else if(pj.posX + 8 > 106 && pj.posX + 8 < 133) pj.maquina = 3;
            else if(pj.posX + 8 > 138 && pj.posX + 8 < 165) pj.maquina = 4;
            else if(pj.posX + 8 > 170 && pj.posX + 8 < 197) pj.maquina = 5;
            else pj.maquina = -1;
        }
        else pj.maquina = -1;
    }
    else if(pj.fabrica == -1)
    {
        if(pj.posY + 32 < 56 && pj.posY + 32 > 31)
        {
            if(pj.posX + 8 > 79 && pj.posX + 8 < 101) pj.maquina = 2;
            else if(pj.posX + 8 > 106 && pj.posX + 8 < 133) pj.maquina = 1;
            else if(pj.posX + 8 > 138 && pj.posX + 8 < 165) pj.maquina = 0;
            else pj.maquina = -1;
        }
        else if(pj.posY + 32 > 58 && pj.posY + 32 < 149 && pj.posX > 79 && pj.posX < 88)
        {
            if(pj.posY + 32 > 58 && pj.posY + 32 < 85) pj.maquina = 3;
            else if(pj.posY + 32 > 90 && pj.posY + 32 < 117) pj.maquina = 4;
            else if(pj.posY + 32 > 122 && pj.posY + 32 < 149) pj.maquina = 5;
            else pj.maquina = -1;
        }
        else pj.maquina = -1;
    }
}

void HUD() // Interfaz de Usuario para ver las piezas y dinero disponible (solo el texto, las imagenes forman parte del fondo)
{
    sprintf(pieza1, "%d      ", pj.piezas[0]);
    sprintf(pieza2, "%i      ", pj.piezas[1]);
    sprintf(pieza3, "%i      ", pj.piezas[2]);
    sprintf(pieza4, "%i      ", pj.piezas[3]);
    sprintf(pieza5, "%i      ", pj.piezas[4]);
    sprintf(pieza6, "%i      ", pj.piezas[5]);
    sprintf(dineros, "%i      ", pj.dinero);
    if ((60-(tiempoJuego%60 - 1)) >= 12) sprintf(tiempoRestante, "%i:%i  ", (10 - (tiempoJuego/60)) - 1, (60 - (tiempoJuego%60)) - 1);
    else sprintf(tiempoRestante, "%i:0%i  ", (10 - (tiempoJuego/60)) - 1, (60 - (tiempoJuego%60)) - 1);

    NF_WriteText(0, 0, 5, 1, pieza1);
    NF_WriteText(0, 0, 9, 1, pieza2);
    NF_WriteText(0, 0, 13, 1, pieza3);
    NF_WriteText(0, 0, 17, 1, pieza4);
    NF_WriteText(0, 0, 21, 1, pieza5);
    NF_WriteText(0, 0, 25, 1, pieza6);
    if(!mostrandoMenu)
    {
        NF_WriteText(1, 0, 22, 21, tiempoRestante);
        NF_WriteText(1, 0, 8, 21, dineros);
    } 
}

void MostrarMenu() //Muestra el menu según a qué máquina se acerque el jugador
{
    if(pj.fabrica == 0)
    {
        if(pj.maquina != -1)
        {
            if(!mostrandoMenu)
            {
                for(int i = 0; i < 15; i++)
                {
                    if(enemigos[i].pantalla == 1)
                    {
                        NF_ShowSprite(1, enemigos[i].id, false);
                    }
                }
            }
            mostrandoMenu = true;
            NF_ClearTextLayer(1, 0);
            if (pj.maquina <=5) //Menu maquina de piezas
            {
                NF_CreateTiledBg(1, 1, "menuManual");
                sprintf(nivel, "Nivel:%i  ", maquinasPiezas[pj.maquina].nivel);
                sprintf(tiempo, "Tiempo:%i ", maquinasPiezas[pj.maquina].tiempoPieza);
                sprintf(salud, "Salud:%i  ", maquinasPiezas[pj.maquina].salud);
                sprintf(costeDeMejora, "Mejorar:%i$ ", maquinasPiezas[pj.maquina].costeMejora);

                NF_WriteText(1, 0, 2, 3, salud);
                NF_WriteText(1, 0, 12, 3, nivel);
                NF_WriteText(1, 0, 21, 3, tiempo);
                if(maquinasPiezas[pj.maquina].tiempoEmpiece != -1) NF_WriteText(1, 0, 9, 12, " En proceso...");
                else NF_WriteText(1, 0, 9, 12, "Producir pieza");
                NF_WriteText(1, 0, 10, 20, costeDeMejora);
            }
            else
            {   
                if(existeTorreta[pj.maquina - 6]) //Menus torreta
                {
                    NF_CreateTiledBg(1, 1, "menuTorreta"); 
                    sprintf(dañoTorreta, "Ataque:%i  ", torretas[pj.maquina - 6].daño);
                    sprintf(saludTorreta, "Salud:%i  ", torretas[pj.maquina - 6].salud);
                    sprintf(velocidadAtaqueTorreta, "V.ataque:%2.1f  ", torretas[pj.maquina - 6].velocidadAtaque);
                    sprintf(reparar, "Reparar:%i$  ", ((torretasGenericas[(torretas[pj.maquina - 6].tipo)-20].salud - torretas[pj.maquina - 6].salud)*torretas[pj.maquina - 6].reparacion));

                    NF_WriteText(1, 0, 2, 5, saludTorreta);
                    NF_WriteText(1, 0, 2, 8, dañoTorreta);
                    NF_WriteText(1, 0, 2, 11, velocidadAtaqueTorreta);
                    NF_WriteText(1, 0, 19, 8, reparar);
                    NF_WriteText(1, 0, 12, 20, "Destruir");
                }
                else
                {
                    NF_CreateTiledBg(1, 1, "menuTorreta2"); 
                    sprintf(cantidadTorretas1, "x%i", pj.armamento[0]);
                    sprintf(cantidadTorretas2, "x%i", pj.armamento[1]);
                    sprintf(cantidadTorretas3, "x%i", pj.armamento[2]);
                    sprintf(cantidadTorretas4, "x%i", pj.armamento[3]);
                    sprintf(cantidadTorretas5, "x%i", pj.armamento[4]);

                    NF_WriteText(1, 0, 5, 11,cantidadTorretas1);
                    NF_WriteText(1, 0, 15, 11,cantidadTorretas2);
                    NF_WriteText(1, 0, 25, 11,cantidadTorretas3);
                    NF_WriteText(1, 0, 10, 18,cantidadTorretas4);
                    NF_WriteText(1, 0, 20, 18,cantidadTorretas5);
                }
            }
        }
        else if (mostrandoMenu)
        {
            NF_DeleteTiledBg(1, 1);
            mostrandoMenu = false;
            NF_ClearTextLayer(1, 0);
            for(int i = 0; i < 15; i++)
            {
                if(enemigos[i].pantalla == 1)
                {
                    NF_ShowSprite(1, enemigos[i].id, true);
                }
            }
        }
    }
    else if (pj.fabrica == 1) 
    {
        if(pj.maquina != -1 && pj.maquina < 5)
        {
            if(!mostrandoMenu)
            {
                for(int i = 0; i < 15; i++)
                {
                    if(enemigos[i].pantalla == 1)
                    {
                        NF_ShowSprite(1, enemigos[i].id, false);
                    }
                }
            } //Menu maquina de torretas
            NF_CreateTiledBg(1, 1, "menuArma");
            mostrandoMenu = true;
            NF_ClearTextLayer(1, 0);
            sprintf(dañoTorreta, "Ataque:%i  ", torretasGenericas[pj.maquina].daño);
            sprintf(saludTorreta, "Salud:%i  ", torretasGenericas[pj.maquina].salud);
            sprintf(velocidadAtaqueTorreta, "V.ataque:%2.1f  ", torretasGenericas[pj.maquina].velocidadAtaque);
            sprintf(comprar, "Comprar:%i$  ", maquinasArmas[pj.maquina].costeArmamento);

            NF_WriteText(1, 0, 2, 9, saludTorreta);
            NF_WriteText(1, 0, 2, 12, dañoTorreta);
            NF_WriteText(1, 0, 2, 15, velocidadAtaqueTorreta);
            NF_WriteText(1, 0, 18, 12, comprar);
        }
        else if (pj.maquina == 5)
        {
            if(!mostrandoMenu)
            {
                for(int i = 0; i < 15; i++)
                {
                    if(enemigos[i].pantalla == 1)
                    {
                        NF_ShowSprite(1, enemigos[i].id, false);
                    }
                }
            }
            NF_CreateTiledBg(1, 1, "menuArma");
            mostrandoMenu = true;
            NF_ClearTextLayer(1, 0);
            NF_WriteText(1, 0, 2, 12, "Ataque++");
            NF_WriteText(1, 0, 2, 14, "150$");
            NF_WriteText(1, 0, 19, 12, "Ataque++++");
            NF_WriteText(1, 0, 19, 14, "300$");
        }
        else if (mostrandoMenu)
        {
            NF_DeleteTiledBg(1, 1);
            mostrandoMenu = false;
            NF_ClearTextLayer(1, 0);
            for(int i = 0; i < 15; i++)
            {
                if(enemigos[i].pantalla == 1)
                {
                    NF_ShowSprite(1, enemigos[i].id, true);
                }
            }
        }
    }
    else if(pj.fabrica == -1)
    {
        if(pj.maquina != -1)
        {
            if(!mostrandoMenu)
            {
                for(int i = 0; i < 15; i++)
                {
                    if(enemigos[i].pantalla == 1)
                    {
                        NF_ShowSprite(1, enemigos[i].id, false);
                    }
                }
            }
            NF_CreateTiledBg(1, 1, "menuManual"); //Menu maquina de dinero
            mostrandoMenu = true;

            if (cantidadVenta > 9) sprintf(cantidadAVender, "%i  ", cantidadVenta);
            else sprintf(cantidadAVender, "0%i  ", cantidadVenta);
            sprintf(ganancia, "Vender por:%i$  ", (cantidadVenta * maquinasDinero[pj.maquina].costePieza));
            NF_ClearTextLayer(1, 0);
            NF_WriteText(1, 0, 4, 3, " <<< ");
            NF_WriteText(1, 0, 15, 3, cantidadAVender);
            NF_WriteText(1, 0, 23, 3, " >>> ");
            NF_WriteText(1, 0, 9, 12, ganancia);
        }
        else if (mostrandoMenu)
        {
            NF_DeleteTiledBg(1, 1);
            mostrandoMenu = false;
            NF_ClearTextLayer(1, 0);
            cantidadVenta = 1;
            for(int i = 0; i < 15; i++)
            {
                if(enemigos[i].pantalla == 1)
                {
                    NF_ShowSprite(1, enemigos[i].id, true);
                }
            }
        }
    }
}

void ComprobarTactil() //Funcion para comprobar el estado de la pantalla tactil y realizar las acciones correspondientes
{
    touchRead(&posicionXY); // Se lee la posición actual del tactil
    if(faseJuego == 0)
    {
        if(keysDown() && posicionXY.px > 12 && posicionXY.px < 243 && posicionXY.py > 154 && posicionXY.py < 181)
        {
            NF_PlayRawSound(0, 127, 64, false, 0);                  //Ejecutar sonido de Pulsar Boton
            faseJuego = 1;
        } 
    }
    else if(faseJuego == 1)
    {
        if(keysDown() && posicionXY.px > 0 && posicionXY.px < 256 && posicionXY.py > 0 && posicionXY.py < 192)
        {
            NF_PlayRawSound(0, 127, 64, false, 0);                  //Ejecutar sonido de Pulsar Boton
            faseJuego = 2;
        }
    }
    else if(faseJuego == 2)
    {
        if(keysDown() && posicionXY.px > 0 && posicionXY.px < 256 && posicionXY.py > 0 && posicionXY.py < 192)
        {
            NF_PlayRawSound(0, 127, 64, false, 0);                  //Ejecutar sonido de Pulsar Boton
            faseJuego = 3;
        }
    }
    else if(faseJuego == 3)
    {
        if(mostrandoMenu)
        {
            if(pj.fabrica == 0)
            {
                if(pj.maquina <=5)
                {
                    if(keysDown() && posicionXY.px > 12  && posicionXY.px < 243 && posicionXY.py > 57 && posicionXY.py < 145) CreaPieza(pj.maquina);
                    else if(keysDown() && posicionXY.px > 12 && posicionXY.px < 243 && posicionXY.py > 154 && posicionXY.py < 181) MejoraMaquina(pj.maquina);
                }
                else
                {
                    if(!existeTorreta[pj.maquina - 6])
                    {
                        if(keysDown() && posicionXY.px > 15  && posicionXY.px < 88 && posicionXY.py > 51 && posicionXY.py < 80) ColocarTorreta(0);
                        else if(keysDown() && posicionXY.px > 92 && posicionXY.px < 166 && posicionXY.py > 51 && posicionXY.py < 80) ColocarTorreta(1);
                        else if(keysDown() && posicionXY.px > 170 && posicionXY.px < 243 && posicionXY.py > 51 && posicionXY.py < 80) ColocarTorreta(2);
                        else if(keysDown() && posicionXY.px > 52 && posicionXY.px < 125 && posicionXY.py > 112 && posicionXY.py < 141) ColocarTorreta(3);
                        else if(keysDown() && posicionXY.px > 130 && posicionXY.px < 203 && posicionXY.py > 112 && posicionXY.py < 141) ColocarTorreta(4);
                    }
                    else
                    {
                        if(keysDown() && posicionXY.px > 144  && posicionXY.px < 247 && posicionXY.py > 24 && posicionXY.py < 114) RepararTorreta(pj.maquina - 6);
                        else if(keysDown() && posicionXY.px > 11  && posicionXY.px < 242 && posicionXY.py > 142 && posicionXY.py < 180) 
                        {
                            NF_PlayRawSound(6, 127, 64, false, 0);                  //Ejecutar sonido de Romper y ataque enemigos
                            DestruirTorreta(pj.maquina - 6);
                        }
                    }
                } 
            }
            else if (pj.fabrica == 1)
            {
                if(pj.maquina < 5)
                {
                    if(keysDown() && posicionXY.px > 144 && posicionXY.px < 247 && posicionXY.py > 56 && posicionXY.py < 146) ComprarTorreta(pj.maquina);
                }
                else
                {
                    if(keysDown() && posicionXY.px > 144 && posicionXY.px < 247 && posicionXY.py > 56 && posicionXY.py < 146 && tiempoActivacionVAtq == -1) MejorarTorretas(2);
                    else if(keysDown() && posicionXY.px > 8 && posicionXY.px < 111 && posicionXY.py > 56 && posicionXY.py < 146 && tiempoActivacionDaño == -1) MejorarTorretas(1);
                }
            }
            else if(pj.fabrica == -1)
            {
                if(keysDown() && posicionXY.px > 12 && posicionXY.px < 243 && posicionXY.py > 57 && posicionXY.py < 145) VendePieza(pj.maquina);
                else if(keysDown() && posicionXY.px > 12 && posicionXY.px < 87 && posicionXY.py > 14 && posicionXY.py < 45 && cantidadVenta > 1) 
                {
                    NF_PlayRawSound(0, 127, 64, false, 0);                  //Ejecutar sonido de Pulsar Boton
                    cantidadVenta--;
                }
                else if(keysDown() && posicionXY.px > 167 && posicionXY.px < 243 && posicionXY.py > 14 && posicionXY.py < 45 && cantidadVenta < pj.piezas[pj.maquina]) 
                {
                    cantidadVenta++;
                    NF_PlayRawSound(0, 127, 64, false, 0);                  //Ejecutar sonido de Pulsar Boton
                }
            }
        }
    }
    else if(faseJuego == 4 || faseJuego == 5)
    {
        if(keysDown() && posicionXY.px > 12 && posicionXY.px < 243 && posicionXY.py > 154 && posicionXY.py < 181) faseJuego = 0;
    }
}

void Timer()
{
    if (faseJuego == 3)
    {
        if(moving)                                // Si el jugador se esta moviendo se ejecuta la animacion de caminar
        {
            pj.anim_frame++;
            if(pj.anim_frame >= 3) pj.anim_frame = 0;
            NF_SpriteFrame(0, pj.id, pj.anim_frame + 3 * pj.direccion);
        }
        else
        {
            pj.direccion = 4;                    // Si no se esta moviendo se ejecuta la animacion de estar quieto
            pj.anim_frame++;
            if(pj.anim_frame >= 3) pj.anim_frame = 0;
            NF_SpriteFrame(0, pj.id, pj.anim_frame + 3 * pj.direccion);
        }

        if (empezarTimer) 
        {
            tiempoActual += 0.1;                 // Aumenta el tiempo en 1
            tiempoEnemigos += 0.1;
        }
        
        if((tiempoActual - tiempoJuego) >= 1) tiempoJuego += 1;

        for (int i = 0; i < 6; i++ )                                          // Recorre el array de maquinas
        {
            if (maquinasPiezas[i].state == 2)
            {
                maquinasPiezas[i].tiempoEmpiece = -1;
            }
            else if (maquinasPiezas[i].tiempoEmpiece != -1)                   // Si la maquina esta trabajando
            {
                ComprobarEstadoPieza(i);                                      // Comprueba si la pieza esta lista
            }
            else if(maquinasPiezas[i].nivel >= 5)                             // Si el nivel de la maquina  es mayor que 5                             
            {
                maquinasPiezas[i].tiempoEmpiece = tiempoActual;               // Empieza a trabajar sola
            }
        }
        
        for(int i = 0; i < 6; i++)                                            // Recorre el array de maquinas para actualizar su animacion en base a su estado
        {

            if (maquinasPiezas[i].state == 2)
            {
                maquinasPiezas[i].tiempoEmpiece = -1;
            }
            else if (maquinasPiezas[i].tiempoEmpiece != -1)                   // Si la maquina esta trabajando
            {
                ComprobarEstadoPieza(i);                                      // Comprueba si la pieza esta lista
            }
            else if(maquinasPiezas[i].nivel >= 5)                             // Si el nivel de la maquina  es mayor que 5                             
            {
                maquinasPiezas[i].tiempoEmpiece = tiempoActual;               // Empieza a trabajar sola
            }

            if(maquinasPiezas[i].salud >= 3 && maquinasPiezas[i].salud < 10) maquinasPiezas[i].state = 0;           // Cambia los estados de la maquina en base a su vida para luego cambiar el sprite
            else if(maquinasPiezas[i].salud >= 2 && maquinasPiezas[i].salud < 6) maquinasPiezas[i].state = 1;
            else if(maquinasPiezas[i].salud >= 1 && maquinasPiezas[i].salud < 3) maquinasPiezas[i].state = 2;
            if(maquinasPiezas[i].nivel != 0)
            {
                maquinasPiezas[i].anim_frame++;
                if(maquinasPiezas[i].tiempoEmpiece == -1) maquinasPiezas[i].anim_frame = 0;
                else if(maquinasPiezas[i].anim_frame >= 4) maquinasPiezas[i].anim_frame = 1;
                NF_SpriteFrame(0, maquinasPiezas[i].id, maquinasPiezas[i].anim_frame + 4 * maquinasPiezas[i].state);
            }
            else
            {
                NF_SpriteFrame(0, maquinasPiezas[i].id, 4 * maquinasPiezas[i].state);
            }
        }
        for(int i = 0; i < 15; i++) // Estado de animacion de los enemigos
        { 
            enemigos[i].anim_frame++;
            if(enemigos[i].anim_frame >= 3) enemigos[i].anim_frame = 0;
            if(enemigos[i].salud != -1)
            {
                NF_SpriteFrame(enemigos[i].pantalla, enemigos[i].id, enemigos[i].anim_frame + 3 * enemigos[i].direccion);
            }
        }

        if ((tiempoEnemigos / 45) > cantEnemMax) // Se aumenta la cantidad maxima de enemigos en pantalla
        {
            cantEnemMax++;
        }
        
        if (cantidadEnemigos < cantEnemMax) // Si hay menos enemigos que la cantidad maxima, se crea uno
        {
            GenerarEnemigos();
        }

        reset = true;

        for (int i = 0; i<6; i++)
        {
            if (maquinasPiezas[i].nivel > 0)    // Si hay alguna maquina sin romper, el reset se cancela
            {
                reset = false;
                break;
            }
        }

        if (reset && empezarTimer) // Si se tiene todas las maquinas rotas y el timer ya ha empezado, la cantidad maxima de enemigos se reinicia, como modo de balanceo
        {
            tiempoEnemigos = 30;
            cantEnemMax = 0;
        }

        for (int i = 0; i < 15; i++)
        {
            if (enemigos[i].atacando)
            {
                AtaqueEnemigos(i); // Realiza el ataque a las torretas y a las maquinas
            }
        }

        if ((tiempoActivacionDaño != -1) && (tiempoActual - tiempoActivacionDaño) >= 30) // Se retira el bufo 1
        {
            tiempoActivacionDaño = -1;
            MejorarTorretas(3);
        }

        if ((tiempoActivacionVAtq != -1) && (tiempoActual - tiempoActivacionVAtq) >= 30) // Se retira el bufo 1
        {
            tiempoActivacionVAtq = -1;
            MejorarTorretas(4);
        }
                
    }
}


void ComprobarEstadoPieza(int numPieza) //Comprueba si la pieza esta lista y la añade al inventario del jugador
{
    if((tiempoActual - maquinasPiezas[numPieza].tiempoEmpiece) >= maquinasPiezas[numPieza].tiempoPieza)
    {
        maquinasPiezas[numPieza].tiempoEmpiece = -1;
        pj.piezas[numPieza] += 1;
    }
}

void VendePieza(int numPieza) // El jugador vende una cantidad de piezas a cambio de dinero
{

    if (pj.piezas[numPieza] >= cantidadVenta)
    {
        NF_PlayRawSound(7, 127, 64, false, 0);                  //Ejecutar sonido de Comprar
        pj.dinero += cantidadVenta * maquinasDinero[numPieza].costePieza;
        pj.piezas[numPieza] -= cantidadVenta;
    }
    else
    {
        NF_WriteText(1, 0, 9, 20, "No tienes piezas");
        NF_PlayRawSound(1, 127, 64, false, 0);                  //Ejecutar sonido de No Puedes Pulsar Boton
    }

    cantidadVenta = 1;
}

void ComprarTorreta(int numMaquina) // El jugador compra una torreta a la maquina de armamento
{
    if(pj.dinero >= maquinasArmas[numMaquina].costeArmamento)
    {
        NF_PlayRawSound(7, 127, 64, false, 0);                  // Ejecutar sonido de Comprar
        pj.dinero -= maquinasArmas[numMaquina].costeArmamento;
        pj.armamento[numMaquina] += 1;
    }
    else
    {
        NF_PlayRawSound(1, 127, 64, false, 0);                  // Ejecutar sonido de No Puedes Pulsar Boton
    }
}

void ColocarTorreta(int torreta) // El jugador coloca una torreta en las posiciones determinadas
{
    if (!existeTorreta[pj.maquina - 6])
    {
        if(pj.armamento[torreta] > 0)
        {
            // Coloca las stats que le corresponde segun el tipo que era
            existeTorreta[pj.maquina - 6] = true;
            torretas[pj.maquina - 6].salud = torretasGenericas[torreta].salud;
            torretas[pj.maquina - 6].daño = torretasGenericas[torreta].daño;
            torretas[pj.maquina - 6].tipo = torretasGenericas[torreta].tipo;
            torretas[pj.maquina - 6].velocidadAtaque = torretasGenericas[torreta].velocidadAtaque;
            torretas[pj.maquina - 6].reparacion = torretasGenericas[torreta].reparacion;
            cooldown[pj.maquina - 6] = torretas[pj.maquina-6].velocidadAtaque;
            
            pj.armamento[torreta] -= 1;
            
            NF_PlayRawSound(5, 127, 64, false, 0);                  // Ejecutar sonido de Colocar Torreta
            NF_VramSpriteGfx(0, torretas[pj.maquina - 6].tipo, torretas[pj.maquina - 6].id, false); // Crea el sprite
            NF_CreateSprite(0, torretas[pj.maquina - 6].id, torretas[pj.maquina - 6].id, MARMASTORRETAPAL, torretas[pj.maquina - 6].posX, torretas[pj.maquina - 6].posY);
            NF_ClearTextLayer(1, 0);
        }
        else 
        {
            NF_PlayRawSound(1, 127, 64, false, 0);                  // Ejecutar sonido de No Puedes Pulsar Boton
        }
    }
}

void RepararTorreta(int numTorreta) // El jugador repara una torreta si dispone de dinero
{
    int saludReparacion = torretasGenericas[(torretas[numTorreta].tipo)-20].salud - torretas[numTorreta].salud; // Precio basado por punto de vida, es decir, a menos vida mas caro la reparacion
    
    if(pj.dinero >= (saludReparacion * torretas[numTorreta].reparacion))
    {
        NF_PlayRawSound(7, 127, 64, false, 0);                  // Ejecutar sonido de Comprar
        pj.dinero -= (saludReparacion * torretas[numTorreta].reparacion);
        torretas[numTorreta].salud += saludReparacion;
        NF_SpriteFrame(0, torretas[numTorreta].id, 0);
    }
    else
    {
        NF_PlayRawSound(1, 127, 64, false, 0);                  // Ejecutar sonido de No Puedes Pulsar Boton
    }
} 

void DestruirTorreta(int numTorreta) // Funcion para destruir una torreta si se queda sin salud o la destruye el jugador
{
    if(existeTorreta[numTorreta])
    {
        cooldown[numTorreta] = -1;
        existeTorreta[numTorreta] = false;          // Indicador de que esta torreta está en desuso
        NF_DeleteSprite(0, torretas[numTorreta].id);
        NF_FreeSpriteGfx(0, torretas[numTorreta].id);
        torretas[numTorreta].salud = 10;
        torretas[numTorreta].tipo = -1;
        torretas[numTorreta].daño = 1;
        torretas[numTorreta].velocidadAtaque = 1;
        NF_ClearTextLayer(1, 0);
    }
}

void CreaPieza(int numPieza)  // Se comienza a crear la pieza si esta no esta en proceso de creacion y si la maquina tiene nivel 1 o superior
{
    if(maquinasPiezas[numPieza].tiempoEmpiece == -1 && maquinasPiezas[numPieza].nivel != 0)
    {
        if (maquinasPiezas[numPieza].nivel != 0)
        {
            maquinasPiezas[numPieza].tiempoEmpiece = tiempoActual;
        }
        NF_PlayRawSound(3, 127, 64, false, 0);                   // Ejecutar sonido de producir pieza
    }
    else
    {
        NF_PlayRawSound(1, 127, 64, false, 0);                  // Ejecutar sonido de No Puedes Pulsar Boton
    }
}

void MejoraMaquina(int maquina) // El jugador mejora la maquina un nivel si dispone de dinero
{
    if (pj.dinero >= maquinasPiezas[maquina].costeMejora)
    {
        NF_PlayRawSound(7, 127, 64, false, 0);                  // Ejecutar sonido de Comprar
        pj.dinero -= maquinasPiezas[maquina].costeMejora;       // Al mejorar la maquina, el valor de venta de las piezas aumenta
        maquinasPiezas[maquina].nivel += 1;
        maquinasPiezas[maquina].salud = 3;
        maquinasDinero[maquina].costePieza += costePieza[maquina]/5;
        if (maquinasPiezas[maquina].nivel > 1) // Si no es el primer nivel, aumenta el coste de la siguiente mejora
        {
            maquinasPiezas[maquina].costeMejora += maquinasPiezas[maquina].costeMejora/4;
        }
        else // Primera mejora
        {
            lineaDisponible[maquina] = true;
            empezarTimer = true;
        }
    }
    else
    {
        NF_PlayRawSound(1, 127, 64, false, 0);                  // Ejecutar sonido de No Puedes Pulsar Boton
    }
}

double GetRandNumber(double x0, double x1) // Crea un numero random entre dos valores
{
	return x0 + (x1 - x0) * rand() / ((double) RAND_MAX);
}

void GenerarEnemigos() // Genera enemigos en una linea aleatoria si hay espacio  
{
    int linea = GetRandNumber(0, 7); // Coge una linea aleatoria 
    if (lineaDisponible[linea])
    {
        for (int i = 0; i < 15; i++)
        {
            if (enemigos[i].salud == -1) // Si no esta creado, coge una id que no se esté usando y lo crea
            {
                enemigos[i].pantalla = 1;
                enemigos[i].posX =  32 * (linea + 1);
                enemigos[i].posY = 80;
                enemigos[i].linea = linea;
                cantidadEnemigos++;
                enemigos[i].salud = 10;
                NF_VramSpriteGfx(1, 30, enemigos[i].id, false);
                NF_CreateSprite(1, enemigos[i].id, enemigos[i].id, ENEMIGOPAL, enemigos[i].posX, enemigos[i].posY);
                if (mostrandoMenu) NF_ShowSprite(1, enemigos[i].id, false);
                break;
            }
        }
    }
}

void MoverEnemigos() // Movimiento de enemigos 
{
    for (int i = 0; i < 15; i++)
    {
        if (enemigos[i].salud > 0)
        {
            if((enemigos[i].pantalla == 0) && existeTorreta[enemigos[i].linea]) // Si hay torreta
            {
                if (enemigos[i].posY > torretas[enemigos[i].linea].posY) // Si aun no ha llegado a la torreta
                {
                    enemigos[i].posY--;
                    if (pj.fabrica == 0) NF_MoveSprite(enemigos[i].pantalla, enemigos[i].id, enemigos[i].posX,  enemigos[i].posY);
                }
                else if(enemigos[i].posY > maquinasPiezas[enemigos[i].linea].posY && enemigos[i].posY < torretas[enemigos[i].linea].posY) // Si ya ha pasado a la torreta y no ha llegado a la maquina
                {
                    enemigos[i].posY--;
                    if (pj.fabrica == 0) NF_MoveSprite(enemigos[i].pantalla, enemigos[i].id, enemigos[i].posX,  enemigos[i].posY);
                }
            }
            else if ((enemigos[i].pantalla == 0) && enemigos[i].posY > maquinasPiezas[enemigos[i].linea].posY) // Si no hay torreta y no ha llegado a la maquina
            {
                enemigos[i].posY--;
                if (pj.fabrica == 0) NF_MoveSprite(enemigos[i].pantalla, enemigos[i].id, enemigos[i].posX,  enemigos[i].posY);
            }
            else // Si no esta en la pantalla 0
            {
                enemigos[i].posY--;
                NF_MoveSprite(enemigos[i].pantalla, enemigos[i].id, enemigos[i].posX,  enemigos[i].posY);
            }
        }
    }
}

void ControlEnemigos() //Controla el estado de enemigos
{
    for (int i = 0; i < 15; i++)
    {
        if (enemigos[i].salud > 0)
        {
            if((enemigos[i].pantalla == 0) && (existeTorreta[enemigos[i].linea]) && ((enemigos[i].posY + 32) == torretas[enemigos[i].linea].posY + 32))
            {
                enemigos[i].atacando = true;
                if (enemigos[i].sonido) 
                {
                    NF_PlayRawSound(6, 127, 64, false, 0);                  //Ejecutar sonido de Romper y ataque enemigos
                    enemigos[i].sonido = false;
                }
                if(torretas[enemigos[i].linea].salud <= 0 && existeTorreta[enemigos[i].linea]) NF_PlayRawSound(6, 127, 64, false, 0);                  //Ejecutar sonido de Romper y ataque enemigos
            }
            else if((enemigos[i].pantalla == 0) && enemigos[i].posY + 32 < 64)
            {
                if(maquinasPiezas[enemigos[i].linea].salud > 0) maquinasPiezas[enemigos[i].linea].salud -= 1; // Las maquinas pierden 1 de salud por golpe
                else // Se reinician todas las stats de la maquina, vuelve a nivel 0
                {
                    maquinasPiezas[enemigos[i].linea].nivel = 0;
                    maquinasPiezas[enemigos[i].linea].costeMejora = costeMejora[enemigos[i].linea];
                    maquinasDinero[enemigos[i].linea].costePieza = costePieza[enemigos[i].linea];
                    lineaDisponible[enemigos[i].linea] = false;
                }
                NF_PlayRawSound(6, 127, 64, false, 0);                  //Ejecutar sonido de Romper y ataque enemigos
                MuerteEnemigo(i);
            }
            else if((enemigos[i].posY) < -32) // Si han llegado al borde, cambiar de pantaña
            {
                CambiarDePantalla(i);
            }
        }
    }
}

void AtaqueEnemigos(int i) //Ataque de enemigos
{
    if(enemigos[i].salud > 0 && enemigos[i].pantalla == 0) 
    {
        if (enemigos[i].contAtaque >= 3) // Mini cooldown entre ataques
        {
            torretas[enemigos[i].linea].salud -= enemigos[i].daño;
            enemigos[i].sonido = true;
            enemigos[i].contAtaque = 0;
        }
        else
        {
            enemigos[i].contAtaque++;
        }
    }
    if(torretas[enemigos[i].linea].salud <= 0) // Si la torreta ya no tiene salud
    {
        DestruirTorreta(enemigos[i].linea);
        enemigos[i].atacando = false;
    }
    else if (torretas[enemigos[i].linea].salud <= 3 && existeTorreta[enemigos[i].linea] && pj.fabrica == 0) NF_SpriteFrame(0, torretas[enemigos[i].linea].id, 2); // Cambio de estado de animacion
    else if (torretas[enemigos[i].linea].salud <= 6 && existeTorreta[enemigos[i].linea] && pj.fabrica == 0) NF_SpriteFrame(0, torretas[enemigos[i].linea].id, 1); // Cambio de estado de animacion
}

void CambiarDePantalla(int enemigo) // Funcion para cambiar de pantalla los sprites de los enemigos
{
    NF_DeleteSprite(1, enemigos[enemigo].id);
    NF_FreeSpriteGfx(1, enemigos[enemigo].id);
    enemigos[enemigo].posY = 176;
    enemigos[enemigo].pantalla = 0;
    NF_MoveSprite(enemigos[enemigo].pantalla, enemigos[enemigo].id, enemigos[enemigo].posX,  enemigos[enemigo].posY);
    enemigosLinea[enemigos[enemigo].linea] += 1;
    if(pj.fabrica == 0)
    {
        NF_ShowSprite(0, enemigos[enemigo].id, true);
    }
}

void ControlProyectiles() //Controla los proyectiles             
{
    for(int i = 0; i < 20; i++)
    {
        if (proyectiles[i].daño != -1) // Si el proyectil existe, esta presente
        {
            if (proyectiles[i].disparado) // Booleano para que haga sonido al disparar
            {
                proyectiles[i].disparado = false;
                NF_PlayRawSound(4, 127, 64, false, 0);                  //Ejecutar sonido de Disparo
            }
            NF_MoveSprite(0, proyectiles[i].id, proyectiles[i].posX, proyectiles[i].posY);
            proyectiles[i].posY++;
            if (proyectiles[i].posY > 192) // Si esta fuera de pantalla, desaparece
            {
                NF_ShowSprite(0, proyectiles[i].id, false);
                proyectiles[i].daño= -1;
            }
            else
            {
                for (int j = 0; j < 15; j++)
                {
                    if (enemigos[j].salud != -1 && proyectiles[i].posX == enemigos[j].posX && enemigos[j].pantalla == 0) // Comprueba que haya enemigos vivos en su misma linea
                    {
                        if ((proyectiles[i].posY + 32 > enemigos[j].posY + 10) && proyectiles[i].daño != -1) // si les golpea, hace daño y desaparece.
                        {
                            NF_ShowSprite(0, proyectiles[i].id, false);
                            enemigos[j].salud -= proyectiles[i].daño;
                            if (enemigos[j].salud < 0) enemigos[j].salud = 0;
                            if (enemigos[j].salud == 0) MuerteEnemigo(j);
                            proyectiles[i].daño= -1;
                        }
                    }
                }
            }
        }
    }
}

void MuerteEnemigo(int enemigo) // Reinicio de sus stats y eliminacion del sprite en pantalla
{
    NF_ShowSprite(0, enemigos[enemigo].id, false);
    enemigosLinea[enemigos[enemigo].linea] -= 1;
    enemigos[enemigo].posX =  32 * (enemigos[enemigo].linea + 1);
    enemigos[enemigo].posY = 192;
    NF_MoveSprite(enemigos[enemigo].pantalla, enemigos[enemigo].id, enemigos[enemigo].posX,  enemigos[enemigo].posY);
    enemigos[enemigo].atacando = false;
    cantidadEnemigos--;
    enemigos[enemigo].pantalla = -1;
    enemigos[enemigo].salud = -1;
}

void DisparoTorreta()
{
    for (int i = 0; i < 6; i++)
    {
        if (existeTorreta[i])
        {
            cooldown[i] -= 0.1;
            if (cooldown[i] <= 0 && enemigosLinea[i] > 0) // Si hay enemigos en la linea y si ha pasado el tiempo de recarga, dispara
            {
                cooldown[i] = torretas[i].velocidadAtaque;
                for (int j = 0; j < 20; j++)
                {
                    if (proyectiles[j].daño == -1) // Escoge el primer proyectil de la lista que no se este usando, es decir, daño = -1
                    {
                        // Asigna todas las stats a las balas
                        if (pj.fabrica == 0) NF_ShowSprite(0, proyectiles[j].id, true);
                        proyectiles[j].daño = torretas[i].daño;
                        proyectiles[j].posX = torretas[i].posX;
                        proyectiles[j].posY = torretas[i].posY;
                        proyectiles[j].disparado = true;
                        break;
                    }
                }
            }
        }
    }
}

void MejorarTorretas(int tipo)
{
    switch (tipo)
    {
        case 1: // Buff de daño
            if(pj.dinero >= 150)
            {
                NF_PlayRawSound(7, 127, 64, false, 0);                  // Ejecutar sonido de Comprar
                pj.dinero -= 150;
                tiempoActivacionDaño = tiempoActual;
                for (int i = 0; i < 5; i++) torretasGenericas[i].daño += 2;
                break;
            }
        case 2: // Buff de daño V2 increible no fake
            if (pj.dinero >= 300)
            {
                NF_PlayRawSound(7, 127, 64, false, 0);                  // Ejecutar sonido de Comprar
                pj.dinero -= 300;
                tiempoActivacionVAtq = tiempoActual;
                for (int i = 0; i < 5; i++) torretasGenericas[i].daño += 5;
                break;
            }
        case 3: // Reset daño
            for (int i = 0; i < 5; i++) torretasGenericas[i].daño -= 2;
            break;
        case 4: // Reset daño 2 espectacular
            for (int i = 0; i < 5; i++) torretasGenericas[i].daño -= 5;
            break;
    }

    for (int i = 0; i < 6; i++)
    {
        if (torretas[i].tipo != -1)
        {
            torretas[i].daño = torretasGenericas[(torretas[i].tipo-20)].daño; // Reinicia el bufo de daño
        }
    }
}

void ComprobarCondiciones()
{
    if((tiempoActual >= 600 && pj.dinero < 10000) || pj.dinero >= 10000)
    {
        // Mueve al jugador a la sala principal tanto al perder como al ganar para evitar errores
        if (pj.fabrica == -1)
        {
            NF_DeleteTiledBg(0,1);
            NF_CreateTiledBg(0, 1, "fp");
            CargarYDescargarMapa(0, 1);
            pj.fabrica = 0;
            NF_MoveSprite(0, pj.id, 10, pj.posY);
            pj.posX = 10;
        }
        else if (pj.fabrica == 1)
        {
            NF_DeleteTiledBg(0,1);
            NF_CreateTiledBg(0, 1, "fp");
            CargarYDescargarMapa(2, 1);
            pj.fabrica = 0;
            NF_MoveSprite(0, pj.id, 230, pj.posY);
            pj.posX = 230;
        }

        if (mostrandoMenu)                                          // Si hay un menú activo, lo deshabilita
        {
            NF_DeleteTiledBg(1, 1);
            mostrandoMenu = false;
        }
        if(tiempoActual >= 600 && pj.dinero < 10000) 
        {
            NF_PlayRawSound(8, 127, 64, false, 0);                 // Ejecutar sonido de Derrota
            faseJuego = 5;
        }
        else if(pj.dinero >= 10000)
        {
            NF_PlayRawSound(9, 127, 64, false, 0);                 // Ejecutar sonido de Victoria
            faseJuego = 4;
        }
    }
}

void CargarPantallaFinal()
{
    // Borra TODOS los sprites cargados
    NF_DeleteTiledBg(0, 1);
    for (int i = 0; i < 6; i++)
    {
        NF_DeleteSprite(0, maquinasPiezas[i].id);
        NF_FreeSpriteGfx(0, maquinasPiezas[i].id);
        NF_DeleteSprite(0, maquinasDinero[i].id);
        NF_FreeSpriteGfx(0, maquinasDinero[i].id);
        NF_DeleteSprite(0, maquinasArmas[i].id);
        NF_FreeSpriteGfx(0, maquinasArmas[i].id);
        if(existeTorreta[i])
        {
            NF_DeleteSprite(0, torretas[i].id);
            NF_FreeSpriteGfx(0, torretas[i].id);
        }
        NF_DeleteSprite(0, enemigos[i].id);
        NF_FreeSpriteGfx(0, enemigos[i].id);
        if (enemigos[i].pantalla == 1)
        {
            NF_DeleteSprite(1, enemigos[i].id);
            NF_FreeSpriteGfx(1, enemigos[i].id);
        }
        NF_DeleteSprite(0, proyectiles[i].id);
        NF_FreeSpriteGfx(0, proyectiles[i].id);
    }
    for (int i = 6; i < 15; i++)
    {
        NF_DeleteSprite(0, enemigos[i].id);
        NF_FreeSpriteGfx(0, enemigos[i].id);
        if (enemigos[i].pantalla == 1)
        {
            NF_DeleteSprite(1, enemigos[i].id);
            NF_FreeSpriteGfx(1, enemigos[i].id);
        }
        NF_DeleteSprite(0, proyectiles[i].id);
        NF_FreeSpriteGfx(0, proyectiles[i].id);
    }
    for (int i = 15; i < 20; i++)
    {
        NF_DeleteSprite(0, proyectiles[i].id);
        NF_FreeSpriteGfx(0, proyectiles[i].id);
    }
    NF_DeleteSprite(0, pj.id);
    NF_FreeSpriteGfx(0, pj.id);

    NF_LoadTiledBg("bg/menuPrincipal", "menuPrincipal", 256, 256); // Carga los menus correspondientes
    NF_CreateTiledBg(0, 3, "menuPrincipal");
    NF_CreateTiledBg(1, 3, "menuManual");
    NF_ClearTextLayer(0, 0);
    NF_ClearTextLayer(1, 0);
    mmStart(MOD_PRINCIPAL, MM_PLAY_LOOP);

    // Refresco de pantalla para evitar glitches visuales
    NF_SpriteOamSet(0);
	NF_SpriteOamSet(1);
	swiWaitForVBlank();		// Espera al sincronismo vertical
	oamUpdate(&oamMain);    // Actualiza el OAM
	oamUpdate(&oamSub);
    
    if(faseJuego == 4)
    {
        NF_WriteText(1, 0, 11, 12, "Has ganado");   //Texto de victoria
    }
    else if(faseJuego == 5)
    {
        NF_WriteText(1, 0, 11, 12, "Has perdido");  // Texto de derrota
    }
    NF_WriteText(1, 0, 4, 20, "Pulsa para volver al menu");
    NF_UpdateTextLayers();
    
    while(faseJuego == 4 || faseJuego == 5) // Consulta de estado: hasta que no se presione la pantalla tactil no se vuelve al menú principal
    {
        scanKeys();
        ComprobarTactil();
    }
}