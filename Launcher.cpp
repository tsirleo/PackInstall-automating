#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

string beginning = beginning +"ansible-playbook /etc/ansible/playbook.yaml -e " "'{" + '"' + "MY_HOSTS" + '"' + ":[{" + '"' + "name" + '"' + ":[";
string name = name + "]},{" + '"' + "name" + '"' + ":[";
string acheck_true = acheck_true + "]," + '"' + "acheck" + '"' + ":" + '"' + "Y" + '"';
string acheck_false = acheck_false + "]," + '"' + "acheck" + '"' + ":" + '"' + "N" + '"';
string pcheck_true = pcheck_true + "]," + '"' + "pcheck" + '"' + ":" + '"' + "Y" + '"';
string pcheck_false = pcheck_false + "]," + '"' + "pcheck" + '"' + ":" + '"' + "N" + '"';
string ppackages = ppackages + "," + '"' + "ppackages" + '"' + ":[";
string apackages = apackages + "," + '"' + "apackages" + '"' + ":[";
string ending = ending + "]}]}" + "'";

int main() {
//-------------------- Variables -------------------//
	bool acheck = false, pcheck = false;
	pid_t pid;
	string output = "";
	string input;
	string prev;
	
//------------------- Json-string forming ------------------//
	output += beginning;
	getline(cin, input);
	prev = input;
	output = output + '"' + input + '"'; 
	getline(cin, input);
	while (input.find("-end-")==-1) {
		if (input.find("apt ")!=-1) {
			acheck = true;
			output = output + acheck_true + apackages + '"';
			for(int i=4; i<input.length(); i++) {
				if (input[i] != ' ')
					output += input[i];
				else
					output = output + '"' + "," + '"';
			}
			output += '"';
		}
		else if (input.find("pip ")!=-1) {
			pcheck = true;
			output = output + pcheck_true + ppackages + '"';
			for(int i=4; i<input.length(); i++) {
				if (input[i] != ' ')
					output += input[i];
				else
					output = output + '"' + "," + '"';
			}
			output += '"';
		}
		else {
			if (prev.find("apt ")==-1 && prev.find("pip ")==-1)
				output = output + "," + '"' + input + '"';
			else {
				if (!pcheck)
					output = output + pcheck_false + ppackages;
				else if (!acheck)
					output = output + acheck_false + apackages;
				output = output + name + '"' + input + '"';
				pcheck = false;
				acheck = false;
			}
		}
		prev = input;
		getline(cin, input);	
	}
	if (!pcheck)
		output = output + pcheck_false + ppackages;
	else if (!acheck)
		output = output + acheck_false + apackages;		
	output = output + ending;
	
//------------------- Launching a playbook for execution ----------------//
	if ((pid=fork())==-1) {
		cerr << "Error: Failed to calling fork()" << endl;
		exit(1);	
	}    
	if (pid==0) {
		//son process
		const char *out = output.c_str();
		execl("/bin/sh", "sh", "-c", out, NULL);
		cerr << "Error: Failed to calling exec()" << endl;
		exit(1);
	}  
	wait(NULL);
	return 0;
}
