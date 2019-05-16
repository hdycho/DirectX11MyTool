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


//������ : ��� �ڽĳ�带 Ž��
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


//������: ù��° ��尡 true�� ��ȯ�ϸ� �״��� ���γѾ
//		 ���� �׷����ʴٸ� ������
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


//===================================�ӱ��̳��=============================//
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


	int ID;				//	�������̵�
	char Name[32];		//	���(�ൿ)�̸�
	ImVec2 Pos;			//	��ġ
	ImVec2 Size;		//	������
	int FromChildCount;	//	������ �����ڽİ���
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

	ImVec2 GetOutputSlotPos(int slot_no) const // �ش縵ũ������ �����ǹ�ȯ
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

	void ImguiRender(); // ������ ���׷�������� �����Ҽ�����
	void ClearBehaviorTree(); // ������ ���׷����� ��������� �Լ�

							  // ������ �����ϴ��Լ�
	void SaveBehavior();

	// �̸����� �ִ��Լ�:�÷��̾��ʱ�ȭ�Ҷ����
	void InputBehaviorsName(vector<wstring>&names);

	// ��Ʈ���� �����ϴ� �Լ�
	void GetChildNode(CompositeNode*Onode, CompositeNode*Cnode);

	// xml�� �����ϴ� �Լ�
	void SaveTree(Xml::XMLElement * parent, Xml::XMLElement * element, Xml::XMLDocument*document, CompositeNode*node);

	// ��ü���� �����̹�Ʈ�� �д��Լ�
	void ReadBehaviorTree(Xml::XMLElement * element, CompositeNode*node);

	// ��Ʈ�������� �ӱ��̳��� �о���� �Լ�
	void GetImguiTreeNode(CompositeNode*node, int curIdx, int parentIdx, int parentSibCount);
	void GetImguiLink();
private:
	Sequence*root;

	vector<wstring> behaviorsName;

	Xml::XMLElement*readBehavior;
private:
	//======================�ӱ��̳��======================//
	ImVector<ImGuiNode> nodes;
	ImVector<NodeLink> links;	// �θ�->�ڽ������� ����
	ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	bool show_grid = true;
	int node_selected = -1;	//	������ �����̵�
};