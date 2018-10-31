#include "stdafx.h"
#include "ExeportModel.h"
#include "../Fbx/Exporter.h"
#include "../Objects/GameModel.h"


ExeportModel::ExeportModel(ExecuteValues * values)
	: Execute(values)
{
	Fbx::Exporter*exporter = NULL;
	//Cube
	exporter = new Fbx::Exporter(Assets + L"Meshes/Cube.fbx");
	exporter->ExportMaterial(Models + L"/Meshes/Cube/", L"Cube.material");
	SAFE_DELETE(exporter);

	//Quad
	exporter = new Fbx::Exporter(Assets + L"Meshes/Quad.fbx");
	exporter->ExportMaterial(Models + L"/Meshes/Quad/", L"Quad.material");
	SAFE_DELETE(exporter);

	//Plane
	exporter = new Fbx::Exporter(Assets + L"Meshes/Plane.fbx");
	exporter->ExportMaterial(Models + L"/Meshes/Plane/", L"Plane.material");
	SAFE_DELETE(exporter);

	//Sphere
	exporter = new Fbx::Exporter(Assets + L"Meshes/Sphere.fbx");
	exporter->ExportMaterial(Models + L"/Meshes/Sphere/", L"Sphere.material");
	SAFE_DELETE(exporter);

	//Cylinder
	exporter = new Fbx::Exporter(Assets + L"Meshes/Cylinder.fbx");
	exporter->ExportMaterial(Models + L"/Meshes/Cylinder/", L"Cylinder.material");
	SAFE_DELETE(exporter);

	//Teapot
	exporter = new Fbx::Exporter(Assets + L"Meshes/Teapot.fbx");
	exporter->ExportMaterial(Models + L"/Meshes/Teapot/", L"Teapot.material");
	SAFE_DELETE(exporter);

	//Tank
	///*exporter = new Fbx::Exporter(Assets + L"Tank/Tank.fbx");
	//exporter->ExportMaterial(Models + L"/Tank/", L"Tank.material");
	//exporter->ExportMesh(Models + L"/Tank/", L"Tank.mesh");
	//SAFE_DELETE(exporter);*/

	//Model*model = new Model();
	//model->ReadMaterial(Models + L"/Tank/", L"Tank.material");
	//model->ReadMesh(Models + L"/Tank/", L"Tank.mesh");

}

ExeportModel::~ExeportModel()
{
}

void ExeportModel::Update()
{
}

void ExeportModel::PreRender()
{

}

void ExeportModel::Render()
{
}

void ExeportModel::PostRender()
{
	
}

void ExeportModel::ResizeScreen()
{
}

