#include <stdlib.h>
#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>

using namespace std;

int main () {

	string cmd;

	while(1) {
		cmd.clear();
		cout << "Enter a command" << endl;
	
		// break on ctrl d
		if(!getline(cin, cmd)) {
			break;
		}
		
		// break on exit or quit
		if ((cmd == "exit") || (cmd == "quit")) {
			break;
		}


		vector<char*> vec;

		// Extract the first token
		char * token = strtok((char*)cmd.c_str(), "|");

		// Push first token
		vec.push_back(token);

		// loop through the string to extract all other tokens
		while (token != NULL) {
			//		cout << token << endl; //printing each token
			token = strtok(NULL, "|");

			// push token to vec
			vec.push_back(token);
		}

		// vector of vectors to store each parsed command
		vector< vector<char*> > commands;

		//cout << "beginning for loop" << endl;	

		// parse each command by space
		for (int i = 0; i < vec.size() - 1; i++) {

			// create temporary vector
			vector<char*> tmp;
			//cout << "created tmp for command " << i << endl;

			// Extract the first token
			char * token2 = strtok((char*)vec[i], " ");

			// add first token
			tmp.push_back(token2);

			// loop to find other tokens
			while (token2 != NULL) {
				//cout << token2 << endl; //printing each token
				token2 = strtok(NULL, " ");

				tmp.push_back(token2);
			}

			//tmp.push_back(NULL);
			commands.push_back(tmp);
			//cout << "pushed command " << i << endl;

		}


		//-----------------------------------------------------------------

		int numCommands = commands.size();
		int fd[numCommands-1][2];

		for (int i = 0; i < numCommands - 1; i++) {
			if (pipe(fd[i]) < 0) {
				cout << "Pipe failed" << endl;
				return 1;
			}

		}


//		cout << "Parent PID: " << getpid() << endl;

		// 1 - write end
		// 0 - read end

		// create a child to execute each command, parent only waits
		for(int i=0; i < numCommands; i++) // loop will run numCommands times  
		{ 
			if(fork() == 0) 
			{ 
				//cout << "Child " << i+1 << " pid " << getpid() << " from Parent pid " << getppid() << endl; 



				for (int j = 0; j < numCommands - 1; j++) {

					if ((j == i) || (j == i - 1)) { // if current or past command

						if (j == i) { // current command
							close(fd[j][0]); // don't need to read from current pipe, only writing
						}
						else if (j == i - 1) { // previous command
							close(fd[j][1]); // don't need to write to previous pipe, only reading
						}
					}
					else {	
						close(fd[j][0]);
						close(fd[j][1]);
					}
				}

				//commands[i] --> vector of parsed current command
				if (i == 0) {
					dup2(fd[i][1],1);

					close(fd[i][1]);

					execvp(commands[i][0], commands[i].data());

				}
				else if (i == numCommands - 1) { // last child
					dup2(fd[i-1][0],0);

					close(fd[i-1][0]);

					execvp(commands[i][0], commands[i].data());
				}
				else {
					dup2(fd[i][1],1);
					dup2(fd[i-1][0],0);

					close(fd[i][1]);
					close(fd[i-1][0]);

					execvp(commands[i][0], commands[i].data());
				}

				exit(0); // kill child	
			} // if

			if (i > 0) {
				close(fd[i-1][0]);
				close(fd[i-1][1]);
			}

			wait(NULL);
		} // for loop

	}

	return 0;
}	
