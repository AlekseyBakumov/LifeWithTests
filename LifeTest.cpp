#include <gtest/gtest.h>
#include "Life.h"

Field* f = new Field(5,5);
Field* f2 = new Field(5,7);

TEST(FieldClassTest, GetEmpty) {
	EXPECT_FALSE(f->getAt(0,0));
}

TEST(FieldClassTest, SetEmpty) {
	f->setAt(1,1,true);
	EXPECT_TRUE(f->getAt(1,1));
}

TEST(FieldClassTest, SetFalse) {
	f->setAt(1,1,true);
	f->setAt(1,1,false);
	EXPECT_FALSE(f->getAt(1,1));
}

TEST(FieldClassTest, Clear) {
	f->setAt(1,1,true);
	f->setAt(2,3,true);
	f->setAt(4,0,true);
	f->Clear();
	EXPECT_FALSE(f->getAt(1,1));
	EXPECT_FALSE(f->getAt(2,3));
	EXPECT_FALSE(f->getAt(4,0));
}

TEST(FieldClassTest, GetN) {
	EXPECT_EQ(5, f2->getN());
}
TEST(FieldClassTest, GetM) {
	EXPECT_EQ(7, f2->getM());
}

TEST(FieldClassTest, NormalizationXNegative) {
	EXPECT_EQ(3, f->normalizeX(-2));
}
TEST(FieldClassTest, NormalizationXPositive) {
	EXPECT_EQ(2, f->normalizeX(7));
}

TEST(FieldClassTest, NormalizationYNegative) {
	EXPECT_EQ(3, f->normalizeY(-2));
}

TEST(FieldClassTest, NormalizationYPositive) {
	EXPECT_EQ(2, f->normalizeY(7));
}

TEST(FieldClassTest, SetOutOfBoundsNegative) {
	f->setAt(-1,-1,true);
	EXPECT_TRUE(f->getAt(4,4));
}

TEST(FieldClassTest, SetOutOfBoundsPositive) {
	f->setAt(5,5,true);
	EXPECT_TRUE(f->getAt(0,0));
}

TEST(FieldClassTest, GetOutOfBonudsNegative) {

	f->setAt(4,4,true);
	EXPECT_TRUE(f->getAt(-1,-1));
}

TEST(FieldClassTest, GetOutOfBoundsPositive) {
	f->setAt(0,0,true);
	EXPECT_TRUE(f->getAt(5,5));
}

TEST(LogicClass, TickB123456780_S123456780) {

}

TEST(LogicClass, TickB3_S23) {
	Field* f4 = new Field(10,10);
	f4->DefaultPreset();
	int s = 23;
	int s2 = s;
	Logic l(f4,3,s2);
std::cout << s << std::endl;
	l.Tick();
	l.Tick();
	l.Tick();
	l.Tick();
	EXPECT_TRUE(f4->getAt(2,2));
}
