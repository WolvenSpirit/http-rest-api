# HTTP REST API in C++

### Dependencies:
- g++ 7.5
- cmake 3.15+
- vcpkg 
- POCO libraries
- prometheus-cpp

### How to use

Easiest way is to build it using `build_dockerfile.sh` and then running `docker-compose up`.
If you do initiate the build without the script then do reference the proper image name in the docker-compose file.

---

Services:

- Postgres
- Adminer
- Prometheus
- Traefik
- Grafana
- This api via the image you built
    - API exposes stats on base path `/`
    - Test endpoints 

### test endpoints 

This API is an example (or template), thus it only has one insert item and one get all items endpoints as a showcase or "how to".

- [GET] `/` will expose server stats in json format
- [GET] `get/items`
- [POST] `/item`  
    - content type `application/json`
    with payload, for simplicity and consistency the price is submitted as a string (example)
```json
                    {
                        "name":"foo",
                        "price": "3.20",
                        "data":{
                            "something":{
                                "else":"here"
                            }
                        }
                    }
```

The service can be easily extended by defining handler functions in router file and then registering them with the mux map in Router class. 

### Prometheus

This setup already scrapes API and Traefik, check it here `http://localhost:9007/graph`.
![Traefik heap graph](readme_assets/traefik_heap_graph.png?raw=true)

Endpoint for checking discovered targets (Traefik and API should be there) `http://localhost:9007/api/v1/targets`.

### Grafana
Grafana default credentials are admin/admin.
This dashboard is added as a convenience for visualizations of data from PostgreSQL and Prometheus.
The above databases can be added easily as sources, reference the host as the service name:

- Postgres is `db:5432`
- Prometheus is `prometheus:9090`

### Traefik dashboard

Can be accessed locally here `http://localhost:9009/dashboard/#/`.

### Alternative

Make sure you have the g++ compiler and CMake available through PATH.
Run `./vcpkg_install.sh` to pull in vcpkg and Poco (this builds the libraries).

Run `build.sh` and you should have a main binary produced.

Start the server `./main`.
