#include "Framework.h"
#include "BehaviorTree.h"

BehaviorTree::BehaviorTree()
{
	root = new Sequence();
	root->name = L"Root";
}

BehaviorTree::BehaviorTree(BehaviorTree & copy)
{
	root = new Sequence();
	root->name = L"Root";

	GetChildNode(copy.root, root);

	nodes.resize(30);
	GetImguiTreeNode(root, 0, -1, 0);
	GetImguiLink();

	nodes.resize(links[links.size() - 1].ToChildIdx + 1);
}

BehaviorTree::~BehaviorTree()
{
	root->Remove();

	SAFE_DELETE(root);
}

Sequence*BehaviorTree::CreateSequence(wstring name)
{
	Sequence*sequence = new Sequence();
	sequence->name = name;

	return sequence;
}

Selector*BehaviorTree::CreateSelector(wstring name)
{
	Selector*selector = new Selector();
	selector->name = name;

	return selector;
}

Tesk * BehaviorTree::CreateTesk(wstring name)
{
	Tesk*tesk = new Tesk();
	tesk->name = name;

	return tesk;
}

void BehaviorTree::Update(function<bool(wstring)> func)
{
	root->Invoke(func);
}

static ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }
void BehaviorTree::ImguiRender()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Create Behavior Tree"))
	{
		ImGui::End();
		return;
	}

	//���ʸ鿡 ����� ����Ʈ�� �����ش�
	bool open_context_menu = false; // ����Ʈ������ ���콺������ ������ �޴�������
	int node_hovered_in_list = -1;  // ����Ʈ���� ���콺�� �����ġ�� �ø���
	int node_hovered_in_scene = -1; // ������ ���콺�� �����ġ�� �ø���

	ImGui::BeginChild("[Node List]", ImVec2(100, 0));
	ImGui::Text("Behaivor Node");
	ImGui::Separator();
	if (ImGui::Button("All Clear"))
	{
		nodes.clear();
		links.clear();
		ClearBehaviorTree();
	}

	for (int nodeIndex = 0; nodeIndex < nodes.size(); nodeIndex++)
	{
		ImGuiNode* node = &nodes[nodeIndex];

		//Selectable������ Ǫ�����̵�����
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Name, node->ID == node_selected))
			node_selected = node->ID;

		//����Ʈ���� �ش������� ���콺�� �÷���������
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_list = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginGroup();

	const float NODE_SLOT_RADIUS = 4.0f;			//���Ե��׶��ũ��
	const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

	// �߾��˹��� ����
	ImGui::Text("Center Position : (%.2f,%.2f)", scrolling.x, scrolling.y);
	ImGui::SameLine();
	ImGui::Checkbox("Show grid", &show_grid);
	if (ImGui::Button("Save Behavior Tree"))
	{
		SaveBehavior();
	}
	ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Require Tesks] :");
	for (size_t i = 0; i < behaviorsName.size(); i++)
	{
		ImGui::SameLine();
		ImGui::BulletText("%s", String::ToString(behaviorsName[i]).c_str());
	}

	//�׷����� ����� ũ��� ����
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));

	//�߾��˹��� ������ ����� ���μӼ� �ʺ�
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);

	//�߾��˹��� �������� �������� ���ϱ�����
	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;

	//�˹����ȿ� ��峪 ���� �׸�������
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Display grid
	if (show_grid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
	}

	// Display links
	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0); // Background


	for (int j = 0; j < links.Size; j++)
	{
		NodeLink* link = &links[j];
		if (link->isMovingList) // ��尣���� ����ȉ�����
		{
			ImGuiNode* node_inp = &nodes[link->FromChildIdx];
			ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->FromChildSlot);
			ImVec2 p2 = ImGui::GetIO().MousePos;
			draw_list->AddBezierCurve(p1, p1 + ImVec2(0, 50), p2 + ImVec2(0, -50), p2, IM_COL32(200, 200, 100, 255), 3.0f);
		}
		else
		{
			ImGuiNode* node_inp = &nodes[link->FromChildIdx];
			ImGuiNode* node_out = &nodes[link->ToChildIdx];
			ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->FromChildSlot);
			ImVec2 p2 = offset + node_out->GetInputSlotPos(link->ToChildSlot);
			draw_list->AddBezierCurve(p1, p1 + ImVec2(0, 50), p2 + ImVec2(0, -50), p2, IM_COL32(200, 200, 100, 255), 3.0f);
		}
	}



	// Display nodes
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		ImGuiNode* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Node Name]");
		ImGui::Text("%s", node->Name);

		ImGui::TextColored(ImVec4(1, 0, 0, 1), "[Node Type]");
		if (node->nType == ImGuiNode::NodeType::Root)
		{
			ImGui::BulletText("Root");
		}
		if (node->nType == ImGuiNode::NodeType::Selector)
		{
			ImGui::BulletText("Selector");
		}
		if (node->nType == ImGuiNode::NodeType::Sequence)
		{
			ImGui::BulletText("Sequence");
		}
		if (node->nType == ImGuiNode::NodeType::Tesk)
		{
			ImGui::BulletText("Tesk");
		}

		ImGui::EndGroup();

		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_scene = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);

			if (links.size() >= 1)
			{
				if (ImGui::IsMouseClicked(0) && links[links.size() - 1].isMovingList)
				{

					links[links.size() - 1].isMovingList = false;
					links[links.size() - 1].ToChildIdx = node->ID;

					node->parentIdx = nodes[links[links.size() - 1].FromChildIdx].ID;
				}
			}

		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->ID;
		if (node_moving_active && ImGui::IsMouseDragging(0))
			node->Pos = node->Pos + ImGui::GetIO().MouseDelta;


		if (node->nType == ImGuiNode::NodeType::Root)
		{
			ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(20, 20, 150, 255) : IM_COL32(10, 10, 60, 255);
			draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
			draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(20, 20, 150, 255), 4.0f);
		}
		if (node->nType == ImGuiNode::NodeType::Selector)
		{
			ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(150, 150, 150, 255) : IM_COL32(60, 60, 60, 255);
			draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
			draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(150, 150, 150, 255), 4.0f);
		}
		if (node->nType == ImGuiNode::NodeType::Sequence)
		{
			ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(150, 150, 20, 255) : IM_COL32(60, 60, 10, 255);
			draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
			draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(150, 150, 20, 255), 4.0f);
		}
		if (node->nType == ImGuiNode::NodeType::Tesk)
		{
			ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(0, 200, 10, 255) : IM_COL32(0, 60, 10, 255);
			draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
			draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(0, 200, 10, 255), 4.0f);
		}

		for (int slot_idx = 0; slot_idx < node->FromChildCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		for (int slot_idx = 0; slot_idx < node->ToChildCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));

		ImGui::PopID();
	}
	draw_list->ChannelsMerge();

	// Open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu)
	{
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
			node_selected = node_hovered_in_list;
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		ImGuiNode* node = node_selected != -1 ? &nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::InputText("Name", node->Name, sizeof(node->Name));
			ImGui::Separator();
			if (ImGui::MenuItem("Add Child"))
			{
				node->ToChildCount++;
				links.push_back(NodeLink(node->ID, node->ToChildCount - 1));
				links[links.size() - 1].isMovingList = true;
			}
			if (ImGui::MenuItem("Delete"))
			{
				ImVector<ImGuiNode>::iterator iIter = nodes.begin();

				for (; iIter != nodes.end(); iIter++)
				{
					if (iIter->ID == node->ID)
					{
						nodes.erase(iIter);
						break;
					}
				}
				ImVector<NodeLink>::iterator lIter = links.begin();

				for (; lIter != links.end(); lIter++)
				{
					if (lIter->FromChildIdx == node->ID ||
						lIter->ToChildIdx == node->ID)
					{
						nodes[lIter->FromChildIdx].ToChildCount--;
						links.erase(lIter);
						break;
					}
				}
			}
		}
		else
		{
			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::MenuItem("Root"))
				{
					ImGuiNode node(nodes.Size, "Root", scene_pos);
					node.nType = ImGuiNode::NodeType::Root;

					nodes.push_back(node);
				}
				if (ImGui::MenuItem("Selector"))
				{
					ImGuiNode node(nodes.Size, "Selector", scene_pos);
					node.nType = ImGuiNode::NodeType::Selector;

					nodes.push_back(node);
				}
				if (ImGui::MenuItem("Sequence"))
				{
					ImGuiNode node(nodes.Size, "Sequence", scene_pos);
					node.nType = ImGuiNode::NodeType::Sequence;

					nodes.push_back(node);
				}
				if (ImGui::MenuItem("Tesk"))
				{
					ImGuiNode node(nodes.Size, "New Node", scene_pos);
					node.nType = ImGuiNode::NodeType::Tesk;

					nodes.push_back(node);
				}
				ImGui::EndMenu();
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;


	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::End();
}

