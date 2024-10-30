#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "structs.h"

#define NMAX 8 //nr. maxim de operatii
#define CMAX 100 // nr. maxim de caractere din numele fisierului

void get_operations(void **operations);
void read(sensor **array, int n, FILE *in);
void print(sensor *array, int index);
void menu_command(sensor **array, void **operations, int *n);
void analyze(sensor *array, int index, void **operations);
void sort (sensor **array, int n);
void clear(sensor **array, int *n);

int main(int argc, char const *argv[])
{
	void **operations = malloc(NMAX * sizeof(void*));

	get_operations(operations);


    int read_int;
    FILE *in = fopen(argv[1], "rb");

    fread(&read_int, sizeof(int), 1, in);
    int n = read_int; 
    sensor *array = malloc(n * sizeof(sensor));
	
	read(&array, n, in);
	sort(&array, n);
    menu_command(&array, operations, &n);

	free(operations);
    for(int i = 0; i < n; i++) {
        free(array[i].sensor_data);
        free(array[i].operations_idxs);
    }

	free(array);
	
	return 0;
}

void read(sensor **array, int n, FILE *in)
{

    int index = 0;
    float read;
    int read_int;

    while(index != n) {
        fread(&read_int, sizeof(int), 1, in);

        if(read_int == 0) {
            (*array)[index].sensor_type = TIRE;
            (*array)[index].sensor_data = malloc(sizeof(tire_sensor));
            tire_sensor *data = (*array)[index].sensor_data;

            fread(&read, sizeof(float), 1, in);

            data->pressure = read;

            fread(&read, sizeof(float), 1, in);

            data->temperature = read;

            fread(&read_int, sizeof(int), 1, in);

            data->wear_level = read_int;

            fread(&read_int, sizeof(int), 1, in);

            data->performace_score = read_int;
            
        } else if (read_int == 1) {
            (*array)[index].sensor_type = PMU;
            (*array)[index].sensor_data = malloc(sizeof(power_management_unit));
            power_management_unit *data = (*array)[index].sensor_data;

            fread(&read, sizeof(float), 1, in);

            data->voltage = read;

            fread(&read, sizeof(float), 1, in);

            data->current = read;

            fread(&read, sizeof(float), 1, in);

            data->power_consumption = read;

            fread(&read_int, sizeof(int), 1, in);

            data->energy_regen = read_int;

            fread(&read_int, sizeof(int), 1, in);

            data->energy_storage = read_int;
   
        }

        fread(&read_int, sizeof(int), 1, in);

        (*array)[index].nr_operations = read_int;
        (*array)[index].operations_idxs = malloc(sizeof(int) * read_int);
        
        for(int i = 0; i < (*array)[index].nr_operations; i++) {
            fread(&read_int, sizeof(int), 1, in);
            (*array)[index].operations_idxs[i] = read_int;
        }

        index++;
    }

    fclose(in);
}


void print(sensor *array, int index)
{
    char c = '%';

	if (array[index].sensor_type == PMU) {
        printf("Power Management Unit\n");
		power_management_unit *data = array[index].sensor_data;
		printf("Voltage: %.2f\n", data->voltage);
		printf("Current: %.2f\n", data->current);
		printf("Power Consumption: %.2f\n", data->power_consumption);
        printf("Energy Regen: %d%c\n", data->energy_regen, c);
        printf("Energy Storage: %d%c\n", data->energy_storage, c);
		

	} else if (array[index].sensor_type == TIRE) {
        printf("Tire Sensor\n");
		tire_sensor *data = array[index].sensor_data;
		printf("Pressure: %.2f\n", data->pressure);
		printf("Temperature: %.2f\n", data->temperature);
		printf("Wear Level: %d%c\n", data->wear_level, c);
        printf("Performance Score: ");
        if (data->performace_score == 0)
            printf("Not Calculated\n");
        else
            printf("%d\n", data->performace_score);
	}

}

void analyze(sensor *array, int index, void **operations)
{
	for (int i = 0; i < array[index].nr_operations; i++) {
		int x = array[index].operations_idxs[i];

		((void (*)(void*)) operations[x])(array[index].sensor_data);
	}
}

void transformare_numar(int *nr, char *sir)
{
	*nr = 0;

	for (int i = 0; i < (int)strlen(sir); i++)
		*nr = *nr * 10 + sir[i] - '0';
}

