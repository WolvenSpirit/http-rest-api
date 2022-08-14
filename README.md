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

### Tests

Make sure the test target is built.
Run the available tests using `./build/test -v high -s`. Make sure the server is running since it includes e2e.

<!---

### Profile the app with perf tool

Check if perf is allowed to run:

```sh
# Perf won't be allowed to run if the value is 3
# Check the value here
cat /proc/sys/kernel/perf_event_paranoid
# Modify temporarily the value by editing at this location
````

Run with profiling:

```sh
perf stat -d -d -d -r 1 ./build/main
```

While pinging repeatedly the GET items endpoint in every run:

```sh

Performance counter stats for './build/main' (5 runs):

         36,009934      task-clock (msec)         #    0,002 CPUs utilized            ( +-  9,47% )
               316      context-switches          #    0,009 M/sec                    ( +-  5,37% )
                38      cpu-migrations            #    0,001 M/sec                    ( +-  6,05% )
               587      page-faults               #    0,016 M/sec                    ( +-  0,19% )
        71.302.981      cycles                    #    1,980 GHz                      ( +- 10,86% )  (8,97%)
        55.551.681      instructions              #    0,78  insn per cycle           ( +- 13,29% )  (27,62%)
        10.433.708      branches                  #  289,745 M/sec                    ( +- 20,04% )  (43,41%)
           272.264      branch-misses             #    2,61% of all branches          ( +-  4,34% )  (67,01%)
         8.595.796      L1-dcache-loads           #  238,706 M/sec                    ( +-  7,53% )  (80,78%)
           502.667      L1-dcache-load-misses     #    5,85% of all L1-dcache hits    ( +-  8,83% )  (76,35%)
           152.060      LLC-loads                 #    4,223 M/sec                    ( +-  9,39% )  (42,59%)
            74.277      LLC-load-misses           #   48,85% of all LL-cache hits     ( +-  5,51% )  (25,91%)
   <not supported>      L1-icache-loads                                             
           802.663      L1-icache-load-misses                                         ( +- 44,66% )  (17,95%)
     <not counted>      dTLB-loads                                                    ( +-100,00% )  (14,68%)
     <not counted>      dTLB-load-misses                                              ( +-100,00% )  (14,00%)
     <not counted>      iTLB-loads                                                    ( +-100,00% )  (7,07%)
     <not counted>      iTLB-load-misses                                              ( +-100,00% )  (1,27%)
   <not supported>      L1-dcache-prefetches                                        
   <not supported>      L1-dcache-prefetch-misses                                   

      21,634251388 seconds time elapsed                                          ( +- 25,41% )

Some events weren't counted. Try disabling the NMI watchdog:
        echo 0 > /proc/sys/kernel/nmi_watchdog
        perf stat ...
        echo 1 > /proc/sys/kernel/nmi_watchdog

```
--->