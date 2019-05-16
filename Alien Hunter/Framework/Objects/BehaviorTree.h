#pragma once

class Node
{
public:
	Node() {}
	virtual ~Node() {}
	virtual bool Invoke(function<bool(wstring)> func) = 0;
	wstring name;
	wstring type;

	virtual void Remove() = 0;
};

class CompositeNode : public Node
{
public:
	void AddChild(Node *node)
	{
		mChildren.emplace_back(node);
	}
	list<Node*>& GetChildren()
	{
		return mChildren;
	}

private:
	list<Node*> mChildren;
};


//셀렉터 : 모든 자식노드를 탐색
class Selector : public CompositeNode
{
public:
	Selector()
	{
		type = L"Selector";
		name = L"Selector";
	}
	virtual bool Invoke(function<bool(wstring)> func) override
	{
		for (auto node : GetChildren())
		{
			if (node->Invoke(func))
				return true;
		}
		return false;
	}

	virtual void Remove()
	{
		list<Node*>::iterator iter = GetChildren().begin();

		for (; iter != GetChildren().end(); iter++)
		{
			(*iter)->Remove();
			SAFE_DELETE(*iter);
		}
		GetChildren().clear();
	}


};


//시퀀스: 첫번째 노드가 true를 반환하면 그다음 노드로넘어감
//		 만약 그렇지않다면 종료함
class Sequence : public CompositeNode
{
public:
	Sequence()
	{
		type = L"Sequence";
		name = L"Sequence";
	}
	virtual bool Invoke(function<bool(wstring)> func) override
	{
		for (auto node : GetChildren())
		{
			if (!node->Invoke(func))
				return false;
		}
		return true;
	}

	virtual void Remove()
	{
		list<Node*>::iterator iter = GetChildren().begin();

		for (; iter != GetChildren().end(); iter++)
		{
			(*iter)->Remove();
			SAFE_DELETE(*iter);
		}
		GetChildren().clear();
	}
};

class Tesk : public CompositeNode
{
public:
	Tesk()
	{
		type = L"Tesk";
	}
	virtual ~Tesk(){}
	virtual bool Invoke(function<bool(wstring)> func) override
	{
		if (func(name))
			return true;
		else
			return false;
	}

	virtual void Remove()
	{
		
	}
};


//===================================임구이노드=============================//
struct NodeLink
{
	int FromChildIdx, FromChildSlot;
	int ToChildIdx, ToChildSlot;
	int sibCount = 0;

	bool isMovingList = false;

	NodeLink() {}
	NodeLink(int fromIdx, int fromSlot)
	{
		FromChildIdx = fromIdx;
		FromChildSlot = fromSlot;
		ToChildIdx = 0;
		ToChildSlot = 0;
	}
};

struct ImGuiNode
{
	enum NodeType
	{
		Root,
		Selector,
		Sequence,
		Tesk
	}nType;


	int ID;				//	고유아이디
	char Name[32];		//	노드(행동)이름
	ImVec2 Pos;			//	위치
	ImVec2 Size;		//	사이즈
	int FromChildCount;	//	밑으로 내릴자식갯수
	int ToChildCount;
	int parentIdx;

	ImGuiNode() {}
	ImGuiNode(int id, char*name, const ImVec2& pos)
		:ID(id)
		, Pos(pos)
		, FromChildCount(1)
		, ToChildCount(0)
		, parentIdx(-1)
	{
		strncpy(Name, name, 31);
		Name[31] = 0;
	}

	ImVec2 GetInputSlotPos(int slot_no) const
	{
		return ImVec2(Pos.x + Size.x * ((float)slot_no + 1) / ((float)FromChildCount + 1), Pos.y);
	}

	ImVec2 GetOutputSlotPos(int slot_no) const // 해당링크슬롯의 포지션반환
	{
		return ImVec2(Pos.x + Size.x * ((float)slot_no + 1) / ((float)ToChildCount + 1), Pos.y + Size.y);
	}
};

//==========================================================================//


class BehaviorTree
{
public:
	BehaviorTree();
	BehaviorTree(BehaviorTree&copy);
	~BehaviorTree();

	Sequence*CreateSequence(wstring name);
	Selector*CreateSelector(wstring name);
	Tesk*CreateTesk(wstring name);

	Sequence*&RootNode() { return root; }

	void Update(function<bool(wstring)> func);

	void ImguiRender(); // 툴에서 노드그래프띄워서 설정할수있음
	void ClearBehaviorTree(); // 툴에서 노드그래프들 전부지우는 함수

							  // 툴에서 저장하는함수
	void SaveBehavior();

	// 이름정보 넣는함수:플레이어초기화할때사용
	void InputBehaviorsName(vector<wstring>&names);

	// 루트부터 복사하는 함수
	void GetChildNode(CompositeNode*Onode, CompositeNode*Cnode);

	// xml로 저장하는 함수
	void SaveTree(Xml::XMLElement * parent, Xml::XMLElement * element, Xml::XMLDocument*document, CompositeNode*node);

	// 객체에서 비헤이버트리 읽는함수
	void ReadBehaviorTree(Xml::XMLElement * element, CompositeNode*node);

	// 루트에서부터 임구이노드로 읽어오는 함수
	void GetImguiTreeNode(CompositeNode*node, int curIdx, int parentIdx, int parentSibCount);
	void GetImguiLink();
private:
	Sequence*root;

	vector<wstring> behaviorsName;

	Xml::XMLElement*readBehavior;
private:
	//======================임구이노드======================//
	ImVector<ImGuiNode> nodes;
	ImVector<NodeLink> links;	// 부모->자식으로의 간선
	ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	bool show_grid = true;
	int node_selected = -1;	//	선택한 노드아이디
};