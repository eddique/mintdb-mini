<p align="center">
    <img width="400" src="./docs/img/logo-light.png#gh-light-mode-only" alt="mintDB Logo">
    <img width="400" src="./docs/img/logo.png#gh-dark-mode-only" alt="mintDB Logo">
</p>
<h2 align="center">An Open Source Vector Database</h2>
<p align="center">
    <img src="https://img.shields.io/badge/version-0.1.0-10d99d">
    <img src="https://img.shields.io/docker/pulls/eddique/mintdb-stack">
    <img src="https://img.shields.io/badge/built_with-Rust-dca282.svg">
    <img src="https://img.shields.io/badge/license-MIT-critical">
    <a href="https://www.linkedin.com/in/eric-rodriguez-3a402811b/"><img src="https://img.shields.io/badge/linkedIn-connect-4777AF"></a>
</p>

# Quick Start

```sh
make
./bin/mint -p 8080 data
```

```sh
# with argo cli
argo run -p 8080 data
```

# Usage

```sh
mint -p PORT_NUMBER DATA_PATH
```

# API

## Endpoints

### 1. Upsert Document

**Endpoint**: `/upsert`

**Method**: `POST`

**Description**: Upserts a document into the specified table. If no table is specified, the default table is used.

**Request Body**:

-   JSON document containing the data to be upserted.

**Response**:

-   **200 OK**: If the document is successfully upserted.
-   **500 Internal Server Error**: If there is an error during upsertion.

**Example**:
{
"title": "Sample Title",
"content": "Sample content",
"embeddings": [0.1, 0.2, 0.3, ...]
}

### 2. Query Documents

**Endpoint**: `/query`

**Method**: `POST`

**Description**: Queries documents from the specified table based on the provided embeddings. If no table is specified, the default table is used.

**Request Body**:

-   JSON document containing the embeddings to query.

**Response**:

-   **200 OK**: Returns a JSON array of matching documents.

**Example**:
{
"embeddings": [0.1, 0.2, 0.3, ...]
}

### 3. Print Titles

**Endpoint**: `/print`

**Method**: `GET`

**Description**: Prints the titles of documents in the cache.

**Response**:

-   **204 No Content**: If titles are printed successfully.

### 4. Health Check

**Endpoint**: `/health`

**Method**: `GET`

**Description**: Checks the health status of the server.

**Response**:

-   **200 OK**: Returns a JSON object indicating the server is healthy.

**Example**:
{
"ok": true,
"message": "all good 😎"
}

### 5. Echo

**Endpoint**: `/echo`

**Method**: `POST`

**Description**: Echoes back the request body.

**Request Body**:

-   Any text content.

**Response**:

-   **200 OK**: Returns the same text content.

### 6. Drop Table

**Endpoint**: `/drop`

**Method**: `DELETE`

**Description**: Drops the specified table. If no table is specified, the default table is dropped.

**Response**:

-   **204 No Content**: If the table is successfully dropped.
-   **404 Not Found**: If there is an error deleting the table.

### 7. Developer Upsert Document

**Endpoint**: `/dev/upsert`

**Method**: `POST`

**Description**: Developer version of the upsert endpoint with additional logging.

**Request Body**:

-   JSON document containing the data to be upserted.

**Response**:

-   **200 OK**: If the document is successfully upserted.
-   **500 Internal Server Error**: If there is an error during upsertion.

### 8. Developer Query Documents

**Endpoint**: `/dev/query`

**Method**: `POST`

**Description**: Developer version of the query endpoint with a fixed sample response.

**Request Body**:

-   JSON document containing the embeddings to query.

**Response**:

-   **200 OK**: Returns a fixed sample JSON array of documents.

### 9. Developer Print Titles

**Endpoint**: `/dev/print`

**Method**: `GET`

**Description**: Developer version of the print endpoint with additional logging.

**Response**:

-   **204 No Content**: If titles are printed successfully.

## Response Status Codes

-   **200 OK**: The request was successful.
-   **204 No Content**: The request was successful but there is no content to return.
-   **404 Not Found**: The requested resource was not found.
-   **500 Internal Server Error**: An error occurred on the server.

## Example Usage

### Upsert Document

```sh
curl -X POST http://localhost:8080/upsert/:table -d '{
    "title": "Sample Title",
    "content": "Sample content",
    "embeddings": [0.1, 0.2, 0.3, ...]
}' -H "Content-Type: application/json"
```

### Query Documents

```sh
curl -X POST http://localhost:8080/query/:table -d '{
    "embeddings": [0.1, 0.2, 0.3, ...]
}' -H "Content-Type: application/json"
```

### Health Check

```sh
curl http://localhost:8080/health
```

### Echo

```sh
curl -X POST http://localhost:8080/echo -d 'Hello, World!' -H "Content-Type: text/plain"
```

### Drop Table

```sh
curl -X DELETE http://localhost:8080/drop/:table
```
