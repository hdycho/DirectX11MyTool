#define ThreadX 32
#define ThreadY 32

const int NODE_NULL = 0;
const int NODE_OPEN = 1;
const int NODE_CLOSE = 2;

const int WALKABLE = 0;
const int COLLISION = 1;

int GridBlockCountX;
int MapDimension;

struct Agent // 갈길
{
    uint Id;
    uint2 Start;
    uint2 target;
};

StructuredBuffer<Agent> AgentList;

struct GridNode
{
    uint X;
    uint Y;
    uint ParentID;
    uint Status;
    uint Cost;
    uint GoalCost;
    uint Id;
};

RWStructuredBuffer<GridNode> GridNodeList;
RWStructuredBuffer<uint> OpenList;

struct Search
{
    uint AgentId;
    uint FinalCost;
    uint2 Target;
};

void Insert(uint offset,uint x,uint y,uint currentCost,uint goalCost,uint parentId,uint status)
{
    uint currentSize = OpenList[offset];
    uint i = currentCost + 1;
    
    uint parentCost = GridNodeList[offset + OpenList[offset + (i / 2)]].Cost;

    [allow_uav_condition]
    while(i>1&&parentCost>currentCost)
    {
        OpenList[offset + i] = OpenList[offset + (i / 2)];
        GridNodeList[offset + OpenList[offset + (i / 2)]].Id=i;

        i = i / 2;
        parentCost = GridNodeList[offset + OpenList[offset + i]].Cost;
    }

    GridNode node;
    node.X = x;
    node.Y = y;
    node.Cost = currentCost;
    node.ParentID = offset + parentId;
    node.Status = status;
    node.GoalCost = goalCost;
    node.Id = i;

    uint gridId = MapDimension * y + x;
    GridNodeList[offset + gridId] = node;

    OpenList[offset + i] = gridId;
    OpenList[offset + 0] = OpenList[offset] + 1;
}

void Update(uint offset, uint id, uint currentCost)
{
    uint i = id;

    uint originValue = OpenList[offset + i];
    uint parentCost = GridNodeList[offset + OpenList[offset + (i / 2)]].Cost;

    [allow_uav_condition]
    while (i > 1 && parentCost > currentCost)
    {
        OpenList[offset + i] = OpenList[offset + (i / 2)];
        GridNodeList[offset + OpenList[offset + i]].Id = i;

        i = i / 2;
        parentCost = GridNodeList[offset + OpenList[offset + i]].Cost;
    }

    OpenList[offset + i] = originValue;
    GridNodeList[offset + OpenList[offset + i]].Id = i;
}

void AddToOpenList(uint offset,uint2 node,GridNode parentGridNode,uint parentGridId,uint2 targetNode,uint goalCost)
{
    uint gridNodeId = MapDimension * node.y + node.x;
    GridNode gridNode = GridNodeList[offset + gridNodeId];

    uint currentGoalCost = parentGridNode.Cost + goalCost;

    int x1 = node.x;
    int x2 = targetNode.x;

    int x = abs((int)node.x - (int)targetNode.x);
    int y = abs((int) node.y - (int)targetNode.y);

    int temp = max(x, y);
    uint F = currentGoalCost * uint(temp) * 10;

    if(gridNode.Status==NODE_NULL)
    {
        Insert(offset, node.x, node.y, F, currentGoalCost, parentGridId, NODE_OPEN);
    }
    else if(gridNode.Status==NODE_OPEN)
    {
        currentGoalCost = parentGridNode.GoalCost + goalCost;

        [flatten]
        if(currentGoalCost<gridNode.GoalCost)
        {
            GridNodeList[offset + gridNodeId].Cost = F;

            Update(offset, gridNode.Id, F);
        }
    }
}

