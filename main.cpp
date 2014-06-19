#include <stdio.h>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

#define EMPTY_FIELD 1
#define BOTTOM_LINE 2

using namespace std;

extern const string XOR = "@";
extern const string OR = " ";
extern const string NOT = "-";

int andCtr=0;

string AND() {
	andCtr++;
	return " 0\n";
}

/*
 * vector<int> encode/decode global
 * vector->at(i) mehrmals benutzt -> in int zwischenspeichern
 * int tmp außerhalb von for schleifen deklarieren
 * (x, y) anstatt wert
 *
 * encode funktion sodass keine unnützen variablen verwendet werden
 *
 * */

int size;

struct Field {
	int x;
	int y;
};

vector<int> values;

/**
 * returns the number of the field that is "steps" steps in the left direction of "from"
 * if there is no such field in this direction simply 0 will be returned
 *
 * performance hint: if ever the left top (which means a combination of two funcitions is needed)
 * try to store the result of left (it's slow in computing) and use top twice (it's fast :D )
 */
int left(int from, int steps) {
	if (from == 0) {
		return from;
	}
	int mod = from % size;

	//prevents from "overflow, which means that you can substract 3 from 7, get 4,
	//and 4 would be left of 4, but not in the same row
	if (mod > size) {
		return 0;
	}
	int left = from - steps;
	if (left > from - mod) {
		return left;
	} else if (mod == 0) { //special case for the last entries in each row
		int d = from / steps;
		if (left > from - d * size) {
			return left;
		}
	}
	return 0;
}

int right(int from, int steps) {
	if (from == 0) {
		return from;
	}
	int right = from + steps, d = from / size;

	if (from % size == 0) {
		return 0;
	} else if (right <= (d + 1) * size) {
		return right;
	}
	return 0;
}

int top(int from, int steps) {
	if (from == 0) {
		return from;
	}
	int top = from - steps * size;
	if (top > 0) {
		return top;
	}
	return 0;
}

int bottom(int from, int steps) {
	if (from == 0) {
		return from;
	}
	int bottom = from + steps * size;
	if (bottom <= size * size) {
		return bottom;
	}
	return 0;
}

int getXPos(int a) {
	return ((a - 1) % size) + 1;
}

int getYPos(int a) {
	return floor((a - 1) / size) + 1;
}

//a, b sind die nummern der felder/indizes
int dist(int a, int b) {
	int xa = getXPos(a);
	int ya = getYPos(a);
	int xb = getXPos(b);
	int yb = getYPos(b);

	int xdif, ydif;
	if (xa > xb) {
		xdif = xa - xb;
	} else
		xdif = xb - xa;
	if (ya > yb) {
		ydif = ya - yb;
	} else
		ydif = yb - ya;

	if (xdif > ydif)
		return xdif;
	else
		return ydif;
}

//a=6, b=1
//ceil((6-1)-dist(a,b))
//---------------------
//       2
int max(int a, int b) {
	int distance = dist(a, b);
	cout << "dist: " << distance << endl;
	int valueA = values.at(a-1);
	int valueB = values.at(b-1);

	cout << "valA: " << valueA <<endl;
	cout << "valB: " << valueB <<endl;

	if (valueA > valueB) {
		return ceil((float) ((valueA - valueB) - distance) / 2);
	} else {
		return ceil((float) ((valueB - valueA) - distance) / 2);
	}
}



int convertXYToIndex(int y, int x) {
	return size*y-(size*x);
}

// order = 0: a tl, b br
// order = 1: a bl, b tr
void getRectangle(int a, int b, vector<int>*possibleValues, int order) {
	int xa = getXPos(a);
	int ya = getYPos(a);
	int xb = getXPos(b);
	int yb = getYPos(b);

	int index;
	if(order == 0) {
		for(int i=xa; i<xb; i++) {
			cout << "(" << i;
			for(int j=ya; j<yb; j++) {
				cout << ", " << j << ")";
				index = convertXYToIndex(i, j);
				if(values.at(index-1) != 0) {
					possibleValues->push_back(index);
				}
			}
		}
	} else if (order == 1) {
		for(int i=xa; i<xb; i++) {
			for(int j=yb; j<ya; j++) {
				index = convertXYToIndex(i, j);
				if(values.at(index-1) != 0) {
					possibleValues->push_back(index);
				}

			}
		}
	}

}

