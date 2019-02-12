/*
Author: Kellan Loew
Date: 11/5/2018

Project operation: From a list of IP addresses, it prints those that belong to the same network and OS.

Directions:
- Ask the user to enter an IP address in the IP format x:y:z:w
- Ask the user to enter the length of the network part of the IP address.
- Ask the user to enter an os, or enter \?" for all operating systems.
- Print all IP addresses that belong to the same network and are assigned to hosts running the same operating
system.

*/
#include<stdio.h>
#include <string.h>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

typedef struct { //Host structure
	int x, y, z, w;
	char os[8];
}host;

host convert_IP(char s[]); //Function declarations
void print_host(host *h);
void find_hosts(host *net, int size, host t, int len);
long int decimalToEightBinary(int in);
long int power(int base, int exp);

int main(void) {
	FILE* inp;
	host myhost[20];
	int size = 0;
	int length;
	inp = fopen("hosts.txt", "r");
	char targetString[30];
	host targetIP;
	int i = 0;

	while (fscanf(inp, "%d.%d.%d.%d %s", &myhost[i].x, &myhost[i].y, &myhost[i].z, &myhost[i].w, myhost[i].os) != EOF) {//Reads each line of host file
		++i;
		++size; //Keeps track of number of IP addresses
	}

	printf("Enter target IP: ");
	scanf("%s", targetString); //Stores target IP address

	targetIP = convert_IP(targetString); //Converts target string to a binary host type for the function call later

	printf("Enter the operating system:");
	scanf("%s", targetIP.os); //Stores OS

	printf("Enter the length of the network part of the IP address:");
	scanf("%d", &length); //Stores number of significant binary digits of network part

	find_hosts(myhost, size, targetIP, length); //Calls function to find matching hosts

	fclose(inp);
	return 0;
}

host convert_IP(char s[]) {
	host temp;
	char network[3];
	long int out;
	int i, k, l;
	int counter = 0;


	for (i = 0; i < 4; ++i) { //This loop goes over each of the four parts of IP address
		k = 0;
		while ((s[counter] != '.') && (s[counter] != '\0')) { //Reads numbers from input string until a period is reached, or the end of the input string
			network[k] = s[counter];
			++k;
			++counter;
		}
		out = atoi(network);//The string is trasnformed into an integer
		switch (i) {
		case 0:
			temp.x = decimalToEightBinary(out); //Depending on which part of the IP address we're on, that part is copied into a temp host var as binary
			break;
		case 1:
			temp.y = decimalToEightBinary(out);
			break;
		case 2:
			temp.z = decimalToEightBinary(out);
			break;
		case 3:
			temp.w = decimalToEightBinary(out);
			break;
		default:
			temp.w = 666666666;
		}
		for (l = 0; l < 3; ++l) { //Deletes members of network string for next loop
			network[l] = '\0';
		}
		++counter; //Skips over period
	}
	return temp;
}

long int decimalToEightBinary(int in) { //Converts an integer less than 256 to a binary number
	int counter = 0;
	int out = 0;
	char binary[9];
	long int binaryInt;
	int k = 128; //The largest possible integer factor

	while (k >= 1) {
		if ((k + out) <= in) { //If the sum of the output + successivley smaller powers of 128 fit into the original, that power of two gets a '1'
			out = out + k;
			binary[counter] = '1';
		}
		else				//else it gets a '0'
			binary[counter] = '0';
		k = k / 2;
		++counter;
	}
	binary[8] = '\0';
	binaryInt = atoi(binary);//String is turned into an integer
	return binaryInt;
}

void print_host(host *h) {
	printf("%d.%d.%d.%d %s", h->x, h->y, h->z, h->w, h->os);
	printf("\n");
}

void find_hosts(host *net, int size, host t, int len) {
	int i, k;
	int counter = 0;
	int match = 0;
	host binary[20];

	for (i = 0; i < size; ++i) {  //This loop copies the list of input addresses to a temporary host array
		for (k = 0; k < 4; ++k) {
			switch (k) { //This part changes the first three parts of the addresses to binary
			case 0:
				binary[i].x = decimalToEightBinary(net[i].x);
				break;
			case 1:
				binary[i].y = decimalToEightBinary(net[i].y);
				break;
			case 2:
				binary[i].z = decimalToEightBinary(net[i].z);
				break;
			}
		}
	}

	for (k = 0; k < size; ++k) {//This loop checks for matches between target address and given addresses, running as many times as there possible string matches scanned in through the file
		counter = 0;
		if (len <= 8) { //If length is <= 8, 
			for (i = 8; i >= (8 - len); --i) { //Starts with 8 and decrements as long as the length is
				if ((binary[k].x / power(10, i)) == (t.x / power(10, i))) { //This divides coressponding binary numbers by succesively smaller powers of 10 and checks if they're equal
					++counter;
				}
			}
		}
		else if (len <= 16) { //If length is <= 16, then
			if (binary[k].x == t.x) //This checks if previous part is also equal
				counter = 8; //Removes the length that coressponds to previous part of IP address
			for (i = 8; i >= (8 - (len - 8)); --i) { //Starts with 8 and decrements as long as the excess length is
				if ((binary[k].y / power(10, i)) == (t.y / power(10, i))) {//As above
					++counter;
				}
			}
		}
		else { //If length is <= 24, then
			if ((binary[k].y == t.y) && (binary[k].x == t.x)) //This checks if previous parts are also equal
				counter = 16; //Removes the length that coressponds to previous parts of IP address
			for (i = 8; i >= (8 - (len - 16)); --i) {
				if ((binary[k].z / power(10, (i - 1)) == (t.z / power(10, (i - 1)))))
					++counter;
			}
		}
		if ((counter >= len) && ((strcmp(net[k].os, t.os) == 0) || (strcmp(t.os, "?") == 0))) { //Prints elements of given host if equivalent and the OS is the either the same or unknown
			print_host(&net[k]);
			++match; //Keeps track of times when no match is found
		}
	}

	if (match == 0)
		printf("No matches.\n"); //Prints message if no matches
}
long int power(int base, int exp) {
	int output = 1;
	int i;
	if (exp > 0) {
		for (i = 0; i < exp; ++i) {
			output = output * base;
		}
	}
	else  //If exponent is 0, output is always 1
		output = 1;
	return output;
}