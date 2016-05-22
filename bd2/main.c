//
//  main.c
//  bd2
//
//  Created by Marcelo Cristiano Araujo Silva on 22/05/16.
//  Copyright © 2016 marcelo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"

/* ERROR TABLE
 *
 *   0 - Everything is fine
 *  -1 - Connection error
 *  -2 - Query with data error
 *  -3 - Query without data error
 *
 */

typedef struct table{
    PGresult *query_result;
    int number_of_rows;
    int number_of_columns;
}Table;

#define NUMBER_OF_CONNECTIONS 10
#define NUMBER_OF_RESULTS 20

PGconn * create_connection();
int check_connection(PGconn *connection);
PGresult * run_query(PGconn *connection, const char *query);
int check_non_data_query(PGresult *query_result);
int check_data_query(PGresult *query_result);
Table create_table_with_query(PGresult *query_result);
void print_query_result(Table table);
void do_exit(PGconn *connections_array[NUMBER_OF_CONNECTIONS],PGresult *result_array[NUMBER_OF_RESULTS], int error_code);
void add_conection_to_array(PGconn *connection, PGconn *connections_array[NUMBER_OF_CONNECTIONS]);
void add_result_to_array(PGresult *result, PGresult *result_array[NUMBER_OF_RESULTS]);
void clear_all_results(PGresult *result_array[NUMBER_OF_RESULTS]);
void close_all_connections(PGconn *connections_array[NUMBER_OF_CONNECTIONS]);
char * add_db_names_to_array(Table *table);

int main(int argc, const char * argv[]) {
    
    /* creating a array of connections and result to close them all at end of program */
    PGconn *connections_array[NUMBER_OF_CONNECTIONS] = {NULL};
    PGresult *results_array[NUMBER_OF_RESULTS] = {NULL};
    
    /* connecting to postgres */
    PGconn *postgres_connection = create_connection("host=localhost port=5432 user=postgres password=123 connect_timeout=3");
   
    int is_postgres_connection_fine = check_connection(postgres_connection);
    if (is_postgres_connection_fine != 0) {
        do_exit(connections_array, results_array, is_postgres_connection_fine);
    }
    
    add_conection_to_array(postgres_connection, connections_array);
    
    /* getting db names */
    PGresult *db_names = run_query(postgres_connection, "SELECT datname FROM pg_database WHERE datistemplate = false;");
    int is_db_names_query_fine = check_data_query(db_names);
    if (is_db_names_query_fine != 0) {
        do_exit(connections_array, results_array, is_db_names_query_fine);
    }
    
    add_result_to_array(db_names, results_array);
    
    /* creating a table with db names */
    Table db_names_table = create_table_with_query(db_names);
    print_query_result(db_names_table);
    
    
    /* all db names in an array */
    char **array_of_db_names = (char **)malloc(db_names_table.number_of_rows * sizeof(char **));
   
    for (int i = 0; i < db_names_table.number_of_rows; i++) {
        array_of_db_names[i] = PQgetvalue(db_names, i, 0);
    }
    
    printf("\nwould you like to create the script of which database? ");
    int selected_db = 0;
    scanf("%d", &selected_db);
    
    printf("\nYou chose: %s\n", array_of_db_names[selected_db]);
    
//    for (int i = 0; i < db_names_table.number_of_rows; i++) {
//        printf("%s\n", array_of_db_names[i]);
//    }
    
//    
//    PGresult *create_db = run_query(postgres_connection, "CREATE DATABASE testDB;");
//    int is_create_db_query_fine = check_non_data_query(create_db);
//    if (is_create_db_query_fine != 0) {
//        do_exit(connections_array, results_array, is_create_db_query_fine);
//    }

    
//    /* getting table names */
//    PGresult *tables = run_query(postgres_connection, "SELECT table_schema,table_name FROM information_schema.tables WHERE table_schema = \"public\" ORDER BY table_schema,table_name;");
//    check_query(postgres_connection, tables);
//    add_result_to_array(tables, results_array);
//    
//    
//    /* creating a table with table names */
//    Table tables_table = create_table_with_query(tables);
//    print_query_result(tables_table);
//    

    clear_all_results(results_array);
    close_all_connections(connections_array);
        
    return 0;
}

/* create a connection to postgres */
PGconn * create_connection(const char *connection_string) {
    
    PGconn *connection = PQconnectdb(connection_string);
    return connection;
}

/* check if successfully connected, if dont force a disconnect */
int check_connection(PGconn *connection) {
    
    int is_connection_fine = 0;
    if (PQstatus(connection) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(connection));
        is_connection_fine = -1;
    }
    
    return is_connection_fine;
}

/* run a query and return its result */
PGresult * run_query(PGconn *connection, const char *query) {
    PGresult *query_result = PQexec(connection, query);
    
    return query_result;
}


/* check if query returned anything, if dont force a disconnect */
int check_data_query(PGresult *query_result) {
    
    int is_query_fine = 0;
    if (PQresultStatus(query_result) != PGRES_TUPLES_OK) {
        puts("We did not get any data!");
        is_query_fine = -2;
    }
    
    return is_query_fine;
}


int check_non_data_query(PGresult *query_result) {
    
    int is_query_fine = 0;
    if (PQresultStatus(query_result) != PGRES_COMMAND_OK) {
        puts("Probrem trying to execute given command !");
        is_query_fine = -3;
    }
    
    return is_query_fine;
}


/* create a table with a query result */
Table create_table_with_query(PGresult *query_result){
    
    int number_of_rows = PQntuples(query_result);
    int number_of_columns = PQnfields(query_result);
    
    Table table;
    
    table.query_result = query_result;
    table.number_of_rows = number_of_rows;
    table.number_of_columns = number_of_columns;
    
    return table;
}

void print_query_result(Table table) {

    printf("We received %d records.\n", table.number_of_rows);
    puts("==========================");
    
    for (int row = 0; row < table.number_of_rows; row++) {
        for (int column = 0; column < table.number_of_columns; column++) {
            printf("%s\t", PQgetvalue(table.query_result, row, column));
        }
        puts("");
    }
    
    puts("==========================");
}

/* Force an exit if any error was found */
void do_exit(PGconn *connections_array[NUMBER_OF_CONNECTIONS],PGresult *result_array[NUMBER_OF_RESULTS], int error_code) {

    clear_all_results(result_array);
    close_all_connections(connections_array);
    
    exit(error_code);
}

/* add a conection to an array with all conections */
void add_conection_to_array(PGconn *connection, PGconn *connections_array[NUMBER_OF_CONNECTIONS]) {
    
    int i;
    for (i = 0; connections_array[i] != NULL; i++) {}
    
    connections_array[i] = connection;
    
}

/* add a result to an array with all results */
void add_result_to_array(PGresult *result, PGresult *result_array[NUMBER_OF_RESULTS]) {
    
    int i;
    for (i = 0; result_array[i] != NULL; i++) {}
    
    result_array[i] = result;
    
}

void clear_all_results(PGresult *result_array[NUMBER_OF_RESULTS]) {
    
    int i = 0;
    
    while (result_array[i] != NULL) {
        PQclear(result_array[i]);
        i++;
    }
    
}

void close_all_connections(PGconn *connections_array[NUMBER_OF_CONNECTIONS]) {
    
    int i = 0;
    
    while (connections_array[i] != NULL) {
        PQfinish(connections_array[i]);
        i++;
    }
    
}

//char * add_db_names_to_array(Table *table) {
//    return;
//}