void getMaxRange(int a, int b, vector<int>* possibleValues) {
	int xa = getXPos(a);
	int ya = getYPos(a);
	int xb = getXPos(b);
	int yb = getYPos(b);
	int m = max(a, b);
	cout << a << ":(" << xa << ", " << ya << ") bis ";
	cout << b << ":(" << xb << ", " << yb << ") \n";
	cout << "max: " << m<< endl;


	int cornerA;
	int cornerB;
	//a links
	if(xa <= xb) {
		//a links oben
		if(ya <= yb) {
			int l= left(a,m);
			if(l== 0) {
				l= a;
			}
			cout <<"lo";
			cornerA = top(l, m);
			if(cornerA == 0) {
				cornerA = a;
			}
			cout << "ecke a " << cornerA << endl;


			int r= right(b,m);
			if(r == 0) {
				r = b;
			}
			cout << "rechts: " << r << endl;
			cornerB = bottom(r, m);
			cout << "recunt: " << cornerB << endl;
			if(cornerB == 0) {
				cornerB = b;
			}
			cout << " ecke b " << cornerB << endl;
			getRectangle(cornerA, cornerB, possibleValues, 0);
		} else { //a links unten
			cornerA = bottom(left(a, m), m);
			if(cornerA == 0) {
				cornerA = a;
			}
			cornerB = top(right(b, m), m);
			if(cornerB == 0) {
				cornerB = b;
			}
			getRectangle(cornerA, cornerB, possibleValues, 1);
		}
	} else {//a rechts
		if(ya<=yb) { //a rechts oben
			cornerA = top(right(a, m), m);
			if(cornerA == 0) {
				cornerA = a;
			}
			cornerB = bottom(left(b, m), m);
			if(cornerB == 0) {
				cornerB = b;
			}
			getRectangle(cornerB, cornerA, possibleValues, 1);
		} else { // a rechtes unten
			cornerA = bottom(right(a, m), m);
			if(cornerA == 0) {
				cornerA = a;
			}
			cornerB = top(left(b, m), m);
			if(cornerB == 0) {
				cornerB = b;
			}
			getRectangle(cornerB, cornerA, possibleValues, 0);
		}

	}
}


Field* encoding;
int* encodingReverse;
int encodingCount = 1;

int encode(int index, int value) {
	//if encoding contains already (index, value) return it
	int tmp = encodingReverse[index * size * size + value];
	if (tmp != 0) {
		return tmp;
	}
	encodingReverse[index * size * size + value] = encodingCount;

	Field f = { index, value };
	encoding[encodingCount] = f;
//	cout << "(" << index << ", " << value << ") \t\t->" << encodingCount << endl;
	encodingCount++;

	return encodingCount - 1;
}

Field decode(int number) {
//	cout <<number << "\t\t-> (" << encoding[number].x << ", " << encoding[number].y << ")" << endl;
	return encoding[number];
}

//the vector need to have the first element on position with index 1 -> write 0 in index 0
void fillData(vector<int> * possibleValues, vector<int>* emptyFields) {
	//fill all Values in possible Values field
	for (int j = 1; j <= size * size; j++) {
		possibleValues->push_back(j);
	}
	//erasecounter necessary because of erasing elements is changing the index
	int erasecounter = 1;
	for (int i = 1; i < size * size + 1; i++) {
		int value = values.at(i - 1);
		//if field is empty add it to emptyField
		if (value == 0) {
			emptyFields->push_back(i);
		}
		//if not empty create a Point and save it to values and remove it from possible values
		else {
			possibleValues->erase(
			    remove(possibleValues->begin(), possibleValues->end(),
			           value), possibleValues->end());
			erasecounter++;
		}
	}
}

