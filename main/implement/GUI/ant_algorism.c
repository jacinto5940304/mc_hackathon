#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_NODES 1000
#define ALPHA 1.0
#define BETA 2.0
#define RHO 0.1

typedef struct {
    int x, y;
} Point;

Point coordinates[MAX_NODES];
double pheromone[MAX_NODES][MAX_NODES];

int num_nodes;
int start_node; // Starting node for the ant

double calculate_distance(Point p1, Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

void initialize_pheromone() {
    for (int i = 0; i < num_nodes; i++) {
        for (int j = 0; j < num_nodes; j++) {
            pheromone[i][j] = 1.0;
        }
    }
}

void ant_colony_optimization() {
    initialize_pheromone();
    double max_pheromone = 0.0;
    int most_likely_node = start_node;
    for (int i = 0; i < num_nodes; i++) {
        if (i == start_node) continue;
        double distance = calculate_distance(coordinates[start_node], coordinates[i]);
        double probability = pow(pheromone[start_node][i], ALPHA) / pow(distance, BETA);
        if (probability > max_pheromone) {
            max_pheromone = probability;
            most_likely_node = i;
        }
    }
    printf("Most likely node: %d\n", most_likely_node);
}
int main() {
    num_nodes = 100;
    start_node = 0;  // Starting node for the ant
    FILE *file = fopen("read_pos.txt", "r");
    if (file == NULL) {
        printf("File can not be found");
        return 1;  // 離開程式，因為無法讀取檔案
    } else {
        int pos_x, pos_y;
        int i = 0;
        while (fscanf(file, "%d %d", &pos_x, &pos_y) == 2) { // 檢查 fscanf 的回傳值
            coordinates[i].x = pos_x;
            coordinates[i].y = pos_y;
            i++;
        }
        fclose(file);  // 記得關閉檔案
    }
    ant_colony_optimization();
}
