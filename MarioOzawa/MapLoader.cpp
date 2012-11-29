﻿#pragma once

#include "MapLoader.h"
#include "Global.h"
#include "Brick.h"
#include "BrickBreak.h"
#include "BrickItem.h"
#include "BrickQuestion.h"
#include "Turtle.h"
#include "Coin.h"
#include "Fungi.h"

//macro to get byte in color
#define getA© ((©&0xff000000)>>24)
#define getR© ((©&0x00ff0000)>>16)
#define getG© ((©&0x0000ff00)>>8)
#define getB© (©&0x000000ff)

int MapLoader::_mapW = 0;
int MapLoader::_mapH = 0;
 int MapLoader::_mapNumber = 0; //số thứ tự của map
 int MapLoader::_gold = 0;
 int MapLoader::_life = 3;
 int MapLoader::_curForm = 0;
 int MapLoader::_nextForm = 0;
 int MapLoader::_marioState = 0; //state of mario

 int MapLoader::_mariox = 10; //vị trí của mario
 int MapLoader::_marioy = 10;

MapLoader::MapLoader(void)
{
	_mapW = 0;
	_mapH = 0;
}

MapLoader::~MapLoader(void)
{
	delete _mapTexture;
	delete[] _board;
}

void MapLoader::LoadSavedGameFormFile(LPCTSTR _filesavegame)
{
	_mapNumber = 1; //default map1.png

	ifstream fin(_filesavegame);
	if(fin != NULL)
	{
		//read file
		//dòng 1: map thứ mấy:
		fin>> _mapNumber;

		//gold
		fin>> _gold;

		//life
		fin>> _life;

		//curForm
		fin>> _curForm;

		//nextForm
		fin>> _nextForm;

		//state
		fin>> _marioState;

		//read mario x, y
		fin>> _mariox;
		fin>> _marioy;

		//load map from file map
		char mapfile[50];
		sprintf(mapfile, "map%d.png", _mapNumber);
		this->LoadMapFormFile(_mapNumber, false, true, false, true);

		//read game state
		//add to quadtree
		int id;
		int x;
		int y;

		while(!fin.eof())
		{
			fin>> id;
			fin>> x;
			fin>> y;

			//add to board
			_board[y][x] = id;
		}
	}
	else
	{
		_mapNumber = 1;
		//load map from file map
		this->LoadMapFormFile(_mapNumber, true, true, true, true);
	}

	fin.close();
}

