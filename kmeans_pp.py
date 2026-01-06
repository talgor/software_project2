import sys, math

global DIM, K, ITER, points, EPSILON
DIM = None
K = None
ITER = 400
points = []
EPSILON = 0.001

class Point:
    def __init__(self, coordinates):
        self.coords = tuple(coordinates)
        self.d = len(self.coords)
        self.closestCenter = None

    def updateClosestCenter(self, CenterPoints):
        for center in CenterPoints:
            dist = self.distanceTo(center)
            if self.closestCenter is None or dist < self.distanceTo(self.closestCenter):
                self.closestCenter = center

    def distanceTo(self, otherPoint):
        if self.d != otherPoint.d:
            raise ValueError("Points must have the same dimension to calculate distance.")
        return math.sqrt(sum((a - b) ** 2 for a, b in zip(self.coords, otherPoint.coords)))

class CenterPoint:
    def __init__(self, coordinates):
        self.coords = tuple(coordinates)
        self.d = len(self.coords)
        self.assignedPoints = []

    def updateCoordinates(self):
        sum_squared = 0
        if not self.assignedPoints:
            return
        new_coords = []
        for i in range(self.d):
            coord_sum = sum(point.coords[i] for point in self.assignedPoints)
            new_coords.append(round(coord_sum / len(self.assignedPoints), 4))
            squared_diff = (new_coords[-1] - self.coords[i]) ** 2
            sum_squared += squared_diff
        if len(new_coords) != self.d:
            raise ValueError("Dimension mismatch when updating coordinates.")
        self.coords = tuple(new_coords)
        self.assignedPoints = []
        movement_distance = math.sqrt(sum_squared)
        return movement_distance > EPSILON
    
def updateAllPoints(points, centerPoints):
    for point in points:
        try:
            point.updateClosestCenter(centerPoints)
        except Exception as e:
            print(f"Error updating closest center for a point: {e}")
            continue
        point.closestCenter.assignedPoints.append(point)

def updateAllCenters(centerPoints):
    stopRun = True
    for center in centerPoints:
        if center.updateCoordinates():
            stopRun = False
    return stopRun

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
    parse(sys.stdin)
    if(K >= len(points)):
        print("Incorrect number of clusters!")
        sys.exit(1)

    if points:
        final_centers = kmeans(points, K)
        
        for center in final_centers:
            formatted_coords = (f"{c:.4f}" for c in center.coords)
            print(*formatted_coords, sep=",")


if __name__ == "__main__":
    try:
        main()
    except Exception:
        print("An Error Has Occured")
        sys.exit(1)



