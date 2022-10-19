#include "AnalogOut.h"
#include "DigitalIn.h"
#include "DigitalOut.h"
#include "PinNames.h"
#include "mbed.h" 
#include "deployableSolar.h"
#include <ctime>
#include<chrono>


#define MAX_BATTERY_DELAY 5000000
#define BURN_RESISTOR_VAL 1
#define BATTERY_THRESH 0.7

//defining all the pins which are needed for the program

//this is going to be for burn line 1
DigitalOut burnOne(PA_12);

//this is going to be for burn  line 2
DigitalOut burnTwo(PA_10);

//limit switch signal
DigitalIn switchOne(PA_9);
DigitalIn switchTwo(PA_11);

//this will be the vairables which are going to be used for the batteries
AnalogIn battery(ADC_VBAT);

int main()
{
    //setting up the push down buttons on the solar panel
    //to pull up mode, to allow an easy togglement between 0 and 1
    switchOne.mode(PullUp);
    switchTwo.mode(PullUp);
    bool endProgarm = false; 
    bool startBurnOne = true;
    bool startBurnTwo = false;

    //having this as the total time the burn resistors can burn for which is one second
    int burnResistorMax = 1 * CLOCKS_PER_SEC;

    printf("Launch sequence timer\n");

    //waiting till the launch timer has finished
    //a cubesat will take 30 minutes to laucnh, we have done for 45 minutes for extra precautionary steps
    wait_us(2700000000);

    printf("Launch sequence finished, and now in orbit\n");

    //TODO:come back and implement this functionlaity
    //checking battery state to ensure, it's appropriately charged
    /*if (!checkBattery())
    {
        //the battery is not charged to the appropriate capacity 
        waitToChargeBattery();

    }*/

    //checking deployment state just in case programme has being restarted through launch
    if (checkDeploymentState(switchOne.read(), switchTwo.read()))
    {
        //we want to end the program now, and ending with a success
        printf("System has successfuslly deployed on restart\n");
        //exiting the programme, so the rest of the code doesn't execute or run
        exit(0);
    }

    //timers used to monitor the state of the burn resistors
    clock_t startTimer = clock();
    clock_t startTimerTwo = 0;
    clock_t upTime = clock();
    int time = 0;

    while(true)
    {
        //getting a time the programme has being running for
        time = clock() - upTime;
        printf("Current Time %d \n", time); 

        //start the first burn resistor
        if (startBurnOne)
        {
            printf("Activating the first burn resistor \n");
            startBurnResistorOne();
        }

        //start the second burn resistor
        if (startBurnTwo)
        {
            printf("Activating the second burn resistor \n");
            startBurnResistorTwo();
        }
        
        //checking the deployment state 
        if(checkDeploymentState(switchOne.read(), switchTwo.read()))
        {
            printf("System has deployed from the burning of the resistor\n");
            exit(0);
        }

        //checking the elapsed time, to switch over the burn resistors
        if (clock() - startTimer > burnResistorMax)
        {

            //we will want to start this timer once, and that's it
            if ( startBurnOne && !startBurnTwo )
            {
                printf("Starting second burn resitor's time\n");
                startTimerTwo = clock();
            }

            //switching off burn resistor one, and turning on burn resistor two
            startBurnOne = false;
            startBurnTwo = true;
            
        }

        //checking the elapsed time for the second burn resistor
        if(clock() - startTimerTwo > burnResistorMax)
        {
            //we would like to switch off both resistors now
            startBurnOne = false;
            startBurnTwo = false;
        }
        //checking the error state of the system
        if ( !startBurnOne && !startBurnTwo)
        {
            if (!checkDeploymentState(switchOne.read(), switchTwo.read()))
            {
                printf("Both resistors have finsihed burning, and I still have not deployed\n");

                //exiting the program with an error code for that something is going to be wrong in the programme
                exit(1);
            }
        }
    }
}

/*
PURPOSE: Actiavates the first burn resistor
*/
void startBurnResistorOne()
{
    burnOne.write(BURN_RESISTOR_VAL);
}

/*
PURPOSE: Actiavates the second burn resistor
*/
void startBurnResistorTwo()
{
    burnTwo.write(BURN_RESISTOR_VAL);
}

/*
PURPOSE: To allow sufficient time for the CubeSat's batteries to charge to the appropriate levels
*/
void waitToChargeBattery()
{
    printf("Battery is not sufficient, waitig for a day\n");

    //only going to allow for 5 seconds for testing purposes 
    //in real life this will be a lot longer
    wait_us(MAX_BATTERY_DELAY);
    printf("I am done charging now, lets try to re-deploy again\n");
}


/*
PURPOSE: to check if the solar panel has deployed by checking the states of the limit swtiches
-   if both limit swithces are low, the solar panel has deployed succesfully
-   if both limit siwthces are high, the solar panel is still in the stowed position
-   if either limit switch is low, the system is most likely in an error state
*/
bool checkDeploymentState(uint8_t switchOne, uint8_t  switchTwo)
{
    bool valid = false;

    //the system will be deployed when the limit switches are not pressed 
    //switchOne = !switchOne;
    //switchTwo = !switchTwo; 
   // printf("Switch One State: %d \n", switchOne);
    //printf("Switch Two State: %d \n", switchTwo);

    //if both switches are not pressed down 
    if (switchOne && switchTwo)
    {
        valid = true;
    }
    else if(!switchOne && switchTwo)
    {
        printf("Switch two side of the panel hasn't deployed as yet\n");
        //I don't think that we will need this as yet. But, you should do something here
    }
    else if(switchOne && !switchTwo)
    {
        printf("Switch one side of the panel hasn't deployed as yet\n");
        //I don't think that we will need this as yet. But, you should do something here
    }

    //printf("Return value: %d \n", valid);

    return valid;
}

/*
PURPOSE: To check the battery status of the battery, to ensure it's sufficient enough to activate the burn resisors
*/
bool checkBattery()
{
    //TODO: you will need to have a look in the teams discussions to see what the appropriate way is to do it 
    //checks if the battery is going to be above the required limt
    //YOU WILL NEED TO FINISH THIS
    bool valid = false;

    printf("The curent battery voltage: %d \n", battery.read());

    //if the battery is more than 60% carged
    if (battery.read() >= BATTERY_THRESH)
    {
        valid = true;
    }
    
    return valid;
}