void BehaviorTree::ClearBehaviorTree()
{
	root->Remove();
}

void BehaviorTree::SaveBehavior()
{
	root->Remove();
	vector<CompositeNode*> saveNodes;

	for (int i = 0; i < nodes.size(); i++)
	{
		if (nodes[i].nType == ImGuiNode::NodeType::Root)
		{
			wstring name = String::ToWString(nodes[i].Name);
			root->name = name;

			saveNodes.push_back(root);
		}
		else if (nodes[i].nType == ImGuiNode::NodeType::Sequence)
		{
			wstring name = String::ToWString(nodes[i].Name);
			Sequence*sel = CreateSequence(name);

			saveNodes.push_back(sel);
		}
		else if (nodes[i].nType == ImGuiNode::NodeType::Selector)
		{
			wstring name = String::ToWString(nodes[i].Name);
			Selector*sel = CreateSelector(name);

			saveNodes.push_back(sel);
		}
		else if (nodes[i].nType == ImGuiNode::NodeType::Tesk)
		{
			wstring name = String::ToWString(nodes[i].Name);
			Tesk*sel = CreateTesk(name);

			saveNodes.push_back(sel);
		}
	}


	for (size_t i = 0; i < saveNodes.size(); i++)
	{
		for (size_t j = 0; j < saveNodes.size(); j++)
		{
			if (i == j)continue;

			if (i == nodes[j].parentIdx)
			{
				saveNodes[i]->AddChild(saveNodes[j]);
			}
		}
	}
}