HRESULT MapLoader::LoadMapFormFile (int mapNumber, bool isLoadMario, bool isLoadBackground, bool isLoadObjects, bool isLoadTileMap)
{
	_mapNumber = mapNumber;
	HRESULT	hr;
	D3DXIMAGE_INFO imageInfo;
	char fileName[50];
	sprintf(fileName, "map/map%d.png", mapNumber);
	hr = D3DXGetImageInfoFromFile( fileName , &imageInfo);

	if(FAILED(hr))
	{
		GLMessage("Load MAP failed!!!");
		return hr;
	}

	//Create a raw map as a texture file with a 32-bit format
	hr = D3DXCreateTextureFromFileEx( 
		GLDevice ,							// DirectX device
		fileName ,							// File Path
		imageInfo.Width ,					// width
		imageInfo.Height ,					// Height
		imageInfo.MipLevels ,				// MipLevels
		D3DUSAGE_DYNAMIC ,					// D3DUSAGE
		D3DFMT_A8R8G8B8 ,					// Format
		D3DPOOL_DEFAULT ,					// Pool
		D3DX_DEFAULT ,						// Filter
		D3DX_DEFAULT ,						// Mip filter
		0 ,									// Color key
		&imageInfo ,						// Image Info
		NULL ,								// NULL
		&_mapTexture );						// Output texture

	//Store the raw map width and height
	_mapW	= imageInfo.Width;
	_mapH	= imageInfo.Height;

	GL_MapW = _mapW * TILE;
	GL_MapH = _mapH * TILE;

	GL_MapTileW = _mapW;
	GL_MapTileH = _mapH;

	if(FAILED(hr))
		return hr;
	
	//load pixel ID to board
	//
	D3DLOCKED_RECT		rect;
	D3DSURFACE_DESC		desc;
	DWORD				i,j;
	D3DCOLOR**			pColor;

	//First get the D3DSURFACE_DESC of this raw map
	hr = _mapTexture->GetLevelDesc( 0 , &desc );
	if(FAILED(hr))
		return hr;

	//We create a new D3DCOLOR 2D array has enough room to hold pixel color 
	pColor = new D3DCOLOR*[desc.Height];
	for ( i = 0 ; i < desc.Height ; ++i)
		pColor[i] = new D3DCOLOR[desc.Width];

	//Lock entire texture to read pixel
	hr = _mapTexture->LockRect( 0 , &rect , NULL , D3DLOCK_READONLY );
	if(FAILED(hr))
		return hr;

	BYTE* pBits = (BYTE*)rect.pBits;

	for (i = 0 ; i < desc.Height ; ++i)
	{
		for ( j = 0 ; j < desc.Width ; ++j)
		{
			DWORD index =( j*4 + ( i * (rect.Pitch) ));

			// Blue
			BYTE b = pBits[index];

			// Green
			BYTE g = pBits[index+1];

			// Red
			BYTE r = pBits[index+2];

			// Alpha
			BYTE a = pBits[index+3];
			D3DCOLOR color = D3DCOLOR_ARGB(a , r , g , b );
			pColor[i][j] = color;
		}
	}
	//Unlock texture
	_mapTexture->UnlockRect(0);

	//----------------------
	_board = new char*[_mapH];

	for(int i = 0; i < _mapH; i++)
	{
		_board[i] = new char[_mapW];
	}

	//gan gia tri ve 0
	for (int i = 0 ; i < _mapH; i++)	{
		for (int j = 0 ; j < _mapW; j++)	{			 
			_board[i][j] = 0;
		}
	}

	//add pixel ID to board
	for (i = 0 ; i < _mapH ; ++i)
	{
		for ( j = 0 ; j < _mapW ; ++j)
		{
			int blue = (pColor[i][j] & 0x000000ff);
			
			//filter and insert
			if(blue == 1) //MARIO
			{
				if(isLoadMario)
				{
					_mariox = j;
					_marioy = i;
				}
			}

			if(blue >= MIN_OBJ_ID && blue <= MAX_OBJ_ID) //OBJECT ID
			{
				if(isLoadObjects)
				{
					_board[i][j] = blue;
				}
			}

			if(blue >= MIN_TILE_ID && blue <= MAX_TILE_ID) //TILE MAP ID
			{
				if(isLoadTileMap)
				{
					_board[i][j] = blue;
				}
			}

			if((blue >= MIN_BG_ID && blue <= MAX_BG_ID) ||
				(blue >= MIN_TILE_ID && blue <= MAX_TILE_ID))  //BACKGROUND ID
			{
				if(isLoadBackground)
				{
					_board[i][j] = blue;
				}
			}
		}
	}

	return S_OK;
}