void parseHidoku(char* path) {
	ifstream myfile(path);
	char c;
	string number;
	int flags = 0;
	int lineIndex = 0;
	int linesize = 0;
	int count;

	// read out size
	myfile.ignore(17);
	myfile >> size;
	linesize = (3 + ceil(log10(size * size + 0.5))) * size + 2;
	//cout << "size: " << size << "linesize: " << linesize << endl;
	// ignore heading
	myfile.ignore(1); // ignore linebreak
	myfile.ignore(linesize * 2); // ignore next two lines
	while (!myfile.eof()) {

		if ((flags & EMPTY_FIELD) == 0) {
			myfile.get(c);
		}

		if (c == '|') {
			myfile >> number;
			switch (number[0]) {
			case '|':
				values.push_back(0);
				flags |= EMPTY_FIELD;
				lineIndex++;
				count++;
				//cout << " EMPTY ";
				break;
			default:
				flags = 0;
				values.push_back(atoi(number.c_str()));
				int ignores = ceil(log10(size * size + 0.5));
				ignores -= ceil(log10(atoi(number.c_str()) + 0.5));
//				cout << ignores;
				myfile.ignore(ignores);
				lineIndex++;
				count++;
				//	cout << " " << number << " ";
				break;
			}
			if (lineIndex == size) {
				myfile.ignore(linesize * 3 + 1);
				if ((flags & EMPTY_FIELD) == 0) {
					myfile.ignore(2);
				}
				lineIndex = 0;
				flags = 0;
			}
		}

	}
	myfile.close();
}

void drawLine(int linesize, char c) {
	for (int i = 0; i < linesize; i++) {
		cout.put('-');
	}
	cout << endl;
}

/**
 * This function should write the head (or the footer) of a cell of the Hidoku-Board.
 *
 * @param numbersize Count of characters for a Number in the Hidoku
 * @param size Number of cells in a row of the Hidoku
 */
void drawCellBody(int numbersize, int cellCount) {
	for (int i = 0; i < cellCount; i++) {
		cout.put('|');
		for (int j = 0; j < (numbersize + 2); j++) {
			cout.put(' ');
		}
	}
	cout << '|' << endl;
}

void drawNumbers(int numsize, int size, int values[], int linenumber) {
	for (int i = 0; i < size; i++) {
		cout << "| ";
		int number = values[i + (linenumber * size)];
		if (number != 0) {
			cout << number;
			int numberLength = ceil(log10(number + 0.5));
			while (numberLength < numsize) {
				cout.put(' ');
				numberLength++;
			}
		} else {
			for (int j = 0; j < (numsize); j++) {
				cout.put(' ');
			}
		}
		cout << " ";
		//		cout << values->at(i);
	}
	cout << '|' << endl;
}

void drawBoard(int values[], int size) {
	int linesize = (3 + ceil(log10(size * size + 0.5))) * size + 1;
	cout << "hidoku with size " << size << endl;
	drawLine(linesize, '-');
	for (int i = 0; i < size; i++) {
		drawCellBody(ceil(log10(size * size + 0.5)), size);
		drawNumbers(ceil(log10(size * size + 0.5)), size, values, i);
		drawCellBody(ceil(log10(size * size + 0.5)), size);
		drawLine(linesize, '-');
	}
}

int getNumberFromCode(istringstream* code) {
	int result;
	char c;
	string number = "";
	code->get();
	do {
		code->get(c);
		cout << c;
		if (c != ' ') {
			number += c;
		}
	} while (c != ' ');
	result = atoi(number.c_str());
	return result;
}

