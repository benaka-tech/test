#include <iostream>
#include <vector>
#include <omp.h>
#include <chrono>
#include <thread>
#include <ctime>
#include <cstdlib>

struct Node {
    int id;
    bool isAlive;
    int utilization; // Simulated resource utilization
};

std::vector<Node> clusterNodes;

void slaveRoutine(int nodeId) {
    while (true) {
        // Simulate resource utilization
        int utilization = rand() % 101; // Random utilization between 0 and 100
        #pragma omp critical
        {
            std::cout << "Node " << nodeId << " - Resource Utilization: " << utilization << "%\n";
            clusterNodes[nodeId].utilization = utilization;
        }

        // Sleep for demonstration purposes
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void masterRoutine() {
    while (true) {
        // Simulate collecting resource status on slaves
        #pragma omp parallel for
        for (int i = 0; i < clusterNodes.size(); ++i) {
            #pragma omp critical
            {
                std::cout << "Master collecting resource status from Node " << clusterNodes[i].id
                          << " - Utilization: " << clusterNodes[i].utilization << "%\n";
            }
        }

        // Simulate detecting a slave crash (randomly select one node to crash)
        int crashedNode = rand() % clusterNodes.size();
        #pragma omp critical
        {
            std::cout << "Master detected a crash on Node " << clusterNodes[crashedNode].id << ".\n";
            clusterNodes[crashedNode].isAlive = false; // Mark the crashed node as dead
        }

        // Sleep for demonstration purposes
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

int main() {
    const int numNodes = 3;

    srand(static_cast<unsigned>(time(nullptr)));

    // Initialize cluster nodes
    for (int i = 0; i < numNodes; ++i) {
        Node newNode;
        newNode.id = i;
        newNode.isAlive = true;
        newNode.utilization = 0;
        clusterNodes.push_back(newNode);
    }

    // Master node
    #pragma omp parallel sections
    {
        #pragma omp section
        {
            std::cout << "Master node initialized.\n";
            masterRoutine();
        }

        // Slave nodes
        #pragma omp section
        {
            #pragma omp parallel for
            for (int i = 0; i < numNodes; ++i) {
                #pragma omp critical
                {
                    std::cout << "Initializing Node " << i << ".\n";
                }
                slaveRoutine(i);
            }
        }
    }

    return 0;
}