void MapLoader::TranslateMap (QuadTree* quadtree, BackgroundManager* background, Mario* mario)
{
	//
	background->_mapW = _mapW;
	background->_mapH = _mapH;

	//
	TileMap::GetInst()->_mapW = this->_mapW;
	TileMap::GetInst()->_mapH = this->_mapH;

	//init array
	background->_board = new char*[_mapH];
	TileMap::GetInst()->_board = new char*[_mapH];

	for(int i = 0; i < _mapH; i++)
	{
		background->_board[i] = new char[_mapW];
		TileMap::GetInst()->_board[i] = new char[_mapW];
	}

	//gan gia tri ve 0
	for (int i = 0 ; i < _mapH; i++)	{
		for (int j = 0 ; j < _mapW; j++)	{
			background->_board[i][j] = 0;
			TileMap::GetInst()->_board[i][j] = 0;
		}
	}
	
	//mario
	mario->_x = _mariox * TILE; //debug
	mario->_y = _marioy * TILE;
	GL_CurForm = _curForm;
	GL_NextForm = _nextForm;
	mario->gold = _gold;
	mario->life = _life;
	mario->_State = (State)_marioState;

	//Insert Object
	int blue;
	for (int i = 0 ; i < _mapH ; ++i)
	{
		for (int j = 0 ; j < _mapW ; ++j)
		{
			blue = _board[i][j];

			//if(blue == 0)
			//	continue;

			//OBJECT ID
			if(blue >= MIN_OBJ_ID && blue <= MAX_OBJ_ID)
			{
				switch(blue)
				{
				case 151: //brick break
					quadtree->Insert(new brickBreak(j * TILE, i * TILE));
					break;

				case 152: //coin
					quadtree->Insert(new coin(j * TILE, i * TILE));
					break;

				case 153: //flower
					quadtree->Insert(new brickItem(j * TILE, i * TILE, EBrickItemKind::FLOWER));
					break;

				case 154: //fungi
					quadtree->Insert(new fungi(j * TILE, i * TILE));
					break;

				case 155: //brick question
					quadtree->Insert(new brickQuestion(j * TILE, i * TILE));
					break;

				case 156: //larger blue
					quadtree->Insert(new brickItem(j * TILE, i * TILE, EBrickItemKind::SHOOTER));
					break;

				case 157: //larger red
					quadtree->Insert(new brickItem(j * TILE, i * TILE, EBrickItemKind::LARGER));
					break;

				case 158: //turtle
					quadtree->Insert(new turtle(j * TILE, i * TILE));
					break;
				}
			}

			//BACKGROUND ID
			if((blue >= MIN_BG_ID && blue <= MAX_BG_ID) ||
				blue >= MIN_TILE_ID && blue <= MAX_TILE_ID)
			{
				background->_board[i][j] = blue;
			}

			//TILE MAP ID
			if(blue >= MIN_TILE_ID && blue <= MAX_TILE_ID)
			{
				TileMap::GetInst()->_board[i][j] = 1;
				
				if(blue == 103 || blue == 104 || blue == 105) //gound
				{
					if(i + 1 < _mapH)
						TileMap::GetInst()->_board[i + 1][j] = 1;
				}

				if(blue == 106) //pipe
				{
					if(i + 1 < _mapH && j + 1 < _mapW)
					{
						TileMap::GetInst()->_board[i + 1][j] = 1;
						TileMap::GetInst()->_board[i][j + 1] = 1;
						TileMap::GetInst()->_board[i + 1][j + 1] = 1;
					}
				}
			}

		}
	}
}

void MapLoader::SaveGameToFile(QuadTree* quadtree, Mario* mario, LPCTSTR fileToSave)
{
	ofstream fout(fileToSave, ios::end, ios_base::out);

	//mapNumber
	fout<< _mapNumber << endl;

	//gold
	fout<< mario->gold << endl;

	//life
	fout<< mario->life << endl;

	//curForm
	fout<< GL_CurForm << endl;

	//nextForm
	fout<< GL_NextForm << endl;

	//state
	fout<< (int)mario->_State << endl;

	//mario.x
	fout<< mario->_x << endl;

	//mario.y
	fout<< mario->_y << endl;

	//read from quadtree
	RECT rect;
	rect.left = 0;
	rect.top = 0;
	rect.right = GL_MapW;
	rect.bottom = GL_MapH;

	vector<MyObject*>* listObject = quadtree->_rootNode->GetObj(rect);

	for (std::vector<MyObject*>::iterator i = listObject->begin(); 
		i != listObject->end(); ++i)
	{
		MyObject* obj = (MyObject*)(*i);
		switch (obj->_ID)
		{
		case EObject::BRICK: //////////////////////////////////////////////////////////////////////////zs
			break;

		case EObject::BRICKBREAK: //151
			{
				fout<< 151 << " " << obj->_x / TILE << " " << obj->_y / TILE << endl;
			}
			break;

		case EObject::BRICKITEM:
			{
				brickItem* item = (brickItem*)obj;
				if (item->_item == EBrickItemKind::FLOWER) //153
				{

				} 
				else if (item->_item == EBrickItemKind::LARGER) //157
				{

				} 
				else if (item->_item == EBrickItemKind::SHOOTER) //156
				{

				}
			}
			break;

		case EObject::BRICKQUESTION: //155
			{
				fout<< 155 << " " << obj->_x / TILE << " " << obj->_y / TILE << endl;
			}
			break;

		case EObject::COIN: //152
			{
				fout<< 152 << " " << obj->_x / TILE << " " << obj->_y / TILE << endl;
			}
			break;

		case EObject::FUNGI: //154
			{
				fout<< 154 << " " << obj->_x / TILE << " " << obj->_y / TILE << endl;
			}
			break;

		case EObject::MARIO: //////////////////////////////////////////////////////////////////////////
			break;

		case EObject::PIPE: //////////////////////////////////////////////////////////////////////////
			break;

		case EObject::TURTLE: //158
			{
				fout<< 158 << " " << obj->_x / TILE << " " << obj->_y / TILE << endl;
			}
			break;
		}
	}

	fout.close();
}

