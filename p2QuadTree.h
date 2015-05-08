// ----------------------------------------------------
// Quadtree implementation --
// ----------------------------------------------------

#ifndef __P2QUADTREE_H__
#define __P2QUADTREE_H__

#include "Collider.h"
#include "p2DynArray.h"

#define QUADTREE_MAX_ITEMS 2

// Helper function to check if one rectangle complately contains another
bool Contains(const SDL_Rect& a, const SDL_Rect& b);
bool Intersects(const SDL_Rect& a, const SDL_Rect& b);

// Tree node -------------------------------------------------------
class p2QuadTreeNode
{

public:

	SDL_Rect				rect;
	p2DynArray<Collider*>	objects;
	p2QuadTreeNode*			parent;
	p2QuadTreeNode*			childs[4];

public:

	p2QuadTreeNode(SDL_Rect r) : 
		rect(r),
		objects(QUADTREE_MAX_ITEMS)
	{
		parent = childs[0] = childs[1] = childs[2] = childs[3] = NULL;
	}

	~p2QuadTreeNode()
	{
		for(int i = 0; i < 4; ++i)
			if(childs[i] != NULL) delete childs[i];
	}
	 
	void Insert(Collider* col)
	{
		if (objects.Count() >= QUADTREE_MAX_ITEMS)
		{
			if (childs[0] == NULL)
			{
				SDL_Rect a, b, c, d;

				// child[0]
				a.x = rect.x;
				a.y = rect.y;
				a.w = rect.w / 2;
				a.h = rect.h / 2;
				// child[1]
				b.x = rect.x + (rect.w / 2);
				b.y = rect.y;
				b.w = rect.w / 2;
				b.h = rect.h / 2;
				// child[2]
				c.x = rect.x;
				c.y = rect.y + (rect.h / 2);
				c.w = rect.w / 2;
				c.h = rect.h / 2;
				// child[3]
				d.x = rect.x + b.x;
				d.y = rect.y + c.y;
				d.w = rect.w / 2;
				d.h = rect.h / 2;

				childs[0] = new p2QuadTreeNode(a);
				childs[1] = new p2QuadTreeNode(b);
				childs[2] = new p2QuadTreeNode(c);
				childs[3] = new p2QuadTreeNode(d);
				
				p2DynArray<Collider*> objects_tmp = objects;
				objects.Clear();

				for (unsigned int i = 0; i < QUADTREE_MAX_ITEMS; i++)
				{
					if (Intersects(col->rect, childs[0]->rect) &&
						Intersects(col->rect, childs[1]->rect) &&
						Intersects(col->rect, childs[2]->rect) &&
						Intersects(col->rect, childs[3]->rect))
					{
						objects.PushBack(objects_tmp[i]);
					}
					else
					{
						for (unsigned int j = 0; j < 4; j++)
						{
							if (Intersects(objects_tmp[i]->rect, childs[j]->rect))
							{
								childs[j]->objects.PushBack(objects_tmp[i]);
							}	
						}
					}
				}
			}
			
				if (Intersects(col->rect, childs[0]->rect) &&
					Intersects(col->rect, childs[1]->rect) &&
					Intersects(col->rect, childs[2]->rect) &&
					Intersects(col->rect, childs[3]->rect))
				{
					objects.PushBack(col);
				}
				else
				{
					for (unsigned int i = 0; i < 4; i++)
					{
						if (Intersects(col->rect, childs[i]->rect))
						{
							childs[i]->Insert(col);
						}
					}
				}
			
		}
		else
		{
			objects.PushBack(col);
		}		

		// TODO: Insertar un nou Collider al quadtree
		// En principi cada node pot enmagatzemar QUADTREE_MAX_ITEMS nodes (encara que podrien ser més)
		// Si es detecten més, el node s'ha de tallar en quatre
		// Si es talla, ha de redistribuir tots els seus colliders pels nous nodes (childs) sempre que pugui
		// Nota: un Collider pot estar a més de un node del quadtree
		// Nota: si un Collider intersecciona als quatre childs, deixar-lo al pare
	}

	int CollectCandidates(p2DynArray<Collider*>& nodes, const SDL_Rect& r) const
	{
		// TODO:
		// Omplir el array "nodes" amb tots els colliders candidats
		// de fer intersecció amb el rectangle r
		// retornar el número de intersección calculades en el procés
		// Nota: és una funció recursiva
		int tests = 0;

		if (Intersects(rect, r) == true)
		{
			for (unsigned int i = 0; i < objects.Count(); i++)
			{
				tests++;
				nodes.PushBack(objects[i]);
			}
		}

		if (childs[0] != NULL)
			for (unsigned int i = 0; i < 4; i++)
				tests += childs[i]->CollectCandidates(nodes, r);

		return tests;
	}

	void CollectRects(p2DynArray<p2QuadTreeNode*>& nodes) 
	{
		nodes.PushBack(this);

		for(int i = 0; i < 4; ++i)
			if(childs[i] != NULL) childs[i]->CollectRects(nodes);
	}

};

// Tree class -------------------------------------------------------
class p2QuadTree
{
public:

	// Constructor
	p2QuadTree() : root(NULL)
	{}

	// Destructor
	virtual ~p2QuadTree()
	{
		Clear();
	}

	void SetBoundaries(const SDL_Rect& r)
	{
		if(root != NULL)
			delete root;

		root = new p2QuadTreeNode(r);
	}

	void Insert(Collider* col)
	{
		if(root != NULL)
		{
			if(Intersects(root->rect, col->rect))
				root->Insert(col);
		}
	}

	void Clear()
	{
		if(root != NULL)
		{
			delete root;
			root = NULL;
		}
	}

	int CollectCandidates(p2DynArray<Collider*>& nodes, const SDL_Rect& r) const
	{
		int tests = 1;
		if(root != NULL && Intersects(root->rect, r))
			tests = root->CollectCandidates(nodes, r);
		return tests;
	}

	void CollectRects(p2DynArray<p2QuadTreeNode*>& nodes) const
	{
		if(root != NULL)
			root->CollectRects(nodes);
	}

public:

	p2QuadTreeNode*	root;

};

#endif // __p2QuadTree_H__