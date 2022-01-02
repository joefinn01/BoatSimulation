#include "GameObject.h"
#include "Application.h"
#include "Structures.h"
#include "DDSTextureLoader.h"

GameObject::GameObject(XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale, char* meshFilePath, wchar_t* textureFilePath, Application* app, ConstantBuffer* cb)
{
	mPosition = position;
	mRotation = rotation;
	mScale = scale;

	mMeshFilePath = meshFilePath;
	mTextureFilePath = textureFilePath;

	mApp = app;

	mTexture = nullptr;

	mMaterial = new Material;

	mMaterial->AmbientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	mMaterial->DiffuseMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mMaterial->SpecularMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 10.0f);

	mCB = cb;

	mCB->mMaterial.AmbientMaterial = mMaterial->AmbientMaterial;
	mCB->mMaterial.DiffuseMaterial = mMaterial->DiffuseMaterial;
	mCB->mMaterial.SpecularMaterial = mMaterial->SpecularMaterial;
}

GameObject::~GameObject()
{
	delete mMaterial;
	mMaterial = nullptr;

	mApp = nullptr;
	mCB = nullptr;
	if (mTexture) mTexture->Release();
}

void GameObject::Load()
{
	mMesh = OBJLoader::Load(mMeshFilePath, GameObject::mApp->GetDevice());
	CreateDDSTextureFromFile(mApp->GetDevice(), mTextureFilePath, nullptr, &mTexture);
}

void GameObject::Update(float deltaTime)
{

}

XMFLOAT3 GameObject::GetPosition()
{
	return mPosition;
}

XMFLOAT3 GameObject::GetRotation()
{
	return mRotation;
}

XMFLOAT3 GameObject::GetScale()
{
	return mScale;
}

void GameObject::SetPosition(XMFLOAT3 position)
{
	mPosition = position;
}

void GameObject::SetRotation(XMFLOAT3 rotation)
{
	mRotation = rotation;
}

void GameObject::SetScale(XMFLOAT3 scale)
{
	mScale = scale;
}

void GameObject::Draw()
{
	XMFLOAT4X4 transformationMatrix;

	//Setting up constant buffer

	XMStoreFloat4x4(&transformationMatrix, XMMatrixScalingFromVector(XMLoadFloat3(&mScale)) * XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&mRotation)) * XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition)));

	mCB->mWorld = XMMatrixTranspose(XMLoadFloat4x4(&transformationMatrix));
	mCB->mView = XMMatrixTranspose(XMLoadFloat4x4(&mApp->GetSelectedCamera()->GetViewMatrix()));
	mCB->mProjection = XMMatrixTranspose(XMLoadFloat4x4(&mApp->GetSelectedCamera()->GetProjectionMatrix()));
	mCB->mMaterial.AmbientMaterial = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
 	mCB->mMaterial.DiffuseMaterial = mMaterial->DiffuseMaterial;
	mCB->mMaterial.SpecularMaterial = mMaterial->SpecularMaterial;
	mCB->EyePosW = mApp->GetSelectedCamera()->GetPosition();

	transformationMatrix._41 = 0.0f;
	transformationMatrix._42 = 0.0f;
	transformationMatrix._43 = 0.0f;
	transformationMatrix._44 = 1.0f;

	mCB->mWorldInverseTranspose = XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(XMLoadFloat4x4(&transformationMatrix)), XMLoadFloat4x4(&transformationMatrix)));

	//Remapping the buffer
	D3D11_MAPPED_SUBRESOURCE resource;

	HRESULT hr = mApp->GetDeviceContext()->Map(mApp->GetConstantBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, mCB, sizeof(ConstantBuffer));
	mApp->GetDeviceContext()->Unmap(mApp->GetConstantBuffer(), 0);

	mApp->GetDeviceContext()->PSSetShaderResources(0, 1, &mTexture);

	mApp->GetDeviceContext()->IASetVertexBuffers(0, 1, &mMesh.VertexBuffer, &mMesh.VBStride, &mMesh.VBOffset);
	mApp->GetDeviceContext()->IASetIndexBuffer(mMesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	mApp->GetDeviceContext()->VSSetShader(mApp->GetVertexShader(), nullptr, 0);
	mApp->GetDeviceContext()->PSSetShader(mApp->GetPixelShader(), nullptr, 0);

	ID3D11Buffer* tempCB = mApp->GetConstantBuffer();

	mApp->GetDeviceContext()->VSSetConstantBuffers(0, 1, &tempCB);
	mApp->GetDeviceContext()->PSSetConstantBuffers(0, 1, &tempCB);

	mApp->GetDeviceContext()->DrawIndexed(mMesh.IndexCount, 0, 0);
}
