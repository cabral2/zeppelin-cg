#include "libraries.h"

// gcc main.c glm.c glmimg.c glm_util.c -lglut -lGLU -lGLEW -lGL -lSOIL -lSDL -lSDL_mixer -lm

#define TRUE 1
#define FALSE 0
#define ROTACAO_HELICE 3
#define QTD_PINHEIROS 200
#define QTD_NUVENS 100

GLMmodel *m_arvore = NULL, *m_casa = NULL, *m_poste = NULL, *m_nuvens = NULL;

int i = 0, j = 0;

typedef struct{
    GLdouble x, y, z;
}POSICAO, NUVEM;

typedef struct{
	double scale[3];
	double x, z;
}PINHEIRO;

typedef struct{
	GLfloat ambiente[4];
	GLfloat difusa[4];
	GLfloat especular[4];
	GLfloat posicao[4];
    GLfloat emissiva[4];
    GLfloat brilhosidade[1];
}LUZ, COR, MATERIAL;

int vira = 0, camType = 1, cam2Side = 1, luz = 1, fog = 1, dia = 0; //flags "booleanas"

PINHEIRO pinheiros[QTD_PINHEIROS];
NUVEM nuvens[QTD_NUVENS];

POSICAO cam, zeppelin, casa, poste, focoCamera;

LUZ sol, lampada;

MATERIAL plasticoDoZeppelin, madeira, chao, cabine, dome;

Mix_Music *jazzgostosinho;

GLuint texturaGrama, texturaHelice, texturaZeppelin, texturaDome;

GLfloat ambiente = 0.2;

double anguloRotacao = 360, anguloTranslacao = 2*M_PI, rotacaoHelice = 0, anguloCam3 = 2*M_PI;

void redimensiona(int w, int h){
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90, (float)(w)/h, 0.1, 30000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void setMaterial(MATERIAL m){
    glMaterialfv(GL_FRONT, GL_AMBIENT, m.ambiente);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, m.difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, m.especular);
    glMaterialfv(GL_FRONT, GL_EMISSION, m.emissiva);
    glMaterialfv(GL_FRONT, GL_SHININESS, m.brilhosidade);
}

void desenhaNuvem(){
    glColor3f(1.0f, 1.0f, 1.0f);
	if (!m_nuvens){
	    m_nuvens = glmReadOBJ("obejotas/nuvenzinhas/cloud.obj");
	    if (!m_nuvens) exit(0);
	    glmUnitize(m_nuvens);
	    glmFacetNormals(m_nuvens);
	    glmVertexNormals(m_nuvens, 90.0f, GL_TRUE);
    }
    glmDraw(m_nuvens, GLM_SMOOTH);
}

void desenhaArvore(){
    glColor3f(1.0f, 1.0f, 1.0f);
	if (!m_arvore){
	    m_arvore = glmReadOBJ("obejotas/arvorezinha/lowpolytree.obj");
	    if (!m_arvore) exit(0);
	    glmUnitize(m_arvore);
	    glmFacetNormals(m_arvore);
	    glmVertexNormals(m_arvore, 90.0f, GL_TRUE);
    }
    glmDraw(m_arvore, GLM_SMOOTH | GLM_COLOR | GLM_TEXTURE);
}

void desenhaPoste(){
	glColor3f(1.0f, 1.0f, 1.0f);
	if (!m_poste){
	    m_poste = glmReadOBJ("obejotas/postezinho/STLamp.obj");
	    if (!m_poste) exit(0);
	    glmUnitize(m_poste);
	    glmFacetNormals(m_poste);
	    glmVertexNormals(m_poste, 90.0f, GL_TRUE);
    }
    glmDraw(m_poste, GLM_SMOOTH | GLM_COLOR | GLM_TEXTURE);
}

