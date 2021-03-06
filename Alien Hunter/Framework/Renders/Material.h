#pragma once

class Material
{
public:
	Material();
	Material(wstring effectFile);
	Material(Effect * effect);
	~Material();

	// --------------------------------------------------------------------- //
	//  Name
	// --------------------------------------------------------------------- //
	void Name(wstring val) { name = val; }   //  Set the name of the Material
	wstring Name() { return name; }         //  Set the name of the Material

	// --------------------------------------------------------------------- //
	//  Shader
	// --------------------------------------------------------------------- //
	Effect *GetEffect();   //  Get the shader file
	void SetEffect(wstring file);         //  Set the shader file(wstring)
	void SetEffect(string file);         //  Set the shader file(string)
	void SetEffect(Effect * effect);

	// --------------------------------------------------------------------- //
	//  Color
	// --------------------------------------------------------------------- //
	D3DXCOLOR GetAmbient() { return color.Ambient; }         //   Get ambient color
	void SetAmbient(D3DXCOLOR& color);                     //   Set ambient color
	void SetAmbient(float r, float g, float b, float a = 1.0f);   //   Set ambient color

	D3DXCOLOR GetDiffuse() { return color.Diffuse; }         //   Get diffuse color
	void SetDiffuse(D3DXCOLOR& color);                     //   Set diffuse color
	void SetDiffuse(float r, float g, float b, float a = 1.0f);   //   Set diffuse color

	D3DXCOLOR GetSpecular() { return color.Specular; }         //   Get specular color
	void SetSpecular(D3DXCOLOR& color);                     //   Set specular color
	void SetSpecular(float r, float g, float b, float a = 1.0f);//   Set specular color

											 // --------------------------------------------------------------------- //
	Texture *GetDiffuseMap() { return diffuseMap; }                     //   Get diffuse texture
	void SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set diffuse texture
	void SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set diffuse texture

	Texture *GetSpecularMap() { return specularMap; }                  //   Get specular texture
	void SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set specular texture
	void SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set specular texture

	Texture *GetNormalMap() { return normalMap; }                     //   Get normal texture
	void SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set normal texture
	void SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set normal texture

	Texture *GetDetailMap() { return detailMap; }                     //   Get detail texture
	void SetDetailMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set detail texture
	void SetDetailMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);   //   Set detail texture

public:
	void Clone(void **clone);   //   Clone

private:
	bool      bErase;
	wstring      name;         //   Material Name

	Effect      *effect;      //   Shader file

	Texture      *diffuseMap;   //   Diffuse Texture
	Texture      *specularMap;   //   Specular Texture
	Texture      *normalMap;      //   Normal Texture
	Texture      *detailMap;      //   Detail Texture

private:
	struct ColorDesc
	{
		D3DXCOLOR   Ambient;      //   Ambient Color
		D3DXCOLOR   Diffuse;      //   Diffuse Color
		D3DXCOLOR   Specular;      //   Specular Color
	} color;
	CBuffer * buffer;

private:
	ID3DX11EffectMatrixVariable*worldVariable;
	
	ID3DX11EffectMatrixVariable*lightView;
	ID3DX11EffectMatrixVariable*lightProjection;
	ID3DX11EffectMatrixVariable*shadowTransform;
	ID3DX11EffectShaderResourceVariable*shadowMap;

public:
	ID3DX11EffectMatrixVariable*WorldVariable(){return worldVariable;}
	ID3DX11EffectMatrixVariable*LightView(){return lightView;}
	ID3DX11EffectMatrixVariable*LightProjection(){return lightProjection;}
	ID3DX11EffectMatrixVariable*ShadowTransform(){return shadowTransform;}
	ID3DX11EffectShaderResourceVariable*ShadowMap(){return shadowMap;}
};