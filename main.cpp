#include <iostream>
#include <vector>
#include <igl/readOBJ.h>
#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "vector3.h"
#include "image.h"
#include <iomanip>
#include <time.h>
#include "bmpIO.h"
#include "MyScene.h"
#include "crossing_judgment.h"



#define mesh_dir_path std::string("../../../mesh/")

#include <DirectXMath.h>
struct vertex_t
{
	DirectX::XMFLOAT4 position;  //�ʒu
	DirectX::XMFLOAT3 normal;    //�@��
	DirectX::XMFLOAT2 uv;        //�e�N�X�`�����W
};

/*
// EigenMatrix����Embree�ň����郁�b�V��(rtcMesh)�ɕϊ�
unsigned geomID MatrixXd_to_rtcMesh(
	Eigen::MatrixXd V,
	Eigen::MatrixXd F
) {

	unsigned geomID = rtcNewTriangleMesh2(scene, RTC_GEOMETRY_STATIC, ntri, nvert);
	return
}*/


// - - - print  - - - - - - - - - - - - -
void myprint(float* list, string name = "noname") {
	std::cout << name << " ";
	for (int i = 0; i < sizeof(list); i++) {
		std::cout << list[i] << " ";
	}
	std::cout << std::endl;
}


//- - - - ���C�g�\���� - - - - - - - - - 
typedef struct {
	Vector3 position;
	Colorub color;
	char* name;
} PointLight;

//----------------------------------------------------------------------------
/*
Vector3 calcViewingRay(Camera& cam, int ix, int iy, int width, int height)
{
	Vector3 view_vec = cam.refe - cam.view;
	Vector3 side_vec = view_vec.cross(cam.up);
	Vector3 up_vec = side_vec.cross(view_vec);
	side_vec.normalize();
	up_vec.normalize();
	double vr = view_vec.length();
	double dp = (2.0 * vr * tan(cam.fovy / 2.0)) / (double)height;
	double xs = ((ix + 0.5f) - width / 2.0) * dp;
	double ys = ((iy + 0.5f) - height / 2.0) * dp;
	Vector3 ray = xs * side_vec + ys * up_vec + view_vec;
	ray.normalize();
	return(ray);
}
*/
// ---------------------------------------------------------------------------
/* �����_�̌v�Z */
float* calc_col_point( RTCRayHit rayhit, bool print = false) {
	float col_point[3];

	col_point[0] = rayhit.ray.org_x + (rayhit.ray.tfar * rayhit.ray.dir_x);
	col_point[1] = rayhit.ray.org_y + (rayhit.ray.tfar * rayhit.ray.dir_y);
	col_point[2] = rayhit.ray.org_z + (rayhit.ray.tfar * rayhit.ray.dir_z);

	if(print==true)	std::cout << "col_point : " << col_point[0] << " " << col_point[1] << " " << col_point[2] << std::endl;

	return (col_point);
}

// �l��RGB�ɕϊ�
Colorub convert_to_color(int index, int max_int) {
	Colorub col = {0,0,0};

	float x2 = float(index);
	float max = float(max_int);
	float r = x2 * 256.0 / max_int;

	col.r = int(r);

	return (col);
}

// �l(0~1)��RGB�ɕϊ�
Colorub convert_to_color(float f) {
	Colorub col = { 0,0,0 };

	col.r = (int)(f * 255);
	col.g = (int)(f * 255);
	col.b = (int)(f * 255);

	return(col);
}



// �x�N�g����RGB�ɕϊ�
Colorub convert_to_color(Vector3 v, bool positive = true, bool normalize = false) {
	Colorub col = { 0,0,0 };

	// �ǂꂩ�̗v�f���P�𒴂��Ă����ꍇ�̂ݐ��K��
	if (normalize) {
		v.normalize();
	}

	if (positive) {
		col.r = (int)(v.x * 255.0);
		col.g = (int)(v.y * 255.0);
		col.b = (int)(v.z * 255.0);
	}
	if (!positive) {
		col.r = (int)((v.x + 1.0) * 255.0 / 2.0);
		col.g = (int)((v.y + 1.0) * 255.0 / 2.0);
		col.b = (int)((v.z + 1.0) * 255.0 / 2.0);
	}
	// std::cout << v.x << " " << col.b << " " << col.g << " " << std::endl;

	return(col);
}


// diffusion���v�Z


