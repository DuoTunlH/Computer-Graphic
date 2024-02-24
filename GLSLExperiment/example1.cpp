//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;
const int diemHinhTru = 720;

const int n = 30;
double PI = 3.14;

point4 points[NumPoints + diemHinhTru]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints + diemHinhTru]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints + diemHinhTru]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;
GLuint loc_modelMatrix;
GLuint loc_projection;
GLuint view_loc;
GLfloat l = -1.0, r = 1.0, t = 1.0, b = -1.0, zNear = 0.5, zFar = 6;
GLfloat radius = 1, thetal = 180, phi = 0;
GLfloat dr = 5.0 * M_PI / 180;
GLfloat theta[3] = { 0,0,0 };


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
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
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

void add(point4 A, point4 B, point4 C) {
	vec4 u = B - A;
	vec4 v = C - B;
	vec3 normal = normalize(cross(u, v));
	normals[Index] = normal;
	points[Index] = A;
	colors[Index] = vertex_colors[2];
	Index++;
	normals[Index] = normal;
	points[Index] = B;
	colors[Index] = vertex_colors[7];
	Index++;
	normals[Index] = normal;
	points[Index] = C;
	colors[Index] = vertex_colors[2];
	Index++;

}
void quadHinhTru(int n, float r1, float r2, float h, float h2) {
	const float apha = (2 * PI) / n;
	for (int i = 0; i < n; i++) {
		vec4 A = point4(r1 * cos((i - 2) * apha), h, r1 * sin((i - 2) * apha), 1.0);
		vec4 B = point4(r1 * cos((i - 1) * apha), h, r1 * sin((i - 1) * apha), 1.0);
		vec4 C = point4(r2 * cos((i - 2) * apha), h2, r2 * sin((i - 2) * apha), 1.0);
		vec4 D = point4(r2 * cos((i - 1) * apha), h2, r2 * sin((i - 1) * apha), 1.0);
		add(A, C, D);
		add(A, D, B);
	}
};

