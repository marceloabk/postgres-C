#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

typedef struct table{
    PGresult *query_result;
    int number_of_rows;
    int number_of_columns;
}Table;

PGconn * create_connection();
int check_connection(PGconn *connection);
PGresult * run_query(PGconn *connection, const char *query);
Table create_table_with_query(PGresult *query_result);
void print_query_result(Table table);

int main(int argc, const char * argv[]) {
    
    PGconn *postgres_connection = create_connection("host=localhost port=5432 user=postgres password=123 connect_timeout=3");
    
    /* getting db names */
    PGresult *db_names = run_query(postgres_connection, "SELECT datname FROM pg_database WHERE datistemplate = false;");
    
    /* creating a table with db names */
    Table db_names_table = create_table_with_query(db_names);
    print_query_result(db_names_table);
    
    return 0;
}

/* create a connection to postgres */
PGconn * create_connection(const char *connection_string) {
    PGconn *connection = PQconnectdb(connection_string);
    
    if (!check_connection(connection)) {
        PQfinish(connection);
        exit(0);
    }
    return connection;
}

/* check if successfully connected */
int check_connection(PGconn *connection) {
    
    int is_connection_fine = 1;
    if (PQstatus(connection) == CONNECTION_BAD) {
        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(connection));
        is_connection_fine = 0;
    }
    
    return is_connection_fine;
}

/* run a query and return its result */
PGresult * run_query(PGconn *connection, const char *query) {
    PGresult *query_result = PQexec(connection, query);
    if (PQresultStatus(query_result) == PGRES_FATAL_ERROR) {
        printf("Query error!");
        PQfinish(connection);
        PQclear(query_result);
        exit(0);
    }
    return query_result;
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
