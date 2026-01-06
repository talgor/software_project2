import sys, math, pandas as pd, numpy as np

global DIM, K, ITER, points, EPSILON
DIM = None
K = None
ITER = 400
points = []
EPSILON = 0.001

def kmeans_pp_init(data_points, data_indices, k):
    np.random.seed(1234)

    keys = np.array(data_indices)

    N, D = data_points.shape
    chosen_indices = []

    first_index = np.random.choice(N)
    chosen_indices.append(first_index)

    for _ in range(k-1):
        current_centers = data_points[chosen_indices]

        dists = np_euclidean_distance(data_points[:, np.newaxis, :], current_centers[np.newaxis, :, :])
        min_dists = np.min(dists, axis=1)
        sum_dists = np.sum(min_dists)
        if sum_dists == 0:
            probs = np.ones(N) / N
        else:
            probs = min_dists / sum_dists
        next_index = np.random.choice(N, p=probs)
        chosen_indices.append(next_index)

    return data_points[chosen_indices], keys[chosen_indices]
         

def np_euclidean_distance(a, b):
            return np.sqrt(np.sum((a - b) ** 2, axis=2))
    


def kmeans(points, k, max_iterations=ITER):
    centerPoints = [CenterPoint(point.coords) for point in points[:k]]
    for iteration in range(max_iterations):
        updateAllPoints(points, centerPoints)
        if updateAllCenters(centerPoints):
            break
    return centerPoints

def parse(inp):
    global DIM, points
    for line in inp:
        coaordinates = line.strip().split(",")
        coaordinates = [float(co) for co in coaordinates]
        if DIM == None:
            DIM = len(coaordinates)
        if len(coaordinates) != DIM:
            if not line and len(inp) == len(points):
                return
            else:
                raise Exception()
        points.append(Point(coaordinates))
    return

def main():
    if(len(sys.argv) < 2 or len(sys.argv) > 3):
        print("An Error Has Occurred")
        sys.exit(1)
    try:
        K = int(sys.argv[1])
    except Exception:
        print("Incorrect number of clusters!")
        sys.exit(1)
    if K < 2:
        print("Incorrect number of clusters!")
        sys.exit(1)
    if len(sys.argv) > 2:
        try:
            ITER = int(sys.argv[2])
            if (ITER < 2) or (ITER > 799):
                print("Incorrect maximum iteration!")
                sys.exit(1)
        except Exception:
            print("Incorrect maximum iteration!")
            sys.exit(1)

    #parse and dreate points
    
    if(K >= len(points)):
        print("Incorrect number of clusters!")
        sys.exit(1)

    #kmeans logic in c
        
        for center in final_centers:
            formatted_coords = (f"{c:.4f}" for c in center.coords)
            print(*formatted_coords, sep=",")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        print("An Error Has Occured")
        sys.exit(1)



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