uint Remove(uint offset)
{
    uint current = OpenList[offset];
    uint node = 0;

    if(current>=1)
    {
        node = OpenList[offset + 1];
        OpenList[offset + 1] = OpenList[offset + current];

        GridNodeList[offset + OpenList[offset + 1]].Id = 1;
        OpenList[offset + current] = 0;

        current = current - 1;
        OpenList[offset] = current;

        uint i = 1;
        if(current>=1)
        {
            [allow_uav_condition]
            while(true)
            {
                uint rightChild = i * 2 + 1;
                uint leftChild = i * 2;

                uint replaceId = 1;
                uint replaceCost = 0;

                if(rightChild>=current)
                {
                    if (leftChild>=current)
                        break;
                    else
                        replaceId = leftChild;

                    replaceCost = GridNodeList[offset + OpenList[offset + replaceId]].Cost;
                }
                else
                {
                    uint leftChildCost = GridNodeList[offset + OpenList[offset + leftChild]].Cost;
                    uint rightChildCost = GridNodeList[offset + OpenList[offset + rightChild]].Cost;
              
                    if(leftChildCost<rightChildCost)
                    {
                        replaceId = leftChild;
                        replaceCost = leftChildCost;
                    }
                    else
                    {
                        replaceId = rightChild;
                        replaceCost = rightChildCost;
                    }
                }

                uint currentNodeCost = GridNodeList[offset + OpenList[offset + i]].Cost;
                if(currentNodeCost>replaceCost)
                {
                    uint temp = OpenList[offset + replaceId];
                    OpenList[offset + replaceId] = OpenList[offset + i];
                    OpenList[offset + i] = temp;

                    GridNodeList[offset + OpenList[offset + replaceId]].Id = replaceId;
                    GridNodeList[offset + OpenList[i]].Id = i;
                }
                else
                    break;

            }//while(true)
        }//if(current)
    }

    return node;
}

[numthread(ThreadX,ThreadY,1)]
void CS(uint3 groupThreadId:SV_GroupThreadID,uint3 dispatchThreadId:SV_DispatchThreadId,uint3 groupId:SV_GroupId)
{
    uint gridId = ((ThreadX * GridBlockCountX) * dispatchThreadId.y) + dispatchThreadId.x;

    uint threadId = gridId;
    Agent agent = AgentList[threadId];

    uint2 start = agent.Start;
    uint2 target = agent.target;

    uint targetGridId = MapDimension * target.y + target.x;
    
    uint offset = gridId * (MapDimension * MapDimension);
    Insert(offset, start.x, start.y, 10, 0, 0, NODE_OPEN);
    
    bool bFound = false;
    GridNode currentNode;

    [allow_uav_condition]
    while (bFound == false)
    {
        uint tempGridId = Remove(offset);

        GridNodeList[offset + tempGridId].Status = NODE_CLOSE;
        currentNode = GridNodeList[offset + tempGridId];

        if(currentNode.Cost==0&&tempGridId==targetGridId)
        {
            bFound = true;
            break;
        }
        else
        {
            uint tempX = float(currentNode.X);
            uint tempY = float(currentNode.Y);

            uint2 node = 0;
            if(MapDimension>tempX+1) // 오른쪽
            {
                node = uint2(currentNode.X + 1, currentNode.Y);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }

            if (MapDimension > tempX + 1&&tempY-1>=0) // 오른쪽하단 대각선
            {
                node = uint2(currentNode.X + 1, currentNode.Y-1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            if (tempY-1>=0) // 하단
            {
                node = uint2(currentNode.X, currentNode.Y - 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            if (tempX-1>=0&&tempY-1>=0) //왼쪽하단
            {
                node = uint2(currentNode.X-1, currentNode.Y - 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            if (tempX - 1 >= 0) // 왼쪽
            {
                node = uint2(currentNode.X-1, currentNode.Y );
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }

            if (tempX - 1 >= 0 && MapDimension > (tempY + 1)) // 왼쪽상단
            {
                node = uint2(currentNode.X - 1, currentNode.Y+1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }

            if (MapDimension > (tempY + 1)) // 상단
            {
                node = uint2(currentNode.X , currentNode.Y + 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 10);
            }

            if (MapDimension > (tempX + 1)&&MapDimension > (tempY + 1)) //우측 상단
            {
                node = uint2(currentNode.X+1, currentNode.Y + 1);
                AddToOpenList(offset, node, currentNode, tempGridId, target, 14);
            }
        }
    }

    Search search;
    search.AgentId = agent.Id;
    search.FinalCost = currentNode.Cost;
    search.Target = float2(currentNode.X, currentNode.Y);
    searchlist[threadId] = search;

}
