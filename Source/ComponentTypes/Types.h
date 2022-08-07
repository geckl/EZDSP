//
//  Types.h
//  EZDSP
//
//  Created by Garrett Eckl on 8/6/22.
//  Copyright © 2022 Squatch Sounds. All rights reserved.
//

#pragma once

enum numberType { floatingVal, intVal };

inline const std::string TypeToString(numberType t)
{
    switch (t)
    {
        case floatingVal:   return "float";
        case intVal:   return "int";
        default:      return "unknown type";
    }
}

class EZDSPComponent {
public:
    std::string name;
    virtual std::string getCode();
};

class Number: public EZDSPComponent {
public:
    Number();
    
    float initValue;
    std::string type;
    
    std::string getCode()
    {
        return type + " " + name + " = " + std::to_string(initValue);
    }
};

class Slider: public EZDSPComponent {
public:
    Slider();
    
    int minValue;
    int maxValue;
    int initValue;
    int intervalValue;
    
    std::string getCode()
    {
        return "input stream float " + name + " [[ name: \"" + name + "\", min: " + std::to_string(minValue) + ", max: " + std::to_string(maxValue) + ", init:  " + std::to_string(initValue) + ", step: " + std::to_string(intervalValue) + " ]];\n";
    }
};

class Button: public EZDSPComponent {
public:
    Button();
    
    bool state;
    
    std::string getCode()
    {
        return "input stream float " + name + " [[ name: \"" + name + "\", boolean ]];\n";
    }
    
};

class Buffer: public EZDSPComponent {
public:
    Buffer();
    
    std::string type;
    int size;
    
    std::string getCode()
    {
        return type + "[" + std::to_string(size) + "] " + name + ";\n" + "wrap<" + std::to_string(size) + "> " + name + "Index;\n";
    }
};

