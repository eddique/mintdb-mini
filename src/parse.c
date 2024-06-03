#include "parse.h"

int parse_json_document(char *json, Data *data)
{
    char *key = strstr(json, "\"id\":");
    if (key != NULL)
    {
        key += 5;
        while (*key == ' ')
            key++;
        if (*key == '\"')
            key++;
        char *id_end = strstr(key, "\"");
        if (id_end != NULL)
        {
            *id_end = '\0';
        }
        strncpy(data->id, key, strlen(key));
        data->id[strlen(key)] = '\0';
        *id_end = '\"';
    }
    else
    {
        printf("request body missing key 'id\n");
        return 1;
    }

    char *title_key = strstr(json, "\"title\":");
    if (title_key != NULL)
    {
        title_key += 8;
        while (*title_key == ' ')
            title_key++;
        if (*title_key == '\"')
            title_key++;
        char *title_end = strstr(title_key, "\"");
        if (title_end != NULL)
        {
            *title_end = '\0';
        }
        strncpy(data->title, title_key, strlen(title_key));
        data->title[strlen(title_key)] = '\0';
        *title_end = '\"';
    }
    else
    {
        printf("request body missing key 'title'\n");
        return 1;
    }

    char *url_key = strstr(json, "\"url\":");
    if (url_key != NULL)
    {
        url_key += 6;
        while (*url_key == ' ')
            url_key++;
        if (*url_key == '\"')
            url_key++;
        char *url_end = strstr(url_key, "\"");
        if (url_end != NULL)
        {
            *url_end = '\0';
        }
        strncpy(data->url, url_key, strlen(url_key));
        data->url[strlen(url_key)] = '\0';
        *url_end = '\"';
    }
    else
    {
        printf("request body missing key 'url'\n");
        return 1;
    }

    char *content_key = strstr(json, "\"content\":");
    if (content_key != NULL)
    {
        content_key += 10;
        while (*content_key == ' ')
            content_key++;
        if (*content_key == '\"')
            content_key++;
        char *content_end = strstr(content_key, "\",");
        if (content_end != NULL)
        {
            *content_end = '\0';
        }
        data->content = (char *)malloc(strlen(content_key) + 1);
        memcpy(data->content, content_key, strlen(content_key));
        data->content[strlen(content_key)] = '\0';
        *content_end = '\"';
        *(content_end + 1) = ',';
    }
    else
    {
        printf("request body missing key 'content'\n");
        return 1;
    }

    char *embedding_key = strstr(json, "\"embedding\":");
    if (embedding_key != NULL)
    {
        embedding_key += 12;
        while (*embedding_key == ' ')
            embedding_key++;
        if (*embedding_key == '[')
            embedding_key++;

        for (int i = 0; i < EMBEDDING_SIZE; i++)
        {
            char *endptr;
            double value = strtod(embedding_key, &endptr);
            if (embedding_key == endptr)
                break;
            data->embeddings[i] = value;
            embedding_key = endptr;
            if (*embedding_key == ',')
                embedding_key++;
        }
    }
    else
    {
        printf("request body missing key 'embedding'\n");
        return 1;
    }
    return 0;
}

int parse_embedding(char *json, Embedding *emb)
{
     char *embedding_key = strstr(json, "\"embedding\":");
    if (embedding_key != NULL)
    {
        embedding_key += 12;
        while (*embedding_key == ' ')
            embedding_key++;
        if (*embedding_key == '[')
            embedding_key++;

        for (int i = 0; i < EMBEDDING_SIZE; i++)
        {

            char *endptr;
            double value = strtod(embedding_key, &endptr);
            if (embedding_key == endptr)
                break;
            // printf("emb value: %f\n", value);
            emb->embeddings[i] = value;
            embedding_key = endptr;
            if (*embedding_key == ',')
                embedding_key++;
        }
    }
    else
    {
        printf("request body missing key 'embedding'\n");
        return 1;
    }
    return 0;
}