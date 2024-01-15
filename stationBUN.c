#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "station.h"


/* Creeaza o gara cu un numar fix de peroane.
 * 
 * platforms_no: numarul de peroane ale garii
 *
 * return: gara creata
 */
TrainStation* open_train_station(int platforms_no) {
    TrainStation* station = (TrainStation*)malloc(sizeof(TrainStation));
    station->platforms_no = platforms_no;
    station->platforms = (Train**)malloc(platforms_no * sizeof(Train*));
    for (int i = 0; i < platforms_no; i++) {
        // station->platforms[i] = (Train*)malloc(sizeof(Train));
        station->platforms[i] = NULL;
    }
    return station;
}


/* Elibereaza memoria alocata pentru gara.
 *
 * station: gara existenta
 */
void close_train_station(TrainStation *station) {
    if (station == NULL) {
        return;
    }
    if (station->platforms != NULL) {
        for (int i = 0; i < station->platforms_no; i++) {
            leave_train(station, i);
        }
    }
    free(station->platforms);
    station->platforms = NULL;
    free(station);
    station = NULL;
}


/* Afiseaza trenurile stationate in gara.
 *
 * station: gara existenta
 * f: fisierul in care se face afisarea
 */
void show_existing_trains(TrainStation *station, FILE *f) {
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    for (int i = 0; i < station->platforms_no; i++) {
        if (station->platforms[i] != NULL) {
            fprintf(f, "%d: (%d)", i, station->platforms[i]->locomotive_power);
            TrainCar* iter = station->platforms[i]->train_cars;
            while (iter != NULL) {
                fprintf(f, "-|%d|", iter->weight);
                iter = iter->next;
            }
        } else {
            fprintf(f, "%d: ", i);
        }
        fprintf(f, "\n");
    }
}


/* Adauga o locomotiva pe un peron.
 * 
 * station: gara existenta
 * platform: peronul pe care se adauga locomotiva
 * locomotive_power: puterea de tractiune a locomotivei
 */
void arrive_train(TrainStation *station, int platform, int locomotive_power) {
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    if (platform < 0 || platform >= station->platforms_no) {
        return;
    }
    if (station->platforms[platform] != NULL) {
        return;
    }
    station->platforms[platform] = (Train*)malloc(sizeof(Train));
    station->platforms[platform]->locomotive_power = locomotive_power;
    station->platforms[platform]->train_cars = NULL;
}


/* Elibereaza un peron.
 * 
 * station: gara existenta
 * platform: peronul de pe care pleaca trenul
 */
void leave_train(TrainStation *station, int platform) {
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    if (platform < 0 || platform > station->platforms_no - 1) {
        return;
    }
    if (station->platforms[platform] != NULL) {
        TrainCar* cars = station->platforms[platform]->train_cars;
        while (cars != NULL) {
            cars = cars->next;
            free(station->platforms[platform]->train_cars);
            station->platforms[platform]->train_cars = cars;
        }
        free(station->platforms[platform]->train_cars);
        free(cars);
        station->platforms[platform]->train_cars = NULL;
    }
    free(station->platforms[platform]);
    station->platforms[platform] = NULL;
}


/* Adauga un vagon la capatul unui tren.
 * 
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 * weight: greutatea vagonului adaugat
 */
void add_train_car(TrainStation *station, int platform, int weight) {
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    if (platform < 0 || platform > station->platforms_no - 1) {
        return;
    }
    if (station->platforms[platform] == NULL) {
        return;
    }

    if (station->platforms[platform]->train_cars == NULL) {
        TrainCar* cars = (TrainCar*)malloc(sizeof(TrainCar));
        cars->weight = weight;
        cars->next = NULL;
        station->platforms[platform]->train_cars = cars;
    } else {
        TrainCar* car = (TrainCar*)malloc(sizeof(TrainCar));
        car->weight = weight;
        car->next = NULL;
        TrainCar* ptr = station->platforms[platform]->train_cars;
        while (ptr->next != NULL) {
            ptr = ptr->next;
        }
        ptr->next = car;
    }
}


/* Scoate vagoanele de o anumita greutate dintr-un tren.
 * 
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 * weight: greutatea vagonului scos
 */
