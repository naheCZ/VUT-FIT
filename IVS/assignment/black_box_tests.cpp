//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     Red-Black Tree - public interface tests
//
// $NoKeywords: $ivs_project_1 $black_box_tests.cpp
// $Author:     Roman Nahálka <xnahal01@stud.fit.vutbr.cz>
// $Date:       $2018-02-25
//============================================================================//
/**
 * @file black_box_tests.cpp
 * @author Roman Nahálka
 * 
 * @brief Implementace testu binarniho stromu.
 */

#include <vector>

#include "gtest/gtest.h"

#include "red_black_tree.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy Red-Black Tree, testujte nasledujici:
// 1. Verejne rozhrani stromu
//    - InsertNode/DeleteNode a FindNode
//    - Chovani techto metod testuje pro prazdny i neprazdny strom.
// 2. Axiomy (tedy vzdy platne vlastnosti) Red-Black Tree:
//    - Vsechny listove uzly stromu jsou *VZDY* cerne.
//    - Kazdy cerveny uzel muze mit *POUZE* cerne potomky.
//    - Vsechny cesty od kazdeho listoveho uzlu ke koreni stromu obsahuji
//      *STEJNY* pocet cernych uzlu.
//============================================================================//

class EmptyTree : public ::testing::Test //Trida pro testovani prazdneho stromu
{
protected:
	BinaryTree tree;
};

class NonEmptyTree : public ::testing::Test //Trida pro testovani neprazdneho stromu
{
protected:
	BinaryTree tree;

	virtual void SetUp()
	{
		int keys[] = { 10, 15, 33, 66, 75, 5, 4, 69, 77, 78, 12, 23, 24, 99 };

		for (int i = 0; i < 14; i++)
			tree.InsertNode(keys[i]);
	}
};

class TreeAxioms : public ::testing::Test //Trida pro testovani Axiomu
{
protected:
	BinaryTree tree;

	virtual void SetUp()
	{
		int keys[] = { 10, 15, 33, 66, 75, 5, 4, 69, 77, 78, 12, 23, 24, 99 };

		for (int i = 0; i < 14; i++)
			tree.InsertNode(keys[i]);
	}
};

TEST_F(EmptyTree, InsertNode)
{
	EXPECT_TRUE(tree.GetRoot() == NULL);

	tree.InsertNode(10);
	EXPECT_TRUE(tree.GetRoot() != NULL);
	EXPECT_EQ(tree.GetRoot()->key, 10);
}

TEST_F(EmptyTree, DeleteNode)
{
	EXPECT_FALSE(tree.DeleteNode(5));
}

TEST_F(EmptyTree, FindNode)
{
	EXPECT_TRUE(tree.FindNode(5) == NULL);
}

TEST_F(NonEmptyTree, InsertNode_Exists)
{
	std::pair<bool, BinaryTree::Node_t*> x = tree.InsertNode(10);

	EXPECT_FALSE(x.first);
}

TEST_F(NonEmptyTree, InsertNode_NonExists)
{
	std::pair<bool, BinaryTree::Node_t*> x = tree.InsertNode(11);

	EXPECT_TRUE(x.first);
}

TEST_F(NonEmptyTree, DeleteNode_Exists)
{
	EXPECT_TRUE(tree.DeleteNode(10));
}

TEST_F(NonEmptyTree, DeleteNode_NonExists)
{
	EXPECT_FALSE(tree.DeleteNode(11));
}

TEST_F(NonEmptyTree, FindNode_Exists)
{
	EXPECT_TRUE(tree.FindNode(10) != NULL);
}

TEST_F(NonEmptyTree, FindNode_NonExists)
{
	EXPECT_TRUE(tree.FindNode(11) == NULL);
}

TEST_F(TreeAxioms, Axiom1)
{
	std::vector<BinaryTree::Node_t *> leafs;

	tree.GetLeafNodes(leafs);

	for (int i = 0; i < leafs.size(); i++)
		EXPECT_EQ(leafs[i]->color, BinaryTree::Color_t::BLACK);
}

TEST_F(TreeAxioms, Axiom2)
{
	std::vector<BinaryTree::Node_t *> leafs;

	tree.GetAllNodes(leafs);

	for (int i = 0; i < leafs.size(); i++)
	{
		if (leafs[i]->color == BinaryTree::Color_t::RED)
		{
			EXPECT_EQ(leafs[i]->pLeft->color, BinaryTree::Color_t::BLACK);
			EXPECT_EQ(leafs[i]->pRight->color, BinaryTree::Color_t::BLACK);
		}
	}
}

TEST_F(TreeAxioms, Axiom3)
{
	std::vector<BinaryTree::Node_t *> leafs;
	BinaryTree::Node_t * root;
	int path = 0;

	tree.GetLeafNodes(leafs);
	root = tree.GetRoot();

	for (int i = 0; i < leafs.size(); i++)
	{
		BinaryTree::Node_t * actualLeaf = leafs[i];
		int currentPath = 0;

		while (actualLeaf != root)
		{
			actualLeaf = actualLeaf->pParent;

			if (actualLeaf->color == BinaryTree::Color_t::BLACK)
			{
				currentPath++;

				if (i == 0)
					path++;
			}
		}

		EXPECT_EQ(currentPath, path);
	}
}

/*** Konec souboru black_box_tests.cpp ***/