int main(void) {
	clock_t start = clock();

	//--------- �V�[���쐬 -------------------
	MyScene myscene;

	//- - - - - - ���b�V���ǂݍ���- - - - - - - - - 
	MyObject sphere(mesh_dir_path + "sphere.obj", false);
	sphere.mat.transparency = 0.8;
	sphere.mat.IOR = 1.4;
	myscene.add_object_to_scene(sphere);

	//sphere.print_info();
	// - - - - - - - - - - - - - - - - - - - - - - -

	MyObject room(mesh_dir_path + "room.obj", true);
	myscene.add_object_to_scene(room);


	//�e�N�X�`���ǂݍ���
	std::string texture_path("C:/Users/piedp/Desktop/work/mesh/texture/ColorGrid_s_24.bmp");
	Image texture;
	myscene.add_texture(texture_path);
		
	/* �f�o�C�X���쐬 */
	RTCDevice device = rtcNewDevice(NULL);
	/* �V�[�����쐬 */
	RTCScene scene = rtcNewScene(device);

	// - - - - �J�������쐬 - - - - - - - - - - - - - - - - -
	myscene.make_camera();
	myscene.camera.view = Vector3(0.0, 0.0, 5);	    // ���_
	myscene.camera.refe = Vector3(0, 0.0, 0.0);        // �����_
	myscene.camera.up = Vector3(0, 1.0, 0.0);		// �A�b�v�x�N�g���H			
	myscene.camera.fovy = 45.0 / 180.0 * M_PI;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - -
		
	// ����
	PointLight light;
	light.position = Vector3(5, 5, 0);
	light.color = { 255,255,255 };

	myscene.add_to_embree_scene(device, scene);

	myscene.scene_info();

	// --------------------------------------------------------------------

	// - - - ���C�Ƃ̌������� - - - //

	Image image(512, 512);	// �摜�o�͗p

	crossing_judgement(image, scene, myscene);

	/* �V�[�����폜 */
	rtcReleaseScene(scene);
	/* �f�o�C�X���폜 */
	rtcReleaseDevice(device);


	char picturename[] = "out.bmp";
	image.save(picturename);



	clock_t end = clock();
	const double time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;
	std::cout << "time " << time << "[ms]" << std::endl;
	return 0;
}
/*
// ---------------------------------------------------------------------------
int mainl(void)
{

	clock_t start = clock();
	
	//�e�N�X�`���ǂݍ���
	char texture_path[] = "C:/Users/piedp/Desktop/work/mesh/texture/ColorGrid24.bmp";
	Image texture;
	if (texture.load(texture_path) == false) {
		fprintf(stderr, "Cant read texture\n");
		return(-1);
	}
	
	char testname[] = "test.bmp";
	texture.save(testname);

	// ���b�V���ǂݍ���
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	igl::readOBJ(mesh_dir_path + "randomSphere_small.obj", V, F);

	Eigen::MatrixXd V2, TC, N;
	Eigen::MatrixXi F2, FTC, FN;
	igl::readOBJ(mesh_dir_path + "randomSphere_small.obj", V2, TC, N, F2, FTC, FN);
	//std::cout << "TC : " << TC << std::endl;
	//std::cout << "N : " << N << std::endl;
	//std::cout << "FTC : " << FTC << std::endl;
	//std::cout << "FN : " << FN << std::endl;


	//std::cout <<"V : " << V << std::endl;
	//std::cout << "F : " << F << std::endl;


	// �f�o�C�X���쐬 
	RTCDevice device = rtcNewDevice(NULL);
	// �V�[�����쐬 
	RTCScene scene = rtcNewScene(device);

	// �J�������쐬
	Camera cam;
	cam.view = Vector3(0.0, 0.0, 5);	    // ���_
	cam.refe = Vector3(0, 0.0, 0.0);        // �����_
	cam.up   = Vector3(0, 1.0, 0.0);		// �A�b�v�x�N�g���H			
	cam.fovy = 45.0 / 180.0 * M_PI;

	// ����
	PointLight light;
	light.position = Vector3(5, 5, 0);
	light.color = { 255,255,255 };
	// -------------------- �V�[���̍\�� ---------------------------------------

	int ntri = F.rows();      // �V�[�����̎O�p�`�̐�
	int nvert = V.rows();     // �V�[�����̒��_�̑���
	
	// �R�p�`�W�I���g�����쐬 
	RTCGeometry geom = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
	
	// ���_���W���Z�b�g 
	int j = 0;
	float* vertices = (float*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, 3 * sizeof(float), nvert);
	for (int i = 0; i < nvert; i++) {
		vertices[j] = float(V(i, 0));
		vertices[j + 1] = float(V(i, 1));
		vertices[j + 2] = float(V(i, 2));
		j += 3;
	}

	// �ʂ��\�����钸�_�ԍ����Z�b�g
	j = 0;
	unsigned* indices = (unsigned*)rtcSetNewGeometryBuffer(geom, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(unsigned), ntri);
	for (int i = 0; i < ntri; i++) {
		indices[j] = F(i, 0);
		indices[j + 1] = F(i, 1);
		indices[j + 2] = F(i, 2);
		// std::cout << "F?" << i<< " "<< sizeof(indices) <<" "<<F(i, 0) << F(i, 1) << F(i, 2) << " "<< indices[i]<<" "<< indices[i3 + 1] << std::endl;
		j += 3;
	}

	// �V�[���֓o�^ /
	rtcCommitGeometry(geom);
	rtcAttachGeometry(scene, geom);
	rtcReleaseGeometry(geom);
	rtcCommitScene(scene); 


	
	


	
	//- - - ���C�Ƃ̌������� - - - 

	Image image(512,512);

	Vector3 ray = calcViewingRay(cam, 16, 16, image.getWidth(), image.getHeight());

	std::cout << "ray : " << ray.x <<" "<<ray.y<<" "<<ray.z<< endl;


	// ���C�𐶐����� 
	struct RTCRayHit rayhit;
	
	// ���C�̎n�_ 
	rayhit.ray.org_x = cam.view.x;  // x
	rayhit.ray.org_y = cam.view.y;  // y
	rayhit.ray.org_z = cam.view.z;  // z
	
	// �������肷��͈͂��w�� 
	rayhit.ray.tnear = 0.0f;     // �͈͂̎n�_
	rayhit.ray.tfar = INFINITY;  // �͈͂̏I�_�D���������ɂ͌����_�܂ł̋������i�[�����D

	rayhit.ray.flags = false;
	rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;

	// �������� 
	struct RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	
	for (int j = 0; j < image.getHeight(); j++) {
		for (int i = 0; i < image.getWidth(); i++) {

			ray = calcViewingRay(cam, i, j, image.getWidth(), image.getHeight());
			rayhit.ray.dir_x = ray.x;  // x
			rayhit.ray.dir_y = ray.y;  // y
			rayhit.ray.dir_z = ray.z;  // z

			rayhit.ray.flags = false;
			rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
			rayhit.ray.tfar = INFINITY;

			rtcIntersect1(scene, &context, &rayhit);

			//std::cout << "ij : " << i << " " << j ;
			//std::cout << "ray : " << ray.x << " " << ray.y << " " << ray.z <<" "  << rtcray.geomID << endl;

			// �摜�ɐF��o�^
			if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
				//Colorub temp = { int(rayhit.ray.tfar)*8 , 0, 0 };
				//image.setPixel(i, j, COL_RED);
				
				// uv���W���v�Z
				Eigen::RowVectorXi ftc = FTC.row(rayhit.hit.primID);
				
				Eigen::RowVector2d a = TC.row(ftc(0));
				Eigen::RowVector2d b = TC.row(ftc(1));
				Eigen::RowVector2d c = TC.row(ftc(2));

				
				
				Eigen::RowVector2d uv = a + (rayhit.hit.u * (b - a)) + (rayhit.hit.v * (c - a));
				// UV���o��
				//Vector3 uv3(uv(0), uv(1), 0);
				//image.setPixel(i, j, convert_to_color(uv3));

				// ��UV���o��
				//Vector3 uvtemp(rayhit.hit.u, rayhit.hit.v, 0);
				//image.setPixel(i, j, convert_to_color(uvtemp));
				
				
				
				
				if (0 > uv(0) || uv(0) > 1) {
					std::cout << "u_ERROR " << uv(0) << std::endl;
				}
				if (0 > uv(1) || uv(1) > 1) {
					std::cout << "v_ERROR " << uv(1) << std::endl;
				}
				// �e�N�X�`�����o��
				Colorub texcol = texture.getPixel( (int)((double)texture.getWidth()*uv(0)) , (int)((double)texture.getHeight()*uv(1)) );
				//Colorub texcol = texture.getPixel(i, j);
				image.setPixel(i, j, texcol);

				
			}
			else {
				image.setPixel(i, j, COL_BLACK);
			}
			
		}
		//std::cout << std::endl;
	}
	

	// �V�[�����폜 
	rtcReleaseScene(scene);
	// �f�o�C�X���폜 //
	rtcReleaseDevice(device);

	char picturename[] = "out.bmp";

	image.save(picturename);

	clock_t end = clock();
	const double time = static_cast<double>(end - start) / CLOCKS_PER_SEC * 1000.0;
	std::cout << "time " << time << "[ms]" << std::endl;

	return 0;
}
*/
