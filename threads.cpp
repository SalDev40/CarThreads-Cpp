#include <pthread.h>
#include <iostream>
#include <string>
#include <unistd.h>

struct carData
{
    int numCar;
    int timeDelay;
    char direction;
    int timeTaken;
};

//child thread which will decide which cars to let into the tunnel
// if there is room for more southbound cars and total cars in tunnel
// is not full it will let a south car in if it has lock and vice versa

void *carTunnel(void *arg);

//max total of all cars
int maxCars;
int maxNBcars = 0;
int maxSBcars = 0;

//track of how many south and nb cars in tunnel
int sbCars = 0;
int nbCars = 0;

//track of how many south and north cars
int southBound = 0;
int northBound = 0;

int delayed = 0;     //how many cars had to wait
int currentCars = 0; //total cars in tunnel

static pthread_mutex_t tunnelLock;
static pthread_cond_t fullTunnel = PTHREAD_COND_INITIALIZER;

int main()
{
    int southTotalCars = 0;
    int northTotalCars = 0;
    int totalCars = 0;

    pthread_t cartid[100];
    pthread_mutex_init(&tunnelLock, NULL);

    //get data about cars
    std::string word;
    std::cin >> word;
    maxCars = stoi(word);
    std::cin >> word;
    maxNBcars = stoi(word);
    std::cin >> word;
    maxSBcars = stoi(word);

    std::cout << "Maximum number of cars in the tunnel: " << maxCars << std::endl;
    std::cout << "Maximum number of north bound cars in the tunnel: " << maxNBcars << std::endl;
    std::cout << "Maximum number of south bound cars in the tunnel: " << maxSBcars << std::endl;
    while (std::cin >> word)
    {
        sleep(stoi(word)); //sleep for time
        std::cin >> word;
        carData *temp = new carData;
        temp->direction = word[0];
        std::cin >> word;
        temp->timeTaken = stoi(word);

        if (temp->direction == 'S')
        {
            temp->numCar = southTotalCars + 1;
        }
        else
        {
            temp->numCar = northTotalCars + 1; //number of the car being put
        }

        //create child thread
        pthread_t tid;
        pthread_create(&tid, NULL, carTunnel, (void *)temp);
        cartid[totalCars] = tid;

        //increment num of cars in that direction
        if (temp->direction == 'S')
        {
            southTotalCars++;
        }
        else
        {
            northTotalCars++;
        }

        totalCars++;
    }

    //join threads
    for (int i = 0; i < totalCars; i++)
        pthread_join(cartid[i], NULL);

    //display report
    std::cout << northBound << " northbound car(s) crossed the tunnel." << std::endl;
    std::cout << southBound << " southbound car(s) crossed the tunnel." << std::endl;
    std::cout << delayed << " car(s) were delayed." << std::endl;

    return 0;
}

void *carTunnel(void *arg)
{
    //get paramter input
    carData *car = (struct carData *)arg;
    bool toWait = false;

    //if south bound car
    if (car->direction == 'S')
    {
        //get lock
        pthread_mutex_lock(&tunnelLock);
        southBound++;
        std::cout << "SouthBound car # " << car->numCar << " arrives at the tunnel." << std::endl;
        //conditional wait until it can enter tunnel
        if (currentCars >= maxCars || sbCars >= maxSBcars)
        {
            delayed++;
            std::cout << "-- Southbound car # " << car->numCar << " has to wait" << std::endl;
            while (currentCars >= maxCars || sbCars >= maxSBcars)
            {
                pthread_cond_wait(&fullTunnel, &tunnelLock);
            }
        }
        currentCars++;
        sbCars++;
        std::cout << "SouthBound car # " << car->numCar << " enters the tunnel." << std::endl;

        //release lock
        pthread_mutex_unlock(&tunnelLock);
        sleep(car->timeTaken);

        //signal condition
        pthread_mutex_lock(&tunnelLock);
        std::cout << "SouthBound car # " << car->numCar << " exits the tunnel." << std::endl;
        currentCars--;
        sbCars--;
        pthread_cond_signal(&fullTunnel);
        pthread_mutex_unlock(&tunnelLock);
    }
    else //nortbound cars
    {

        //get lock
        pthread_mutex_lock(&tunnelLock);
        northBound++;
        std::cout << "NorthBound car # " << car->numCar << " arrives at the tunnel." << std::endl;

        //conditional wait if not allowed in tunnel
        if (currentCars >= maxCars || nbCars >= maxNBcars)
        {
            delayed++;
            std::cout << "-- Northbound car # " << car->numCar << " has to wait" << std::endl;
            while (currentCars >= maxCars || nbCars >= maxNBcars)
            {
                pthread_cond_wait(&fullTunnel, &tunnelLock);
            }
        }
        currentCars++;
        nbCars++;
        std::cout << "NorthBound car # " << car->numCar << " enters the tunnel." << std::endl;

        //enter tunnel
        pthread_mutex_unlock(&tunnelLock);
        sleep(car->timeTaken);

        //signal
        pthread_mutex_lock(&tunnelLock);
        currentCars--;
        pthread_cond_signal(&fullTunnel);
        nbCars--;
        std::cout << "NorthBound car # " << car->numCar << " exits the tunnel." << std::endl;
        pthread_mutex_unlock(&tunnelLock);
    }
}