void desenhaCasa(){
	glColor3f(1.0f, 1.0f, 1.0f);
	if (!m_casa){
	    m_casa = glmReadOBJ("obejotas/casinha/WoodenCabinObj.obj");
	    if (!m_casa) exit(0);
	    glmUnitize(m_casa);
	    glmFacetNormals(m_casa);
	    glmVertexNormals(m_casa, 90.0f, GL_TRUE);
    }
    glmDraw(m_casa, GLM_SMOOTH | GLM_COLOR | GLM_TEXTURE);
}

void desenhaHelice(){
	glColor3f(1.0f, 1.0f, 1.0f);
	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texturaHelice);
	glBegin(GL_TRIANGLE_FAN);
	   	glTexCoord2f(1, 1); glVertex3f( 1.4, 1.4, 0);
	   	glTexCoord2f(0, 1); glVertex3f(-1.4, 1.4, 0);
	   	glTexCoord2f(0, 0); glVertex3f(-1.4,-1.4, 0);
	   	glTexCoord2f(1, 0); glVertex3f( 1.4,-1.4, 0);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void solidSphere(int radius, int stacks, int columns){
    GLUquadric* quadObj = gluNewQuadric();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    gluQuadricNormals(quadObj, GLU_SMOOTH);
    gluQuadricTexture(quadObj, GL_TRUE);
    gluSphere(quadObj, radius, stacks, columns);
    gluDeleteQuadric(quadObj);
}

void desenhaZeppelin(){
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texturaZeppelin);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glPolygonMode(GL_FRONT, GL_FILL);
        solidSphere(5, 30, 45);
	glDisable(GL_TEXTURE_2D);
}

void desenhaCabine(){
	glColor4f(1.0f, 1.0f, 1.0f, 0.45);
    setMaterial(cabine);
	glTranslated(zeppelin.x, zeppelin.y - 5.5, zeppelin.z);
	glRotated(anguloRotacao, 0.0, 1.0, 0.0);
    glutSolidCube(1);
}

void desenhaPlano(){
    glColor3f(1.0f, 1.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    setMaterial(chao);
    glBindTexture(GL_TEXTURE_2D, texturaGrama);
 	    glBegin(GL_TRIANGLE_FAN);
            glColor3f(0.6, 0.6, 0.6);
            glTexCoord2f(0, 0);glVertex3f(0,0,0);
            glTexCoord2f(0, 1); glVertex3f(-10000, 0, -10000);
            glTexCoord2f(1, 1);glVertex3f(-10000,0,-5000);
            glTexCoord2f(1, 0);glVertex3f(-10000,0,0);
            glTexCoord2f(0, 1);glVertex3f(-10000,0,5000);
            glTexCoord2f(1, 1); glVertex3f(-10000, 0, 10000);
            glTexCoord2f(1, 0);glVertex3f(-5000,0,10000);
            glTexCoord2f(0, 1);glVertex3f(0,0,10000);
            glTexCoord2f(1, 1);glVertex3f(5000,0,10000);
            glTexCoord2f(1, 0); glVertex3f( 10000, 0,  10000);
            glTexCoord2f(0, 1);glVertex3f(10000,0,5000);
            glTexCoord2f(1, 1);glVertex3f(10000,0,0);
            glTexCoord2f(1, 0);glVertex3f(10000,0,-5000);
            glTexCoord2f(0, 1); glVertex3f( 10000, 0, -10000);
            glTexCoord2f(1, 1);glVertex3f(5000,0,-10000);
            glTexCoord2f(1, 0);glVertex3f(0,0,-10000);
            glTexCoord2f(0, 1);glVertex3f(-5000,0,-10000);
            glTexCoord2f(1, 1); glVertex3f(-10000, 0, -10000);
        glEnd();
    glDisable(GL_TEXTURE_2D);
}

void desenhaDome(){
	glColor3f(1.0f, 1.0f, 1.0f);
    setMaterial(dome);
    glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, texturaDome);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glPolygonMode(GL_FRONT, GL_FILL);
        solidSphere(10000, 30, 45);
	glDisable(GL_TEXTURE_2D);
}

