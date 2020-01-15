#pragma once
#include <igl/readOBJ.h>
#include <iostream>
#include <vector>
#include <string>
#include "Vector3.h"
#include <embree3/rtcore.h>
#include <embree3/rtcore_ray.h>
#include "image.h"

// Colorub型と互換
#define COL_RED    {255,0,0}
#define COL_BLACK  {0,0,0}
#define COL_WHITE  {255,255,255}
#define COL_ERROR  {236,113,240}
#define COL_BACKGOUND {107,124,143}


#ifndef STRUCT_CAMERA	// フラグが立っていなければ（ if HEADER_H is not defined ） {
#define STRUCT_CAMERA	//     フラグを立てる（ define HEADER_H ）
typedef struct {
	Vector3	view;
	Vector3	refe;
	Vector3	up;
	double	fovy;
} Camera;		//     }
#endif	




// - - - メッシュ情報とマテリアル情報をまとめたクラス - - - - - - - - - - - - - 
class MyObject {
private:
	
	typedef struct {
		double transparency = 0;	// 透過度:0で非透過
		double IOR = 1;				// 屈折率

		double reflectivity = 0;	// 反射率:0で反射無し
	}MyMaterial;

	int index = -1;
	int geomID = -1; // embreeでの識別ID
	
public:
	Eigen::MatrixXd V;			// 頂点の位置情報
	Eigen::MatrixXi F;			// 面を構成する頂点のインデックス
	Eigen::MatrixXd TC, N;
	Eigen::MatrixXi FTC, FN;

	RTCGeometry geom;
	MyMaterial mat;

	bool has_texture = false;
	Colorub color = COL_RED;
	bool smooth_shading = true;

	//  objファイルの読み込み (texture情報はmtlファイルから読んだ方がスマート)
	//  ※シーンへ登録してください.
	MyObject(std::string path, bool has_texture = false);

	void print_info();

	//  embree用識別IDの操作
	void set_geomID(int id);
	int get_geomID();

	void add_to_embree_scene(RTCDevice &device, RTCScene &scene);
	
};
// ---------------------------------------------------------------------------


// - - - オブジェクトをまとめたクラス - - - - - - - - - - - - - - - - - - - -
/*　テクスチャはMyScene以下に配置するので、参照したい場合はここから。
    将来的にはオブジェクトもMySceneのローカル変数にしたいけどオブジェクト名で呼べなくなるのが惜しい*/
class MyScene
{
private:
	bool camera_exist = false;
public:
	std::vector<MyObject> object;
	//std::vector<std::string> object_name;
	std::vector<Image> texture;

	Camera camera;

	void add_object_to_scene(MyObject &obj);

	void scene_info();

	void add_to_embree_scene(RTCDevice &device, RTCScene &scene);
	
	// geomIDからMyObjectの検索
	// アドレスを返したほうがいいのでは？
	int geomID_to_objectID(int id);

	int add_texture(std::string path);
	void make_camera();
	void add_camera(Camera &cam);
};
// ---------------------------------------------------------------------------
