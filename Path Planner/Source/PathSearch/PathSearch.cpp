#include "PathSearch.h"

namespace fullsail_ai { namespace algorithms {

	PathSearch::PathSearch()
	{
		sNode_Goal = nullptr;
		sNode_Start = nullptr;
		weight_Heuristic = 1.2f;
		goalIsFound = false;
	}

	PathSearch::~PathSearch()
	{
		if (sNode_Goal != nullptr)
		{
			delete sNode_Goal; 
			sNode_Goal = nullptr;
		}

		if (sNode_Start != nullptr)
		{
			delete sNode_Start;
			sNode_Start = nullptr;
		}
	}

	void PathSearch::initialize(TileMap* _tileMap)
	{
		tileMap = _tileMap;
		int rowCount = tileMap->getRowCount();
		int colCount = tileMap->getColumnCount();

		// iterate over tileMap and handle raw data to build nodes masterlist
		for (int row = 0; row < rowCount; row++)
		{
			for (int col = 0; col < colCount; col++)
			{
				Tile* curTile = tileMap->getTile(row, col);
				if (curTile->getWeight() > 0)
				{
					SearchNode* sNode = new SearchNode();
					sNode->tile = curTile;

					nodes.insert(std::make_pair(sNode->tile, sNode));
				}
			}
		}

		// handle neighbors data for each node in masterlist
		SearchNode* sNode;
		Tile* findMe;
		for (int row = 0; row < rowCount; row++)
		{
			for (int col = 0; col < colCount; col++)
			{
				Tile* curTile = tileMap->getTile(row, col);
				if (curTile->getWeight() != 0)
				{
					sNode = nodes[curTile];

					// check for even row or odd row
					if (row % 2 == 0) // even
					{
						// up right (0,-1)
						findMe = tileMap->getTile(row - 1, col);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// up left (-1,-1)
						findMe = tileMap->getTile(row - 1, col - 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// left (-1,0)
						findMe = tileMap->getTile(row, col - 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// right (+1,0)
						findMe = tileMap->getTile(row, col + 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// down left (-1,+1)
						findMe = tileMap->getTile(row + 1, col - 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// down right (0,+1)
						findMe = tileMap->getTile(row + 1, col);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}
					}
					else // odd
					{
						// up right (+1,-1)
						findMe = tileMap->getTile(row - 1, col + 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// up left (0,-1)
						findMe = tileMap->getTile(row - 1, col);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// left (-1,0)
						findMe = tileMap->getTile(row, col - 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// right (+1,0)
						findMe = tileMap->getTile(row, col + 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// down left (0,+1)
						findMe = tileMap->getTile(row + 1, col);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}

						// down right (+1,+1)
						findMe = tileMap->getTile(row + 1, col + 1);
						if (findMe != 0 && findMe->getWeight() > 0)
						{
							SearchNode* sNode_Neighbor = nodes[findMe];
							if (sNode_Neighbor != 0)
							{
								sNode->neighbors.push_back(sNode_Neighbor);
							}
						}
					}
				}
			}
		}
		// debug code to check search graph accuracy
		/*for (auto node : nodes)
		{
			for (auto neighbor : node.second->neighbors)
			{
				node.second->tile->addLineTo(neighbor->tile, 0xff0000ff);
			}
		}*/
	}

	void PathSearch::enter(int startRow, int startColumn, int goalRow, int goalColumn)
	{
		goalIsFound = false;

		// get start and goal nodes
		sNode_Start = nodes[tileMap->getTile(startRow, startColumn)];
		sNode_Goal = nodes[tileMap->getTile(goalRow, goalColumn)];

		// create planner node
		PlannerNode* pNode = new PlannerNode;
		pNode->searchNode = sNode_Start;
		pNode->parent = nullptr;
		pNode->cost_Given = 0;
		pNode->cost_Heuristic = estimateHeuristic(sNode_Start, sNode_Goal);
		pNode->cost_Final = pNode->cost_Given + (pNode->cost_Heuristic * weight_Heuristic);

		// add planner node to relevant containers
		openQueue.push(pNode);
		visited[sNode_Start] = openQueue.front();
	}

	void PathSearch::update(long timeslice)
	{
		// iterate over openQueue
		while (openQueue.empty() == false)
		{
			PlannerNode* pNode_Current = openQueue.front();
			if (pNode_Current->searchNode->tile == sNode_Goal->tile)
			{
				goalIsFound = true;
				break;
			}
			openQueue.pop();
			
			// iterate over neighbors to find successors
			for (int i = 0; i < pNode_Current->searchNode->neighbors.size(); i++)
			{
				// get new successor and compute new given cost
				SearchNode* sNode_Successor = pNode_Current->searchNode->neighbors[i];
				float cost_TempGiven = pNode_Current->cost_Given + (sNode_Successor->tile->getWeight() * tileMap->getTileRadius() * 2);

				// color fill
				sNode_Successor->tile->setFill(0xff0000ff);

				// check for unvisited nodes
				if (visited[sNode_Successor] != NULL)
				{
					if (cost_TempGiven < visited[sNode_Successor]->cost_Given)
					{
						PlannerNode* pNode_Successor = visited[sNode_Successor];
						openQueue.remove(pNode_Successor);
						pNode_Successor->cost_Given = cost_TempGiven;
						pNode_Successor->cost_Final = cost_TempGiven + (pNode_Successor->cost_Heuristic * weight_Heuristic);
						pNode_Successor->parent = pNode_Current;
						openQueue.push(pNode_Successor);
					}
				}
				else
				{
					PlannerNode* pNode_Successor = new PlannerNode();
					pNode_Successor->searchNode = sNode_Successor;
					pNode_Successor->parent = pNode_Current;
					pNode_Successor->cost_Given = cost_TempGiven;
					pNode_Successor->cost_Heuristic = estimateHeuristic(pNode_Current->searchNode, sNode_Goal);
					pNode_Successor->cost_Final = cost_TempGiven + (pNode_Successor->cost_Heuristic * weight_Heuristic);

					visited[sNode_Successor] = pNode_Successor;
					openQueue.push(pNode_Successor);
				}
			}

			if (timeslice == 0)
			{
				break;
			}
		}
	}

	void PathSearch::exit()
	{
		if (visited.empty() == false)
		{
			auto itr = visited.begin();

			while (itr != visited.end())
			{
				delete (itr->second);
				++itr;
			}

			visited.clear();
		}

		visited.clear();
		openQueue.clear();
	}

	void PathSearch::shutdown()
	{
		// handle visited map
		if (visited.empty() == false)
		{
			auto itr = visited.begin();

			while (itr != visited.end())
			{
				delete (itr->second); 
				++itr;
			}

			visited.clear();	
		}

		// handle node map
		if (nodes.empty() == false)
		{
			auto itr = nodes.begin();

			while (itr != nodes.end())
			{
				delete itr->second;
				++itr;
			}

			nodes.clear();
		}

		// clear priority queue
		openQueue.clear();
	}

	bool PathSearch::isDone() const
	{
		return goalIsFound;
	}

	std::vector<Tile const*> const PathSearch::getSolution() const
	{
		std::vector<Tile const*> temp;
		PlannerNode* pNode_Start = openQueue.front();

		// iterate through pNodes
		while (pNode_Start != nullptr)
		{
			temp.push_back(pNode_Start->searchNode->tile);
			pNode_Start = pNode_Start->parent;
		}

		return temp;
	}

	// helper functions:
	bool algorithms::PathSearch::isGreater(PathSearch::PlannerNode* const& lhs, PathSearch::PlannerNode* const& rhs)
	{
		return (lhs->cost_Final > rhs->cost_Final);
	}
	float fullsail_ai::algorithms::PathSearch::estimateHeuristic(SearchNode* sNode_Start, SearchNode* sNode_Goal)
	{
		float deltaX = abs(sNode_Start->tile->getXCoordinate() - sNode_Goal->tile->getXCoordinate());
		float deltaY = abs(sNode_Start->tile->getYCoordinate() - sNode_Goal->tile->getYCoordinate());

		return sqrt(pow(deltaX, 2) + pow(deltaY, 2));
	}

}}  // namespace fullsail_ai::algorithms

