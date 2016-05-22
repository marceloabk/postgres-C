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

PGconn * create_connection();
void check_connection(PGconn *connection);
PGresult * run_query(PGconn *connection, const char *query);
void check_query(PGconn *connection, PGresult *query_result);
void do_exit(PGconn *connection);



int main(int argc, const char * argv[]) {
    
    PGconn *connection = create_connection();
    check_connection(connection);
    
    PGresult *db_names = PQexec(connection, "SELECT datname FROM pg_database WHERE datistemplate = false;");
    check_query(connection, db_names);
    
    
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
    
    PQfinish(connection);
    
    return 0;
}

/* creeate a connect with postgres */
PGconn * create_connection() {
    
    PGconn *connection = PQconnectdb("host=localhost port=5432 user=postgres password=123 connect_timeout=10");
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

/* Force an exit if any error was found */
void do_exit(PGconn *connection) {
    
    PQfinish(connection);
    exit(1);
}
