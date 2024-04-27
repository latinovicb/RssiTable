//****************************MY_CODE************************************
//************************************************************************
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define MAX_NODES 40
#define MAX_VALS 10 // don't touch this!
#define RSSI_INDEX 1
#define DISTANCE_INDEX 2
#define MEDIAN_ARRAY_INDEX 9
#define MEDIAN_ARRAY_FIRST_VAL 3
#define PATH_LOSS 2.0 // path loss exponent
#define STATIC_ANCHOR 59
#define DEFAULT_VALUE 0 // nevermind

uint16_t my_arr[MAX_NODES][MAX_VALS] = {{DEFAULT_VALUE}}; 

int compare(const void *a, const void *b) {
    return (*(uint16_t *)a - *(uint16_t *)b);
}

uint16_t find_median(uint16_t arr[], int n) {
    qsort(arr, n, sizeof(uint16_t), compare);
    if (n % 2 == 0) {
        return (arr[n / 2 - 1] + arr[n / 2]) / 2;
    } else {
        return arr[n / 2];
    }
}

uint16_t calculate_distance(uint16_t rssi, uint16_t lqi, uint16_t ref_power_level, uint16_t node) {
    double rssi_d = - (double)rssi;

    double lqi_d = (double)lqi;
    double ref_power_level_d = -(double)ref_power_level;

    double distance = pow(10.0, (ref_power_level_d - rssi_d) / (10.0 * PATH_LOSS));
    printf("calculated distance in double %f",distance);
    printf(" node addr %u\n",  node);
    uint16_t result = (uint16_t) distance;

    return result;
}

// after inserting number it calculates median from the existing numbers
void insert_number(uint16_t arr[][MAX_VALS], int row, uint16_t number, uint16_t lqi) { 
    int index = arr[row][MEDIAN_ARRAY_INDEX];
    arr[row][index + MEDIAN_ARRAY_FIRST_VAL] = number;
    arr[row][MEDIAN_ARRAY_INDEX] = (index + 1) % 6;
        
    uint16_t values[6] = {arr[row][3], arr[row][4], arr[row][5], arr[row][6], arr[row][7], arr[row][8]};
    arr[row][RSSI_INDEX] = find_median(values, 6);


    int coordinator_index = -1; 
    for (int i = 0; i < MAX_NODES; i++) {
        if (arr[i][0] == 999) { // we look for coordinator to use as an anchor point
            coordinator_index = i;
            break;
        }
    }
    uint16_t coordinator_rssi = arr[coordinator_index][RSSI_INDEX];
    
    uint16_t refernce_rssi = 0;

    if ((coordinator_rssi != 0) && (arr[row][0] != 999)){  // if we have no rssi for coordinator yet - we will use static value as the reference_rssi instead
                                                         // at the same time we want coordinator to always use static rssi
        refernce_rssi = coordinator_rssi;
    } else {
        refernce_rssi = STATIC_ANCHOR;
    }

	// printf("used rssi %u\n",refernce_rssi);
    arr[row][DISTANCE_INDEX] = calculate_distance(arr[row][RSSI_INDEX], lqi, refernce_rssi, arr[row][0]);
}

int add_node(uint16_t arr[][MAX_VALS], uint16_t addr){ // looks into table - if address exists, it returns index of address, if it does not exist - it creates and entry returns an index
    int row_index = DEFAULT_VALUE;
    for (int row = 0; row < MAX_NODES; row++) { // check whole table
        for (int col = 0; col < MAX_VALS; col++) {
            if(arr[row][0] == addr){
               row_index = row; 
               return row_index;
            }
        }
    }
    if (row_index == DEFAULT_VALUE){  // iterate through table again and assign value to first zero entry
        for (int row = 0; row < MAX_NODES; row++) {
            for (int col = 0; col < MAX_VALS; col++) {
                if(arr[row][0] == DEFAULT_VALUE){
                    arr[row][0] = addr;
                    row_index = row;
                    return row_index;
                }
            }
        }
    }
    
}


void printTable(uint16_t arr[][MAX_VALS]){
    for (int row = 0; row < MAX_NODES; row++) {
        for (int col = 0; col < MAX_VALS; col++) {
            printf("%u ", arr[row][col]);
        }
        printf("\n");
    }
}

void print_row(uint16_t arr[][MAX_VALS], int row) {
    printf("Row %d: ", row);
    for (int i = 0; i < MAX_VALS; i++) {
        printf("%d ", arr[row][i]);
    }
    printf("\n");
}

//************************************************************************


int main() { 

  int8_t rssi = -60;
  int8_t rssi2 = -33;
  int8_t rssi3 = -59;
	uint16_t addr = 0;
	uint16_t addr2 = 1; 
	uint16_t addr3 = 3; 
	uint16_t lqi = 0;
    
  printf("x = %u\n",addr);
  if (addr == 0){
      addr = 999;
  }

	uint16_t rssi_new = - (uint16_t) (rssi);
	uint16_t rssi_new2 = - (uint16_t) (rssi2);
	uint16_t rssi_new3 = - (uint16_t) (rssi2);
	
	int curr_addr = add_node(my_arr, addr); // create address entry in the table or select existing
    int curr_addr2 = add_node(my_arr, addr2);
    // int curr_addr3 = add_node(my_arr, addr3);

    
    for (int i = 0; i < 5; i++){
	    insert_number(my_arr, curr_addr, rssi_new, lqi);
	    insert_number(my_arr, curr_addr2, rssi_new2, lqi);
	    // insert_number(my_arr, curr_addr3, rssi_new3, lqi);
        printTable(my_arr);
    }

    return 0;
}
