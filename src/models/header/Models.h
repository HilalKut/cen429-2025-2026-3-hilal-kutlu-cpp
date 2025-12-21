/* src/models/header/Models.h */
#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <ctime>

struct Entry {
    std::string title;
    std::string content; 
    std::string mood;
    time_t timestamp;
};

struct User {
    std::string username;
    std::string passwordHash;
    std::string salt;
};

#endif