void BehaviorTree::InputBehaviorsName(vector<wstring>& names)
{
	behaviorsName.assign(names.begin(), names.end());
}

void BehaviorTree::GetChildNode(CompositeNode * Onode, CompositeNode * Cnode)
{
	//���� ��Ʈ�� �ڽĵ��� �ٺ����Ѵ�
	Cnode->GetChildren().assign(Onode->GetChildren().begin(), Onode->GetChildren().end());


	//�״��� ������������ �ڽĵ���� �ϳ��ϳ� ���Ž��
	list<Node*>::iterator oIter = Onode->GetChildren().begin();
	list<Node*>::iterator cIter = Cnode->GetChildren().begin();

	for (;oIter != Onode->GetChildren().end(); oIter++, cIter++)
	{
		CompositeNode*oNode = dynamic_cast<CompositeNode*>(*oIter);
		CompositeNode*cNode = dynamic_cast<CompositeNode*>(*cIter);

		GetChildNode(oNode, cNode);
	}

}

void BehaviorTree::SaveTree(Xml::XMLElement * parent, Xml::XMLElement * element, Xml::XMLDocument*document, CompositeNode*node)
{
	//��ó�� ��Ʈ���
	static Xml::XMLElement*sElement = element;

	sElement = document->NewElement(String::ToString(node->type).c_str());
	sElement->SetText(String::ToString(node->name).c_str());
	parent->LinkEndChild(sElement);

	sElement = document->NewElement("ChildCount");
	sElement->SetText(node->GetChildren().size());
	parent->LinkEndChild(sElement);

	sElement = document->NewElement("Child");
	parent->LinkEndChild(sElement);
	list<Node*>::iterator nIter;

	for (nIter = node->GetChildren().begin(); nIter != node->GetChildren().end(); nIter++)
	{
		Xml::XMLElement * childElement = NULL;
		childElement = document->NewElement(String::ToString((*nIter)->type).c_str());
		childElement->SetText(String::ToString((*nIter)->name).c_str());
		sElement->LinkEndChild(childElement);
	}

	for (nIter = node->GetChildren().begin(); nIter != node->GetChildren().end(); nIter++)
	{
		CompositeNode*child = dynamic_cast<CompositeNode*>(*nIter);

		if ((*nIter)->type == L"Selector" || (*nIter)->type == L"Sequence")
			SaveTree(parent, sElement, document, child);
	}
}

