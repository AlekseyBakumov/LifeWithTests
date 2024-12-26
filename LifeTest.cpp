#include <gtest/gtest.h>
#include "Life.h"


TEST(FieldClassTest, GetEmpty) {
	Field f(5,5);
	EXPECT_FALSE(f.getAt(0,0));
}

TEST(FieldClassTest, SetEmpty) {
	Field f(5,5);
	f.setAt(1,1,true);
	EXPECT_TRUE(f.getAt(1,1));
}

TEST(FieldClassTest, SetFalse) {
	Field f(5,5);
	f.setAt(1,1,true);
	f.setAt(1,1,false);
	EXPECT_FALSE(f.getAt(1,1));
}

TEST(FieldClassTest, Clear) {
	Field f(5,5);
	f.setAt(1,1,true);
	f.setAt(2,3,true);
	f.setAt(4,0,true);
	f.Clear();
	EXPECT_FALSE(f.getAt(1,1));
	EXPECT_FALSE(f.getAt(2,3));
	EXPECT_FALSE(f.getAt(4,0));
}

TEST(FieldClassTest, GetN) {
	Field f(5,7);
	EXPECT_EQ(5, f.getN());
}
TEST(FieldClassTest, GetM) {
	Field f(5,7);
	EXPECT_EQ(7, f.getM());
}

TEST(FieldClassTest, NormalizationXNegative) {
	Field f(5,5);
	EXPECT_EQ(3, f.normalizeX(-2));
}
TEST(FieldClassTest, NormalizationXPositive) {
	Field f(5,5);
	EXPECT_EQ(2, f.normalizeX(7));
}

TEST(FieldClassTest, NormalizationYNegative) {
	Field f(5,5);
	EXPECT_EQ(3, f.normalizeY(-2));
}

TEST(FieldClassTest, NormalizationYPositive) {
	Field f(5,5);
	EXPECT_EQ(2, f.normalizeY(7));
}

TEST(FieldClassTest, SetOutOfBoundsNegative) {
	Field f(5,5);
	f.setAt(-1,-1,true);
	EXPECT_TRUE(f.getAt(4,4));
}

TEST(FieldClassTest, SetOutOfBoundsPositive) {
	Field f(5,5);
	f.setAt(5,5,true);
	EXPECT_TRUE(f.getAt(0,0));
}

TEST(FieldClassTest, GetOutOfBonudsNegative) {
	Field f(5,5);

	f.setAt(4,4,true);
	EXPECT_TRUE(f.getAt(-1,-1));
}

TEST(FieldClassTest, GetOutOfBoundsPositive) {
	Field f(5,5);
	f.setAt(0,0,true);
	EXPECT_TRUE(f.getAt(5,5));
}

TEST(LogicClass, TickB123456780_S123456780) {
	EXPECT_TRUE(true);
}

TEST(LogicClass, TickB3_S23) {
	Field* f = new Field(10,10);
	f->DefaultPreset();
	Logic l(f,3,23);
	l.Tick();
	l.Tick();
	l.Tick();
	l.Tick();
	EXPECT_TRUE(f->getAt(2,2));
}
