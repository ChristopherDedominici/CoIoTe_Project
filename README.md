#CoIoTe Project: optimization algorithm.


General description:

A number of different tasks:

- People are able to control the status of the dumpsters through a sensor system located into the dumpsters. 
- Collecting data directly from the dumpsters they can inform the trash company and ask to empty the full dumpsters

- People need to complete different tasks at different time steps during the day in a certain number of places and cities. The trash company can give people a prize/small payment in order to involve them to go in that zone.
- There are three different types of people: type 0, type 1 and type 2. Each type can complete a different number of tasks in a time step and the cost of the type depends on how many tasks they can complete. 

--------

Goal:
The goal of the project is to minimise the total amount of the reward.

Constrains:
Every dumpster must be visited;
The total amount of requests cannot exceed the number of people in the cell.

--------

Input file:

In the input file we have:
	-number of cells in which the city is divided;
	-number of time steps available;
	-tasks that must be solved in every cell at the end of all the time steps;
	-users available in each time step.

--------

Variables:

There are nT different time steps.
A city is divided in nC cells. 
In some of the N cells there are a nA number of tasks to be solved.
In some of the N cells there also nP people that can complete those tasks.
People are divided in three type: - type 0 can complete 1 tasks every time step
                          		      - type 1 can complete 2 tasks every time step 
				      - type 2 can complete 3 tasks every time step

--------

Result: On 120 instances we have an average of 0,15% from the optimal solution.

--------

You can also find the PDF file with a more detailed explanation and others uploaded files.   