void initHinhTru(void) {

	quadHinhTru(n, 1.0f, 1.0f, -0.5f, 0.5f);
	quadHinhTru(n, 0.2f, 0.2f, -0.5f, 0.5f);
	quadHinhTru(n, 0.8f, 1.0f, 0.5f, 0.5f);
	quadHinhTru(n, 0.8f, 1.0f, -0.5f, -0.5f);

}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
	initHinhTru();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	//glBufferData( GL_ARRAY_BUFFER, sizeof(points)+sizeof(colors), NULL, GL_STATIC_DRAW );
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);


	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);

}
/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(0.0, 0.0, 1.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 1.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	loc_modelMatrix = glGetUniformLocation(program, "modelMatrix");
	loc_projection = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");
	glEnable(GL_DEPTH_TEST);

	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

GLfloat z, x;
vec4 mautu = vec4(0.9, 0.9, 0.9, 1.0);
vec4 maucuatu = vec4(0.8, 0.8, 0.8, 1.0);
vec4 mautaykeocuatu = vec4(0.5, 0.5, 0.4, 1.0);
vec4 maumaychieu = vec4(1.0, 0.0, 1.0, 1.0);
vec4 maudieuhoa = vec4(0.6, 0.7, 0.5, 1.0);
vec4 maubang = vec4(0.0, 0.0, 0.0, 1.0);
vec4 maumanchieu = vec4(1.0, 1.0, 1.0, 1.0);
vec4 maucua = vec4(1.0, 1.0, 0.0, 1.0);
vec4 maubanghe = vec4(1.0, 0.0, 0.0, 1.0);
vec4 maumaytinh = vec4(0.0, 1.0, 0.0, 1.0);
vec4 mautuong = vec4(0.0, 1.0, 1.0, 1.0);
vec4 mautrang = vec4(1.0, 1.0, 1.0, 1.0);
vec4 mauden = vec4(0, 0, 0.0, 1.0);

GLfloat xcam, ycam, zcam;
void matPhang(GLfloat x, GLfloat y, GLfloat z, mat4 mt, vec4 colorCode) {

	material_diffuse = colorCode;
	diffuse_product = light_diffuse * material_diffuse;

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	mat4 ins = Scale(x, y, z);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, quayBase * mt * ins);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void hinhTru(GLfloat x, GLfloat y, GLfloat z, mat4 mt, vec4 colorCode) {

	material_diffuse = colorCode;
	diffuse_product = light_diffuse * material_diffuse;

	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	mat4 ins = Scale(x, y, z);
	glUniformMatrix4fv(loc_modelMatrix, 1, GL_TRUE, quayBase * mt * ins);
	glDrawArrays(GL_TRIANGLES, NumPoints, diemHinhTru);
}

point4 Rotate_at(point4 eye, point4 at, GLfloat theta) {
	GLfloat angle = DegreesToRadians * theta;

	//vec4 eye_at_vec = vec4(at.x - eye.x, at.y - eye.y, at.z - eye.z, 1.0);

	mat4 at_mat;
	at_mat[0][0] = at.x;
	at_mat[1][0] = at.y;
	at_mat[2][0] = at.z;
	at_mat[3][0] = 1.0;
	//mat4 eye_mat = Translate(eye.x, eye.y, eye.z);
	mat4 eye_mat;
	eye_mat[0][3] = eye.x;
	eye_mat[1][3] = eye.y;
	eye_mat[2][3] = eye.z;
	eye_mat[0][0] = eye_mat[1][1] = eye_mat[2][2] = eye_mat[3][3] = 1.0;

	//mat4 rotate_mat = RotateY(theta);
	mat4 rotate_mat;
	rotate_mat[2][2] = rotate_mat[0][0] = cos(angle);
	rotate_mat[0][2] = sin(angle);
	rotate_mat[2][0] = -rotate_mat[0][2];
	rotate_mat[1][1] = rotate_mat[3][3] = 1.0;

	//mat4 at_mat;
	//at_mat[3][0] = at.x;
	//at_mat[3][1] = at.y;
	//at_mat[3][2] = at.z;
	//at_mat[0][0] = at_mat[1][1] = at_mat[2][2] = at_mat[3][3] = 1;
	//Print("Mat");
	//Print((float) at_mat[3][0]);
	//Print((float) at_mat[3][1]);
	//Print((float) at_mat[3][2]);
	mat4 at2_mat = rotate_mat * at_mat * eye_mat;


	/*point4 at_point(at_mat[3][0], at_mat[3][1], at_mat[3][2], 1.0);*/

	point4 at_point(at2_mat[0][0], at2_mat[1][0], at2_mat[2][0], 1.0);
	return at_point;
}

point4 Rotate_eye(point4 eye, GLfloat theta) {
	GLfloat angle = DegreesToRadians * theta;

	//vec4 eye_at_vec = vec4(at.x - eye.x, at.y - eye.y, at.z - eye.z, 1.0);


	mat4 eye_mat;
	eye_mat[0][0] = eye.x;
	eye_mat[1][0] = eye.y;
	eye_mat[2][0] = eye.z;
	/*at_mat[3][0] = 1.0;*/
	//mat4 eye_mat = Translate(eye.x, eye.y, eye.z);
	//mat4 eye_mat;
	//eye_mat[0][3] = eye.x;
	//eye_mat[1][3] = eye.y;
	//eye_mat[2][3] = eye.z;
	/*eye_mat[0][0] = eye_mat[1][1] = eye_mat[2][2] = eye_mat[3][3] = 1.0;*/

	mat4 rotate_mat = RotateY(theta);
	//mat4 rotate_mat;
	//rotate_mat[2][2] = rotate_mat[0][0] = cos(angle);
	//rotate_mat[0][2] = sin(angle);
	//rotate_mat[2][0] = -rotate_mat[0][2];
	/*rotate_mat[1][1] = rotate_mat[3][3] = 1.0;*/

	//mat4 at_mat;
	//at_mat[3][0] = at.x;
	//at_mat[3][1] = at.y;
	//at_mat[3][2] = at.z;
	//at_mat[0][0] = at_mat[1][1] = at_mat[2][2] = at_mat[3][3] = 1;
	//Print("Mat");
	//Print((float) at_mat[3][0]);
	//Print((float) at_mat[3][1]);
	//Print((float) at_mat[3][2]);
	mat4 eye2_mat = rotate_mat * eye_mat;


	/*point4 at_point(at_mat[3][0], at_mat[3][1], at_mat[3][2], 1.0);*/

	point4 eye_point(eye2_mat[0][0], eye2_mat[1][0], eye2_mat[2][0], 1.0);
	return eye_point;

}

void khungNhin() {

	point4 eye(xcam, 1, zcam, 1.0);

	vec4 up(0, 1, 0, 1.0);

	point4 at(xcam, 1, zcam + 1, 1.0);

	point4 rotateEye = Rotate_eye(eye, thetal);

	point4 rotateAt = Rotate_at(eye, at, thetal);

	mat4 v = LookAt(rotateEye, rotateAt, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, v);

	mat4 p = Frustum(l, r, b, t, zNear, zFar);
	glUniformMatrix4fv(loc_projection, 1, GL_TRUE, p);
}


void truMayChieu() {
	model = Translate(0, 0.25, 0);
	matPhang(0.2, 0.5, 0.2, model, maumaychieu);
}
void hopMayChieu() {
	model = Translate(-0.5 * 0.025, 0, 0);
	matPhang(0.6, 0.05, 0.2, model, maumaychieu);
}

GLfloat wM = 1.5, hM = 2.5, dM = 0.05;
GLfloat wTN = 1.5, hTN = 0.1, dTN = 0.2;
GLfloat keo = 0;
bool checkkeo = false;

void manChieu() {
	model = Translate(0, 0.5 * keo, -6.8) * RotateY(180);
	matPhang(wM, hM - keo, dM, model, maumanchieu);
	model = Translate(0, 0.5 * hM + 0.5 * hTN, -6.8) * RotateY(180);
	matPhang(wTN, hTN, dTN, model, maumanchieu);
}


void mayChieu() {
	truMayChieu();
	hopMayChieu();
}

void khungDieuHoa() {
	model = Translate(1.2, 0.6, 1.7);
	matPhang(1.2, 0.05, 1.0, model, maudieuhoa);
}
void voDieuHoa() {
	model = Translate(1.2, 0.5, 1.7);
	matPhang(1.0, 0.05, 0.7, model, maudieuhoa);
}
void dieuHoa() {
	khungDieuHoa();
	voDieuHoa();
}


void matBan() {
	model = Translate(0, 0.3, 0);
	matPhang(0.8, 0.02, 0.4, model, maubanghe);
}

void chanBan() {
	//chan 1
	model = Translate(-0.375, -0.01, -0.2);
	matPhang(0.02, 0.6, 0.02, model, maubanghe);

	//chan 2
	model = Translate(-0.375, -0.01, 0.2);
	matPhang(0.02, 0.6, 0.02, model, maubanghe);

	//chan 3
	model = Translate(0.375, -0.01, -0.2);
	matPhang(0.02, 0.6, 0.02, model, maubanghe);

	//chan 4
	model = Translate(0.375, -0.01, 0.2);
	matPhang(0.02, 0.6, 0.02, model, maubanghe);

	//thang doc 1
	model = Translate(-0.375, -0.24, 0);
	matPhang(0.02, 0.02, 0.4, model, maubanghe);

	//thanh doc 2
	model = Translate(0.375, -0.24, 0);
	matPhang(0.02, 0.02, 0.4, model, maubanghe);

	//thanh ngang
	model = Translate(0, -0.24, 0);
	matPhang(0.78, 0.02, 0.02, model, maubanghe);

	//hop may tinh
	model = Translate(-0.2, 0.02, 0.05);
	matPhang(0.2, 0.35, 0.4, model, maumaytinh);
}

void MayTinh() {
	//De may tinh
	model = Translate(0, 0.3, 0);
	matPhang(0.3, 0.03, 0.02, model, maumaytinh);

	//Than may tinh
	model = Translate(0, 0.45, 0);
	matPhang(0.05, 0.25, 0.02, model, maumaytinh);

	//Man may tinh
	model = Translate(0, 0.7, 0);
	matPhang(0.35, 0.4, 0.04, model, maumaytinh);
}

GLfloat keoghe = 0;
bool checkkeoghe = false;
void matGhe() {
	model = Translate(0, 0, -0.5 - keoghe);
	matPhang(0.4, 0.02, 0.25, model, maubanghe);
	model = Translate(0, 0.15, -0.62 - keoghe) * RotateX(90);
	matPhang(0.4, 0.02, 0.25, model, maubanghe);
}

void chanGhe() {
	//chan 1
	model = Translate(-0.175, -0.15, -0.4 - keoghe);
	matPhang(0.02, 0.3, 0.02, model, maubanghe);

	//chan 2
	model = Translate(-0.175, -0.15, -0.6 - keoghe);
	matPhang(0.02, 0.3, 0.02, model, maubanghe);

	//chan 3
	model = Translate(0.175, -0.15, -0.4 - keoghe);
	matPhang(0.02, 0.3, 0.02, model, maubanghe);

	//chan 4
	model = Translate(0.175, -0.15, -0.6 - keoghe);
	matPhang(0.02, 0.3, 0.02, model, maubanghe);


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

GLfloat quayLaptop = 0;
int laptopState = 0;
bool isLaptopOpen = true;

void laptop() {
	//than duoi
	model = Translate(0, 0, 0);
	matPhang(0.6, 0.02, 0.5, model, mautu);

	//banphim
	model = Translate(0, 0.009, -0.03);
	matPhang(0.5, 0.005, 0.25, model, mautrang);

	//touchpad
	model = Translate(0, 0.009, 0.18);
	matPhang(0.25, 0.005, 0.09, model, mautrang);

	quayBase *= Translate(0, 0.02, -0.25) * RotateX(quayLaptop) * Translate(0, -0.02, 0.25);

	//than tren thanh doc
	model = Translate(-0.2925, 0.25, -0.25);
	matPhang(0.015, 0.5, 0.015, model, mautu);

	model = Translate(0.2925, 0.25, -0.25);
	matPhang(0.015, 0.5, 0.015, model, mautu);

	//than tren thanh ngang
	model = Translate(0, 0.025, -0.25);
	matPhang(0.6, 0.05, 0.01, model, mautu);

	model = Translate(0, 0.515, -0.25);
	matPhang(0.6, 0.03, 0.01, model, mautu);

	//man hinh
	vec4 mauManHinh = isLaptopOpen ? mautuong : vec4(0, 0, 0, 0);
	model = Translate(0, 0.28, -0.25);
	matPhang(0.57, 0.45, 0.005, model, mauManHinh);

	model = Translate(0, 0.28, -0.255);
	matPhang(0.57, 0.455, 0.001, model, mautu);

	model = Translate(0, 0.28, -0.255) * RotateX(90);
	hinhTru(0.1, 0.003, 0.1, model, mauden);
}

void banGiaoVien() {
	matBan();
	chanBan();
	caiGhe();
	mat4 tmpQuayBase = quayBase;
	quayBase *= Translate(-0.2, 0.35, 0) * RotateY(180) * Scale(0.5, 0.5, 0.5);
	laptop();
	quayBase = tmpQuayBase;
	quayBase *= RotateY(20) * Translate(0.25, 0, 0.15);
	MayTinh();

}

GLfloat heighTu = 1.8;
GLfloat widthTu = 0.6;
GLfloat day = 0.02;

void khungTu() {
	//Mặt trái
	model = Translate(-0.5 * widthTu - 0.5 * day, 0, 0);
	matPhang(day, heighTu, widthTu, model, mautu);
	//Mặt phải
	model = Translate(0.5 * widthTu + 0.5 * day, 0, 0);
	matPhang(day, heighTu, widthTu, model, mautu);
	//Mặt trên
	model = Translate(0, 0.5 * heighTu - 0.5 * day, 0);
	matPhang(widthTu, day, widthTu, model, mautu);
	//Mặt dưới
	model = Translate(0, -0.5 * heighTu + 0.5 * day, 0);
	matPhang(widthTu, day, widthTu, model, mautu);
	// mặt sau
	model = Translate(0, 0, 0.5 * widthTu - 0.5 * day);
	matPhang(widthTu, heighTu, day, model, mautu);

	// ngăn ngag tủ trên 
	model = Translate(0, 0.25 * heighTu, 0);
	matPhang(widthTu, day + 0.02, widthTu, model, mautu);
	// ngăn ngag tủ giữa 
	model = Translate(0, 0, 0);
	matPhang(widthTu, day + 0.02, widthTu, model, mautu);
	// ngăn ngag tủ dưới 
	model = Translate(0, -0.25 * heighTu, 0);
	matPhang(widthTu, day + 0.02, widthTu, model, mautu);

	// ngăn dọc giữa 
	model = Translate(0, 0, 0);
	matPhang(day + 0.02, heighTu, widthTu, model, mautu);
}
GLfloat widthCT = 0.5 * widthTu;
GLfloat heightCT = 0.25 * heighTu;
GLfloat MoTu = 0;
int tuState = 0;
void cuaTu() {
	// cửa 1 trái
	model = Translate(-widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(-widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day - day) * RotateY(MoTu) * Translate(widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
	model = Translate(-widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day - day) * RotateY(MoTu) * Translate(widthCT * 0.8, 0, -0.018 * 0.5) * RotateX(90);
	hinhTru(0.018, 0.02, 0.018, model, mauden);

	// cửa 2 trái
	model = Translate(-widthCT, 0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(-widthCT, 0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
	// cửa 3 trái
	model = Translate(-widthCT, -0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(-widthCT, -0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
	// cửa 4 trái
	model = Translate(-widthCT, -1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(-widthCT, -1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(MoTu) * Translate(widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);

	// cửa 1 phai
	model = Translate(widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(widthCT, 1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
	// cửa 2 phai
	model = Translate(widthCT, 0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(widthCT, 0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
	// cửa 3 phai
	model = Translate(widthCT, -0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(widthCT, -0.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
	// cửa 4 phai
	model = Translate(widthCT, -1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.5, 0, 0);
	matPhang(widthCT, heightCT - 0.025, day, model, maucuatu);
	model = Translate(widthCT, -1.5 * heightCT, -0.5 * widthTu + 0.5 * day) * RotateY(-MoTu) * Translate(-widthCT * 0.7, 0, -0.03 * 0.5);
	matPhang(0.02, 0.1, 0.03, model, mautaykeocuatu);
}
void tu() {
	khungTu();
	cuaTu();
}


void tuongTrai() {
	model = Translate(-3.25, 0.4, -0.35);
	matPhang(0.02, 1.5, 6.8, model, mautuong);

	model = Translate(-3.25, 0.4, -6.1);
	matPhang(0.02, 1.5, 1.7, model, mautuong);

	model = Translate(-3.26, 2.9, -1.95);
	matPhang(0.02, 3.5, 10, model, mautuong);
}

void tuongPhai() {

	model = Translate(3.25, 3.3, -1.95);
	matPhang(0.02, 2.65, 10, model, mautuong);

	model = Translate(3.25, 0.35, -1.95);
	matPhang(0.02, 1.3, 10, model, mautuong);

	model = Translate(3.25, 1.5, -5.97);
	matPhang(0.02, 1, 1.95, model, mautuong);

	model = Translate(3.25, 1.5, -3.15);
	matPhang(0.02, 1, 2.3, model, mautuong);

	model = Translate(3.25, 1.5, -0.15);
	matPhang(0.02, 1, 2.3, model, mautuong);

	model = Translate(3.25, 1.5, 2.35);
	matPhang(0.02, 1, 1.32, model, mautuong);
}

GLfloat quayCuaSo;
int cuaSoState = 0;
void cuaSo() {
	//khung doc
	model = Translate(0, 0, 0);
	matPhang(0.04, 1, 0.03, model, mautu);
	model = Translate(0, 0, 0.7);
	matPhang(0.04, 1, 0.03, model, mautu);
	//khung ngang
	model = Translate(0, 0.5, 0.35);
	matPhang(0.04, 0.04, 0.72, model, mautu);
	model = Translate(0, -0.5, 0.35);
	matPhang(0.04, 0.04, 0.72, model, mautu);

	//cua kinh
	mat4 tmpQuayBase = quayBase;
	quayBase *= Translate(0, 0.5, 0) * RotateZ(quayCuaSo) * Translate(0, -0.5, 0) ;

	model = Translate(0, 0, 0.04);
	matPhang(0.04, 1 - 2 * 0.04, 0.03, model, maucuatu);
	model = Translate(0, 0, 0.66);
	matPhang(0.04, 1 - 2 * 0.04, 0.03, model, maucuatu);


	model = Translate(0, 0.5 - 0.04, 0.35);
	matPhang(0.04, 0.04, 0.72 - 2*0.04, model, maucuatu);
	model = Translate(0, -0.5 + 0.04, 0.35);
	matPhang(0.04, 0.04, 0.72 - 2 * 0.04, model, maucuatu);

	//tay nam cua
	model = Translate(-0.08, -0.5 + 0.04, 0.35);
	matPhang(0.08, 0.04, 0.04, model, mauden);


	model = Translate(-0.08 - 0.08, -0.5 + 0.04, 0.35  + 0.15 * 0.5 - 0.02);
	matPhang(0.08, 0.04, 0.15, model, mauden);

	model = Translate(0, 0, 0.35);
	matPhang(0.02, 1, 0.7, model, mautrang);

	quayBase = tmpQuayBase;

	quayBase *= Translate(0, -0.5, 0) * RotateZ(-quayCuaSo) *Translate(0,0.5,0);
	model = Translate(0, -0.25, 0.03);
	matPhang(0.02, 0.5, 0.01, model, maucuatu);

	model = Translate(0, -0.25, 0.67);
	matPhang(0.02, 0.5, 0.01, model, maucuatu);


}
void tranNha() {
	model = Translate(0, 4.68, -1.95);
	matPhang(6.5, 0.02, 10, model, maucua);
}
void tuongSau() {
	model = Translate(0, 2.2, -6.9);
	matPhang(6.5, 5, 0.02, model, maucua);
}
void san() {
	model = Translate(0, -0.31, -1.95);
	matPhang(6.5, 0.02, 10, model, mautuong);
}


void bang() {
	model = Translate(0, 2.2, -6.85) * RotateY(180);
	matPhang(3, 3, 0.02, model, maubang);
}

GLfloat quayCuaChinh1 = 0, quayCuaChinh2 = 0;

void cuaChinh() {
	//canh cua 1 
	model = Translate(-3.25, 0, -5.25) * RotateY(quayCuaChinh1) * Translate(0, 0.4, 0.5 * 0.75);
	matPhang(0.02, 1.5, 0.75, model, maucua);

	//canh cua 2 
	model = Translate(-3.25, 0, -3.75) * RotateY(180) * RotateY(quayCuaChinh2) * Translate(0, 0.4, 0.5 * 0.75);
	matPhang(0.02, 1.5, 0.75, model, maucua);
}

void canPhong() {
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, 0) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, 0) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, -1.2) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, -1.2) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, -2.4) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, -2.4) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-0.23, 0, -3.6) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(0.23, 0, -3.6) * RotateY(-90);
	caiBan();


	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, 0) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, 0) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -1.2) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, -1.2) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, -2.4) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -2.4) * RotateY(90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(-1.8, 0, -3.6) * RotateY(-90);
	caiBan();
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -3.6) * RotateY(90);
	caiBan();

	//tu
	quayBase = Translate(0, 0, z) * Translate(-2.8, 0.6, -6.5) * RotateY(180);
	tu();

	//tuong 
	quayBase = Translate(0, 0, z);
	tuongTrai();
	quayBase = Translate(0, 0, z);
	tuongPhai();

	//san
	quayBase = Translate(0, 0, z);
	san();

	//tran nha
	quayBase = Translate(0, 0, z);
	tranNha();

	//tuong sau
	quayBase = Translate(0, 0, z);
	tuongSau();

	quayBase = Translate(0, 0, z) * Translate(0, 0, 9.9);
	tuongSau();

	//bang
	quayBase = Translate(0, 0, z);
	bang();

	//Man chieu
	quayBase = Translate(0, 0, z) * Translate(0, 2.45, 0);
	manChieu();

	//dieuhoa
	quayBase = Translate(0, 0, z) * Translate(0.28, 4, -2.25) * RotateY(-90);//phai duoi
	dieuHoa();
	quayBase = Translate(0, 0, z) * Translate(0.28, 4, -4.66) * RotateY(-90);//
	dieuHoa();
	quayBase = Translate(0, 0, z) * Translate(-0.28, 4, -2.25) * RotateY(90);//trai tren
	dieuHoa();
	quayBase = Translate(0, 0, z) * Translate(-0.28, 4, 0.16) * RotateY(90);//trai duoi
	dieuHoa();
	//ban giao vien
	quayBase = Translate(0, 0, z) * Translate(1.8, 0, -4.8);
	banGiaoVien();

	//maychieu
	quayBase = Translate(0, 0, z) * Translate(0.28, 4, -2.25);
	mayChieu();

	//cua chinh
	quayBase = Translate(0, 0, z);
	cuaChinh();

	//cua so
	quayBase = Translate(0, 0, z) * Translate(3.25, 1.5, 1);
	cuaSo();
	quayBase = Translate(0, 0, z) * Translate(3.25, 1.5, -2);
	cuaSo();
	quayBase = Translate(0, 0, z) * Translate(3.25, 1.5, -5);
	cuaSo();

}

void display(void)
{
	const vec3 viewer_pos(0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	canPhong();
	khungNhin();
	glutSwapBuffers();
}


void molaptop(int value) {
	if (laptopState == 0) {
		if (quayLaptop == 90) {
			laptopState = 1;
			return;
		}
		quayLaptop += 5;
	}
	if (laptopState == 1) {
		if (quayLaptop == -5) {
			laptopState = 0;
			return;
		}
		quayLaptop -= 5;
	}
	glutPostRedisplay();
	glutTimerFunc(100, molaptop, 0);
}

void motu(int value) {
	if (tuState == 0) {
		if (MoTu == 100) {
			tuState = 1;
			return;
		}
		MoTu += 5;
	}
	if (tuState == 1) {
		if (MoTu == 0) {
			tuState = 0;
			return;
		}
		MoTu -= 5;
	}
	glutPostRedisplay();
	glutTimerFunc(100, motu, 0);
}

void moCuaso(int value) {
	if (cuaSoState == 0) {
		if (quayCuaSo == 18) {
			cuaSoState = 1;
			return;
		}
		quayCuaSo += 2;
	}
	if (cuaSoState == 1) {
		if (quayCuaSo == 0) {
			cuaSoState = 0;
			return;
		}
		quayCuaSo -= 2;
	}
	glutPostRedisplay();
	glutTimerFunc(70, moCuaso, 0);
}

void camera_movement(unsigned char key) {
	switch (key) {
	case 'd': xcam -= 0.1; break;
	case 'a': xcam += 0.1; break;
	case 'w': zcam += 0.1; break;
	case 's': zcam -= 0.1; break;
	}
}


void camera_direction(int key, int a, int b) {
	switch (key) {
	case GLUT_KEY_LEFT: thetal -= 1; break;
	case GLUT_KEY_RIGHT: thetal += 1; break;
	}
	glutPostRedisplay();
}


void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
		//quay cửa - c
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

		//mo cua so - v
	case 'v':
		moCuaso(cuaSoState);
		break;
		//mo tu - o
	case 'o':
		motu(tuState);
		break;
		//gap laptop - l
	case 'l':
		molaptop(laptopState);
		break;
		//mo laptop - p
	case 'i':
		isLaptopOpen = !isLaptopOpen;
		break;
		//kéo màn chiếu - k
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

			//kéo ghế - h
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



			//case 'd': thetal -= 0.1; break;
			//case 'a': thetal += 0.1; break;
			//case 'w': z += 0.1; break;
			//case 's': z -= 0.1; break;



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
		zFar = 6.0;
		radius = 1.0;
		thetal = 0.0;
		phi = 0.0;
		//theta[0] = 0;
		//theta[1] = 0;
		//theta[2] = 0;
		break;
	}

	camera_movement(key);
	glutPostRedisplay();
}


int main(int argc, char** argv)
{
	// main function: program starts here

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Drawing a Room A1");


	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutSpecialFunc(camera_direction);
	glutKeyboardFunc(keyboard);

	glutMainLoop();
	return 0;
}
