#include <stdio.h>
#include <json-c/json.h>

int main() {
    const char *json_string = "{\"name\": \"John\", \"age\": 30, \"city\": \"New York\"}";

    // Parse the JSON string
    struct json_object *parsed_json = json_tokener_parse(json_string);

    if (parsed_json == NULL) {
        fprintf(stderr, "Failed to parse JSON string\n");
        return 1;
    }

    // Extract values from the parsed JSON object
    struct json_object *name_obj;
    struct json_object *age_obj;
    struct json_object *city_obj;

    // Get the name
    if (json_object_object_get_ex(parsed_json, "name", &name_obj)) {
        const char *name = json_object_get_string(name_obj);
        printf("Name: %s\n", name);
    } else {
        fprintf(stderr, "Failed to get 'name' from JSON object\n");
    }

    // Get the age
    if (json_object_object_get_ex(parsed_json, "age", &age_obj)) {
        int age = json_object_get_int(age_obj);
        printf("Age: %d\n", age);
    } else {
        fprintf(stderr, "Failed to get 'age' from JSON object\n");
    }

    // Get the city
    if (json_object_object_get_ex(parsed_json, "city", &city_obj)) {
        const char *city = json_object_get_string(city_obj);
        printf("City: %s\n", city);
    } else {
        fprintf(stderr, "Failed to get 'city' from JSON object\n");
    }

    // Clean up
    json_object_put(parsed_json);

    return 0;
}
