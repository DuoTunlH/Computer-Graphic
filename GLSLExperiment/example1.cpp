//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/


point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint loc_modelMatrix;
GLuint loc_projection;
GLfloat l = -1.0, r = 1.0, t = 1.0, b = -1.0, zNear = 0.5, zFar = 6;
GLfloat radius = 1, thetal = 180, phi = 0;
GLfloat dr = 5 * M_PI / 180;  
GLfloat theta[] = { 0,0,0 };
GLfloat quay[1] = { 0 };

mat4 model;
mat4 quayBase;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers( void )
{
	// Tạo một VAO - vertex array object
	GLuint vao;
    glGenVertexArrays( 1, &vao );     
    glBindVertexArray( vao );

    // Tạo và khởi tạo một buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors); 
	
}


void shaderSetup( void )
{
	// Nạp các shader và sử dụng chương trình shader
    program = InitShader( "vshader1.glsl", "fshader1.glsl" );   // hàm InitShader khai báo trong Angel.h
    glUseProgram( program );

    // Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
    GLuint loc_vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( loc_vPosition );
    glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	loc_modelMatrix = glGetUniformLocation(program, "modelMatrix");
	loc_projection = glGetUniformLocation(program, "projection");
	glEnable(GL_DEPTH_TEST);

    glClearColor( 1.0, 1.0, 1.0, 1.0 );        /* Thiết lập màu trắng là màu xóa màn hình*/
}

void matPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt) {
	point4 eye(0,
		       1,
		       1,
		       1.0);
	point4 at(sin(thetal), 1, 1 + cos(thetal), 1.0);
	vec4 up(0, 1, 0, 1.0);

	mat4 v = LookAt(eye, at, up);
	mat4 ins = Scale(x,y,z);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, v*quayBase*mt*ins);

	mat4 p = Frustum(l, r, b, t, zNear, zFar);
	glUniformMatrix4fv(loc_projection, 1, GL_TRUE, p);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void truMayChieu() {
	model = Translate(0, 0.25, 0);
	matPhang(0.2, 0.5, 0.2, model);
}
void hopMayChieu() {
	model = Translate(-0.5 * 0.025, 0, 0);
	matPhang(0.6, 0.05, 0.2, model);
}

GLfloat wM = 1.5, hM = 2.5, dM = 0.05;
GLfloat wTN = 1.5, hTN = 0.1, dTN = 0.2;
GLfloat keo = 0;
bool checkkeo = false;

void manChieu() {
	model = Translate(0, 0.5 * keo, -6.8 );
	matPhang(wM, hM - keo, dM, model);
	model = Translate(0, 0.5 * hM + 0.5 * hTN, -6.8);
	matPhang(wTN, hTN, dTN, model);

}


void mayChieu() {
	truMayChieu();
	hopMayChieu();
}


void matBan() {
	model = Translate(0, 0.3, 0);
	matPhang(0.8, 0.02, 0.4, model);
}

void chanBan() {
	//chan 1
	model = Translate(-0.375,-0.01,-0.2);
	matPhang(0.02, 0.6, 0.02, model);

	//chan 2
	model = Translate(-0.375, -0.01, 0.2);
	matPhang(0.02, 0.6, 0.02, model);

	//chan 3
	model = Translate(0.375, -0.01, -0.2);
	matPhang(0.02, 0.6, 0.02, model);

	//chan 4
	model = Translate(0.375, -0.01, 0.2);
	matPhang(0.02, 0.6, 0.02, model);

	//thang doc 1
	model = Translate(-0.375, -0.24, 0);
	matPhang(0.02, 0.02, 0.4, model);

	//thanh doc 2
	model = Translate(0.375, -0.24, 0);
	matPhang(0.02, 0.02, 0.4, model);

	//thanh ngang
	model = Translate(0, -0.24, 0);
	matPhang(0.78, 0.02, 0.02, model);

}

void MayTinh() {
	//De may tinh
	model = Translate(0, 0.3, 0);
	matPhang(0.3, 0.03, 0.02, model);

	//Than may tinh
	model = Translate(0, 0.45, 0);
	matPhang(0.05, 0.25, 0.02, model);

	//Man may tinh
	model = Translate(0, 0.7, 0) * RotateY(180);
	matPhang(0.35, 0.4, 0.04, model);
}

GLfloat keoghe = 0;
bool checkkeoghe = false;
void matGhe() {
	model = Translate(0, 0, 0.5 + keoghe);
	matPhang(0.4, 0.02, 0.25, model);
	model = Translate(0, 0.15, 0.62 + keoghe) * RotateX(90);
	matPhang(0.4, 0.02, 0.25, model);
}