int parseSolution(string solution, int values[]) {
	ifstream solFile(solution);
	string line;
	string number;
	if (solFile.is_open()) {
		getline(solFile, line);
		if (line == "UNSAT") {
			return 0;
		}
		bool skip = 0;
		while (getline(solFile, line)) {
			for (size_t i = 0; i < line.size(); ++i) {
				char c = line[i];

				//new number read
				if (c == ' ') {
					skip = 0;
					if (!number.empty()) {
						Field cell = decode(atoi(number.c_str()));
						values[(cell.x - 1)] = cell.y;
						number.clear();
					}
				} else //negative number, skip it!
					if (c == '-') {
						skip = 1;
						i++;
					} else {
						if (skip == 0) {
							number += c;
						}
					}
			}
		}
		solFile.close();
	}
}

/*

 void deleteNumberFromStream(istringstream* code) {
 code->get();
 char c;
 do {
 code->get(c);
 //cout << c << ", ";
 } while(c != ' ');
 }

 void parseSolution(string solution, int values[]) {
 istringstream sol(solution);
 sol.ignore(4);
 int number = 1;
 char sign;

 for(int i=1; i< size*size*size*size + size ; i++) {
 sol.get(sign);
 //cout << sign << ", ";
 if (sign != '-') {
 Field cell = decode(number);
 cout << number << ", ";
 values[(cell.x-1)] = cell.y;
 deleteNumberFromStream(&sol);
 } else {
 cout << "-" << number << ", ";
 deleteNumberFromStream(&sol);
 }
 number++;
 }
 }
 */

string getExactlyOne(vector<int>* vars) {
	string result = to_string(vars->at(0));
	for (int i = 1; i < vars->size(); i++) {
		result += OR + to_string(vars->at(i));
	}

	for (int i = 0; i < vars->size(); i++) {
		for (int j = i + 1; j < vars->size(); j++) {
			result += AND() + NOT + to_string(vars->at(i)) + OR + NOT
			          + to_string(vars->at(j));
		}
	}
	return result + AND();
}