void atualizaCam(){
    if(camType == 1){
        cam.x = zeppelin.x + 15*sin(anguloTranslacao);
        cam.z = zeppelin.z + 15*cos(anguloTranslacao);
        focoCamera.x = zeppelin.x;
        focoCamera.y = zeppelin.y;
        focoCamera.z = zeppelin.z;
    }
    if(camType == 2){
        focoCamera.x = zeppelin.x;
        focoCamera.y = zeppelin.y;
        focoCamera.z = zeppelin.z;
        switch(cam2Side){
        	case 1:
                cam.x = zeppelin.x + 15;
                cam.y = zeppelin.y + 12;
                cam.z = zeppelin.z;
                break;
            case 2:
                cam.x = zeppelin.x - 15;
                cam.y = zeppelin.y + 12;
                cam.z = zeppelin.z;
                break;
                case 3:
                cam.x = zeppelin.x;
                cam.y = zeppelin.y;
                cam.z = zeppelin.z + 15;
                break;
                case 4:
                cam.x = zeppelin.x;
                cam.y = zeppelin.y;
                cam.z = zeppelin.z - 13;
                break;
    	}
    }
    if(camType == 3){
        cam.x = zeppelin.x;
        cam.y = zeppelin.y - 5.5;
        cam.z = zeppelin.z;
        focoCamera.x = zeppelin.x - 10*sin(anguloCam3);
        focoCamera.y = zeppelin.y - 5;
        focoCamera.z = zeppelin.z - 10*cos(anguloCam3);
    }
}

void desenhaCena(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
        gluLookAt(cam.x, cam.y, cam.z, focoCamera.x, focoCamera.y, focoCamera.z, 0, 1, 0);
        glLightfv(GL_LIGHT0, GL_POSITION, sol.posicao);

        if(luz) glEnable(GL_LIGHTING);
        if(fog) glEnable(GL_FOG);
        
        //ARVORES
    	for(i = 0; i < QTD_PINHEIROS; i++){
	    	glPushMatrix();
                setMaterial(madeira);
	    		glTranslated(pinheiros[i].x, pinheiros[i].scale[1] + 1, pinheiros[i].z);
	    		glScaled(pinheiros[i].scale[0], pinheiros[i].scale[1], pinheiros[i].scale[2]);
		      	desenhaArvore();
        	glPopMatrix();
        }
        
        //NUVENS
        for(i = 0; i < QTD_NUVENS; i++){
            glPushMatrix();
                glTranslated(nuvens[i].x, nuvens[i].y + 70, nuvens[i].z);
                glScaled(90, 90, 90);
                desenhaNuvem();
            glPopMatrix();
        }
        //ZEPPELIN
        glPushMatrix();
            setMaterial(plasticoDoZeppelin);
            glTranslated(zeppelin.x, zeppelin.y, zeppelin.z);
            glRotated(anguloRotacao, 0, 1, 0);
            glScaled(1, 1, 2);
            glRotated(180, 0, 0, 1);
          	desenhaZeppelin();
        glPopMatrix();
        
        //SKYDOME
        glPushMatrix();
            setMaterial(dome);
            glTranslated(zeppelin.x, 0, zeppelin.z);
        	glRotated(-90,1,0,0);
    		desenhaDome();
    	glPopMatrix();

        desenhaPlano();

        //CASA
        glPushMatrix();
        	glTranslated(casa.x, casa.y + 20, casa.z);
        	glScaled(50, 50, 50);
        	desenhaCasa();
        glPopMatrix();

        //POSTE
        glPushMatrix();
            glTranslated(poste.x, poste.y, poste.z);
            glScaled(30, 30, 30);
            desenhaPoste();
        glPopMatrix();

        //HELICE 
        glPushMatrix();
            glColor3f(0.0, 0.0, 0.0);
            glTranslated(zeppelin.x, zeppelin.y, zeppelin.z);
            glRotated(anguloRotacao, 0.0, 1.0, 0.0);
            glTranslated(0.0, 0.0 , 10);
            glRotated(rotacaoHelice, 0.0, 0.0, 1.0);
            desenhaHelice();
        glPopMatrix();

        //CABINE
        glPushMatrix();
            setMaterial(cabine);
            desenhaCabine();
        glPopMatrix();

       	if(luz) glDisable(GL_LIGHTING);
        if(fog) glDisable(GL_FOG);
    	
    glPopMatrix();

    glutSwapBuffers();
}