void menu_command(sensor **array, void **operations, int *n)
{
	char *comanda = malloc(CMAX * sizeof(char));
    int ok = 1;
	char *p;
    while (ok) {
        fgets(comanda, CMAX, stdin);
		p = strtok(comanda, "\n");
		if (strcmp(p, "print") == 0 && strlen(p) == 5) {
			for (int i = 0; i < *n; i++)
			print(*array, i);
			continue;
		}
	    	p = strtok(comanda, " \n");
        if (strcmp(p, "print") == 0) {
            p = strtok(NULL, "\n");
            int index;
            transformare_numar(&index, p);
            if (index >= 0 && index < *n)
                print(*array, index);
            else 
                printf("Index not in range!\n");
        } else if (strcmp(p, "analyze") == 0) {
		 	p = strtok(NULL, "\n");
        	int index;
        	transformare_numar(&index, p);
        	analyze(*array, index, operations);
        } else if (strcmp(p, "clear") == 0) {
        	clear(array, n);
        } else if (strcmp(p, "exit") == 0) {
            ok = 0;
        }

    }
    free(comanda);
}

void sort (sensor **array, int n)
{
	int t = 0, p = 0;
	sensor *aux_tire = malloc(sizeof((*array)[0]) * n);
    sensor *aux_pmu = malloc(sizeof((*array)[0]) * n);
	for (int i = 0; i < n; i++) {
		if ((*array)[i].sensor_type == TIRE) {
			aux_tire[t++] = (*array)[i];
            //printf("%d ", k);
		} else
            aux_pmu[p++] = (*array)[i];
    }

    for (int i = 0; i < p; i++)
        (*array)[i] = aux_pmu[i];
    int x = 0;
    for(int i = p; i < n; i++)
        (*array)[i] = aux_tire[x++];

    free(aux_tire);
    free(aux_pmu);
}

int pmu_check(void *data)
{
	power_management_unit *pmu = (power_management_unit *)data;
	int ok = 0;

	if (pmu->voltage < 10 || pmu->voltage > 20) {
		ok = 1;
	}

	if (pmu->current < -100 || pmu->current > 100) {
		ok = 1;
	}

	if (pmu->power_consumption < 0 || pmu->power_consumption > 1000) {
		ok = 1;
	}

	if (pmu->energy_regen < 0 || pmu->energy_regen > 100) {
		ok = 1;
	}

	if (pmu->energy_storage < 0 || pmu->energy_storage > 100) {
		ok = 1;
	}

	return ok;
}

int tire_check(void *data)
{
    int ok = 0;
    tire_sensor *tire = (tire_sensor *)data;

    if (tire->pressure < 19 || tire->pressure > 28) {
        ok = 1;
    }

    if(tire->temperature < 0 || tire->temperature> 120) {
        ok = 1;
    }

    if (tire->wear_level < 0 || tire->wear_level > 100) {
        ok = 1;
    }

    return ok;
}

void clear(sensor **array, int *n)
{
    sensor *aux = malloc(sizeof((*array)[0]) * *n);
    int k = 0;

    for (int i = 0; i < *n; i++) {
        if ((*array)[i].sensor_type == TIRE) {
            if (!tire_check((*array)[i].sensor_data)) {
                aux[k].operations_idxs = malloc(sizeof((*array)[i].operations_idxs));
                aux[k].sensor_data = malloc(sizeof((*array)[i].sensor_data));

                aux[k].nr_operations = (*array)[i].nr_operations;
                aux[k].sensor_type = (*array)[i].sensor_type;

                memcpy(aux[k].operations_idxs, (*array)[i].operations_idxs, sizeof(int) *  aux[k].nr_operations);
                memcpy(aux[k].sensor_data, (*array)[i].sensor_data, sizeof(tire_sensor));


                k++;
            }
        }
        if ((*array)[i].sensor_type == PMU) {
            if (!pmu_check((*array)[i].sensor_data)) {
               aux[k].operations_idxs = malloc(sizeof((*array)[i].operations_idxs));
                aux[k].sensor_data = malloc(sizeof((*array)[i].sensor_data));

                aux[k].nr_operations = (*array)[i].nr_operations;
                aux[k].sensor_type = (*array)[i].sensor_type;

                memcpy(aux[k].operations_idxs, (*array)[i].operations_idxs, sizeof(int) *  aux[k].nr_operations);
                memcpy(aux[k].sensor_data, (*array)[i].sensor_data, sizeof(power_management_unit));

                k++;
            }

        }
    }

    for(int i = 0; i < *n; i++) {
        free((*array)[i].sensor_data);
        free((*array)[i].operations_idxs);
    }

    free(*array);

    *array = aux;
    *n = k;
    *array = realloc(*array, *n * sizeof(sensor));

}

