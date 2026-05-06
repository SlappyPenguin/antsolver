/*
On the turn and river, card sets are clustered into 1000 clusters according to the distribution-aware card 
abstraction from https://www.ifaamas.org/Proceedings/aamas2013/docs/p271.pdf. 

To speed up the k-means clustering, k-means++ from https://theory.stanford.edu/~sergei/papers/kMeansPP-soda.pdf
is used to initialise the cluster centres. To speed up convergence, Hamerly's k-means optimisation is used from
https://www.ccs.neu.edu/home/radivojac/classes/2024fallcs6140/hamerly_sdm_2010.pdf. 
*/

#include <chrono>
#include <iomanip>
#include <iostream>
#include "../../include/random.h"
#include "../../include/solver.h"
using namespace std;

constexpr int MAX_NUM_CLUSTERS = []{
    return max(NUM_CLUSTERS[(int) Street::Flop], NUM_CLUSTERS[(int) Street::Turn]);
}();
constexpr float INF = 1e9;
constexpr arr<int, NUM_STREETS> NUM_ROUNDS = {0, 10, 2, 0};
constexpr arr<int, NUM_STREETS> NUM_ITERATIONS = {0, 75, 5, 0};
const arr<str, NUM_STREETS> DISTRIBUTIONS_FILE = {
    "", "../data/flop_distributions.bin",
    "../data/turn_distributions.bin", ""
};
const arr<str, NUM_STREETS> CLUSTERS_FILE = {
    "", "../data/flop_clusters.bin",
    "../data/turn_clusters.bin", ""
};

int num_points, num_clusters;
vec<lint> ids;
vec<arr<float, NUM_INTERVALS>> point;
vec<int> clusters;
vec<float> lower, upper;
void init_street(int street) {
    num_points = NUM_SETS[street], num_clusters = NUM_CLUSTERS[street];
    ifstream file(DISTRIBUTIONS_FILE[street], ios::binary);
    ids.resize(num_points), point.resize(num_points), clusters.resize(num_points), lower.resize(num_points), upper.resize(num_points);
    read_range(file, ids);
    for (int i = 0; i < num_points; i++) 
        for (int j = 0; j < NUM_INTERVALS; j++)
            read(file, point[i][j]);
}

inline float dist(const arr<float, NUM_INTERVALS>& x, const arr<float, NUM_INTERVALS>& y) {
    float ans = 0, diff = 0;
    for (int i = 0; i < NUM_INTERVALS; i++) {
        diff += x[i] - y[i];
        ans += abs(diff);
    }
    return ans;
}
arr<arr<float, NUM_INTERVALS>, MAX_NUM_CLUSTERS> centre;
void init_centre() {
    centre[0] = point[get_generator().get_random_int(0, num_points - 1)];
    vec<float> min_dist(num_points, INF);
    for (int i = 1; i < num_clusters; i++) {
        vec<float> squared_dist;
        float sum = 0;
        for (int j = 0; j < num_points; j++) {
            min_dist[j] = min(min_dist[j], dist(centre[i - 1], point[j]));
            squared_dist.push_back(min_dist[j] * min_dist[j]);
            sum += squared_dist.back();
        }

        bool found = false;
        float random = sum * get_generator().get_random_float();
        for (int j = 0; j < num_points; j++) {
            random -= squared_dist[j];
            if (random > 0) continue;
            centre[i] = point[j], found = true;
            break;
        }
        if (!found) centre[i] = point[num_points - 1];
    }
}

void find_cluster(int point_id) {
    pair<float, int> closest = {INF, -1};
    for (int i = 0; i < num_clusters; i++)
        closest = min(closest, {dist(point[point_id], centre[i]), i});
    upper[point_id] = closest.first, clusters[point_id] = closest.second;

    lower[point_id] = INF;
    for (int i = 0; i < num_clusters; i++) {
        if (i == clusters[point_id]) continue;
        lower[point_id] = min(lower[point_id], dist(centre[i], point[point_id]));
    }
}

