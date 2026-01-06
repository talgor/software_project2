import sys, math, pandas as pd, numpy as np

global DIM, K, ITER, points, EPSILON
DIM = None
K = None
ITER = 400
points = []
EPSILON = 0.001


def parse_files(file_name_1, file_name_2):
    df1 = pd.read_csv(file_name_1, header = None, index_col = 0)
    df2 = pd.read_csv(file_name_2, header = None, index_col = 0)
    joined = pd.concat([df1, df2], axis = 1, join = "inner")
    joined = joined.sort_values(by=0)
    return joined.to_numpy(), joined.index.values


def main():
    matrix, indices = parse_files(sys.argv[1], sys.argv[2])
    print(matrix)
    print(indices)