void teclado(unsigned char key, int x, int y){
    switch(key){
        case 27: // ESC
            exit(0);
            break;
        case 'a':
            anguloRotacao += 2;
            anguloTranslacao += M_PI/90;
            atualizaCam();
            break;
        case 'd':
            anguloTranslacao -= M_PI/90;
            anguloRotacao -= 2;
            atualizaCam();
            break;
        case 'w':
            zeppelin.x -= 4*sin(anguloTranslacao);
            zeppelin.z -= 4*cos(anguloTranslacao);
            atualizaCam();
            break;
        case 's':
            zeppelin.x += 2*sin(anguloTranslacao);
            zeppelin.z += 2*cos(anguloTranslacao);
            atualizaCam();
            break;
        case 'l':
        case 'L':
        	luz = !luz; break;
        case 'f':
        case 'F':
        	fog = !fog; break;
        case '+':
            if(cam.y > 0)
                cam.y -= 2;
            break;
        case '-':
            cam.y += 2;
            break;
        case '1':
            camType = 1;
            cam.y = zeppelin.y + 10;
            atualizaCam();
            break;
        case '2':
            cam.x = 0;
            camType = 2;
            atualizaCam();
            break;
        case 'c':
            if(camType == 2){
                cam2Side++;
                if(cam2Side == 5)
                    cam2Side = 1;
                }
            atualizaCam();
            break;
        case '3':
            camType = 3;
            atualizaCam();
            break;
        case '^':
        	cam.y +=20;
        	break;
    }
}

void SpecialInput(int key, int x, int y){
	switch(key){
	    case GLUT_KEY_UP:
	        zeppelin.x -= 40*sin(anguloTranslacao);
	        zeppelin.z -= 40*cos(anguloTranslacao);
	        atualizaCam();
	        break;  
	    case GLUT_KEY_DOWN:
	        zeppelin.x += 40*sin(anguloTranslacao);
	        zeppelin.z += 40*cos(anguloTranslacao);
	        atualizaCam();
	        break;
	    case GLUT_KEY_LEFT:
	        if(camType == 3){
	            anguloCam3 += M_PI/180;
	            atualizaCam();
	        }
	        break;
	    case GLUT_KEY_RIGHT:
	        if(camType == 3){
	            anguloCam3 -= M_PI/180;
	            atualizaCam();
	        }
	        break;
	}
}

