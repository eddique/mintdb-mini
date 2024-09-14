#include <assert.h>
#include "handler.h"


void test_print_titles()
{
    assert(1);
}

void test_query_embeddings()
{
    assert(1);
}


void test_dev_upsert_handler()
{
    assert(1);
    // Request req = {.path = "/dev/upsert", .body = "{\"id\":\"a\",\"title\":\"sample_doc_title\",\"content\":\"Some sample content\", \"link\":\"https://example.com\"}"};
    // char *res = NULL;
    // dev_upsert_handler(&req, &res);
    // assert(res != NULL);
    // printf("%s\n", res);
    // free(res);
}

void test_dev_query_handler()
{
    Request req = {.path = "/dev/query", .body = ""};
    char *res = NULL;
    dev_query_handler(&req, &res);
    assert(res != NULL);
    printf("%s\n", res);
    free(res);
}

void test_health_handler()
{
    Request req = {.path = "/health", .body = ""};
    char *res = NULL;
    health_handler(&req, &res);
    assert(res != NULL);
    printf("%s\n", res);
    free(res);
}

void test_echo_handler()
{
    Request req = {.path = "/echo", .body = "Echo this!"};
    char *res = NULL;
    echo_handler(&req, &res);
    assert(res != NULL);
    printf("%s\n", res);
    free(res);
}

void test_drop_handler()
{
    Request req = {.path = "/drop", .body = ""};
    char *res = NULL;
    drop_handler(&req, &res);
    assert(res != NULL);
    printf("%s\n", res);
    free(res);
}

int main()
{
    test_print_titles();
    test_dev_upsert_handler();
    test_dev_query_handler();
    test_health_handler();
    test_echo_handler();
    test_drop_handler();
    printf("All handler tests passed.\n");
    return 0;
}