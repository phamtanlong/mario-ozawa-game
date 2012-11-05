#include "Sprite.h"
#include "Global.h"


Sprite::Sprite(TEXTURE* texture, int start, int end, int timeAnimation)
	: _texture(texture)
{
	_start = start;
	_end = end;
	_timeAni = timeAnimation;
	_index = start;
	_timeLocal = 0;
}

Sprite::Sprite(TEXTURE* texture, int timeAnimation)
	: _texture(texture)
{
	_start = 0;
	_end = _texture->Count - 1;
	_timeAni = timeAnimation;
	_index = 0;
	_timeLocal = 0;
}

Sprite::~Sprite()
{
	delete _texture;
}

void Sprite::Next() 
{
	_index++;
	if(_index > _end)
		_index = _start;
}

void Sprite::Reset()
{
	_index = _start;
	_timeLocal = 0;
}

void Sprite::SelectIndex(int index){
	_index = index;
}

void Sprite::Update(int time)
{
	_timeLocal += time;

	if(_timeLocal >= _timeAni){
		_timeLocal = 0;
		//
		this->Next();
	}
}

void Sprite::Render(int X, int Y)
{
	RECT srect;

	srect.left = (_index % _texture->Cols)*(_texture->Width);// + 1;
	srect.top = (_index / _texture->Cols)*(_texture->Height);// + 1;
	srect.right = srect.left + _texture->Width;
	srect.bottom = srect.top + _texture->Height;// + 1;

	D3DXVECTOR3 position((float)X, (float)Y, 0);

	GLSpriteHandler->Draw(
		_texture->Texture,
		&srect,
		NULL,
		&position,
		0xFFFFFFFF //color
	);
}


void Sprite::RenderScale(int x, int y)
{
	D3DXMATRIX oldMt;
	GLSpriteHandler->GetTransform(&oldMt);
	
	D3DXMATRIX newMt;
	D3DXVECTOR2 center = D3DXVECTOR2(x + _texture->Width / 2, y + _texture->Height / 2);
	D3DXVECTOR2 rotate = D3DXVECTOR2(-1, 1);

	D3DXMatrixTransformation2D(&newMt, &center, 0.0f, &rotate, NULL, 0.0f, NULL);
	D3DXMATRIX finalMt = newMt * oldMt ;
	GLSpriteHandler->SetTransform(&finalMt);

	this->Render(x, y);

	GLSpriteHandler->SetTransform(&oldMt);
}

// tieu nun
void Sprite::RenderScaleY(int x, int y)
{
	D3DXMATRIX oldMt;
	GLSpriteHandler->GetTransform(&oldMt);

	D3DXMATRIX newMt;
	D3DXVECTOR2 center = D3DXVECTOR2(x + _texture->Width / 2, y + _texture->Height / 2);
	D3DXVECTOR2 rotate = D3DXVECTOR2(1, -1);

	D3DXMatrixTransformation2D(&newMt, &center, 0.0f, &rotate, NULL, 0.0f, NULL);
	D3DXMATRIX finalMt = newMt * oldMt ;
	GLSpriteHandler->SetTransform(&finalMt);

	this->Render(x, y);

	GLSpriteHandler->SetTransform(&oldMt);
}

// tieu nun
void Sprite::RenderRect(int X, int Y,RECT Rect)
{
	D3DXVECTOR3 position((float)X, (float)Y, 0);
	GLSpriteHandler->Draw(
		_texture->Texture,
		&Rect,
		NULL,
		&position,
		D3DCOLOR_XRGB(255, 255, 255)
		);
}

void Sprite::RenderTransform(int x, int y, D3DXVECTOR2 scale, float degRotate, float depth)
{
	RECT srect;

	srect.left = (_index % _texture->Cols)*(_texture->Width) + 1;
	srect.top = (_index / _texture->Cols)*(_texture->Height) + 1;
	srect.right = srect.left + _texture->Width ;
	srect.bottom = srect.top + _texture->Height + 1;

	//save old matrix
	D3DXMATRIX oldMt;
	GLSpriteHandler->GetTransform(&oldMt);

	//create new matrix
	D3DXMATRIX newMt;
	D3DXVECTOR2 center = D3DXVECTOR2(x + _texture->Width / 2, y + _texture->Height / 2);

	D3DXMatrixTransformation2D(
		&newMt,
		&center,			//scale center
		0.0f,				//scaling rotate
		&scale,				//scale
		&center,			//rotate center
		D3DXToRadian(degRotate),	//rotate degRotate ^o
		NULL);

	//multi
	D3DXMATRIX finalMt = newMt * oldMt ;
	GLSpriteHandler->SetTransform(&finalMt);

	GLSpriteHandler->Draw(
		_texture->Texture,
		&srect,
		NULL,
		&D3DXVECTOR3(x, y, depth),
		0xFFFFFFFF
		);

	GLSpriteHandler->SetTransform(&oldMt);
}