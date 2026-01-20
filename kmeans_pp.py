import sys, math, pandas as pd, numpy as np, mykmeanssp as c_kmeans

global K, iters, points, eps
K = None
iters = 300
points = []
eps = None

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

def parse_files(file_name_1, file_name_2):
    df1 = pd.read_csv(file_name_1, header = None, index_col = 0)
    df2 = pd.read_csv(file_name_2, header = None, index_col = 0)
    joined = pd.concat([df1, df2], axis = 1, join = "inner")
    joined = joined.sort_values(by=0)
    return joined.to_numpy(), joined.index.values

def main():
    global K, iters, eps
    args = sys.argv[1:]

    file_name_1 = None
    file_name_2 = None

    if len(args) == 5:
        K = args[0]
        iters = args[1]
        eps = args[2]
        file_name_1 = args[3]
        file_name_2 = args[4]

    elif len(args) == 4:
        K = args[0]
        eps = args[1]
        file_name_1 = args[2]
        file_name_2 = args[3]
    
    else:
        print("An Error Has Occurred")
        sys.exit(1)

    try:
        K = int(K)
    except Exception:
        print("Incorrect number of clusters!")
        sys.exit(1)
    if K < 2:
        print("Incorrect number of clusters!")
        sys.exit(1)

    try:
        eps = float(eps)
    except Exception:
        print("Incorrect epsilon!")
        sys.exit(1)
    if eps < 0:
        print("Incorrect epsilon!")
        sys.exit(1)

    if len(args) == 5:
        try:
            iters = int(iters)
            if (iters < 2) or (iters > 799):
                print("Incorrect maximum iteration!")
                sys.exit(1)
        except Exception:
            print("Incorrect maximum iteration!")
            sys.exit(1)

    matrix, indices = parse_files(file_name_1, file_name_2)

    if K >= len(matrix):
        print("Incorrect number of clusters!")
        sys.exit(1)

    centers, center_indices = kmeans_pp_init(matrix, indices, K)
    print(",".join(map(str, center_indices.astype(int))))

    updated_centers = c_kmeans.fit(iters, eps, matrix.tolist(), centers.tolist())
        
    for center in updated_centers:
        formatted_cords = (f"{c:.4f}" for c in center)
        print(*formatted_cords, sep=",")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        print("An Error Has Occurred")
        sys.exit(1)