string computeClauses(vector<int>* possibleValues, vector<int>*emptyFields) {
	int n = size * size;

	string result;

	for (int i = 1; i <= n; i++) {
		if (values.at(i - 1) != 0) {
			result += to_string(encode(i, values.at(i - 1))) + AND();
		}
	}
	//result = "schritt 1: jede zahl kommt im Spielfeld genau einmal vor";
	vector<int>* vars = new vector<int>();

	//nach sortieren wird diese for schleife nicht von k=0 bis possibleValues->size() gehen, sondern in einzelnen
	//schritten von a bis b, und so weiter
	for (int k = 0; k < possibleValues->size(); k++) { //toggle of ¬
		for (int i = 0; i < emptyFields->size(); i++) { //value
			vars->push_back(encode(emptyFields->at(i), possibleValues->at(k)));
		}
		result += getExactlyOne(vars);
		vars->clear();
	}

	result += "\n";

//	result = result + "schritt 2: Jedes Feld hat einen Nachbarn mit einer kleineren Zahl:" + "\n";
	for (int i = 1; i <= n; i++) { //i represents the field index
ende: /**
		 *0-top
		 *1-left
		 *2-right
		 *3-bottom
		 *4-tl
		 *5-tr
		 *6-bl
		 *7-br
		 *
		 */
		int neighbours[8] = { top(i, 1), left(i, 1), right(i, 1), bottom(i, 1) };

		neighbours[4] = top(neighbours[1], 1);
		neighbours[5] = top(neighbours[2], 1);
		neighbours[6] = bottom(neighbours[1], 1);
		neighbours[7] = bottom(neighbours[2], 1);

		if (values.at(i - 1) == 0) {

			for (int j = 0; j < possibleValues->size(); j++) { // j stands for the value of the field i
hinfort:
				if (possibleValues->at(j) < 2
				        || find(possibleValues->begin(), possibleValues->end(),
				                possibleValues->at(j) - 1)
				        == possibleValues->end()) {
					continue;
				}

				//result = result + "( " + NOT + "("  + to_string( i ) +  ", "  + to_string( j ) +  ") ";

				for (int k = 0; k < 8; k++) {
					//	if(neighbours[k] != 0)
					//	result += to_string(possibleValues->at(j)) + " ?= " + to_string(values.at(neighbours[k]-1)+1);
					if (neighbours[k] != 0
					        && ((possibleValues->at(j)
					             == values.at(neighbours[k] - 1) + 1))) {
						//result += "weg";
						/* j++;
						 if(j>= possibleValues->size()) {
						 i++;
						 goto ende;
						 }
						 goto hinfort;*/
					}
				}

				result += NOT + to_string(encode(i, possibleValues->at(j)));
				//	result += "(" + to_string(i) + ", " + to_string(possibleValues->at(j)) + ")";

				int nk;

				for (int k = 0; k < 8; k++) {
					nk = neighbours[k];
					if (find(emptyFields->begin(), emptyFields->end(), nk)
					        != emptyFields->end()) {
						result += OR
						          + to_string(
						              encode(nk, possibleValues->at(j) - 1));
						//	result += "(" + to_string(nk) + ", " + to_string(possibleValues->at(j) -1) + ")";

					}

				}
				result += AND();
			}
		} else {
			if (values.at(i - 1) > 1
			        && find(possibleValues->begin(), possibleValues->end(),
			                values.at(i - 1) - 1) != possibleValues->end()) {

				int tmp = 0, nk;
				for (int k = 0; k < 8; k++) {
					nk = neighbours[k];
					if (nk != 0
					        && find(emptyFields->begin(), emptyFields->end(),
					                nk) != emptyFields->end()) {
						if (tmp == 1) {
							result += OR;
						}
						result += to_string(encode(nk, values.at(i - 1) - 1));
						//	result += "(" + to_string(nk) + ", " + to_string(values.at(i-1)-1) + ") ";
						tmp = 1;
					}

				}
				result += AND();
			}

			if (values.at(i - 1) < n
			        && find(possibleValues->begin(), possibleValues->end(),
			                values.at(i - 1) + 1) != possibleValues->end()) {

				int tmp = 0, nk;
				for (int k = 0; k < 8; k++) {
					nk = neighbours[k];
					if (nk != 0
					        && find(emptyFields->begin(), emptyFields->end(),
					                nk) != emptyFields->end()) {
						if (tmp == 1) {
							result += OR;
						}
						result += to_string(encode(nk, values.at(i - 1) + 1));
						//result += "(" + to_string(nk) + ", " + to_string(values.at(i-1)+1) + ") ";
						tmp = 1;
					}

				}
				result += AND() + "\n";
			}
		}

	}

//result = result + "\n" + "schritt 3: Nur 1 Zahl pro Feld" + "\n";

	for (int i = 0; i < emptyFields->size(); i++) {
		for (int k = 0; k < possibleValues->size(); k++) {
			vars->push_back(encode(emptyFields->at(i), possibleValues->at(k)));
		}
		result += getExactlyOne(vars);
		vars->clear();

	}

	return result;

}

int main(int argc, char* argv[]) {
	parseHidoku(argv[1]);

	vector<int>* range = new vector<int>();
	getMaxRange(1, 3, range);

	for(int i=0; i<range->size(); i++) {
		cout << range->at(i) << ", ";
	}




	vector<int>* possibleValues = new vector<int>();
	vector<int>* emptyFields = new vector<int>();
	encoding = new Field[size * size * size * size]();
	encodingReverse = new int[size * size * size * size * size]();

	fillData(possibleValues, emptyFields);

	ofstream outfile;
	outfile.open("out.txt");

	outfile << "p cnf " << encodingCount - 1 << " " << andCtr << " \n"
	        << computeClauses(possibleValues, emptyFields);
	outfile.close();

	int n = size * size;
	int newValues[n];
	system("minisat out.txt result.txt > stat.txt");

	int res = parseSolution("result.txt", newValues);

	delete[] encoding;
	delete[] encodingReverse;

	if (res == 0) {
		cout << "unsat";
		return 20;
	}

	drawBoard(newValues, size);
	return 10;
}
