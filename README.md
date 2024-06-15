# UAT-Neural-Network
### Genreal Description

This repository features a Neural Network built entirely from scratch using C++ programming language. The Neural Network features a multi-layered architecture optimized for multi-core processors and utilizes the concepts of multi-threading to efficiently process data.

Initially the network consists of 3 Layers; Input, Middle and the Output Layer, but the network can dynamically creates additional internal layers based on the user input where each layer consists of multiple dynamically created neurons to input and process data. The network supports both forward and backward propagation and uses pipes to communicate data between different layers of the architecture.

Other than that to ensure thread-safe operations and and prevent race conditions, the Neural Network also utilizes mutexes and semaphores to control memory access between multiple shared resources.

### Technologies Used: 
C++ | Operating Systems

### How to Install
Initially you are required to Install a copy of Ubuntu or any other Linus distro in you system.
It is upto you wheater you want to go with Virtual Machine or Dual Boot.
Also, in case you want to set up a Virtual Mechine, the choice is yours wheather VM Ware or Virtual Box.
While programming the Neural Network I went with Virtual Box + Ubuntu configuration.

To Install Ubuntu I have attached multiple resources below:
https://www.youtube.com/watch?v=DhVjgI57Ino
https://www.youtube.com/watch?v=NhlhJFKmzpk
https://www.youtube.com/watch?v=qq-7X8zLP7g

After this you would need to install g++ complier in Ubuntu
https://www.youtube.com/watch?v=QKUhMOd-KOM

### Running the Neural Network
Finally now download all the files and paste everything in a folder.
Then Right Click.
Open Terminal Here.
Then type chmod +X run.sh and Press Enter.
Then type ./run
Press Enter and your good to go.
While running the Neural Network, it will ask you to Enter the Number of layers and Neurons.
The range for both of the values is between 2 and 8.

### Warning
Always keep the config file in the folder where you kept all the other code.
Also remember to never delete a row from the config file and always keep the number of lines to 51.

<img width="731" alt="1" src="https://github.com/umairkhalidx/UAT-Neural-Network/assets/109782978/0d77c2dc-8559-4fd8-b630-3d728c8ad7ad">

<img width="715" alt="2" src="https://github.com/umairkhalidx/UAT-Neural-Network/assets/109782978/f96dc160-80b0-4120-ae0c-9d583f2844a2">

<img width="512" alt="3" src="https://github.com/umairkhalidx/UAT-Neural-Network/assets/109782978/827f2ff6-ca04-4d6d-aa37-e1f26e71fb2b">

<img width="801" alt="4" src="https://github.com/umairkhalidx/UAT-Neural-Network/assets/109782978/07ce2846-60ef-4692-adad-b4d91d2deacb">

<img width="777" alt="5" src="https://github.com/umairkhalidx/UAT-Neural-Network/assets/109782978/123fe328-497f-4c4e-8eba-380107740cc7">