void remove_train_cars(TrainStation *station, int platform, int weight) {
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    if (platform < 0 || platform > station->platforms_no - 1) {
        return;
    }
    if (station->platforms[platform] == NULL) {
        return;
    }
    int ok = 0;
    TrainCar* iter = station->platforms[platform]->train_cars;
    while (iter != NULL) {
        if (iter->weight == weight) {
            ok = 1;
            break;
        }
        iter = iter->next;
    }
    while (ok == 1) {
        TrainCar* curr = station->platforms[platform]->train_cars;
        TrainCar* prev = NULL;
        if (station->platforms[platform]->train_cars->weight == weight) {
            station->platforms[platform]->train_cars = station->platforms[platform]->train_cars->next;
            free(curr);
        } else {
            while (curr != NULL && curr->weight != weight) {
                prev = curr;
                curr = curr->next;
            }
            prev->next = curr->next;
            free(curr);
        }

        ok = 0;
        iter = station->platforms[platform]->train_cars;
        while (iter != NULL) {
            if (iter->weight == weight) {
                ok = 1;
                break;
            }
            iter = iter->next;
        }
    }
}


/* Muta o secventa de vagoane dintr-un tren in altul.
 * 
 * station: gara existenta
 * platform_a: peronul pe care se afla trenul de unde se scot vagoanele
 * pos_a: pozitia primului vagon din secventa
 * cars_no: lungimea secventei
 * platform_b: peronul pe care se afla trenul unde se adauga vagoanele
 * pos_b: pozitia unde se adauga secventa de vagoane
 */
void move_train_cars(TrainStation *station, int platform_a, int pos_a, 
                                int cars_no, int platform_b, int pos_b) {
    if (cars_no == 0) {
        return;
    }
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    if (station->platforms[platform_a] == NULL) {
        return;
    }
    int len_a = 0, len_b = 0;
    TrainCar* a = station->platforms[platform_a]->train_cars;
    TrainCar* b = station->platforms[platform_b]->train_cars;
    if (a == NULL) {
        return;
    }
    TrainCar* iter = a;
    while (iter != NULL) {
        len_a++;
        iter = iter->next;
    }
    iter = b;
    while (iter != NULL) {
        len_b++;
        iter = iter->next;
    }
    if (pos_a < 1 || pos_a > len_a || pos_a + cars_no - 1 > len_a) {
        return;
    }
    if (pos_b < 1) {
        return;
    }
    if (b != NULL && pos_b > len_b + 1) {
        return;
    }
    if (b == NULL && pos_b > 1) {
        return;
    }
    TrainCar* prev_a = a;
    TrainCar* prev_b = b;
    for (int i = 1; i < pos_a; i++) {
        prev_a = a;
        a = a->next;
    }
    for (int i = 1; i < pos_b; i++) {
        prev_b = b;
        b = b->next;
    }
    TrainCar* cars = a;
    iter = cars;
    for (int i = 1; i < cars_no; i++) {
        iter = iter->next;
    }
    if (pos_a == 1 && pos_b == 1 && b != NULL) {
        station->platforms[platform_a]->train_cars = iter->next;
        iter->next = b;
        station->platforms[platform_b]->train_cars = cars;
    } else if (pos_a == 1 && pos_b == 1 && b == NULL) {
        station->platforms[platform_a]->train_cars = iter->next;
        iter->next = NULL;
        station->platforms[platform_b]->train_cars = cars;
    } else if (pos_a == 1 && pos_b != 1) {
        station->platforms[platform_a]->train_cars = iter->next;
        prev_b->next = cars;
        iter->next = b;
    } else if (pos_a != 1 && pos_b == 1) {
        prev_a->next = iter->next;
        iter->next = b;
        station->platforms[platform_b]->train_cars = cars;
    } else {
        prev_a->next = iter->next;
        prev_b->next = cars;
        iter->next = b;
    }
    
}


