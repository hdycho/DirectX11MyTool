#pragma once

#include <Windows.h>
#include <assert.h>

//STL
#include <string>
#include <vector>
#include <list>
#include <map>
#include <stack>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <iterator>
#include <mutex>
#include <thread>
using namespace std;

// DirectWrite
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

//Direct3D
#include <dxgi1_2.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>
#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <d3dx11effect.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "effects11d.lib")

//ImGui
#include <imgui.h>
#include <imguiDx11.h>
#pragma comment(lib, "imgui.lib")

//Json Cpp
#include <json/json.h>
#pragma comment(lib, "jsoncpp.lib")
#pragma warning( disable : 4996 )


//DirectXTex
// 11은 이미지 몇개 안됨 추가 라이브러리 ms에 공개해준거 Texture
#include <DirectXTex.h>
#pragma comment(lib, "directxtex.lib")

const wstring Shaders = L"../../_Shaders/";
const wstring Effects = L"../../_Effect/";
const wstring Textures = L"../../_Textures/";
const wstring Assets = L"../../_Assets/";
const wstring Models = L"../../_Models/";
const wstring Datas = L"../../_Datas/";
const string Sounds = "../../_Sounds/";

#define SAFE_RELEASE(p){ if(p){ (p)->Release(); (p) = NULL; } }
#define SAFE_DELETE(p){ if(p){ delete (p); (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p){ if(p){ delete [] (p); (p) = NULL; } }

#include "./Systems/D3D.h"
#include "./Systems/DirectWrite.h"
#include "./Systems/Keyboard.h"
#include "./Systems/Mouse.h"
#include "./Systems/Time.h"

#include "./Renders/VertexLayouts.h"
#include "./Renders/CBuffer.h"
#include "./Renders/Effect.h"
#include "./Renders/Texture.h"
#include "./Renders/Context.h"
#include "./Renders/Material.h"
#include "./Renders/CsResource.h"
#include "./Renders/Render2D.h"
#include "./Renders/RenderTarget.h"
#include "./Renders/TrailRenderer.h"
#include "./Renders/ProgressBar.h"

#include "./Viewer/Viewport.h"
#include "./Viewer/RenderTargetView.h"
#include "./Viewer/DepthStencilView.h"
#include "./Viewer/Camera.h"
#include "./Viewer/Perspective.h"
#include "./Viewer/TPSCamera.h"

#include "./Utilities/Point.h"
#include "./Utilities/Rect.h"
#include "./Utilities/Json.h"
#include "./Utilities/String.h"
#include "./Utilities/Path.h"
#include "./Utilities/Math.h"
#include "./Utilities/Xml.h"
#include "./Utilities/BinaryFile.h"

#include "./Collider/CollisionContext.h"
#include "./Collider/ColliderElement.h"

#include "./Boundings/Ray.h"
#include "./Boundings/BBox.h"
#include "./Boundings/BSphere.h"

#include "./Draw/DebugLine.h"
#include "./Draw/MeshCube.h"
#include "./Draw/MeshCylinder.h"
#include "./Draw/MeshGrid.h"
#include "./Draw/MeshSphere.h"
#include "./Draw/MeshQuad.h"

#include "./Objects/BehaviorTree.h"

#include "./Manager/ColliderManager.h"
#include "./Manager/FileManager.h"
#include "./Manager/InstanceManager.h"
#include "./Manager/BulletManager.h"
#include "./Manager/SceneManager.h"
#include "./Manager/ParticleManager.h"
#include "./Manager/UIManager.h"
#include "./Manager/SoundManager.h"

#include "./Model/Model.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelMeshPart.h"
#include "./Model/ModelBone.h"
#include "./Model/ModelClip.h"
#include "./Model/ModelInstance.h"
#include "./Model/AnimInstance.h"

#include "./Objects/GameRender.h"
#include "./Objects/GameModel.h"
#include "./Objects/GameAnimator.h"
#include "./Objects/BoneSphere.h"
#include "./Objects/Player.h"
#include "./Objects/Enemy.h"
#include "./Objects/NearEnemy.h"
#include "./Objects/Boss.h"
#include "./Objects/Weapon.h"

typedef VertexTextureNormalTangentBlend ModelVertexType;