void chanGhe() {
	//chan 1
	model = Translate(-0.175, -0.15, 0.4 + keoghe);
	matPhang(0.02, 0.3, 0.02, model);

	//chan 2
	model = Translate(-0.175, -0.15, 0.6+ keoghe);
	matPhang(0.02, 0.3, 0.02, model);

	//chan 3
	model = Translate(0.175, -0.15, 0.4 + keoghe);
	matPhang(0.02, 0.3, 0.02, model);

	//chan 4
	model = Translate(0.175, -0.15, 0.6 + keoghe);
	matPhang(0.02, 0.3, 0.02, model);

	
}
void caiGhe() {
	chanGhe();
	matGhe();
}


void caiBan() {
	matBan();
	chanBan();
	MayTinh();
	caiGhe();
}

GLfloat heighTu = 1.8;
GLfloat widthTu = 0.8;
GLfloat day = 0.02;

void khungTu() {
	//Mặt trái
	model = Translate(-0.5 * widthTu - 0.5 * day, 0, 0);
	matPhang(day, heighTu, widthTu, model);
	//Mặt phải
	model = Translate(0.5 * widthTu + 0.5 * day, 0, 0);
	matPhang(day, heighTu, widthTu, model);
	//Mặt trên
	model = Translate(0, 0.5 * heighTu - 0.5 * day, 0);
	matPhang(widthTu, day, widthTu, model);
	//Mặt dưới
	model = Translate(0, -0.5 * heighTu + 0.5 * day, 0);
	matPhang(widthTu, day, widthTu, model);
	// mặt sau
	model = Translate(0, 0, 0.5 * widthTu - 0.5 * day);
	matPhang(widthTu, heighTu, day, model);

	// ngăn ngag tủ trên 
	model = Translate(0, 0.25 * heighTu, 0);
	matPhang(widthTu, day, widthTu, model);
	// ngăn ngag tủ giữa 
	model = Translate(0, 0, 0);
	matPhang(widthTu, day, widthTu, model);
	// ngăn ngag tủ dưới 
	model = Translate(0, -0.25 * heighTu, 0);
	matPhang(widthTu, day, widthTu, model);

	// ngăn dọc giữa 
	model = Translate(0, 0, 0);
	matPhang(day, heighTu, widthTu, model);
}
GLfloat widthCT = 0.5 * widthTu;
GLfloat heightCT = 0.25 * heighTu;
GLfloat MoTu[8] = { 0,0,0,0,0,0,0,0 };
bool checkMoTu[8] = { false,false ,false,false,false,false,false,false };
void cuaTu() {
	// cửa 1 trái
	model = Translate(-widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu[0]) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
	// cửa 2 trái
	model = Translate(-widthCT, 0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu[1]) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
	// cửa 3 trái
	model = Translate(-widthCT, -0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu[2]) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
	// cửa 4 trái
	model = Translate(-widthCT, -1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu[3]) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);

	// cửa 1 phai
	model = Translate(widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu[4]) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
	// cửa 2 phai
	model = Translate(widthCT, 0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu[5]) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
	// cửa 3 phai
	model = Translate(widthCT, -0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu[6]) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
	// cửa 4 phai
	model = Translate(widthCT, -1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu[7]) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT, day, model);
}
void tu() {
	khungTu();
	cuaTu();
}


void tuongTrai() {
	model = Translate(-3.25, 0.4, -0.35);
	matPhang(0.02, 1.5, 6.8, model);

	model = Translate(-3.25, 0.4, -6.1);
	matPhang(0.02, 1.5, 1.7, model);

	model = Translate(-3.26, 2.9, -1.95);
	matPhang(0.02, 3.5, 10, model);
}

void tuongPhai() {

	model = Translate(3.25, 3.9, -1.95);
	matPhang(0.02, 1.5, 10, model);

	model = Translate(3.25, 0.45, -1.95);
	matPhang(0.02, 1.5, 10, model);

	model = Translate(3.25, 2.18, -6.4);
	matPhang(0.02, 2, 1, model);

	model = Translate(3.25, 2.18, -1.95);
	matPhang(0.02, 2, 1, model);

	model = Translate(3.25, 2.18, 2.55);
	matPhang(0.02, 2, 1, model);

}

