//======== Copyright (c) 2017, FIT VUT Brno, All rights reserved. ============//
//
// Purpose:     White Box - Tests suite
//
// $NoKeywords: $ivs_project_1 $white_box_code.cpp
// $Author:     Roman Nahálka <xnahal01@stud.fit.vutbr.cz>
// $Date:       $2018-02-25
//============================================================================//
/**
 * @file white_box_tests.cpp
 * @author Roman Nahálka
 * 
 * @brief Implementace testu prace s maticemi.
 */

#include "gtest/gtest.h"
#include "white_box_code.h"

//============================================================================//
// ** ZDE DOPLNTE TESTY **
//
// Zde doplnte testy operaci nad maticemi. Cilem testovani je:
// 1. Dosahnout maximalniho pokryti kodu (white_box_code.cpp) testy.
// 2. Overit spravne chovani operaci nad maticemi v zavislosti na rozmerech 
//    matic.
//============================================================================//

class MatrixConstruction : public ::testing::Test
{
protected:
	Matrix matrix;
};

class Matrix1x1 : public ::testing::Test
{
protected:
	Matrix matrix;

	virtual void SetUp()
	{
		matrix = Matrix();
	}
};

class Operations : public ::testing::Test
{
protected:
	Matrix oneOne;
	Matrix oneOne2;
	Matrix twoOne;
	Matrix oneTwo;
	Matrix twoTwo;

	virtual void SetUp()
	{
		oneOne = Matrix();
		oneOne2 = Matrix();
		twoOne = Matrix(2, 1);
		oneTwo = Matrix(1, 2);
		twoTwo = Matrix(2, 2);
	}
};

class Equations : public ::testing::Test
{
protected:
	Matrix threeThree;
	Matrix fourFour;
	Matrix singular;
	Matrix oneTwo;
	Matrix oneOne;

	virtual void SetUp()
	{
		threeThree = setThree();
		fourFour = setFour();
		singular = setSingular();
		Matrix oneOne = Matrix();
	}

	Matrix setThree()
	{
		std::vector<std::vector<double>> values(3, std::vector<double>(3));
		Matrix matrix = Matrix(3, 3);

		values[0][0] = 1;
		values[0][1] = 2;
		values[0][2] = 3;

		values[1][0] = 2;
		values[1][1] = -1;
		values[1][2] = 1;

		values[2][0] = 3;
		values[2][1] = 0;
		values[2][2] = -1;

		matrix.set(values);

		return matrix;
	}

	Matrix setFour()
	{
		std::vector<std::vector<double>> values(4, std::vector<double>(4));
		Matrix matrix = Matrix(4, 4);

		values[0][0] = 3;
		values[0][1] = 2;
		values[0][2] = 0;
		values[0][3] = 1;

		values[1][0] = 2;
		values[1][1] = 0;
		values[1][2] = 3;
		values[1][3] = 2;

		values[2][0] = 1;
		values[2][1] = 3;
		values[2][2] = 5;
		values[2][3] = 2;

		values[3][0] = 2;
		values[3][1] = 3;
		values[3][2] = 0;
		values[3][3] = 5;

		matrix.set(values);

		return matrix;
	}

	Matrix setSingular()
	{
		std::vector<std::vector<double>> values(2, std::vector<double>(2));
		Matrix matrix = Matrix(2, 2);

		values[0][0] = 1;
		values[0][1] = -2;

		values[1][0] = -2;
		values[1][1] = 4;

		matrix.set(values);

		return matrix;
	}
};

TEST(MatrixConstruction, Constructor)
{
	EXPECT_NO_THROW(Matrix());
	EXPECT_NO_THROW(Matrix(3, 3));
	EXPECT_NO_THROW(Matrix(2, 5));
	EXPECT_NO_THROW(Matrix(5, 2));
	
	EXPECT_ANY_THROW(Matrix(0, 0));
	EXPECT_ANY_THROW(Matrix(0, 1));
	EXPECT_ANY_THROW(Matrix(1, 0));
}

TEST_F(Matrix1x1, Set)
{
	EXPECT_TRUE(matrix.set(0, 0, 10.0));
	
	EXPECT_FALSE(matrix.set(0, 1, 10.5));
	EXPECT_FALSE(matrix.set(1, 0, 11.0));
	EXPECT_FALSE(matrix.set(1, 1, 11.5));
}

TEST_F(Matrix1x1, SetField)
{
	std::vector<std::vector<double>> values(1, std::vector<double>(1, 0));

	values[0][0] = 555.0;

	EXPECT_TRUE(matrix.set(values));

	values.resize(2, std::vector<double>(2));

	values[0][0] = 55.0;
	values[0][1] = 12.0;
	values[1][0] = 13.5;
	values[1][1] = 14.0;

	EXPECT_FALSE(matrix.set(values));
}

TEST_F(Matrix1x1, Get)
{
	EXPECT_EQ(matrix.get(0, 0), 0);

	EXPECT_TRUE(std::isnan(matrix.get(0, 1)));
	EXPECT_TRUE(std::isnan(matrix.get(1, 0)));
	EXPECT_TRUE(std::isnan(matrix.get(1, 1)));
}

TEST_F(Matrix1x1, SetAndGet)
{
	EXPECT_TRUE(matrix.set(0, 0, 10.5));

	EXPECT_EQ(matrix.get(0, 0), 10.5);
}

