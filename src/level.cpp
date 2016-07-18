#include "level.h"

namespace level {
	int number_of_enemies = 0, health_value = 0, attack_value = 0;

	int enemies() {return number_of_enemies;}
	int health() {return health_value;}
	int attack() {return attack_value;}

	int initLevel(int level) {
		number_of_enemies = getLevelFromFile(level, 1);
		health_value = getLevelFromFile(level, 2);
		attack_value = getLevelFromFile(level, 3);
		if (number_of_enemies == 0 || health_value == 0 || attack_value == 0)
			return -1;
		std::cout << "Initializing level " << level << "." << std::endl;
		std::cout << enemies() << " enemies." << std::endl;
		std::cout << health() << " points of health for each." << std::endl;
		std::cout << attack() << " points of attack power on each." << std::endl;
		return 1;
	}

	// WARNING: messy function (works though) (Update, 2016: No, it doens't)
	int getLevelFromFile (int x, int control) { // x = level, control = attribute (1 = num. of enemies, 2 = healt of enemies, 
	//																			3 = attack power of enemies)
		char c, v[32];
		int i, j, tmp, aux[10];
		std::fstream file;
		file.open ("data/levels");
		file.get(c);
		// will put the content of the desired level (x) into the array "v"
		while (c!='*' && x > 0) {
			i = 0;
			while (c != '#' && c != '*') {
				v[i++] = c;
				file.get(c);
			}
			v[i] = c;
			if (c!='*')
				file.get(c);
			x--;
		}

		if (c == '*' && x > 0)
			return -1; //means that there is no x levels

		// will get from the aux array the desired value (number, healt or attack of enemies (1, 2, 3))
		for (i = 0; i < 32 && v[i] != '#' && control > 0; i++) {
				j = 0, tmp = i;
				while (v[tmp] > 47 && v[tmp] < 58)
					aux[j++] = v[tmp++];
				aux[j] = '\0';
				if (v[tmp] == ' ') {
					i = tmp;
					control--;
				}
		}

		// convert the algarisms in the aux array into an integer
		// BEGIN
		i = 0, j = 0;
		while (aux[i] != '\0') {
			i++;
		}
		j = pow(10, i-1); i = 0; tmp = 0;
		while (aux[i] != '\0') {
			tmp += (aux[i]-'0')*j;
			j = j/10;
			i++;
		}

		return tmp;
		// END
	}
};