/* Gaseste trenul cel mai rapid.
 * 
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_express_train(TrainStation *station) {
    int max = 0, platform = 0;
    for (int i = 0; i < station->platforms_no; i++) {
        if (station->platforms[i] == NULL) {
            continue;
        }
        int sum = 0, dif = 0;
        if (station->platforms[i]->train_cars != NULL) {
            TrainCar* cars = station->platforms[i]->train_cars;
            while (cars != NULL) {
                sum = sum + cars->weight;
                cars = cars->next;
            }
            cars = NULL;
        }
        dif = station->platforms[i]->locomotive_power - sum;
        if (dif >= max ) {
            max = dif;
            platform = i;
        }
    }
    return platform;
}


/* Gaseste trenul supraincarcat.
 * 
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_overload_train(TrainStation *station) {
    int platform = 0;
    for (int i = 0; i < station->platforms_no; i++) {
        if (station->platforms[i] == NULL || station->platforms[i]->train_cars == NULL) {
            continue;
        }
        int sum = 0;
        TrainCar* cars = station->platforms[i]->train_cars;
        while (cars != NULL) {
            sum = sum + cars->weight;
            cars = cars->next;
        }
        cars = NULL;
        if (station->platforms[i]->locomotive_power < sum) {
            return i;
        }
    }
    return -1;
}


/* Gaseste trenul cu incarcatura optima.
 * 
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_optimal_train(TrainStation *station) {
    int min = 101, platform = -1;
    for (int i = 0; i < station->platforms_no; i++) {
        if (station->platforms[i] == NULL || station->platforms[i]->train_cars == NULL) {
            continue;
        }
        int sum = 0, dif = 0;
        TrainCar* cars = station->platforms[i]->train_cars;
        while (cars != NULL) {
            sum = sum + cars->weight;
            cars = cars->next;
        }
        cars = NULL;
        dif = station->platforms[i]->locomotive_power - sum;
        if (dif <= min) {
            min = dif;
            platform = i;
        }
    }
    return platform;
}


/* Gaseste trenul cu incarcatura nedistribuita bine.
 * 
 * station: gara existenta
 * cars_no: lungimea secventei de vagoane
 *
 * return: peronul pe care se afla trenul
 */
int find_heaviest_sequence_train(TrainStation *station, int cars_no, TrainCar **start_car) {
    int platform = -1, summax = 0;
    TrainCar* result = NULL;
    if (station == NULL || station->platforms == NULL) {
        return platform;
    }
    if (cars_no == 0) {
        return platform;
    }
    for (int i = 0; i < station->platforms_no; i++) {
        if (station->platforms[i] == NULL || station->platforms[i]->train_cars == NULL) {
            continue;
        }
        int len = 0, pos = 1;
        TrainCar* iter = station->platforms[i]->train_cars;
        while (iter != NULL) {
            len++;
            iter = iter->next;
        }
        iter = station->platforms[i]->train_cars;
        while (pos + cars_no - 1 <= len) {
            TrainCar* temp = iter;
            int sum = 0, cpy = cars_no;
            while (cpy > 0) {
                sum = sum + temp->weight;
                temp = temp->next;
                cpy--;
            }
            if (sum > summax) {
                summax = sum;
                result = iter;
                platform = i;
            }
            iter = iter->next;
            pos++;
        }
    }
    *start_car = *(&result);
    return platform;
}


/* Ordoneaza vagoanele dintr-un tren in ordinea descrescatoare a greutatilor.
 * 
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 */
void order_train(TrainStation *station, int platform) {
    if (station == NULL || station->platforms == NULL || station->platforms[platform] == NULL) {
        return;
    }
    if (station->platforms[platform]->train_cars == NULL) {
        return;
    } 
    int temp = 0;
    for (TrainCar* i = station->platforms[platform]->train_cars; i->next != NULL; i = i->next) {
        for (TrainCar* j = i->next; j != NULL; j = j->next) {
            if (i->weight < j->weight) {
                temp = i->weight;
                i->weight = j->weight;
                j->weight = temp;
            }
        }
    }
}


/* Scoate un vagon din trenul supraincarcat.
 * 
 * station: gara existenta
 */
void fix_overload_train(TrainStation *station) {
    if (station == NULL || station->platforms == NULL) {
        return;
    }
    int platform = find_overload_train(station);
    if (platform == -1) {
        return;
    }
    int min = 101, pos = 1, i = 1, power = station->platforms[platform]->locomotive_power;
    TrainCar* iter = station->platforms[platform]->train_cars;
    while (iter != NULL) {
        TrainCar* temp = station->platforms[platform]->train_cars;
        int j = 1, sum = 0;
        while (temp != NULL) {
            if (j == i) {
                temp = temp->next;
                j++;
            } else {
                sum = sum + temp->weight;
                j++;
                temp = temp->next;
            }
        }
        int dif = power - sum;
        if (dif >= 0 && dif < min) {
            min = dif;
            pos = i;
        }
        i++;
        iter = iter->next;
    }
    iter = station->platforms[platform]->train_cars;
    TrainCar* prev = station->platforms[platform]->train_cars;
    if (pos == 1) {
        station->platforms[platform]->train_cars = iter->next;
        free(iter);
        iter = NULL;
    } else {
        while (pos != 1) {
            prev = iter;
            iter = iter->next;
            pos--;
        }
        prev->next = iter->next;
        free(iter);
        iter = NULL;
    }
}
