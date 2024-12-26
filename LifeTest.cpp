#include <gtest/gtest.h>
#include "Life.h"


TEST(FieldClassTest, GetEmpty) {
	Field f(5,5);
	f.getAt(0,0);
}