TEST_F(Operations, Equal)
{
	EXPECT_TRUE(oneOne.operator==(oneOne2));
	
	ASSERT_TRUE(oneOne2.set(0, 0, 5.0));
	EXPECT_FALSE(oneOne.operator==(oneOne2));

	EXPECT_ANY_THROW(oneOne.operator==(oneTwo));
	EXPECT_ANY_THROW(oneOne.operator==(twoOne));
	EXPECT_ANY_THROW(oneOne.operator==(twoTwo));
}

TEST_F(Operations, Add)
{
	Matrix result;

	ASSERT_TRUE(oneOne.set(0, 0, 5.0));
	ASSERT_TRUE(oneOne2.set(0, 0, 2.5));

	EXPECT_NO_THROW(result = oneOne.operator+(oneOne2));
	EXPECT_EQ(result.get(0, 0), 7.5);

	EXPECT_ANY_THROW(oneOne.operator+(oneTwo));
	EXPECT_ANY_THROW(oneOne.operator+(twoOne));
	EXPECT_ANY_THROW(oneOne.operator+(twoTwo));
}

TEST_F(Operations, MatrixMultiplication)
{
	Matrix result;

	ASSERT_TRUE(oneOne.set(0, 0, 10.0));
	ASSERT_TRUE(oneOne2.set(0, 0, 1.5));

	EXPECT_NO_THROW(result = oneOne.operator*(oneOne2));
	EXPECT_EQ(result.get(0, 0), 15.0);

	ASSERT_TRUE(oneTwo.set(0, 0, 1.0));
	ASSERT_TRUE(oneTwo.set(0, 1, 2.0));

	EXPECT_NO_THROW(result = oneOne.operator*(oneTwo));
	EXPECT_EQ(result.get(0, 0), 10.0);
	EXPECT_EQ(result.get(0, 1), 20.0);

	ASSERT_TRUE(twoOne.set(0, 0, 3.4));
	ASSERT_TRUE(twoOne.set(1, 0, 7.5));

	EXPECT_NO_THROW(result = twoOne.operator*(oneTwo));
	EXPECT_EQ(result.get(0, 0), 3.4);
	EXPECT_EQ(result.get(0, 1), 6.8);
	EXPECT_EQ(result.get(1, 0), 7.5);
	EXPECT_EQ(result.get(1, 1), 15.0);

	EXPECT_ANY_THROW(oneOne.operator*(twoOne));
	EXPECT_ANY_THROW(oneOne.operator*(twoTwo));
}

TEST_F(Operations, NumberMultiplication)
{
	Matrix result;

	ASSERT_TRUE(oneOne.set(0, 0, 15.5));
	
	ASSERT_TRUE(oneTwo.set(0, 0, 13.3));
	ASSERT_TRUE(oneTwo.set(0, 1, 12.7));
	
	ASSERT_TRUE(twoOne.set(0, 0, 7.5));
	ASSERT_TRUE(twoOne.set(1, 0, 11.1));

	ASSERT_TRUE(twoTwo.set(0, 0, 20.0));
	ASSERT_TRUE(twoTwo.set(0, 1, 3.3));
	ASSERT_TRUE(twoTwo.set(1, 0, 8.7));
	ASSERT_TRUE(twoTwo.set(1, 1, 5.5));

	result = oneOne.operator*(2);
	EXPECT_DOUBLE_EQ(result.get(0, 0), 31.0);

	result = oneTwo.operator*(3.5);
	EXPECT_DOUBLE_EQ(result.get(0, 0), 46.55);
	EXPECT_DOUBLE_EQ(result.get(0, 1), 44.45);

	result = twoOne.operator*(1.5);
	EXPECT_DOUBLE_EQ(result.get(0, 0), 11.25);
	EXPECT_DOUBLE_EQ(result.get(1, 0), 16.65);

	result = twoTwo.operator*(3.0);
	EXPECT_DOUBLE_EQ(result.get(0, 0), 60.0);
	EXPECT_DOUBLE_EQ(result.get(0, 1), 9.9);
	EXPECT_DOUBLE_EQ(result.get(1, 0), 26.1);
	EXPECT_DOUBLE_EQ(result.get(1, 1), 16.5);
}

TEST_F(Equations, EquationsSolve)
{
	std::vector<double> result = std::vector<double>(3);
	std::vector<double> rightSide = std::vector<double>(3);

	rightSide[0] = 9;
	rightSide[1] = 8;
	rightSide[2] = 3;

	EXPECT_NO_THROW(result = threeThree.solveEquation(rightSide));
	EXPECT_EQ(result[0], 2);
	EXPECT_EQ(result[1], -1);
	EXPECT_EQ(result[2], 3);

	rightSide.resize(4);
	result.resize(4);
	EXPECT_ANY_THROW(threeThree.solveEquation(rightSide));

	rightSide[0] = 16;
	rightSide[1] = 12;
	rightSide[2] = 19;
	rightSide[3] = 33;

	EXPECT_NO_THROW(result = fourFour.solveEquation(rightSide));
	EXPECT_EQ(result[0], 2);
	EXPECT_EQ(result[1], 3);
	EXPECT_EQ(result[2], 0);
	EXPECT_EQ(result[3], 4);

	rightSide.resize(2);
	rightSide[0] = 5.0;
	rightSide[1] = 1.0;
	oneTwo = Matrix(1, 2);
	EXPECT_ANY_THROW(oneTwo.solveEquation(rightSide));
	EXPECT_ANY_THROW(singular.solveEquation(rightSide));

	rightSide.resize(1);
	rightSide[0] = 10.0;
	oneOne.set(0, 0, 10.0);
	EXPECT_NO_THROW(oneOne.solveEquation(rightSide));
}

/*** Konec souboru white_box_tests.cpp ***/
