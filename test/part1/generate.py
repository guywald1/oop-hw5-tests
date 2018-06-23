#!/usr/bin/python3

import random

# ***** configuration ***** #

# number of tests to run
NUM_TESTS = 10
# range of values in matrix
NUM_RANGE = [-10, 10]
# range of rows/column
ROW_COL_RANGE = [1, 10]
# name of template that gets a value from a matrix (template<typename R, typename C, typename M>)
MATRIX_GET_NAME = "MatrixGet"

# ***** helpers ***** #

def matmul(a, b):
    zip_b = zip(*b)
    zip_b = list(zip_b)
    return [[sum(ele_a*ele_b for ele_a, ele_b in zip(row_a, col_b)) 
             for col_b in zip_b] for row_a in a]

def generateMatrix(rows, cols):
    return [[random.randint(*NUM_RANGE) for _ in range(0, cols)] for _ in range(0, rows)]

def matUuidFromCount(count):
    return "mat_" + str(count)

def defineMatrix(mat, state, comment = None):
    uuid = "mat_" + str(state["count"])
    print("// matrix {}".format(uuid) if comment is None else comment)
    print("/*{}*/".format(mat))
    rows = []
    for r in range(0, len(mat)):
        row = []
        for c in range(0, len(mat[0])):
            row.append("Int<{}>".format(mat[r][c]))
        rows.append("List<{}>".format(",".join(row)))
    state["count"] += 1
    print("typedef List<\n{}\n> {};\n".format(",\n".join(map(lambda r: "\t" + r, rows)), uuid), end='\n')

def assertMultiply(mult, state):
    expectedUuid = "mat_{}".format(state["count"])
    actualUuid = "mat_{}".format(state["count"]-1)
    print("typedef typename Multiply<{}, {}>::result {};".format(matUuidFromCount(state["count"]-3), matUuidFromCount(state["count"]-2), expectedUuid))
    state["count"] += 1
    for r in range(0, len(mult)):
        for c in range(0, len(mult[0])):
            print("static_assert({0}<{1}, {2}, {3}>::value::value == {0}<{1}, {2}, {4}>::value::value, \"Failed: index {1},{2} should be equal to {5}.\");".format(MATRIX_GET_NAME, r, c, actualUuid, expectedUuid, mult[r][c]))

# ***** main ***** #

def main():
    state = { "count": 0}
    for i in range(0, NUM_TESTS):
        print("// {0} TEST {1} {0} //".format("*" * 10, i))
        rows1 = random.randint(*ROW_COL_RANGE)
        cols1 = random.randint(*ROW_COL_RANGE)
        rows2 = cols1
        cols2 = random.randint(*ROW_COL_RANGE)
        mat1 = generateMatrix(rows1, cols1)
        mat2 = generateMatrix(rows2, cols2)
        defineMatrix(mat1, state)
        defineMatrix(mat2, state)
        mult = matmul(mat1, mat2)
        mult_comment = "// matrix {}  :  {} x {}".format(*map(lambda n: matUuidFromCount(state["count"]-n), range(0,3)))
        defineMatrix(mult, state, mult_comment)
        assertMultiply(mult, state)

main()