void setupTextura(){
    texturaGrama    = SOIL_load_OGL_texture("images/grama.png",            SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    texturaHelice   = SOIL_load_OGL_texture("images/helice-serie-vs2.png", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
	texturaZeppelin = SOIL_load_OGL_texture("images/proerd.png",           SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    texturaDome     = SOIL_load_OGL_texture("images/sk.png",               SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
    if (!texturaGrama || !texturaHelice || !texturaZeppelin || !texturaDome)
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
}

void setupSom(){
    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    jazzgostosinho = Mix_LoadMUS("music/lofi.mp3");
}

void setupMaterial(){
    //essa feiura aqui eh pq nao tava conseguindo inicializar essa me$@%T* de jeito nenhum
    //ZEPPELIN
    for(i = 0; i < 3; i++) plasticoDoZeppelin.ambiente[i] = 0.25;
    for(i = 0; i < 3; i++) plasticoDoZeppelin.difusa[i] = 0.9;
    for(i = 0; i < 3; i++) plasticoDoZeppelin.especular[i] = 1;
    plasticoDoZeppelin.ambiente[3] = plasticoDoZeppelin.difusa[3] = plasticoDoZeppelin.especular[3] = plasticoDoZeppelin.emissiva[3] = 1;
    plasticoDoZeppelin.brilhosidade[0] = 5;

    //MADEIRA
    for(i = 0; i < 3; i++) madeira.ambiente[i] = 0.1;
    madeira.ambiente[1] = .1;
    for(i = 0; i < 3; i++) madeira.difusa[i] = .4;
    madeira.difusa[1] = .4;
    for(i = 0; i < 3; i++) madeira.especular[i] = 0;
    madeira.especular[1] = .04;
    for(i = 0; i < 3; i++) madeira.emissiva[i] = .0;
    madeira.ambiente[3] = madeira.difusa[3] = madeira.especular[3] = madeira.emissiva[3] = 1;
    madeira.brilhosidade[0] = 0;

    //CHAO
    for(i = 0; i < 3; i++) chao.ambiente[i] = 0;
    chao.ambiente[1] = .2;
    for(i = 0; i < 3; i++) chao.difusa[i] = .5;
    chao.difusa[1] = .6;
    for(i = 0; i < 3; i++) chao.especular[i] = .0;
    chao.especular[1] = .1;
    for(i = 0; i < 3; i++) chao.emissiva[i] = .0;
    chao.ambiente[3] = chao.difusa[3] = chao.especular[3] = chao.emissiva[3] = 1;
    chao.brilhosidade[0] = 0;

    //DOME
    for(i = 0; i < 3; i++) dome.ambiente[i] = 0;
    dome.ambiente[2] = .2;
    for(i = 0; i < 3; i++) dome.difusa[i] = .5;
    dome.difusa[2] = .6;
    for(i = 0; i < 3; i++) dome.especular[i] = 0;
    dome.especular[2] = .11;      
    for(i = 0; i < 3; i++) chao.emissiva[i] = .0;
    chao.ambiente[3] = dome.difusa[3] = dome.especular[3] = dome.emissiva[3] = 1;
    dome.brilhosidade[0] = 0;

    //CABINE
    for(i = 0; i < 3; i++) cabine.ambiente[i] = 0;
    for(i = 0; i < 3; i++) cabine.difusa[i] = 0;
    for(i = 0; i < 3; i++) cabine.especular[i] = 1;
    for(i = 0; i < 3; i++) cabine.emissiva[i] = .0;
    cabine.ambiente[3] = 0;
    cabine.difusa[3] = cabine.especular[3] = cabine.emissiva[3] = 1;
    cabine.brilhosidade[0] = 0.01;
}

void randomPines(){
	for(i = 0; i < QTD_PINHEIROS; i++)
		for( j = 0; j < 3; j++)
			pinheiros[i].scale[j] = rand()%30 + 20;     //essa funcao randomiza tamanho comprimento posicao etc dos pinheiros pelo mapa
	for(i = 0; i < QTD_PINHEIROS/4; i++)
		pinheiros[i].x = rand()%2000 -1000;
	for(i = 0; i < QTD_PINHEIROS/4; i++)
		pinheiros[i].z = rand()%2000 -1000;
	for(j = i; j < QTD_PINHEIROS*3/4; j++)
		pinheiros[j].x = rand()%20000 -10000;
	for(j = i; j < QTD_PINHEIROS*3/4; j++)
		pinheiros[j].z = rand()%20000 -10000;
}

void randomClouds(){
	for(i = 0; i < QTD_NUVENS; i++)
		nuvens[i].x = rand()%20000 - 10000;     //randomiza posicao das nuvens pelo mapa
	for(i = 0; i < QTD_NUVENS; i++)
		nuvens[i].z = rand()%20000 - 10000;
    for(i = 0; i < QTD_NUVENS; i++)             //randomiza altura das nuvens
        nuvens[i].y = rand()%150;
}

void setupLuz(){
    if(ambiente <= -0.3) dia = 1;
    if(ambiente >= 0.5) dia = 0;
    if(dia) ambiente += 0.0005;
    else ambiente -= 0.0005;
    for(i = 0; i < 3; i++) sol.ambiente[i] = ambiente;
    for(i = 0; i < 3; i++) sol.difusa[i] = 0.7;
    for(i = 0; i < 3; i++) sol.especular[i] = 1;
    sol.ambiente[3] = sol.difusa[3] = sol.especular[3] = 1;
    sol.posicao[0] = 0; sol.posicao[1] = 1; sol.posicao[2] = 0; sol.posicao[3] = 0;

    for(i = 0; i < 3; i++) lampada.ambiente[i] = .05;
    for(i = 0; i < 3; i++) lampada.difusa[i] = .3;
    for(i = 0; i < 3; i++) lampada.especular[i] = .7;
    lampada.ambiente[3] = lampada.difusa[3] = lampada.especular[3] = 1;
    lampada.posicao[0] = poste.x; lampada.posicao[1] = poste.y + 30; lampada.posicao[2] = poste.z; lampada.posicao[3] = 1;
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, sol.ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sol.difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sol.especular);
    
    glLightfv(GL_LIGHT1, GL_AMBIENT, lampada.ambiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lampada.difusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lampada.especular);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.05f);

    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    glShadeModel (GL_SMOOTH);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
}

void setupFog(){
    float cor[] = {.0f, .1f, .11f};
    glClearColor(cor[0], cor[1], cor[2], .2f);
    glFogi(GL_FOG_MODE, GL_EXP);
    glFogfv(GL_FOG_COLOR, cor);
    glFogf(GL_FOG_START, zeppelin.x + 300);
    glFogf(GL_FOG_END,  zeppelin.x + 10000);
    glFogf(GL_FOG_DENSITY, 0.001);
}

void setupPosicoes(){
    zeppelin.x = 0;
    zeppelin.y = 100;
    zeppelin.z = 0;
    casa.x = 100;
    casa.y = 23;
    casa.z = 50;
    poste.x = 50;
    poste.y = 30;
    poste.z = 130;
    cam.x = zeppelin.x;
    cam.y = zeppelin.y + 8;
    cam.z = zeppelin.z + 10;
}

void mensagens(){
    printf("WASD para movimentação do zeppelin\n");
    printf("'1', '2' e '3' para alternar entre câmeras\n");
    printf("Pressione +/- para controlar a altura da câmera\n");
    printf("Pressione 'f' para ligar/desligar a neblina\n");
    printf("Pressione 'l' para ligar/desligar a luz\n");
}
void setup(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setupPosicoes();
    setupSom();
    setupMaterial();
    setupTextura();
    setupLuz();
    randomPines();
    randomClouds();
    atualizaCam();
    setupFog();
    mensagens();
    Mix_PlayMusic(jazzgostosinho, -1);
}
void atualiza(int periodo){
	rotacaoHelice += ROTACAO_HELICE;
    setupLuz();
    setupMaterial();
	glutTimerFunc(25, atualiza, 0);	
	glutPostRedisplay();
}
int main(int argc, char *argv[]){

    glutInit(&argc, argv);
    
    srand(time(NULL));

    glutInitContextVersion(2, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("...");

    glutReshapeFunc(redimensiona);
    glutDisplayFunc(desenhaCena);
    glutSpecialFunc(SpecialInput);
    glutIdleFunc(desenhaCena);
    glutTimerFunc(25, atualiza, 0);
    glutKeyboardFunc(teclado);
    glewInit();
    setup();
    glutMainLoop();
    return 0;
}