arr<arr<float, NUM_INTERVALS>, MAX_NUM_CLUSTERS> sum;
arr<int, MAX_NUM_CLUSTERS> num;
arr<float, MAX_NUM_CLUSTERS> moved, half;
void init_data() {   
    fill(num.begin(), num.end(), 0);
    fill(sum.begin(), sum.end(), arr<float, NUM_INTERVALS>{0});
    for (int i = 0; i < num_points; i++) {
        find_cluster(i);    
        num[clusters[i]]++;
        for (int j = 0; j < NUM_INTERVALS; j++)
            sum[clusters[i]][j] += point[i][j];
    }
}

void upd_centres() {
    for (int i = 0; i < num_clusters; i++) {
        arr<float, NUM_INTERVALS> old_centre = centre[i];
        for (int j = 0; j < NUM_INTERVALS; j++)
            centre[i][j] = sum[i][j] / num[i];
        moved[i] = dist(old_centre, centre[i]);
    }
}
void upd_data() {
    pair<float, int> most_moved = {-INF, -1};
    for (int i = 0; i < num_clusters; i++)
        most_moved = max(most_moved, {moved[i], i});
    pair<float, int> most_moved_2nd = {-INF, -1};
    for (int i = 0; i < num_clusters; i++) {
        if (i == most_moved.second) continue;
        most_moved_2nd = max(most_moved_2nd, {moved[i], i});
    }            
    for (int i = 0; i < num_points; i++) {
        upper[i] += moved[clusters[i]];
        if (clusters[i] == most_moved.second) lower[i] -= most_moved_2nd.first;
        else lower[i] -= most_moved.first;
    }
}

void do_iteration() {
    for (int i = 0; i < num_clusters; i++) {
        half[i] = INF;
        for (int j = 0; j < num_clusters; j++) {
            if (j == i) continue;
            half[i] = min(half[i], dist(centre[i], centre[j]) / 2); 
        }
    }

    for (int i = 0; i < num_points; i++) {
        float bound = max(half[clusters[i]], lower[i]);
        if (upper[i] <= bound) continue;

        upper[i] = dist(centre[clusters[i]], point[i]);
        if (upper[i] <= bound) continue;

        int old_cluster = clusters[i];
        find_cluster(i);
        if (clusters[i] == old_cluster) continue;

        num[old_cluster]--, num[clusters[i]]++;
        for (int j = 0; j < NUM_INTERVALS; j++) {
            sum[old_cluster][j] -= point[i][j];
            sum[clusters[i]][j] += point[i][j];
        }
    }

    upd_centres(); 
    upd_data();
}

float get_score() {
    float score = 0;
    for (int i = 0; i < num_points; i++) {
        float dist_to_centre = dist(centre[clusters[i]], point[i]);
        score += dist_to_centre * dist_to_centre;
    }
    return score;
}

void print_clusters(const vec<int>& best_clusters, int street) {
    ofstream file(CLUSTERS_FILE[street], ios::binary);
    write_range(file, ids);
    for (int i = 0; i < num_points; i++) {
        short cluster = best_clusters[i];
        write(file, cluster);
    } 
}

int main() {
    cout << "Generating for... ";
    auto start_time = chrono::high_resolution_clock::now();

    for (int street : {(int) Street::Flop, (int) Street::Turn}) {
        init_street(street);
        float best_score = INF;
        vec<int> best_clusters;
        for (int round = 0; round < NUM_ROUNDS[street]; round++) {
            init_centre();
            init_data();

            for (int i = 0; i < NUM_ITERATIONS[street]; i++) 
                do_iteration();
        
            float score = get_score();
            if (score >= best_score) continue;
            best_score = score, best_clusters = clusters;
        }
        print_clusters(best_clusters, street);
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<doub> elapsed_time = end_time - start_time;
    cout << fixed << setprecision(3);
    cout << elapsed_time.count() << " seconds" << '\n';
}




 