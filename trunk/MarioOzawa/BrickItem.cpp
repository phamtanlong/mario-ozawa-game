
#include "SoundManager.h"
#include "BrickItem.h"
#include "ResourceManager.h"
#include "TileMap.h"

brickItem::brickItem(float x, float y,EBrickItemKind kindOfItem) : MyObject(x, y)
{
	_curSprite = new Sprite(ResourceMng::GetInst()->GetTexture("image/Question.png"), 500);
	_ID = EObject::BRICKITEM;
	_curSprite->SelectIndex(1);
	_curSprite->_start = 1;
	_curSprite->_end = 2;
	_State = hasItem;
	_kindofitem = kindOfItem;
	_x = x;
	_y = y;
	_item = new Item(_x + 5,_y,_kindofitem);
}


brickItem::~brickItem(void)
{
	delete _curSprite;
	_curSprite = NULL;
}

void brickItem::Update(int time)
{
	if(_State == stand)
		return;
	if(_State == hasItem)
		_curSprite->Update(time);
	else _item->Update(time);
}

void brickItem::Render()
{
	if(_State != hasItem)
		_item->Render();
	_curSprite->Render((int)_x, (int)_y);
}

void brickItem::UpdateRealTimeCollision(int time,vector<MyObject*>*listcollision)
{
	//if(_State == stand)
	//	return;
	if(_State == hasItem)
		_curSprite->Update(time);
	else _item->UpdateRealTimeCollision(time,listcollision);
}

void brickItem::CheckCollision(MyObject* obj)
{
	if(_State != hasItem)
	{
		if(obj->_ID == EObject::BRICKBREAK)
			_item->CheckCollision(obj);
	}
}