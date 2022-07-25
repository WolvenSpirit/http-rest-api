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

### Alternative

Make sure you have the g++ compiler and CMake available through PATH.
Run `./vcpkg_install.sh` to pull in vcpkg and Poco (this builds the libraries).

Run `build.sh` and you should have a main binary produced.

Start the server `./main`.