void BehaviorTree::ReadBehaviorTree(Xml::XMLElement * element, CompositeNode * node)
{
	readBehavior = element;
	readBehavior = readBehavior->NextSiblingElement(); // ����̸�
	wstring nodeName = String::ToWString(readBehavior->GetText());

	readBehavior = readBehavior->NextSiblingElement(); // �ڽİ���
	int childCount = readBehavior->IntText();

	readBehavior = readBehavior->NextSiblingElement(); // �ڽ�

	Xml::XMLElement * childElement = readBehavior->FirstChildElement();

	if (childElement == NULL)return;
	for (int i = 0; i < childCount; i++)
	{
		wstring childName = String::ToWString(childElement->GetText());
		if (childName == L"Selector")
		{
			Selector*selector = new Selector();
			selector->name = childName;

			node->AddChild(selector);
		}
		else if (childName == L"Sequence")
		{
			Sequence*sequence = new Sequence();
			sequence->name = childName;

			node->AddChild(sequence);
		}
		else
		{
			Tesk*tesk = new Tesk();
			tesk->name = childName;

			node->AddChild(tesk);
		}
		childElement = childElement->NextSiblingElement();
	}

	list<Node*>::iterator nIter = node->GetChildren().begin();

	for (; nIter != node->GetChildren().end(); nIter++)
	{
		CompositeNode*child = dynamic_cast<CompositeNode*>(*nIter);

		if ((*nIter)->type == L"Selector" || (*nIter)->type == L"Sequence")
			ReadBehaviorTree(readBehavior, child);
	}
}

void BehaviorTree::GetImguiTreeNode(CompositeNode * node, int curIdx, int parentIdx, int parentSibCount)
{
	int nodeId = curIdx;
	int curParentSibCnt = parentSibCount;
	int nextParentCount = 0;
	ImVec2 nodePos = { 0,0 };
	int toChildCount = 0;
	list<Node*>::iterator nIter;

	for (nIter = node->GetChildren().begin(); nIter != node->GetChildren().end(); nIter++)
	{
		toChildCount++;
		nextParentCount++;
	}

	// �ڱ��ڽź��� �־��ش�
	ImGuiNode imguiNode;

	nodePos.x += 100 * curIdx;
	nodePos.y += 100 * parentIdx;

	imguiNode.ID = nodeId;
	imguiNode.Pos = nodePos;
	imguiNode.FromChildCount = 1;
	imguiNode.ToChildCount = toChildCount;
	imguiNode.parentIdx = parentIdx;

	strncpy(imguiNode.Name, String::ToString(node->name).c_str(), 31);
	imguiNode.Name[31] = 0;

	if (node->name == L"Root")
	{
		imguiNode.nType = ImGuiNode::NodeType::Root;
	}
	else if (node->name == L"Selector")
	{
		imguiNode.nType = ImGuiNode::NodeType::Selector;
	}
	else if (node->name == L"Sequence")
	{
		imguiNode.nType = ImGuiNode::NodeType::Sequence;
	}
	else
	{
		imguiNode.nType = ImGuiNode::NodeType::Tesk;
	}

	nodes[nodeId] = imguiNode;

	if (nodes[nodeId].nType == ImGuiNode::NodeType::Tesk)return;

	int currentIdx;
	if (nodeId == 0)
		currentIdx = nodeId + 1;
	else
		currentIdx = nodeId;
	for (nIter = node->GetChildren().begin(); nIter != node->GetChildren().end(); nIter++)
	{
		CompositeNode*childNode = dynamic_cast<CompositeNode*>(*nIter);
		GetImguiTreeNode(childNode, currentIdx + curParentSibCnt, curIdx, nextParentCount);
		currentIdx++;
	}
}

void BehaviorTree::GetImguiLink()
{
	for (int i = 0; i < nodes.size(); i++)
	{
		for (int j = 0; j < nodes[i].ToChildCount; j++)
		{
			NodeLink link;
			if (nodes[i].parentIdx>0)
				link.sibCount = nodes[nodes[i].parentIdx].ToChildCount;
			link.FromChildIdx = i;
			link.FromChildSlot = j;
			link.ToChildSlot = 0;

			if (link.sibCount > 0)
				link.ToChildIdx = i + (j + 1) + link.sibCount - 1;
			else
				link.ToChildIdx = i + (j + 1);

			links.push_back(link);
		}
	}
}




