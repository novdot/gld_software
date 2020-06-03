/**
  ******************************************************************************
  * @file    utility.c
  *
  * @brief   Общие вспомогательные алгоритмы преобразований.
  *
  * @author  Дмитрий Новиков novikov@elektrooptika.ru
  *
  ******************************************************************************
  * @attention
  *
  * в разработке
  *
  * <h2><center>&copy; 2020 Электрооптика</center></h2>
  ******************************************************************************
  */
#include "utility.h"

x_uint32_t utility_convert_str2time(const x_uint8_t *str) 
{
    /*char mon[4];
    int day, mon_int, year;

    mon[0] = str[0];
	mon[1] = str[1];
	mon[2] = str[2];
	mon[3] = '\0';

	int parser_pos = 3;

	while (str[parser_pos] == ' ') {
		parser_pos++;
	}

	day = 0;
	while (str[parser_pos] != ' ') {
		day = day * 10 + str[parser_pos] - 48;
		parser_pos++;
	}

	while (str[parser_pos] == ' ') {
		parser_pos++;
	}

	year = 0;
	while (str[parser_pos] != '\0') {
		year = year * 10 + str[parser_pos] - 48;
		parser_pos++;
	}

    mon_int = 0;
    switch (mon[0]) {
        case 'J':
            switch (mon[1]) {
                case 'a':
                    mon_int = 1;
                    break;
                case 'u':
                    switch (mon[2]) {
                        case 'n':
                            mon_int = 6;
                            break;
                        case 'l':
                            mon_int = 7;
                            break;
                    }
            }
            break;
        case 'F':
            mon_int = 2;
            break;
        case 'M':
            switch (mon[2]) {
                case 'r':
                    mon_int = 3;
                    break;
                case 'y':
                    mon_int = 5;
                    break;
            }
            break;
        case 'A':
            switch (mon[1]) {
                case 'p':
                    mon_int = 4;
                    break;
                case 'u':
                    mon_int = 8;
                    break;
            }
            break;
        case 'S':
            mon_int = 9;
            break;
        case 'O':
            mon_int = 10;
            break;
        case 'N':
            mon_int = 11;
            break;
        case 'D':
            mon_int = 12;
            break;
        default:
            mon_int = 0;
            break;
    }

	x_uint32_t time_in_days = day;

	for (int i = 1970; i <= year; ++i) {
		int months = (i == year) ? mon_int : 13;
		for (int j = 1; j < months; ++j) {
			switch (j) {
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					time_in_days += 31;
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					time_in_days += 30;
					break;
				case 2:
					if (((i % 4 == 0) && (i % 100 != 0)) || (i % 400 == 0)) {
						time_in_days += 29;
					} else {
						time_in_days += 28;
					}
					break;
				default:
					break;
			}
		}
	}

    return time_in_days;*/
}
/******************************************************************************/