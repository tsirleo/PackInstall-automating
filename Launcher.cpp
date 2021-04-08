#include <iostream>
#include <cstring>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

string beginning="", name="", acheck_true="", acheck_false="", pcheck_true="", pcheck_false="", ppackages="", apackages="", archeck_true="", archeck_false="", archives="", furl="", url="", dest="", ending="";

int main() {
//-------------------- Variables -------------------//
	beginning = beginning + "ansible-playbook /etc/ansible/playbook.yaml -e " "'{" + '"' + "MY_HOSTS" + '"' + ":[{" + '"' + "name" + '"' + ":[";
	name = name + "]},{" + '"' + "name" + '"' + ":[";
	acheck_true = acheck_true + "]," + '"' + "acheck" + '"' + ":" + '"' + "Y" + '"';
	acheck_false = acheck_false + "]," + '"' + "acheck" + '"' + ":" + '"' + "N" + '"';
	pcheck_true = pcheck_true + "]," + '"' + "pcheck" + '"' + ":" + '"' + "Y" + '"';
	pcheck_false = pcheck_false + "]," + '"' + "pcheck" + '"' + ":" + '"' + "N" + '"';
	ppackages = ppackages + "," + '"' + "ppackages" + '"' + ":[";
	apackages = apackages + "," + '"' + "apackages" + '"' + ":[";
	archeck_true = archeck_true + "]," + '"' + "archeck" + '"' + ":" + '"' + "Y" + '"';
	archeck_false = archeck_false + "]," + '"' + "archeck" + '"' + ":" + '"' + "N" + '"';
	archives = archives + "," + '"' + "arch" + '"' + ":[{";
	furl = furl + '"' + "url" + '"' + ":";
	url = url + "},{" + '"' + "url" + '"' + ":";
	dest = dest + "," + '"' + "dst" + '"' + ":";
	ending = ending + "]}]}" + "'";
	bool acheck = false, pcheck = false, archeck = false;
	pid_t pid;
	int k = 1;
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
			if (prev.find("arch ")!=-1)
				output += "}";
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
			if (prev.find("arch ")!=-1)
				output += "}";
			output = output + pcheck_true + ppackages + '"';
			for(int i=4; i<input.length(); i++) {
				if (input[i] != ' ')
					output += input[i];
				else
					output = output + '"' + "," + '"';
			}
			output += '"';
		}
		else if (input.find("arch ")!=-1) {
			archeck = true;
			output = output + archeck_true + archives + furl + '"';
			for(int i=5; i<input.length(); i++) {
				if (input[i] != ' ')
					output += input[i];
				else {
					if (k%2==1)
						output = output + '"' + dest + '"';
					else 
						output = output + '"' + url + '"';
					k += 1;
				}
			}
			output += '"';
			k=1;
		}
		else {
			if (prev.find("apt ")==-1 && prev.find("pip ")==-1 && prev.find("arch ")==-1)
				output = output + "," + '"' + input + '"';
			else {
				if (!pcheck)
					output = output + pcheck_false + ppackages;
				if (!acheck)
					output = output + acheck_false + apackages;
				if (!archeck)
					output = output + archeck_false + archives + "}";
				output = output + name + '"' + input + '"';
				pcheck = false;
				acheck = false;
				archeck = false;
			}
		}
		prev = input;
		getline(cin, input);	
	}
	if (!pcheck) {
		if (prev.find("arch ")!=-1)
			output += "}";
		output = output + pcheck_false + ppackages;
	}
	if (!acheck) {
		if (prev.find("arch ")!=-1 && pcheck)
			output += "}";
		output = output + acheck_false + apackages;	
	}
	if (!archeck)
		output = output + archeck_false + archives + "}";
	output = output + ending;
	char *out = new char [output.length() +1];
	strcpy(out, output.c_str());
	
//------------------- Launching a playbook for execution ----------------//
	if ((pid=fork())==-1) {
		cerr << "Error: Failed to calling fork()" << endl;
		exit(1);	
	}    
	if (pid==0) {
		//son process
		execl("/bin/sh", "sh", "-c", out, NULL);
		cerr << "Error: Failed to calling exec()" << endl;
		exit(1);
	}  
	wait(NULL); 
	delete [] out;
	return 0;
}