GLfloat quayCuaSo1 = 0, quayCuaSo2 = 0;
void cuaSo() {
	//canh cua 1 
	model = Translate(3.25, 0, -5.9) * RotateY(quayCuaSo1) * Translate(0, 2.18, 0.5 * 1.75);
	matPhang(0.02, 2, 1.75, model);

	//canh cua 2 
	model = Translate(3.25, 0, -2.45) * RotateY(180) * RotateY(quayCuaSo2) * Translate(0, 2.18, 0.5 * 1.75);
	matPhang(0.02, 2, 1.75, model);
}
void tranNha() {
	model = Translate(0, 4.68, -1.95);
	matPhang(6.5, 0.02, 10, model);
}
void tuongSau() {
	model = Translate(0, 2.2, -6.9);
	matPhang(6.5, 5, 0.02, model);
}
void san() {
	model = Translate(0, -0.31, -1.95);
	matPhang(6.5, 0.02, 10, model);
}


void bang() {
	model = Translate(0, 2.2, -6.85);
	matPhang(3, 3, 0.02, model);
}

GLfloat quayCuaChinh1 = 0, quayCuaChinh2 = 0;

void cuaChinh() {

	//canh cua 1 
	model =  Translate(-3.25,0,-5.25) * RotateY(quayCuaChinh1) * Translate(0, 0.4, 0.5 * 0.75);
	matPhang(0.02, 1.5, 0.75, model);

	//canh cua 2 
	model =  Translate(-3.25, 0, -3.75)*RotateY(180)  * RotateY(quayCuaChinh2) * Translate(0, 0.4, 0.5 * 0.75);
	matPhang(0.02, 1.5, 0.75, model);
}



GLfloat z,x;
void canPhong() {
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, 0) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, 0) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, -1.2) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, -1.2) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, -2.4) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, -2.4) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, -3.6) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, -3.6) * RotateY(90);
	caiBan();


	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, 0) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, 0) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -1.2) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, -1.2) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, -2.4) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -2.4) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, -3.6) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -3.6) * RotateY(-90);
	caiBan();

	//tu
	quayBase = Translate(-2.5, 0.6, -6) * RotateY(180);
	tu();

	//tuong 
	quayBase = Translate(0, 0, z);
	tuongTrai();
	quayBase = Translate(0, 0, z);
	tuongPhai();

	//san
	quayBase = Translate(0, 0, z) ;
	san();

	//tran nha
	quayBase = Translate(0, 0, z) ;
	tranNha();

	//tuong sau
	quayBase = Translate(0, 0, z) ;
	tuongSau();

	quayBase = Translate(0, 0, z) * Translate(0,0,9.9);
	tuongSau();

	//bang
	quayBase = Translate(0, 0, z);
	bang();
	
	//Man chieu
	quayBase = Translate(0, 0, z) * Translate(0, 2.45 ,0);
	manChieu();

	//ban giao vien
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -4.8) * RotateY(180);
	caiBan();

	//maychieu
	quayBase = Translate(0, 0, z) * Translate(0.28, 4, -2.25);
	mayChieu();

	//cua chinh
	quayBase = Translate(0, 0, z);
	cuaChinh();
	//cua phu
	quayBase = Translate(0, 0, z) * Translate(0,0,5);
	cuaChinh();

	//cua so
	quayBase = Translate(0, 0, z);
	cuaSo();
	quayBase = Translate(0, 0, z) * Translate(0,0,4.48);
	cuaSo();
	
}

void display( void )
{

    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	
	canPhong();
	

        /*Vẽ các tam giác*/
	glutSwapBuffers();									   
}



