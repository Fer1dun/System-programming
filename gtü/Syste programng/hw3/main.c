#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define MAX_AUTOMOBILES 8
#define MAX_PICKUPS 4
#define TOTAL_VEHICLES 12 // Total number of vehicles to be parked

sem_t newAutomobile, inChargeforAutomobile;
sem_t newPickup, inChargeforPickup;
sem_t finished; // Semaphore to signal the end of the simulation

int mFree_automobile = MAX_AUTOMOBILES;
int mFree_pickup = MAX_PICKUPS;
int parkedVehicles = 0;
int simulationEnded = 0; // Flag to signal the end of the simulation

// Array to keep track of parking spot statuses
char automobileSpots[MAX_AUTOMOBILES] = {0};
char pickupSpots[MAX_PICKUPS] = {0};

// Function for car owner behavior
void *carOwner(void *arg) {
    while (!simulationEnded) {
        sleep(rand() % 3); // Simulate time taken for cars to arrive

        int vehicleType = rand() % 2; // Randomly determine vehicle type (0 for automobile, 1 for pickup)

        if (vehicleType == 0) { // Automobile
            sem_wait(&inChargeforAutomobile);
            if (mFree_automobile > 0) {
                for (int j = MAX_AUTOMOBILES - 1; j >= 0; j--) {
                    if (automobileSpots[j] == 0) {
                        automobileSpots[j] = 1;
                        mFree_automobile--;
                        parkedVehicles++; // Increment parkedVehicles here
                        printf("Automobile parked in spot %d. Free automobile spots: %d\n", j + 1, mFree_automobile);
                        sem_post(&newAutomobile);
                        break;
                    }
                }
            } else {
                printf("No free spots for automobiles. Automobile exits.\n");
            }
            sem_post(&inChargeforAutomobile);
        } else { // Pickup
            sem_wait(&inChargeforPickup);
            if (mFree_pickup > 0) {
                for (int j = MAX_PICKUPS - 1; j >= 0; j--) {
                    if (pickupSpots[j] == 0) {
                        pickupSpots[j] = 1;
                        mFree_pickup--;
                        parkedVehicles++; // Increment parkedVehicles here
                        printf("Pickup parked in spot %d. Free pickup spots: %d\n", j + 1, mFree_pickup);
                        sem_post(&newPickup);
                        break;
                    }
                }
            } else {
                printf("No free spots for pickups. Pickup exits.\n");
            }
            sem_post(&inChargeforPickup);
        }

        if (parkedVehicles == TOTAL_VEHICLES) {
            sem_post(&finished); // Signal the end of parking
            simulationEnded = 1; // Set the flag to end the simulation
            sem_post(&newAutomobile); // Wake up the attendant in case it's waiting
            sem_post(&newPickup); // Wake up the attendant in case it's waiting
        }
    }
    return NULL;
}

// Function for car attendant behavior
void *carAttendant(void *arg) {
    while (1) {
        // Attend to automobiles
        sem_wait(&newAutomobile);
        sem_wait(&inChargeforAutomobile);
        if (simulationEnded) {
            sem_post(&inChargeforAutomobile);
            break;
        }
        printf("Attendant is processing an automobile.\n");
        sem_post(&inChargeforAutomobile);

        // Attend to pickups
        sem_wait(&newPickup);
        sem_wait(&inChargeforPickup);
        if (simulationEnded) {
            sem_post(&inChargeforPickup);
            break;
        }
        printf("Attendant is processing a pickup.\n");
        sem_post(&inChargeforPickup);
    }
    return NULL;
}

int main() {
    srand(time(NULL));
    pthread_t carOwnerThread, carAttendantThread;

    // Initialize semaphores
    sem_init(&newAutomobile, 0, 0);
    sem_init(&inChargeforAutomobile, 0, 1);
    sem_init(&newPickup, 0, 0);
    sem_init(&inChargeforPickup, 0, 1);
    sem_init(&finished, 0, 0);

    // Create threads
    pthread_create(&carOwnerThread, NULL, carOwner, NULL);
    pthread_create(&carAttendantThread, NULL, carAttendant, NULL);

    // Wait for the threads to finish
    pthread_join(carOwnerThread, NULL);
    pthread_join(carAttendantThread, NULL);

    // Destroy semaphores
    sem_destroy(&newAutomobile);
    sem_destroy(&inChargeforAutomobile);
    sem_destroy(&newPickup);
    sem_destroy(&inChargeforPickup);
    sem_destroy(&finished);

    printf("Simulation finished.\n");
    return 0;
}
