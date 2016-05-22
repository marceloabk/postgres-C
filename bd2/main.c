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

typedef struct table{
    PGresult *query_result;
    int number_of_rows;
    int number_of_columns;
}Table;

#define NUMBER_OF_CONNECTIONS 10
#define NUMBER_OF_RESULTS 20

PGconn * create_connection();
void check_connection(PGconn *connection);
PGresult * run_query(PGconn *connection, const char *query);
void check_query(PGconn *connection, PGresult *query_result);
Table create_table_with_query(PGresult *query_result);
void print_query_result(Table table);
void do_exit(PGconn *connection);
void add_conection_to_array(PGconn *connection, PGconn *connections_array[NUMBER_OF_CONNECTIONS]);
void add_result_to_array(PGresult *result, PGresult *result_array[NUMBER_OF_RESULTS]);
void clear_all_results(PGresult *result_array[NUMBER_OF_RESULTS]);
void close_all_connections(PGconn *connections_array[NUMBER_OF_CONNECTIONS]);

int main(int argc, const char * argv[]) {
    
    /* creating a array of connections and result to close them all at end of program */
    PGconn *connections_array[NUMBER_OF_CONNECTIONS] = {NULL};
    PGresult *results_array[NUMBER_OF_RESULTS] = {NULL};
    
    
    
    /* connecting to postgres */
    PGconn *postgres_connection = create_connection("host=localhost port=5432 user=postgres password=123 connect_timeout=10");
    check_connection(postgres_connection);
    add_conection_to_array(postgres_connection, connections_array);
    
    
    
    /* getting db names */
    PGresult *db_names = run_query(postgres_connection, "SELECT datname FROM pg_database WHERE datistemplate = false;");
    check_query(postgres_connection, db_names);
    add_result_to_array(db_names, results_array);
    
    /* creating a table with db names */
    Table db_names_table = create_table_with_query(db_names);
    print_query_result(db_names_table);

    
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
    
/*    PGresult *res = PQexec(connection, "SELECT * FROM movimento");
    
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        puts("We did not get any data!");
        do_exit(connection);
    }
    
    PGresult *number_of_collums = PQexec(connection, "SELECT count(*) FROM information_schema.columns WHERE table_schema = 'public' AND table_name   = 'movimento'");
    
    int rec_count = PQntuples(res);
    int colluns_count = PQnfields(res);
    
    printf("We received %d records.\n", rec_count);
    puts("==========================");
    
    for (int row=0; row<rec_count; row++) {
        for (int col=0; col<colluns_count; col++) {
            printf("%s\t", PQgetvalue(res, row, col));
         }
         puts("");
    }
    
    puts("==========================");
    
    PQclear(res);
 */
        
    return 0;
}

/* create a connection to postgres */
PGconn * create_connection(const char *connection_string) {
    
    PGconn *connection = PQconnectdb(connection_string);
    return connection;
}

/* check if successfully connected, if dont force a disconnect */
void check_connection(PGconn *connection) {
    
    if (PQstatus(connection) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(connection));
        do_exit(connection);
    }
    
}

/* run a query and return its result */
PGresult * run_query(PGconn *connection, const char *query) {
    PGresult *query_result = PQexec(connection, query);
    
    return query_result;
}


/* check if query returned anything, if dont force a disconnect */
void check_query(PGconn *connection, PGresult *query_result) {
    
    if (PQresultStatus(query_result) != PGRES_TUPLES_OK) {
        puts("We did not get any data!");
        do_exit(connection);
    }
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
void do_exit(PGconn *connection) {
    
    PQfinish(connection);
    exit(1);
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