void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {

	//quay cửa
	case 'c':
		if (quayCuaChinh1 < 90)
		quayCuaChinh1 += 5;
		if (quayCuaChinh2 > -90)
		quayCuaChinh2 -= 5;
		break;

	case 'C':
		if (quayCuaChinh1 > 0)
		quayCuaChinh1 -= 5;
		if (quayCuaChinh2 < 0)
		quayCuaChinh2 += 5;
		break;

	//quay cửa sổ
	case 'v':
		if (quayCuaSo1 < 90)
			quayCuaSo1 += 5;
		if (quayCuaSo2 > -90)
			quayCuaSo2 -= 5;
		break;

	case 'V':
		if (quayCuaSo1 > 0)
			quayCuaSo1 -= 5;
		if (quayCuaSo2 < 0)
			quayCuaSo2 += 5;
		break;
	
	//kéo màn chiếu
	case 'k': {
		if (!checkkeo) {
			keo += 0.02;
			if (keo >= hM) {
				keo = hM;
				checkkeo = true;
			}
		}
		else {
			keo -= 0.02;
			if (keo <= 0) {
				keo = 0;
				checkkeo = false;
			}
		}
		break;
	}

	//kéo ghế
	case 'h': {
		if (!checkkeoghe) {
			keoghe -= 0.02;
			if (keoghe <= -0.4) {
				keoghe = -0.4;
				checkkeoghe = true;
			}
		}
		else {
			keoghe += 0.02;
			if (keoghe >= 0) {
				keoghe = 0;
				checkkeoghe = false;
			}
		}
		break;
	}

	//mở tủ
	case '1': {

		if (!checkMoTu[0]) {
			MoTu[0] += 5;
			if (MoTu[0] > 90) {
				MoTu[0] = 90;
				checkMoTu[0] = true;
			}
		}
		else {
			MoTu[0] -= 5;
			if (MoTu[0] < 0) {
				MoTu[0] = 0;
				checkMoTu[0] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '2': {
		if (!checkMoTu[1]) {
			MoTu[1] += 5;
			if (MoTu[1] > 90) {
				MoTu[1] = 90;
				checkMoTu[1] = true;
			}
		}
		else {
			MoTu[1] -= 5;
			if (MoTu[1] < 0) {
				MoTu[1] = 0;
				checkMoTu[1] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '3': {
		if (!checkMoTu[2]) {
			MoTu[2] += 5;
			if (MoTu[2] > 90) {
				MoTu[2] = 90;
				checkMoTu[2] = true;
			}
		}
		else {
			MoTu[2] -= 5;
			if (MoTu[2] < 0) {
				MoTu[2] = 0;
				checkMoTu[2] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '4': {
		if (!checkMoTu[3]) {
			MoTu[3] += 5;
			if (MoTu[3] > 90) {
				MoTu[3] = 90;
				checkMoTu[3] = true;
			}
		}
		else {
			MoTu[3] -= 5;
			if (MoTu[3] < 0) {
				MoTu[3] = 0;
				checkMoTu[3] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '5': {
		if (!checkMoTu[4]) {
			MoTu[4] += 5;
			if (MoTu[4] > 90) {
				MoTu[4] = 90;
				checkMoTu[4] = true;
			}
		}
		else {
			MoTu[4] -= 5;
			if (MoTu[4] < 0) {
				MoTu[4] = 0;
				checkMoTu[4] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '6': {
		if (!checkMoTu[5]) {
			MoTu[5] += 5;
			if (MoTu[5] > 90) {
				MoTu[5] = 90;
				checkMoTu[5] = true;
			}
		}
		else {
			MoTu[5] -= 5;
			if (MoTu[5] < 0) {
				MoTu[5] = 0;
				checkMoTu[5] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '7': {
		if (!checkMoTu[6]) {
			MoTu[6] += 5;
			if (MoTu[6] > 90) {
				MoTu[6] = 90;
				checkMoTu[6] = true;
			}
		}
		else {
			MoTu[6] -= 5;
			if (MoTu[6] < 0) {
				MoTu[6] = 0;
				checkMoTu[6] = false;
			}
		}
		glutPostRedisplay();
		break;
	}
	case '8': {
		if (!checkMoTu[7]) {
			MoTu[7] += 5;
			if (MoTu[7] > 90) {
				MoTu[7] = 90;
				checkMoTu[7] = true;
			}
		}
		else {
			MoTu[7] -= 5;
			if (MoTu[7] < 0) {
				MoTu[7] = 0;
				checkMoTu[7] = false;
			}
		}
		glutPostRedisplay();
		break;
	}


	case 'd': thetal -= 0.1; break;
	case 'a': thetal += 0.1; break;
	case 'w': z += 0.1; break;
	case 's': z -= 0.1; break;

	

	case 'x': l *= 1.1; r *= 1.1; break;
	case 'X': l *= 0.9; r *= 0.9; break;
	case 'y': b *= 1.1; t *= 1.1; break;
	case 'Y': b *= 0.9; t *= 0.9; break;
	case 'z': zNear *= 1.1; zFar *= 1.1; break;
	case 'Z': zNear *= 0.9; zFar *= 0.9; break;
	case 'r': radius *= 2.0; break;
	case 'R': radius *= 0.5; break;
	case 't': thetal += dr; break;
	case 'T': thetal -= dr; break;
	case 'p': phi += dr; break;
	case 'P': phi -= dr; break;


	case ' ': // reset values to their defaults
		l = -1.0;
		r = 1.0;
		b = -1.0;
		t = 1.0;
		zNear = 0.5;
		zFar = 3.0;
		radius = 1.0;
		thetal = 0.0;
		phi = 0.0;
		break;
	}
	glutPostRedisplay();
}


int main( int argc, char **argv )
{
	// main function: program starts here

    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a Room A1" );            

   
	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               

    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
    return 0;
}
