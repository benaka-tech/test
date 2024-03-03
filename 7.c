#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <unistd.h> // for sleep function

struct Node {
    int id;
    bool isAlive;
    double utilization;
    // Add other relevant information
};

std::vector<Node> clusterNodes;

void simulateNodeActivity(int nodeId) {
    while (true) {
        #pragma omp critical
        {
            // Simulate resource utilization on node
            clusterNodes[nodeId].utilization = static_cast<double>(rand()) / RAND_MAX;

            // Simulate node processing and health checking
            std::cout << "Node " << nodeId << " - Resource Utilization: " << clusterNodes[nodeId].utilization << "\n";

            // Simulate detecting a crash
            if (clusterNodes[nodeId].utilization < 0.1) {
                clusterNodes[nodeId].isAlive = false;
                std::cout << "Node " << nodeId << " detected a crash.\n";
            }
        }

        // Simulate some work on the node
        sleep(2); // Sleep for 2 seconds

        // Simulate rejoining (creating a new thread)
        #pragma omp critical
        {
            if (!clusterNodes[nodeId].isAlive) {
                clusterNodes[nodeId].isAlive = true;
                std::cout << "Node " << nodeId << " has rejoined.\n";
            }
        }
    }
}

int main() {
    const int numNodes = 3;

    // Initialize cluster nodes
    for (int i = 0; i < numNodes; ++i) {
        Node newNode;
        newNode.id = i;
        newNode.isAlive = true;
        clusterNodes.push_back(newNode);
    }

    // Simulate node activity using OpenMP threads
    #pragma omp parallel num_threads(numNodes)
    {
        int nodeId = omp_get_thread_num();
        simulateNodeActivity(nodeId);
    }

    return 0;
}
